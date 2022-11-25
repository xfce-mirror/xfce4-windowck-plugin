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
 *  Parts of this code is derived from xfwm4 sources (setting.c)
 *
 */

#include <libxfce4util/libxfce4util.h>

#include <common/wck-plugin.h>

#include "theme.h"
#include "wckbuttons-theme.h"


static GdkPixbuf *
pixbuf_alpha_load (const gchar *dir, const gchar *file)
{
    static const char* image_types[] = {
        "svg",
        "png",
        "gif",
        "jpg",
        "bmp",
        NULL };
    GdkPixbuf *pixbuf = NULL;
    int i = 0;

    while (!pixbuf && image_types[i])
    {
        gchar *image_file = g_strdup_printf ("%s.%s", file, image_types[i]);
        gchar *filepath = g_build_filename (dir, image_file, NULL);

        g_free (image_file);

        if (g_file_test (filepath, G_FILE_TEST_IS_REGULAR))
        {
            pixbuf = gdk_pixbuf_new_from_file (filepath, NULL);
        }
        g_free (filepath);

        ++i;
    }

    return pixbuf;
}


static void get_unity_pixbuf (const gchar *themedir, WckButtonsPlugin *wbp) {
    gchar imagename[40];

    static const char *button_names[] = {
      "minimize",
      "unmaximize",
      "maximize",
      "close"
    };

    static const char *button_state_names[] = {
      "unfocused",
      "focused_normal",
      "focused_prelight",
      "focused_pressed"
    };

    for (gint i = 0; i < IMAGES_BUTTONS; i++)
    {
        for (gint j = 0; j < IMAGES_STATES; j++)
        {
            g_snprintf(imagename, sizeof (imagename), "%s_%s", button_names[i], button_state_names[j]);
            wbp->pixbufs[i][j] = pixbuf_alpha_load (themedir, imagename);
        }
    }
}


static gboolean
is_layout_button (gchar c)
{
    return c == 'H' || c == 'M' || c == 'C';
}


gchar *button_layout_filter (const gchar *string, const gchar *default_layout)
{
    const size_t n = strlen (string);
    guint j = 0;
    gchar layout[BUTTONS+1] = {0};

    /* WARNING: beware of buffer overflow !!! */
    for (size_t i = 0; i < n && j < BUTTONS; i++)
    {
        if (is_layout_button (string[i]))
            layout[j++] = string[i];
    }

    layout[j] = '\0';

    if (layout[0] == '\0')
        return g_strdup (default_layout);

    return g_strdup (layout);
}


gchar *opposite_layout_filter (const gchar *string)
{
    const size_t n = strlen (string);
    size_t j = 0;
    gchar layout[n+1];

    for (size_t i = 0; i < n; i++)
    {
        if (!is_layout_button (string[i]))
            layout[j++] = string[i];
    }

    g_assert (j < n+1);
    layout[j] = '\0';

    return g_strdup (layout);
}


static int get_button_from_letter (char chr)
{

    TRACE ("entering get_button_from_letter");

    switch (chr)
    {
        case 'H':
                return MINIMIZE_BUTTON;
        case 'M':
                return MAXIMIZE_BUTTON;
        case 'C':
                return CLOSE_BUTTON;
        default:
            return -1;
    }
}


/* Replace buttons accordingly to button_layout and visible state */
void replace_buttons (const gchar *button_layout, WckButtonsPlugin *wbp)
{
    const size_t n = strlen (button_layout);
    guint j = 0;

    for (guint i = 0; i < BUTTONS; i++)
        gtk_widget_hide (GTK_WIDGET (wbp->button[i]->eventbox));

    for (guint i = 0; i < n; i++)
    {
        gint button = get_button_from_letter (button_layout[i]);
        if (button >= 0 && wbp->button[button]->image)
        {
            gtk_box_reorder_child (GTK_BOX (wbp->box), GTK_WIDGET(wbp->button[button]->eventbox), j);

            gtk_widget_show_all (GTK_WIDGET (wbp->button[button]->eventbox));
            j++;
        }
    }
}


gchar *get_rc_button_layout (const gchar *theme)
{
    gchar *wm_themedir;

    wm_themedir = test_theme_dir(theme, "xfwm4", THEMERC);

    if (G_LIKELY(wm_themedir))
    {
        gchar  *filename;
        XfceRc *rc;

        /* check in the rc if the theme supports a custom button layout */
        filename = g_build_filename (wm_themedir, THEMERC, NULL);
        g_free (wm_themedir);

        rc = xfce_rc_simple_open (filename, TRUE);
        g_free (filename);

        if (G_LIKELY (rc))
        {
            const gchar *rc_button_layout = xfce_rc_read_entry (rc, "button_layout", NULL);

            xfce_rc_close (rc);

            if (rc_button_layout)
                return button_layout_filter (rc_button_layout, NULL);
        }
    }

    return NULL;
}


