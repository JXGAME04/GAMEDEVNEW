# -*- coding: utf-8 -*-
"""kiem_res_critter.py - kiem tung buoc chuoi nap anh cho NPC trang tri cua map 53:
  1. NpcS.txt -> cot NpcResType (critterNNN)
  2. \\settings\\npcres\\npc_res_kind_file_name.txt -> co hang khong, ResFilePath la gi
  3. \\settings\\npcres\\npc_normal_res_file.txt -> co hang khong, ten .spr cua tung hanh dong
  4. chinh tep .spr do co trong pak khong
Chi doc.
"""
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS  # noqa: E402

MAU = [418, 419, 430, 434, 436, 437, 439, 442]
NPCS = BS + "settings" + BS + "NpcS.txt"
KIND = BS + "settings" + BS + "npcres" + BS + "npc_res_kind_file_name.txt"
NORM = BS + "settings" + BS + "npcres" + BS + "npc_normal_res_file.txt"


def bang(k, ten):
    d = k.doc(ten.encode("ascii"))
    if not d:
        return None, None
    ls = d.replace(b"\r\n", b"\n").split(b"\n")
    hdr = [c.strip().decode("gbk", "replace") for c in ls[0].split(b"\t")]
    hang = {}
    for l in ls[1:]:
        c = l.split(b"\t")
        if c and c[0].strip():
            hang[c[0].strip().decode("gbk", "replace")] = [x.decode("gbk", "replace") for x in c]
    return hdr, hang


def main():
    k = Kho(DUAN)
    d = k.doc(NPCS.encode("ascii"))
    ls = d.replace(b"\r\n", b"\n").split(b"\n")
    hdr = [c.strip().decode("gbk", "replace") for c in ls[0].split(b"\t")]
    ci = hdr.index("NpcResType")
    hk, kind = bang(k, KIND)
    hn, norm = bang(k, NORM)
    print("bang kind %d hang | bang normal %d hang" % (len(kind or {}), len(norm or {})))
    for tid in MAU:
        c = ls[tid + 1].split(b"\t")
        ten = c[0].decode("gbk", "replace")[:14]
        res = c[ci].strip().decode("gbk", "replace")
        rk = kind.get(res)
        rn = norm.get(res)
        duong = rk[2] if rk and len(rk) > 2 else ""
        print("\nmau %-4d %-14s res=%-11s kind=%-9s normal=%-9s duong=%s" % (
            tid, ten, res, "CO" if rk else "THIEU", "CO" if rn else "THIEU", duong))
        if rn and duong:
            co = thieu = 0
            vd = []
            for j, v in enumerate(rn[1:], 1):
                v = v.strip()
                if not v or not v.lower().endswith(".spr"):
                    continue
                p = BS + duong.strip(BS) + BS + v if not v.startswith(BS) else v
                dd = k.doc(p.encode("gbk", "replace"))
                if dd:
                    co += 1
                    if len(vd) < 2:
                        vd.append("%s %d B" % (v, len(dd)))
                else:
                    thieu += 1
                    if len(vd) < 2:
                        vd.append("%s THIEU" % v)
            print("      anh: %d co / %d thieu | %s" % (co, thieu, "; ".join(vd)))


if __name__ == "__main__":
    main()
