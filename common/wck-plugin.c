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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <gtk/gtk.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

#include "wck-plugin.h"

XfconfChannel *
wck_properties_get_channel (GObject *object_for_weak_ref, const gchar *channel_name)
{
  GError        *error = NULL;
  XfconfChannel *channel;

  g_return_val_if_fail (G_IS_OBJECT (object_for_weak_ref), NULL);

  if (!xfconf_init (&error))
    {
      g_critical ("Failed to initialize Xfconf: %s", error->message);
      g_error_free (error);
      return NULL;
    }

  //~ channel = xfconf_channel_get (XFCE_PANEL_CHANNEL_NAME);
  channel = xfconf_channel_get (channel_name);
  g_object_weak_ref (object_for_weak_ref, (GWeakNotify) xfconf_shutdown, NULL);

  return channel;
}


void
wck_about (XfcePanelPlugin *plugin, const gchar *icon_name)
{
    /* about dialog code. you can use the GtkAboutDialog
    * or the XfceAboutInfo widget */
    GdkPixbuf *icon;

    const gchar *auth[] =
    {
        "Alessio Piccoli <alepic@geckoblu.net>",
        "Cedric Leporcq <cedl38@gmail.com>",
        "Felix Krull <f_krull@gmx.de>",
        "Pavel Zlámal <zlamal@cesnet.cz>",
        "",
        "This code is derived from 'Window Applets' from Andrej Belcijan.",
        "See https://www.gnome-look.org/p/1115400 for details.",
        NULL
    };

    icon = xfce_panel_pixbuf_from_source(icon_name, NULL, 32);

    gtk_show_about_dialog (NULL,
            "logo", icon,
            "license", xfce_get_license_text(XFCE_LICENSE_TEXT_GPL),
            "version", PACKAGE_VERSION,
            "program-name", xfce_panel_plugin_get_display_name (plugin),
            "comments", xfce_panel_plugin_get_comment (plugin),
            "website", PACKAGE_URL,
            "copyright", "Copyright \302\251 2013-2015\n",
            "authors", auth,
            NULL );
    // TODO: add translators.

    if (icon)
        g_object_unref(G_OBJECT(icon) );
}


GtkWidget *show_refresh_item (XfcePanelPlugin *plugin)
{
    GtkWidget *refresh;
#if LIBXFCE4UI_CHECK_VERSION (4, 16, 0)
    refresh = xfce_gtk_image_menu_item_new_from_icon_name (_("_Refresh"), NULL, NULL, NULL, NULL, "view-refresh", NULL);
#else
    refresh = gtk_image_menu_item_new_from_stock (GTK_STOCK_REFRESH, NULL);
#endif
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(refresh));
    gtk_widget_show (refresh);

    return refresh;
}

void
wck_settings_save (XfcePanelPlugin *plugin, WckSettingsCb save_settings, gpointer prefs)
{
    XfceRc *rc;
    gchar *file;

    /* get the config file location */
    file = xfce_panel_plugin_save_location (plugin, TRUE);

    if (G_UNLIKELY (file == NULL))
    {
        DBG ("Failed to open config file");
        return;
    }

    /* open the config file, read/write */
    rc = xfce_rc_simple_open (file, FALSE);
    g_free (file);

    if (G_LIKELY (rc != NULL))
    {
        /* save the settings */
        DBG (".");
        save_settings (rc, prefs);

        /* close the rc file */
        xfce_rc_close (rc);
    }
}

void
wck_settings_load (XfcePanelPlugin *plugin, WckSettingsCb load_settings, gpointer prefs)
{
    /* get the plugin config file location */
    gchar *file = xfce_panel_plugin_save_location (plugin, TRUE);

    if (G_LIKELY (file != NULL))
    {
        /* open the config file, readonly */
        XfceRc *rc = xfce_rc_simple_open (file, TRUE);

        /* cleanup */
        g_free (file);

        if (G_LIKELY (rc != NULL))
        {
            /* read the settings */
            load_settings (rc, prefs);

            /* cleanup */
            xfce_rc_close (rc);

            /* leave the function, everything went well */
            return;
        }
    }

    /* something went wrong, apply default values */
    DBG ("Applying default settings");
    load_settings (NULL, prefs);
}

void
wck_configure_dialog (XfcePanelPlugin *plugin, GtkWidget *ca, GCallback response_cb, gpointer data)
{
    GtkWidget *dialog;
    GtkWidget *content_area;
    const gchar *name;

    /* block the plugin menu */
    xfce_panel_plugin_block_menu (plugin);

    /* create the dialog */
    name = xfce_panel_plugin_get_display_name (plugin);
    dialog = xfce_titled_dialog_new_with_mixed_buttons (_(name),
                                                        GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                                        "help-browser", _("Help"), GTK_RESPONSE_HELP,
                                                        "window-close", _("_Close"), GTK_RESPONSE_OK,
                                                        NULL);

    /* center dialog on the screen */
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

    /* set dialog icon */
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "xfce4-settings");

    /* link the dialog to the plugin, so we can destroy it when the plugin
    * is closed, but the dialog is still open */
    g_object_set_data (G_OBJECT (plugin), "dialog", dialog);

    /* connect the response signal to the dialog */
    g_signal_connect (G_OBJECT (dialog), "response",
                      response_cb, data);

    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog) );

    if (G_LIKELY (ca != NULL))
        gtk_container_add (GTK_CONTAINER (content_area), ca);
    else
        DBG("Failed to create content area");

    /* show the entire dialog */
    gtk_widget_show (dialog);
}

void
wck_configure_response (XfcePanelPlugin *plugin, GtkWidget *dialog, gint response, WckSettingsCb save_settings, gpointer data)
{
    if (response == GTK_RESPONSE_HELP)
    {
        gboolean result;

        result = g_spawn_command_line_async ("exo-open --launch WebBrowser " PACKAGE_URL, NULL);

        if (G_UNLIKELY (result == FALSE))
            g_warning (_("Unable to open the following url: %s"), PACKAGE_URL);
    }
    else
    {
        /* remove the dialog data from the plugin */
        g_object_set_data (G_OBJECT (plugin), "dialog", NULL);

        /* unlock the panel menu */
        xfce_panel_plugin_unblock_menu (plugin);

        /* save the plugin */
        wck_settings_save (plugin, save_settings, data);

        /* destroy the properties dialog */
        gtk_widget_destroy (dialog);
    }
}
