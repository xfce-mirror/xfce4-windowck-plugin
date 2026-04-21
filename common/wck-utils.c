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
 *  This code is derived from original 'Window Applets' from Andrej Belcijan.
 */

#include "wck-utils.h"

/* Prototypes */
static XfwWindow *get_root_window(XfwScreen *);
static XfwWindow *get_upper_maximized(WckUtils *);
static XfwMonitor *get_plugin_monitor(XfcePanelPlugin *);
static XfwWorkspaceGroup *get_workspace_group(XfwScreen *, XfwMonitor *, gboolean);
static void track_controlled_window (WckUtils *);
static void active_workspace_changed(XfwWorkspaceGroup *, XfwWorkspace *, WckUtils *);
static void active_window_changed(XfwScreen *, XfwWindow *, WckUtils *);
static void track_changed_max_state(XfwWindow *, XfwWindowState, XfwWindowState, WckUtils *);
static void on_umaxed_window_state_changed(XfwWindow *, XfwWindowState, XfwWindowState, WckUtils *);
static void on_viewports_changed(XfwWorkspaceGroup *, WckUtils *);
static void on_window_closed(XfwScreen *, XfwWindow *, WckUtils *);
static void on_window_opened(XfwScreen *, XfwWindow *, WckUtils *);


gboolean wck_signal_handler_disconnect (GObject *object, gulong handler)
{
    if (object && handler > 0)
    {
        if (g_signal_handler_is_connected(object, handler))
        {
            g_signal_handler_disconnect(object, handler);
            return TRUE;
        }
    }
    return FALSE;
}


static XfwWindow *get_root_window (XfwScreen *screen)
{
    GList *winstack = xfw_screen_get_windows_stacked(screen);
    // we can't access data directly because sometimes we will get NULL or not desktop window
    if (winstack && window_is_desktop (winstack->data))
        return winstack->data;
    else
        return NULL;
}


/* Trigger when activewindow's workspaces changes */
static void umax_window_workspace_changed (XfwWindow *window,
                                           WckUtils *win)
{
        track_controlled_window (win);
}


/* Trigger when a specific window's state changes */
static void track_changed_max_state (XfwWindow *window,
                                         XfwWindowState changed_mask,
                                         XfwWindowState new_state,
                                         WckUtils *win)
{
    /* track the window max state only if it isn't the control window */
    if (window != win->controlwindow)
    {
        if (window
            && !xfw_window_is_minimized(window)
            && xfw_window_is_maximized(window))
        {
            track_controlled_window (win);
        }
    }
}

static XfwMonitor *get_plugin_monitor(XfcePanelPlugin *plugin)
{
    GdkMonitor *mon;

    if (plugin == NULL) return NULL;
    mon = gdk_display_get_monitor_at_window(gdk_display_get_default(),
                    gtk_widget_get_window(GTK_WIDGET(plugin)));
    if (mon == NULL) return NULL;

    GList* monitors = xfw_screen_get_monitors (xfw_screen_get_default());
    while (monitors && monitors->data)
    {
        if (xfw_monitor_get_gdk_monitor(monitors->data) == mon) return monitors->data;
        monitors = monitors->next;
    }
    return NULL;
}

/* Triggers when umaxedwindow's state changes */
static void on_umaxed_window_state_changed (XfwWindow *window,
                                          XfwWindowState changed_mask,
                                          XfwWindowState new_state,
                                          WckUtils *win)
{
    /* WARNING : only if window is unmaximized to prevent growing loop !!!*/
    if (!xfw_window_is_maximized(window)
        || xfw_window_is_minimized(window)
        || changed_mask & (XFW_WINDOW_STATE_ABOVE))
    {
        track_controlled_window (win);
    }
    else {
        on_wck_state_changed(win->controlwindow, win->data);
    }
}

static gboolean window_is_in_monitor(XfwWindow* win, XfwMonitor *m) {
    if (win == NULL || m == NULL)
        return FALSE;
    GList* monitors = xfw_window_get_monitors (win);
    return g_list_find (monitors, m) != NULL;
}

