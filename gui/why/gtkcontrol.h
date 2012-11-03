/* gtkcontrol.h - happy shiny image-based knobs and sliders
 *
 * Copyright (C) 2009-2012 Sean Bolton.
 *
 * This version of gtkcontrol.h contains no subpixmap code, which
 * means it needs about GTK+ version 2.10 or later to function
 * correctly.
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

#ifndef __GTK_CONTROL_H__
#define __GTK_CONTROL_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#define GTK_CONTROL_SET_BG_IN_STYLE
    
G_BEGIN_DECLS

typedef enum
{
    GTK_CONTROL_ROTARY,      /* rotary 'knob', 270 degree rotation, minimum at 1.25 pi */
    GTK_CONTROL_HORIZONTAL,  /* horizontal slider, minimum to the left */
    GTK_CONTROL_VERTICAL,    /* vertical slider, minimum at the bottom */
    GTK_CONTROL_DRAWBAR,     /* vertical slider, minimum at the top */
    GTK_CONTROL_BUTTON,      /* momentary on-off button */
    GTK_CONTROL_SWITCH,      /* toggling on-off button */
    GTK_CONTROL_RADIO        /* one of several radio buttons, sharing a single adjustment */
} GtkControlType;

#define GTK_TYPE_CONTROL            (gtk_control_get_type ())
#define GTK_CONTROL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CONTROL, GtkControl))
#define GTK_CONTROL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_CONTROL, GtkControlClass))
#define GTK_IS_CONTROL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CONTROL))
#define GTK_IS_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_CONTROL))
#define GTK_CONTROL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CONTROL, GtkControlClass))

typedef struct _GtkControl       GtkControl;
typedef struct _GtkControlClass  GtkControlClass;

struct _GtkControl
{
    GtkWidget widget;

    GtkAdjustment *adjustment;

    guint          policy;   /* update policy (GTK_UPDATE_[CONTINUOUS/DELAYED/DISCONTINUOUS]) */
    guint          state;
    GtkControlType type;
    gint           saved_x;
    gint           saved_y;
    gfloat         saved_value; /* adjustment value at button press */
    gfloat         old_value;   /* value at last 'value-changed' signal */
    gint           prelit : 1;
    gboolean       prefer_radial;

    GdkPixbuf     *pixbuf;
    guint          src_width;
    guint          src_height;

    guint          width;
    guint          height;
    guint          range;    /* 0 to 'range' inclusive means (range + 1) animation frames or slider steps. */
                             /* For GTK_CONTROL_RADIO, range must be 1. */
    gint           radio_value;
    gboolean       integral;

    gint           origin_x;
    gint           origin_y;
    gint           stride_x;
    gint           stride_y;
    gint           prelight_x;  /* offset from non-prelit animation frame to prelit one; leave at zero if no prelight animation */
    gint           prelight_y;
    gint           insensitive_x; /* offset from origin to single insensitive animation frame; if zero, widget sensitivity is ignored when rendering */
    gint           insensitive_y;

    guint32        timer;    /* ID of update timer, or 0 if none */
};

struct _GtkControlClass
{
    GtkWidgetClass parent_class;
};


GType          gtk_control_get_type (void) G_GNUC_CONST;
GtkWidget     *gtk_control_new (GtkAdjustment *adjustment,
                                GdkPixbuf     *pixbuf,
                                guint          width,
                                guint          height,
                                guint          range,
                                gint           origin_x,
                                gint           origin_y,
                                gint           stride_x,
                                gint           stride_y,
                                GtkControlType type);

GtkAdjustment *gtk_control_get_adjustment(GtkControl *control);
void           gtk_control_set_adjustment(GtkControl *control, GtkAdjustment *adjustment);

GtkUpdateType  gtk_control_get_update_policy(GtkControl *control);
void           gtk_control_set_update_policy(GtkControl *control, GtkUpdateType  policy);

gint           gtk_control_get_radio_value (GtkControl *control);
void           gtk_control_set_radio_value (GtkControl *control, gint value);

gboolean       gtk_control_get_integral (GtkControl *control);
void           gtk_control_set_integral (GtkControl *control, gboolean setting);

/* -FIX- add getters for these */
void           gtk_control_set_prelight_offsets (GtkControl *control, gint offset_x, gint offset_y);
void           gtk_control_set_insensitive_offsets (GtkControl *control, gint offset_x, gint offset_y);

#ifdef GTK_CONTROL_SET_BG_IN_STYLE
void           gtk_control_set_widget_bg_parent_relative_in_style(GtkWidget *widget);
#endif

G_END_DECLS

#endif /* __GTK_CONTROL_H__ */
