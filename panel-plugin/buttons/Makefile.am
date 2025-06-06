AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-wckbuttons-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# Windowck Buttons plugin
#
plugin_LTLIBRARIES = \
	libwckbuttons.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libwckbuttons_built_sources = \
	wckbuttons-dialogs-resources.c

libwckbuttons_la_SOURCES = \
	$(libwckbuttons_built_sources) \
	theme.c \
	theme.h \
	wckbuttons.c \
	wckbuttons.h \
	wckbuttons-theme.c \
	wckbuttons-theme.h \
	wckbuttons-dialogs.c \
	wckbuttons-dialogs.h

libwckbuttons_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBWNCK_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCONF_CFLAGS) \
	$(PLATFORM_CFLAGS) \
	-DDATADIR=\"$(datadir)\"

libwckbuttons_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libwckbuttons_la_LIBADD = \
	$(top_builddir)/common/libwck-common.la \
	$(GTK_LIBS) \
	$(LIBWNCK_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCONF_LIBS)

libwckbuttons_la_DEPENDENCIES = \
	$(top_builddir)/common/libwck-common.la

#
# Desktop file
#
desktopdir =								\
	$(datadir)/xfce4/panel/plugins

desktop_DATA =								\
	wckbuttons.desktop

%.desktop: %.desktop.in
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST =								\
	meson.build								\
	wckbuttons-dialogs.glade				\
	wckbuttons-dialogs.gresource.xml		\
	wckbuttons.desktop.in

CLEANFILES =								\
	$(desktop_DATA)

BUILT_SOURCES = \
	$(libwckbuttons_built_sources)

CLEANFILES += \
	$(libwckbuttons_built_sources)

wckbuttons_dialogs_glade_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir) $(srcdir)/wckbuttons-dialogs.gresource.xml)

wckbuttons-dialogs-resources.c: $(srcdir)/wckbuttons-dialogs.gresource.xml $(wckbuttons_dialogs_glade_files) Makefile
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $<

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
