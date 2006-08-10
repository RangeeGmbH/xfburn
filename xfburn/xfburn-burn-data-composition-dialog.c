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

#include "xfburn-global.h"
#include "xfburn-utils.h"
#include "xfburn-settings.h"
#include "xfburn-burn-data-composition-progress-dialog.h"
#include "xfburn-create-iso-from-composition-progress-dialog.h"
#include "xfburn-device-box.h"
#include "xfburn-write-mode-combo-box.h"

#include "xfburn-burn-data-composition-dialog.h"

/* prototypes */
static void xfburn_burn_data_composition_dialog_class_init (XfburnBurnDataCompositionDialogClass * klass);
static void xfburn_burn_data_composition_dialog_init (XfburnBurnDataCompositionDialog * obj);
static void xfburn_burn_data_composition_dialog_finalize (GObject * object);

static void cb_check_only_iso_toggled (GtkToggleButton * button, XfburnBurnDataCompositionDialog * dialog);
static void cb_browse_iso (GtkButton * button, XfburnBurnDataCompositionDialog * dialog);
static void cb_dialog_response (XfburnBurnDataCompositionDialog * dialog, gint response_id,
                                XfburnBurnDataCompositionDialogPrivate * priv);

/* structures */
struct XfburnBurnDataCompositionDialogPrivate
{
  gchar *command_iso;
  gchar *command_burn;

  GtkWidget *frame_device;
  GtkWidget *device_box;
  GtkWidget *combo_mode;

  GtkWidget *check_eject;
  GtkWidget *check_burnfree;
  GtkWidget *check_only_iso;
  GtkWidget *hbox_iso;
  GtkWidget *entry_path_iso;
  GtkWidget *check_dummy;

  gchar *file_list;
  XfburnDataComposition *composition;
};

/* globals */
static XfceTitledDialogClass *parent_class = NULL;

GtkType
xfburn_burn_data_composition_dialog_get_type ()
{
  static GtkType type = 0;

  if (type == 0) {
    static const GTypeInfo our_info = {
      sizeof (XfburnBurnDataCompositionDialogClass),
      NULL,
      NULL,
      (GClassInitFunc) xfburn_burn_data_composition_dialog_class_init,
      NULL,
      NULL,
      sizeof (XfburnBurnDataCompositionDialog),
      0,
      (GInstanceInitFunc) xfburn_burn_data_composition_dialog_init,
    };

    type = g_type_register_static (XFCE_TYPE_TITLED_DIALOG, "XfburnBurnDataCompositionDialog", &our_info, 0);
  }

  return type;
}

static void
xfburn_burn_data_composition_dialog_class_init (XfburnBurnDataCompositionDialogClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = xfburn_burn_data_composition_dialog_finalize;

}

