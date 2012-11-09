#!/usr/bin/env ylua

-- sg01-fields_demo.lua

-- A libwhy simple_graphics module demonstration program.

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
      print(string.format("%s: could not require('%s'): %s", arg[0], module, message))
      os.exit(1)
    end
end
try_require('ygtk', "perhaps you need to run this with libwhy's ylua?")
try_require('simple_graphics', 'perhaps you need to start this in the same directory as simple_graphics.lua?')
simple_graphics = require("simple_graphics")

gtk.init()
local window = gtk.window.new()
local canvas = gtk.drawing_area.new()

canvas:set("width-request", 400, "height-request", 300)
window:set("title", "simple drawingarea demo")
window:add(canvas)
window:show_all()

window:signal_connect('destroy', gtk.main_quit)
window:signal_connect("delete-event", function() gtk.main_quit() return true end)

local sg = simple_graphics.new(canvas, 400, 300)

for y0 = 0, 289 do
  for x = 5, 394 do
    local y = ((y0 * 7) % 290) + 5
    local d1 = math.sqrt(((x - 200) * (x - 200) + (y - 100) * (y - 100)) / 5)
    local d2 = math.sqrt(((x - 100) * (x - 100) + (y - 200) * (y - 200)) / 5)
    local d3 = math.sqrt(((x - 300) * (x - 300) + (y - 150) * (y - 150)) / 5)
    local d = math.sqrt(d1) + math.sqrt(d2) * -0.75 + math.sqrt(d3) * -0.8
    sg:set_color_hsv(d - math.floor(d), 1.0, 1.0)
    sg:point(x, y)
  end
  sg:update()
end

gtk.main()

simple_graphics.destroy(sg)

