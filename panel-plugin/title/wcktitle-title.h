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

#ifndef __WCKTITLE_TITLE_H__
#define __WCKTITLE_TITLE_H__

#include "wcktitle.h"

#define TITLE_SIZE_MAX 999 /* title size max for expand option in characters */

void init_title (WckTitlePlugin *wtp);
void resize_title (WckTitlePlugin *wtp);
void reload_wnck_title (WckTitlePlugin *wtp);
void set_title_padding (WckTitlePlugin *wtp);
void set_title_alignment (WckTitlePlugin *wtp);
gboolean on_title_pressed (GtkWidget *title, GdkEventButton *event, WckTitlePlugin *wtp);
gboolean on_title_released (GtkWidget *title, GdkEventButton *event, WckTitlePlugin *wtp);

#endif /* __WCKTITLE_TITLE_H__ */
