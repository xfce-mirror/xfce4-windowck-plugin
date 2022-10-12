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

#include <common/wck-plugin.h>

#include "windowck.h"
#include "windowck-title.h"
#include "windowck-dialogs.h"
#include "windowck-dialogs_ui.h"

#define TITLE_SIZE_MIN 3


static void on_only_maximized_toggled(GtkRadioButton *only_maximized, WindowckPlugin *wckp)
{
    wckp->prefs->only_maximized = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(only_maximized));
    reload_wnck_title (wckp);
}


static void on_show_on_desktop_toggled(GtkToggleButton *show_on_desktop, WindowckPlugin *wckp)
{
    wckp->prefs->show_on_desktop = gtk_toggle_button_get_active(show_on_desktop);

    reload_wnck_title (wckp);
}


static void on_titlesize_changed(GtkSpinButton *titlesize, WindowckPlugin *wckp)
{
    wckp->prefs->title_size = gtk_spin_button_get_value(titlesize);
    resize_title(wckp);
}


static void
set_titlesize_sensitive (const WindowckPlugin *wckp, gboolean sensitive)
{
    GtkWidget *titlesize;
    GtkWidget *width_unit;

    titlesize = GTK_WIDGET (gtk_builder_get_object (wckp->prefs->builder, "titlesize"));
    if (G_LIKELY (titlesize != NULL))
    {
        gtk_widget_set_sensitive (titlesize, sensitive);
    }

    width_unit = GTK_WIDGET (gtk_builder_get_object (wckp->prefs->builder, "width_unit"));
    if (G_LIKELY (width_unit != NULL))
    {
        gtk_widget_set_sensitive (width_unit, sensitive);
    }
}

static void on_size_mode_changed (GtkComboBox *size_mode, WindowckPlugin *wckp)
{
    gint id;

    id = gtk_combo_box_get_active (size_mode);

    if (id < 0 || id > 2)
    {
      g_critical ("Trying to set a default size but got an invalid item");
      return;
    }

    if (id == 0)
    {
        wckp->prefs->size_mode = SHRINK;
    }
    else if (id == 1)
    {
        wckp->prefs->size_mode = FIXED;
    }
    else if (id == 2)
    {
        wckp->prefs->size_mode = EXPAND;
    }

    xfce_panel_plugin_set_shrink (wckp->plugin, wckp->prefs->size_mode != SHRINK);
    set_titlesize_sensitive (wckp, wckp->prefs->size_mode != EXPAND);

    /* dynamic resizing */
    /* don't work for title shrinking -> need to restart the applet */
    // TODO: make it working...
    resize_title(wckp);
}


static void on_full_name_toggled(GtkToggleButton *full_name, WindowckPlugin *wckp)
{
    wckp->prefs->full_name = gtk_toggle_button_get_active(full_name);
    on_wck_state_changed (wckp->win->controlwindow, wckp);
}


static void on_two_lines_toggled(GtkToggleButton *two_lines, WindowckPlugin *wckp)
{
    GtkWidget *sync_wm_font, *subtitle_font, *subtitle_font_label;

    sync_wm_font = GTK_WIDGET(gtk_builder_get_object(wckp->prefs->builder, "sync_wm_font"));
    subtitle_font = GTK_WIDGET(gtk_builder_get_object(wckp->prefs->builder, "subtitle_font"));
    subtitle_font_label = GTK_WIDGET(gtk_builder_get_object(wckp->prefs->builder, "subtitle_font_label"));
    wckp->prefs->two_lines = gtk_toggle_button_get_active(two_lines);

    on_wck_state_changed (wckp->win->controlwindow, wckp);

    if (wckp->prefs->two_lines)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sync_wm_font), FALSE);
    }

    gtk_widget_set_sensitive (subtitle_font, wckp->prefs->two_lines);
    gtk_widget_set_sensitive (subtitle_font_label, wckp->prefs->two_lines);
    gtk_widget_set_sensitive (sync_wm_font, !wckp->prefs->two_lines);
}


static void on_sync_wm_font_toggled(GtkToggleButton *sync_wm_font, WindowckPlugin *wckp)
{
    GtkFontButton *title_font;

    wckp->prefs->sync_wm_font = gtk_toggle_button_get_active (sync_wm_font);
    init_title (wckp);
    title_font = GTK_FONT_BUTTON(gtk_builder_get_object(wckp->prefs->builder, "title_font"));
    gtk_font_chooser_set_font (GTK_FONT_CHOOSER (title_font), wckp->prefs->title_font);
}


