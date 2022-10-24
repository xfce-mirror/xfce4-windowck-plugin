theme_DATA = \
	bottom-active.xpm \
	bottom-inactive.xpm \
	bottom-left-active.xpm \
	bottom-left-inactive.xpm \
	bottom-right-active.xpm \
	bottom-right-inactive.xpm \
	close-active.xpm \
	close-inactive.xpm \
	close-prelight.xpm \
	close-pressed.xpm \
	hide-active.xpm \
	hide-inactive.xpm \
	hide-prelight.xpm \
	hide-pressed.xpm \
	left-active.xpm \
	left-inactive.xpm \
	maximize-active.xpm \
	maximize-inactive.xpm \
	maximize-prelight.xpm \
	maximize-pressed.xpm \
	maximize-toggled-active.xpm \
	maximize-toggled-inactive.xpm \
	maximize-toggled-prelight.xpm \
	maximize-toggled-pressed.xpm \
	menu-active.xpm \
	menu-inactive.xpm \
	menu-prelight.xpm \
	menu-pressed.xpm \
	right-active.xpm \
	right-inactive.xpm \
	shade-active.xpm \
	shade-inactive.xpm \
	shade-prelight.xpm \
	shade-pressed.xpm \
	shade-toggled-active.xpm \
	shade-toggled-inactive.xpm \
	shade-toggled-prelight.xpm \
	shade-toggled-pressed.xpm \
	stick-active.xpm \
	stick-inactive.xpm \
	stick-prelight.xpm \
	stick-pressed.xpm \
	stick-toggled-active.xpm \
	stick-toggled-inactive.xpm \
	stick-toggled-prelight.xpm \
	stick-toggled-pressed.xpm \
	themerc \
	title-1-active.xpm \
	title-1-inactive.xpm \
	title-2-active.xpm \
	title-2-inactive.xpm \
	title-3-active.xpm \
	title-3-inactive.xpm \
	title-4-active.xpm \
	title-4-inactive.xpm \
	title-5-active.xpm \
	title-5-inactive.xpm \
	top-left-active.xpm \
	top-left-inactive.xpm \
	top-right-active.xpm \
	top-right-inactive.xpm

$(theme_DATA):
	@echo -e "Going to generate xpm files"
	$(PYTHON) $(srcdir)/generator.py

clean: clean-data-local clean-am

clean-data-local:
	@echo -e "Going to clean xpm files"
	rm -f *.xpm

uninstall-local: uninstall-am clean-data-local
	rm -f $(DESTDIR)$(themedir)/*.xpm $(DESTDIR)$(themedir)/themerc

EXTRA_DIST = $(theme_DATA)
