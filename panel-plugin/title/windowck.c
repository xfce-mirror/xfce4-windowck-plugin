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

#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>
#include <common/wck-plugin.h>

#include "windowck.h"
#include "windowck-dialogs.h"
#include "windowck-title.h"

/* default settings */
#define DEFAULT_ONLY_MAXIMIZED TRUE
#define DEFAULT_SHOW_ON_DESKTOP FALSE
#define DEFAULT_HIDE_TITLE FALSE
#define DEFAULT_SHOW_TOOLTIPS TRUE
#define DEFAULT_SHOW_ICON TRUE
#define DEFAULT_ICON_ON_RIGHT FALSE
#define DEFAULT_SIZE_MODE FIXE
#define DEFAULT_TITLE_SIZE 80
#define DEFAULT_TITLE_ALIGNMENT CENTER
#define DEFAULT_TITLE_PADDING 3
#define DEFAULT_CUSTOM_FONT FALSE
#define DEFAULT_TITLE_FONT "sans 10"

/* prototypes */
static void windowck_construct(XfcePanelPlugin *plugin);

void windowck_save(XfcePanelPlugin *plugin, WindowckPlugin *wckp) {
    XfceRc *rc;
    gchar *file;

    /* get the config file location */
    file = xfce_panel_plugin_save_location(plugin, TRUE);

    if (G_UNLIKELY (file == NULL)) {
        DBG("Failed to open config file");
        return;
    }

    /* open the config file, read/write */
    rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    if (G_LIKELY (rc != NULL)) {
        /* save the settings */
        DBG(".");
        xfce_rc_write_bool_entry(rc, "only_maximized", wckp->prefs->only_maximized);
        xfce_rc_write_bool_entry(rc, "show_on_desktop", wckp->prefs->show_on_desktop);
        xfce_rc_write_bool_entry(rc, "show_icon", wckp->prefs->show_icon);
        xfce_rc_write_bool_entry(rc, "icon_on_right", wckp->prefs->icon_on_right);
        xfce_rc_write_bool_entry(rc, "hide_title", wckp->prefs->hide_title);
        xfce_rc_write_bool_entry(rc, "show_tooltips", wckp->prefs->show_tooltips);
        xfce_rc_write_int_entry(rc, "size_mode", wckp->prefs->size_mode);
        xfce_rc_write_int_entry(rc, "title_size", wckp->prefs->title_size);
        xfce_rc_write_bool_entry(rc, "custom_font", wckp->prefs->custom_font);
        if (wckp->prefs->title_font)
            xfce_rc_write_entry(rc, "title_font", wckp->prefs->title_font);

        xfce_rc_write_int_entry(rc, "title_alignment", wckp->prefs->title_alignment);
        xfce_rc_write_int_entry(rc, "title_padding", wckp->prefs->title_padding);

        /* close the rc file */
        xfce_rc_close(rc);
    }
}

