AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"libwck-common\" \
	$(PLATFORM_CPPFLAGS)

noinst_LTLIBRARIES = \
	libwck-common.la

libwck_common_la_SOURCES = \
	ui_style.c \
	ui_style.h \
	wck-plugin.c \
	wck-plugin.h \
	wck-utils.c \
	wck-utils.h

libwck_common_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBWNCK_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCONF_CFLAGS) \
	$(PLATFORM_CFLAGS) \
	-DDATADIR=\"$(datadir)\"

libwck_common_la_LDFLAGS = \
	-no-undefined \
	$(PLATFORM_LDFLAGS)

libwck_common_la_LIBADD = \
	$(GTK_LIBS) \
	$(LIBWNCK_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCONF_LIBS)

EXTRA_DIST = \
	meson.build \
	$(NULL)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