static XfwWorkspaceGroup *get_workspace_group(XfwScreen *screen, XfwMonitor *monitor, gboolean only_current_display)
{
    XfwWorkspaceManager* mgr = xfw_screen_get_workspace_manager(screen);

    GList *groups = xfw_workspace_manager_list_workspace_groups(mgr);
    while(groups) {
        GList *monitors = xfw_workspace_group_get_monitors(groups->data);

        if (g_list_find(monitors, monitor) || ! only_current_display)
            return groups->data;

        groups = groups->next;
    }
    return g_list_first(groups)->data;
}

/* Returns the highest maximized window */
static XfwWindow *get_upper_maximized (WckUtils *win)
{
    XfwWindow      *umaxedwindow = NULL;

    GList *windows = xfw_screen_get_windows_stacked(win->activescreen);

    while (windows && windows->data)
    {

        if ((!win->activeworkspace
                || xfw_window_is_in_viewport(windows->data, win->activeworkspace))
            && (!win->only_current_display
                || window_is_in_monitor(windows->data, win->monitor))
            && xfw_window_is_maximized(windows->data)
            && !xfw_window_is_minimized(windows->data))
        {
                umaxedwindow = windows->data;
        }
        windows = windows->next;
    }
    /* NULL if no maximized window found */
    return umaxedwindow;
}


/* track the new controlled window according to preferences */
static void track_controlled_window (WckUtils *win)
{
    XfwWindow      *previous_umax = NULL;
    XfwWindow      *previous_control = NULL;

    previous_control = win->controlwindow;

    if (win->only_maximized)
    {
        previous_umax = win->umaxwindow;
        win->umaxwindow = get_upper_maximized(win);
        win->controlwindow = win->umaxwindow;
    }
    else if (win->activewindow
            && (!win->activeworkspace
                || xfw_window_is_in_viewport(win->activewindow, win->activeworkspace))
            && !xfw_window_is_minimized(win->activewindow)
            && (window_is_desktop (win->activewindow)
                || !xfw_window_is_pinned(win->activewindow))
            && (!win->only_current_display
                || window_is_in_monitor(win->activewindow, win->monitor)))
    {
            win->controlwindow = win->activewindow;
    }

    if (!win->umaxwindow || (win->umaxwindow != previous_umax))
    {
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->msh);
        wck_signal_handler_disconnect (G_OBJECT(previous_umax), win->mwh);
    }

    if (win->only_maximized)
    {
        if (win->umaxwindow && (win->umaxwindow != previous_umax))
        {
            /* track the new upper maximized window state */
            win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                           "state-changed",
                                           G_CALLBACK (on_umaxed_window_state_changed),
                                           win);
            win->mwh = g_signal_connect(G_OBJECT (win->umaxwindow),
                                        "workspace-changed",
                                        G_CALLBACK (umax_window_workspace_changed),
                                        win);
        }
        else if (win->controlwindow == previous_control)
        {
            /* track previous upper maximized window state on desktop */
            win->umaxwindow = previous_umax;
            if (win->umaxwindow) {
                win->msh = g_signal_connect(G_OBJECT(win->umaxwindow),
                                               "state-changed",
                                               G_CALLBACK (track_changed_max_state),
                                               win);
            }
        }
    }

    if (!win->controlwindow)
        win->controlwindow = get_root_window(win->activescreen);

    if (win->controlwindow != previous_control)
        on_control_window_changed(win->controlwindow, previous_control, win->data);
    else
        on_wck_state_changed(win->controlwindow, win->data);
}


/* Triggers when a new window has been opened */
static void on_window_opened (XfwScreen *screen,
                           XfwWindow *window,
                           WckUtils *win)
{
    // track new maximized window
    if (xfw_window_is_maximized(window))
        track_controlled_window (win);
}


