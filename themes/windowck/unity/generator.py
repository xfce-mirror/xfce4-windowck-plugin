#!/usr/bin/env python3
'''
    simple-gtk xpm generator

    Copyright (C) 2012  Felipe A. Hernandez <spayder26@gmail.com>
    Portions adapted by Cedric Leporcq.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

'''

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from generator_common import IconMap, build_unity


#close
close_focused_normal = '''
..................
..................
==================
=====@@====@@=====
====@++@==@++@====
====@+++@@+++@====
=====@++++++@=====
======@++++@======
======@++++@======
=====@++++++@=====
====@+++@@+++@====
====@++@==@++@====
=====@@====@@=====
==================
'''.strip()

close_focused_normal_map = {
    "." : "None",
    "=" : "None",
    "+" : "#202020",
    "@" : "None",
    "#" : "None",
}

close_focused_prelight_map = {
    "." : "None",
    "=" : "None",
    "+" : "#D92626",
    "@" : "#000000",
    "#" : "None",
}

close_focused_pressed_map = {
    "." : "None",
    "=" : "None",
    "+" : "#D92626",
    "@" : "None",
    "#" : "None",
}

close_unfocused_map = {
    "." : "None",
    "=" : "None",
    "+" : "#606060",
    "@" : "None",
    "#" : "None",
}

#hide
minimize_focused_normal = '''
..................
..................
==================
==================
==================
==================
==================
==================
==================
===@@@@@@@@@@@@===
===@++++++++++@===
===@++++++++++@===
===@@@@@@@@@@@@===
==================
'''.strip()

minimize_focused_prelight_map = {
    "." : "None",
    "=" : "None",
    "+" : "#000000",
    "@" : "#FFFFFF",
    "#" : "#000000",
}

minimize_focused_pressed_map = {
    "." : "None",
    "=" : "None",
    "+" : "#000000",
    "@" : "None",
    "#" : "#000000",
}

#maximize
maximize_focused_normal = '''
..................
..................
===@@@@@@@@@@@@===
===@++++++++++@===
===@++++++++++@===
===@+@@@@@@@@+@===
===@+@======@+@===
===@+@======@+@===
===@+@======@+@===
===@+@======@+@===
===@+@@@@@@@@+@===
===@++++++++++@===
===@@@@@@@@@@@@===
==================
'''.strip()

#maximize-toggled
unmaximize_focused_normal = '''
..................
..................
==================
==================
====@@@@@@@@@@====
====@++++++++@====
====@++++++++@====
====@+@@@@@@+@====
====@+@====@+@====
====@+@====@+@====
====@+@@@@@@+@====
====@++++++++@====
====@@@@@@@@@@====
==================
'''.strip()

#menu
menu_focused_normal = '''
..................
..................
==================
==================
==================
====@@@@@@@@@@====
====@++++++++@====
=====@++++++@=====
======@++++@======
=======@++@=======
========@@========
==================
==================
==================
'''.strip()

icons = {
    # close
    'close_focused_normal': IconMap(close_focused_normal, close_focused_normal_map),
    'close_focused_prelight': IconMap(close_focused_normal, close_focused_prelight_map),
    'close_focused_pressed': IconMap(close_focused_normal, close_focused_pressed_map),
    'close_unfocused': IconMap(close_focused_normal, close_unfocused_map),

    # hide
    'minimize_focused_normal': IconMap(minimize_focused_normal, close_focused_normal_map),
    'minimize_focused_prelight': IconMap(minimize_focused_normal, minimize_focused_prelight_map),
    'minimize_focused_pressed': IconMap(minimize_focused_normal, minimize_focused_pressed_map),
    'minimize_unfocused': IconMap(minimize_focused_normal, close_unfocused_map),

    # maximize
    'maximize_focused_normal': IconMap(maximize_focused_normal, close_focused_normal_map),
    'maximize_focused_prelight': IconMap(maximize_focused_normal, minimize_focused_prelight_map),
    'maximize_focused_pressed': IconMap(maximize_focused_normal, minimize_focused_pressed_map),
    'maximize_unfocused': IconMap(maximize_focused_normal, close_unfocused_map),

    # maximize-toggled
    'unmaximize_focused_normal': IconMap(unmaximize_focused_normal, close_focused_normal_map),
    'unmaximize_focused_prelight': IconMap(unmaximize_focused_normal, minimize_focused_prelight_map),
    'unmaximize_focused_pressed': IconMap(unmaximize_focused_normal, minimize_focused_pressed_map),
    'unmaximize_unfocused': IconMap(unmaximize_focused_normal, close_unfocused_map),

    # menu
    'menu_focused_normal': IconMap(menu_focused_normal, close_focused_normal_map),
    'menu_focused_prelight': IconMap(menu_focused_normal, minimize_focused_prelight_map),
    'menu_focused_pressed': IconMap(menu_focused_normal, minimize_focused_prelight_map),
    'menu_unfocused': IconMap(menu_focused_normal, close_unfocused_map),
}

if __name__ == "__main__":
    build_unity(icons)
