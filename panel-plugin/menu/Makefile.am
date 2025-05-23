AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-wckmenu-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# Windowck Menu plugin
#
plugin_LTLIBRARIES = \
	libwckmenu.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libwckmenu_built_sources = \
	wckmenu-dialogs-resources.c

libwckmenu_la_SOURCES = \
	$(libwckmenu_built_sources) \
	wckmenu.c \
	wckmenu.h \
	wckmenu-dialogs.c \
	wckmenu-dialogs.h \
	wckmenu-icon.c \
	wckmenu-icon.h

libwckmenu_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBWNCK_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCONF_CFLAGS) \
	$(PLATFORM_CFLAGS)

libwckmenu_la_LDFLAGS = \
	-avoid-version \
	-module \
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libwckmenu_la_LIBADD = \
	$(top_builddir)/common/libwck-common.la \
	$(GTK_LIBS) \
	$(LIBWNCK_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCONF_LIBS)

libwckmenu_la_DEPENDENCIES = \
	$(top_builddir)/common/libwck-common.la

#
# Desktop file
#
desktopdir = \
	$(datadir)/xfce4/panel/plugins

desktop_DATA = \
	wckmenu.desktop

%.desktop: %.desktop.in
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = \
	meson.build \
	wckmenu-dialogs.glade \
	wckmenu-dialogs.gresource.xml \
	wckmenu.desktop.in

CLEANFILES = \
	$(desktop_DATA)

BUILT_SOURCES = \
	$(libwckmenu_built_sources)

CLEANFILES += \
	$(libwckmenu_built_sources)

wckmenu_dialogs_glade_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir) $(srcdir)/wckmenu-dialogs.gresource.xml)

wckmenu-dialogs-resources.c: $(srcdir)/wckmenu-dialogs.gresource.xml $(wckmenu_dialogs_glade_files) Makefile
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $<

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
