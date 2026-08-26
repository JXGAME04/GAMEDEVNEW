# -*- coding: utf-8 -*-
"""C54 - chu game: "bo tui duoc pham di" (bang thuong boss sat thu cap 90).
Chu game chon: 68,2639% o quay do DOI THANH DIEM KINH NGHIEM, giu nguyen ti le
moi vat pham khac.

TRUOC (kill_level.lua OnFinishKillerTask, bang tbAward[1] - quay 1 mon):
    {szName="Tui Duoc Pham", tbProp={6,1,4813,1,0,0}, nCount=1, nRate=68.2639}
SAU:
    {szName="Diem kinh nghiem", nExp=HD_CFG("HD3_ST_EXP_QUAY",5000000), nRate=68.2639}

Vi sao thay CHO CHO chu khong xoa: tbAwardTemplet:GivByRandom (awardtemplet.lua:17-30)
quay tren tong nRate cong don; xoa han mot muc 68% se lam 68% luot quay khong trung
muc nao va nguoi choi KHONG NHAN GI. Thay tai cho thi ti le moi mon con lai giu
nguyen tuyet doi (Tinh Tinh Khoang 1,04%, Dai Luc/Phi Toc hoan 15% moi thu...).

Muc thuong CO DINH 10 trieu exp (tbAward[2]) van giu nguyen - nguoi choi nhan CA HAI.
Chinh so exp cua o quay: khoa HD3_ST_EXP_QUAY trong cauhinh_hoatdong.lua.
Doi han ca bang: khoa HD3_ST_THUONG (da co san).
"""
import io, os, re, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# ---------------------------------------------------------------- 1) doi muc
rel = r"script\task\tollgate\killer\kill_level.lua"
p = os.path.join(SRV, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_ST_EXP_QUAY" in d:
    print("1) kill_level.lua: da va roi")
else:
    rx = re.compile(r'\{szName="[^"]*",tbProp=\{6,1,4813,1,0,0\},nCount=1,nRate=([\d.]+)\},')
    m = rx.search(d)
    assert m, "khong tim thay muc Tui duoc pham 6,1,4813"
    nRate = m.group(1)
    moi = ('{szName="' + V("Điểm kinh nghiệm") + '",nExp=HD_CFG("HD3_ST_EXP_QUAY",5000000)'
           ',nRate=' + nRate + '},')
    d = d[:m.start()] + moi + d[m.end():]
    nl = "\r\n" if "\r\n" in d else "\n"
    ghichu = nl.join([
        "\t\t\t\t-- [3HD 25/08 C54] Chu game yeu cau BO Tui duoc pham khoi bang thuong.",
        "\t\t\t\t-- THAY TAI CHO (khong xoa han): GivByRandom quay tren tong nRate cong",
        "\t\t\t\t-- don nen xoa mot muc 68,26%% se lam 68,26%% luot quay khong trung gi.",
        "\t\t\t\t-- Thay bang diem kinh nghiem => ti le moi mon con lai giu nguyen tuyet doi.",
    ])
    neo = moi
    d = d.replace(neo, ghichu + nl + "\t\t\t\t" + neo.lstrip("\t"), 1)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    print("1) da thay Tui duoc pham (nRate=%s) bang Diem kinh nghiem" % nRate)

# ---------------------------------------------------------------- 2) khoa cau hinh
rel = r"script\header\cauhinh_hoatdong.lua"
p = os.path.join(SRV, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "HD3_ST_EXP_QUAY" in d:
    print("2) cauhinh: da co khoa")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    neo = "HD3_ST_SO_LENH"
    i = d.find(neo)
    assert i >= 0, "khong tim thay neo HD3_ST_SO_LENH"
    # chen TRUOC dong chua HD3_ST_SO_LENH
    j = d.rfind(nl, 0, i) + len(nl)
    them = nl.join([
        "-- So kinh nghiem cua o quay thay cho Tui duoc pham trong bang thuong boss",
        "-- sat thu cap 90 (chiem 68,2639% luot quay). Muc 10 trieu CO DINH van giu.",
        "HD3_ST_EXP_QUAY = 5000000,",
        "",
        "",
    ])
    d = d[:j] + them + d[j:]
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    print("2) da them khoa HD3_ST_EXP_QUAY = 5000000")
