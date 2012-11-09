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

-- local width  = 280
-- local height = 192

-- local width  = 800
local width  = 128
-- local width  = 300
local height = math.floor(width * 192 / 280)
local window = gtk.window.new()
local canvas = gtk.drawing_area.new()

canvas:set("width-request", width, "height-request", height)
window:set("title", "fields.bas")
window:add(canvas)
window:show_all()

window:signal_connect('destroy', gtk.main_quit)
window:signal_connect("delete-event", function() gtk.main_quit() return true end)

local w = 0

local function xyzzy(sg)
    sg:set_color(0, 0, 0)
    sg:rectangle(true, 0, 0, width, height)

    w = w + math.pi / 128

    local n = 3
    local px = { width * 0.5, width * 0.25, width * 0.75 }
    local py = { height / 3.0, height * 2.0 / 3.0, height / 2.0 }
    local pc = { math.sin(w), math.sin(w * 1.2), math.sin(w * 0.7) }
    local s = width / 12.0
    local m = 0.33
    local mc = m + (1.0 - m) * 0.5 * (math.sin(w * 0.4) + 1.0)

    local yc = 0

    for y0 = 0, height - 1 do
      local y = (y0 * 7) % height
      for x = 0, width - 1 do
        local t = 0
        for i = 1, n do
          local zx = x - px[i]
          local zy = y - py[i]
          local z = zx * zx + zy * zy
          t = t + pc[i] * s / math.sqrt(math.sqrt(z))
        end
        t = t - math.floor(t)
        if t < m then
          sg:set_color_hsv(mc - t, 1, 1)
          sg:point(x, y)
        end
      end
    end

    sg:update()
    return true -- keep calling me
end

local sg = simple_graphics.new(canvas, width, height)

g.idle_add(xyzzy, sg)

gtk.main()

sg:destroy()

