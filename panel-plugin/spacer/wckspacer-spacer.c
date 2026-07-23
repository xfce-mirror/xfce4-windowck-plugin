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
#include <libxfce4windowingui/libxfce4windowingui.h>

#include <common/ui_style.h>
#include <common/wck-plugin.h>
#include <common/wck-utils.h>

#include "wckspacer.h"
#include "wckspacer-spacer.h"

void reload_wnck_spacer (WckSpacerPlugin *wsp)
{
    /* disconect previous window title signal */
    wck_signal_handler_disconnect (G_OBJECT (wsp->win->controlwindow), wsp->cnh);

    reload_wnck (wsp->win, wsp->prefs->only_maximized, wsp->prefs->only_current_display);
}

static void
on_name_changed (XfwWindow *controlwindow, WckSpacerPlugin *wsp)
{
    if (! controlwindow || (window_is_desktop (controlwindow) && !wsp->prefs->show_on_desktop))
        gtk_widget_set_tooltip_text(GTK_WIDGET(wsp->plugin), NULL);
    else
        gtk_widget_set_tooltip_text(GTK_WIDGET(wsp->plugin), xfw_window_get_name (controlwindow));
}

void on_wck_state_changed (XfwWindow *controlwindow, gpointer data) {
    WckSpacerPlugin *wsp = data;

    on_name_changed(controlwindow, wsp);
}

void on_control_window_changed (XfwWindow *controlwindow, XfwWindow *previous, gpointer data)
{
    WckSpacerPlugin *wsp = data;

    /* disconect previous window title signal */
    if (wsp->controlwindow) {
        wck_signal_handler_disconnect (G_OBJECT(wsp->controlwindow), wsp->cnh);
        wsp->controlwindow = NULL;
    }
    if(wsp->menu && GTK_IS_MENU(wsp->menu)) {
        gtk_menu_detach(GTK_MENU(wsp->menu));
        g_clear_object(&wsp->menu);
    }

    on_name_changed(controlwindow, wsp);
    if (controlwindow && (! window_is_desktop (controlwindow) || wsp->prefs->show_on_desktop)) {
        wsp->menu = xfw_window_action_menu_new (controlwindow);
        gtk_menu_attach_to_widget (GTK_MENU(wsp->menu), GTK_WIDGET(wsp->plugin), NULL);
    }

    if (controlwindow && !window_is_desktop (controlwindow))
    {
        wsp->controlwindow = controlwindow;
        wsp->cnh = g_signal_connect (G_OBJECT (controlwindow), "name-changed",
                                     G_CALLBACK (on_name_changed), wsp);
    }
}


gboolean on_spacer_pressed(GtkWidget *widget, GdkEventButton *event, WckSpacerPlugin *wsp)
{
    XfwWindow *controlwindow = wsp->win->controlwindow;

    if (controlwindow && event->button == 1) {
        if(window_is_desktop (controlwindow) && !wsp->prefs->show_on_desktop)
            return FALSE;

        xfw_window_activate (controlwindow, NULL, GDK_CURRENT_TIME, NULL);
        return TRUE;
    }

    /* ctrl + right click is used to show plugin's menu */
    if (event->button == 3 && (event->state & GDK_CONTROL_MASK) != GDK_CONTROL_MASK) {
        if ((window_is_desktop (controlwindow) && !wsp->prefs->show_on_desktop)
                || ! (wsp->menu && GTK_IS_MENU(wsp->menu)))
            return FALSE;

        gtk_menu_popup_at_pointer (GTK_MENU (wsp->menu), (GdkEvent *) event);
        return TRUE;
    }

    return FALSE;
}

gboolean on_spacer_released (GtkWidget *title, GdkEventButton *event, WckSpacerPlugin *wsp)
{
    if (!wsp->win->controlwindow
            || (window_is_desktop (wsp->win->controlwindow) && !wsp->prefs->show_on_desktop))
        return FALSE;

    if (event->button == 2)
    {
        /* middle-click */
        xfw_window_close(wsp->win->controlwindow, GDK_CURRENT_TIME, NULL);
        return TRUE;
    }

    return FALSE;
}

/* vim: ts=4:et */
