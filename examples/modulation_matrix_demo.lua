#!/usr/bin/env ylua

-- modulation_matrix_demo.lua -- a libwhy demonstration program.

-- This program, written by Sean Bolton, is in the public domain:
--
-- To the extent possible under law, the author has waived all copyright
-- and related or neighboring rights to the contents of this file, as
-- described in the CC0 legalcode.
--
-- You should have received a copy of the CC0 legalcode along with this
-- work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

function try_require(module, message)
    if not pcall(require, module) then
      print(string.format("%s: could not load module '%s': %s", arg[0], module, message))
      os.exit(1)
    end
end

try_require("ygtk", "perhaps you need to run this with libwhy's ylua?")

pcall(require, "stringent") -- like strict.lua, but more even so

-- declare globals
main_window = false

source = { { name = "Source1", type = "slider" },
           { name = "Source2", type = "slider" },
           { name = "Source3", type = "slider" },
           { name = "LFO1",    type = "LFO", freq = 1 },
           { name = "LFO2",    type = "LFO", freq = 0.2 },
           { name = "VSource", type = "virtual" } }

mod = {}
modcount = 4

dest = { { name = "Dest1" },
         { name = "Dest2" },
         { name = "VDest", type = "virtual" } }

virtual = {}

if stringent then
  stringent(_ENV, true) -- no more new or undefined global accesses after now, except function definitions
end

function set_up_objects()
  for i = 1, #source do
    source[i].value = 0
    if source[i].type == "LFO" then
      source[i].w = 0
    end
  end
  
  mod = {}
  for i = 1, modcount do
    mod[i] = {}
    mod[i].amount = 0
    mod[i].bias = 0
  end
  
  for i = 1, #dest do
    dest[i].init = 0
  end
  
  -- match virtual sources with virtual dests
  local vs, vd = 0, 0
  for i = 1, #source do
    if source[i].type == "virtual" then
      vs = vs + 1
      virtual[vs] = {}
      virtual[vs].source = i
    end
  end
  for i = 1, #dest do
    if dest[i].type == "virtual" then
      vd = vd + 1
      if not virtual[vd] then
        virtual[vd] = {}
      end
      virtual[vd].dest = i
    end
  end
  if vs ~= vd then
    print("error: number of virtual sources and destinations does not match!")
    os.exit(1)
  end
end

function update_dests()
  for i = 1, #dest do
    dest[i].value = dest[i].init
  end
  for i = 1, modcount do
    local d = mod[i].dest
    if d then
      -- print("active mod:", i)
      d.value = d.value + mod[i].source.value * mod[i].amount + mod[i].bias
    end
  end
  for i = 1, #dest do
    local f = dest[i].value / 2 + 0.5
    if f < 0 then f = 0 elseif f > 1 then f = 1 end
    dest[i].progress:set_fraction(f)
  end
  for i = 1, #virtual do
    local vs = virtual[i].source
    local vd = virtual[i].dest
    --source[vs].value = dest[vd].value * dest[vd].value -- square
    --source[vs].value = source[vd].value * 0.95 + dest[vd].value * 0.05 -- simple LPF
    source[vs].value = dest[vd].value
    local f = source[vs].value / 2 + 0.5
    if f < 0 then f = 0 elseif f > 1 then f = 1 end
    source[vs].progress:set_fraction(f)
  end
end

function on_source_change(self, index)
  source[index].value = self:get_value()
  -- update_dests()
end

function set_mod(index, sourceindex, destindex)
  if sourceindex > 0 and destindex > 0 then
    mod[index].source = source[sourceindex]
    mod[index].dest = dest[destindex]
  else
    mod[index].dest = nil
  end
  -- update_dests()
end

function on_mod_source_combo_changed(self, index)
  local sourceindex = self:get_active()
  local destindex = mod[index].dest_combo:get_active()
  set_mod(index, sourceindex, destindex)
end

function on_mod_dest_combo_changed(self, index)
  local sourceindex = mod[index].source_combo:get_active()
  local destindex = self:get_active()
  set_mod(index, sourceindex, destindex)
end

function on_mod_amount_adj_change(self, index)
  mod[index].amount = self:get_value()
  -- update_dests()
end

function on_mod_bias_adj_change(self, index)
  mod[index].bias = self:get_value()
  -- update_dests()
end

function update_LFOs()
  for i = 1, #source do
    if source[i].type == "LFO" then
      local w = source[i].w
      w = w + math.pi * source[i].freq / 10
      source[i].w = w
      source[i].value = math.sin(w)
      source[i].progress:set_fraction(source[i].value / 2 + 0.5)
    end
  end
  update_dests()
  return true -- reschedule timeout
end

