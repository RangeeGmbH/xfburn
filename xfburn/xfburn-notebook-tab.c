/* $Id$ */
/*
 *  Copyright (c) 2005-2006 Jean-François Wauthy (pollux@xfce.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif /* !HAVE_CONFIG_H */

#include <gtk/gtk.h>

#include <libxfce4util/libxfce4util.h>

#include "xfburn-notebook-tab.h"

#define XFBURN_NOTEBOOK_TAB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), XFBURN_TYPE_NOTEBOOK_TAB, XfburnNotebookTabPrivate))

enum {
  BUTTON_CLOSE_CLICKED,
  LAST_SIGNAL,
};

enum {
  PROP_0,
  PROP_LABEL,
};

/* private members */
typedef struct
{
  GtkWidget *label;
  GtkWidget *button_close;
} XfburnNotebookTabPrivate;


/* prototypes */
static void xfburn_notebook_tab_class_init (XfburnNotebookTabClass * klass);
static void xfburn_notebook_tab_init (XfburnNotebookTab * tab);
static void xfburn_notebook_tab_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void xfburn_notebook_tab_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);

static void cb_composition_close (GtkButton *button, XfburnNotebookTab *tab);

/* static member */
static GtkHBoxClass *parent_class = NULL;
static guint notebook_tab_signals[LAST_SIGNAL];

/************************/
/* class initiliazation */
/************************/
GtkType
xfburn_notebook_tab_get_type ()
{
  static GtkType type = 0;

  if (type == 0) {
    static const GTypeInfo our_info = {
      sizeof (XfburnNotebookTabClass),
      NULL,
      NULL,
      (GClassInitFunc) xfburn_notebook_tab_class_init,
      NULL,
      NULL,
      sizeof (XfburnNotebookTab),
      0,
      (GInstanceInitFunc) xfburn_notebook_tab_init,
    };

    type = g_type_register_static (GTK_TYPE_HBOX, "XfburnNotebookTab", &our_info, 0);
  }

  return type;
}

static void
xfburn_notebook_tab_class_init (XfburnNotebookTabClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (XfburnNotebookTabPrivate));

  parent_class = g_type_class_peek_parent (klass);
  
  object_class->set_property = xfburn_notebook_tab_set_property;
  object_class->get_property = xfburn_notebook_tab_get_property;
  
  notebook_tab_signals[BUTTON_CLOSE_CLICKED] = g_signal_new ("button-close-clicked",
                                                             G_TYPE_FROM_CLASS (object_class), G_SIGNAL_ACTION,
                                                             G_STRUCT_OFFSET (XfburnNotebookTabClass, button_close_clicked),
                                                             NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
  
  g_object_class_install_property (object_class, PROP_LABEL, 
                                   g_param_spec_string ("label", _("Label"), _("The text of the label"), 
                                                        NULL, G_PARAM_READWRITE));
}

static void
xfburn_notebook_tab_init (XfburnNotebookTab * tab)
{
  XfburnNotebookTabPrivate *priv = XFBURN_NOTEBOOK_TAB_GET_PRIVATE (tab);
  GtkBox *hbox = GTK_BOX (tab);
  GtkWidget *img;
  
  priv->label = gtk_label_new ("");
  gtk_widget_show (priv->label);
  gtk_box_pack_start (hbox, priv->label, TRUE, TRUE, 0);
    
  priv->button_close = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (priv->button_close), GTK_RELIEF_NONE);
  gtk_widget_show (priv->button_close);
	
  img = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
  gtk_widget_show (img);
  gtk_container_add (GTK_CONTAINER (priv->button_close), img);
  gtk_box_pack_start (hbox, priv->button_close, FALSE, FALSE, 0);

  g_signal_connect (G_OBJECT (priv->button_close), "clicked", G_CALLBACK (cb_composition_close), tab);
}

static void
xfburn_notebook_tab_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  XfburnNotebookTabPrivate *priv = XFBURN_NOTEBOOK_TAB_GET_PRIVATE (object);

  switch (prop_id) {
    case PROP_LABEL:
      g_value_set_string (value, gtk_label_get_text (GTK_LABEL (priv->label)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
xfburn_notebook_tab_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  XfburnNotebookTabPrivate *priv = XFBURN_NOTEBOOK_TAB_GET_PRIVATE (object);
  
  switch (prop_id) {
    case PROP_LABEL:
      gtk_label_set_text (GTK_LABEL (priv->label), g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
                                          
/*************/
/* internals */
/*************/
static void
cb_composition_close (GtkButton *button, XfburnNotebookTab *tab)
{
  g_signal_emit (G_OBJECT (tab), notebook_tab_signals[BUTTON_CLOSE_CLICKED], 0);
}

/**********/
/* public */
/**********/
GtkWidget *
xfburn_notebook_tab_new (const gchar *label)
{
  GtkWidget *obj;

  obj = GTK_WIDGET (g_object_new (XFBURN_TYPE_NOTEBOOK_TAB, "homogeneous", FALSE, "label", label, NULL));

  return obj;
}

void
xfburn_notebook_tab_set_label (XfburnNotebookTab *tab, const gchar *label)
{
  XfburnNotebookTabPrivate *priv = XFBURN_NOTEBOOK_TAB_GET_PRIVATE (tab);
  
  gtk_label_set_text (GTK_LABEL (priv->label), label);
}