/* Triggers when a window has been closed */
static void on_window_closed (XfwScreen *screen,
                           XfwWindow *window,
                           WckUtils *win)
{
    // track closed maximized window
    if (xfw_window_is_maximized(window))
        track_controlled_window (win);
}


/* Triggers when a new active window is selected */
static void active_window_changed (XfwScreen *screen,
                                   XfwWindow *previous,
                                   WckUtils *win)
{

    win->activewindow = xfw_screen_get_active_window(screen);

    if (win->activewindow
        && (win->activewindow != previous))
    {
        wck_signal_handler_disconnect (G_OBJECT(previous), win->ash);

        track_controlled_window (win);
    }

    if (win->activewindow
        && (win->activewindow != previous)
        && !window_is_desktop (win->activewindow))
    {
        /* Start tracking the new active window */
        win->ash = g_signal_connect(G_OBJECT (win->activewindow), "state-changed", G_CALLBACK (track_changed_max_state), win);
    }
}


/* Triggers when user changes viewports on Compiz */
// We ONLY need this for Compiz (Marco doesn't use viewports)
static void on_viewports_changed (XfwWorkspaceGroup *screen, WckUtils *win)
{
    reload_wnck (win, win->only_maximized, win->only_current_display, win->data);
}


/* Triggers when user changes workspace on Marco (?) */
static void active_workspace_changed (XfwWorkspaceGroup *screen,
                                      XfwWorkspace *previous,
                                      WckUtils *win)
{
    reload_wnck (win, win->only_maximized, win->only_current_display, win->data);
}


void toggle_maximize (XfwWindow *window)
{
    if (window)
        xfw_window_set_maximized(window, !xfw_window_is_maximized(window), NULL);
}


void reload_wnck (WckUtils *win, gboolean only_maximized, gboolean only_current_display, gpointer data)
{
    disconnect_wnck (win);

    init_wnck (win, only_maximized, only_current_display, data);
}


void init_wnck (WckUtils *win, gboolean only_maximized, gboolean only_current_display, gpointer data)
{
    /* save data */
    win->data = data;

    win->activescreen = xfw_screen_get_default ();
    win->monitor = get_plugin_monitor(win->get_plugin(win->data));
    win->workspaces = get_workspace_group(win->activescreen, win->monitor, only_current_display);
    win->activeworkspace = xfw_workspace_group_get_active_workspace(win->workspaces);
    win->activewindow = xfw_screen_get_active_window(win->activescreen);
    win->umaxwindow = NULL;
    win->controlwindow = NULL;
    win->only_maximized = only_maximized;
    win->only_current_display = only_current_display;

    /* Global window tracking */
    win->sah = g_signal_connect(win->activescreen, "active-window-changed", G_CALLBACK (active_window_changed), win);

    if (win->only_maximized)
    {
        win->sch = g_signal_connect(win->activescreen, "window-closed", G_CALLBACK (on_window_closed), win);
        win->soh = g_signal_connect(win->activescreen, "window-opened", G_CALLBACK (on_window_opened), win);
    }

    win->svh = g_signal_connect(win->workspaces, "viewports-changed", G_CALLBACK (on_viewports_changed), win);
    win->swh = g_signal_connect(win->workspaces, "active-workspace-changed", G_CALLBACK (active_workspace_changed), win);

    /* Get controlled window */
    track_controlled_window (win);

    if (!win->controlwindow)
        on_control_window_changed (NULL, NULL, win->data);
}


void disconnect_wnck (WckUtils *win)
{
    /* disconnect all signal handlers */
    if (win->controlwindow)
      g_signal_handlers_disconnect_by_data (win->controlwindow, win);
    if (win->activewindow)
      g_signal_handlers_disconnect_by_data (win->activewindow, win);
    if (win->umaxwindow)
      g_signal_handlers_disconnect_by_data (win->umaxwindow, win);

    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->sah);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->sch);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->soh);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->svh);
    wck_signal_handler_disconnect (G_OBJECT(win->activescreen), win->swh);
}
