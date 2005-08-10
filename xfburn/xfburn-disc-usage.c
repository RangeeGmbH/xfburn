/*
 * Copyright (c) 2005 Jean-François Wauthy (pollux@xfce.org)
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
#include "xfburn-utils.h"

/* prototypes */
static void xfburn_disc_usage_class_init (XfburnDiscUsageClass *);
static void xfburn_disc_usage_init (XfburnDiscUsage *);

static void cb_combo_changed (GtkComboBox *, XfburnDiscUsage *);

/* globals */
static GtkHBoxClass *parent_class = NULL;

#define DEFAULT_DISK_SIZE_LABEL 2
struct
{
  gdouble size;
  gchar *label;
} datadisksizes[] = {
  {
  200 *1000 * 1000, "200MB CD"},
  {
  600 *1000 * 1000, "600MB CD"},
  {
  700 *1000 * 1000, "700MB CD"},
  {
  4.7 *1000 * 1000 * 1000, "4.7GB DVD"},
  {
8.5 *1000 * 1000 * 1000, "8.5GB DVD"},};

/***************************/
/* XfburnDiscContent class */
/***************************/
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
      (GInstanceInitFunc) xfburn_disc_usage_init
    };

    disc_usage_type = g_type_register_static (GTK_TYPE_HBOX, "XfburnDiscUsage", &disc_usage_info, 0);
  }

  return disc_usage_type;
}

static void
xfburn_disc_usage_class_init (XfburnDiscUsageClass * klass)
{
  parent_class = g_type_class_peek_parent (klass);
}

static void
xfburn_disc_usage_init (XfburnDiscUsage * disc_usage)
{
  int i;

  disc_usage->size = 0;

  disc_usage->progress_bar = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (disc_usage->progress_bar), "0 B");
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->progress_bar, TRUE, TRUE, BORDER);
  gtk_widget_show (disc_usage->progress_bar);

  disc_usage->combo = gtk_combo_box_new_text ();
  for (i = 0; i < G_N_ELEMENTS (datadisksizes); i++)
    gtk_combo_box_append_text (GTK_COMBO_BOX (disc_usage->combo), datadisksizes[i].label);
  gtk_combo_box_set_active (GTK_COMBO_BOX (disc_usage->combo), DEFAULT_DISK_SIZE_LABEL);
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->combo, FALSE, FALSE, BORDER);
  gtk_widget_show (disc_usage->combo);

  disc_usage->button = xfce_create_mixed_button (GTK_STOCK_CDROM, _("Burn Data CD"));
  gtk_box_pack_start (GTK_BOX (disc_usage), disc_usage->button, FALSE, FALSE, BORDER);
  gtk_widget_set_sensitive (disc_usage->button, FALSE);
  gtk_widget_show (disc_usage->button);

  g_signal_connect (G_OBJECT (disc_usage->combo), "changed", G_CALLBACK (cb_combo_changed), disc_usage);
}

/* internals */
static void
xfburn_disc_usage_update_size (XfburnDiscUsage * disc_usage)
{
  gfloat fraction;
  gchar *human_size;

  fraction = disc_usage->size / datadisksizes[gtk_combo_box_get_active (GTK_COMBO_BOX (disc_usage->combo))].size;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (disc_usage->progress_bar), fraction > 1.0 ? 1.0 : fraction);

  human_size = xfburn_humanreadable_filesize ((guint64) disc_usage->size);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (disc_usage->progress_bar), human_size);

  g_free (human_size);
}

static void
cb_combo_changed (GtkComboBox * combo, XfburnDiscUsage * usage)
{
  xfburn_disc_usage_update_size (usage);
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
  xfburn_disc_usage_update_size (disc_usage);
}

void
xfburn_disc_usage_add_size (XfburnDiscUsage * disc_usage, gdouble size)
{
  disc_usage->size = disc_usage->size + size;
  xfburn_disc_usage_update_size (disc_usage);
}

GtkWidget *
xfburn_disc_usage_new (void)
{
  return g_object_new (xfburn_disc_usage_get_type (), NULL);
}