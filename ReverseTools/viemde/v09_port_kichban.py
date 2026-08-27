# -*- coding: utf-8 -*-
"""VIEM DE - buoc 6a: CHEP 26 tep kich ban tu ban Linux sang JX1 + doi ma vat pham.

Lam duoc bang may (buoc nay):
  * giai ma tep Linux (dec2) roi ma hoa lai thanh byte TCVN3 cua JX1
  * doi ma vat pham theo BANG TRA da chot o buoc 4 (chi doi o nhung CHO NHAN RA
    DUOC la vat pham, va GHI LAI TUNG CHO DOI de kiem lai)
  * doi task 200 (JX1 da dung) sang ma trong
  * bao cao nhung cho CON PHAI SUA TAY

KHONG lam bang may (buoc 6b, sua tay):
  * lop dieu phoi dung chung state (tbReady...) - xem BANGIAO_VIEMDE_2608.md muc 7.1
  * cac bay #3/#4/#5/#6 cua huong dan

Chay lai duoc nhieu lan (ghi de tep dich).
"""
import io
import os
import re
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

NGUON = r"D:\ServerLinux\server1\script\missions\yandibaozang"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\yandibaozang"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\script\missions\yandibaozang"

# ---- BANG TRA VAT PHAM (Linux -> JX1), chot o buoc 4 ----
# khoa "g,d,p"  ->  (g,d,p) moi
DOI_ITEM = {
    "6,0,3":     ("6", "1", "3"),        # Dai Luc hoan
    "6,0,6":     ("6", "1", "6"),        # Phi Toc hoan
    "6,1,147":   ("6", "1", "146"),      # Huyen Tinh Khoang Thach
    "6,1,215":   ("6", "1", "214"),      # Can Khon Tao Hoa Dan (dai)
    "6,1,398":   ("6", "1", "397"),      # Than bi khoang thach
    "6,1,1309":  ("6", "1", "1314"),     # Manh Bo Thien Thach (trung)
    "6,1,1604":  ("6", "1", "1613"),     # Anh Hung Thiep
    "6,1,1605":  ("6", "1", "1614"),     # Hinh nhan
    "6,1,1606":  ("6", "1", "1615"),     # Viem De Do Dang
    "6,1,1617":  ("6", "1", "1626"),     # Viem De Lenh
    "6,1,1746":  ("6", "1", "1755"),     # Phuong Minh Chuy
    "6,1,2351":  ("6", "1", "2360"),     # Huyen Vien Lenh
    "6,1,2532":  ("6", "1", "2541"),     # Viem De Lenh Ky
    "6,1,2744":  ("6", "1", "3362"),     # Chia Khoa Nhu y
    "6,1,2805":  ("6", "1", "3423"),     # Viem De Bi Bao
    "6,1,3203":  ("6", "1", "3821"),     # Ho Mach Don
    "6,1,3810":  ("6", "1", "4428"),     # Tinh Thiet Khoang
    "6,1,3811":  ("6", "1", "4429"),     # Tinh Tinh Khoang
    "6,1,4134":  ("6", "1", "3926"),     # Chan Nguyen Dan (theo dot Bao Ruong)
    "6,1,30006": ("6", "1", "4862"),     # Do Pho Tinh Suong Y
    "6,1,30008": ("6", "1", "4867"),     # Do Pho Tinh Suong Yeu Dai   (moi)
    "6,1,30011": ("6", "1", "4868"),     # Do Pho Tinh Suong Ngoc Boi  (moi)
    "6,1,30191": ("6", "1", "2953"),     # Chia khoa vang
    "6,1,30228": ("6", "1", "4846"),     # Chan Nguyen Don (trung)
    "6,1,30229": ("6", "1", "4847"),     # Chan Nguyen Don (dai)
    "6,1,30289": ("6", "1", "3051"),     # Huyet Long Dang
    "6,1,30301": ("6", "1", "4857"),     # Hon nguyen chan don
    "6,1,30350": ("6", "1", "4865"),     # Hop Mat Na Chien Truong     (moi)
    "6,1,30505": ("6", "1", "4863"),     # Do Pho Tinh Suong Khi Gioi
    "6,1,30506": ("6", "1", "4860"),     # Tinh Suong Lenh
    "6,1,30507": ("6", "1", "4861"),     # Huyen Thiet
    "6,1,30529": ("6", "1", "4858"),     # Do Pho Dang Long Y
    "6,1,30534": ("6", "1", "4869"),     # Do Pho Dang Long Boi        (moi)
    "6,1,30537": ("6", "1", "4859"),     # Do Pho Dang Long Khi Gioi
    "6,1,30538": ("6", "1", "4866"),     # Dang Long Thach - Ha        (moi)
    "6,1,30557": ("6", "1", "4813"),     # Tui Duoc Pham
    # 4,417,1 Tien dong: KHOP SAN o hai ben, khong doi
}

# task 200 da bi JX1 dung (battlehead.lua / hoatdong_admin.lua / bwhead.lua)
TASK_CU, TASK_MOI = "200", "2619"

