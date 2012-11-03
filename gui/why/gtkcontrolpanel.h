/* gtkcontrolpanel.h - come at the end of the shift, we'll go and
 *
 * Copyright (C) 2009 Sean Bolton
 *
 * This version of gtkcontrolpanel.h contains no subpixmap code,
 * which means it needs about GTK+ version 2.10 or later to
 * function correctly.
 *
 * Parts of this code come from GTK+, both the library source and
 * the example programs.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
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

#ifndef __GTK_CONTROL_PANEL_H__
#define __GTK_CONTROL_PANEL_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_TYPE_CONTROL_PANEL            (gtk_control_panel_get_type ())
#define GTK_CONTROL_PANEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CONTROL_PANEL, GtkControlPanel))
#define GTK_CONTROL_PANEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_CONTROL_PANEL, GtkControlPanelClass))
#define GTK_IS_CONTROL_PANEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CONTROL_PANEL))
#define GTK_IS_CONTROL_PANEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_CONTROL_PANEL))
#define GTK_CONTROL_PANEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CONTROL_PANEL, GtkControlPanelClass))

typedef struct _GtkControlPanel       GtkControlPanel;
typedef struct _GtkControlPanelClass  GtkControlPanelClass;
typedef struct _GtkControlPanelChild  GtkControlPanelChild;

struct _GtkControlPanel
{
    GtkContainer container;

    GdkPixbuf *pixbuf;
    GdkPixmap *pixmap;
    gboolean   fixed_width;
    gboolean   fixed_height;
    guint      width;
    guint      height;

    GList   *children;
};

struct _GtkControlPanelClass
{
    GtkContainerClass parent_class;
};

struct _GtkControlPanelChild
{
    GtkWidget *widget;
    gint x;
    gint y;
};


GType      gtk_control_panel_get_type  (void) G_GNUC_CONST;

/* gtk_control_panel_new:
 * if width is 0, then the width of the pixbuf is used
 * if width is -1, then the width is calculated based on the needs of the child widgets
 * if width is >0, then that is the fixed width
 * height is handled similarly */
GtkWidget* gtk_control_panel_new       (GdkPixbuf *pixbuf,
                                        gint width,
                                        gint height);
void       gtk_control_panel_put       (GtkControlPanel   *control_panel,
                                        GtkWidget         *widget,
                                        gint               x,
                                        gint               y);
void       gtk_control_panel_move      (GtkControlPanel   *control_panel,
                                        GtkWidget         *widget,
                                        gint               x,
                                        gint               y);

G_END_DECLS

#endif /* __GTK_CONTROL_PANEL_H__ */

