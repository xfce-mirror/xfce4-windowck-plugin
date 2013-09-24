/*
 *  Copyright (C) 2013 Alessio Piccoli <alepic@geckoblu.net>
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
 */

/*
 * This code is derived from original 'Window Applets' code of Andrej Belcijan.
 * See http://gnome-look.org/content/show.php?content=103732 for details.
 */

#ifndef WCK_UTILS_H_
#define WCK_UTILS_H_

#ifndef WNCK_I_KNOW_THIS_IS_UNSTABLE
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <libwnck/libwnck.h>
#include <libxfce4panel/xfce-panel-plugin.h>

G_BEGIN_DECLS

/* Wnck structure */
typedef struct {
    WnckScreen *activescreen;          // Active screen
    WnckWorkspace *activeworkspace;    // Active workspace
    WnckWindow *controlwindow;          // Controled window according to only_maximized option
    WnckWindow *activewindow;          // Active window

    gulong csh;                         // controled window state handler id
    gulong ash;                         // active state handler id
    gulong awh;                         // active workspace handler id
    gulong sch;                         // window closed handler id
    gulong soh;                         // window opened handler id
    gulong svh;                         // viewport changed handler id
    gulong swh;                         // workspace changed handler id

    gboolean only_maximized;           // [T/F] Only track maximized windows

    gpointer data;
} WckUtils;

void initWnck (WckUtils *win, gboolean only_maximized, gpointer data);
void toggleMaximize (WckUtils *win);

G_END_DECLS

#endif /* WCK_UTILS_H_ */