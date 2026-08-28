# -*- coding: utf-8 -*-
"""x2_dopho_hien_nguyenlieu.py - CHU GIAI DO PHO HOANG KIM liet ke NGUYEN LIEU.

CHU GAME: "do pho hoang kim dung no se hien thi du nguyen lieu tren item do chu
- toi nho ban linux la nhu vay".

Dung. JX1 hien CHUA co duong hien thi do: `KItem.cpp` chi co nhanh cho Huyen Tinh
(ptc 146) va khoang thuoc tinh (ptc 199..204), khong co nhanh nao cho do pho.
`Intro` cua do pho trong magicscript.txt chi ghi mot dong "Do pho Hoang Kim",
khong liet ke nguyen lieu => phai sinh DONG tu bang cong thuc.

MIENG VA: them nhanh cho do pho Hoang Kim (genre 6 / detail 1 / ptc 238..390 -
dai that sau khi nan bang, 132 ma). Doc `settings\\item\\atlas_compound.txt`,
tim dong co `ATLAS_PARTICULAR` == ptc cua vien do pho, roi in 6 o nguyen lieu
(cot i_NAME va i_LEVEL) duoi dang:

    Nguyen lieu can:
      Huyen Thiet khoang (cap 8)
      Khong Tuoc Thach (cap 8)
      ...

Ky thuat: dung KTabFile nhu nhanh khoang thuoc tinh san co (KItem.cpp:1514),
duyet tu dong 2 den GetHeight() tim ATLAS_PARTICULAR khop. Bang 1088 dong nen
vong duyet khong dang ke, va chi chay khi ve chu giai MOT vien do pho.

An toan:
  - Chi THEM mot khoi `if`, khong sua duong cu.
  - Moi chuoi ghi vao `pszMsg` deu qua bo dem co han va dem do dai truoc khi
    noi, giu dung thoi quen cua ham nay (pszMsg la bo dem chung, tran la sap).
  - Dat NGAY SAU khoi khoang thuoc tinh de thu tu chu giai giong ban goc.

Tep dich: Sources\\Core\\Src\\KItem.cpp => build Core CA HAI cau hinh.
Kem theo: chep `atlas_compound.txt` (ban DA NAN) tu server sang client, vi ban
o client la ban cu 2022 chua nan.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_dophotip lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] Chu giai Do pho: liet ke nguyen lieu"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItem.cpp"
HAU_TO = ".truoc_dophotip"
SRC_TAB = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
           r"\settings\item\atlas_compound.txt")
DST_TAB = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
           r"\settings\item\atlas_compound.txt")

# Neo: dat khoi moi NGAY SAU khoi "Pham chat thuoc tinh" cua khoang (KItem.cpp:1540-1546)
CU = [
    T + "if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType >= 199 && m_CommonAttrib.nParticularType <= 204))",
    T + "{",
    T*2 + "char szLevel[128];",
]

KHOI = [
    T + "// " + NHAN + ".",
    T + "// Ban goc hien du nguyen lieu ngay tren vien do pho. JX1 truoc day khong",
    T + "// co nhanh nao cho do pho (chi Huyen Tinh 146 va khoang 199..204), con",
    T + "// Intro trong magicscript.txt chi ghi mot dong \"Do pho Hoang Kim\" - nen",
    T + "// phai sinh DONG tu bang cong thuc.",
    T + "// Dai 238..390 la dai THAT sau khi nan atlas_compound.txt sang ma JX1.",
    T + "if (m_CommonAttrib.nItemGenre == item_magicscript && m_CommonAttrib.nDetailType == 1 &&",
    T*2 + "m_CommonAttrib.nParticularType >= 238 && m_CommonAttrib.nParticularType <= 390)",
    T + "{",
    T*2 + "KTabFile AtlasTab;",
    T*2 + "if (AtlasTab.Load(\"\\\\Settings\\\\Item\\\\atlas_compound.txt\"))",
    T*2 + "{",
    T*3 + "int nHang = 0;",
    T*3 + "for (int r = 2; r <= AtlasTab.GetHeight(); r++)",
    T*3 + "{",
    T*4 + "int nPtc = -1;",
    T*4 + "AtlasTab.GetInteger(r, (LPSTR)\"ATLAS_PARTICULAR\", -1, &nPtc);",
    T*4 + "if (nPtc == m_CommonAttrib.nParticularType)",
    T*4 + "{",
    T*5 + "nHang = r;",
    T*5 + "break;",
    T*4 + "}",
    T*3 + "}",
    T*3 + "if (nHang >= 2)",
    T*3 + "{",
    T*4 + "strcat(pszMsg, \" <color=Fire>Nguy\\352n li\\214u c\\254n thi\\322t:<color>  \\n  \");",
    T*4 + "for (int k = 1; k <= 6; k++)",
    T*4 + "{",
    T*5 + "char szCot[32];",
    T*5 + "char szTen[64];",
    T*5 + "int  nCap = -1;",
    T*5 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_NAME\", k);",
    T*5 + "szCot[sizeof(szCot) - 1] = 0;",
    T*5 + "szTen[0] = 0;",
    T*5 + "AtlasTab.GetString(nHang, (LPSTR)szCot, (LPSTR)\"\", szTen, sizeof(szTen));",
    T*5 + "if (szTen[0] == 0)",
    T*6 + "continue;",
    T*5 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_LEVEL\", k);",
    T*5 + "szCot[sizeof(szCot) - 1] = 0;",
    T*5 + "AtlasTab.GetInteger(nHang, (LPSTR)szCot, -1, &nCap);",
    T*5 + "char szMot[128];",
    T*5 + "if (nCap > 0)",
    T*6 + "_snprintf(szMot, sizeof(szMot) - 1, \"   <color=Green>%s<color> (c\\244p %d)  \\n  \", szTen, nCap);",
    T*5 + "else",
    T*6 + "_snprintf(szMot, sizeof(szMot) - 1, \"   <color=Green>%s<color>  \\n  \", szTen);",
    T*5 + "szMot[sizeof(szMot) - 1] = 0;",
    T*5 + "strcat(pszMsg, szMot);",
    T*4 + "}",
    T*3 + "}",
    T*2 + "}",
    T + "}",
    "",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x2_dopho_hien_nguyenlieu - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: moc neo khop %d lan (can 1)" % len(vt))
        for i, l in enumerate(dong):
            if "Pham chat thuoc tinh" in l or "nParticularType <= 204))" in l:
                print("    dong %d: %r" % (i + 1, l[:80]))
        return 1
    i = vt[0]
    print("  ok  chen %d dong truoc dong %d" % (len(KHOI), i + 1))
    dong = dong[:i] + KHOI + dong[i:]

    nd = eol.join(dong)
    hi1 = sum(1 for c in nd if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d (khoi moi phai la ASCII thuan)" % (hi0, hi1))
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    print("  byte cao %d (khong doi) | ngoac can bang" % hi0)

    print("--- bang cho client ---")
    if os.path.isfile(SRC_TAB):
        a = io.open(SRC_TAB, "rb").read()
        b = io.open(DST_TAB, "rb").read() if os.path.isfile(DST_TAB) else b""
        print("   server %d byte | client %d byte -> %s"
              % (len(a), len(b), "DA KHOP" if a == b else "SE CHEP (client dang la ban CU chua nan)"))
    else:
        print("   !!! khong thay %s" % SRC_TAB)
        return 1

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KItem.cpp")
    if io.open(SRC_TAB, "rb").read() != (io.open(DST_TAB, "rb").read() if os.path.isfile(DST_TAB) else b""):
        if os.path.isfile(DST_TAB) and not os.path.isfile(DST_TAB + HAU_TO):
            shutil.copy2(DST_TAB, DST_TAB + HAU_TO)
        shutil.copy2(SRC_TAB, DST_TAB)
        print("  DA CHEP atlas_compound.txt (ban da nan) sang client")
    print("\n  => build Core CA HAI cau hinh, dat CoreClient.dll + CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
