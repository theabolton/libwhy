/* ygtk.h - a lightweight Lua GTK+ binding
 *
 * Copyright (c) 2012 Sean Bolton.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License, version 2.1, as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifndef __YGTK_H__
#define __YGTK_H__

#include <glib.h>

#ifdef YGTK_STANDALONE
#include "lua.h"
#else
#include "why/lua.h"
#endif

typedef struct _ygtk_object {
    gpointer instance;  /* silly that it contains only this, but light user data are not garbage collected.... */
} ygtk_object;

void *ygtk_checkuserdata(lua_State *L, int index);

int luaopen_ygtk (lua_State *L);

#endif /* __YGTK_H__ */

