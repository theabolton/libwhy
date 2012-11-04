libwhy
------
libwhy is a library for developing `GTK+`_ applications using Lua_.

libwhy is written and copyright (c) 2012 by `Sean Bolton`_, and is
licensed under the GNU Lesser General Public License, version 2.1.
See the enclosed file LICENSE-LGPL for more information.

A number of the files within this package are also licensed under
other terms. In particular, Lua is under an MIT/Expat license, see
``gui/lua/LICENSE-MIT``, and the example code has been released to the
public domain, see ``examples/LICENSE-CC0``.

There is no documentation, other than this README and the source.

The Lua_ in libwhy is a stock Lua 5.2.1, with no ``LUA_COMPAT_*``
macros defined, plus a patch which adds a '%C' format specifier to
string.format. This format acts just like '%g', but always outputs
numbers in the 'C' locale, that is, with '.' for the decimal point.

libwhy also installs a Lua binary as ``<prefix>/bin/ylua``. ``ylua``
links to libwhy, making select portions of GTK+ and the libwhy
custom GTK+ widgets available to Lua scripts.  ``ylua`` includes Mike
Pall's `advanced readline patch`_.  The example programs in the
``example/`` directory can be run using ``ylua``, for example:

``$ ylua examples/epicycloids.lua``

The latest version of libwhy may be found on github:

    https://github.com/smbolton/libwhy

while release tarballs may be found here:

    http://smbolton.com/linux.html

.. _GTK+: http://www.gtk.org/
.. _Lua: http://www.lua.org/
.. _`Sean Bolton`: http://smbolton.com/
.. _`advanced readline patch`: http://smbolton.com/lua.html#readline

