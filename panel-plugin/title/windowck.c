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
 *  Copyright (C) 2013 Alessio Piccoli <alepic@geckoblu.net>
 *                     Cedric Leporcq  <cedl38@gmail.com>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include <common/wck-plugin.h>

#include "windowck.h"
#include "windowck-dialogs.h"
#include "windowck-title.h"

#define SETTING_FULL_NAME           "full_name"
#define SETTING_TWO_LINES           "two_lines"
#define SETTING_SHOW_TOOLTIPS       "show_tooltips"
#define SETTING_SIZE_MODE           "size_mode"
#define SETTING_TITLE_SIZE          "title_size"
#define SETTING_TITLE_ALIGNMENT     "title_alignment"
#define SETTING_TITLE_PADDING       "title_padding"
#define SETTING_SYNC_WM_FONT        "sync_wm_font"
#define SETTING_TITLE_FONT          "title_font"
#define SETTING_SUBTITLE_FONT       "subtitle_font"
#define SETTING_INACTIVE_TEXT_ALPHA "inactive_text_alpha"
#define SETTING_INACTIVE_TEXT_SHADE "inactive_text_shade"

/* default settings */
#define DEFAULT_FULL_NAME TRUE
#define DEFAULT_TWO_LINES FALSE
#define DEFAULT_SHOW_TOOLTIPS TRUE
#define DEFAULT_SIZE_MODE FIXED
#define DEFAULT_TITLE_SIZE 80
#define DEFAULT_TITLE_ALIGNMENT CENTER
#define DEFAULT_TITLE_PADDING 3
#define DEFAULT_SYNC_WM_FONT TRUE
#define DEFAULT_TITLE_FONT "sans 10"
#define DEFAULT_SUBTITLE_FONT "sans 10"
#define DEFAULT_INACTIVE_TEXT_ALPHA 60
#define DEFAULT_INACTIVE_TEXT_SHADE 110

/* prototypes */
static void windowck_construct(XfcePanelPlugin *plugin);


void
wcktitle_settings_save (XfceRc *rc, WCKPreferences *prefs)
{
    xfce_rc_write_bool_entry(rc, SETTING_ONLY_MAXIMIZED, prefs->only_maximized);
    xfce_rc_write_bool_entry(rc, SETTING_SHOW_ON_DESKTOP, prefs->show_on_desktop);
    xfce_rc_write_bool_entry(rc, SETTING_FULL_NAME, prefs->full_name);
    xfce_rc_write_bool_entry(rc, SETTING_TWO_LINES, prefs->two_lines);
    xfce_rc_write_bool_entry(rc, SETTING_SHOW_TOOLTIPS, prefs->show_tooltips);
    xfce_rc_write_int_entry(rc, SETTING_SIZE_MODE, prefs->size_mode);
    xfce_rc_write_int_entry(rc, SETTING_TITLE_SIZE, prefs->title_size);
    xfce_rc_write_bool_entry(rc, SETTING_SYNC_WM_FONT, prefs->sync_wm_font);
    if (prefs->title_font)
        xfce_rc_write_entry(rc, SETTING_TITLE_FONT, prefs->title_font);

    if (prefs->subtitle_font)
        xfce_rc_write_entry(rc, SETTING_SUBTITLE_FONT, prefs->subtitle_font);

    xfce_rc_write_int_entry(rc, SETTING_TITLE_ALIGNMENT, prefs->title_alignment);
    xfce_rc_write_int_entry(rc, SETTING_TITLE_PADDING, prefs->title_padding);
    xfce_rc_write_int_entry(rc, SETTING_INACTIVE_TEXT_ALPHA, prefs->inactive_text_alpha);
    xfce_rc_write_int_entry(rc, SETTING_INACTIVE_TEXT_SHADE, prefs->inactive_text_shade);
}

static void
windowck_save (XfcePanelPlugin *plugin, WindowckPlugin *wckp)
{
    wck_settings_save (plugin, (WckSettingsCb) wcktitle_settings_save, wckp->prefs);
}


