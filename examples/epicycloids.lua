#!/usr/bin/env ylua

-- Epicycloid (hypotrochoid, epitrochoid) demo, by Sean Bolton

-- Like the Spirograph(TM) toy.

-- This libwhy example program is in the public domain:
--
-- To the extent possible under law, the author has waived all copyright
-- and related or neighboring rights to the contents of this file, as
-- described in the CC0 legalcode.
--
-- You should have received a copy of the CC0 legalcode along with this
-- work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

-- This is complete for the current feature set. I would like to add a
-- PDF output option, and it would be cool to be able to change colors
-- and place multiple epicycloids on the page.

-- See http://www.math.psu.edu/dlittle/java/parametricequations/spirograph/index.html for
-- the Java version that inspired some of the features of this version.

-- smbolton 20091130: initial version

function try_require(module, message)
  if not pcall(require, module) then
    print(string.format("%s: could not load module '%s': %s", arg[0], module, message))
    os.exit(1)
  end
end

try_require("ygtk", "perhaps you need to run this with libwhy's ylua?")

pcall(require, "strict")

-- declare globals
size = 600

wheel1 = 96
wheel2 = -30
offset = 27
resolution = 7
pen_width = 1
color = false
draw_wheels = true
revolutions = nil -- calculated
points = nil      -- calculated

main_window = nil
revs_label = nil
points_label = nil
da = nil

function GCD (a, b) -- returns greatest common divisor
  repeat
    a, b = b, math.fmod(a, b)
  until b == 0
  return a
end

-- to find number of points, iteratively
--    i = 1 while math.fmod(72*i,105) ~= 0 do i = i + 1 end print(i)
-- to find number of points, directly
--    points = w1 * revolutions / w2

function update_labels()
  revolutions = math.abs(wheel2) / GCD(wheel1, math.abs(wheel2))
  points = wheel1 * revolutions / math.abs(wheel2)
  revs_label:set("label", string.format("Revolutions: %d", revolutions))
  points_label:set("label", string.format("Points: %d", points))
end

function on_spin_change(self, index)
-- print(index, self:get_value())
  if index == 1 then
    wheel1 = self:get_value()
  elseif index == 2 then
    wheel2 = self:get_value()
    if wheel2 == 0 then wheel2 = 1 end
  elseif index == 3 then
    offset = self:get_value()
  elseif index == 4 then
    resolution = self:get_value()
  elseif index == 5 then
    pen_width = self:get_value()
  end
  update_labels()
  da:queue_draw()
end

function on_button_toggle(self, index)
  if index == 1 then
    color = self:get("active")
  elseif index == 2 then
    draw_wheels = self:get("active")
  end
  da:queue_draw()
end

function expose(widget, event, data)
    local cr = gdk.cairo_create(widget:get_window())
    local t

    cr:translate(size / 2, size / 2)
    if wheel1 < -wheel2 then
      t = (-wheel1 - wheel2 + offset) * 2.2
    else
      t = (wheel1 + wheel2 + offset) * 2.2
    end
    cr:scale(size / t, size / t)
    cr:set_line_width(t / size * pen_width)

    -- white background
    cr:set_source_rgb(1, 1, 1)
    cr:paint()

    if draw_wheels then
      -- wheel 1
      cr:set_source_rgb(1, 0, 0)
      cr:arc(0, 0, wheel1, 0, math.pi * 2)
      cr:close_path()
      cr:stroke()

      -- wheel 2
      cr:set_source_rgb(0, 0, 1)
      cr:arc(wheel1 + wheel2, 0, math.abs(wheel2), 0, math.pi * 2)
      cr:close_path()
      cr:stroke()

      -- offset
      cr:set_source_rgb(0, 1, 0)
      t = wheel1 + wheel2
      cr:move_to(t, 0)
      cr:line_to(t + offset, 0)
      cr:stroke()
    end

    -- Spiro that graph!
    local x2o = wheel1 + wheel2 + offset
    local y2o = 0
    local d = 1
    for _ = 1, resolution do d = d * 2 end
    if color then

      local ni = d * revolutions
      for i = 1, ni do
        local w = i / d * math.pi * 2
        local r = wheel1 + wheel2
        local x = math.cos(w) * r
        local y = math.sin(w) * r
        local w2 = w * (1 + wheel1 / wheel2)
        local x2 = x + math.cos(w2) * offset
        local y2 = y + math.sin(w2) * offset
        cr:set_source_hsv(i / ni, 1, 1)
        cr:move_to(x2o, y2o)
        cr:line_to(x2, y2)
        cr:stroke()
        x2o = x2
        y2o = y2
      end

    else -- not color

      cr:set_source_rgb(0, 0, 0)
      for i = 1, d * revolutions do
        local w = i / d * math.pi * 2
        local r = wheel1 + wheel2
        local x = math.cos(w) * r
        local y = math.sin(w) * r
        local w2 = w * (1 + wheel1 / wheel2)
        local x2 = x + math.cos(w2) * offset
        local y2 = y + math.sin(w2) * offset
        cr:move_to(x2o, y2o)
        cr:line_to(x2, y2)
        cr:stroke()
        x2o = x2
        y2o = y2
      end

    end

    cr:destroy()
    return true
