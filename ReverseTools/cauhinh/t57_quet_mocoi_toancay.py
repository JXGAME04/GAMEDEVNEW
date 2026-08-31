# -*- coding: utf-8 -*-
"""t57_quet_mocoi_toancay.py - quet TOAN CAY tim tep .lua khong con duong nap.

Mot tep .lua co the duoc nap bang BAY duong. Phai kiem DU bay duong roi moi
dam goi la mo coi:
  a. Include / dofile / ReLoadScript / DynamicExecute tu tep khac
  b. Gan vao NPC: AddNpc / AddNpcNew / AddNpcEx1
  c. SetDeathScript / SetNpcDropScript / AddTrapEx1 / AddTrapEx2 / OpenGiveBox
  d. Bang settings: TimerTask.txt, item\\magicscript.txt, npc\\*, task\\*,
     droprate\\*, trigger_*.lua
  e. Quy uoc TEN THEO SO HIEU: missions\\missionNN.lua, timertask\\taskNN.lua
  f. Ma C++ goi cung duong dan (D:\\GAMEDEVNEW\\Sources)
  g. Bang chuoi trong C++ header (KHoatDongTables.h, KDaTauTables.h)

Cach so: chuan hoa (/ -> \\, gom \\\\ thanh mot, ha chu thuong), va khop theo
TEN TEP truoc roi moi xac nhan bang duong dan - vi nhieu cho viet duong dan
tuong doi hoac chi ten tep.

CHI DOC - khong sua gi.
"""
import io
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
S = os.path.join(SV, "script")
NGUON = r"D:\GAMEDEVNEW\Sources"
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "mocoi_toancay.txt")

BO_QUA_TEN = (".truoc_", ".goc", ".cu_", ".hong_", ".daap", ".bak", ".old",
              " - copy", ".tuche_")
BO_QUA_TM = ("_backup", "_gobo", "_dara", "tinhnang.rar")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan(s):
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def la_ban_sao(ten):
    t = ten.lower()
    return any(x in t for x in BO_QUA_TEN)


def main():
    # ---- 1. thu thap moi tep .lua trong cay ----
    tep = []
    for goc, tm, ts in os.walk(S):
        tm[:] = [x for x in tm
                 if not any(b in x.lower() for b in BO_QUA_TM)]
        for t in ts:
            if not t.lower().endswith(".lua") or la_ban_sao(t):
                continue
            tep.append(os.path.join(goc, t))
    print("Tong tep .lua (bo ban sao, bo thu muc sao luu): %d" % len(tep))

    # ---- 2. gom TOAN BO van ban co the chua duong nap ----
    kho = []
    n_lua = n_set = n_cpp = 0
    for p in tep:
        try:
            kho.append((os.path.relpath(p, S), doc(p)))
            n_lua += 1
        except OSError:
            pass
    for goc, _, ts in os.walk(os.path.join(SV, "settings")):
        for t in ts:
            if not t.lower().endswith((".txt", ".ini", ".lua")) or la_ban_sao(t):
                continue
            try:
                kho.append(("settings\\" + t, doc(os.path.join(goc, t))))
                n_set += 1
            except OSError:
                pass
    for goc, _, ts in os.walk(os.path.join(NGUON, "Core", "Src")):
        for t in ts:
            if not t.lower().endswith((".cpp", ".h")):
                continue
            try:
                kho.append(("CPP\\" + t, doc(os.path.join(goc, t))))
                n_cpp += 1
            except OSError:
                pass
    print("Kho van ban: %d tep .lua + %d tep settings + %d tep C++"
          % (n_lua, n_set, n_cpp))

    # ---- 3. voi tung tep, tim xem co ai nhac toi khong ----
    # Gom san: voi moi tep nguon, tap cac dong KHONG phai comment (chuan hoa)
    dong_sach = []
    for ten, noi_dung in kho:
        ds = []
        for i, l in enumerate(noi_dung.replace("\r\n", "\n").split("\n"), 1):
            if l.lstrip().startswith("--") or l.lstrip().startswith("//"):
                continue
            ds.append((i, chuan(l)))
        dong_sach.append((ten, ds))

    # ---- (h) DUONG NAP THU 8: script gan vao NPC/bay bang DU LIEU BAN DO ----
    # Ten script nam trong khoi DA NEN cua bin\server\Pak\*.pak - grep van ban
    # KHONG thay. Sinh bang t58_duong_nap_thu8_mapdata.py.
    # Thieu duong nay la bao nham hang nghin tep (lan dau toi bao 1675/3065).
    p_map = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                         "duongnap_mapdata.txt")
    ten_mapdata = set()
    if os.path.isfile(p_map):
        for l in io.open(p_map, encoding="utf-8"):
            l = l.strip().lower()
            if l.endswith(".lua"):
                ten_mapdata.add(l)
        print("Duong nap thu 8 (du lieu ban do): %d ten" % len(ten_mapdata))
    else:
        print("!! CHUA CO %s - chay t58 truoc, khong thi ket qua SAI"
              % os.path.basename(p_map))

    mo_coi = []
    for p in tep:
        rel = os.path.relpath(p, S)
        ten_tep = os.path.basename(p)
        c_ten = chuan(ten_tep)
        c_rel = chuan(rel)
        # (e) quy uoc ten theo so hieu -> coi nhu CO duong nap
        if re.match(r"^missions\\mission\d+\.lua$", c_rel) or \
           re.match(r"^timertask\\task\d+\.lua$", c_rel) or \
           re.match(r"^missions\\missionpubg\.lua$", c_rel):
            continue
        # (h) du lieu ban do co nhac ten tep nay
        if c_ten in ten_mapdata:
            continue
        thay = []
        for ten_nguon, ds in dong_sach:
            if chuan(ten_nguon) == c_rel:      # chinh no
                continue
            for i, l in ds:
                if c_ten in l:
                    thay.append((ten_nguon, i))
                    break
            if thay:
                break
        if not thay:
            try:
                n_dong = doc(p).count("\n") + 1
            except OSError:
                n_dong = 0
            mo_coi.append((rel, n_dong))

    # ---- 4. gom theo thu muc ----
    nhom = {}
    for rel, n in mo_coi:
        tm = os.path.dirname(rel) or "(goc)"
        nhom.setdefault(tm, []).append((rel, n))

    print()
    print("=> %d tep MO COI (khong ai nhac toi ten tep, ke ca C++ va settings)"
          % len(mo_coi))
    print("   tong %d dong" % sum(n for _, n in mo_coi))
    print()
    f = io.open(RA, "w", encoding="utf-8", newline="")
    f.write("TEP .lua MO COI TRONG CAY script\\ (khong duong nap nao)\n")
    f.write("Sinh boi ReverseTools/cauhinh/t57_quet_mocoi_toancay.py\n")
    f.write("Da kiem du 7 duong nap a..g. Bo qua ban sao luu va thu muc _dara/"
            "_backup/_gobo.\n")
    f.write("=" * 78 + "\n\n")
    for tm, ds in sorted(nhom.items(), key=lambda x: -sum(n for _, n in x[1])):
        tong = sum(n for _, n in ds)
        dong = "%-46s %3d tep %6d dong" % (tm, len(ds), tong)
        print("  " + dong)
        f.write("## %s  (%d tep, %d dong)\n" % (tm, len(ds), tong))
        for rel, n in sorted(ds, key=lambda x: -x[1]):
            f.write("    %-62s %5d dong\n" % (os.path.basename(rel), n))
        f.write("\n")
    f.close()
    print()
    print("=> chi tiet: %s" % RA)
    return 0


if __name__ == "__main__":
    sys.exit(main())
