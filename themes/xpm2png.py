#!/usr/bin/env python3

import re
from dataclasses import dataclass
from pathlib import Path

import png

STRIP_CHARS = '", '


@dataclass
class XpmConfig:
    width: int
    height: int
    colors: int
    char: int


def get_xpm_config(config_str: str):
    s = config_str.strip(STRIP_CHARS).split(' ')
    width = int(s[0])
    height = int(s[1])
    colors = int(s[2])
    ch = int(s[3])
    return XpmConfig(width, height, colors, ch)


def get_colors(xpm: list, config: XpmConfig):
    colors = {}
    for i, line in enumerate(xpm):
        if i >= config.colors:
            break
        line = line.strip(STRIP_CHARS)
        ch = line[:config.char]
        if 'none' in line.lower():
            color = 'none'
        else:
            x = line.find('c #')
            s = line[x + 3:]
            color = '#' + line[5] + line[6] + line[9] + line[10] + line[13] + line[14] if (len(s) > 6) else '#' + s

        colors[ch] = color
    return colors


def hex2rgb(hex_str: str):
    hex_re = re.compile(r'^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$', re.IGNORECASE)
    result = hex_re.match(hex_str)
    if not result:
        return None
    return {
        'r': int(result[1], 16),
        'g': int(result[2], 16),
        'b': int(result[3], 16),
    }


def get_pixels(xpm: list, config: XpmConfig, colors: dict):
    pixels = []
    for i, line in enumerate(xpm):
        row = []
        line = line.strip(STRIP_CHARS)
        for j in range(0, len(line), config.char):
            ch = line[j:j + config.char]
            color = colors[ch]
            try:
                if color == 'none':
                    p = [255, 255, 255, 0]
                else:
                    rgb = hex2rgb(color)
                    p = [rgb['r'], rgb['g'], rgb['b'], 255]
                row.extend(p)
            except Exception as e:
                print(f'Error parsing line {i} ({line}): {e}')
        pixels.append(row)
    return pixels


def xpm2png(xpm_file: Path, png_file: Path):
    xpm = xpm_file.read_text().splitlines()
    xpm = xpm[2:-1]  # skip header

    xpm_cfg = get_xpm_config(xpm[0])
    xpm = xpm[1:]  # skip config

    colors = get_colors(xpm, xpm_cfg)
    xpm = xpm[xpm_cfg.colors:]  # skip colors

    pixels = get_pixels(xpm, xpm_cfg, colors)

    return png.from_array(pixels, 'RGBA').save(png_file)
