#!/usr/bin/env ylua

-- basic_GTKHScale_example.lua

-- A simple demonstration of using GtkHScale widgets with libwhy.

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

-- initialize GTK+
gtk.init()

-- create the widgets
local window  = gtk.window.new()
local vbox    = gtk.vbox.new(true, 8)
local adjust1 = gtk.adjustment.new(1, 1, 100, 1, 10, 0)
local hscale1 = gtk.hscale.new(adjust1)
local adjust2 = gtk.adjustment.new(1, 1, 100, 1, 10, 0)
local hscale2 = gtk.hscale.new(adjust2)

-- set widget properties
window:set("title", "ylua GtkHScale demo")
window:set("width-request", 200)
hscale2:set("update-policy", gtk.UPDATE_DELAYED,
            "digits", 0)

-- assemble the layout
window:add(vbox)
vbox:add(hscale1)
vbox:add(hscale2)

-- the callback function for when the hscale adjustments are changed
function hscale_value_changed(self, index)
  local value = self:get_value()
  print (string.format("hscale_value_changed: index = %d, value = %f", index, value))
end

-- connect the signals
window:signal_connect('destroy', gtk.main_quit)
window:signal_connect("delete-event", function() gtk.main_quit() return true end)
adjust1:signal_connect('value-changed', hscale_value_changed, 1)
adjust2:signal_connect('value-changed', hscale_value_changed, 2)

-- and go...
window:show_all()
gtk.main()

