# -*- coding: utf-8 -*-
import os
from PIL import Image, ImageChops
here = os.path.dirname(os.path.abspath(__file__))
a = Image.open(os.path.join(here, "r2_pw.bmp")).convert("RGB")
b = Image.open(os.path.join(here, "r3_pw.bmp")).convert("RGB")
print("goc r2", a.getpixel((5, 5)), "r3", b.getpixel((5, 5)), "giua r2", a.getpixel((512, 384)), "r3", b.getpixel((512, 384)))
def stats(box, label):
    x = a.crop(box); y = b.crop(box); d = ImageChops.difference(x, y).convert("L"); h = d.histogram(); n = sum(h)
    print("%-28s khac>0 %6.2f%%  khac>=32 %6.2f%%  TB %.2f" % (label, 100.0 * (n - h[0]) / n, 100.0 * sum(h[32:]) / n, sum(i * c for i, c in enumerate(h)) / n))
stats((0, 0, 1024, 768), "TOAN ANH")
for s, l in enumerate(["ALPHA a=255", "ALPHA a=128", "OPACITY", "3LEVEL", "NOT_BE_LIT", "COLOR_ADJUST", "BORDER"]):
    stats((0, 10 + s * 84, 700, 10 + s * 84 + 83), "hang " + l)
stats((0, 685, 700, 740), "spr moi (screen)")
stats((690, 490, 1024, 740), "clip/part/line/rect/shadow")
stats((0, 740, 1024, 768), "chu")
stats((400, 300, 624, 470), "LookAt giua")
W, H = a.size
d = ImageChops.difference(a, b).point(lambda v: min(255, v * 4))
out = Image.new("RGB", (W * 3 + 20, H), (40, 40, 40)); out.paste(a, (0, 0)); out.paste(b, (W + 10, 0)); out.paste(d, (2 * W + 20, 0))
out.save(os.path.join(here, "rep_compare.png"))
c = Image.new("RGB", (1024, 768 * 2 + 10), (40, 40, 40)); c.paste(a, (0, 0)); c.paste(b, (0, 778)); c.save(os.path.join(here, "rep_stack.png"))
a.crop((0, 0, 700, 600)).save(os.path.join(here, "r2_c1.png")); b.crop((0, 0, 700, 600)).save(os.path.join(here, "r3_c1.png"))
a.crop((0, 600, 1024, 768)).save(os.path.join(here, "r2_c2.png")); b.crop((0, 600, 1024, 768)).save(os.path.join(here, "r3_c2.png"))
print("saved")
