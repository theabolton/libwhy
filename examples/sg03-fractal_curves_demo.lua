#!/usr/bin/env ylua

-- sg03-fractal_curves_demo.lua

-- A libwhy demonstration program.  This is translation of some Apple Pascal
-- turtlegraphics programs.  It demonstrates the use of:
--    - the simple_graphics.lua module
--    - GtkComboBox and GtkHScale widgets and signal callbacks

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
try_require('ygtk', "perhaps you need to run this with libwhy's ylua?")
try_require('simple_graphics', 'perhaps you need to start this in the same directory as simple_graphics.lua?')
simple_graphics = require("simple_graphics")

-- local width  = 280
local width  = 800
local height = math.floor(width * 192 / 280)
local sg

-- dragoncurve.pas

-- program dragoncurve;
-- 
--   uses
--     turtlegraphics;

local pendown = true
local old_x = 0
local old_y = 0
local angle = math.pi / 2
function pencolor(color)
  if color == "none" then
    pendown = false
  elseif color == "white" then
    pendown = true
    sg:set_color(1, 1, 1)
  else
    error(string.format("unknown pen color '%s'", color))
  end
end
function moveto(x, y)
  if pendown then
    sg:line(old_x, old_y, x, y)
  end
  old_x = x
  old_y = y
end
function move(len)
  moveto(old_x + math.sin(angle) * len, old_y + math.cos(angle) * len)
end
function turn(w)
  angle = angle - w / 180 * math.pi
end
function turnto(w)
  -- "Headings (angles) are measured in degrees clockwise from the positive Y axis."
  angle = (90 - w) / 180 * math.pi
end

--   procedure dragon (size, level : integer);
function dragon (size, level)
    --     var
    --       newsize : integer;
    local newsize
    --     begin
    --       if level <= 0 then
    if level <= 0 then
      --       move (size)
      move (size)
      --     else
      --       begin
    else
      --         newsize := round (size * 0.707);
      --newsize = math.floor(size * 0.707)
      newsize = size * 2^-0.5
      --         turn (45);
      turn (45)
      --         dragon (newsize, level - 1);
      dragon (newsize, level - 1)
      --         turn (-90);
      turn (-90)
      --         dragon (newsize, level - 1);
      dragon (newsize, level - 1)
      --         turn (45);
      turn (45)
      --       end (* if *)
    end
    --     end; (* dragon *)
end

function draw_dragon(lev)
    -- begin
    --   initturtle;
    --print(string.format("level = %d", lev))
    --       fillscreen (black);
    sg:set_color(0, 0, 0)
    sg:rectangle(true, 0, 0, width, height)
    --       pencolor (none);
    pencolor ("none")
    --       moveto (70, 50);
    moveto (width / 4, height / 4)
    --       pencolor (white);
    pencolor ("white")
    --       dragon (140, lev);
    dragon (width / 2, lev)
    --       readln
    sg:update()
    -- end.
end

-- dragoncurve2.pas

--   procedure dragon (size, level, sign : integer);
function dragon2 (size, level, sign)
  --     var
  --       newsize : integer;
  local newsize
  --     begin
  --       if level <= 0 then
  if level <= 0 then
    --       move (size)
    move (size)
    --     else
    --       begin
  else
    --         newsize := round (size * 0.707);
    --newsize = math.floor(size * 0.707)
    newsize = size * 2^-0.5
    --         turn (45 * sign);
    turn (45 * sign)
    --         dragon (newsize, level - 1, 1);
    dragon2 (newsize, level - 1, 1)
    --         turn (-90 * sign);
    turn (-90 * sign)
    --         dragon (newsize, level - 1, -1);
    dragon2 (newsize, level - 1, -1)
    --         turn (45 * sign);
    turn (45 * sign)
    --       end (* if *)
  end
  --     end; (* dragon *)
end

