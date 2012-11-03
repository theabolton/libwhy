/* gtkcontrol.c - happy shiny image-based knobs and sliders
 *
 * Copyright (C) 2009-2012 Sean Bolton.
 *
 * This version of gtkcontrol.c contains no subpixmap code, which
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

#include <stdlib.h>
#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "why/gtkcontrol.h"

#define UPDATE_DELAY_LENGTH    300

enum {
    PROP_0,
    PROP_UPDATE_POLICY,
    PROP_RADIO_VALUE,
    PROP_INTEGRAL,
    PROP_PRELIGHT_X,
    PROP_PRELIGHT_Y,
    PROP_INSENSITIVE_X,
    PROP_INSENSITIVE_Y
};

enum {
    STATE_IDLE,
    STATE_PRESSED,
    STATE_DRAGGING
};

G_DEFINE_TYPE (GtkControl, gtk_control, GTK_TYPE_WIDGET);

static void     gtk_control_class_init    (GtkControlClass *klass);
static void     gtk_control_set_property  (GObject          *object,
                                           guint             prop_id,
                                           const GValue     *value,
                                           GParamSpec       *pspec);
static void     gtk_control_get_property  (GObject          *object,
                                           guint             prop_id,
                                           GValue           *value,
                                           GParamSpec       *pspec);
static void     gtk_control_init          (GtkControl *control);
static void     gtk_control_destroy       (GtkObject *object);
static void     gtk_control_realize       (GtkWidget *widget);
static void     gtk_control_size_request  (GtkWidget *widget, GtkRequisition *requisition);
static void     gtk_control_size_allocate (GtkWidget *widget, GtkAllocation  *allocation);
static gint     gtk_control_button_press  (GtkWidget *widget, GdkEventButton *event);
static gint     gtk_control_button_release(GtkWidget *widget, GdkEventButton *event);
static gint     gtk_control_motion_notify (GtkWidget *widget, GdkEventMotion *event);
static gboolean gtk_control_scroll        (GtkWidget *widget, GdkEventScroll *event);
static gint     gtk_control_timer_callback(GtkControl *control);
static void     gtk_control_update_mouse  (GtkControl *control, gint x, gint y, gboolean absolute);
static gint     gtk_control_enter_notify  (GtkWidget * widget, GdkEventCrossing * event);
static gint     gtk_control_leave_notify  (GtkWidget * widget, GdkEventCrossing * event);
static void     gtk_control_adjustment_value_changed (GtkAdjustment *adjustment,
                                                      gpointer       data);
static void     gtk_control_adjustment_changed (GtkAdjustment *adjustment,
                                                gpointer       data);
static gboolean gtk_control_expose        (GtkWidget *control, GdkEventExpose *event);


static void
gtk_control_class_init (GtkControlClass *class)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS(class);
    GtkObjectClass *object_class  = (GtkObjectClass*) class;
    GtkWidgetClass *widget_class  = (GtkWidgetClass*) class;

    gobject_class->set_property = gtk_control_set_property;
    gobject_class->get_property = gtk_control_get_property;
    
    object_class->destroy = gtk_control_destroy;

    widget_class->realize = gtk_control_realize;
    widget_class->size_request = gtk_control_size_request;
    widget_class->size_allocate = gtk_control_size_allocate;
    widget_class->expose_event = gtk_control_expose;
    widget_class->button_press_event = gtk_control_button_press;
    widget_class->button_release_event = gtk_control_button_release;
    widget_class->motion_notify_event = gtk_control_motion_notify;
    widget_class->scroll_event = gtk_control_scroll;
    widget_class->enter_notify_event = gtk_control_enter_notify;
    widget_class->leave_notify_event = gtk_control_leave_notify;

#ifndef GTK_PARAM_READWRITE
#define GTK_PARAM_READWRITE G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB
#endif
    g_object_class_install_property (gobject_class,
                                     PROP_UPDATE_POLICY,
                                     g_param_spec_enum ("update-policy",
                                                        "Update policy",
                                                        "When changes to a control should update its GtkAdjustment",
                                                        GTK_TYPE_UPDATE_TYPE,
                                                        GTK_UPDATE_CONTINUOUS,
                                                        GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_RADIO_VALUE,
                                     g_param_spec_int ("radio-value",
                                                       "Radio value",
                                                       "Value this control represents in a GTK_CONTROL_RADIO grouping",
                                                       G_MININT,
                                                       G_MAXINT,
                                                       0,
                                                       GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_INTEGRAL,
                                     g_param_spec_boolean ("integral",
                                                           "Integral",
                                                           "Whether to restrict control values to integers",
                                                           FALSE,
                                                           GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_PRELIGHT_X,
                                     g_param_spec_int ("prelight-x",
                                                       "Prelight X",
                                                       "Offset on X-axis from non-prelit animation frame to prelit one",
                                                       G_MININT,
                                                       G_MAXINT,
                                                       0,
                                                       GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_PRELIGHT_Y,
                                     g_param_spec_int ("prelight-y",
                                                       "Prelight Y",
                                                       "Offset on Y-axis from non-prelit animation frame to prelit one",
                                                       G_MININT,
                                                       G_MAXINT,
                                                       0,
                                                       GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_INSENSITIVE_X,
                                     g_param_spec_int ("insensitive-x",
                                                       "Insensitive X",
                                                       "Offset on X-axis from origin to insensitive animation frame",
                                                       0,
                                                       G_MAXINT,
                                                       0,
                                                       GTK_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_INSENSITIVE_Y,
                                     g_param_spec_int ("insensitive-y",
                                                       "Insensitive Y",
                                                       "Offset on Y-axis from origin to insensitive animation frame",
                                                       0,
                                                       G_MAXINT,
                                                       0,
                                                       GTK_PARAM_READWRITE));
    /* ugly: GtkKnob also uses this setting property, but GtkKnob may be used independently
     * from GtkControl, so GtkKnob will also install this setting property if necessary. So,
     * we have to check first if it already exists. */
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(gtk_settings_get_default()),
                                     "gtk-control-rotary-prefer-radial") == NULL)
        gtk_settings_install_property (g_param_spec_boolean ("gtk-control-rotary-prefer-radial",
                                                             "Rotary GtkControls prefer radial mode",
                                                             "Whether rotary control's primary mode of operation should be radial",
                                                             FALSE,
                                                             GTK_PARAM_READWRITE));
}


