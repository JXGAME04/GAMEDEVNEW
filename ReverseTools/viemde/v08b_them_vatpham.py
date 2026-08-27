# -*- coding: utf-8 -*-
"""VIEM DE - buoc 4d (LAM LAI): them 5 vat pham JX1 chua co vao magicscript.txt.

=========================== VI SAO PHAI LAM LAI ==============================
Ban dau toi CAT 30 cot dau cua hang ban Linux roi dan thang vao JX1. SAI:
hai bang KHAC NGHIA COT tu cot 9 tro di.

  cot | JX1 (KBASICPROP_MAGICSCRIPT, KBasPropTbl.cpp:1004) | ban Linux
  ----+-----------------------------------------------------+-------------------
   9  | Script       (CHUOI: duong dan .lua hoac "0")        | ngu hanh
   10 | Price        (so)                                    | gia
   11 | bShortKey    (so)                                    | cap
   12 | nMaxStack    (so - so luong xep chong)               | co xep chong 0/1
   13 | nPickExecute (SO)                                    | TEN SCRIPT (chuoi!)
   14 | nParam       (so)                                    | ID ky nang

Hau qua: cot 13 cua JX1 la SO nhung nhan chuoi "\\script\\item\\noscript.lua".
KBasPropTbl.cpp LoadRecord goi pTF->GetInteger(...) cho cot do; that bai thi
  { _ASSERT(FALSE); bEC = FALSE; break; }
tuc BO DO viec nap ban ghi -> bang vat pham hong -> GameServer DUMP.

Cach lam dung (lan nay): dung tung o THEO NGHIA COT CUA JX1, lay mau tu chinh
cac mon anh em da co trong bang JX1 (4858 Do Pho Dang Long Y, 4862 Do Pho Tinh
Suong Y, 4860 Tinh Suong Lenh): 15 cot, Script="0", bShortKey=1, nPickExecute=0,
nParam rong. So luong xep chong lay tu cot 20 cua ban Linux (物品最大叠放值),
KHONG phai cot 12 (chi la co 0/1).

Rieng "Hop Mat Na Chien Truong" co script that o ban Linux
(\\script\\vng_event\\item\\boxmask\\mask_param.lua) nen cot Script tro toi ban
port JX1 do v16_gen_maskbox.py sinh ra.
==============================================================================
"""
import os
import shutil
import struct
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import Bang, uni2tcvn, tcvn2uni, so_sanh_byte

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

SRV_ITEM = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
CLI_ITEM = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\item\magicscript.txt"
LNX_ITEM = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SPR_NGUON = r"D:\ServerLinux\Patch"
BAK = ".truoc_viemde_2608"

SCRIPT_HOPMATNA = "\\script\\item\\viemde\\mask_param.lua"

# ma Linux -> (script JX1 cho cot 9)   ("0" = khong co script, giong noscript.lua)
CAN_TAO = [
    ("6,1,30350", SCRIPT_HOPMATNA),   # Hop Mat Na Chien Truong  (co script that)
    ("6,1,30538", "0"),               # Dang Long Thach - Ha
    ("6,1,30008", "0"),               # Do Pho Tinh Suong Yeu Dai
    ("6,1,30011", "0"),               # Do Pho Tinh Suong Ngoc Boi
    ("6,1,30534", "0"),               # Do Pho Dang Long Boi
]

# chi so cot ben BAN LINUX
LX_TEN, LX_GENRE, LX_DETAIL, LX_PART = 0, 1, 2, 3
LX_ANH, LX_OBJ, LX_W, LX_H, LX_INTRO = 4, 5, 6, 7, 8
LX_GIA = 10
LX_XEPCHONG_MAX = 20      # 物品最大叠放值  <- DUNG CAI NAY, khong phai cot 12


