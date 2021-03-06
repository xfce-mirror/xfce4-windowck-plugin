/*      $Id$

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2, or (at your option)
        any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., Inc., 51 Franklin Street, Fifth Floor, Boston,
        MA 02110-1301, USA.


        Original XPM load routines from gdk-pixbuf:

        Copyright (C) 1999 Mark Crichton
        Copyright (C) 1999 The Free Software Foundation

        Authors: Mark Crichton <crichton@gimp.org>
                 Federico Mena-Quintero <federico@gimp.org>

        A specific version of the gdk-pixbuf routines are required to support
        XPM color substitution used by the themes to apply gtk+ colors.

        oroborus - (c) 2001 Ken Lynch
        xfwm4    - (c) 2002-2011 Olivier Fourdan
        xfwm4-windowck-plugin - (c) 2013 Cedric leporcq
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <libxfce4util/libxfce4util.h>
#include <stdlib.h>
#include <stdio.h>

#include "mypixmap.h"
#include "xpm-color-table.h"

enum buf_op
{
    op_header,
    op_cmap,
    op_body
};

struct file_handle
{
    FILE *infile;
    gchar *buffer;
    guint buffer_size;
};

/* The following 2 routines (parse_color, find_color) come from Tk, via the Win32
 * port of GDK. The licensing terms on these (longer than the functions) is:
 *
 * This software is copyrighted by the Regents of the University of
 * California, Sun Microsystems, Inc., and other parties.  The following
 * terms apply to all files associated with the software unless explicitly
 * disclaimed in individual files.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
 * DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
 * IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
 * NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * GOVERNMENT USE: If you are acquiring this software on behalf of the
 * U.S. government, the Government shall have only "Restricted Rights"
 * in the software and related documentation as defined in the Federal
 * Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
 * are acquiring the software on behalf of the Department of Defense, the
 * software shall be classified as "Commercial Computer Software" and the
 * Government shall have only "Restricted Rights" as defined in Clause
 * 252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
 * authors grant the U.S. Government and others acting in its behalf
 * permission to use and distribute the software in accordance with the
 * terms specified in this license.
 */

static int
compare_xcolor_entries (const void *a, const void *b)
{
    return g_ascii_strcasecmp ((const char *) a,
                               color_names + ((const XPMColorEntry *) b)->name_offset);
}

static gboolean
find_color(const char *name, XPMColor *colorPtr)
{
    XPMColorEntry *found;

    found = bsearch (name, xColors, G_N_ELEMENTS (xColors), sizeof (XPMColorEntry),
                     compare_xcolor_entries);
    if (found == NULL)
    {
        return FALSE;
    }

    colorPtr->red   = (found->red   * 0xFFFF) / 0xFF;
    colorPtr->green = (found->green * 0xFFFF) / 0xFF;
    colorPtr->blue  = (found->blue  * 0xFFFF) / 0xFF;

    return TRUE;
}

static gboolean
parse_color (const char *spec, XPMColor   *colorPtr)
{
    if (spec[0] == '#')
    {
        char fmt[16];
        int i, red, green, blue;

        if ((i = strlen (spec + 1)) % 3)
        {
                return FALSE;
        }
        i /= 3;

        g_snprintf (fmt, 16, "%%%dx%%%dx%%%dx", i, i, i);

        if (sscanf (spec + 1, fmt, &red, &green, &blue) != 3)
        {
            return FALSE;
        }
        if (i == 4)
        {
            colorPtr->red   = red;
            colorPtr->green = green;
            colorPtr->blue  = blue;
        }
        else if (i == 1)
        {
            colorPtr->red   = (red   * 0xFFFF) / 0xF;
            colorPtr->green = (green * 0xFFFF) / 0xF;
            colorPtr->blue  = (blue  * 0xFFFF) / 0xF;
        }
        else if (i == 2)
        {
            colorPtr->red   = (red   * 0xFFFF) / 0xFF;
            colorPtr->green = (green * 0xFFFF) / 0xFF;
            colorPtr->blue  = (blue  * 0xFFFF) / 0xFF;
        }
        else /* if (i == 3) */
        {
            colorPtr->red   = (red   * 0xFFFF) / 0xFFF;
            colorPtr->green = (green * 0xFFFF) / 0xFFF;
            colorPtr->blue  = (blue  * 0xFFFF) / 0xFFF;
        }
    }
    else
    {
        if (!find_color(spec, colorPtr))
        {
            return FALSE;
        }
    }
    return TRUE;
}

