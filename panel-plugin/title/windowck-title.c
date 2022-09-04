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

#include <libxfce4panel/libxfce4panel.h>

#include <common/wck-plugin.h>
#include <common/wck-utils.h>

#include "ui_style.h"
#include "windowck.h"
#include "windowck-title.h"

/* Prototypes */
static void on_name_changed(WnckWindow *window, WindowckPlugin *);


void reload_wnck_title (WindowckPlugin *wckp)
{
    /* disconnect controlled window name signal handler */
    wck_signal_handler_disconnect (G_OBJECT(wckp->win->controlwindow), wckp->cnh);

    reload_wnck (wckp->win, wckp->prefs->only_maximized, wckp);
}


static gboolean is_window_on_active_workspace_and_no_other_maximized_windows_above(WnckWindow *window)
{
    WnckWorkspace *workspace = wnck_window_get_workspace(window);
    WnckScreen *screen = wnck_workspace_get_screen(workspace);
    GList *windows;
    GList *top_window;
    GList *bottom_window;

    if (wnck_screen_get_active_workspace(screen) != workspace) {
        return FALSE;
    }

    windows = wnck_screen_get_windows_stacked(screen);
    top_window = g_list_last(windows);
    bottom_window = g_list_first(windows);
    while (top_window->data != window && top_window != bottom_window) {
        if (wnck_window_is_maximized((WnckWindow *)top_window->data)) {
            return FALSE;
        }
        top_window = top_window->prev;
    }
    return TRUE;
}


static gchar *
get_title_color (WnckWindow *controlwindow, WindowckPlugin *wckp)
{
    if (wnck_window_is_active (controlwindow))
    {
        /* window focused */
        /*~ gtk_widget_set_sensitive(GTK_WIDGET(wckp->title), TRUE); */
        return wckp->prefs->active_text_color;
    }

    if (is_window_on_active_workspace_and_no_other_maximized_windows_above (controlwindow))
    {
        /* window unfocused */
        /*~ gtk_widget_set_sensitive(GTK_WIDGET(wckp->title), FALSE); */
        return wckp->prefs->inactive_text_color;
    }

    return NULL;
}


/* Triggers when controlwindow's name changes */
/* Warning! This function is called very often, so it should only do the most necessary things! */
static void on_name_changed (WnckWindow *controlwindow, WindowckPlugin *wckp)
{
    const gchar *title_text;

    if (controlwindow
        && wnck_window_get_pid(controlwindow)  /* if active window has been closed, pid is 0 */
        && (!window_is_desktop (controlwindow)
            || wckp->prefs->show_on_desktop))
    {
        const gchar *title_color;
        gchar *title_markup;

        title_color = get_title_color (controlwindow, wckp);
        if (!title_color)
        {
            return;
        }

        title_text = wnck_window_get_name (controlwindow);

        /* Set tooltips */
        if (wckp->prefs->show_tooltips)
        {
            gtk_widget_set_tooltip_text(GTK_WIDGET(wckp->title), title_text);
        }

        /* get application and instance names */
        if (wckp->prefs->full_name && !wckp->prefs->two_lines)
        {
            title_markup = g_markup_printf_escaped("<span font=\"%s\" color=\"%s\">%s</span>", wckp->prefs->title_font, title_color, title_text);
        }
        else {
            /* split title text */
            gchar **part = g_strsplit (title_text, " - ", 0);
            const gint n = g_strv_length (part);
            gchar *title = g_strdup (part[n - 1]);

            if (n > 1 && wckp->prefs->two_lines)
            {
                gchar *subtitle;

                if (wckp->prefs->full_name)
                {
                    g_free (part[n - 1]);
                    part[n - 1] = NULL;
                    subtitle = g_strjoinv (" - ", part);
                }
                else
                {
                    subtitle = g_strdup (part[0]);
                }

                title_markup = g_markup_printf_escaped("<span font=\"%s\" color=\"%s\">%s</span><span font=\"%s\" color=\"%s\">\n%s</span>",
                                                       wckp->prefs->title_font, title_color, title, wckp->prefs->subtitle_font, title_color, subtitle);
                g_free (subtitle);
            }
            else
            {
                title_markup = g_markup_printf_escaped("<span font=\"%s\" color=\"%s\">%s</span>", wckp->prefs->title_font, title_color, title);
            }

            g_free (title);
            g_strfreev (part);
        }

        gtk_label_set_markup(wckp->title, title_markup);

        if (wckp->prefs->title_alignment == LEFT)
        {
            gtk_label_set_justify(wckp->title, GTK_JUSTIFY_LEFT);
        }
        else if (wckp->prefs->title_alignment == CENTER)
        {
            gtk_label_set_justify(wckp->title, GTK_JUSTIFY_CENTER);
        }
        else if (wckp->prefs->title_alignment == RIGHT)
        {
            gtk_label_set_justify(wckp->title, GTK_JUSTIFY_RIGHT);
        }

        g_free (title_markup);
    }
    else {
        /* hide text */
        title_text = "";
        gtk_label_set_text(wckp->title, title_text);
    }
}


