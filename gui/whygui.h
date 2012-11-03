/* whygui.h - a lightweight Lua GTK+ binding
 *
 * Copyright (c) 2011 Sean Bolton and others.
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

#ifndef _WHYGUI_H_
#define _WHYGUI_H_

#ifdef YGTK_USE_GTK_KNOB
#include "why/gtkknob.h"
#endif

#ifdef YGTK_USE_GTK_CONTROL
#include "why/gtkcontrolpanel.h"
#include "why/gtkcontrol.h"
#endif

#include "why/lua.h"
#include "why/lualib.h"
#include "why/lauxlib.h"

#include "why/ygtk.h"

#endif /* _WHYGUI_H_ */