static void
wcktitle_settings_load (XfceRc *rc, WCKPreferences *prefs)
{
    const gchar *title_font;
    const gchar *subtitle_font;

    prefs->only_maximized = xfce_rc_read_bool_entry (rc, SETTING_ONLY_MAXIMIZED, DEFAULT_ONLY_MAXIMIZED);
    prefs->show_on_desktop = xfce_rc_read_bool_entry (rc, SETTING_SHOW_ON_DESKTOP, DEFAULT_SHOW_ON_DESKTOP);
    prefs->full_name = xfce_rc_read_bool_entry (rc, SETTING_FULL_NAME, DEFAULT_FULL_NAME);
    prefs->two_lines = xfce_rc_read_bool_entry (rc, SETTING_TWO_LINES, DEFAULT_TWO_LINES);
    prefs->show_tooltips = xfce_rc_read_bool_entry (rc, SETTING_SHOW_TOOLTIPS, DEFAULT_SHOW_TOOLTIPS);
    prefs->size_mode = xfce_rc_read_int_entry (rc, SETTING_SIZE_MODE, DEFAULT_SIZE_MODE);
    prefs->title_size = xfce_rc_read_int_entry (rc, SETTING_TITLE_SIZE, DEFAULT_TITLE_SIZE);
    prefs->sync_wm_font = xfce_rc_read_bool_entry (rc, SETTING_SYNC_WM_FONT, DEFAULT_SYNC_WM_FONT);

    title_font = xfce_rc_read_entry (rc, SETTING_TITLE_FONT, DEFAULT_TITLE_FONT);
    prefs->title_font = g_strdup (title_font);

    subtitle_font = xfce_rc_read_entry (rc, SETTING_SUBTITLE_FONT, DEFAULT_SUBTITLE_FONT);
    prefs->subtitle_font = g_strdup (subtitle_font);

    prefs->title_alignment = xfce_rc_read_int_entry (rc, SETTING_TITLE_ALIGNMENT, DEFAULT_TITLE_ALIGNMENT);
    prefs->title_padding = xfce_rc_read_int_entry (rc, SETTING_TITLE_PADDING, DEFAULT_TITLE_PADDING);
    prefs->inactive_text_alpha = xfce_rc_read_int_entry (rc, SETTING_INACTIVE_TEXT_ALPHA, DEFAULT_INACTIVE_TEXT_ALPHA);
    prefs->inactive_text_shade = xfce_rc_read_int_entry (rc, SETTING_INACTIVE_TEXT_SHADE, DEFAULT_INACTIVE_TEXT_SHADE);
}


static WCKPreferences *
windowck_read (XfcePanelPlugin *plugin)
{
    /* allocate memory for the preferences structure */
    WCKPreferences *prefs = g_slice_new0(WCKPreferences);

    wck_settings_load (plugin, (WckSettingsCb) wcktitle_settings_load, prefs);

    return prefs;
}


