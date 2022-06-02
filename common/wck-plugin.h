/*  $Id$
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Copyright (C) 2013 Cedric Leporcq  <cedl38@gmail.com>
 *
 */

#ifndef WCK_PLUGIN_H_
#define WCK_PLUGIN_H_

#include <xfconf/xfconf.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>

G_BEGIN_DECLS

typedef void (*WckSettingsCb) (XfceRc *rc, gpointer prefs);
typedef void (*WckSettingsSetDefault) (gpointer prefs);

XfconfChannel *
wck_properties_get_channel (GObject *object_for_weak_ref, const gchar *channel_name);
void wck_about (XfcePanelPlugin *plugin, const gchar *icon_name);
GtkWidget *show_refresh_item (XfcePanelPlugin *plugin);
void wck_settings_save (XfcePanelPlugin *plugin, WckSettingsCb save_settings, gpointer prefs);
void wck_settings_load (XfcePanelPlugin *plugin, WckSettingsCb load_settings, WckSettingsSetDefault set_default, gpointer prefs);
void wck_configure_dialog (XfcePanelPlugin *plugin, GtkWidget *ca, GCallback response_cb, gpointer data);
void wck_configure_response (XfcePanelPlugin *plugin, GtkWidget *dialog, gint response, WckSettingsCb save_settings, gpointer data);

G_END_DECLS

#endif /* WCK_PLUGIN_H_ */
