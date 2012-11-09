-- simple_graphics.lua

-- A module for libwhy which provides a cairo surface backed drawing area
-- and primitives for easy drawing.  Okay, it was really written just for
-- porting some Applesoft BASIC and Apple Pascal graphics code.

-- This program, written by Sean Bolton, is in the public domain:
--
-- To the extent possible under law, the author has waived all copyright
-- and related or neighboring rights to the contents of this file, as
-- described in the CC0 legalcode.
--
-- You should have received a copy of the CC0 legalcode along with this
-- work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

require('ygtk')

local M = {}

local function expose(widget, event, sg)
    local cr = gdk.cairo_create(widget:get_window())
    cr:set_source_surface(sg.surface, 0, 0)
    cr:paint()
    cr:destroy()
end

function M.destroy(sg)
    sg.drawing_area:signal_handler_disconnect(sg.expose_handler_id)
    sg.cr:destroy()
    sg.cr = nil
    sg.surface:destroy()
    sg.surface = nil
    sg.drawing_area = nil
    setmetatable(sg, nil)
end

function M.new(drawing_area, width, height)
    local sg = setmetatable({}, { __index = M, __gc = M.destroy })
    sg.drawing_area = drawing_area
    sg.surface = cairo.image_surface.create(cairo.FORMAT_RGB24, width, height)
    sg.cr = cairo.context.create(sg.surface)
    sg.cr:set_line_width(1)
    sg.expose_handler_id = drawing_area:signal_connect('expose-event', expose, sg)
    return sg
end

function M.update(sg)
    sg.drawing_area:queue_draw()
    while gtk.events_pending() do
      gtk.main_iteration_do (false)
    end
end

function M.clear(sg)
    sg.cr:set_source_rgb(0, 0, 0)
    sg.cr:paint()
end

function M.line(sg, x0, y0, x1, y1)
    sg.cr:move_to(x0 + 0.5, y0 + 0.5)
    sg.cr:line_to(x1 + 0.5, y1 + 0.5)
    sg.cr:stroke()
end

function M.point(sg, x, y)
    sg.cr:move_to(x, y + 0.5)
    sg.cr:line_to(x + 1, y + 0.5)
    sg.cr:stroke()
end

function M.rectangle(sg, fill, x, y, w, h)
    if fill then
      sg.cr:rectangle(x, y, w, h)
      sg.cr:fill()
    else
      sg.cr:rectangle(x + 0.5, y + 0.5, w - 1, h - 1)
      sg.cr:stroke()
    end
end

function M.set_color(sg, r, g, b)
    sg.cr:set_source_rgb(r, g, b)
end

function M.set_color_hsv(sg, h, s, v)
    sg.cr:set_source_hsv(h, s, v)
end

return M