static WindowckPlugin * windowck_new(XfcePanelPlugin *plugin)
{
    WindowckPlugin *wckp;

    GtkOrientation orientation;

    /* allocate memory for the plugin structure */
    wckp = g_slice_new0 (WindowckPlugin);

    /* pointer to plugin */
    wckp->plugin = plugin;

    /* read the user settings */
    wckp->prefs = windowck_read(wckp->plugin);

    /* get the current orientation */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* not needed for shrink mode */
    if (wckp->prefs->size_mode != SHRINK)
        xfce_panel_plugin_set_shrink (plugin, TRUE);

    /* create some panel widgets */
    wckp->ebox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(wckp->ebox), FALSE);
    gtk_widget_set_name(wckp->ebox, "XfceWindowckPlugin");

    wckp->box = gtk_box_new (orientation, 2);
    gtk_box_set_homogeneous (GTK_BOX (wckp->box), FALSE);
    gtk_widget_set_halign (wckp->box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (wckp->box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top (wckp->box, DEFAULT_TITLE_PADDING);
    gtk_widget_set_margin_bottom (wckp->box, DEFAULT_TITLE_PADDING);

    /* some wckp widgets */
    wckp->title = GTK_LABEL (gtk_label_new (""));
    gtk_box_pack_start (GTK_BOX (wckp->box), GTK_WIDGET (wckp->title), TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(wckp->ebox), GTK_WIDGET(wckp->box));

    /* show widgets */
    gtk_widget_show(wckp->ebox);
    gtk_widget_show(wckp->box);
    gtk_widget_show(GTK_WIDGET (wckp->title));

    return wckp;
}


static void windowck_free(XfcePanelPlugin *plugin, WindowckPlugin *wckp)
{
    GtkWidget *dialog;

    disconnect_wnck (wckp->win);

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy(dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy(wckp->box);

    /* free the plugin structure */
    g_slice_free(WckUtils, wckp->win);
    g_slice_free(WCKPreferences, wckp->prefs);
    g_slice_free(WindowckPlugin, wckp);
}


static void windowck_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, WindowckPlugin *wckp)
{
    /* change the orienation of the box */
    gtk_orientable_set_orientation (GTK_ORIENTABLE (wckp->box), orientation);
}


static void windowck_screen_position_changed(XfcePanelPlugin *plugin, XfceScreenPosition *position, WindowckPlugin *wckp)
{
    if (wckp->prefs->size_mode != SHRINK)
    {
        xfce_panel_plugin_set_shrink (plugin, FALSE);
        gtk_label_set_width_chars(wckp->title, 1);
        xfce_panel_plugin_set_shrink (plugin, TRUE);
        resize_title(wckp);
    }
}


static gboolean windowck_size_changed(XfcePanelPlugin *plugin, gint size, WindowckPlugin *wckp)
{
    GtkOrientation orientation;

    /* get the orientation of the plugin */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* set the widget size */
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request(GTK_WIDGET (plugin), -1, size);
    else
        gtk_widget_set_size_request(GTK_WIDGET (plugin), size, -1);

    /* we handled the orientation */
    return TRUE;
}


static void on_refresh_item_activated (GtkMenuItem *refresh, WindowckPlugin *wckp)
{
    init_title(wckp);
    reload_wnck_title (wckp);
}


static void windowck_construct(XfcePanelPlugin *plugin)
{
    WindowckPlugin *wckp;
    GtkWidget *refresh;

    /* setup transation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* create the plugin */
    wckp = windowck_new(plugin);

    /* add the ebox to the panel */
    gtk_container_add(GTK_CONTAINER (plugin), wckp->ebox);

    /* show the panel's right-click menu on this ebox */
    xfce_panel_plugin_add_action_widget(plugin, wckp->ebox);

    /* Set event handling (title clicks) */
    g_signal_connect(G_OBJECT (wckp->ebox), "button-press-event", G_CALLBACK (on_title_pressed), wckp);

    g_signal_connect(G_OBJECT (wckp->ebox), "button-release-event", G_CALLBACK (on_title_released), wckp);

    /* connect plugin signals */

    g_signal_connect(G_OBJECT (plugin), "free-data", G_CALLBACK (windowck_free), wckp);

    g_signal_connect(G_OBJECT (plugin), "save", G_CALLBACK (windowck_save), wckp);

    g_signal_connect(G_OBJECT (plugin), "size-changed", G_CALLBACK (windowck_size_changed), wckp);

    g_signal_connect(G_OBJECT (plugin), "screen-position-changed", G_CALLBACK (windowck_screen_position_changed), wckp);

    g_signal_connect(G_OBJECT (plugin), "orientation-changed", G_CALLBACK (windowck_orientation_changed), wckp);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure(plugin);
    g_signal_connect(G_OBJECT (plugin), "configure-plugin", G_CALLBACK (windowck_configure), wckp);

    /* show the about menu item and connect signal */
    xfce_panel_plugin_menu_show_about(plugin);
    g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (wck_about), WCKTITLE_ICON);

    /* add custom menu items */
    refresh = show_refresh_item (plugin);
    g_signal_connect (G_OBJECT (refresh), "activate", G_CALLBACK (on_refresh_item_activated), wckp);

    /* start tracking title text */
    wckp->win = g_slice_new0 (WckUtils);
    init_wnck(wckp->win, wckp->prefs->only_maximized, wckp);

    /* start tracking title size */
    init_title(wckp);
}


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER(windowck_construct);
