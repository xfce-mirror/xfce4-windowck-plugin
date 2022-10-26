theme_DATA = \
	close_focused_normal.png \
	close_focused_prelight.png \
	close_focused_pressed.png \
	close.png \
	close_unfocused.png \
	maximize_focused_normal.png \
	maximize_focused_prelight.png \
	maximize_focused_pressed.png \
	maximize.png \
	maximize_unfocused.png \
	menu_focused_normal.png \
	menu_focused_prelight.png \
	menu_focused_pressed.png \
	menu.png \
	menu_unfocused.png \
	minimize_focused_normal.png \
	minimize_focused_prelight.png \
	minimize_focused_pressed.png \
	minimize.png \
	minimize_unfocused.png \
	unmaximize_focused_normal.png \
	unmaximize_focused_prelight.png \
	unmaximize_focused_pressed.png \
	unmaximize.png \
	unmaximize_unfocused.png

$(theme_DATA) &:
	@echo -e "Going to generate xpm and convert to png files"
	$(PYTHON) $(srcdir)/generator.py

clean: clean-data-local clean-am

clean-data-local:
	@echo -e "Going to clean png and xpm files"
	rm -f *.png *.xpm

uninstall-local: uninstall-am clean-data-local
	rm -f $(DESTDIR)$(themedir)/*.png

EXTRA_DIST = $(theme_DATA)
