#!/usr/bin/env ylua

-- sg04-turtle_graphics_demo.lua

-- A libwhy simple_graphics module demonstration program. It is a translation
-- of Logo turtle graphics routines featured in the Logo Art Gallery, once
-- found at:
--     http://www.geocities.com/CollegePark/Lab/2276/
-- and more recently available at:
--     http://portal.ort.spb.ru/lib/Documents/LogoGallery/fractals.htm

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

local width  = 960
local height = 720
local window = gtk.window.new()
local canvas = gtk.drawing_area.new()

canvas:set("width-request", width, "height-request", height)
window:set("title", "simple_graphics Logo Art Gallery demo")
window:add(canvas)
window:show_all()

window:signal_connect('destroy', gtk.main_quit)
window:signal_connect("delete-event", function() gtk.main_quit() return true end)

local sg = simple_graphics.new(canvas, width, height)

local pendown = true
local old_x = 0
local old_y = 0
local dx = width / 2
local dy = height / 2
local angle = math.pi / 2
function pencolor(color)
  if color == "none" then
    pendown = false
  elseif color == "black" or color == 0 then
    pendown = true
    sg:set_color(0, 0, 0)
  elseif color == "blue" or color == 1 then
    pendown = true
    sg:set_color(0, 0, 1)
  elseif color == "green" or color == 2 then
    pendown = true
    sg:set_color(0, 1, 0)
  elseif color == "cyan" or color == 3 then
    pendown = true
    sg:set_color(0, 1, 1)
  elseif color == "red" or color == 4 then
    pendown = true
    sg:set_color(1, 0, 0)
  elseif color == "magenta" or color == 5 then
    pendown = true
    sg:set_color(1, 1, 0)
  elseif color == "yellow" or color == 6 then
    pendown = true
    sg:set_color(1, 1, 0)
  elseif color == "white" or color == 7 then
    pendown = true
    sg:set_color(1, 1, 1)
  elseif color == "grey" or color == 15 then
    pendown = true
    sg:set_color(0.5, 0.5, 0.5)
  else
    error(string.format("unknown pen color '%s'", color))
  end
end
function setpc(x) pencolor(x) end
function pu() pendown = false end
function pd() pendown = true end
function moveto(x, y)
  if pendown then
    sg:line(dx + old_x, dy - old_y, dx + x, dy - y)
  end
  old_x = x
  old_y = y
end
local setxy = moveto
function move(len)
  moveto(old_x + math.cos(angle) * len, old_y + math.sin(angle) * len)
end
local fd = move
function bk(len) move(-len) end
function turn(w)
  angle = angle - w / 180 * math.pi
end
local rt = turn
function lt(w) turn(-w) end
function turnto(w)
  -- "Headings (angles) are measured in degrees clockwise from the positive Y axis."
  angle = (90 - w) / 180 * math.pi
end
local seth = turnto
function reset()
  sg:set_color(0, 0, 0)
  sg:rectangle(true, 0, 0, width, height)
end

function _3d_frame() -- draw "3D" frame
  -- reset()
  setpc(7)
  sph(-110, -110,  0) fd(220) rt(90) fd(220)
  sph(-109, -109,  0) fd(218) rt(90) fd(218)
  sph(-102, -102, 90) fd(204) lt(90) fd(204)
  setpc(0)
  sph(-110, -110, 90) fd(220) lt(90) fd(220)
  sph(-109, -109, 90) fd(218) lt(90) fd(218)
  sph(-102, -102,  0) fd(204) rt(90) fd(204)
  -- was: sph(0, 0, 0) setfc(0) fill()
  setpc(15)
  for i = 103, 108 do
    local i2 = i * 2
    sph(-i, -i, 0) fd(i2) rt(90) fd(i2) rt(90) fd(i2) rt(90) fd(i2)
  end
  sph(0, 0, 0)
end

function sp(x, y) -- move to selected location
  pu() setxy(x, y) pd()
end

function sph(x, y, head) -- move to selected location & set heading
  pu() setxy(x, y) pd()
  seth(head)
end

function sphc(x, y, head, col) -- move to selected location, set heading & color
  pu() setxy(x, y) pd()
  seth(head)
  setpc(col)
end

function pos()
  return {old_x, old_y}
end

function setpos(l)
  local x, y = table.unpack(l)
  setxy(x, y)
end

-- -----------------------------

function next_dx_and_dy()
  dx = dx + 240
  if dx > 840 then
    dx = 120
    dy = dy + 240
  end
end

-------------- codes_str.thm: String Art -----------------

-- -----------------------------

function _4sqrs ()
  _3d_frame()
  setpc(3) squares(90, 4)
end

