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

from generator_xfwm4 import IconMap, build


#close
close_active = '''
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

close_active_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#E6E6E6",
    "@" : "#3C3C3C",
    "#" : "None",
}

close_prelight_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#D92626",
    "@" : "#000000",
    "#" : "None",
}

close_pressed_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#D92626",
    "@" : "#3C3C3C",
    "#" : "#000000",
}

close_inactive_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#A1A1A1",
    "@" : "#3C3C3C",
    "#" : "None",
}

#hide
hide_active = '''
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

hide_prelight_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#FFFFFF",
    "@" : "#000000",
}

hide_pressed_map = {
    "." : "None",
    "=" : "#3C3C3C",
    "+" : "#FFFFFF",
    "@" : "#3C3C3C",
}

#maximize
maximize_active = '''
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
maximize_toggled_active = '''
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
menu_active = '''
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

#shade
shade_active = '''
..................
..................
==================
========@@========
=======@++@=======
======@++++@======
=====@++++++@=====
====@++++++++@====
====@++@++@++@====
=====@@@++@@@=====
=======@++@=======
=======@++@=======
========@@========
==================
'''.strip()

#shade-toggled
shade_toggled_active = '''
..................
..................
==================
========@@========
=======@++@=======
=======@++@=======
=====@@@++@@@=====
====@++@++@++@====
====@++++++++@====
=====@++++++@=====
======@++++@======
=======@++@=======
========@@========
==================
'''.strip()

#stick
stick_active = '''
..................
..................
==================
=======@@@@=======
======@++++@======
=====@++++++@=====
====@++@@@@++@====
====@++@==@++@====
====@++@==@++@====
====@++@@@@++@====
=====@++++++@=====
======@++++@======
=======@@@@=======
==================
'''.strip()

#stick-toggled
stick_toggled_active = '''
..................
..................
==================
=======@@@@=======
======@++++@======
=====@++++++@=====
====@++@@@@++@====
====@++@++@++@====
====@++@++@++@====
====@++@@@@++@====
=====@++++++@=====
======@++++@======
=======@@@@=======
==================
'''.strip()

icons = {
    # close
    'close-active': IconMap(close_active, close_active_map),
    'close-prelight': IconMap(close_active, close_prelight_map),
    'close-pressed': IconMap(close_active, close_pressed_map),
    'close-inactive': IconMap(close_active, close_inactive_map),

    # hide
    'hide-active': IconMap(hide_active, close_active_map),
    'hide-prelight': IconMap(hide_active, hide_prelight_map),
    'hide-pressed': IconMap(hide_active, hide_pressed_map),
    'hide-inactive': IconMap(hide_active, close_inactive_map),

    # maximize
    'maximize-active': IconMap(maximize_active, close_active_map),
    'maximize-prelight': IconMap(maximize_active, hide_prelight_map),
    'maximize-pressed': IconMap(maximize_active, hide_pressed_map),
    'maximize-inactive': IconMap(maximize_active, close_inactive_map),

    # maximize-toggled
    'maximize-toggled-active': IconMap(maximize_toggled_active, close_active_map),
    'maximize-toggled-prelight': IconMap(maximize_toggled_active, hide_prelight_map),
    'maximize-toggled-pressed': IconMap(maximize_toggled_active, hide_pressed_map),
    'maximize-toggled-inactive': IconMap(maximize_toggled_active, close_inactive_map),

    # menu
    'menu-active': IconMap(menu_active, close_active_map),
    'menu-prelight': IconMap(menu_active, hide_prelight_map),
    'menu-pressed': IconMap(menu_active, hide_prelight_map),
    'menu-inactive': IconMap(menu_active, close_inactive_map),

    # shade
    'shade-active': IconMap(shade_active, close_active_map),
    'shade-prelight': IconMap(shade_active, hide_prelight_map),
    'shade-pressed': IconMap(shade_active, hide_pressed_map),
    'shade-inactive': IconMap(shade_active, close_inactive_map),  # menu_inactive_map

    # shade-toggled
    'shade-toggled-active': IconMap(shade_toggled_active, close_active_map),
    'shade-toggled-prelight': IconMap(shade_toggled_active, hide_prelight_map),
    'shade-toggled-pressed': IconMap(shade_toggled_active, hide_pressed_map),
    'shade-toggled-inactive': IconMap(shade_toggled_active, close_inactive_map),  # menu_inactive_map

    # stick
    'stick-active': IconMap(stick_active, close_active_map),
    'stick-prelight': IconMap(stick_active, hide_prelight_map),
    'stick-pressed': IconMap(stick_active, hide_pressed_map),
    'stick-inactive': IconMap(stick_active, close_inactive_map),  # menu_inactive_map

    # stick-toggled
    'stick-toggled-active': IconMap(stick_toggled_active, close_active_map),
    'stick-toggled-prelight': IconMap(stick_toggled_active, hide_prelight_map),
    'stick-toggled-pressed': IconMap(stick_toggled_active, hide_pressed_map),
    'stick-toggled-inactive': IconMap(stick_toggled_active, close_inactive_map),  # menu_inactive_map
}

active = '''
..+++++++++..
.+%%%%%%%%%+.
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
+%         %+
+%%%%%%%%%%%+
+%%%%%%%%%%%+
.+++++++++++. 
'''.strip()
amap = {
    "=" : "#000000",
    "+" : "#000000",
    "%" : "#3C3C3C",
    "#" : "#000000",
    "." : "None",
}


if __name__ == "__main__":
    active_icon = IconMap(active, amap)
    build(icons, active_icon, active_icon)