void on_wck_state_changed (WnckWindow *controlwindow, gpointer data)
{
    WindowckPlugin *wckp = data;

    on_name_changed (controlwindow, wckp);
}


void on_control_window_changed (WnckWindow *controlwindow, WnckWindow *previous, gpointer data)
{
    WindowckPlugin *wckp = data;

    /* disconect previous window title signal */
    wck_signal_handler_disconnect (G_OBJECT(previous), wckp->cnh);

    on_wck_state_changed (controlwindow, wckp);

    if (!controlwindow
        || (window_is_desktop (controlwindow)
            && !wckp->prefs->show_on_desktop))
    {
        if (gtk_widget_get_visible(GTK_WIDGET(wckp->box)))
            gtk_widget_hide(GTK_WIDGET(wckp->box));
    }
    else
    {
        if (!gtk_widget_get_visible(GTK_WIDGET(wckp->box)))
            gtk_widget_show_all(GTK_WIDGET(wckp->box));
    }

    if (controlwindow && !window_is_desktop (controlwindow))
    {
        wckp->cnh = g_signal_connect(G_OBJECT(controlwindow), "name-changed", G_CALLBACK(on_name_changed), wckp);
    }
}


void resize_title(WindowckPlugin *wckp)
{
    switch (wckp->prefs->size_mode)
    {
        case SHRINK:
            gtk_label_set_max_width_chars(wckp->title, wckp->prefs->title_size);
            break;
        case EXPAND:
            gtk_label_set_width_chars(wckp->title, TITLE_SIZE_MAX);
            break;
        default:
            gtk_label_set_width_chars(wckp->title, wckp->prefs->title_size);
    }
}


void set_title_padding (WindowckPlugin *wckp)
{
    gtk_widget_set_margin_start (wckp->box, wckp->prefs->title_padding);
    gtk_widget_set_margin_end (wckp->box, wckp->prefs->title_padding);
    gtk_box_set_spacing (GTK_BOX (wckp->box), wckp->prefs->title_padding);
}


gboolean on_title_pressed(GtkWidget *title, GdkEventButton *event, WindowckPlugin *wckp)
{

    if (!wckp->win->controlwindow)
        return FALSE;

    if (event->button == 1
        && !window_is_desktop (wckp->win->controlwindow))
    {
        /* double/tripple click */
        if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
        {
            toggle_maximize(wckp->win->controlwindow);
        }
        else /* left-click */
        {
            wnck_window_activate(wckp->win->controlwindow, gtk_get_current_event_time());
        }
        return TRUE;
    }

    if (event->button == 3)
    {
        /* right-click */
        wnck_window_activate(wckp->win->controlwindow, gtk_get_current_event_time());

        /* let the panel show the menu */
        return TRUE;
    }

    return FALSE;
}


gboolean on_title_released(GtkWidget *title, GdkEventButton *event, WindowckPlugin *wckp)
{
    if (!wckp->win->controlwindow)
        return FALSE;

    if (event->button == 2)
    {
        /* middle-click */
        wnck_window_close(wckp->win->controlwindow, GDK_CURRENT_TIME);
        return TRUE;
    }

    return FALSE;
}


