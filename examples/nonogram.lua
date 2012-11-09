#!/usr/bin/env ylua

-- Nonogram, by Sean Bolton

-- A puzzle game in the vein of Griddlers, Picross, Picma, or World Mosaics.

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

try_require('ygtk', "perhaps you need to run this with libwhy's ylua?")

gtk.init()
local window     = gtk.window.new()
local wtable     = gtk.table.new(3, 3, false)
local directions = gtk.label.new('Digits show the number of consecutive coins in the ' ..
                                 'respective row or column. Left-click to mark a coin, ' ..
                                 'middle-click to mark a space.')
local fillbutton = gtk.check_button.new_with_label('Auto-fill spaces')
local newbutton  = gtk.button.new_with_label('New game')
local mistake_label = gtk.label.new('(Starting....)')
local field      = gtk.drawing_area.new()

function new_puzzle()
  puzzle = {}
  guess = {}
  mistakes = 0
  for i = 1, 10 do
    puzzle[i] = {}
    guess[i] = {}
    for j = 1, 10 do
      puzzle[i][j] = math.random() < 0.5
      guess[i][j] = false
    end
  end
end

function expose(widget, event, data)
    local cr = gdk.cairo_create(widget:get_window())

    cr:set_line_width(1)
    cr:set_source_rgb(0.7, 0.7, 0.7)
    cr:move_to(500.5, 100.5)
    cr:line_to(500.5, 500.5)
    cr:line_to(100.5, 500.5)
    cr:stroke()
    for x = 1, 10 do
      cr:move_to(100.5, 60.5 + 40 * x)
      cr:line_to(500.5, 60.5 + 40 * x)
      cr:stroke()
      cr:move_to(60.5 + 40 * x, 100.5)
      cr:line_to(60.5 + 40 * x, 500.5)
      cr:stroke()
      cr:save()
      for y = 1, 10 do
        if guess[x][y] then
          if puzzle[x][y] then
            cr:arc(80 + x * 40, 80 + y * 40, 16, 0, 2 * math.pi)
            cr:set_source_rgb(0, 0, 1)
            cr:fill_preserve()
            cr:set_source_rgb(0, 0, 0)
            cr:stroke()
          else
            cr:arc(80 + x * 40, 80 + y * 40, 8, 0, 2 * math.pi)
            cr:set_source_rgb(0.3, 0.3, 0.3)
            cr:fill()
          end
          if guess[x][y] == 'x' then
            cr:set_line_width(2)
            cr:set_source_rgb(1, 0, 0)
            cr:move_to(85 + x * 40, 75 + y * 40)
            cr:line_to(75 + x * 40, 85 + y * 40)
            cr:stroke()
            cr:move_to(75 + x * 40, 75 + y * 40)
            cr:line_to(85 + x * 40, 85 + y * 40)
            cr:stroke()
            cr:set_line_width(1)
          end
        end
      end
      cr:restore()
    end
    cr:select_font_face('Sans', cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
    cr:set_font_size(12)
    for y = 1, 10 do
    -- -FIX- this works, but it gives away information!
      local count, known = {}, {}
      local x0 = 1
      local t = puzzle[1][y]
      local k = guess[1][y]
      for x = 2, 11 do
        if x == 11 or puzzle[x][y] ~= t then
          if t then
            table.insert(count, x - x0)
            if (x == 11 or guess[x][y]) and k then
              table.insert(known, true)
            else
              table.insert(known, false)
            end
          end
          t = not t
          x0 = x
        end
        if x < 11 then
          if puzzle[x][y] then
            if not guess[x][y] then
              k = false
            end
          else
            k = guess[x][y]
          end
        end
      end
      local all = true
      for x = 1, 10 do
        if puzzle[x][y] and not guess[x][y] then
          all = false
        end
      end
      for n = 1, #count do
        cr:move_to(100 - #count * 20 + (n - 1) * 20, 100 + (y - 1) * 40 + 27)
        if all then
          cr:set_source_rgb(0, 0, 0)
        elseif known[n] then
          cr:set_source_rgb(0, 0, 1)
        else
          cr:set_source_rgb(1, 0, 0)
        end
        cr:show_text(tostring(count[n]))
      end
    end
    for x = 1, 10 do
      local count, known = {}, {}
      local y0 = 1
      local t = puzzle[x][1]
      local k = guess[x][1]
      for y = 2, 11 do
        if y == 11 or puzzle[x][y] ~= t then
          if t then
            table.insert(count, y - y0)
            if (y == 11 or guess[x][y]) and k then
              table.insert(known, true)
            else
              table.insert(known, false)
            end
          end
          t = not t
          y0 = y
        end
        if y < 11 then
          if puzzle[x][y] then
            if not guess[x][y] then
              k = false
            end
          else
            k = guess[x][y]
          end
        end
      end
      local all = true
      for y = 1, 10 do
        if puzzle[x][y] and not guess[x][y] then
          all = false
        end
      end
      for n = 1, #count do
        cr:move_to(100 + (x - 1) * 40 + 14, 100 - #count * 20 + (n - 1) * 20 + 12)
        if all then
          cr:set_source_rgb(0, 0, 0)
        elseif known[n] then
          cr:set_source_rgb(0, 0, 1)
        else
          cr:set_source_rgb(1, 0, 0)
        end
        cr:show_text(tostring(count[n]))
      end
    end

    cr:destroy()

    return true
end

function check_for_fill(x, y)
  local all = true
  for xi = 1, 10 do
    if puzzle[xi][y] and not guess[xi][y] then
      all = false
      break
    end
  end
  if all then
    for xi = 1, 10 do
      if not puzzle[xi][y] then
        guess[xi][y] = guess[xi][y] or true
      end
    end
  end
  all = true
  for yi = 1, 10 do
    if puzzle[x][yi] and not guess[x][yi] then
      all = false
      break
    end
  end
  if all then
    for yi = 1, 10 do
      if not puzzle[x][yi] then
        guess[x][yi] = guess[x][yi] or true
      end
    end
  end
end

function on_newbutton_clicked()
    -- print('New!')
    new_puzzle()
    mistake_label:set('label', 'Mistakes: 0')
    if fillbutton:get_active() then
      for i = 1, 10 do
        check_for_fill(i, i)
      end
    end
    field:queue_draw()
end

function on_field_clicked(widget, event, data)
  local button, _, _x, _y = event:get_button()
  local x = _x - 100
  local y = _y - 100
  if x > 0 and y > 0 and x % 40 ~= 0 and y % 40 ~= 0 then
    x = math.floor(x / 40) + 1
    y = math.floor(y / 40) + 1
    -- print('Click!', button, _x, _y, x, y)
    if not guess[x][y] then
      if button == 1 then
        if puzzle[x][y] then
          guess[x][y] = true
          if fillbutton:get_active() then
            check_for_fill(x, y)
          end
        else
          guess[x][y] = 'x'
          mistakes = mistakes + 1
        end
      elseif button == 2 then
        if not puzzle[x][y] then
          guess[x][y] = true
        else
          guess[x][y] = 'x'
          mistakes = mistakes + 1
          if fillbutton:get_active() then
            check_for_fill(x, y)
          end
        end
      end
      mistake_label:set('label', string.format('Mistakes: %d', mistakes))
      field:queue_draw()
    end
  else
    -- print('(Miss)', button, _x, _y)
  end
end

window:set('title', 'Nonogram')
window:signal_connect('destroy', gtk.main_quit)
window:signal_connect('delete-event', function() gtk.main_quit() return true end)
window:add(wtable)
wtable:set('border-width', 10)
wtable:attach(directions, 0, 3, 0, 1, 0, 0, 0, 0)
wtable:attach(field, 0, 3, 1, 2, 0, 0, 0, 10)
wtable:attach(mistake_label, 0, 1, 2, 3, gtk.EXPAND, 0, 0, 0)
wtable:attach(fillbutton, 1, 2, 2, 3, 0, 0, 0, 0)
wtable:attach(newbutton, 2, 3, 2, 3, gtk.EXPAND, 0, 0, 0)
directions:set('wrap', true)
field:set('width-request', 501, 'height-request', 501)
field:signal_connect('expose-event', expose)
field:add_events(gdk.BUTTON_PRESS_MASK)
field:signal_connect('button-press-event', on_field_clicked)
newbutton:signal_connect('clicked', on_newbutton_clicked)

math.randomseed(os.time())
on_newbutton_clicked() -- does the whole new puzzle thing

window:show_all()

gtk.main()