end

gtk.init()

main_window = gtk.window.new(gtk.WINDOW_TOPLEVEL)
main_window:set("title", "epicycloids")
main_window:signal_connect("destroy", gtk.main_quit)
main_window:signal_connect("delete-event", function() gtk.main_quit() return true end)

local hbox = gtk.hbox.new(false, 5)
main_window:add(hbox)

-- controls
local vbox = gtk.vbox.new(false, 5)
hbox:pack_start(vbox, false, false, 5)

local label = gtk.label.new ("Wheel 1 Radius")
vbox:pack_start(label, false, false, 5)

local adj = gtk.adjustment.new (wheel1, 8, 120, 1, 1, 0)
local spin = gtk.spin_button.new(adj, 0, 0)
vbox:pack_start(spin, false, false, 5)
adj:signal_connect('value_changed', on_spin_change, 1)

label = gtk.label.new ("Wheel 2 Radius")
vbox:pack_start(label, false, false, 5)

adj = gtk.adjustment.new(wheel2, -100, 100, 1, 1, 0)
spin = gtk.spin_button.new(adj, 1, 0)
vbox:pack_start(spin, false, false, 5)
adj:signal_connect('value-changed', on_spin_change, 2)

label = gtk.label.new ("Pen Radius")
vbox:pack_start(label, false, false, 5)

adj = gtk.adjustment.new(offset, 0, 200, 1, 1, 0)
spin = gtk.spin_button.new(adj, 1, 0)
vbox:pack_start(spin, false, false, 5)
adj:signal_connect('value-changed', on_spin_change, 3)

revs_label = gtk.label.new ("Revolutions:")
vbox:pack_start(revs_label, false, false, 5)

points_label = gtk.label.new ("Points:")
vbox:pack_start(points_label, false, false, 5)

local hrule = gtk.hseparator.new()
vbox:pack_start(hrule, false, false, 5)

label = gtk.label.new ("Resolution")
vbox:pack_start(label, false, false, 5)

adj = gtk.adjustment.new(resolution, 1, 15, 1, 1, 0)
spin = gtk.spin_button.new(adj, 1, 0)
vbox:pack_start(spin, false, false, 5)
adj:signal_connect('value-changed', on_spin_change, 4)

label = gtk.label.new ("Pen Width")
vbox:pack_start(label, false, false, 5)

adj = gtk.adjustment.new(pen_width, 0.1, 10, 0.1, 1, 0)
spin = gtk.spin_button.new(adj, 1, 1)
vbox:pack_start(spin, false, false, 5)
adj:signal_connect('value-changed', on_spin_change, 5)

local cb = gtk.check_button.new_with_label("Color")
cb:set("active", color)
vbox:pack_start(cb, false, false, 5)
cb:signal_connect('toggled', on_button_toggle, 1)

local cb = gtk.check_button.new_with_label("Show Wheels")
cb:set("active", draw_wheels)
vbox:pack_start(cb, false, false, 5)
cb:signal_connect('toggled', on_button_toggle, 2)

-- drawing area
da = gtk.drawing_area.new()
da:set("width-request", size, "height-request", size)
da:signal_connect("expose-event", expose)
hbox:pack_start(da, false, false, 0)

update_labels()
main_window:show_all()

gtk.main()