static void windowck_read(WindowckPlugin *wckp) {
    XfceRc *rc;
    gchar *file;
    const gchar *title_font;

    /* allocate memory for the preferences structure */
    wckp->prefs = g_slice_new0(WCKPreferences);

    /* get the plugin config file location */
    file = xfce_panel_plugin_save_location(wckp->plugin, TRUE);

    if (G_LIKELY (file != NULL)) {
        /* open the config file, readonly */
        rc = xfce_rc_simple_open(file, TRUE);

        /* cleanup */
        g_free(file);

        if (G_LIKELY (rc != NULL)) {
            /* read the settings */
            wckp->prefs->only_maximized = xfce_rc_read_bool_entry(rc, "only_maximized", DEFAULT_ONLY_MAXIMIZED);
            wckp->prefs->show_on_desktop = xfce_rc_read_bool_entry(rc, "show_on_desktop", DEFAULT_SHOW_ON_DESKTOP);
            wckp->prefs->show_icon = xfce_rc_read_bool_entry(rc, "show_icon", DEFAULT_SHOW_ICON);
            wckp->prefs->icon_on_right = xfce_rc_read_bool_entry(rc, "icon_on_right", DEFAULT_ICON_ON_RIGHT);
            wckp->prefs->hide_title = xfce_rc_read_bool_entry(rc, "hide_title", DEFAULT_HIDE_TITLE);
            wckp->prefs->show_tooltips = xfce_rc_read_bool_entry(rc, "show_tooltips", DEFAULT_SHOW_TOOLTIPS);
            wckp->prefs->size_mode = xfce_rc_read_int_entry (rc, "size_mode", DEFAULT_SIZE_MODE);
            wckp->prefs->title_size = xfce_rc_read_int_entry(rc, "title_size", DEFAULT_TITLE_SIZE);
            wckp->prefs->custom_font = xfce_rc_read_bool_entry(rc, "custom_font", DEFAULT_CUSTOM_FONT);
            title_font = xfce_rc_read_entry(rc, "title_font", DEFAULT_TITLE_FONT);
            wckp->prefs->title_font = g_strdup(title_font);
            wckp->prefs->title_alignment = xfce_rc_read_int_entry(rc, "title_alignment", DEFAULT_TITLE_ALIGNMENT);
            wckp->prefs->title_padding = xfce_rc_read_int_entry(rc, "title_padding", DEFAULT_TITLE_PADDING);

            /* cleanup */
            xfce_rc_close(rc);

            /* leave the function, everything went well */
            return;
        }
    }

    /* something went wrong, apply default values */
    DBG("Applying default settings");

    wckp->prefs->only_maximized = DEFAULT_ONLY_MAXIMIZED;
    wckp->prefs->show_on_desktop = DEFAULT_SHOW_ON_DESKTOP;
    wckp->prefs->show_icon = DEFAULT_SHOW_ICON;
    wckp->prefs->icon_on_right = DEFAULT_ICON_ON_RIGHT;
    wckp->prefs->hide_title = DEFAULT_HIDE_TITLE;
    wckp->prefs->show_tooltips = DEFAULT_SHOW_TOOLTIPS;
    wckp->prefs->size_mode = DEFAULT_SIZE_MODE;
    wckp->prefs->title_size = DEFAULT_TITLE_SIZE;
    wckp->prefs->custom_font = DEFAULT_CUSTOM_FONT;
    wckp->prefs->title_font = DEFAULT_TITLE_FONT;
    wckp->prefs->title_alignment = DEFAULT_TITLE_ALIGNMENT;
    wckp->prefs->title_padding = DEFAULT_TITLE_PADDING;
}

static void createIcon (WindowckPlugin *wckp) {
    wckp->icon = g_slice_new0 (WindowIcon);
    wckp->icon->eventbox = GTK_EVENT_BOX (gtk_event_box_new());
    wckp->icon->image = GTK_IMAGE (gtk_image_new());

    gtk_widget_set_can_focus (GTK_WIDGET(wckp->icon->eventbox), TRUE);

    gtk_container_add (GTK_CONTAINER (wckp->icon->eventbox), GTK_WIDGET(wckp->icon->image));
    gtk_event_box_set_visible_window (wckp->icon->eventbox, FALSE);

    gtk_box_pack_start (GTK_BOX (wckp->hvbox), GTK_WIDGET(wckp->icon->eventbox), FALSE, FALSE, 0);

    gtk_widget_show_all(GTK_WIDGET(wckp->icon->eventbox));
}