static void
gtk_control_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    GtkControl *control = GTK_CONTROL (object);

    switch (prop_id) {
      case PROP_UPDATE_POLICY:
        gtk_control_set_update_policy (control, g_value_get_enum (value));
        break;
      case PROP_RADIO_VALUE:
        gtk_control_set_radio_value (control, g_value_get_int (value));
        break;
      case PROP_INTEGRAL:
        gtk_control_set_integral (control, g_value_get_boolean (value));
        break;
      case PROP_PRELIGHT_X:
        gtk_control_set_prelight_offsets (control, g_value_get_int (value), control->prelight_y);
        break;
      case PROP_PRELIGHT_Y:
        gtk_control_set_prelight_offsets (control, control->prelight_x, g_value_get_int (value));
        break;
      case PROP_INSENSITIVE_X:
        gtk_control_set_insensitive_offsets (control, g_value_get_int (value), control->insensitive_y);
        break;
      case PROP_INSENSITIVE_Y:
        gtk_control_set_insensitive_offsets (control, control->insensitive_x, g_value_get_int (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
gtk_control_get_property (GObject      *object,
                          guint         prop_id,
                          GValue       *value,
                          GParamSpec   *pspec)
{
    GtkControl *control = GTK_CONTROL (object);

    switch (prop_id) {
      case PROP_UPDATE_POLICY:
        g_value_set_enum (value, control->policy);
        break;
      case PROP_RADIO_VALUE:
        g_value_set_int (value, control->radio_value);
        break;
      case PROP_INTEGRAL:
        g_value_set_boolean (value, control->integral);
        break;
      case PROP_PRELIGHT_X:
        g_value_set_int (value, control->prelight_x);
        break;
      case PROP_PRELIGHT_Y:
        g_value_set_int (value, control->prelight_y);
        break;
      case PROP_INSENSITIVE_X:
        g_value_set_int (value, control->insensitive_x);
        break;
      case PROP_INSENSITIVE_Y:
        g_value_set_int (value, control->insensitive_y);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
gtk_control_init (GtkControl *control)
{
    control->adjustment = NULL;
    control->policy = GTK_UPDATE_CONTINUOUS;
    control->state = STATE_IDLE;
    control->type = GTK_CONTROL_ROTARY;
    control->old_value = 0.0;
    control->prelit = FALSE;
    g_object_get (gtk_widget_get_settings (GTK_WIDGET(control)),
                  "gtk-control-rotary-prefer-radial",
                  &control->prefer_radial, NULL);
    control->pixbuf = NULL;
    control->integral = FALSE;
    control->prelight_x = 0;
    control->prelight_y = 0;
    control->insensitive_x = 0;
    control->insensitive_y = 0;
    control->timer = 0;
}


GtkWidget*
gtk_control_new (GtkAdjustment *adjustment,
                 GdkPixbuf     *pixbuf,
                 guint          width,
                 guint          height,
                 guint          range,
                 gint           origin_x,
                 gint           origin_y,
                 gint           stride_x,
                 gint           stride_y,
                 GtkControlType type)
{
    GtkControl *control;

    g_return_val_if_fail (adjustment == NULL || GTK_IS_ADJUSTMENT (adjustment), NULL);
    g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

    control = g_object_new (GTK_TYPE_CONTROL, NULL);

    gtk_control_set_adjustment(control, adjustment);

    g_object_ref(pixbuf);
    control->pixbuf      = pixbuf;

    control->type        = type;
    control->width       = width;
    control->height      = height;
    control->range       = range;
    control->origin_x    = origin_x;
    control->origin_y    = origin_y;
    control->stride_x    = stride_x;
    control->stride_y    = stride_y;
    control->src_width   = gdk_pixbuf_get_width(pixbuf);
    control->src_height  = gdk_pixbuf_get_height(pixbuf);

    return GTK_WIDGET(control);
}


static void
gtk_control_destroy (GtkObject *object)
{
    GtkControl *control = GTK_CONTROL(object);

    if (control->adjustment) {
        g_signal_handlers_disconnect_by_func (control->adjustment,
                                              gtk_control_adjustment_changed,
                                              control);
        g_signal_handlers_disconnect_by_func (control->adjustment,
                                              gtk_control_adjustment_value_changed,
                                              control);
        g_object_unref(G_OBJECT(control->adjustment));
        control->adjustment = NULL;
    }
    if (control->pixbuf) {
        g_object_unref(control->pixbuf);
        control->pixbuf = NULL;
    }
    if (control->timer) {
        g_source_remove(control->timer);
        control->timer = 0;
    }

    GTK_OBJECT_CLASS (gtk_control_parent_class)->destroy (object);
}


GtkAdjustment*
gtk_control_get_adjustment (GtkControl *control)
{
    g_return_val_if_fail (GTK_IS_CONTROL (control), NULL);

    if (!control->adjustment)
        gtk_control_set_adjustment (control, NULL);

    return control->adjustment;
}


void
gtk_control_set_adjustment (GtkControl *control, GtkAdjustment *adjustment)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    if (!adjustment)
        adjustment = (GtkAdjustment*) gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    else
        g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));

    if (control->adjustment) {
        g_signal_handlers_disconnect_by_func (control->adjustment,
                                              gtk_control_adjustment_changed,
                                              control);
        g_signal_handlers_disconnect_by_func (control->adjustment,
                                              gtk_control_adjustment_value_changed,
                                              control);
        g_object_unref(G_OBJECT(control->adjustment));
    }

    control->adjustment = adjustment;
#if GLIB_CHECK_VERSION(2, 10, 0)
    g_object_ref_sink(G_OBJECT(control->adjustment));
#else
    g_object_ref(G_OBJECT(control->adjustment));
    gtk_object_sink(GTK_OBJECT(control->adjustment));
#endif

    g_signal_connect(G_OBJECT(adjustment), "changed",
                     G_CALLBACK(gtk_control_adjustment_changed), (gpointer) control);
    g_signal_connect(G_OBJECT(adjustment), "value-changed",
                     G_CALLBACK(gtk_control_adjustment_value_changed), (gpointer) control);

    gtk_control_adjustment_changed (adjustment, control);
}


GtkUpdateType
gtk_control_get_update_policy (GtkControl *control)
{
    g_return_val_if_fail (GTK_IS_CONTROL (control), GTK_UPDATE_CONTINUOUS);

    return control->policy;
}


void
gtk_control_set_update_policy(GtkControl *control, GtkUpdateType policy)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    control->policy = policy;
}


gint
gtk_control_get_radio_value (GtkControl *control)
{
    g_return_val_if_fail (GTK_IS_CONTROL (control), 0);

    return control->radio_value;
}


void
gtk_control_set_radio_value (GtkControl *control, gint value)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    control->radio_value = value;
}


