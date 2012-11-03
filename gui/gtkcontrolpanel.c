/* gtkcontrolpanel.c - don't know what it is, but it fits on here
 *
 * Copyright (C) 2009 Sean Bolton
 *
 * This version of gtkcontrolpanel.c contains no subpixmap code,
 * which means it needs about GTK+ version 2.10 or later to
 * function correctly.
 *
 * Parts of this code come from GTK+, both the library source and
 * the example programs (much of it is from gtkfixed.c).
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "why/gtkcontrolpanel.h"

#ifndef GTK_CONTROL_SET_BG_IN_STYLE
#define GTK_CONTROL_SET_BG_IN_STYLE
#endif

enum {
  CHILD_PROP_0,
  CHILD_PROP_X,
  CHILD_PROP_Y
};

static void gtk_control_panel_destroy       (GtkObject *object);
static void gtk_control_panel_realize       (GtkWidget        *widget);
static void gtk_control_panel_size_request  (GtkWidget        *widget,
                                                  GtkRequisition   *requisition);
static void gtk_control_panel_size_allocate (GtkWidget        *widget,
                                                  GtkAllocation    *allocation);
static void gtk_control_panel_add           (GtkContainer     *container,
                                                  GtkWidget        *widget);
static void gtk_control_panel_remove        (GtkContainer     *container,
                                                  GtkWidget        *widget);
static void gtk_control_panel_forall        (GtkContainer     *container,
                                                  gboolean          include_internals,
                                                  GtkCallback       callback,
                                                  gpointer          callback_data);
static GType gtk_control_panel_child_type   (GtkContainer     *container);

static void gtk_control_panel_set_child_property (GtkContainer *container,
                                                       GtkWidget    *child,
                                                       guint         property_id,
                                                       const GValue *value,
                                                       GParamSpec   *pspec);
static void gtk_control_panel_get_child_property (GtkContainer *container,
                                                       GtkWidget    *child,
                                                       guint         property_id,
                                                       GValue       *value,
                                                       GParamSpec   *pspec);

G_DEFINE_TYPE (GtkControlPanel, gtk_control_panel, GTK_TYPE_CONTAINER)

static void
gtk_control_panel_class_init (GtkControlPanelClass *class)
{
  GtkObjectClass *object_class = (GtkObjectClass*) class;
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (class);

  object_class->destroy = gtk_control_panel_destroy;

  widget_class->realize = gtk_control_panel_realize;
  widget_class->size_request = gtk_control_panel_size_request;
  widget_class->size_allocate = gtk_control_panel_size_allocate;

  container_class->add = gtk_control_panel_add;
  container_class->remove = gtk_control_panel_remove;
  container_class->forall = gtk_control_panel_forall;
  container_class->child_type = gtk_control_panel_child_type;

  container_class->set_child_property = gtk_control_panel_set_child_property;
  container_class->get_child_property = gtk_control_panel_get_child_property;

  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_X,
					      g_param_spec_int ("x",
                                                                "X position",
                                                                "X position of child widget",
                                                                G_MININT,
                                                                G_MAXINT,
                                                                0,
                                                                G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB));

  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_Y,
					      g_param_spec_int ("y",
                                                                "Y position",
                                                                "Y position of child widget",
                                                                G_MININT,
                                                                G_MAXINT,
                                                                0,
                                                                G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB));
}

static GType
gtk_control_panel_child_type (GtkContainer     *container)
{
  return GTK_TYPE_WIDGET;
}

static void
gtk_control_panel_init (GtkControlPanel *cp)
{
    cp->pixbuf = NULL;
    cp->pixmap = NULL;
    cp->fixed_width = FALSE;
    cp->fixed_height = FALSE;
    cp->children = NULL;
}

GtkWidget*
gtk_control_panel_new (GdkPixbuf *pixbuf,
                       gint width,
                       gint height)
{
    GtkWidget *widget;
    GtkControlPanel *cp;

    g_return_val_if_fail (pixbuf != NULL, NULL);

    widget = g_object_new (GTK_TYPE_CONTROL_PANEL, NULL);
    cp = GTK_CONTROL_PANEL(widget);

    g_object_ref(pixbuf);
    cp->pixbuf = pixbuf;
    cp->fixed_width  = (width  >= 0);
    cp->fixed_height = (height >= 0);
    if (width == 0)
        width = gdk_pixbuf_get_width(pixbuf);
    if (height == 0)
        height = gdk_pixbuf_get_height(pixbuf);
    cp->width  = width;
    cp->height = height;

    return widget;
}

static void
gtk_control_panel_destroy (GtkObject *object)
{
    GtkControlPanel *cp = GTK_CONTROL_PANEL (object);

    if (cp->pixbuf) {
        g_object_ref(cp->pixbuf);
        cp->pixbuf = NULL;
    }
    if (cp->pixmap) {
        g_object_ref(cp->pixmap);
        cp->pixmap = NULL;
    }

    GTK_OBJECT_CLASS (gtk_control_panel_parent_class)->destroy (object);
}

static GtkControlPanelChild*
get_child (GtkControlPanel  *cp,
           GtkWidget *widget)
{
  GList *children;
  
  children = cp->children;
  while (children)
    {
      GtkControlPanelChild *child;
      
      child = children->data;
      children = children->next;

      if (child->widget == widget)
        return child;
    }

  return NULL;
}

void
gtk_control_panel_put (GtkControlPanel       *cp,
                       GtkWidget      *widget,
                       gint            x,
                       gint            y)
{
  GtkControlPanelChild *child_info;

  g_return_if_fail (GTK_IS_CONTROL_PANEL (cp));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  child_info = g_new (GtkControlPanelChild, 1);
  child_info->widget = widget;
  child_info->x = x;
  child_info->y = y;

  /* -FIX- note: this will force realization of widget if cp is realized */
  gtk_widget_set_parent (widget, GTK_WIDGET (cp));

  cp->children = g_list_append (cp->children, child_info);
}

