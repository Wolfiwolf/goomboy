#!/usr/bin/env python3
"""Replace the DOOM intro/title screen (TITLEPIC lump) inside an IWAD with a photo."""
import struct, sys
import numpy as np
from PIL import Image

WAD = "/home/wolfiwolf/dev/goom/src/doomgeneric/DOOM1.WAD"
IMG = "/home/wolfiwolf/.claude/image-cache/59c2639f-f26c-4bde-9821-0748778a9a9d/1.png"
PREVIEW = "/home/wolfiwolf/dev/goom/src/doomgeneric/title_preview.png"

def read_dir(buf):
    magic, nlumps, infotab = struct.unpack_from("<4sii", buf, 0)
    lumps = []
    for i in range(nlumps):
        pos, size, raw = struct.unpack_from("<ii8s", buf, infotab + i * 16)
        name = raw.split(b"\0")[0].decode("ascii", "replace")
        lumps.append([name, pos, size])
    return magic, lumps

def palette_from(buf, lumps):
    for name, pos, size in lumps:
        if name == "PLAYPAL":
            return np.frombuffer(buf, np.uint8, 768, pos).reshape(256, 3).astype(np.int16)
    raise RuntimeError("PLAYPAL not found")

def patch_header(buf, pos):
    return struct.unpack_from("<hhhh", buf, pos)

def make_patch(idx, w, h, lo, to):
    """Encode a fully-opaque HxW index array as a DOOM picture lump."""
    head = struct.pack("<hhhh", w, h, lo, to)
    coloffs, cols = bytearray(), bytearray()
    base = 8 + 4 * w
    for x in range(w):
        coloffs += struct.pack("<i", base + len(cols))
        y = 0
        while y < h:                       # posts of <=254 rows (h=200 -> one post)
            n = min(254, h - y)
            seg = idx[y:y + n, x].tobytes()
            cols += bytes([y & 0xFF, n]) + bytes([seg[0]]) + seg + bytes([seg[-1]])
            y += n
        cols += b"\xff"
    return head + bytes(coloffs) + bytes(cols)

def fit(src, w, h, pal):
    """Center-crop src to w:h aspect, resize, Floyd-Steinberg dither to palette."""
    sw, sh = src.size
    target = w / h
    if sw / sh > target:
        nw = int(round(sh * target)); x0 = (sw - nw) // 2
        box = (x0, 0, x0 + nw, sh)
    else:
        nh = int(round(sw / target)); y0 = (sh - nh) // 2
        box = (0, y0, sw, y0 + nh)
    im = src.crop(box).resize((w, h), Image.LANCZOS)
    palimg = Image.new("P", (1, 1))
    palimg.putpalette(pal.astype(np.uint8).reshape(-1).tolist())
    q = im.quantize(palette=palimg, dither=Image.FLOYDSTEINBERG)
    return np.asarray(q, np.uint8)

def main():
    buf = open(WAD, "rb").read()
    magic, lumps = read_dir(buf)
    pal = palette_from(buf, lumps)
    src = Image.open(IMG).convert("RGB")

    new_data = {}
    for name, pos, size in lumps:
        if name != "TITLEPIC":
            continue
        w, h, lo, to = patch_header(buf, pos)
        idx = fit(src, w, h, pal)
        new_data[name] = make_patch(idx, w, h, lo, to)

    if not new_data:
        sys.exit("TITLEPIC lump not found!")

    # Rebuild WAD: header + lump data (TITLEPIC replaced) + directory
    out = bytearray(12)
    newdir = []
    for name, pos, size in lumps:
        data = new_data.get(name, buf[pos:pos + size] if size > 0 else b"")
        fp = len(out) if data else 0
        out += data
        newdir.append((name, fp, len(data)))
    infotab = len(out)
    for name, fp, sz in newdir:
        out += struct.pack("<ii8s", fp, sz, name.encode("ascii")[:8].ljust(8, b"\0"))
    struct.pack_into("<4sii", out, 0, magic, len(newdir), infotab)
    open(WAD, "wb").write(out)

    # Visual sanity check: decode the new TITLEPIC back to PNG
    nb = bytes(out)
    _, nl = read_dir(nb)
    for name, p, s in nl:
        if name == "TITLEPIC":
            w, h, lo, to = patch_header(nb, p)
            img = np.zeros((h, w, 3), np.uint8)
            offs = struct.unpack_from("<%di" % w, nb, p + 8)
            for x, co in enumerate(offs):
                c = p + co
                while nb[c] != 0xFF:
                    td, ln = nb[c], nb[c + 1]
                    px = np.frombuffer(nb, np.uint8, ln, c + 3)
                    img[td:td + ln, x] = pal[px]
                    c += 4 + ln
            Image.fromarray(img).save(PREVIEW)
            print(f"verify TITLEPIC: {w}x{h} off=({lo},{to}), lump {s} bytes")
            break

    print(f"{magic.decode()} rebuilt: TITLEPIC replaced; total lumps {len(newdir)}")
    print("preview:", PREVIEW)

if __name__ == "__main__":
    main()
