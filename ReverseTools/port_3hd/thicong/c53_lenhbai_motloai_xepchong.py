# -*- coding: utf-8 -*-
"""C53 - chu game: "nhung ra 2 loai lenh bai bac dau nhu tren hinh va item do co
xep chong duoc khong".

DO THAT (settings\\item\\magicscript.txt cot 13 nMaxStack; engine KItem.cpp:3021
gop chong khi GetStackNum() < GetMaxStackNum() nen nMaxStack = 0 la KHONG chong):
    6,1,4126 Lenh bai Bac Dau                nMaxStack = 50  -> CHONG duoc
    6,1,4127..4139 (13 loai theo hoat dong)  nMaxStack = 0   -> KHONG chong
    6,1,398 Sat Thu lenh / 399 Sat thu gian  nMaxStack = 0   -> KHONG chong
      (hai cai nay BUOC PHAI vay: moi cai mang CAP + NGU HANH rieng, chong lai
       se mat thong tin do - ban Linux cung de 0)

=> moi con boss cap 90 dang cho 2 lenh bai, trong do cai "- Boss sat thu" khong
   xep chong nen an 1 o tui moi con.

VA (hai lop):
 1) MAC DINH chi phat MOT loai - "Lenh bai Bac Dau" (chong 50/o, la ma DUY NHAT
    dung de doi thuong 15/20). Khoa moi HD3_BD_LENHBAI_RIENG = 0.
    Dat = 1 thi phat them lenh bai rieng cua hoat dong nhu ban Linux.
 2) Doi nMaxStack cua 13 ma 4127..4139 tu 0 -> 50 (ca server LAN client), de neu
    chu game bat HD3_BD_LENHBAI_RIENG = 1 thi tui do khong bi day.
    KHONG dung toi 398/399 (phai giu 0 vi mang cap + ngu hanh).
"""
import io, os, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

MA = [str(x) for x in range(4127, 4140)]   # 13 lenh bai rieng cua hoat dong

# ------------------------------------------------- 1) nMaxStack 0 -> 50
for goc in (SRV, CLI):
    q = os.path.join(goc, r"settings\item\magicscript.txt")
    if not os.path.exists(q + ".truoc_c53_2508"):
        shutil.copyfile(q, q + ".truoc_c53_2508")
    dd = io.open(q, encoding="latin-1", newline="").read().split("\n")
    n = 0
    for i, l in enumerate(dd):
        c = l.split("\t")
        if len(c) > 12 and c[1] == "6" and c[2] == "1" and c[3] in MA and c[12] != "50":
            c[12] = "50"
            dd[i] = "\t".join(c)
            n += 1
    if n:
        io.open(q, "w", encoding="latin-1", newline="").write("\n".join(dd))
    print("1) bang item %s: doi %d dong nMaxStack 0 -> 50"
          % ("server" if goc == SRV else "client", n))
shutil.copyfile(os.path.join(SRV, r"settings\item\magicscript.txt"),
                os.path.join(MIR, r"settings\item\magicscript.txt"))

# ------------------------------------------------- 2) khoa cau hinh
rel = r"script\header\cauhinh_hoatdong.lua"
p = os.path.join(SRV, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_BD_LENHBAI_RIENG" in d:
    print("2) cauhinh: da co khoa")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    neo = "HD3_BD_QUYDOI_LENHBAI = 1,"
    assert d.count(neo) == 1, d.count(neo)
    d = d.replace(neo, nl.join([
        neo,
        "",
        "-- Co phat them 'Lenh bai Bac Dau - <ten hoat dong>' rieng khong.",
        "-- 0 = KHONG (mac dinh): moi hoat dong chi cho 1 'Lenh bai Bac Dau' - ma nay",
        "--     xep chong 50 cai/o va la ma DUY NHAT dung de doi thuong 15/20.",
        "-- 1 = CO, dung y ban Linux: cho them 1 lenh bai rieng cua hoat dong (chi de",
        "--     suu tap, khong doi thuong duoc) - nguoi choi se nhan 2 vat pham/lan.",
        "HD3_BD_LENHBAI_RIENG = 0,",
    ]))
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    print("2) da them khoa HD3_BD_LENHBAI_RIENG = 0 vao cauhinh_hoatdong.lua")