function squares(size, step)
  local function draw(dir, size)
    function square()
      for _ = 1, 4 do
        fd(size) rt(dir * 90)
      end
    end
    function _move()
      fd(step)
      rt(dir * math.atan(step/(size-step)) / math.pi * 180)
    end
    function newsize()
      return math.sqrt(size * size - 2 * step * (size - step))
    end
    if size > step then
      square() _move()
      draw(dir, newsize())
    end
  end
                 draw( 1, size)
  sph(0, 0,   0) draw(-1, size)
  sph(0, 0, 180) draw( 1, size)
  sph(0, 0, 180) draw(-1, size)
end

reset()
dx = 120 dy = 120
_4sqrs()
sg:update()

-- -----------------------------

function _6tris()
  local function tri(size, dir)
    for _ = 1, 3 do
      fd(size) rt(dir * 120)
    end
  end
  local function tris(size, dir)
    if size < 20 then
      return
    end
    tri(size, dir)
    fd(size / 15)
    rt(dir * 4)
    tris(0.9 * size, dir)
  end
  local function draw(size)
    for i = 0, 2 do
      sph(0, 0, 120 * i) tris(size, 1)
      sph(0, 0, 120 * i) tris(size, -1)
    end
  end
  _3d_frame()
  setpc(4) draw(96)
end

next_dx_and_dy()
_6tris()
sg:update()

-- -----------------------------

-- function Eye()
--   local function draw(ll1, ll2, times, incr)
--     local function connect()
--       pu() setpos(table.remove(ll1))
--       pd() setpos(table.remove(ll2))
--     end
--     for _ = 1, times do
--       fd(incr)
--       table.insert(ll1, pos())
--     end
--     lt(90)
--     for _ = 1, times do
--       fd(incr)
--       table.insert(ll2, pos())
--     end
--     for _ = 1, times do
--       connect()
--     end
--     fd((times-1)*incr) lt(90)
--   end
--   _3d_frame()
--   sphc(86, -86, 0, 6)
--   for _ = 1, 2 do
--     draw({}, {}, 25, 7)
--   end
-- end
-- 
-- next_dx_and_dy()
-- Eye()
-- sg:update()

-- -----------------------------

function cross() -- by Olga Tusova
  local function go(l, n)
    local function _do(x, y, x1, y1, stepx, stepy)
      local function draw()
        pd() setxy(x, y1)
        x1 = x1 + stepx
        y1 = y1 + stepy
        pu() setxy(x1, y)
      end
      setxy(x1, y1)
      for _ = 1, n + 1 do
        draw()
      end
    end
    local function setup(step, l)
      _do( 0,  0, -l,  0,  step,  step) -- NW inside
      _do( 0,  0,  l,  0, -step,  step) -- NE inside
      _do( 0,  0, -l,  0,  step, -step) -- SW inside
      _do( 0,  0,  l,  0, -step, -step) -- SE inside
      _do(-l,  l,  0,  l, -step, -step) -- NW frame
      _do( l,  l,  0,  l,  step, -step) -- NE frame
      _do( l, -l,  0, -l,  step,  step) -- SE frame
      _do(-l, -l,  0, -l, -step,  step) -- SW frame
    end
    setup(l/n, l)
  end
  _3d_frame() setpc(3)
  go(85, 14)
end

next_dx_and_dy()
cross()
sg:update()

-- -----------------------------

function parabola_and_evolute() -- Parabola & Evolute by Mike Sandy
  local function plot_line(p1, p2)
    pu() setpos(p1)
    pd() setpos(p2)
  end
  local function envelope(x_low, x_upp, y_low, y_upp, incr)
    -- 1ST 4 PARAMS BOUNDARY VALUES :incr - X INCREMENT
    local function pt(x_bound, fny, fnx)
      local y = fny(x_bound)
      if y < y_low then
        return {fnx(y_low), y_low}
      end
      if y > y_upp then
        return {fnx(y_upp), y_upp}
      end
      return {x_bound, y}
    end
    local a = 1/x_upp
    local function grad(x)
      return 2 * a * x
    end
    local function y(x)
      return a * x * x - 50
    end
    for x0 = x_low, x_upp, incr do
      local function yt(x)
        return (x - x0) * grad(x0)+y(x0)
      end
      local function xt(_y)
        return (_y - y(x0)) / grad(x0) + x0
      end
      local function yn(x)
        return y(x0) - (x - x0) / grad(x0)
      end
      local function xn(_y)
        return grad(x0) * (y(x0) - _y) + x0
      end
      setpc(2) plot_line(pt(x_low, yt, xt), pt(x_upp, yt, xt))
      if x0 == 0 then
        setpc(4) plot_line({0, y_low}, {0, y_upp})
      else
        setpc(4) plot_line(pt(x_low, yn, xn), pt(x_upp, yn, xn))
      end
    end
  end
  _3d_frame()
  envelope(-93, 93, -93, 93, 5)
end