/* load the theme according to xfwm4 theme format */
void load_theme (const gchar *theme, WckButtonsPlugin *wbp)
{
    gchar *themedir;

    /* get theme dir */
    themedir = get_unity_theme_dir (theme, DEFAULT_THEME);

    if (themedir)
    {
        get_unity_pixbuf (themedir, wbp);
        g_free (themedir);

        /* try to replace missing images */

        for (gint i = 0; i < IMAGES_STATES; i++)
        {
            if (!wbp->pixbufs[IMAGE_UNMAXIMIZE][i])
                wbp->pixbufs[IMAGE_UNMAXIMIZE][i] = wbp->pixbufs[IMAGE_MAXIMIZE][i];
        }

        for (gint i = 0; i < IMAGES_BUTTONS; i++)
        {
            if (!wbp->pixbufs[i][IMAGE_UNFOCUSED] || !wbp->pixbufs[i][IMAGE_PRESSED])
                wbp->pixbufs[i][IMAGE_UNFOCUSED] = wbp->pixbufs[i][IMAGE_FOCUSED];

            if (!wbp->pixbufs[i][IMAGE_PRELIGHT])
                wbp->pixbufs[i][IMAGE_PRELIGHT] = wbp->pixbufs[i][IMAGE_PRESSED];
        }
    }
}


static void apply_wm_theme (WckButtonsPlugin *wbp)
{
    const gchar *wm_theme = xfconf_channel_get_string (wbp->wm_channel, "/general/theme", NULL);

    if (G_LIKELY (wm_theme))
    {
        gchar *button_layout;

        wbp->prefs->theme = g_strdup (wm_theme);
        load_theme (wbp->prefs->theme, wbp);

        button_layout = get_rc_button_layout (wm_theme);

        if (button_layout)
        {
            replace_buttons (button_layout, wbp);
        }
        else
        {
            const gchar *wm_buttons_layout = xfconf_channel_get_string (wbp->wm_channel,
                                                                        "/general/button_layout",
                                                                        wbp->prefs->button_layout);
            wbp->prefs->button_layout = button_layout_filter (wm_buttons_layout, wbp->prefs->button_layout);

            replace_buttons (wbp->prefs->button_layout, wbp);
        }
        g_free (button_layout);
    }

    on_wck_state_changed (wbp->win->controlwindow, wbp);
}


static void
on_x_chanel_property_changed (XfconfChannel *x_channel,
                              const gchar   *property_name,
                              const GValue  *value, WckButtonsPlugin *wbp)
{
    if (g_str_has_prefix (property_name, "/Net/") == TRUE)
    {
        const gchar *name = &property_name[5];
        switch (G_VALUE_TYPE(value))
        {
            case G_TYPE_STRING:
                if (!strcmp (name, "ThemeName"))
                {
                    apply_wm_theme (wbp);
                }
                break;
            default:
                g_warning ("The property '%s' is not supported", property_name);
                break;
        }
    }
}


static void
on_xfwm_channel_property_changed (XfconfChannel *wm_channel,
                                  const gchar   *property_name,
                                  const GValue  *value, WckButtonsPlugin *wbp)
{
    if (g_str_has_prefix (property_name, "/general/") == TRUE)
    {
        const gchar *name = &property_name[9];
        switch (G_VALUE_TYPE(value))
        {
            case G_TYPE_STRING:
                if (!strcmp (name, "theme")
                    || !strcmp (name, "button_layout"))
                {
                    apply_wm_theme (wbp);
                }
                break;
            default:
                g_warning ("The property '%s' is not supported", property_name);
                break;
        }
    }
}


void
init_theme (WckButtonsPlugin *wbp)
{
    /* get the xfwm4 chanel */
    wbp->wm_channel = wck_properties_get_channel (G_OBJECT (wbp->plugin), "xfwm4");

    /* try to use the xfwm4 theme */
    if (wbp->wm_channel && wbp->prefs->sync_wm_theme)
    {
        apply_wm_theme (wbp);

        g_signal_connect (wbp->wm_channel,
                          "property-changed",
                          G_CALLBACK (on_xfwm_channel_property_changed),
                          wbp);
    }
    else
    {
        load_theme (wbp->prefs->theme, wbp);
        replace_buttons (wbp->prefs->button_layout, wbp);
    }

    /* get the xsettings chanel to update the gtk theme */
    wbp->x_channel = wck_properties_get_channel (G_OBJECT (wbp->plugin), "xsettings");

    if (wbp->x_channel)
        g_signal_connect (wbp->x_channel,
                          "property-changed",
                          G_CALLBACK (on_x_chanel_property_changed),
                          wbp);
}