function create_window(title)
  main_window = gtk.window.new (gtk.WINDOW_TOPLEVEL)
  main_window:set_title(title)
  main_window:signal_connect("destroy", gtk.main_quit)
  main_window:signal_connect("delete-event", function() gtk.main_quit() return true end)

  local vbox = gtk.vbox.new (false, 0)
  main_window:add(vbox)

  -- source widgets
  local frame = gtk.frame.new("sources")
  vbox:pack_start(frame, false, false, 0)
  frame:set("border-width", 6)
  local table = gtk.table.new(2, #source, false)
  frame:add(table)
  table:set("border-width", 6,
            "row-spacing", 5,
            "column-spacing", 5)

  for i = 1, #source do
    local label = gtk.label.new (source[i].name)
    table:attach(label, 0, 1, i - 1, i,
                 gtk.FILL, 0, 0, 0)

    if source[i].type == "slider" then
      source[i].adj = gtk.adjustment.new (0.0, -1.0, 1.0, 0.02, 0.1, 0)
      local hscale = gtk.hscale.new (source[i].adj)
      table:attach(hscale, 1, 2, i - 1, i,
                   gtk.EXPAND + gtk.FILL, 0, 0, 0)
      hscale:set("value-pos", gtk.POS_RIGHT,
                 "digits", 2,
                 "update-policy", gtk.UPDATE_CONTINUOUS)
      source[i].adj:signal_connect("value_changed", on_source_change, i)
    else -- LFO or virtual
      source[i].progress = gtk.progress_bar.new()
      table:attach(source[i].progress, 1, 2, i - 1, i,
                   0, 0, 0, 0)
    end
  end

  -- modulation matrix
  frame = gtk.frame.new("modulation matrix")
  vbox:pack_start(frame, false, false, 0)
  frame:set("border-width", 6)
  table = gtk.table.new(4, modcount + 1, false)
  frame:add(table)
  table:set("border-width", 6,
            "row-spacing", 5,
            "column-spacing", 5)

  for i, l in ipairs({ "Source", "Destination", "Amount", "Bias" }) do
    local label = gtk.label.new (l)
    table:attach(label, i, i + 1, 0, 1,
                 gtk.FILL, 0, 0, 0)
  end
  for i = 1, modcount do
    local label = gtk.label.new (string.format("Mod%d", i))
    table:attach(label, 0, 1, i, i + 1,
                 gtk.FILL, 0, 0, 0)

    mod[i].source_combo = gtk.combo_box.new_text()
    mod[i].source_combo:append_text("-none-")
    for j = 1, #source do
      mod[i].source_combo:append_text(source[j].name)
    end
    mod[i].source_combo:set_active(0)
    table:attach(mod[i].source_combo, 1, 2, i, i + 1,
                 gtk.FILL, 0, 0, 0)
    mod[i].source_combo:signal_connect("changed", on_mod_source_combo_changed, i)

    mod[i].dest_combo = gtk.combo_box.new_text()
    mod[i].dest_combo:append_text("-none-")
    for j = 1, #dest do
      mod[i].dest_combo:append_text(dest[j].name)
    end
    mod[i].dest_combo:set_active(0)
    table:attach(mod[i].dest_combo, 2, 3, i, i + 1,
                 gtk.FILL, 0, 0, 0)
    mod[i].dest_combo:signal_connect("changed", on_mod_dest_combo_changed, i)

    mod[i].amount_adj = gtk.adjustment.new (0.0, -1.0, 1.0, 0.02, 0.1, 0)
    local hscale = gtk.knob.new (mod[i].amount_adj)
    table:attach(hscale, 3, 4, i, i + 1,
                 0, 0, 0, 0)
    mod[i].amount_adj:signal_connect("value_changed", on_mod_amount_adj_change, i)

    mod[i].bias_adj = gtk.adjustment.new (0.0, -1.0, 1.0, 0.02, 0.1, 0)
    local hscale = gtk.knob.new (mod[i].bias_adj)
    table:attach(hscale, 4, 5, i, i + 1,
                 0, 0, 0, 0)
    mod[i].bias_adj:signal_connect("value_changed", on_mod_bias_adj_change, i)
  end

  -- destination widgets
  frame = gtk.frame.new("destinations")
  vbox:pack_start(frame, false, false, 0)
  frame:set("border-width", 6)
  table = gtk.table.new(2, #dest, false)
  frame:add(table)
  table:set("border-width", 6,
            "row-spacing", 5,
            "column-spacing", 5)

  for i = 1, #dest do
    local label = gtk.label.new(dest[i].name)
    table:attach(label, 0, 1, i - 1, i,
                 gtk.FILL, 0, 0, 0)

    dest[i].progress = gtk.progress_bar.new()
    table:attach(dest[i].progress, 1, 2, i - 1, i,
                 0, 0, 0, 0)
  end

  vbox:show_all()
end

set_up_objects()

gtk.init()
create_window("modulation matrix demo")
main_window:show()

g.timeout_add(100, update_LFOs, nil)

gtk.main()