static WindowckPlugin * windowck_new(XfcePanelPlugin *plugin) {
    WindowckPlugin *wckp;
    WckUtils *win;

    GtkOrientation orientation;
    GtkWidget *label;

    /* allocate memory for the plugin structure */
    wckp = g_slice_new0 (WindowckPlugin);

    /* pointer to plugin */
    wckp->plugin = plugin;

    /* read the user settings */
    windowck_read(wckp);

    /* get the current orientation */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* not needed for shrink mode */
    if (!wckp->prefs->size_mode == SHRINK)
        xfce_panel_plugin_set_shrink (plugin, TRUE);

    /* create some panel widgets */
    wckp->ebox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(wckp->ebox), FALSE);
    gtk_widget_set_name(wckp->ebox, "XfceWindowckPlugin");

    wckp->alignment = gtk_alignment_new (0.5, 0.5, 0, 0);

    wckp->hvbox = xfce_hvbox_new(orientation, FALSE, 2);

    /* some wckp widgets */
    label = gtk_label_new("");
    wckp->title = GTK_LABEL (label);

    createIcon (wckp);

    gtk_box_pack_start (GTK_BOX(wckp->hvbox), label, TRUE, TRUE, 0);

    if (wckp->prefs->icon_on_right) {
        gtk_box_reorder_child (GTK_BOX (wckp->hvbox), GTK_WIDGET(wckp->icon->eventbox), 1);
    }

    gtk_container_add(GTK_CONTAINER(wckp->alignment), GTK_WIDGET(wckp->hvbox));
    gtk_container_add(GTK_CONTAINER(wckp->ebox), wckp->alignment);

    /* show widgets */
    gtk_widget_show(wckp->ebox);
    gtk_widget_show(wckp->alignment);
    gtk_widget_show(wckp->hvbox);
    gtk_widget_show(label);

    return wckp;
}

static void windowck_free(XfcePanelPlugin *plugin, WindowckPlugin *wckp) {
    GtkWidget *dialog;

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy(dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy(wckp->hvbox);

    /* free the plugin structure */
    g_slice_free(WindowIcon, wckp->icon);
    g_slice_free(WckUtils, wckp->win);
    g_slice_free(WCKPreferences, wckp->prefs);
    g_slice_free(WindowckPlugin, wckp);
}

static void windowck_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, WindowckPlugin *wckp) {
    /* change the orienation of the box */
    xfce_hvbox_set_orientation(XFCE_HVBOX (wckp->hvbox), orientation);
}

static void windowck_screen_position_changed(XfcePanelPlugin *plugin, XfceScreenPosition *position, WindowckPlugin *wckp) {
    if (wckp->prefs->size_mode != SHRINK) {
        xfce_panel_plugin_set_shrink (plugin, FALSE);
        gtk_label_set_width_chars(wckp->title, 1);
        xfce_panel_plugin_set_shrink (plugin, TRUE);
        resizeTitle(wckp);
    }
}

static gboolean windowck_size_changed(XfcePanelPlugin *plugin, gint size, WindowckPlugin *wckp) {
    GtkOrientation orientation;

    /* get the orientation of the plugin */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* set the widget size */
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request(GTK_WIDGET (plugin), -1, size);
    else
        gtk_widget_set_size_request(GTK_WIDGET (plugin), size, -1);

    /* set icon size */
    if (size >= 32)
        wckp->icon->size = GTK_ICON_SIZE_SMALL_TOOLBAR;
    else
        wckp->icon->size = GTK_ICON_SIZE_MENU;

    /* we handled the orientation */
    return TRUE;
}

static void windowck_construct(XfcePanelPlugin *plugin) {
    WindowckPlugin *wckp;

    /* setup transation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* create the plugin */
    wckp = windowck_new(plugin);

    /* add the ebox to the panel */
    gtk_container_add(GTK_CONTAINER (plugin), wckp->ebox);

    /* show the panel's right-click menu on this ebox */
    xfce_panel_plugin_add_action_widget(plugin, wckp->ebox);

    // Set event handling (icon & title clicks)
    g_signal_connect(G_OBJECT (wckp->ebox), "button-press-event", G_CALLBACK (on_title_pressed), wckp);

    g_signal_connect(G_OBJECT (wckp->ebox), "button-release-event", G_CALLBACK (on_title_released), wckp);

    g_signal_connect(G_OBJECT (wckp->icon->eventbox), "button-release-event", G_CALLBACK (on_icon_released), wckp);

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
                    G_CALLBACK (wck_about), "windowck-plugin");

//    //plugin->priv->menu_items;
//    GtkWidget *item1 = gtk_menu_item_new_with_label("Test");
//    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(item1));
//    gtk_widget_show(GTK_WIDGET(item1));

    /* start tracking title size */
    initTitle(wckp);

    /* start tracking title text */
    wckp->win = g_slice_new0 (WckUtils);
    initWnck(wckp->win, wckp->prefs->only_maximized, wckp);
}

/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER(windowck_construct);