function draw_dragon2(lev)
    -- begin
    --   initturtle;
    --print(string.format("level = %d", lev))
    --       fillscreen (black);
    sg:set_color(0, 0, 0)
    sg:rectangle(true, 0, 0, width, height)
    --       pencolor (none);
    pencolor ("none")
    --       moveto (70, 50);
    moveto (width * 0.3, height * 0.35)
    --       pencolor (white);
    pencolor ("white")
    --       dragon (140, lev, 1);
    dragon2 (width / 2, lev, 1)
    --       readln
    sg:update()
    -- end.
end

-- snowflake.pas

--   procedure fractal (size : real; level : integer);
function fractal (size, level)
    --     var
    --       newsize : real;
    local newsize
    --     begin
    --       if level <= 0 then
    if level <= 0 or size < 1 then
      --       move (trunc (size))
      move (size)
      --     else
      --       begin
    else
      --         newsize := size / 3;
      newsize = size / 3
      --         fractal (newsize, level - 1);
      fractal (newsize, level - 1)
      --         turn (60);
      turn (60)
      --         fractal (newsize, level - 1);
      fractal (newsize, level - 1)
      --         turn (-120);
      turn (-120)
      --         fractal (newsize, level - 1);
      fractal (newsize, level - 1)
      --         turn (60);
      turn (60)
      --         fractal (newsize, level - 1);
      fractal (newsize, level - 1)
      --       end (* if *)
    end
    --     end; (* fractal *)
end

function snowflake(fraclevel)
    --   const
    --     fracsize = 1.2124e2;
    --     fraclevel = 4;
    --     fracxstart = 80;
    --     fracystart = 121;
    --local fracsize = width / 280 * 1.2124e2
    local fracsize = width * 0.57
    --local fracxstart = width * 80 / 280
    local fracxstart = width * 0.22
    --local fracystart = height * 121 / 192
    local fracystart = height * 0.735

    -- begin
    --   initturtle;
    sg:set_color(0, 0, 0)
    sg:rectangle(true, 0, 0, width, height)
    --   moveto (fracxstart, fracystart);
    moveto (fracxstart, fracystart)
    --   pencolor (white);
    pencolor ("white")
    --   for fracount := 1 to 3 do
    --     begin
    for fracount = 1, 3 do
      --       fractal (fracsize, fraclevel);
      fractal (fracsize, fraclevel)
      turn (-120)
      --     end;
    end
    --   readln
    sg:update()
    -- end.
end

gtk.init()
local window = gtk.window.new()
local vbox   = gtk.vbox.new (false, 0)
local canvas = gtk.drawing_area.new()
local hbox   = gtk.hbox.new (false, 0)
local label
local combo  = gtk.combo_box.new_text()
local adjust = gtk.adjustment.new(2, 0, 17, 1, 17, 0)
local hscale = gtk.hscale.new(adjust)

sg = simple_graphics.new(canvas, width, height)

local curve = 0
local level = 2

function draw_fractal()
  if curve == 0 then
    draw_dragon(level)
  elseif curve == 1 then
    draw_dragon2(level)
  else -- curve == 3
    snowflake(level)
  end
end

function combo_changed(self)
  curve = self:get("active")
  draw_fractal()
end

function hscale_changed()
  level = adjust:get_value()
  draw_fractal()
end

window:set("title", "simple_graphics fractal curve demo")
window:signal_connect('destroy', gtk.main_quit)
window:signal_connect("delete-event", function() gtk.main_quit() return true end)
window:add(vbox)
canvas:set("width-request", width, "height-request", height)
vbox:add(canvas)
vbox:add(hbox)
label = gtk.label.new("Curve:")
hbox:pack_start(label, false, false, 5)
combo:append_text("dragoncurve", "dragoncurve2", "snowflake")
combo:set_active(curve)
combo:signal_connect("changed", combo_changed)
hbox:pack_start(combo, false, false, 5)
label = gtk.label.new("Level:")
hbox:pack_start(label, false, false, 5)
adjust:signal_connect('value-changed', hscale_changed)
hscale:set("digits", 0)
hbox:add(hscale)
window:show_all()

draw_fractal()

gtk.main()

simple_graphics.destroy(sg)

