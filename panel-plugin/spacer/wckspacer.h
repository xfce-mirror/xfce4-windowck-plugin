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

#ifndef __WCKSPACER_H__
#define __WCKSPACER_H__

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <xfconf/xfconf.h>

#include <common/wck-plugin.h>
#include <common/wck-utils.h>

#define WCKSPACER_ICON "wckspacer-plugin"

G_BEGIN_DECLS

typedef struct {
    WckConf *conf;
    GtkBuilder *builder;
    gboolean only_maximized;        // [T/F] Only track maximized windows
    gboolean show_on_desktop;       // [T/F] Show the plugin on desktop
    gboolean only_current_display;  // [T/F] Only control windows of current display
} WckSpacerPreferences;

/* plugin structure */
typedef struct {
    XfcePanelPlugin *plugin;

    WckSpacerPreferences     *prefs;
    WckUtils *win;

    XfwWindow *controlwindow;
    gulong    cnh;                  // controlled window name handler id
    GtkWidget *menu;
} WckSpacerPlugin;

void wckspacer_settings_save (WckSpacerPreferences *prefs);

G_END_DECLS

#endif /* !__WCKSPACER_H__ */
/* vim: ts=4:et */
