#!/usr/bin/env python3

import os
import argparse
import struct
from PIL import Image

#==========================================
#  WARING: CONTAINS VIBE CODED SEGMENTS
#==========================================

# Usage: python3 tools/wdpacker.py ./res -o game.wd
# ./res is a directory where the images are stored.
# the images are in the following format:
# nameofasset_widthxheight.png
# example: player_40x40.png
# All magenta colors will be taken as alpha 0 (transparent)
# TODO: Specify magenta RGB value

WD_FILE_MARK = 0xDEADBEEF

class Asset():
    def __init__(self):
        self.fpath = ""
        self.name = ""
        self.size = 0
        self.w = 0
        self.h = 0

def filename_to_asset(dir: str, fname: str) -> Asset:
    ass = Asset()

    parts = fname.split(".")[0].split("_")
    ass.fpath = f"{dir}/{fname}"
    ass.name = parts[0].upper()
    ass.w = int(parts[1].split("x")[0])
    ass.h = int(parts[1].split("x")[1])
    ass.size += ass.w * ass.h * 3

    return ass

def get_assets_in_dir(dir: str) -> list[Asset]:
    assets = []
    for f in os.listdir(dir):
        assets.append(filename_to_asset(dir, f))
    return assets

def write_asset_table(file, assets: list[Asset]):
    file.write(struct.pack("I", len(assets)))

    offset = 0

    for a in assets:
        # Name
        file.write(struct.pack("B", len(a.name)))
        for c in a.name:
            file.write(struct.pack("B", ord(c)))

        # Offset
        file.write(struct.pack("I", offset))

        # Size
        file.write(struct.pack("I", a.size))

        # Dimensions
        file.write(struct.pack("I", a.w))
        file.write(struct.pack("I", a.h))

        offset += a.size

def write_asset(file, asset: Asset):
    img = Image.open(asset.fpath).resize((asset.w, asset.h), Image.LANCZOS).convert("RGBA")

    for y in range(asset.h):
        for x in range(asset.w):
            r, g, b, a = img.getpixel((x, y))

            # If pixel is transparent make it magenta
            if a > 200:
                file.write(struct.pack("B", r))
                file.write(struct.pack("B", g))
                file.write(struct.pack("B", b))
            else:
                file.write(struct.pack("B", 254))
                file.write(struct.pack("B", 1))
                file.write(struct.pack("B", 254))

def write_wd_mark(file):
    file.write(struct.pack("I", WD_FILE_MARK))

def write_assets_to_file(fname: str, assets: list[Asset]):
    f = open(fname, "wb")

    write_wd_mark(f)
    write_asset_table(f, assets)
    for a in assets:
        write_asset(f, a)

    f.close()

def main():
    parser = argparse.ArgumentParser(description="Generate .wd (Where-Data) file from dir of images")
    parser.add_argument("dir", help="Location of the wd files")
    parser.add_argument("-o", "--output", help="Output filepath")
    args = parser.parse_args()

    assets = get_assets_in_dir(args.dir)

    for a in assets:
        print(f"{a.name}: {a.w} {a.h}")

    write_assets_to_file(args.output, assets)

if __name__ == "__main__":
    main()
