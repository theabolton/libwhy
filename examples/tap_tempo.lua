#!/usr/bin/env ylua

-- Tap Tempo, by Sean Bolton

-- This libwhy example program is in the public domain:
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

gtk.init()

-- Create the widgets
local window  = gtk.window.new()
local vbox    = gtk.vbox.new(true, 4)
local button  = gtk.button.new_with_label("Tap")
local message = gtk.label.new([[
Tap the button or press a key repeatedly
to estimate a tempo in beats per minute.]])
local label   = gtk.label.new ("-")

local taps = {}

function tap()
  -- This code will average all valid taps, for as long as you want to tap.
  -- There is a window based on the last tap interval, and any older tap intervals which
  -- do not fall within that window will cause all older taps to be discarded
  table.insert(taps, 1, g.get_current_time())
  if #taps > 1 then
    local dt0 = taps[1] - taps[2]
    for i = 2, #taps - 1 do
      local dt = taps[i] - taps[i + 1]
      if dt > dt0 * 1.5 or dt < dt0 * 0.5 then -- the window
        for j = #taps, i + 1, -1 do
          table.remove(taps, j)
        end
        break
      end
    end
  end
  if #taps > 1 then
    local mean_dt = (taps[1] - taps[#taps]) / (#taps - 1)
    -- print("average tempo:", 60 / mean_dt)
    label:set("label", string.format('<span font_size="xx-large">%d</span> bpm', math.floor(60 / mean_dt + 0.5)))
  else
    label:set("label", "-")
  end
end

function on_button_pressed()
  -- print("button pressed")
  tap()
end

function on_key_press_event()
  -- print("key pressed")
  tap()
end

window:set("title", "Tap Tempo")
window:signal_connect("delete-event", function() gtk.main_quit() return true end)
window:add(vbox)
vbox:set("border-width", 10)
vbox:pack_start(message, false, false, 0)
vbox:pack_start(button, false, false, 0)
vbox:pack_start(label, false, false, 0)
button:signal_connect("pressed", on_button_pressed)
button:add_events(gdk.KEY_PRESS_MASK)
button:signal_connect("key-press-event", on_key_press_event)
label:set("use-markup", true)

window:show_all()

gtk.main()