static void
xfburn_burn_data_composition_dialog_init (XfburnBurnDataCompositionDialog * obj)
{
  XfburnBurnDataCompositionDialogPrivate *priv;
  GtkBox *box = GTK_BOX (GTK_DIALOG (obj)->vbox);
  GtkWidget *img;
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *align;
  GtkWidget *label;
  GtkWidget *button;
  gchar *default_path;
  gchar *tmp_dir;

  obj->priv = g_new0 (XfburnBurnDataCompositionDialogPrivate, 1);
  priv = obj->priv;

  priv->command_iso = NULL;
  priv->command_burn = NULL;

  gtk_window_set_title (GTK_WINDOW (obj), _("Burn Composition"));
  gtk_window_set_destroy_with_parent (GTK_WINDOW (obj), TRUE);
  gtk_window_set_icon_name (GTK_WINDOW (obj), GTK_STOCK_CDROM);

  /* burning devices list */
  priv->device_box = xfburn_device_box_new (TRUE, TRUE);
  gtk_widget_show (priv->device_box);

  priv->frame_device = xfce_create_framebox_with_content (_("Burning device"), priv->device_box);
  gtk_widget_show (priv->frame_device);
  gtk_box_pack_start (box, priv->frame_device, FALSE, FALSE, BORDER);

  /* mode */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (priv->device_box), hbox, FALSE, FALSE, BORDER);

  label = gtk_label_new_with_mnemonic (_("Write _mode :"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, BORDER);

  priv->combo_mode = xfburn_write_mode_combo_box_new ();
  gtk_box_pack_start (GTK_BOX (hbox), priv->combo_mode, TRUE, TRUE, BORDER);
  gtk_widget_show (priv->combo_mode);

  /* options */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);

  frame = xfce_create_framebox_with_content (_("Options"), vbox);
  gtk_widget_show (frame);
  gtk_box_pack_start (box, frame, FALSE, FALSE, BORDER);

  priv->check_eject = gtk_check_button_new_with_mnemonic (_("E_ject disk"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->check_eject), TRUE);
  gtk_widget_show (priv->check_eject);
  gtk_box_pack_start (GTK_BOX (vbox), priv->check_eject, FALSE, FALSE, BORDER);

  priv->check_dummy = gtk_check_button_new_with_mnemonic (_("_Dummy write"));
  gtk_widget_show (priv->check_dummy);
  gtk_box_pack_start (GTK_BOX (vbox), priv->check_dummy, FALSE, FALSE, BORDER);

  priv->check_burnfree = gtk_check_button_new_with_mnemonic (_("Burn_Free"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->check_burnfree), TRUE);
  gtk_widget_show (priv->check_burnfree);
  gtk_box_pack_start (GTK_BOX (vbox), priv->check_burnfree, FALSE, FALSE, BORDER);

  /* create ISO ? */
  priv->check_only_iso = gtk_check_button_new_with_mnemonic (_("Only create _ISO"));
  gtk_widget_show (priv->check_only_iso);
  gtk_box_pack_start (GTK_BOX (vbox), priv->check_only_iso, FALSE, FALSE, BORDER);
  g_signal_connect (G_OBJECT (priv->check_only_iso), "toggled", G_CALLBACK (cb_check_only_iso_toggled), obj);

  align = gtk_alignment_new (0, 0, 0, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, BORDER * 4, 0);
  gtk_widget_show (align);
  gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

  priv->hbox_iso = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (priv->hbox_iso);
  gtk_container_add (GTK_CONTAINER (align), priv->hbox_iso);
  gtk_widget_set_sensitive (priv->hbox_iso, FALSE);

  priv->entry_path_iso = gtk_entry_new ();
  tmp_dir = xfburn_settings_get_string ("temporary-dir", g_get_tmp_dir ());
  default_path = g_build_filename (tmp_dir, "xfburn.iso", NULL);
  gtk_entry_set_text (GTK_ENTRY (priv->entry_path_iso), default_path);
  g_free (default_path);
  g_free (tmp_dir);
  gtk_widget_show (priv->entry_path_iso);
  gtk_box_pack_start (GTK_BOX (priv->hbox_iso), priv->entry_path_iso, FALSE, FALSE, 0);

  img = gtk_image_new_from_stock (GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (img);
  button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), img);
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (priv->hbox_iso), button, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_browse_iso), obj);

  /* action buttons */
  button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_widget_show (button);
  gtk_dialog_add_action_widget (GTK_DIALOG (obj), button, GTK_RESPONSE_CANCEL);

  button = xfce_create_mixed_button (GTK_STOCK_CDROM, _("_Burn Composition"));
  gtk_widget_show (button);
  gtk_dialog_add_action_widget (GTK_DIALOG (obj), button, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_focus (button);
  gtk_widget_grab_default (button);

  g_signal_connect (G_OBJECT (obj), "response", G_CALLBACK (cb_dialog_response), priv);
}

