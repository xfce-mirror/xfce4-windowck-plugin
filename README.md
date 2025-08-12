[![License](https://img.shields.io/static/v1?label=License&message=GPL-3.0-or-later&color=blue)](https://gitlab.xfce.org/panel-plugins/xfce4-windowck-plugin/-/blob/master/COPYING)

# xfce4-windowck-plugin

An Xfce panel plugin which allows putting buttons, title and menu of active or maximized windows on the panel.
This code is derived from [Window Applets](https://www.gnome-look.org/p/1115400) by Andrej Belcijan.

----

### Homepage

[xfce4-windowck-plugin documentation](https://docs.xfce.org/panel-plugins/xfce4-windowck-plugin/start)

### Changelog

See [NEWS](https://gitlab.xfce.org/panel-plugins/xfce4-windowck-plugin/-/blob/master/NEWS) for details on changes and fixes made in the current release.

### Source Code Repository

[xfce4-windowck-plugin source code](https://gitlab.xfce.org/panel-plugins/xfce4-windowck-plugin)

### Download a Release Tarball

[xfce4-windowck-plugin archive](https://archive.xfce.org/src/panel-plugins/xfce4-windowck-plugin)
    or
[xfce4-windowck-plugin tags](https://gitlab.xfce.org/panel-plugins/xfce4-windowck-plugin/-/tags)

### Installation

From source code repository:

    $ cd xfce4-windowck-plugin
    $ meson setup build
    $ meson compile -C build
    # meson install -C build

From release tarball:

    $ tar xf xfce4-windowck-plugin-<version>.tar.xz
    $ cd xfce4-windowck-plugin-<version>
    $ meson setup build
    $ meson compile -C build
    # meson install -C build

### Uninstallation

    % ninja uninstall -C build

### Reporting Bugs

Visit the [reporting bugs](https://docs.xfce.org/panel-plugins/xfce4-windowck-plugin/bugs) page to view currently open bug reports and instructions on reporting new bugs or submitting bugfixes.
