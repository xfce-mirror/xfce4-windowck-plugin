# xfce4-windowck-plugin

Xfce panel plugin which allows to put the maximized window title and windows buttons on the panel.

Original plugin's features:

* Show the title and buttons of the maximized window on the panel.
* Allow window actions on buttons and title clicks (activate, (un)maximize, close).
* Allow window action menu on left button click.
* Title formatting options.
* xfwm4/unity theming support for buttons.

FAQ:  
Q: How do I hide the window decorations on maximized windows on Xfce?  
A: Xfce 4.12 now support Titleless feature! Enable it in Window manager tweaks → Accessibility.

This code is derived from [Window Applets](https://www.gnome-look.org/p/1115400) by Andrej Belcijan.

## Screenshots

See [this page](https://goodies.xfce.org/projects/panel-plugins/xfce4-windowck-plugin) for screenshots.

## Installation

### Arch Linux

For Arch Linux users [AUR package](https://aur.archlinux.org/packages/xfce4-windowck-plugin/) is available.

### Debian/Ubuntu

* Install dependencies:

  ```bash
  sudo apt update
  sudo apt install xfce4-dev-tools intltool libgtk-3-dev libpango1.0-dev libwnck-3-dev libxfce4panel-2.0-dev libxfce4ui-2-dev libxfce4util-dev libxfconf-0-dev
  ```

* Generate common makefiles:
  
  ```bash
  ./autogen.sh --prefix=/usr --libdir=/usr/lib/$(uname -m)-linux-gnu
  ```

* Compile and install the plugin:

  ```bash
  make
  sudo make install
  ```

## How to report bugs?

Bugs should be reported right to [this page](https://gitlab.xfce.org/panel-plugins/xfce4-windowck-plugin/-/issues).
