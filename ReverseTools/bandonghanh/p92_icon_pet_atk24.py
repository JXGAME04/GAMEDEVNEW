# -*- coding: utf-8 -*-
r"""[PETKN3 01/09] Sinh icon 24x24 cho 5 chieu danh cua pet (theo he chu).
O Skill_1 cua cua so pet chi 26x26, engine ve SPR NATIVE khong scale
(KSkill::DrawSkillIcon), icon chieu nguoi choi goc 36x36 -> tran o ("qua to").
Doc icon goc tu cot SkillIcon (cot 6) cua skills.txt client, thu nho ve 24x24
(giu palette goc, khop bo icon pet 24x24 co san) va ghi ra
  \spr\Ui3\pet\atkskill\atk_<id>.spr   (ten ASCII, client)
UiPet.cpp (KWndPetAtkBox) ve file nay thay icon goc; tooltip/hover giu nguyen.
Chay lai duoc (ghi de). KHONG dung den file icon goc.
"""
import io, os, struct, sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")
from p15_vietsub_spr import Spr          # bo doc/ghi SPR da kiem 28/08
from PIL import Image

CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
IDS = ["327", "437", "336", "357", "438"]   # Kim Moc Thuy Hoa Tho (KPlayerPet.cpp s_nPetAtkBySeries)
OUT_DIR = os.path.join(CL, r"spr\Ui3\pet\atkskill")
SIZE = 24


def duong_dan_icon():
    kq = {}
    for line in io.open(os.path.join(CL, r"settings\skills.txt"), "r", encoding="latin-1"):
        c = line.rstrip("\r\n").split("\t")
        if len(c) > 6 and c[2] in IDS:
            kq[c[2]] = os.path.join(CL, c[5].lstrip("\\"))
    return kq


def ghi_spr(spr, path):
    """save() cua p15 nhung ghi ra PATH MOI (khong backup .truoc_vietsub)."""
    frames_blob = []
    for img in spr.imgs:
        px = img.load()
        rle = bytearray()
        for y in range(spr.h):
            x = 0
            while x < spr.w:
                n = 0
                while x + n < spr.w and px[x + n, y][3] < 64 and n < 255:
                    n += 1
                if n:
                    rle += bytes((n, 0))
                    x += n
                    continue
                n = 0
                while x + n < spr.w and px[x + n, y][3] >= 64 and n < 255:
                    n += 1
                rle += bytes((n, 255))
                for k in range(n):
                    r, gg, bb, a = px[x + k, y]
                    rle.append(spr.near((r, gg, bb)))
                x += n
        frames_blob.append(bytes(rle))
    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", spr.w, spr.h, spr.cx, spr.cy,
                       spr.frames, spr.colors, spr.dirs, spr.itv)
    out += spr.reserved
    for r, gg, bb in spr.pal:
        out += bytes((r, gg, bb))
    out += spr.pad
    off = 0
    for blob in frames_blob:
        out += struct.pack("<II", off, len(blob) + 8)
        off += len(blob) + 8
    for blob in frames_blob:
        out += struct.pack("<HHHH", spr.w, spr.h, 0, 0)
        out += blob
    open(path, "wb").write(bytes(out))


if __name__ == "__main__":
    os.makedirs(OUT_DIR, exist_ok=True)
    dd = duong_dan_icon()
    for sid in IDS:
        src = dd.get(sid)
        assert src and os.path.exists(src), "khong thay icon goc cua skill %s: %r" % (sid, src)
        spr = Spr(src)
        w0, h0 = spr.w, spr.h
        moi = []
        for img in spr.imgs:
            moi.append(img.resize((SIZE, SIZE), Image.LANCZOS))
        spr.imgs = moi
        spr.cx = spr.cx * SIZE // max(1, w0)
        spr.cy = spr.cy * SIZE // max(1, h0)
        spr.w = spr.h = SIZE
        dst = os.path.join(OUT_DIR, "atk_%s.spr" % sid)
        ghi_spr(spr, dst)
        # kiem lai bang chinh bo doc
        chk = Spr(dst)
        print("skill %s: %dx%d -> %dx%d frames=%d -> %s (%d byte, doc lai %dx%d OK)" % (
            sid, w0, h0, SIZE, SIZE, spr.frames, dst, os.path.getsize(dst), chk.w, chk.h))
    print("XONG")