next_dx_and_dy()
parabola_and_evolute()
sg:update()

-- -----------------------------

-------------- codesf.thm: Fractals and Recursive Graphics -----------------

-- -----------------------------

function Fern() -- Fern by Bill Beaumont with thanks to Bill Kerr
  local function draw(size)                  -- to draw :size
    if size < 5 then return end              -- if :size<5[stop]
    fd(size / 25)                            -- fd :size/25         ; trunk
    lt(80) draw(size * 0.3)                  -- lt 80 draw :size*.3 ; left branch
    rt(82) fd(size / 25)                     -- rt 82 fd   :size/25
    rt(80) draw(size * 0.3)                  -- rt 80 draw :size*.3 ; rite branch
    lt(78) draw(size * 0.9)                  -- lt 78 draw :size*.9 ; spine
    lt( 2) bk(size / 25)                     -- lt  2 bk   :size/25
    lt( 2) bk(size / 25)                     -- lt  2 bk   :size/25
  end                                        -- end
  _3d_frame()
  sphc(-30, -94, 0, 2) draw(305)
end

next_dx_and_dy()
Fern()
sg:update()

-- -----------------------

function Tree()
  local function _do(size, level)            -- to do :size :level
    if level < 1 then return end -- -FIX-    -- if :level<1[setpc 4 label "* setpc 2 stop]
    lt(25)                                   -- lt 25
    fd(size) _do(0.5 * size, level - 1)      -- fd :size do .5*:size :level-1 bk :size       ; left branch
        bk(size)
    rt(20)                                   -- rt 20
    fd(0.6 * size) _do(0.6 * size,           -- fd .6*:size do .6*:size :level-1 bk .6*:size ; scnd branch
        level - 1) bk(0.6 * size)
    rt(25)                                   -- rt 25
    fd(0.5 * size) _do(0.6 * size,           -- fd .5*:size do .6*:size :level-1 bk .5*:size ; thrd branch
        level - 1) bk (0.5 * size)
    rt(25)                                   -- rt 25
    fd(0.5 * size) _do(0.6 * size,           -- fd .5*:size do .6*:size :level-1 bk .5*:size ; rite branch
        level - 1) bk (0.5 * size)
    lt(45)                                   -- lt 45
  end                                        -- end
  _3d_frame()
  sp(0, -45) _do(75, 4)
  bk(40)
end

next_dx_and_dy()
Tree()
sg:update()

-- ---------------------

function Stars()                             -- to main ; Stars
  local function draw(size, level)           -- to draw :size :level
    local function shape()                     -- to shape
      setpc(7 - level)                         -- setpc 7-:level
      fd(size) rt(90)                          -- fd :size rt 90
      fd(size) rt(30)                          -- fd :size rt 30
      draw(size / 2, level - 1)                -- draw :size/2 :level-1
      lt(180)                                  -- lt 180
    end                                        -- end
    if level < 1 then return end             -- if :level<1[stop]
    for i = 1, 6 do shape() end              -- repeat 6[shape]
  end                                        -- end
  _3d_frame()                                -- 3d_frame
  sph(18, -30, -15) draw(25, 4)              -- sph 18 -30 -15 draw 25 4
end                                          -- end

next_dx_and_dy()
Stars()
sg:update()

-- ------------------------

function Peano()
  local function draw(size, ang, level)
    if level < 1 then return end
    lt(ang)
    draw(size, -ang, level - 1) fd(size) rt(ang)
    draw(size,  ang, level - 1) fd(size)
    draw(size,  ang, level - 1) rt(ang) fd(size)
    draw(size, -ang, level - 1) lt(ang)
  end

  _3d_frame()
  sphc(92, -92, 0, 6) draw(6, 90, 5)
end

next_dx_and_dy()
Peano()
sg:update()

-- ---------------------

function Dragons()
  local function dragon(level, turn, size)
    if level < 1 then
      fd(size)
      return
    end
    dragon(level - 1,  90, size)
    rt(turn)
    dragon(level - 1, -90, size)
  end

  _3d_frame()
  for i = 1, 4 do
    sph(0, 0, i * 90)
    setpc(i)
    dragon(9, 90, 4)
  end
end

next_dx_and_dy()
Dragons()
sg:update()

-- -----------------------------

function Bush()
  local function bush(size, level)
    if level < 1 then return end
    setpc(math.random(7)) fd(size)
    rt(75) bush(size / 1.5, level - 1)
    lt(90) bush(size / 1.5, level - 1)
    rt(15) bk(size)
  end

  _3d_frame()
  
  sph(-20, -85, 0) bush(60, 11)
end

next_dx_and_dy()
Bush()
sg:update()

-- --------------------

function Lattice()                           -- to main ; Lattice
  local function draw(long, wide, count)     -- to draw :long :wide :count
    local function shape(long, wide, count)    -- to shape :long :wide :count
      if count < 1 then return end             -- if :count<1[stop]
      for i = 1, 2 do fd(long) rt(120)         -- repeat 2[fd :long rt 120 fd :wide rt 60]
          fd(wide) rt(60) end
      lt(120)                                  -- lt 120
      draw(long, wide, count - 1)              -- draw :long :wide :count-1
      rt(120)                                  -- rt 120
      fd(wide) lt(120) fd(long)                -- fd :wide lt 120 fd :long
    end                                        -- end
    for i = 1, 6 do shape(long, wide, count) -- repeat 6[shape :long :wide :count rt 60]
        rt(60) end
  end                                        -- end
  _3d_frame()                                -- 3d_frame
  sphc(6, 14, 0, 7) draw(19, 9, 3)           -- sphc 6 14 0 [130 130 130] draw 19 9 3
  --sphc(7, 14, 0, ?) draw(19, 9, 3)         -- sphc 7 14 0 [170 170 170] draw 19 9 3
  --sphc(8, 14, 0, ?) draw(19, 9, 3)         -- sphc 8 14 0 [255 255 255] draw 19 9 3
end                                          -- end

next_dx_and_dy()
Lattice()
sg:update()

-- --------------------

-- function Lasvegas()
--   local function draw(size, level)
--     local function shape()
--       fd(size) lt(60)
--       fd(size) rt(60)
--       fd(size)
--       rt(180) draw(size, level - 1) rt(180)
--       draw(size, level - 1)
--     end
-- 
--     if level < 1 then return end
--     for _ = 1, 3 do shape() rt(120) end
--     -- fill_it()
--   end
-- 
--   _3d_frame()
--   sphc(-7, -20, 0, 3) draw(10, 4)
-- end
-- 
-- -- to fill_it
-- -- setfc 4
-- -- pu rt 22 fd 4 fill bk 4 lt 22 pd
-- -- end
-- 
-- next_dx_and_dy()
-- Lasvegas()
-- sg:update()

-- --------------------

function Penrose() -- by Mike Sandy
  local function draw(size, level)
    local ang = 36
    local tau = (1 + math.sqrt(5)) / 2
    local coord_stack = {}
    local penf = 0
    local small

    local function np()
      penf = 0
      local coord_posn
      if pendown then
        coord_posn = { 1, old_x, old_y, angle }  -- xcor, ycor, heading
      else
        coord_posn = { 0, old_x, old_y, angle }  -- xcor, ycor, heading
      end
      table.insert(coord_stack, coord_posn)
    end

    local function rp()
      pu()
      local coord_posn = table.remove(coord_stack)
      penf, old_x, old_y, angle = table.unpack(coord_posn)
      if penf == 1 then pd() end
    end

    local function large(l, n)
      if n == 0 then pu() return end
      if n == 1 then pd() end
      fd(l) lt(2 * ang)
      np()
      lt(ang) small(l / tau, n - 1)
      rp()
      fd(l) lt(3 * ang)
      np()
      lt(ang) large(l / tau, n - 1)
      rt(2 * ang) large(l / tau, n - 1)
      rp()
      fd(l)
      np()
      small(l / tau, n - 1) rt(ang) bk(l / tau)
      lt(ang) large(l / tau, n - 1)
      rp()
      lt(2 * ang)
      fd(l) lt(3 * ang)
      pu()
      if n == 1 then
        np() lt(20) fd(5) -- -FIX- setfc(4) fill()
        rp()
      end
    end

    local function _small(l, n)
      if n == 0 then pu() return end
      if n == 1 then pd() end
      np()
      rt(ang) large(l / tau, n - 1)
      np()
      lt(3 * ang) fd(l / tau) rt(180)
      small(l / tau, n - 1)
      rt(4 * ang) small(l / tau, n - 1)
      rp()
      lt(4 * ang) large(l / tau, n - 1)
      rp()
      fd(l) lt(4 * ang) fd(l) lt(ang)
      fd(l) lt(4 * ang) fd(l) lt(ang)
      pu()
      if n == 1 then
        np() lt(20) fd(5) -- -FIX- setfc(2) fill()
        rp()
      end
    end
    small = _small

    local function pent(l, n)
      if n == 0 then pu() return end
      -- draw central pattern
      for _ = 1, 5 do
        lt(ang) large(l, n) rt(ang)
        fd(l) lt(ang) fd(l) lt(ang)
      end
    end

    bk(size * tau)
    rt(2 * ang)
    -- small :size :level
    -- large :size :level
    pent(size, level)
  end

  _3d_frame()
  setpc(1) pu()
  draw(50, 4)
end

next_dx_and_dy()
Penrose()
sg:update()

-- --------------------

gtk.main()

simple_graphics.destroy(sg)

