#!/usr/bin/env python3
"""
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
"""

import shutil
import subprocess
from dataclasses import dataclass
from os import linesep


@dataclass
class IconMap:
    data: str
    colors: dict


def generate(name: str, icon: IconMap, x0=0, y0=0, w=None, h=None):
    """ Creates xpm file with given name, given draw as string, colors as dict.
        Extra args are for generate parts of xpm.
    """
    icon_lines = icon.data.split("\n")
    if w is None:
        w = len(icon_lines[0])
    if h is None:
        h = len(icon_lines)
    x1 = x0 + w
    y1 = y0 + h
    colors = {}
    lines = [line[x0:x1] for line in icon_lines[y0:y1]]
    for line in lines:
        for c in line:
            if c not in colors:
                colors[c] = icon.colors[c]

    xpmlines = [
        "/* XPM */",
        f'static char * {name.replace("-", "_")} = {{',
        f'"{w} {h} {len(colors)} 1", '
    ]
    xpmlines.extend(f'"{i[0]}\tc {i[1]}", ' for i in list(colors.items()))
    xpmlines.extend(f'"{i}", ' for i in lines)
    xpmlines.append("};")

    with open(f"{name}.xpm", "w") as f:
        f.write(linesep.join(xpmlines))


def hole_pos(txt):
    """ Detects a hole on a xpm string, used to find border sizes."""
    lines = txt.split("\n")
    for i, line in enumerate(lines):
        if " " in line:
            return line.find(" "), i
    raise ValueError


def hole_size(txt):
    """ Detects hole on a xpm string, used to find border sizes."""
    lastwidth = 0
    inhole = 0
    for line in txt.split("\n"):
        if " " in line:
            lastwidth = line.count(" ")
            inhole += 1
        elif inhole > 0:
            return lastwidth, inhole
    raise ValueError


def build_xfwm4(icons: dict, active: IconMap, inactive: IconMap):
    for name, icon in icons.items():
        generate(name, icon)

    alines = active.data.split("\n")
    ilines = inactive.data.split("\n")
    alw, alh = (len(alines[0]), len(alines))
    ilw, ilh = (len(ilines[0]), len(ilines))

    ''' Find corner length on a xpm string.'''
    acw = len(active.data.split("+")[0])
    icw = len(inactive.data.split("+")[0])

    ahx, ahy = hole_pos(active.data)
    ihx, ihy = hole_pos(inactive.data)
    ahw, ahh = hole_size(active.data)
    ihw, ihh = hole_size(inactive.data)
    abw, abh = (alw - ahx - ahw, alh - ahy - ahh)
    ibw, ibh = (ilw - ihx - ihw, ilh - ihy - ihh)

    # top-left
    generate("top-left-active", active, 0, 0, acw, ahy)
    generate("top-left-inactive", inactive, 0, 0, icw, ihy)
    # left
    generate("left-active", active, 0, ahy, ahx, ahh)
    generate("left-inactive", inactive, 0, ihy, ihx, ihh)
    # bottom-left
    generate("bottom-left-active", active, 0, ahy + ahh, ahx, abh)
    generate("bottom-left-inactive", inactive, 0, ihy + ihh, ihx, ibh)

    # top-right
    generate("top-right-active", active, alw - acw, 0, acw, ahy)
    generate("top-right-inactive", inactive, ilw - icw, 0, icw, ihy)
    # right
    generate("right-active", active, ahx + ahw, ahy, abw, ahh)
    generate("right-inactive", inactive, ihx + ihw, ihy, ibw, ihh)
    # bottom-right
    generate("bottom-right-active", active, ahx + ahw, ahy + ahh, abw, abh)
    generate("bottom-right-inactive", inactive, ihx + ihw, ihy + ihh, ibw, ibh)

    # top
    for i in range(1, 6):
        generate(f"title-{i}-active", active, acw, 0, alw - 2 * acw, ahy)
        generate(f"title-{i}-inactive", inactive, icw, 0, alw - 2 * icw, ihy)

    # bottom
    generate("bottom-active", active, ahx, ahy + ahh, ahw, ibh)
    generate("bottom-inactive", inactive, ihx, ihy + ihh, ihw, ibh)


def build_unity(icons: dict):
    for name, icon in icons.items():
        generate(name, icon)
        subprocess.call(["convert", f"{name}.xpm", f"{name}.png"])

    for i in ("close", "maximize", "minimize", "menu", "unmaximize"):
        shutil.copy2(f"{i}_focused_normal.png", f"{i}.png")
