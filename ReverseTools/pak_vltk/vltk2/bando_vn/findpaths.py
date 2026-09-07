# -*- coding: utf-8 -*-
"""Do tim DUONG DAN cua 6 anh ban do 752x576 trong update.pak cua VLTK 2.0.
Bam pak chay tren BYTE GBK => ghep byte, khong ghep chuoi Unicode.
"""
import sys, itertools
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
from pakdump import name2id

WANT = {
    0x278A68BA: "update_278A68BA",
    0x2C3D0BA1: "update_2C3D0BA1 (ban do TQ)",
    0x371420E4: "update_371420E4",
    0x43841ADC: "update_43841ADC (ban do VN kieu 2)",
    0x88D08A38: "update_88D08A38 (son dong VN)",
    0xA7A36384: "update_A7A36384 (ban do VN kieu 1)",
    0xC7C05D3B: "res2 剑侠大地图2 (the gioi TQ)",
    0x9E2787EB: "res2 打怪向导山洞图 (son dong TQ)",
}

def H(b):
    return name2id(b.decode("latin-1"))

BS = b"\\"
roots = [b"\\Spr\\Ui3\\", b"\\spr\\Ui3\\", b"\\Spr\\ui3\\", b"\\spr\\ui3\\",
         b"\\Spr\\Ui4\\", b"\\spr\\Ui4\\", b"\\Spr\\", b"\\spr\\"]
dirs_cn = ["小地图", "世界地图", "大地图", "地图"]
bases = [
    "剑侠大地图2", "剑侠大地图", "打怪向导山洞图", "打开山洞地图",
    "江山社稷图2", "江山社稷图", "山河社稷图", "剑网山洞迷宫分布图",
]
# VNG hay them hau to 'vn' (vd 寻路vn.spr), va cac bien the so
suffixes = ["", "vn", "VN", "_vn", "vn2", "vn1", "2", "3", "new", "moi"]

found = {}
tries = 0
for root in roots:
    for d in dirs_cn:
        for base in bases:
            for suf in suffixes:
                for ext in (".spr", ".SPR"):
                    try:
                        p = root + d.encode("gbk") + BS + (base + suf).encode("gbk") + ext.encode()
                    except Exception:
                        continue
                    tries += 1
                    h = H(p)
                    if h in WANT and h not in found:
                        found[h] = p
                        print("TRUNG %08X  %-34s  %s" % (h, WANT[h], p.decode("gbk", "replace")))

# them: ten tieng Viet / ascii
vnbases = ["bandothegioi", "bando", "worldmap", "worldmapvn", "sonhaxatac", "sonha",
           "mecungsondong", "sondong", "cavemap", "cavemapvn", "bandovn", "banso"]
for root in roots:
    for d in dirs_cn + ["", "ban do", "bando"]:
        for base in vnbases:
            for suf in ("", "vn", "2"):
                seg = (d.encode("gbk") + BS) if d else b""
                p = root + seg + (base + suf).encode("latin-1") + b".spr"
                tries += 1
                h = H(p)
                if h in WANT and h not in found:
                    found[h] = p
                    print("TRUNG %08X  %-34s  %s" % (h, WANT[h], p.decode("gbk", "replace")))

print("\nda thu %d duong dan | tim ra %d/%d" % (tries, len(found), len(WANT)))
for h, lab in WANT.items():
    if h not in found:
        print("   CHUA RA: %08X  %s" % (h, lab))
