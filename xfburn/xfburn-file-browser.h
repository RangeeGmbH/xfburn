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

#ifndef __XFBURN_FILE_BROWSER_H__
#define __XFBURN_FILE_BROWSER_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define XFBURN_TYPE_FILE_BROWSER            (xfburn_file_browser_get_type ())
#define XFBURN_FILE_BROWSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFBURN_TYPE_FILE_BROWSER, XfburnFileBrowser))
#define XFBURN_FILE_BROWSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XFBURN_TYPE_FILE_BROWSER, XfburnFileBrowserClass))
#define XFBURN_IS_FILE_BROWSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFBURN_TYPE_FILE_BROWSER))
#define XFBURN_IS_FILE_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFBURN_TYPE_FILE_BROWSER))
#define XFBURN_FILE_BROWSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), XFBURN_TYPE_FILE_BROWSER, XfburnFileBrowserClass))
typedef struct _XfburnFileBrowser XfburnFileBrowser;
typedef struct _XfburnFileBrowserClass XfburnFileBrowserClass;

struct _XfburnFileBrowser
{
  GtkHPaned hpaned;

  GtkWidget *fs_browser;
  GtkWidget *directory_browser;
};

struct _XfburnFileBrowserClass
{
  GtkHPanedClass parent_class;
};

GtkType xfburn_file_browser_get_type (void) G_GNUC_CONST;
GtkWidget *xfburn_file_browser_new (void);

G_END_DECLS
#endif