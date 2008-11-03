/* $Id$ */
/*
 * Copyright (c) 2005-2006 Jean-François Wauthy (pollux@xfce.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif /* !HAVE_CONFIG_H */

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>

#include "xfburn-disc-usage.h"
#include "xfburn-global.h"
#include "xfburn-settings.h"
#include "xfburn-utils.h"
#include "xfburn-stock.h"
#include "xfburn-main-window.h"


/* prototypes */
static void xfburn_disc_usage_class_init (XfburnDiscUsageClass *);
static void xfburn_disc_usage_init (XfburnDiscUsage *);

static void update_size_default (XfburnDiscUsage *du);
static gboolean can_burn_default (XfburnDiscUsage *du);
static void cb_button_clicked (GtkButton *, XfburnDiscUsage *);
static void cb_combo_changed (GtkComboBox *, XfburnDiscUsage *);

/* globals */
static GtkHBoxClass *parent_class = NULL;

#define DEFAULT_DISK_SIZE_LABEL 2
#define LAST_CD_LABEL 4

XfburnDiscLabels testdiscsizes[] = {
  {
  1, "200MB CD"},
  {
  100, "650MB CD"},
  {
  10000, "700MB CD"},
  {
  1000000, "800MB CD"},
  {
  100000000, "900MB CD"},
  {
  G_GINT64_CONSTANT (10000000000), "4.3GB DVD"},
  {
  G_GINT64_CONSTANT (1000000000000), "7.9GB DVD"},
};

/* signals */
enum
{
  BEGIN_BURN,
  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL];

/*******************************/
/* XfburnDiscUsage class */
/*******************************/
static guint signals[LAST_SIGNAL];

GtkType
xfburn_disc_usage_get_type (void)
{
  static GtkType disc_usage_type = 0;

  if (!disc_usage_type) {
    static const GTypeInfo disc_usage_info = {
      sizeof (XfburnDiscUsageClass),
      NULL,
      NULL,
      (GClassInitFunc) xfburn_disc_usage_class_init,
      NULL,
      NULL,
      sizeof (XfburnDiscUsage),
      0,
      (GInstanceInitFunc) xfburn_disc_usage_init,
      NULL
    };

    disc_usage_type = g_type_register_static (GTK_TYPE_HBOX, "XfburnDiscUsage", &disc_usage_info, 0);
  }

  return disc_usage_type;
}

static void
xfburn_disc_usage_class_init (XfburnDiscUsageClass * klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);

  signals[BEGIN_BURN] = g_signal_new ("begin-burn", G_TYPE_FROM_CLASS (gobject_class), G_SIGNAL_ACTION,
                                      G_STRUCT_OFFSET (XfburnDiscUsageClass, begin_burn),
                                      NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  /* install default values */
  klass->update_size = update_size_default;
  klass->can_burn = can_burn_default;
  klass->labels = testdiscsizes;
  klass->num_labels = G_N_ELEMENTS (testdiscsizes);
}

static void
xfburn_disc_usage_init (XfburnDiscUsage * disc_usage)
{
  XfburnDiscUsageClass *class = XFBURN_DISC_USAGE_GET_CLASS (disc_usage);
  int i;

  disc_usage->size = 0;

  disc_usage->progress_bar = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (disc_usage->progress_bar), "0");
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->progress_bar, TRUE, TRUE, BORDER);
  gtk_widget_show (disc_usage->progress_bar);

  disc_usage->combo = gtk_combo_box_new_text ();
  for (i = 0; i < class->num_labels; i++) {
    gtk_combo_box_append_text (GTK_COMBO_BOX (disc_usage->combo), class->labels[i].label);
  }
  gtk_combo_box_set_active (GTK_COMBO_BOX (disc_usage->combo), DEFAULT_DISK_SIZE_LABEL);
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->combo, FALSE, FALSE, BORDER);
  gtk_widget_show (disc_usage->combo);

  disc_usage->button = xfce_create_mixed_button (XFBURN_STOCK_BURN_CD, _("Proceed to Burn"));
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->button, FALSE, FALSE, BORDER);
  gtk_widget_set_sensitive (disc_usage->button, FALSE);
  gtk_widget_show (disc_usage->button);
  g_signal_connect (G_OBJECT (disc_usage->button), "clicked", G_CALLBACK (cb_button_clicked), disc_usage);
  
  g_signal_connect (G_OBJECT (disc_usage->combo), "changed", G_CALLBACK (cb_combo_changed), disc_usage);

}

/* internals */
static void 
update_size_default (XfburnDiscUsage *du)
{
  gchar *size;

  size = g_strdup_printf ("%.0lf", du->size);

  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (du->progress_bar), size);

  g_free (size);
}

static gboolean 
can_burn_default (XfburnDiscUsage *du)
{
  return TRUE;
}

static void 
cb_button_clicked (GtkButton *button, XfburnDiscUsage *du)
{
  XfburnDiscUsageClass *class = XFBURN_DISC_USAGE_GET_CLASS (du);

  if (du->size <= class->labels[gtk_combo_box_get_active (GTK_COMBO_BOX (du->combo))].size) {
    g_signal_emit (G_OBJECT (du), signals[BEGIN_BURN], 0);
  } else {
    xfce_err (_("You are trying to burn more data than the disc can contain !"));
  }
}

static void
update_size (XfburnDiscUsage * disc_usage, gboolean manual)
{
  XfburnDiscUsageClass *class = XFBURN_DISC_USAGE_GET_CLASS (disc_usage);
  int i;

  class->update_size (disc_usage);

  if (!manual) {
    i = 0;
    while (i < class->num_labels  &&  disc_usage->size > class->labels[i].size) {
      i++;
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (disc_usage->combo), (i<class->num_labels ? i: i-1));
  }

  gtk_widget_set_sensitive (disc_usage->button, class->can_burn (disc_usage));
}

static void
cb_combo_changed (GtkComboBox * combo, XfburnDiscUsage * usage)
{
  update_size (usage, TRUE);
}

/* public methods */
gdouble
xfburn_disc_usage_get_size (XfburnDiscUsage * disc_usage)
{
  return disc_usage->size;
}

void
xfburn_disc_usage_set_size (XfburnDiscUsage * disc_usage, gdouble size)
{
  disc_usage->size = size;
  update_size (disc_usage, FALSE);
}

void
xfburn_disc_usage_add_size (XfburnDiscUsage * disc_usage, gdouble size)
{
  disc_usage->size = disc_usage->size + size;
  update_size (disc_usage, FALSE);
}

void
xfburn_disc_usage_sub_size (XfburnDiscUsage * disc_usage, gdouble size)
{
  disc_usage->size = disc_usage->size - size;
  update_size (disc_usage, FALSE);
}

XfburnDiscType
xfburn_disc_usage_get_disc_type (XfburnDiscUsage * disc_usage)
{
  if (gtk_combo_box_get_active (GTK_COMBO_BOX (disc_usage->combo)) > LAST_CD_LABEL)
    return DVD_DISC;
  else
    return CD_DISC;
}

GtkWidget *
xfburn_disc_usage_new (void)
{
  return g_object_new (xfburn_disc_usage_get_type (), NULL);
}