static gchar *
set_title_font (GtkLabel *title, GtkFontButton *font_button)
{
    gchar *font = g_strdup (gtk_font_chooser_get_font (GTK_FONT_CHOOSER (font_button)));
    PangoFontDescription *font_desc = pango_font_description_from_string (font);
    PangoAttribute *attr = pango_attr_font_desc_new (font_desc);
    PangoAttrList *attr_list = pango_attr_list_new ();

    pango_attr_list_insert (attr_list, attr);
    gtk_label_set_attributes (title, attr_list);

    pango_attr_list_unref (attr_list);
    pango_font_description_free (font_desc);
    return font;
}

static void on_title_font_set(GtkFontButton *title_font, WindowckPlugin *wckp)
{
    g_free (wckp->prefs->title_font);
    wckp->prefs->title_font = set_title_font (wckp->title, title_font);

    if (wckp->prefs->sync_wm_font)
        xfconf_channel_set_string (wckp->wm_channel, "/general/title_font", wckp->prefs->title_font);
}


static void on_subtitle_font_set(GtkFontButton *subtitle_font, WindowckPlugin *wckp)
{
    g_free (wckp->prefs->subtitle_font);
    wckp->prefs->subtitle_font = set_title_font (wckp->title, subtitle_font);
}


static void on_title_alignment_changed (GtkComboBox *title_alignment, WindowckPlugin *wckp)
{
    gint id;

    id = gtk_combo_box_get_active(title_alignment);

    if (id < 0 || id > 2)
    {
        g_critical ("Trying to set a default size but got an invalid item");
        return;
    }

    if (id == 0)
    {
        wckp->prefs->title_alignment = LEFT;
    }
    else if (id == 1)
    {
        wckp->prefs->title_alignment = CENTER;
    }
    else if (id == 2)
    {
        wckp->prefs->title_alignment = RIGHT;
    }

    gtk_label_set_xalign (wckp->title, wckp->prefs->title_alignment / 10.0);
    gtk_label_set_yalign (wckp->title, 0.5);
    on_wck_state_changed (wckp->win->controlwindow, wckp);
}


static void on_title_padding_changed(GtkSpinButton *title_padding, WindowckPlugin *wckp)
{
    wckp->prefs->title_padding = gtk_spin_button_get_value(title_padding);
    set_title_padding (wckp);
}


