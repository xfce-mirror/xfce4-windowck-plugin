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
 *  Copyright (C) 2026 Mubashshir  <ahmubashshir@gmail.com>
 *
 */

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include <common/wck-plugin.h>

#include "wckspacer.h"
#include "wckspacer-dialogs.h"
#include "wckspacer-spacer.h"

/* prototypes */
static void wckspacer_construct(XfcePanelPlugin *plugin);


void
wckspacer_settings_save (WckSpacerPreferences *prefs)
{
    wck_conf_set_bool (prefs->conf, SETTING_ONLY_MAXIMIZED, prefs->only_maximized);
    wck_conf_set_bool (prefs->conf, SETTING_ONLY_CURRDISPLAY, prefs->only_current_display);
    wck_conf_set_bool (prefs->conf, SETTING_SHOW_ON_DESKTOP, prefs->show_on_desktop);
}

static void
wckspacer_save (G_GNUC_UNUSED XfcePanelPlugin *plugin, WckSpacerPlugin *wsp)
{
    wckspacer_settings_save (wsp->prefs);
}


static void
wckspacer_settings_load (WckSpacerPreferences *prefs)
{
    prefs->only_maximized = wck_conf_get_bool (prefs->conf, SETTING_ONLY_MAXIMIZED, DEFAULT_ONLY_MAXIMIZED);
    prefs->only_current_display = wck_conf_get_bool (prefs->conf, SETTING_ONLY_CURRDISPLAY, DEFAULT_ONLY_CURRDISPLAY);
    prefs->show_on_desktop = wck_conf_get_bool (prefs->conf, SETTING_SHOW_ON_DESKTOP, DEFAULT_SHOW_ON_DESKTOP);
}


static WckSpacerPreferences *
wckspacer_read (XfcePanelPlugin *plugin)
{
    /* allocate memory for the preferences structure */
    WckSpacerPreferences *prefs = g_slice_new0(WckSpacerPreferences);

    prefs->conf = wck_conf_new (plugin);
    wckspacer_settings_load (prefs);

    return prefs;
}

static WckSpacerPlugin *
wckspacer_new (XfcePanelPlugin *plugin)
{
    WckSpacerPlugin *wsp;

    /* allocate memory for the plugin structure */
    wsp = g_slice_new0 (WckSpacerPlugin);

    /* pointer to plugin */
    wsp->plugin = plugin;

    /* read the user settings */
    wsp->prefs = wckspacer_read(wsp->plugin);

    xfce_panel_plugin_set_expand (plugin, TRUE);

    return wsp;
}


static void
wckspacer_free (XfcePanelPlugin *plugin, WckSpacerPlugin *wsp)
{
    GtkWidget *dialog;

    if (wsp->controlwindow)
        wck_signal_handler_disconnect (G_OBJECT (wsp->controlwindow), wsp->cnh);
    disconnect_wnck (wsp->win);

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy(dialog);

    /* destroy the panel widgets */
    if (wsp->prefs->builder)
        g_object_unref(G_OBJECT (wsp->prefs->builder));

    if (G_LIKELY(wsp->menu != NULL))
        gtk_widget_destroy(wsp->menu);

    /* free the plugin structure */
    destroy_wnck(wsp->win);
    g_slice_free(WckConf, wsp->prefs->conf);
    g_slice_free(WckSpacerPreferences, wsp->prefs);
    g_slice_free(WckSpacerPlugin, wsp);
}

static gboolean
wckspacer_size_changed (XfcePanelPlugin *plugin, gint size, WckSpacerPlugin *wsp)
{
    if (xfce_panel_plugin_get_orientation(plugin) == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request(GTK_WIDGET (plugin), -1, size);
    else
        gtk_widget_set_size_request(GTK_WIDGET (plugin), size, -1);

    return TRUE;
}


static void on_refresh_item_activated (GtkMenuItem *refresh, WckSpacerPlugin *wsp)
{
    reload_wnck_spacer (wsp);
}

static XfcePanelPlugin* wckspacer_get_plugin(gpointer wtp) {
    return ((WckSpacerPlugin *) wtp)->plugin;
}


static void wckspacer_construct(XfcePanelPlugin *plugin)
{
    WckSpacerPlugin *wsp;
    GtkWidget *refresh;

    /* setup transation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* set client type */
    xfw_set_client_type(XFW_CLIENT_TYPE_PAGER);

    /* create the plugin */
    wsp = wckspacer_new(plugin);

    /* Set event handling (spacer clicks) */
    g_signal_connect (G_OBJECT (wsp->plugin), "button-press-event",
                      G_CALLBACK (on_spacer_pressed), wsp);

    g_signal_connect (G_OBJECT (wsp->plugin), "button-release-event",
                      G_CALLBACK (on_spacer_released), wsp);

    /* connect plugin signals */

    g_signal_connect(G_OBJECT (plugin), "free-data", G_CALLBACK (wckspacer_free), wsp);

    g_signal_connect(G_OBJECT (plugin), "save", G_CALLBACK (wckspacer_save), wsp);

    g_signal_connect(G_OBJECT (plugin), "size-changed", G_CALLBACK (wckspacer_size_changed), wsp);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure(plugin);
    g_signal_connect(G_OBJECT (plugin), "configure-plugin", G_CALLBACK (wckspacer_configure), wsp);

    /* show the about menu item and connect signal */
    xfce_panel_plugin_menu_show_about(plugin);
    g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (wck_about), WCKSPACER_ICON);

    /* add custom menu items */
    refresh = show_refresh_item (plugin);
    g_signal_connect (G_OBJECT (refresh), "activate", G_CALLBACK (on_refresh_item_activated), wsp);

    /* start tracking */
    wsp->win = construct_wnck(wsp);
    wsp->win->get_plugin = wckspacer_get_plugin;
    init_wnck (wsp->win, wsp->prefs->only_maximized, wsp->prefs->only_current_display);
}


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (wckspacer_construct);