def pid(name):
    n = name[1:] if name[:1] in ("\\", "/") else name
    pb = bytes(b + 32 if 65 <= b <= 90 else b for b in ("\\" + n).encode("latin-1"))
    i = 0
    idx = 0
    for b in pb:
        idx += 1
        c = b + 32 if 65 <= b <= 90 else b
        i = ((((i + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF) & 0xFFFFFFFF
    return i ^ 0x12345678


def nap_pak():
    s = set()
    root = CLI + r"\data"
    for f in sorted(os.listdir(root)):
        if not f.lower().endswith(".pak"):
            continue
        try:
            fh = open(os.path.join(root, f), "rb")
        except Exception:
            continue
        head = fh.read(32)
        if head[:4] != b"PACK":
            fh.close()
            continue
        cnt, itoff, _ = struct.unpack_from("<III", head, 4)
        fh.seek(itoff)
        raw = fh.read(16 * cnt)
        fh.close()
        for k in range(cnt):
            s.add(struct.unpack_from("<I", raw, 16 * k)[0])
    return s


def doc_linux():
    ra = {}
    for r in open(LNX_ITEM, "rb").read().split(b"\n")[1:]:
        c = dec2.decline2(r.rstrip(b"\r")).split("\t")
        if len(c) < 21:
            continue
        ra["%s,%s,%s" % (c[1], c[2], c[3])] = c
    return ra


def mau_anh_em(rows):
    """lay mot hang JX1 cung ho lam mau de doi chieu hinh dang"""
    for r in rows[1:]:
        if len(r) >= 14 and r[1] == "6" and r[2] == "1" and r[3] == "4862":
            return r
    return None


def main():
    lnx = doc_linux()
    bsrv = Bang(SRV_ITEM)
    bcli = Bang(CLI_ITEM)
    nCotTieuDe = len(bsrv.rows[0])
    mau = mau_anh_em(bsrv.rows)
    assert mau, "khong tim thay hang mau 6,1,4862"
    nCot = len(mau)                      # 15 - dung y het anh em
    print("hang mau JX1 (6,1,4862 Do Pho Tinh Suong Y): %d cot" % nCot)
    print("   Script=%r  ShortKey=%r  nMaxStack=%r  PickExecute=%r  nParam=%r"
          % (mau[9], mau[11], mau[12], mau[13], mau[14] if len(mau) > 14 else "-"))
    print()

    if any(len(r) > 3 and r[1] == "6" and r[2] == "1" and r[3] in ("4865", "4866", "4867", "4868", "4869")
           for r in bsrv.rows[1:]):
        print("Da co ma 4865-4869 trong bang - bo qua (chay lai khong nhan ban).")
        return

    # ma trong ke tiep
    dung = set()
    for c in bsrv.rows[1:]:
        if len(c) > 3 and c[1] == "6" and c[2] == "1":
            try:
                dung.add(int(c[3]))
            except ValueError:
                pass
    ke = max(dung) + 1
    print("cap ma moi tu %d" % ke)

    pak = nap_pak()
    them, tra, chep = [], [], []
    for ma, script in CAN_TAO:
        L = lnx.get(ma)
        assert L, "khong thay %s o ban Linux" % ma
        anh = L[LX_ANH]
        if pid(anh) not in pak and not os.path.isfile(CLI + anh):
            nguon = SPR_NGUON + anh
            assert os.path.isfile(nguon), "thieu anh %s" % anh
            chep.append((nguon, CLI + anh))
        r = [""] * nCot
        r[0] = uni2tcvn(L[LX_TEN].strip())
        r[1] = "6"
        r[2] = "1"
        r[3] = str(ke)
        r[4] = L[LX_ANH]
        r[5] = L[LX_OBJ] or "41"
        r[6] = L[LX_W] or "1"
        r[7] = L[LX_H] or "1"
        r[8] = uni2tcvn(L[LX_INTRO].strip())
        r[9] = script                       # CHUOI: duong dan .lua hoac "0"
        r[10] = L[LX_GIA] or "0"
        r[11] = "1"                         # bShortKey - moi hang JX1 deu 1
        r[12] = L[LX_XEPCHONG_MAX] or "0"   # so luong xep chong (cot 20 ban Linux)
        r[13] = "0"                         # nPickExecute - SO
        if nCot > 14:
            r[14] = ""                      # nParam - de rong y het anh em
        # moi o phai la SO hoac CHUOI khong chua tab/xuong dong
        for i, v in enumerate(r):
            assert "\t" not in v and "\n" not in v and "\r" not in v, (ma, i)
        them.append(r)
        tra.append((ma, "6,1,%d" % ke, L[LX_TEN].strip(), anh, script, r[12]))
        ke += 1

    for nguon, dich in chep:
        os.makedirs(os.path.dirname(dich), exist_ok=True)
        shutil.copyfile(nguon, dich)
        print("  chep anh: %s (%d B)" % (dich[len(CLI):], os.path.getsize(dich)))

    for b in (bsrv, bcli):
        while b.rows and len(b.rows[-1]) <= 1 and not b.rows[-1][0]:
            b.rows.pop()
        b.rows.extend([list(x) for x in them])
        n = b.ghi(BAK)
        print("  da ghi %s (+%d dong, %d byte)" % (b.path[-46:], len(them), n))

    print()
    print("=== BANG TRA ===")
    for a, bb, t, anh, sc, st in tra:
        print("   %-11s -> %-10s %-28s stack=%-4s script=%s" % (a, bb, t[:27], st, sc))

    # ================= KIEM LAI: doc lai DUNG NHU ENGINE DOC =================
    print()
    print("=== KIEM LAI (mo phong KBasPropTbl::LoadRecord) ===")
    KIEU = ["char", "int", "int", "int", "char", "int", "int", "int",
            "char", "char", "int", "int", "int", "int", "int"]
    loi = 0
    for p in (SRV_ITEM, CLI_ITEM):
        rows = Bang(p).rows
        for r in rows[1:]:
            if not (len(r) > 3 and r[1] == "6" and r[2] == "1" and r[3] in
                    ("4865", "4866", "4867", "4868", "4869")):
                continue
            for i, kieu in enumerate(KIEU):
                v = r[i] if i < len(r) else ""
                if kieu == "int" and v.strip() != "":
                    try:
                        int(v)
                    except ValueError:
                        print("   !! %s cot%d (%s) KHONG PHAI SO: %r" % (r[3], i, kieu, v))
                        loi += 1
        print("   %-6s so cot moi hang moi: %s" % (
            "server" if "server" in p else "client",
            sorted(set(len(r) for r in rows[1:]
                       if len(r) > 3 and r[3] in ("4865", "4866", "4867", "4868", "4869")))))
    print("   so o SAI KIEU: %d %s" % (loi, "(dung)" if loi == 0 else "<< PHAI SUA"))
    for p in (SRV_ITEM, CLI_ITEM):
        kq = so_sanh_byte(p, BAK, len(CAN_TAO))
        print("   KIEM BYTE %-6s CRLF %d->%d  LFdon %d->%d  FFFD=%d"
              % ("server" if "server" in p else "client",
                 kq["crlf_truoc"], kq["crlf_sau"], kq["lf_don_truoc"], kq["lf_don_sau"], kq["fffd"]))
        assert kq["eol_giu_nguyen"] and kq["fffd"] == 0
    assert loi == 0
    print()
    print("*** PHAI KHOI DONG LAI MAY CHU ***")


main()