static gint
xpm_seek_string (FILE *infile, const gchar *str)
{
    char instr[1024];

    while (!feof (infile))
    {
        if (fscanf (infile, "%1023s", instr) < 0)
        {
                return FALSE;
        }
        if (strcmp (instr, str) == 0)
        {
                return TRUE;
        }
    }

    return FALSE;
}

static gint
xpm_seek_char (FILE *infile, gchar c)
{
    gint b, oldb;

    while ((b = getc (infile)) != EOF)
    {
        if (c != b && b == '/')
        {
            b = getc (infile);
            if (b == EOF)
            {
                return FALSE;
            }
            else if (b == '*')
            {   /* we have a comment */
                 b = -1;
                 do
                 {
                     oldb = b;
                     b = getc (infile);
                     if (b == EOF)
                     {
                             return FALSE;
                     }
                 }
                 while (!(oldb == '*' && b == '/'));
            }
        }
        else if (c == b)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static gint
xpm_read_string (FILE *infile, gchar **buffer, guint *buffer_size)
{
    gint c;
    guint cnt = 0, bufsiz, ret;
    gchar *buf;

    buf = *buffer;
    bufsiz = *buffer_size;
    ret = FALSE;

    if (buf == NULL)
    {
        bufsiz = 10 * sizeof (gchar);
        buf = g_new (gchar, bufsiz);
    }

    do
    {
        c = getc (infile);
    }
    while (c != EOF && c != '"');

    if (c != '"')
    {
        goto out;
    }
    while ((c = getc (infile)) != EOF)
    {
        if (cnt == bufsiz)
        {
            guint new_size = bufsiz * 2;

            if (new_size > bufsiz)
            {
                bufsiz = new_size;
            }
            else
            {
                goto out;
            }
            buf = g_realloc (buf, bufsiz);
            buf[bufsiz - 1] = '\0';
        }

        if (c != '"')
        {
            buf[cnt++] = c;
        }
        else
        {
            buf[cnt] = 0;
            ret = TRUE;
            break;
        }
    }

out:
    buf[bufsiz - 1] = '\0';     /* ensure null termination for errors */
    *buffer = buf;
    *buffer_size = bufsiz;
    return ret;
}

static const gchar *
search_color_symbol (gchar *symbol, xfwmColorSymbol *color_sym)
{
    xfwmColorSymbol *i;

    i = color_sym;
    while (i && i->name)
    {
        if (!g_ascii_strcasecmp (i->name, symbol))
        {
            return i->value;
        }
        ++i;
    }
    return NULL;
}

static gchar *
xpm_extract_color (const gchar *buffer, xfwmColorSymbol *color_sym)
{
    const gchar *p;
    gchar word[129], color[129], current_color[129];
    gchar *r;
    gint new_key;
    gint key;
    gint current_key;
    gint space;

    p = &buffer[0];
    space = 128;
    word[0] = '\0';
    color[0] = '\0';
    current_color[0] = '\0';
    current_key = 1;
    new_key = 0;
    key = 0;

    while (1)
    {
        /* skip whitespace */
        for (; *p != '\0' && g_ascii_isspace (*p); p++)
        {
        }
        /* copy word */
        for (r = word;
                 (*p != '\0') &&
                 (!g_ascii_isspace (*p)) &&
                 (r - word < (gint) sizeof (word) - 1);
             p++, r++)
        {
                *r = *p;
        }
        *r = '\0';
        if (*word == '\0')
        {
            if (color[0] == '\0')  /* incomplete colormap entry */
            {
                return NULL;
            }
            else  /* end of entry, still store the last color */
            {
                new_key = 1;
            }
        }
        else if (key > 0 && color[0] == '\0')  /* next word must be a color name part */
        {
                new_key = 0;
        }
        else
        {
            if (strcmp (word, "s") == 0)
            {
                new_key = 5;
            }
            else if (strcmp (word, "c") == 0)
            {
                new_key = 4;
            }
            else if (strcmp (word, "g") == 0)
            {
                new_key = 3;
            }
            else if (strcmp (word, "g4") == 0)
            {
                new_key = 2;
            }
            else if (strcmp (word, "m") == 0)
            {
                new_key = 1;
            }
            else
            {
                new_key = 0;
            }
        }
        if (new_key == 0)
        {  /* word is a color name part */
            if (key == 0)  /* key expected */
            {
                return NULL;
            }
            /* accumulate color name */
            if (color[0] != '\0')
            {
                strncat (color, " ", space);
                space -= MIN (space, 1);
            }
            strncat (color, word, space);
            space -= MIN (space, (gint) strlen (word));
        }
        else if (key == 5)
        {
            const gchar *new_color = NULL;
            new_color = search_color_symbol (color, color_sym);
            if (new_color)
            {
                current_key = key;
                strcpy (current_color, new_color);
            }
            space = 128;
            color[0] = '\0';
            key = new_key;
            if (*p == '\0')
            {
                break;
            }
        }
        else
        {  /* word is a key */
            if (key > current_key)
            {
                current_key = key;
                strcpy (current_color, color);
            }
            space = 128;
            color[0] = '\0';
            key = new_key;
            if (*p == '\0')
            {
                break;
            }
        }
    }
    if (current_key > 1)
    {
        return g_strdup (current_color);
    }
    else
    {
        return NULL;
    }
}

static const gchar *
file_buffer (enum buf_op op, gpointer handle)
{
    struct file_handle *h;

    h = handle;
    switch (op)
    {
        case op_header:
            if (xpm_seek_string (h->infile, "XPM") != TRUE)
            {
                break;
            }
            if (xpm_seek_char (h->infile, '{') != TRUE)
            {
                break;
            }
            /* Fall through to the next xpm_seek_char. */

        case op_cmap:
            xpm_seek_char (h->infile, '"');
            fseek (h->infile, -1, SEEK_CUR);
            /* Fall through to the xpm_read_string. */

        case op_body:
            if(!xpm_read_string (h->infile, &h->buffer, &h->buffer_size))
            {
                return NULL;
            }
            return h->buffer;

        default:
            g_assert_not_reached ();
    }

    return NULL;
}

/* This function does all the work. */
static GdkPixbuf *
pixbuf_create_from_xpm (gpointer handle, xfwmColorSymbol *color_sym)
{
    gchar pixel_str[32];
    const gchar *buffer;
    gchar *name_buf;
    gint w, h, n_col, cpp, items;
    gint cnt, xcnt, ycnt, wbytes, n;
    GHashTable *color_hash;
    XPMColor *colors, *color, *fallbackcolor;
    guchar *pixtmp;
    GdkPixbuf *pixbuf;

    fallbackcolor = NULL;

    buffer = file_buffer (op_header, handle);
    if (!buffer)
    {
        g_warning ("Cannot read Pixmap header");
        return NULL;
    }
    items = sscanf (buffer, "%d %d %d %d", &w, &h, &n_col, &cpp);

    if (items != 4)
    {
        g_warning ("Pixmap definition contains invalid number attributes (expecting at least 4, got %i)", items);
        return NULL;
    }

    if ((w <= 0) ||
        (h <= 0) ||
        (cpp <= 0) ||
        (cpp >= 32) ||
        (n_col <= 0) ||
        (n_col >= G_MAXINT / (cpp + 1)) ||
        (n_col >= G_MAXINT / (gint) sizeof (XPMColor)))
    {
        g_warning ("Pixmap definition contains invalid attributes");
        return NULL;
    }

    /* The hash is used for fast lookups of color from chars */
    color_hash = g_hash_table_new (g_str_hash, g_str_equal);

    name_buf = g_try_malloc (n_col * (cpp + 1));
    if (!name_buf) {
        g_hash_table_destroy (color_hash);
        g_warning ("Cannot allocate buffer");
        return NULL;
    }

    colors = (XPMColor *) g_try_malloc (sizeof (XPMColor) * n_col);
    if (!colors)
    {
        g_hash_table_destroy (color_hash);
        g_free (name_buf);
        g_warning ("Cannot allocate colors for Pixmap");
        return NULL;
    }

    for (cnt = 0; cnt < n_col; cnt++)
    {
        gchar *color_name;

        buffer = file_buffer (op_cmap, handle);
        if (!buffer)
        {
            g_hash_table_destroy (color_hash);
            g_free (name_buf);
            g_free (colors);
            g_warning ("Cannot load colormap attributes");
            return NULL;
        }

        color = &colors[cnt];
        color->color_string = &name_buf[cnt * (cpp + 1)];
        strncpy (color->color_string, buffer, cpp);
        color->color_string[cpp] = 0;
        buffer += strlen (color->color_string);
        color->transparent = FALSE;

        color_name = xpm_extract_color (buffer, color_sym);

        if ((color_name == NULL) ||
            (g_ascii_strcasecmp (color_name, "None") == 0) ||
            (parse_color (color_name, color) == FALSE))
        {
            color->transparent = TRUE;
            color->red = 0;
            color->green = 0;
            color->blue = 0;
        }

        g_free (color_name);
        g_hash_table_insert (color_hash, color->color_string, color);

        if (cnt == 0)
        {
            fallbackcolor = color;
        }
    }

    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, w, h);

    if (!pixbuf)
    {
        g_hash_table_destroy (color_hash);
        g_free (colors);
        g_free (name_buf);
        g_warning ("Cannot allocate Pixbuf");
        return NULL;
    }

    wbytes = w * cpp;

    for (ycnt = 0; ycnt < h; ycnt++)
    {
        pixtmp = gdk_pixbuf_get_pixels (pixbuf) + ycnt * gdk_pixbuf_get_rowstride(pixbuf);

        buffer = file_buffer (op_body, handle);
        if ((!buffer) || (wbytes > (gint) strlen (buffer)))
        {
            continue;
        }

        for (n = 0, cnt = 0, xcnt = 0; n < wbytes; n += cpp, xcnt++)
        {
            strncpy (pixel_str, &buffer[n], cpp);
            pixel_str[cpp] = 0;

            color = g_hash_table_lookup (color_hash, pixel_str);

            /* Bad XPM...punt */
            if (!color)
            {
                color = fallbackcolor;
            }

            *pixtmp++ = color->red   >> 8;
            *pixtmp++ = color->green >> 8;
            *pixtmp++ = color->blue  >> 8;

            if (color->transparent)
            {
                *pixtmp++ = 0;
            }
            else
            {
                *pixtmp++ = 0xFF;
            }
        }
    }

    g_hash_table_destroy (color_hash);
    g_free (colors);
    g_free (name_buf);

    return pixbuf;
}

static GdkPixbuf *
xpm_image_load (const char *filename, xfwmColorSymbol *color_sym)
{
    guchar buffer[1024];
    GdkPixbuf *pixbuf;
    struct file_handle h;
    int size;
    FILE *f;

    f = g_fopen (filename, "rb");
    if (!f)
    {
        return NULL;
    }

    size = fread (&buffer, 1, sizeof (buffer), f);
    if (size == 0)
    {
        fclose (f);
        return NULL;
    }

    fseek (f, 0, SEEK_SET);
    memset (&h, 0, sizeof (h));
    h.infile = f;
    pixbuf = pixbuf_create_from_xpm (&h, color_sym);
    g_free (h.buffer);
    fclose (f);

    return pixbuf;
}

static GdkPixbuf *
xfwmPixmapCompose (GdkPixbuf *pixbuf, const gchar * dir, const gchar * file)
{
    GdkPixbuf *alpha;
    gchar *filepng;
    gchar *filename;
    gint width, height;
    int i;

    static const char* image_types[] = {
      "svg",
      "png",
      "gif",
      "jpg",
      "bmp",
      NULL };

    i = 0;
    alpha = NULL;

    while ((image_types[i]) && (!alpha))
    {
        filepng = g_strdup_printf ("%s.%s", file, image_types[i]);
        filename = g_build_filename (dir, filepng, NULL);
        g_free (filepng);

        if (g_file_test (filename, G_FILE_TEST_IS_REGULAR))
        {
            alpha = gdk_pixbuf_new_from_file (filename, NULL);
        }
        g_free (filename);
        ++i;
    }

    if (!alpha)
    {
        /* We have no suitable image to layer on top of the XPM, stop here... */
        return (pixbuf);
    }

    if (!pixbuf)
    {
        /* We have no XPM canvas and found a suitable image, use it... */
        return (alpha);
    }

    width  = MIN (gdk_pixbuf_get_width (pixbuf),
                  gdk_pixbuf_get_width (alpha));
    height = MIN (gdk_pixbuf_get_height (pixbuf),
                  gdk_pixbuf_get_height (alpha));

    gdk_pixbuf_composite (alpha, pixbuf, 0, 0, width, height,
                          0, 0, 1.0, 1.0, GDK_INTERP_NEAREST, 0xFF);

    g_object_unref (alpha);

    return pixbuf;
}

GdkPixbuf *
xfwmPixbufLoad (const gchar * dir, const gchar * file, xfwmColorSymbol * cs)
{
    gchar *filename;
    gchar *filexpm;
    GdkPixbuf *pixbuf;

    /*
     * Always try to load the XPM first, using our own routine
     * that supports XPM color symbol susbstitution (used to
     * apply the gtk+ colors to the pixmaps).
     */
    filexpm = g_strdup_printf ("%s.%s", file, "xpm");
    filename = g_build_filename (dir, filexpm, NULL);
    g_free (filexpm);
    pixbuf = xpm_image_load (filename, cs);
    g_free (filename);

    /* Compose with other image formats, if any available. */
    pixbuf = xfwmPixmapCompose (pixbuf, dir, file);
    return pixbuf;
}