static void
gtk_control_panel_move_internal (GtkControlPanel       *cp,
                                 GtkWidget      *widget,
                                 gboolean        change_x,
                                 gint            x,
                                 gboolean        change_y,
                                 gint            y)
{
  GtkControlPanelChild *child;

  g_return_if_fail (GTK_IS_CONTROL_PANEL (cp));
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (widget->parent == GTK_WIDGET (cp));  
  
  child = get_child (cp, widget);

  g_assert (child);

  gtk_widget_freeze_child_notify (widget);
  
  if (change_x)
    {
      child->x = x;
      gtk_widget_child_notify (widget, "x");
    }

  if (change_y)
    {
      child->y = y;
      gtk_widget_child_notify (widget, "y");
    }

  gtk_widget_thaw_child_notify (widget);
  
  if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (cp))
    gtk_widget_queue_resize (GTK_WIDGET (cp));
}

void
gtk_control_panel_move (GtkControlPanel       *cp,
                        GtkWidget      *widget,
                        gint            x,
                        gint            y)
{
  gtk_control_panel_move_internal (cp, widget, TRUE, x, TRUE, y);
}

static void
gtk_control_panel_set_child_property (GtkContainer    *container,
                                      GtkWidget       *child,
                                      guint            property_id,
                                      const GValue    *value,
                                      GParamSpec      *pspec)
{
  switch (property_id)
    {
    case CHILD_PROP_X:
      gtk_control_panel_move_internal (GTK_CONTROL_PANEL (container),
                               child,
                               TRUE, g_value_get_int (value),
                               FALSE, 0);
      break;
    case CHILD_PROP_Y:
      gtk_control_panel_move_internal (GTK_CONTROL_PANEL (container),
                               child,
                               FALSE, 0,
                               TRUE, g_value_get_int (value));
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}

static void
gtk_control_panel_get_child_property (GtkContainer *container,
                                      GtkWidget    *child,
                                      guint         property_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
  GtkControlPanelChild *cp_child;

  cp_child = get_child (GTK_CONTROL_PANEL (container), child);
  
  switch (property_id)
    {
    case CHILD_PROP_X:
      g_value_set_int (value, cp_child->x);
      break;
    case CHILD_PROP_Y:
      g_value_set_int (value, cp_child->y);
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}


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
#endif

static void
gtk_control_panel_realize (GtkWidget *widget)
{
    GtkControlPanel *cp = GTK_CONTROL_PANEL(widget);
    GtkWidget *parent = gtk_widget_get_parent(widget);
    GdkWindowAttr attributes;
    gint attributes_mask;
    GdkColormap *colormap;
    GtkStyle *style;

    g_return_if_fail (parent != NULL);
    g_return_if_fail (!GTK_WIDGET_NO_WINDOW (widget));

    /* make sure parent window is realized, because we'll need a valid colormap below */
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
    attributes.event_mask = gtk_widget_get_events (widget);
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
    
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    
    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                     &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, widget);

    /* create a pixmap compatible with the window */
    colormap = gdk_drawable_get_colormap (widget->window);
    gdk_pixbuf_render_pixmap_and_mask_for_colormap(cp->pixbuf, colormap, &cp->pixmap,
                                                   NULL, 0);

    /* done with pixbuf */
    g_object_unref(cp->pixbuf);
    cp->pixbuf = NULL;

#ifdef GTK_CONTROL_SET_BG_IN_STYLE
    /* copy style and set the pixmap as its background */
    style = gtk_widget_get_style (widget);
    style = gtk_style_attach (style, widget->window);
    style = gtk_style_copy (style);
    set_bg_pixmap(style, GTK_STATE_NORMAL,      cp->pixmap);
    set_bg_pixmap(style, GTK_STATE_ACTIVE,      cp->pixmap);
    set_bg_pixmap(style, GTK_STATE_PRELIGHT,    cp->pixmap);
    set_bg_pixmap(style, GTK_STATE_SELECTED,    cp->pixmap);
    set_bg_pixmap(style, GTK_STATE_INSENSITIVE, cp->pixmap);
    gtk_widget_set_style(widget, style);
    g_object_unref(style);
    
    /* set our background for the window from this style */
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
#else /* !GTK_CONTROL_SET_BG_IN_STYLE */
    style = gtk_widget_get_style (widget);
    style = gtk_style_attach (style, widget->window);
    gtk_style_set_background (style, widget->window, GTK_STATE_NORMAL);

    gdk_window_set_back_pixmap(widget->window, cp->pixmap, FALSE);
    gtk_widget_set_app_paintable(widget, TRUE);
#endif /* !GTK_CONTROL_SET_BG_IN_STYLE */

    /* could unref pixmap now... */
}

static void
gtk_control_panel_size_request (GtkWidget      *widget,
             			GtkRequisition *requisition)
{
    GtkControlPanel *cp;
    gint width;
    gint height;
    GList *children;
    GtkControlPanelChild *child;
    GtkRequisition child_requisition;


    cp = GTK_CONTROL_PANEL (widget);

    width = 0;
    height = 0;
    children = cp->children;
    while (children)
      {
        child = children->data;
        children = children->next;

        if (GTK_WIDGET_VISIBLE (child->widget))
          {
            gtk_widget_size_request (child->widget, &child_requisition);

            width  = MAX (width,  child->x + child_requisition.width);
            height = MAX (height, child->y + child_requisition.height);
          }
      }
    if (cp->fixed_width)  width  = cp->width;
    if (cp->fixed_height) height = cp->height;

    /* -FIX- Hmm, is this going to mess up the alignment of the background? */
    /* Or does the border width stuff in size_allocate below compensate? */
    requisition->width  = width  + GTK_CONTAINER (cp)->border_width * 2;
    requisition->height = height + GTK_CONTAINER (cp)->border_width * 2;
}

static void
gtk_control_panel_size_allocate (GtkWidget     *widget,
                                 GtkAllocation *allocation)
{
    GtkControlPanel *cp;
    GtkControlPanelChild *child;
    GtkAllocation child_allocation;
    GtkRequisition child_requisition;
    GList *children;
    guint16 border_width;

    g_return_if_fail (GTK_IS_CONTROL_PANEL (widget));
    g_return_if_fail (allocation != NULL);
    g_return_if_fail (!GTK_WIDGET_NO_WINDOW (widget));

    cp = GTK_CONTROL_PANEL (widget);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
        gdk_window_move_resize (widget->window,
                                allocation->x, 
                                allocation->y,
                                allocation->width, 
                                allocation->height);

    border_width = GTK_CONTAINER (cp)->border_width;
   
    children = cp->children;
    while (children)
      {
        child = children->data;
        children = children->next;

        if (GTK_WIDGET_VISIBLE (child->widget))
        {
          gtk_widget_get_child_requisition (child->widget, &child_requisition);
          child_allocation.x = child->x + border_width;
          child_allocation.y = child->y + border_width;

          child_allocation.width = child_requisition.width;
          child_allocation.height = child_requisition.height;
          gtk_widget_size_allocate (child->widget, &child_allocation);
        }
      }
}

static void
gtk_control_panel_add (GtkContainer *container,
                       GtkWidget    *widget)
{
  gtk_control_panel_put (GTK_CONTROL_PANEL (container), widget, 0, 0);
}

static void
gtk_control_panel_remove (GtkContainer *container,
                          GtkWidget    *widget)
{
  GtkControlPanel *cp;
  GtkControlPanelChild *child;
  GList *children;

  cp = GTK_CONTROL_PANEL (container);

  children = cp->children;
  while (children)
    {
      child = children->data;

      if (child->widget == widget)
	{
	  gboolean was_visible = GTK_WIDGET_VISIBLE (widget);
	  
	  gtk_widget_unparent (widget);

	  cp->children = g_list_remove_link (cp->children, children);
	  g_list_free (children);
	  g_free (child);

	  if (was_visible && GTK_WIDGET_VISIBLE (container))
	    gtk_widget_queue_resize (GTK_WIDGET (container));

	  break;
	}

      children = children->next;
    }
}

static void
gtk_control_panel_forall (GtkContainer *container,
                          gboolean      include_internals,
                          GtkCallback   callback,
                          gpointer      callback_data)
{
  GtkControlPanel *cp = GTK_CONTROL_PANEL (container);
  GtkControlPanelChild *child;
  GList *children;

  children = cp->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      (* callback) (child->widget, callback_data);
    }
}