gboolean
gtk_control_get_integral (GtkControl *control)
{
    g_return_val_if_fail (GTK_IS_CONTROL (control), FALSE);

    return control->integral;
}


void
gtk_control_set_integral (GtkControl *control, gboolean setting)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    control->integral = setting;
}


void
gtk_control_set_prelight_offsets (GtkControl *control, gint offset_x, gint offset_y)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    control->prelight_x = offset_x;
    control->prelight_y = offset_y;
}


void
gtk_control_set_insensitive_offsets (GtkControl *control, gint offset_x, gint offset_y)
{
    g_return_if_fail (GTK_IS_CONTROL (control));

    control->insensitive_x = offset_x;
    control->insensitive_y = offset_y;
}

/* I would like to know the best way to set a widget's background so that it
 * will use its parent's background AND be properly aligned with the parent
 * background.
 *
 * Using gdk_window_set_back_pixmap(widget->window, NULL, GDK_PARENT_RELATIVE)
 * works on my GTK+ 2.16.5 system, and on a 2.10.13 system I tested. However,
 * on a 2.6.10 system, the background pixmaps are not properly aligned (each
 * child widget uses the parent's background pixmap, but does so starting from
 * its origin, rather than using a `parent relative' offset into the pixmap
 * based on the child's (x,y) position. Note also that this may be the result
 * of xlib bugs rather than GTK+ bugs.)
 *
 * It is possible to create subpixmaps for each widget with the proper offset
 * into the parent's background pixmap. This causes the correct appearance on
 * the GTK+ 2.6.10 system, but it's a pain in the a**.
 *
 * While gdk_window_set_back_pixmap() appears sufficient (on my non-themed
 * systems), I have seen two other applications which set the background
 * pixmap in the widget's style. With my lack of knowledge of GTK+ theming,
 * it seems like it might be prudent to use this technique. Defining
 * GTK_CONTROL_SET_BG_IN_STYLE will cause that to happen.
 *
 * Update: On my ubuntu lucid system, GTK_CONTROL_SET_BG_IN_STYLE is needed
 * in order for the background to survive a theme change; except for theme
 * changes, it seems to work well without (GTK+ 2.20.1, Clearlooks theme
 * engine).
 */
