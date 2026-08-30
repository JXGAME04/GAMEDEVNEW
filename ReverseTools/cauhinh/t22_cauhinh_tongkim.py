# -*- coding: utf-8 -*-
"""t22_cauhinh_tongkim.py - noi day cau hinh TONG KIM.

⚠️ PHAT HIEN QUAN TRONG (da tu doc va doi chieu dau thoi gian tep):
   script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua duoc luu luc 28/08 23:45:56,
   va khung gio dau tien trong bang lich la {23,46,...} - tuc dat de no sau
   DUNG 4 GIAY. Day la CAU HINH THU NGHIEM BI BO QUEN, khong phai lich that.

   Nang hon: ba con so thoi gian deu lech han voi chu thich ngay canh chung:
       :56  TIME_BD_TK    = 1   -- chu thich ghi "10 phut bao danh"
       :57  TIME_KT_TK    = 30  -- chu thich ghi "70 phut chien dau"
       :58  TIME_NS_TK    = 1   -- chu thich ghi "30 phut ... nguyen soai"
   Nghia la nguoi choi chi co DUNG MOT PHUT de bao danh Tong Kim thay vi 10
   phut. Tong Kim la hoat dong cay chinh hang ngay nen viec nay cham toi ca
   may chu, moi ngay.

TOI KHONG TU DOI SO NAO. Ly do: khong biet so dung la bao nhieu - rieng khung
gio dau da tung co BA gia tri khac nhau (13h23 / 13h58 / 23h46), va chu thich
thi noi mot dang. Day la viec chu game phai chot.

BAN VA NAY chi dua moi con so ra tep cau hinh voi MAC DINH = DUNG gia tri dang
chay, de chu doi mot dong la xong. Ban than ban va KHONG doi gi.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P = os.path.join(S, "tinhnang", "tong_kim_tcap", "lib_tktc.lua")
P_LICH = os.path.join(S, "cauhinh", "ch_lich.lua")
NHAN = "[CFGTK 29/08]"

VA = [
    ("PLAYER_MS_LIMIT\t\t\t\t\t\t\t\t= 2000 --55 ng\xad\xeei m\xe7i phe",
     'PLAYER_MS_LIMIT\t\t\t\t\t\t\t\t= TK_CFG("TK_NGUOI_MOI_PHE", 2000)'
     " --so nguoi toi da moi phe",
     [("TK_NGUOI_MOI_PHE", 2000, "so nguoi toi da moi phe")]),
]

# ba dong thoi gian: moc ASCII an toan (khong dinh tieng Viet)
VA_SO = [
    ("TIME_BD_TK", 1, "so PHUT bao danh."
                      " !! chu thich trong ma ghi 10 - dang chay 1"),
    ("TIME_KT_TK", 30, "so PHUT ca tran."
                       " !! chu thich trong ma ghi 70 - dang chay 30"),
    ("TIME_NS_TK", 1, "so PHUT den luc ra Nguyen Soai."
                      " !! chu thich ghi 30 - dang chay 1"),
    ("TIME_XOANPC_TK", 1, "so PHUT xoa NPC sau tran"),
]
TEN_KHOA = {
    "TIME_BD_TK": "TK_PHUT_BAODANH",
    "TIME_KT_TK": "TK_PHUT_TRAN",
    "TIME_NS_TK": "TK_PHUT_NGUYENSOAI",
    "TIME_XOANPC_TK": "TK_PHUT_XOANPC",
}

HAM = """
-- %(nhan)s Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu) khi bo cau
-- hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function TK_CFG(szKhoa, macdinh)
\tif (G_CFG ~= nil) then
\t\treturn G_CFG(szKhoa, macdinh)
\tend
\treturn macdinh
end
""" % dict(nhan=NHAN)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t22_cauhinh_tongkim - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  lib_tktc.lua DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)
    nd = raw
    khoa = []

    # --- 4 dong thoi gian: tim theo TEN BIEN, giu nguyen phan chu thich ---
    for ten, mong, mota in VA_SO:
        m = re.search(r"^(%s\s*=\s*)(\d+)(.*)$" % re.escape(ten), nd, re.M)
        if not m:
            print("!!! LOI TO: khong thay dong khai %s" % ten)
            return 1
        gia_tri = int(m.group(2))
        if gia_tri != mong:
            print("!!! LOI TO: %s dang la %d, mong %d - tep da doi, dung lai"
                  % (ten, gia_tri, mong))
            return 1
        k = TEN_KHOA[ten]
        nd = nd[:m.start()] + '%sTK_CFG("%s", %d)%s' % (m.group(1), k, gia_tri,
                                                        m.group(3)) \
            + nd[m.end():]
        khoa.append((k, gia_tri, mota))
        print("  %-22s %s = %d" % (k, ten, gia_tri))

    # --- so nguoi moi phe ---
    m = re.search(r"^(PLAYER_MS_LIMIT\s*=\s*)(\d+)(.*)$", nd, re.M)
    if not m:
        print("!!! LOI TO: khong thay PLAYER_MS_LIMIT")
        return 1
    v = int(m.group(2))
    nd = nd[:m.start()] + '%sTK_CFG("TK_NGUOI_MOI_PHE", %d)%s' \
        % (m.group(1), v, m.group(3)) + nd[m.end():]
    khoa.append(("TK_NGUOI_MOI_PHE", v, "so nguoi toi da moi phe"))
    print("  %-22s PLAYER_MS_LIMIT = %d" % ("TK_NGUOI_MOI_PHE", v))

    # --- bang lich ---
    m = re.search(r"TAB_TIME_TONG_KIM = \{(.*?)\n\}", nd, re.S)
    if not m:
        print("!!! LOI TO: khong thay bang TAB_TIME_TONG_KIM")
        return 1
    than_bang = m.group(1)
    cac_khung = re.findall(r"^\s*\{([\d,\s]+)\}", than_bang, re.M)
    if not cac_khung:
        print("!!! LOI TO: bang lich khong co khung nao dang chay")
        return 1
    lich_txt = ", ".join("{%s}" % re.sub(r"\s+", "", x) for x in cac_khung)
    print("  %-22s %d khung dang chay: %s"
          % ("TK_LICH", len(cac_khung), lich_txt))
    nd = nd[:m.start()] + ("TAB_TIME_TONG_KIM = TK_CFG(\"TK_LICH\", {%s})"
                           % lich_txt) + nd[m.end():]

    # --- chen ham TK_CFG + Include ---
    m = re.search(r'^Include\("[^"]+"\)', nd, re.M)
    if not m:
        print("!!! LOI TO: khong thay dong Include nao")
        return 1
    moc_inc = m.group(0)
    nd = nd.replace(moc_inc, eol.join([
        moc_inc,
        "-- " + NHAN + " hai tep duoi day la LA (khong Include gi).",
        'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
        'Include("\\\\script\\\\cauhinh\\\\ch_lich.lua")',
    ]) + HAM.replace("\n", eol), 1)

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  => can bang tu khoa giu nguyen (%d)" % cb1)

    # ---- do vao ch_lich.lua ----
    raw_l = doc(P_LICH)
    if NHAN in raw_l:
        print("  ch_lich.lua DA VA - bo qua")
        nd_l = raw_l
    else:
        eol_l = "\r\n" if raw_l.count("\r\n") >= (raw_l.count("\n")
                                                 - raw_l.count("\r\n")) else "\n"
        dong = [
            "",
            "-- " + NHAN + " TONG KIM"
            " (script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua)",
            "--",
            "-- !! CAN CHU XEM LAI: tep lib_tktc.lua duoc luu luc 28/08 23:45:56",
            "--    va khung gio dau trong bang lich la 23h46 - tuc dat de no sau",
            "--    dung 4 giay. Rat co the do la cau hinh THU NGHIEM bi bo quen.",
            "--    Ba con so thoi gian cung lech han voi chu thich ngay canh:",
            "--      bao danh dang la 1 phut  (chu thich ghi 10)",
            "--      ca tran   dang la 30 phut (chu thich ghi 70)",
            "--      nguyen soai dang la 1 phut (chu thich ghi 30)",
            "--    Toi khong tu doi vi khong biet so dung - rieng khung gio dau",
            "--    da tung co ba gia tri khac nhau (13h23 / 13h58 / 23h46).",
            "",
            "-- Bang lich: moi khung la {gio, phut, gio_ket_thuc, so_hieu_tran}",
            "-- TK_LICH = {{17,50,18,3}, {20,50,21,4}, {22,50,23,5}},",
            "",
        ]
        for k, v, mota in khoa:
            dong.append("%-22s= %-6d,\t-- %s" % (k, v, mota))
        dong.append("TK_LICH               = {%s}," % lich_txt)
        moc = "tbCFG_LICH = {"
        if raw_l.count(moc) != 1:
            print("!!! LOI TO: ch_lich.lua khong co dung 1 moc tbCFG_LICH")
            return 1
        nd_l = raw_l.replace(moc, moc + eol_l + eol_l.join(dong))
        print("  ch_lich.lua: do %d khoa + bang lich" % len(khoa))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi, cu0 in ((P, nd, raw), (P_LICH, nd_l, raw_l)):
        if moi == cu0:
            continue
        sao = p + ".truoc_cfgtk"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(moi.encode("latin-1"))
        if doc(p) != moi:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
