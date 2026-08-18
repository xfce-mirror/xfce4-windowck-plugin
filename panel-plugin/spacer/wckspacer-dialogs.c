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

#include <common/wck-plugin.h>

#include "wckspacer.h"
#include "wckspacer-spacer.h"
#include "wckspacer-dialogs.h"


static void
on_only_maximized_toggled (GtkRadioButton *only_maximized, WckSpacerPlugin *wsp)
{
    wsp->prefs->only_maximized = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(only_maximized));
    reload_wnck_spacer (wsp);
}

static void
on_only_current_display_toggled (GtkToggleButton *only_current_display, WckSpacerPlugin *wsp)
{
    wsp->prefs->only_current_display = gtk_toggle_button_get_active (only_current_display);
    reload_wnck_spacer (wsp);
}


static void
on_show_on_desktop_toggled (GtkToggleButton *show_on_desktop, WckSpacerPlugin *wsp)
{
    wsp->prefs->show_on_desktop = gtk_toggle_button_get_active(show_on_desktop);

    reload_wnck_spacer (wsp);
}

static GtkWidget *
build_properties_area (WckSpacerPlugin *wsp)
{
    GError *error = NULL;

    g_clear_object (&wsp->prefs->builder);

    wsp->prefs->builder = gtk_builder_new();

    if (gtk_builder_add_from_resource(wsp->prefs->builder, "/org/xfce/windowck-plugin/wckspacer/wckspacer-dialogs.glade", &error)) {
        GObject *area = gtk_builder_get_object(wsp->prefs->builder, "vbox0");

        if (G_LIKELY (area != NULL)) {
            GtkRadioButton *only_maximized, *active_window;
            GtkToggleButton *show_on_desktop;
            GtkToggleButton *only_current_display;

            only_maximized = GTK_RADIO_BUTTON (wck_dialog_get_widget (wsp->prefs->builder, "only_maximized"));
            active_window = GTK_RADIO_BUTTON (wck_dialog_get_widget (wsp->prefs->builder, "active_window"));
            if (G_LIKELY (only_maximized != NULL && active_window != NULL)) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(only_maximized), wsp->prefs->only_maximized);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_window), !wsp->prefs->only_maximized);
                g_signal_connect(only_maximized, "toggled", G_CALLBACK(on_only_maximized_toggled), wsp);
            }

            show_on_desktop = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wsp->prefs->builder, "show_on_desktop"));
            if (G_LIKELY (show_on_desktop != NULL)) {
                gtk_toggle_button_set_active(show_on_desktop, wsp->prefs->show_on_desktop);
                g_signal_connect(show_on_desktop, "toggled", G_CALLBACK(on_show_on_desktop_toggled), wsp);
            }

            only_current_display = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wsp->prefs->builder, "only_current_display"));
            if (G_LIKELY (only_current_display != NULL)) {
                gtk_toggle_button_set_active (only_current_display, wsp->prefs->only_current_display);
                g_signal_connect (only_current_display, "toggled",
                                  G_CALLBACK (on_only_current_display_toggled), wsp);
            }

            return GTK_WIDGET(area);
        } else {
            g_set_error_literal (&error, 0, 0, "No widget with the name \"vbox0\" found");
        }
    }

    g_critical("Failed to construct the builder for plugin %s-%d: %s.", xfce_panel_plugin_get_name (wsp->plugin), xfce_panel_plugin_get_unique_id (wsp->plugin), error->message);
    g_error_free(error);
    g_clear_object (&wsp->prefs->builder);

    return NULL;
}


static void
wckspacer_configure_response (GtkWidget *dialog, gint response, WckSpacerPlugin *wsp)
{
    wck_configure_response (wsp->plugin, dialog, response, (WckSettingsCb) wckspacer_settings_save, wsp->prefs);
}


void
wckspacer_configure (XfcePanelPlugin *plugin, WckSpacerPlugin *wsp)
{
    GtkWidget *ca;

    ca = build_properties_area (wsp);

    wck_configure_dialog (plugin, WCKSPACER_ICON, ca, G_CALLBACK(wckspacer_configure_response), wsp);
}
