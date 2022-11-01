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

#ifndef __WINDOWCK_H__
#define __WINDOWCK_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>
#include <xfconf/xfconf.h>

#include <common/wck-utils.h>

#define WCKTITLE_ICON "windowck-plugin"

G_BEGIN_DECLS

typedef enum Alignment
{
    LEFT = 0, CENTER = 5, RIGHT = 10
} Alignment;

typedef enum SizeMode
{
    SHRINK = 0,
    FIXED = 1,
    EXPAND = 2
} SizeMode;

typedef struct {
    GtkBuilder *builder;
    gboolean only_maximized;        // [T/F] Only track maximized windows
    gboolean show_on_desktop;       // [T/F] Show the plugin on desktop
    gboolean full_name;             // [T/F] Show full name
    gboolean two_lines;             // [T/F] Display the title on two lines
    gboolean show_tooltips;         // [T/F] Show tooltips

    SizeMode size_mode;             // Size mode [SHRINK, FIXED, EXPAND]
    gint title_size;                // Title size in chars
    gint title_padding;             // Title padding in pixels
    Alignment title_alignment;      // Title alignment [LEFT, CENTER, RIGHT]

    gboolean sync_wm_font;          // [T/F] Try to use xfwm4 active theme if possible.
    gchar *title_font;              // Title font
    gchar *subtitle_font;           // Subtitle font
    gint inactive_text_alpha;       // Title inactive alpha
    gint inactive_text_shade;       // Title inactive shade
    gchar *active_text_color;       // active text color
    gchar *inactive_text_color;     // inactive text color
} WCKPreferences;

/* plugin structure */
typedef struct {
    XfcePanelPlugin *plugin;

    /* Widgets */
    GtkWidget *ebox;
    GtkWidget *box;
    GtkLabel *title;

    WCKPreferences     *prefs;
    WckUtils *win;

    gulong cnh;                     // controlled window name handler id

    XfconfChannel *wm_channel;      // window manager chanel
    XfconfChannel *x_channel;       // xsettings chanel
} WindowckPlugin;

void wcktitle_settings_save (XfceRc *rc, WCKPreferences *prefs);

G_END_DECLS

#endif /* !__WINDOWCK_H__ */