static void set_title_colors(WindowckPlugin *wckp)
{
    /* get plugin widget style */
    g_free (wckp->prefs->active_text_color);
    g_free (wckp->prefs->inactive_text_color);
    wckp->prefs->active_text_color = get_ui_color (GTK_WIDGET(wckp->plugin), GTK_STYLE_PROPERTY_COLOR, GTK_STATE_FLAG_NORMAL);
    wckp->prefs->inactive_text_color = mix_bg_fg (GTK_WIDGET(wckp->plugin), GTK_STATE_FLAG_NORMAL, wckp->prefs->inactive_text_alpha / 100.0, wckp->prefs->inactive_text_shade / 100.0);
}


static void apply_wm_settings (WindowckPlugin *wckp)
{
    gchar *wm_theme = xfconf_channel_get_string (wckp->wm_channel, "/general/theme", NULL);

    if (G_LIKELY(wm_theme))
    {
        gchar *wm_title_font = xfconf_channel_get_string (wckp->wm_channel, "/general/title_font", wckp->prefs->title_font);

        g_free (wckp->prefs->title_font);
        wckp->prefs->title_font = wm_title_font;

        on_name_changed (wckp->win->controlwindow, wckp);

        g_free (wm_theme);
    }
}


static void
on_x_chanel_property_changed (XfconfChannel *x_channel, const gchar *property_name, const GValue *value, WindowckPlugin *wckp)
{
    if (g_str_has_prefix(property_name, "/Net/") == TRUE)
    {
        const gchar *name = &property_name[5];
        switch (G_VALUE_TYPE(value))
        {
            case G_TYPE_STRING:
                if (!strcmp (name, "ThemeName"))
                {
                    set_title_colors(wckp);
                    on_name_changed (wckp->win->controlwindow, wckp);
                }
                break;
            default:
                g_warning("The property '%s' is not supported", property_name);
                break;
        }
    }
}


static void on_xfwm_channel_property_changed (XfconfChannel *wm_channel, const gchar *property_name, const GValue *value, WindowckPlugin *wckp)
{
    if (g_str_has_prefix(property_name, "/general/") == TRUE)
    {
        const gchar *name = &property_name[9];
        switch (G_VALUE_TYPE(value))
        {
            case G_TYPE_STRING:
                if (!strcmp (name, "title_font"))
                {
                    apply_wm_settings (wckp);
                }
                else if (!strcmp (name, "theme"))
                {
                    init_title(wckp);
                    reload_wnck_title (wckp);
                }
                break;
            default:
                g_warning("The property '%s' is not supported", property_name);
                break;
        }
    }
}


void init_title (WindowckPlugin *wckp)
{
    set_title_colors(wckp);
    resize_title(wckp);

    gtk_label_set_ellipsize(wckp->title, PANGO_ELLIPSIZE_END);

    if (wckp->prefs->size_mode != SHRINK)
    {
        gtk_label_set_xalign (wckp->title, wckp->prefs->title_alignment / 10.0);
        gtk_label_set_yalign (wckp->title, 0.5);
    }

    /* get the xfwm4 chanel */
    wckp->wm_channel = wck_properties_get_channel (G_OBJECT (wckp->plugin), "xfwm4");

    /* try to set title settings from the xfwm4 theme */
    if (wckp->wm_channel && wckp->prefs->sync_wm_font)
    {
        apply_wm_settings (wckp);
        g_signal_connect (wckp->wm_channel, "property-changed", G_CALLBACK (on_xfwm_channel_property_changed), wckp);
    }

    set_title_padding (wckp);

    /* get the xsettings chanel to update the gtk theme */
    wckp->x_channel = wck_properties_get_channel (G_OBJECT (wckp->plugin), "xsettings");

    if (wckp->x_channel)
        g_signal_connect (wckp->x_channel, "property-changed", G_CALLBACK (on_x_chanel_property_changed), wckp);
}