static GtkWidget * build_properties_area(WindowckPlugin *wckp, const gchar *buffer, gsize length)
{
    GError *error = NULL;

    if (wckp->prefs->builder)
        g_object_unref(G_OBJECT (wckp->prefs->builder));

    wckp->prefs->builder = gtk_builder_new();

    if (gtk_builder_add_from_string(wckp->prefs->builder, buffer, length, &error)) {
        GObject *area = gtk_builder_get_object(wckp->prefs->builder, "vbox0");

        if (G_LIKELY (area != NULL))
        {
            GtkSpinButton *titlesize, *title_padding;
            GtkComboBox *size_mode, *title_alignment;
            GtkToggleButton *sync_wm_font;
            GtkRadioButton *only_maximized, *active_window;
            GtkToggleButton *show_on_desktop, *full_name, *two_lines;
            GtkFontButton *title_font, *subtitle_font;
            GtkWidget *subtitle_font_label;

            only_maximized = GTK_RADIO_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "only_maximized"));
            active_window = GTK_RADIO_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "active_window"));
            if (G_LIKELY (only_maximized != NULL && active_window != NULL))
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(only_maximized), wckp->prefs->only_maximized);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_window), !wckp->prefs->only_maximized);
                g_signal_connect(only_maximized, "toggled", G_CALLBACK(on_only_maximized_toggled), wckp);
            }

            show_on_desktop = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "show_on_desktop"));
            if (G_LIKELY (show_on_desktop != NULL))
            {
                gtk_toggle_button_set_active(show_on_desktop, wckp->prefs->show_on_desktop);
                g_signal_connect(show_on_desktop, "toggled", G_CALLBACK(on_show_on_desktop_toggled), wckp);
            }

            full_name = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "full_name"));
            if (G_LIKELY (full_name != NULL))
            {
                gtk_toggle_button_set_active(full_name, wckp->prefs->full_name);
                g_signal_connect(full_name, "toggled", G_CALLBACK(on_full_name_toggled), wckp);
            }

            two_lines = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "two_lines"));
            if (G_LIKELY (two_lines != NULL))
            {
                gtk_toggle_button_set_active(two_lines, wckp->prefs->two_lines);
                g_signal_connect(two_lines, "toggled", G_CALLBACK(on_two_lines_toggled), wckp);
            }

            titlesize = GTK_SPIN_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "titlesize"));
            if (G_LIKELY (titlesize != NULL))
            {
                gtk_spin_button_set_range(titlesize, TITLE_SIZE_MIN, TITLE_SIZE_MAX);
                gtk_spin_button_set_increments(titlesize, 1, 1);
                gtk_spin_button_set_value(titlesize, wckp->prefs->title_size);
                g_signal_connect(titlesize, "value-changed", G_CALLBACK(on_titlesize_changed), wckp);
            }

            sync_wm_font = GTK_TOGGLE_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "sync_wm_font"));
            if (G_LIKELY (sync_wm_font != NULL))
            {
                if (wckp->wm_channel)
                {
                    gtk_toggle_button_set_active(sync_wm_font, wckp->prefs->sync_wm_font);
                    g_signal_connect(sync_wm_font, "toggled", G_CALLBACK(on_sync_wm_font_toggled), wckp);
                }
                else {
                    gtk_widget_set_sensitive (GTK_WIDGET(sync_wm_font), FALSE);
                }
            }

            title_font = GTK_FONT_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "title_font"));
            if (G_LIKELY (title_font != NULL))
            {
                gtk_font_chooser_set_font (GTK_FONT_CHOOSER (title_font), wckp->prefs->title_font);
                g_signal_connect(title_font, "font-set", G_CALLBACK(on_title_font_set), wckp);
            }

            subtitle_font = GTK_FONT_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "subtitle_font"));
            subtitle_font_label = wck_dialog_get_widget (wckp->prefs->builder, "subtitle_font_label");
            if (G_LIKELY (subtitle_font != NULL && subtitle_font_label != NULL))
            {
                gtk_font_chooser_set_font (GTK_FONT_CHOOSER (subtitle_font), wckp->prefs->subtitle_font);
                gtk_widget_set_sensitive (GTK_WIDGET(subtitle_font), wckp->prefs->two_lines);
                gtk_widget_set_sensitive (subtitle_font_label, wckp->prefs->two_lines);
                g_signal_connect(subtitle_font, "font-set", G_CALLBACK(on_subtitle_font_set), wckp);
            }

            title_alignment = GTK_COMBO_BOX (wck_dialog_get_widget (wckp->prefs->builder, "title_alignment"));
            if (G_LIKELY (title_alignment != NULL))
            {
                /* set active item */
                if ( wckp->prefs->title_alignment == LEFT ) {
                    gtk_combo_box_set_active(title_alignment, 0);
                }
                else if( wckp->prefs->title_alignment == CENTER ) {
                    gtk_combo_box_set_active(title_alignment, 1);
                }
                else if( wckp->prefs->title_alignment == RIGHT ) {
                    gtk_combo_box_set_active(title_alignment, 2);
                }
                g_signal_connect(title_alignment, "changed", G_CALLBACK(on_title_alignment_changed), wckp);
            }

            title_padding = GTK_SPIN_BUTTON (wck_dialog_get_widget (wckp->prefs->builder, "title_padding"));
            if (G_LIKELY (title_padding != NULL))
            {
                gtk_spin_button_set_range(title_padding, 0, 99);
                gtk_spin_button_set_increments(title_padding, 1, 1);
                gtk_spin_button_set_value(title_padding, wckp->prefs->title_padding);
                g_signal_connect(title_padding, "value-changed", G_CALLBACK(on_title_padding_changed), wckp);
            }

            size_mode = GTK_COMBO_BOX (wck_dialog_get_widget (wckp->prefs->builder, "size_mode"));
            if (G_LIKELY (size_mode != NULL))
            {
                /* set active item */
                if ( wckp->prefs->size_mode == SHRINK ) {
                    gtk_combo_box_set_active(size_mode, 0);
                }
                else if( wckp->prefs->size_mode == FIXED ) {
                    gtk_combo_box_set_active(size_mode, 1);
                }
                else if( wckp->prefs->size_mode == EXPAND ) {
                    gtk_combo_box_set_active(size_mode, 2);

                    set_titlesize_sensitive (wckp, FALSE);
                }

                g_signal_connect(size_mode, "changed", G_CALLBACK(on_size_mode_changed), wckp);
            }

            return GTK_WIDGET(area);
        }
        else {
            g_set_error_literal (&error, 0, 0, "No widget with the name \"vbox0\" found");
        }
    }

    g_critical("Failed to construct the builder for plugin %s-%d: %s.", xfce_panel_plugin_get_name (wckp->plugin), xfce_panel_plugin_get_unique_id (wckp->plugin), error->message);
    g_error_free(error);
    g_object_unref (G_OBJECT (wckp->prefs->builder));

    return NULL;
}


static void
windowck_configure_response(GtkWidget *dialog, gint response, WindowckPlugin *wckp)
{
    wck_configure_response (wckp->plugin, dialog, response, (WckSettingsCb) wcktitle_settings_save, wckp->prefs);
}


void windowck_configure(XfcePanelPlugin *plugin, WindowckPlugin *wckp)
{
    GtkWidget *ca;

    ca = build_properties_area (wckp, windowck_dialogs_ui, windowck_dialogs_ui_length);

    wck_configure_dialog (plugin, WCKTITLE_ICON, ca, G_CALLBACK(windowck_configure_response), wckp);
}