# ---- nhung thu chac chan phai sua tay: bao cao chu khong tu doi ----
CAN_SUA_TAY = [
    (re.compile(r"\bGetMSPlayerCount\s*\([^)]*,"), "bay #3: GetMSPlayerCount co tham so NHOM - JX1 dem nhom do, phai bo tham so"),
    (re.compile(r"\bGetMissionV\s*\(\s*\w+\s*\)\s*==\s*0"), "bay #4: dung IsMission() thay vi GetMissionV()==0 de biet mission ton tai"),
    (re.compile(r"\bAddNpc\s*\("), "bay #5: AddNpc tham so 6 lech nghia - dung AddNpcEx/HD3_AddNpc"),
    (re.compile(r"\bunpack\s*\("), "Lua 4.0 cua JX1 khong co unpack()"),
    (re.compile(r"\bIL\s*\(|\bIncludeLib\s*\("), "IncludeLib: kiem thu vien do JX1 co chua"),
    (re.compile(r"\bApplyItemDice\b|\bRollItem\b|\bAddDiceItemInfo\b|\bGetItemDice"), "he xuc xac: chua co o JX1 (buoc 7)"),
    (re.compile(r"\bcurpack\s*\(|\busepack\s*\("), "lop pack cua Linux - JX1 khong can (timer JX2 goi trong state cua tep)"),
    (re.compile(r"\btbReady\b"), "lop dieu phoi dung chung state - phai viet lai (muc 7.1)"),
    (re.compile(r"\bSubWorldName\s*\("), "SubWorldName: JX1 chua co (da bu tam trong dungeon.lua)"),
    (re.compile(r"\bset_MapType\b|\badd_Item2Map\b|\badd_forbit_templatemap\b"), "he cam vat pham theo loai ban do: JX1 khong co"),
]

RE_TB3 = re.compile(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*([,}])")
RE_GOI3 = re.compile(r"\b(AddItem|CalcItemCount|ConsumeItem|CalcEquiproomItemCount|DelItem|GetItemCount)\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*([,)])")


def doi_ma(s, ghi_lai, ten_tep, so_dong):
    """doi ma vat pham o 2 dang nhan ra duoc: bang {g,d,p,...} va goi ham (g,d,p,...)"""

    def f_tb(m):
        k = "%s,%s,%s" % (m.group(1), m.group(2), m.group(3))
        if k in DOI_ITEM:
            g, d, p = DOI_ITEM[k]
            ghi_lai.append((ten_tep, so_dong, k, "%s,%s,%s" % (g, d, p), "bang"))
            return "{%s, %s, %s%s" % (g, d, p, m.group(4))
        return m.group(0)

    def f_goi(m):
        k = "%s,%s,%s" % (m.group(2), m.group(3), m.group(4))
        if k in DOI_ITEM:
            g, d, p = DOI_ITEM[k]
            ghi_lai.append((ten_tep, so_dong, k, "%s,%s,%s" % (g, d, p), m.group(1)))
            return "%s(%s, %s, %s%s" % (m.group(1), g, d, p, m.group(5))
        return m.group(0)

    s = RE_TB3.sub(f_tb, s)
    s = RE_GOI3.sub(f_goi, s)
    return s


def main():
    tep = []
    for dp, dn, fn in os.walk(NGUON):
        for f in sorted(fn):
            if f.endswith(".lua"):
                tep.append(os.path.join(dp, f))
    tep.sort()
    print("tim thay %d tep .lua o ban Linux" % len(tep))
    print()

    doi = []
    canh_bao = {}
    tong_dong = 0
    for p in tep:
        rel = os.path.relpath(p, NGUON)
        raw = open(p, "rb").read().split(b"\n")
        ra = []
        for i, l in enumerate(raw, 1):
            s = dec2.decline2(l.rstrip(b"\r"))
            khong_ct = s.split("--")[0]
            s = doi_ma(s, doi, rel, i)
            for rx, ly in CAN_SUA_TAY:
                if rx.search(khong_ct):
                    canh_bao.setdefault(ly, []).append("%s:%d" % (rel, i))
            ra.append(uni2tcvn(s))
        tong_dong += len(ra)
        noi_dung = "\r\n".join(ra)
        for goc in (DICH, GUONG):
            q = os.path.join(goc, rel)
            os.makedirs(os.path.dirname(q), exist_ok=True)
            tam = q + ".dangghi"
            with open(tam, "wb") as f:
                f.write(noi_dung.encode("latin-1"))
            os.replace(tam, q)

    print("da chep %d tep, %d dong" % (len(tep), tong_dong))
    print()
    print("=== DA DOI %d CHO MA VAT PHAM ===" % len(doi))
    theo_ma = {}
    for t, d, cu, moi, kieu in doi:
        theo_ma.setdefault((cu, moi), []).append("%s:%d" % (t, d))
    for (cu, moi), cho in sorted(theo_ma.items()):
        print("   %-11s -> %-10s  %2d cho: %s" % (cu, moi, len(cho), ", ".join(cho[:3])))
    con = set(DOI_ITEM) - set(c for c, m in theo_ma)
    if con:
        print("   (khong gap trong kich ban: %s)" % ", ".join(sorted(con)))

    print()
    print("=== CON PHAI SUA TAY ===")
    for ly, cho in sorted(canh_bao.items(), key=lambda x: -len(x[1])):
        print("   [%3d cho] %s" % (len(cho), ly))
        print("             %s" % ", ".join(sorted(set(cho))[:6]))


main()
