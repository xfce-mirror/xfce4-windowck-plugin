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

#include <common/wck-plugin.h>

#include "wckbuttons.h"
#include "wckbuttons-dialogs.h"
#include "wckbuttons-theme.h"

/* default settings */
#define DEFAULT_ONLY_MAXIMIZED TRUE
#define DEFAULT_SHOW_ON_DESKTOP FALSE
#define DEFAULT_SYNC_WM_THEME TRUE
#define DEFAULT_BUTTON_LAYOUT "HMC"

#define BUTTONS_SIGNALS_CONNECT(name, id) \
        g_signal_connect (G_OBJECT (wb->button[id]->eventbox), "button-press-event", G_CALLBACK (name##_button_pressed), wb); \
        g_signal_connect (G_OBJECT (wb->button[id]->eventbox), "button-release-event", G_CALLBACK (name##_button_release), wb); \
        g_signal_connect (G_OBJECT (wb->button[id]->eventbox), "enter-notify-event", G_CALLBACK (name##_button_hover_enter), wb); \
        g_signal_connect (G_OBJECT (wb->button[id]->eventbox), "leave-notify-event", G_CALLBACK (name##_button_hover_leave), wb);


/* prototypes */
static void
wckbuttons_construct (XfcePanelPlugin *plugin);


void
wckbuttons_settings_save (XfceRc *rc, WBPreferences *prefs)
{
    xfce_rc_write_bool_entry(rc, "only_maximized", prefs->only_maximized);
    xfce_rc_write_bool_entry(rc, "show_on_desktop", prefs->show_on_desktop);
    xfce_rc_write_bool_entry(rc, "sync_wm_theme", prefs->sync_wm_theme);
    if (prefs->button_layout)
        xfce_rc_write_entry (rc, "button_layout", prefs->button_layout);

    if (prefs->theme)
        xfce_rc_write_entry (rc, "theme", prefs->theme);
}

static void
wckbuttons_save (XfcePanelPlugin *plugin,
             WBPlugin    *wb)
{
    wck_settings_save (plugin, (WckSettingsCb) wckbuttons_settings_save, wb->prefs);
}


static void
wckbuttons_settings_load (XfceRc *rc, WBPreferences *prefs)
{
    if (rc != NULL)
    {
        const gchar *button_layout;
        const gchar *theme;

        prefs->only_maximized = xfce_rc_read_bool_entry(rc, "only_maximized", DEFAULT_ONLY_MAXIMIZED);
        prefs->show_on_desktop = xfce_rc_read_bool_entry(rc, "show_on_desktop", DEFAULT_SHOW_ON_DESKTOP);
        prefs->sync_wm_theme = xfce_rc_read_bool_entry(rc, "sync_wm_theme", DEFAULT_SYNC_WM_THEME);
        button_layout = xfce_rc_read_entry (rc, "button_layout", DEFAULT_BUTTON_LAYOUT);
        prefs->button_layout = button_layout_filter (button_layout, DEFAULT_BUTTON_LAYOUT);
        theme = xfce_rc_read_entry (rc, "theme", DEFAULT_THEME);
        prefs->theme = g_strdup (theme);
    }
    else
    {
        prefs->only_maximized = DEFAULT_ONLY_MAXIMIZED;
        prefs->show_on_desktop = DEFAULT_SHOW_ON_DESKTOP;
        prefs->sync_wm_theme = DEFAULT_SYNC_WM_THEME;
        prefs->button_layout = g_strdup (DEFAULT_BUTTON_LAYOUT);
        prefs->theme = g_strdup (DEFAULT_THEME);
    }
}

static WBPreferences *
wckbuttons_read (XfcePanelPlugin *plugin)
{
    /* allocate memory for the preferences structure */
    WBPreferences *prefs = g_slice_new0(WBPreferences);

    wck_settings_load (plugin, (WckSettingsCb) wckbuttons_settings_load, prefs);

    return prefs;
}

static WindowButton *
window_button_new (WBPlugin *wb)
{
    WindowButton *button = g_new0 (WindowButton, 1);

    button->eventbox = GTK_EVENT_BOX (gtk_event_box_new ());
    button->image = GTK_IMAGE (gtk_image_new ());

    gtk_widget_set_can_focus (GTK_WIDGET (button->eventbox), TRUE);

    gtk_container_add (GTK_CONTAINER (button->eventbox), GTK_WIDGET (button->image));
    gtk_event_box_set_visible_window (button->eventbox, FALSE);
    gtk_box_pack_start (GTK_BOX (wb->box), GTK_WIDGET (button->eventbox), TRUE, TRUE, 0);

    /* Add hover events to eventboxes */
    gtk_widget_add_events (GTK_WIDGET (button->eventbox), GDK_ENTER_NOTIFY_MASK); //add the "enter" signal
    gtk_widget_add_events (GTK_WIDGET (button->eventbox), GDK_LEAVE_NOTIFY_MASK); //add the "leave" signal

    return button;
}

static GtkOrientation get_orientation (XfcePanelPlugin *plugin)
{
    XfcePanelPluginMode mode;
    mode = xfce_panel_plugin_get_mode (plugin);
    if  (mode == XFCE_PANEL_PLUGIN_MODE_DESKBAR)
    return GTK_ORIENTATION_HORIZONTAL;
    else
    return xfce_panel_plugin_get_orientation (plugin);
}

static WBPlugin *
wckbuttons_new (XfcePanelPlugin *plugin)
{
    WBPlugin   *wb;
    GtkOrientation  orientation;

    /* allocate memory for the plugin structure */
    wb = g_slice_new0 (WBPlugin);

    /* pointer to plugin */
    wb->plugin = plugin;

    /* read the user settings */
    wb->prefs = wckbuttons_read (wb->plugin);

    /* get the current orientation */
    orientation = get_orientation (plugin);

    /* create some panel widgets */
    wb->ebox = gtk_event_box_new ();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(wb->ebox), FALSE);
    gtk_widget_set_name(wb->ebox, "XfceWckButtonsPlugin");

    wb->box = gtk_box_new (orientation, 2);
    gtk_box_set_homogeneous (GTK_BOX (wb->box), FALSE);

    /* create buttons */
    for (gint i = 0; i < BUTTONS; i++)
    {
        wb->button[i] = window_button_new (wb);
    }

    gtk_widget_show (wb->ebox);
    gtk_widget_show (wb->box);
    gtk_container_add (GTK_CONTAINER (wb->ebox), wb->box);

    return wb;
}


static void wckbuttons_free (XfcePanelPlugin *plugin, WBPlugin    *wb)
{
    GtkWidget *dialog;

    disconnect_wnck (wb->win);

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy (dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy (wb->box);

    /* cleanup the settings */
    if (G_LIKELY (wb->prefs->button_layout != NULL))
        g_free (wb->prefs->button_layout);

    /* free the plugin structure */
    g_slice_free(WckUtils, wb->win);
    g_slice_free(WBPreferences, wb->prefs);
    g_slice_free (WBPlugin, wb);
}


static void
wckbuttons_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            WBPlugin    *wb)
{
    /* change the orienation of the box */
    gtk_orientable_set_orientation (GTK_ORIENTABLE (wb->box), get_orientation (wb->plugin));
}


static gboolean
wckbuttons_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     WBPlugin    *wb)
{
    XfcePanelPluginMode mode;
    GtkOrientation orientation;

    /* get the orientation of the plugin */
    orientation = xfce_panel_plugin_get_orientation (plugin);
    mode = xfce_panel_plugin_get_mode (plugin);

    /* set the widget size */
    if (mode == XFCE_PANEL_PLUGIN_MODE_DESKBAR)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, -1);
    else if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
    else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

    /* we handled the orientation */
    return TRUE;
}

static WBImageButton
get_maximize_button_image (WBPlugin *wb)
{
    return (wb->win->controlwindow && wnck_window_is_maximized (wb->win->controlwindow)) ? IMAGE_UNMAXIMIZE : IMAGE_MAXIMIZE;
}

void on_wck_state_changed (WnckWindow *controlwindow, gpointer data)
{
    WBPlugin *wb = data;
    WBImageButton image_button = get_maximize_button_image (wb);
    WBImageState image_state;

    if (controlwindow && wnck_window_is_active (controlwindow))
        image_state = IMAGE_FOCUSED;
    else
        image_state = IMAGE_UNFOCUSED;

    /* update buttons images */
    gtk_image_set_from_pixbuf (wb->button[MINIMIZE_BUTTON]->image, wb->pixbufs[IMAGE_MINIMIZE][image_state]);
    gtk_image_set_from_pixbuf (wb->button[MAXIMIZE_BUTTON]->image, wb->pixbufs[image_button][image_state]);
    gtk_image_set_from_pixbuf (wb->button[CLOSE_BUTTON]->image, wb->pixbufs[IMAGE_CLOSE][image_state]);
}

void on_control_window_changed (WnckWindow *controlwindow, WnckWindow *previous, gpointer data)
{
    WBPlugin *wb = data;

    if (!controlwindow
        || (window_is_desktop (controlwindow)
            && !wb->prefs->show_on_desktop))
    {
        if (gtk_widget_get_visible(GTK_WIDGET(wb->box)))
            gtk_widget_hide(GTK_WIDGET(wb->box));
    }
    else
    {
        const gboolean is_desktop = window_is_desktop (controlwindow);

        gtk_widget_set_sensitive (GTK_WIDGET (wb->button[MINIMIZE_BUTTON]->eventbox), !is_desktop);
        gtk_widget_set_sensitive (GTK_WIDGET (wb->button[MAXIMIZE_BUTTON]->eventbox), !is_desktop);
        gtk_widget_set_sensitive (GTK_WIDGET (wb->button[CLOSE_BUTTON]->eventbox), TRUE);

        on_wck_state_changed (controlwindow, wb);

        if (!gtk_widget_get_visible(GTK_WIDGET(wb->box)))
            gtk_widget_show_all(GTK_WIDGET(wb->box));
    }
}


/* Called when we click on a button */
static gboolean
on_button_pressed (GdkEventButton *event, WBPlugin *wb, WBButton button, WBImageButton image_button)
{
    if (event->button != 1)
        return FALSE;

    gtk_image_set_from_pixbuf (wb->button[button]->image, wb->pixbufs[image_button][IMAGE_PRESSED]);

    return TRUE;
}


/* Makes the button 'glow' when the mouse enters it */
static gboolean
on_button_hover_enter (WBPlugin *wb, WBButton button, WBImageButton image_button)
{
    gtk_image_set_from_pixbuf (wb->button[button]->image, wb->pixbufs[image_button][IMAGE_PRELIGHT]);

    return TRUE;
}


/* Makes the button stop 'glowing' when the mouse leaves it */
static gboolean
on_button_hover_leave (WBPlugin *wb, WBButton button, WBImageButton image_button)
{
    WBImageState image_state = wnck_window_is_active (wb->win->controlwindow) ? IMAGE_FOCUSED : IMAGE_UNFOCUSED;

    gtk_image_set_from_pixbuf (wb->button[button]->image, wb->pixbufs[image_button][image_state]);

    return TRUE;
}


/* Called when we release the click on a button */
static gboolean on_minimize_button_release (GtkWidget *event_box,
                               GdkEventButton *event,
                               WBPlugin *wb)
{
    if (event->button != 1) return FALSE;

    wnck_window_minimize(wb->win->controlwindow);

    return TRUE;
}


/* Called when we click on a button */
static gboolean on_minimize_button_pressed (GtkWidget *event_box,
                             GdkEventButton *event,
                             WBPlugin *wb)
{
    return on_button_pressed (event, wb, MINIMIZE_BUTTON, IMAGE_MINIMIZE);
}


/* Makes the button stop 'glowing' when the mouse leaves it */
static gboolean on_minimize_button_hover_leave (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    return on_button_hover_leave (wb, MINIMIZE_BUTTON, IMAGE_MINIMIZE);
}


/* Makes the button 'glow' when the mouse enters it */
static gboolean on_minimize_button_hover_enter (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    return on_button_hover_enter (wb, MINIMIZE_BUTTON, IMAGE_MINIMIZE);
}


/* Called when we release the click on a button */
static gboolean on_maximize_button_release (GtkWidget *event_box,
                               GdkEventButton *event,
                               WBPlugin *wb)
{
    if (event->button != 1) return FALSE;

    toggle_maximize(wb->win->controlwindow);

    return TRUE;
}


/* Called when we click on a button */
static gboolean on_maximize_button_pressed (GtkWidget *event_box,
                             GdkEventButton *event,
                             WBPlugin *wb)
{
    WBImageButton image_button = get_maximize_button_image (wb);

    return on_button_pressed (event, wb, MAXIMIZE_BUTTON, image_button);
}


/* Makes the button stop 'glowing' when the mouse leaves it */
static gboolean on_maximize_button_hover_leave (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    WBImageButton image_button = get_maximize_button_image (wb);

    return on_button_hover_leave (wb, MAXIMIZE_BUTTON, image_button);
}


/* Makes the button 'glow' when the mouse enters it */
static gboolean on_maximize_button_hover_enter (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    WBImageButton image_button = get_maximize_button_image (wb);

    return on_button_hover_enter (wb, MAXIMIZE_BUTTON, image_button);
}


/* Called when we release the click on a button */
static gboolean on_close_button_release (GtkWidget *event_box,
                               GdkEventButton *event,
                               WBPlugin *wb)
{
    if (event->button != 1) return FALSE;

    wnck_window_close(wb->win->controlwindow, GDK_CURRENT_TIME);

    return TRUE;
}


/* Called when we click on a button */
static gboolean on_close_button_pressed (GtkWidget *event_box,
                             GdkEventButton *event,
                             WBPlugin *wb)
{
    return on_button_pressed (event, wb, CLOSE_BUTTON, IMAGE_CLOSE);
}


/* Makes the button stop 'glowing' when the mouse leaves it */
static gboolean on_close_button_hover_leave (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    return on_button_hover_leave (wb, CLOSE_BUTTON, IMAGE_CLOSE);
}


/* Makes the button 'glow' when the mouse enters it */
static gboolean on_close_button_hover_enter (GtkWidget *widget,
                         GdkEventCrossing *event,
                         WBPlugin *wb)
{
    return on_button_hover_enter (wb, CLOSE_BUTTON, IMAGE_CLOSE);
}


static void on_refresh_item_activated (GtkMenuItem *refresh, WBPlugin *wb)
{
    wb->prefs = wckbuttons_read (wb->plugin);
    init_theme(wb);
    reload_wnck (wb->win, wb->prefs->only_maximized, wb);
}


static void
wckbuttons_construct (XfcePanelPlugin *plugin)
{
    WBPlugin *wb;
    GtkWidget *refresh;

    /* setup transation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* create the plugin */
    wb = wckbuttons_new (plugin);

    /* add the ebox to the panel */
    gtk_container_add (GTK_CONTAINER (plugin), wb->ebox);

    /* show the panel's right-click menu on this ebox */
    xfce_panel_plugin_add_action_widget (plugin, wb->ebox);

    /* connect plugin signals */
    g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (wckbuttons_free), wb);

    g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (wckbuttons_save), wb);

    g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (wckbuttons_size_changed), wb);

    g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (wckbuttons_orientation_changed), wb);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (wckbuttons_configure), wb);

    /* show the about menu item and connect signal */
    xfce_panel_plugin_menu_show_about (plugin);
    g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (wck_about), WCKBUTTONS_ICON);


    /* add custom menu items */
    refresh = show_refresh_item (plugin);
    g_signal_connect (G_OBJECT (refresh), "activate", G_CALLBACK (on_refresh_item_activated), wb);

    /* start tracking windows */
    wb->win = g_slice_new0 (WckUtils);
    init_wnck(wb->win, wb->prefs->only_maximized, wb);

    /* get theme */
    init_theme(wb);

    /* start tracking buttons events*/
    BUTTONS_SIGNALS_CONNECT(on_minimize, MINIMIZE_BUTTON);
    BUTTONS_SIGNALS_CONNECT(on_maximize, MAXIMIZE_BUTTON);
    BUTTONS_SIGNALS_CONNECT(on_close, CLOSE_BUTTON);
}


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (wckbuttons_construct);
