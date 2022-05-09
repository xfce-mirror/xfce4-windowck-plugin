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
from os import linesep
import shutil
import subprocess


def generate(name, txt, dic, x0=0, y0=0, w=None, h=None):
    ''' Creates xpm file with given name, given draw as string, colors as dict.
        Extra args are for generate parts of xpm.
    '''
    if w is None:
        w = len(txt.split("\n")[0])
    if h is None:
        h = len(txt.split("\n"))
    x1 = x0 + w
    y1 = y0 + h
    colors = {}
    lines = [i[x0:x1] for i in txt.split("\n")[y0:y1]]
    for i in lines:
        for j in i:
            if j not in colors:
                colors[j] = dic[j]
    xpmlines = [
        "/* XPM */",
        f'static char * {name.replace("-", "_")} = {{',
        f'"{w} {h} {len(colors)} 1", '
        ]
    xpmlines.extend(
        f'"{i[0]}\tc {i[1]}", ' for i in list(colors.items())
        )
    xpmlines.extend(
        f'"{i}", ' for i in lines
        )
    xpmlines.append(
        "};"
        )
    with open(f"{name}.xpm", "w") as f: f.write(linesep.join(xpmlines))

def build():
    gvar = globals()
    for i in ("close", "maximize", "minimize", "menu", "unmaximize"):
        for j in ("focused_normal", "focused_prelight", "focused_pressed", "unfocused"):
            name = f"{i}_{j}"
            if name in gvar:
                generate(name, gvar[name], gvar[f"{name}_map"])
                subprocess.call(["convert", f"{name}.xpm", f"{name}.png"])
        shutil.copy2(f"{i}_focused_normal.png", f"{i}.png")

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

chars = ["+", "@", "#", "=", "-"]
dmap = [(".", "None")]
close_focused_normal_map = {
    "." : "None",
    "=" : "None",
    "+" : "#E6E6E6",
    "@" : "None",
    "#" : "None",
}

close_focused_prelight = close_focused_normal
close_focused_prelight_map = {
    "." : "None",
    "=" : "None",
    "+" : "#D92626",
    "@" : "#000000",
    "#" : "None",
}

close_focused_pressed = close_focused_normal
close_focused_pressed_map = {
    "." : "None",
    "=" : "None",
    "+" : "#D92626",
    "@" : "None",
    "#" : "#000000",
}

close_unfocused = close_focused_normal
close_unfocused_map = {
    "." : "None",
    "=" : "None",
    "+" : "#A1A1A1",
    "@" : "None",
    "#" : "#000000",
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
minimize_focused_normal_map = close_focused_normal_map
minimize_focused_prelight = minimize_focused_normal
minimize_focused_prelight_map = {
    "." : "None",
    "=" : "None",
    "+" : "#FFFFFF",
    "@" : "#000000",
    "#" : "None",
}

minimize_focused_pressed = minimize_focused_normal
minimize_focused_pressed_map = {
    "." : "None",
    "=" : "None",
    "+" : "#FFFFFF",
    "@" : "None",
    "#" : "None",
}

minimize_unfocused = minimize_focused_normal
minimize_unfocused_map = close_unfocused_map

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
maximize_focused_normal_map = close_focused_normal_map
maximize_focused_prelight = maximize_focused_normal
maximize_focused_prelight_map = minimize_focused_prelight_map
maximize_focused_pressed = maximize_focused_normal
maximize_focused_pressed_map = minimize_focused_pressed_map
maximize_unfocused = maximize_focused_normal
maximize_unfocused_map = close_unfocused_map

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
unmaximize_focused_normal_map = close_focused_normal_map
unmaximize_focused_prelight = unmaximize_focused_normal
unmaximize_focused_prelight_map = minimize_focused_prelight_map
unmaximize_focused_pressed = unmaximize_focused_normal
unmaximize_focused_pressed_map = minimize_focused_pressed_map
unmaximize_unfocused = unmaximize_focused_normal
unmaximize_unfocused_map = close_unfocused_map

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
menu_focused_normal_map = close_focused_normal_map
menu_focused_prelight = menu_focused_normal
menu_focused_prelight_map = minimize_focused_prelight_map
menu_focused_pressed = menu_focused_normal
menu_focused_pressed_map = minimize_focused_prelight_map
menu_unfocused = menu_focused_normal
menu_unfocused_map = close_unfocused_map

if __name__ == "__main__":
    build()
