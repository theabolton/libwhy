/* whyutil.h - libwhy utility functions
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

#ifndef _WHYUTIL_H
#define _WHYUTIL_H

#include <stdarg.h>

int y_vsscanf(const char *buffer, const char *format, va_list ap);
int y_atof(const char *buffer, double *result);
int y_sscanf(const char *buffer, const char *format, ...);

#endif /* _WHYUTIL_H */