#ifdef GTK_CONTROL_SET_BG_IN_STYLE

static void
set_bg_pixmap(GtkStyle *style, gint state, GdkPixmap *pixmap)
{
    if (style->bg_pixmap[state] != NULL &&
        style->bg_pixmap[state] != (GdkPixmap *)GDK_PARENT_RELATIVE)
        g_object_unref(style->bg_pixmap[state]);

    style->bg_pixmap[state] = pixmap;

    if (pixmap != NULL &&
        pixmap != (GdkPixmap *)GDK_PARENT_RELATIVE)
        g_object_ref(pixmap);
}


void
gtk_control_set_widget_bg_parent_relative_in_style(GtkWidget *widget)
{
    GtkStyle *style;

    g_return_if_fail (GTK_IS_WIDGET (widget));
    g_return_if_fail (!GTK_WIDGET_NO_WINDOW (widget));

    /* make a copy of the style and set GDK_PARENT_RELATIVE in it */
    style = gtk_style_copy (widget->style);
    set_bg_pixmap(style, GTK_STATE_NORMAL,      (GdkPixmap *)GDK_PARENT_RELATIVE);
    set_bg_pixmap(style, GTK_STATE_ACTIVE,      (GdkPixmap *)GDK_PARENT_RELATIVE);
    set_bg_pixmap(style, GTK_STATE_PRELIGHT,    (GdkPixmap *)GDK_PARENT_RELATIVE);
    set_bg_pixmap(style, GTK_STATE_SELECTED,    (GdkPixmap *)GDK_PARENT_RELATIVE);
    set_bg_pixmap(style, GTK_STATE_INSENSITIVE, (GdkPixmap *)GDK_PARENT_RELATIVE);
    gtk_widget_set_style(widget, style);
    g_object_unref(style);
    
    /* update widget is realized, update its window */
    if (widget->window != NULL) {
        /* if background pixmaps are failing to be used, try commenting out this line: */
        gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
    }
}
#endif