static void
xfburn_burn_data_composition_dialog_finalize (GObject * object)
{
  XfburnBurnDataCompositionDialog *cobj;
  cobj = XFBURN_BURN_COMPOSITION_DIALOG (object);

  g_free (cobj->priv->file_list);
  g_free (cobj->priv->command_iso);
  g_free (cobj->priv->command_burn);
  g_free (cobj->priv);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* internals */
static void
cb_check_only_iso_toggled (GtkToggleButton * button, XfburnBurnDataCompositionDialog * dialog)
{
  XfburnBurnDataCompositionDialogPrivate *priv = dialog->priv;

  gtk_widget_set_sensitive (priv->frame_device, !gtk_toggle_button_get_active (button));
  
  gtk_widget_set_sensitive (priv->hbox_iso, gtk_toggle_button_get_active (button));
  gtk_widget_set_sensitive (priv->check_eject, !gtk_toggle_button_get_active (button));
  gtk_widget_set_sensitive (priv->check_burnfree, !gtk_toggle_button_get_active (button));
  gtk_widget_set_sensitive (priv->check_dummy, !gtk_toggle_button_get_active (button));
}

static void
cb_browse_iso (GtkButton * button, XfburnBurnDataCompositionDialog * dialog)
{
  xfburn_browse_for_file (GTK_ENTRY (dialog->priv->entry_path_iso), GTK_WINDOW (dialog));
}

static void
cb_dialog_response (XfburnBurnDataCompositionDialog * dialog, gint response_id, XfburnBurnDataCompositionDialogPrivate * priv)
{
  gchar *dummy_dir = NULL;
  
  if (response_id == GTK_RESPONSE_OK) {
    gchar *volid = NULL;
    gchar *command = NULL;
    GtkWidget *dialog_progress = NULL;
  
    volid = xfburn_data_composition_get_volume_id (priv->composition);
    
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->check_only_iso))) {
      command = g_strconcat ("sh -c \"mkisofs -gui -graft-points -joliet -full-iso9660-filenames -iso-level 2 -volid '",
                             volid, "' -path-list ", priv->file_list, " > ", gtk_entry_get_text (GTK_ENTRY (priv->entry_path_iso)), "\"", NULL);

      dialog_progress = xfburn_create_iso_from_composition_progress_dialog_new ();
    }
    else {
      gchar *speed = NULL;
      gchar *write_mode = NULL;
      XfburnDevice *device;
      
      device = xfburn_device_box_get_selected_device (XFBURN_DEVICE_BOX (priv->device_box));
      speed = xfburn_device_box_get_speed (XFBURN_DEVICE_BOX (priv->device_box));

      write_mode = xfburn_write_mode_combo_box_get_cdrecord_param (XFBURN_WRITE_MODE_COMBO_BOX (priv->combo_mode));

      command = g_strconcat ("sh -c \"mkisofs -gui -graft-points -joliet -full-iso9660-filenames -iso-level 2 -volid '", volid,
                             "' -path-list ", priv->file_list, " | cdrecord -v gracetime=2", " dev=", device->node_path, 
                             " ", write_mode, " speed=", speed,
                             gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->check_eject)) ? " -eject" : "",
                             gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->check_dummy)) ? " -dummy" : "",
                             gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->check_burnfree)) ? " driveropts=burnfree" : "",
                             " - \"", NULL);
      g_free (speed);
      g_free (write_mode);
      
      dialog_progress = xfburn_burn_data_composition_progress_dialog_new ();
    }

    gtk_window_set_transient_for (GTK_WINDOW (dialog_progress), gtk_window_get_transient_for (GTK_WINDOW (dialog)));
    gtk_widget_hide (GTK_WIDGET (dialog));

    g_object_set_data (G_OBJECT (dialog_progress), "command", command);
    gtk_dialog_run (GTK_DIALOG (dialog_progress));
    
    g_free (command);
    g_free (volid);
  }
  
  unlink (priv->file_list);
  dummy_dir = xfburn_data_composition_get_dummy_dir (priv->composition);
  if (dummy_dir) {
    rmdir (dummy_dir);
  }
}

/* public */
gchar *
xfburn_burn_data_composition_dialog_get_command_iso (XfburnBurnDataCompositionDialog * dialog)
{
  return g_strdup (dialog->priv->command_iso);
}

gchar *
xfburn_burn_data_composition_dialog_get_command_burn (XfburnBurnDataCompositionDialog * dialog)
{
  return g_strdup (dialog->priv->command_burn);
}

GtkWidget *
xfburn_burn_data_composition_dialog_new (XfburnDataComposition *composition, const gchar * file_list)
{
  XfburnBurnDataCompositionDialog *obj;

  obj = XFBURN_BURN_COMPOSITION_DIALOG (g_object_new (XFBURN_TYPE_BURN_COMPOSITION_DIALOG, NULL));

  obj->priv->file_list = g_strdup (file_list);
  obj->priv->composition = composition;
  
  return GTK_WIDGET (obj);
}
