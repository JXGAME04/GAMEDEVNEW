# -*- coding: utf-8 -*-
"""VIEM DE - buoc 5: them cac NPC template cua tinh nang vao settings\\npcs.txt cua JX1.

Nguon: D:\\ServerLinux\\Patch\\settings\\npcs.txt  (npcs.txt cua CLIENT ban VNG/Linux)

Hai bang THANG HANG TUNG DONG (da kiem: dong 1338 ca hai deu la "Cong Thanh Chien Xa")
va le id la:  id = SO DONG DU LIEU - 1   (kiem bang moc da biet: id 769 = "Nhiep Thi Tran"
nam o dong 770). Ngoai ra ngu hanh cua 10 boss doc tu dong (N+1) trung KHOP TUNG CAI
voi bang YDBZ_map_npc trong npc.lua -> chot le chac chan.

JX1 co 87 cot, ban Linux co 103; 87 cot dau TRUNG TEN HOAN TOAN nen chi can cat bot.

An toan: chi ghi de nhung dong dang la "<<chua dinh nghia>>" o JX1. Gap dong da co
du lieu thi DUNG LAI, khong ghi gi.
"""
import io
import os
import re
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import Bang, uni2tcvn, tcvn2uni, so_sanh_byte

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\npcs.txt"
LNX = r"D:\ServerLinux\Patch\settings\npcs.txt"
FEAT = r"D:\ServerLinux\server1\script\missions\yandibaozang"
BAK = ".truoc_viemde_2608"

REV = {}
for cp in range(0x20, 0x2000):
    ch = chr(cp)
    try:
        b = unicode_to_tcvn3_bytes(ch)
    except Exception:
        continue
    if len(b) == 1 and b[0] >= 0x80:
        REV.setdefault(b[0], ch)


def tv(s):
    return "".join(REV.get(ord(c), c) if ord(c) >= 0x80 else c for c in s)


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


# ---------- 1. lay danh sach id NPC ma tinh nang dung ----------
RE_NPC = re.compile(r"\{\s*(?:nil|[A-Za-z_][\w.]*)\s*,\s*(\d{2,5})\s*,\s*\"")
RE_ADDNPC = re.compile(r"AddNpc(?:Ex)?\s*\(\s*(\d+)")


def quet_id():
    ra = {}
    for dp, dn, fn in os.walk(FEAT):
        for f in fn:
            if not f.endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            for i, l in enumerate(open(p, "rb").read().split(b"\n"), 1):
                s = dec2.decline2(l.rstrip(b"\r"))
                if s.lstrip().startswith("--"):
                    continue
                for m in RE_NPC.finditer(s):
                    ra.setdefault(int(m.group(1)), []).append("%s:%d" % (f, i))
                for m in RE_ADDNPC.finditer(s):
                    ra.setdefault(int(m.group(1)), []).append("%s:%d" % (f, i))
    return ra


def main():
    ids = quet_id()
    bang = Bang(JX1)                 # giu nguyen CRLF, ghi qua tep tam
    jr = bang.rows
    lr = [dec2.decline2(r.rstrip(b"\r")).split("\t") for r in open(LNX, "rb").read().split(b"\n")]
    nCot = len(jr[0])
    assert jr[0] == lr[0][:nCot], "tieu de 87 cot dau KHONG trung - dung lai"

    print("JX1 %d dong / %d cot   |   LINUX %d dong / %d cot" % (len(jr), nCot, len(lr), len(lr[0])))
    print()
    print("=== NPC ma tinh nang dung (id -> tinh trang) ===")
    can_them, da_co, thieu_nguon, dung_lai = [], [], [], []
    for i in sorted(ids):
        dong = i + 1                       # id = so dong du lieu - 1
        if dong >= len(jr) or dong >= len(lr):
            thieu_nguon.append(i)
            continue
        ten_j = tv(jr[dong][0])
        ten_l = lr[dong][0]
        trong_j = ten_j.startswith("<<")
        co_l = len(lr[dong]) >= nCot and not (ten_l.strip() == "" )
        if not trong_j:
            da_co.append((i, ten_j))
        elif co_l:
            can_them.append((i, dong, ten_l))
        else:
            thieu_nguon.append(i)
    for i, t in da_co:
        print("   id=%-5d JX1 DA CO: %-30s  (%s)" % (i, t[:29], ", ".join(sorted(set(ids[i]))[:2])))
    for i, d, t in can_them:
        print("   id=%-5d se THEM tu ban Linux dong %-5d res=%-9s (%s)"
              % (i, d, lr[d][11] if len(lr[d]) > 11 else "?", ", ".join(sorted(set(ids[i]))[:2])))
    for i in thieu_nguon:
        print("   id=%-5d !! KHONG CO O CA HAI BEN  (%s)" % (i, ", ".join(sorted(set(ids[i]))[:2])))

    if not can_them:
        print()
        print("Khong co gi de them.")
        return

    # ---------- ghi ----------
    n = 0
    for i, dong, ten_l in can_them:
        moi = lr[dong][:nCot]
        while len(moi) < nCot:
            moi.append("")
        # ten: ban Linux de "<<chua dich>>". Kich ban Viem De TRUYEN TEN o tham so 7
        # cua AddNpc nen ten trong bang chi hien khi co ai sinh NPC ma khong dat ten.
        # Dat mot ten co nghia thay vi de "<<chua dich>>".
        moi = [uni2tcvn(x) for x in moi]
        if ten_l.strip().startswith("<<"):
            moi[0] = uni2tcvn("Viêm Đế - quái %d" % i)
        else:
            moi[0] = uni2tcvn(ten_l)
        jr[dong] = moi
        n += 1
    bang.ghi(BAK)
    print()
    print("DA THEM %d NPC template. Sao luu: %s" % (n, os.path.basename(JX1 + BAK)))

    # ---------- kiem lai ----------
    jr2 = Bang(JX1).rows
    assert len(jr2) == len(jr), "so dong thay doi!"
    assert all(len(r) == nCot for r in jr2 if len(r) > 1), "co dong lech so cot!"
    ok = sum(1 for i, d, t in can_them if not tv(jr2[d][0]).startswith("<<"))
    kq = so_sanh_byte(JX1, BAK, len(can_them))
    print("KIEM BYTE:", kq)
    assert kq["eol_giu_nguyen"], "KIEU XUONG DONG BI DOI!"
    assert kq["fffd"] == 0, "co ky tu hong!"
    print("KIEM LAI: %d/%d dong da co ten, tong so dong %d (khong doi), moi dong du %d cot"
          % (ok, len(can_them), len(jr2), nCot))


main()