static void
gtk_control_realize (GtkWidget *widget)
{
    GtkControl *control;
    GtkWidget *parent = gtk_widget_get_parent(widget);
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (GTK_IS_CONTROL (widget));
    g_return_if_fail (parent != NULL);
    g_return_if_fail (!GTK_WIDGET_NO_WINDOW (widget));

    control = GTK_CONTROL (widget);

    /* make sure parent window is realized, because we'll need a valid colormap below (-FIX- still needed?) */
    if (!GTK_WIDGET_REALIZED (parent))
        gtk_widget_realize(parent);

    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK |
                                GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
                                GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                                GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
                                GDK_SCROLL_MASK;

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, control);

    widget->style = gtk_style_attach (widget->style, widget->window);

#ifdef GTK_CONTROL_SET_BG_IN_STYLE

    gtk_control_set_widget_bg_parent_relative_in_style(widget);

#else /* !GTK_CONTROL_SET_BG_IN_STYLE */

    gdk_window_set_back_pixmap(widget->window, NULL, GDK_PARENT_RELATIVE);

#endif /* !GTK_CONTROL_SET_BG_IN_STYLE */
}


static void
gtk_control_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
    GtkControl *control = GTK_CONTROL (widget);

    requisition->width  = control->width;
    requisition->height = control->height;
}


static void
gtk_control_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
    g_return_if_fail (GTK_IS_CONTROL (widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;
 
    if (GTK_WIDGET_REALIZED (widget)) {
        gdk_window_move_resize (widget->window,
                                allocation->x, allocation->y,
                                allocation->width, allocation->height);
    }
}


static gint
gtk_control_button_press (GtkWidget *widget, GdkEventButton *event)
{
    GtkControl *control;
    gfloat value;

    g_return_val_if_fail(GTK_IS_CONTROL(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    control = GTK_CONTROL(widget);

    if (control->state != STATE_IDLE)
        return FALSE;

    switch (event->button) {
      case 1:
      case 2:
      case 3:
        gtk_grab_add(widget);
        control->state = STATE_PRESSED;
        switch (control->type) {

          case GTK_CONTROL_BUTTON:
            control->adjustment->value = control->adjustment->upper;
            g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
            break;

          case GTK_CONTROL_SWITCH:
            value = (control->adjustment->lower + control->adjustment->upper) / 2.0f;
            if (control->adjustment->value < value)
                value = control->adjustment->upper;
            else
                value = control->adjustment->lower;
            if (control->adjustment->value != value) {
                control->adjustment->value = value;
                g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
            }
            break;

          case GTK_CONTROL_RADIO:
            value = (gfloat)control->radio_value;
            if (control->adjustment->value != value) {
                control->adjustment->value = value;
                g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
            }
            break;

          default:  /* knob or slider */
            control->saved_x = event->x;
            control->saved_y = event->y;
            control->saved_value = control->adjustment->value;
            break;
        }
        break;

      default:
        break;
    }

    return FALSE;
}


static gint
gtk_control_button_release (GtkWidget *widget, GdkEventButton *event)
{
    GtkControl *control;
    gfloat value;

    g_return_val_if_fail(GTK_IS_CONTROL(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    control = GTK_CONTROL(widget);

    if (control->state == STATE_IDLE)
        return FALSE;

    gtk_grab_remove(widget);
    control->state = STATE_IDLE;

    switch (control->type) {

      case GTK_CONTROL_BUTTON:
        control->adjustment->value = control->adjustment->lower;
        g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
        break;

      case GTK_CONTROL_SWITCH:
      case GTK_CONTROL_RADIO:
        /* do nothing */
        break;

      default:  /* knob or slider */
        switch (control->state) {

          case STATE_PRESSED:
            switch (event->button) {
              case 1:
                value = control->adjustment->value - control->adjustment->page_increment;
                if (control->integral)
                    value = roundf(value);
                value = MAX(MIN(value, control->adjustment->upper),
                            control->adjustment->lower);
                if (control->adjustment->value != value) {
                    control->adjustment->value = value;
                    g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
                }
                break;

              case 2:
              case 3:
                value = control->adjustment->value + control->adjustment->page_increment;
                if (control->integral)
                    value = roundf(value);
                value = MAX(MIN(value, control->adjustment->upper),
                            control->adjustment->lower);
                if (control->adjustment->value != value) {
                    control->adjustment->value = value;
                    g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
                }
                break;

              default:
                break;
            }
            break;

          case STATE_DRAGGING:
            if (control->policy != GTK_UPDATE_CONTINUOUS && control->old_value != control->adjustment->value)
                g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");

            break;

          default:
            break;
        }
        break;
    }

    return FALSE;
}


static gint
gtk_control_motion_notify (GtkWidget *widget, GdkEventMotion *event)
{
    GtkControl *control;
    GdkModifierType mods;
    gint x, y;

    g_return_val_if_fail(GTK_IS_CONTROL(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    control = GTK_CONTROL(widget);

    if (control->type == GTK_CONTROL_BUTTON ||
        control->type == GTK_CONTROL_SWITCH ||
        control->type == GTK_CONTROL_RADIO)
        return FALSE; /* don't care, don't use STATE_DRAGGING */

    x = event->x;
    y = event->y;

    if (event->is_hint || (event->window != widget->window))
        gdk_window_get_pointer(widget->window, &x, &y, &mods);

    switch (control->state) {
      case STATE_PRESSED:
        control->state = STATE_DRAGGING;
        /* fall through */

      case STATE_DRAGGING:
        if (mods & GDK_BUTTON1_MASK) {
            gtk_control_update_mouse(control, x, y, TRUE);
            return TRUE;
        } else if ((mods & GDK_BUTTON2_MASK) ||
                   (mods & GDK_BUTTON3_MASK)) {
            gtk_control_update_mouse(control, x, y, FALSE);
            return TRUE;
        }
        break;

      default:
        break;
    }

    return FALSE;
}


static gboolean
gtk_control_scroll (GtkWidget *widget, GdkEventScroll *event)
{
    GtkControl *control;
    int up;
    gfloat value;

    g_return_val_if_fail(GTK_IS_CONTROL(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    control = GTK_CONTROL(widget);

    if (control->type == GTK_CONTROL_BUTTON || control->type == GTK_CONTROL_RADIO)
        return FALSE;

    up = (event->direction == GDK_SCROLL_UP);
    if (control->type == GTK_CONTROL_DRAWBAR)
        up = !up;

    if (control->type == GTK_CONTROL_SWITCH) {
        if (up) {
            value = control->adjustment->upper;
        } else {
            value = control->adjustment->lower;
        }
    } else {
        if (up) {
            value = control->adjustment->value + control->adjustment->page_increment;
        } else {
            value = control->adjustment->value - control->adjustment->page_increment;
        }
        if (control->integral)
            value = roundf(value);
        value = MAX(MIN(value, control->adjustment->upper),
                    control->adjustment->lower);
    }
    if (control->adjustment->value != value) {
        control->adjustment->value = value;
        g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
    }

    return FALSE;
}


static gint
gtk_control_timer_callback (GtkControl *control)
{
    g_return_val_if_fail(GTK_IS_CONTROL(control), FALSE);

    if (control->policy == GTK_UPDATE_DELAYED)
        g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");

    return FALSE;       /* don't keep running this timer */
}


static void
gtk_control_update_mouse(GtkControl *control, gint x, gint y, gboolean first_button)
{
    /* this only gets called for knobs and sliders, not for button, switch, or radio types */

    gfloat old_value, new_value;
    gdouble angle;
    gfloat dv, dh;

    g_return_if_fail(GTK_IS_CONTROL(control));

    old_value = control->adjustment->value;

    switch (control->type) {

      case GTK_CONTROL_ROTARY:  /* rotary 'knob', 270 degree rotation, minimum at 1.25 pi */

        dv = (control->height / 2.0) - y; /* inverted cartesian graphics coordinate system */
        dh = x - (control->width / 2.0);

        if (control->prefer_radial) first_button = !first_button;

        if (first_button) {

            float hyp;
            float coarse_delta = (control->adjustment->upper -
                                  control->adjustment->lower) / 300.0f;

            dv = control->saved_y - y;
            dh = x - control->saved_x;
            hyp = sqrtf(dv * dv + dh * dh);
            if (control->integral && coarse_delta < 0.05f) coarse_delta = 0.05f;
            new_value = control->saved_value +
                      /* abs(dv)  * sin(angle) * coarse_delta */
                        fabsf(dv) * (dv / hyp) * coarse_delta +
                      /* abs(dh)  * cos(angle) * fine_delta */
                        fabsf(dh) * (dh / hyp) * control->adjustment->step_increment;

        } else {

            angle = atan2(dv, dh);
            angle /= M_PI;
            if (angle < -0.5)
                angle += 2;

            new_value = -(2.0/3.0) * (angle - 1.25);   /* map [1.25pi, -0.25pi] onto [0, 1] */
            new_value *= control->adjustment->upper - control->adjustment->lower;
            new_value += control->adjustment->lower;

        }
        break;

      case GTK_CONTROL_HORIZONTAL:  /* horizontal slider, minimum to the left */

        dh = x - control->saved_x;

        if (!first_button) {  /* second button is fine adjust */
            dh /= 800.0f;
        } else if (abs(control->stride_x) == 1) {  /* one-to-one with image */
            dh /= (float)control->range;
        } else {          /* umm, I dunno */
            dh /= 200.0f;
        }
        new_value = dh * (control->adjustment->upper -
                          control->adjustment->lower);
        new_value += control->saved_value;

        break;

      case GTK_CONTROL_VERTICAL:  /* vertical slider, minimum at the bottom */
      case GTK_CONTROL_DRAWBAR:   /* vertical slider, minimum at the top */

        dv = control->saved_y - y;
        if (control->type == GTK_CONTROL_DRAWBAR)
            dv = -dv;

        if (!first_button) {
            dv /= 800.0f;
        } else if (abs(control->stride_y) == 1) {
            dv /= (float)control->range;
        } else {
            dv /= 200.0f;
        }
        new_value = dv * (control->adjustment->upper -
                          control->adjustment->lower);
        new_value += control->saved_value;

        break;

      default:
        return; /* do nothing */
    }

    if (control->integral)
        new_value = roundf(new_value);
    new_value = MAX(MIN(new_value, control->adjustment->upper),
                    control->adjustment->lower);

    control->adjustment->value = new_value;

    if (control->adjustment->value != old_value) {

        if (control->policy == GTK_UPDATE_CONTINUOUS)
            g_signal_emit_by_name(G_OBJECT(control->adjustment), "value-changed");
        else {
            gtk_widget_queue_draw (GTK_WIDGET(control));

            if (control->policy == GTK_UPDATE_DELAYED) {
                if (control->timer)
                    g_source_remove(control->timer);

                control->timer = g_timeout_add (UPDATE_DELAY_LENGTH,
                                                (GSourceFunc) gtk_control_timer_callback,
                                                (gpointer) control);
          }
        }
    }
}


static gint
gtk_control_enter_notify (GtkWidget * widget, GdkEventCrossing * event)
{
    GtkControl *control;
    GtkWidget *event_widget;

    control = GTK_CONTROL (widget);
    event_widget = gtk_get_event_widget ((GdkEvent*) event);

    if ((event_widget == widget) &&
        (event->detail != GDK_NOTIFY_INFERIOR)) {
        control->prelit = TRUE;
        gtk_widget_queue_draw (GTK_WIDGET(control));
    }

    return FALSE;
}


static gint
gtk_control_leave_notify (GtkWidget * widget, GdkEventCrossing * event)
{
    GtkControl *control;
    GtkWidget *event_widget;

    control = GTK_CONTROL (widget);
    event_widget = gtk_get_event_widget ((GdkEvent*) event);

    if ((event_widget == widget) &&
        (event->detail != GDK_NOTIFY_INFERIOR)) {
        control->prelit = FALSE;
        gtk_widget_queue_draw (GTK_WIDGET(control));
    }

    return FALSE;
}


static void
gtk_control_adjustment_changed(GtkAdjustment *adjustment, gpointer data)
{
    GtkControl *control;

    g_return_if_fail(adjustment != NULL);
    g_return_if_fail(data != NULL);

    control = GTK_CONTROL(data);

    control->old_value = adjustment->value;

    gtk_widget_queue_draw (GTK_WIDGET(control));
}

static void
gtk_control_adjustment_value_changed (GtkAdjustment *adjustment, gpointer data)
{
    GtkControl *control;

    g_return_if_fail(adjustment != NULL);
    g_return_if_fail(data != NULL);

    control = GTK_CONTROL(data);

    if (control->old_value != adjustment->value) {
        control->old_value = adjustment->value;

        gtk_widget_queue_draw (GTK_WIDGET(control));
    }
}


static gboolean
gtk_control_expose (GtkWidget *widget, GdkEventExpose *event)
{
    GtkControl *control;
    float value;
    gint xsrc, ysrc;

    g_return_val_if_fail(GTK_IS_CONTROL(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    control = GTK_CONTROL(widget);

    if (!GTK_WIDGET_IS_SENSITIVE (widget) &&
        (control->insensitive_x != 0 || control->insensitive_y != 0)) {

        xsrc = control->insensitive_x;
        ysrc = control->insensitive_y;

    } else {

        if (control->type == GTK_CONTROL_RADIO && control->range == 1) {
            if ((int)control->adjustment->value == control->radio_value)
                value = 1.0f;
            else
                value = 0.0f;
        } else {
            if (control->adjustment->upper - control->adjustment->lower == 0.0)
                value = 0.0f;
            else
                value = (control->adjustment->value - control->adjustment->lower) / 
                        (control->adjustment->upper - control->adjustment->lower);
            value = MIN(MAX(value, 0.0f), 1.0f);
            value = (float)control->range * value;
        }

        xsrc = control->origin_x + control->stride_x * (int)value;
        ysrc = control->origin_y + control->stride_y * (int)value;

        if (control->prelit) {
            xsrc += control->prelight_x;
            ysrc += control->prelight_y;
        }
    }

    gdk_draw_pixbuf(widget->window, NULL, control->pixbuf,
                    xsrc, ysrc,
                    0, 0,
                    MIN(control->width,  control->src_width  - xsrc),
                    MIN(control->height, control->src_height - ysrc),
                    GDK_RGB_DITHER_NORMAL, 0, 0);

    return TRUE;
}

