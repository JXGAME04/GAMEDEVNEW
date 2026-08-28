# -*- coding: utf-8 -*-
"""y6_menu_bo_dopho_dung.py - Thay menu "lay khoang du cap" bang menu PHAT DUNG BO.

CHU GAME: "toi can ban dung doan mo, phai biet nguyen lieu nhu nao pham chat bao
nhieu moi add vao lenh bai admin de lay cho dung".

DA DOC RA LUAT CHINH XAC tu `atlas.lua : verifyAtlasMaterials` (dong 129-168):
voi moi nguyen lieu i cua cong thuc, phai co MOT mon trong o thoa DU:
    nGenre       khop
    nDetailType  khop (neu bang >= 0)
    nParticular  khop (neu >= 0)
    nLevel       mon co cap  >=  cap ghi trong bang     <-- LON HON HOAC BANG
    nSeries      khop (neu >= 0)
    nMagicID     khop (neu >= 0)                        <-- DIEU KIEN TOI TUNG BO SOT
va cuoi cung `nReqItemCount ~= nMaterialCount` => SO MON PHAI KHOP CHINH XAC
(thua mot vien cung hong). nMaterialCount dem cac mon KHONG phai do pho va
KHONG phai Huyen Tinh (146).

CONG THUC THAT (do pho "Phuc Ma Tu Kim Con", ATLAS ptc=243):
  hang 6   -> RA NGUYEN MON:
     Huyen Thiet khoang 199 cap>=8 MagicID 126
     Mat Ngan khoang    201 cap>=7 MagicID 166
     Chu Sa khoang      203 cap>=6 MagicID 137
     Huyen Thiet khoang 199 cap>=7 MagicID 115
     Phu Dung Thach     202 cap>=5 he 0 MagicID 121   (bang ghi cap 7)
     Vo Lam Mat Tich     26
  hang 134 -> RA MANH 1/9: y het nhung cap thap hon (7,6,5,5,5)
Ngoai ra can: cuon Do pho (243) + Huyen Tinh Khoang Thach (146) + tien.

VI SAO MENU CU HONG: `LR_LayKhoangCap` (them o y4) phat khoang voi MagicID = 0,
ma cong thuc doi MagicID 126/166/137/115/121 => khong bao gio khop.
`LR_LayBoDoPho` thi DUNG (no doc cot `k.."_MAGIC_ID"` roi truyen vao AddItemEx),
nhung no chi chon dong DAU TIEN co DES_PIECE 1..9, tuc luon ra MANH.

MIENG VA: thay than `LR_LayKhoangCap` bang ban phat DUNG BO cho cong thuc
RA NGUYEN MON (DES_PIECE = 0) cua do pho dau tien, va IN RA man hinh tung mon
kem cap + MagicID de doi chieu. Doi nhan menu cho dung.

CHI LA KICH BAN LUA - khong phai build.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_bomon lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\item\test_loren_admin.lua")
HAU_TO = ".truoc_bomon"
T = "\t"
CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! chu HOA co dau: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! cham vong tron that bai: %r" % u)
    return t


NHAN_MENU = tcvn(u"L\u1ea5y b\u1ed9 \u0110\u1ed3 ph\u1ed5 (ra nguy\u00ean m\u00f3n)")
M_TIEU = tcvn(u"C\u00f4ng th\u1ee9c d\u00f2ng <color=yellow>%d<color> - ra nguy\u00ean m\u00f3n:")
M_DOPHO = tcvn(u"  \u0110\u1ed3 ph\u1ed5: %s")
M_HT = tcvn(u"  Huy\u1ec1n Tinh Kho\u00e1ng Th\u1ea1ch c\u1ea5p 5")
M_NL = tcvn(u"  %s c\u1ea5p %d (ph\u00e9p %d)")
M_CUOI = tcvn(u"\u0110\u00e3 ph\u00e1t <color=yellow>%d<color> m\u00f3n. B\u1ecf h\u1ebft v\u00e0o l\u00f2 r\u00e8n l\u00e0 gh\u00e9p \u0111\u01b0\u1ee3c.")
M_LOI = tcvn(u"<color=red>Kh\u00f4ng t\u00ecm th\u1ea5y c\u00f4ng th\u1ee9c ra nguy\u00ean m\u00f3n.<color>")
NUT_MO = tcvn(u"M\u1edf th\u1ee3 r\u00e8n")
NUT_LAI = tcvn(u"L\u1ea5y l\u1ea1i")
NUT_VE = tcvn(u"Quay l\u1ea1i")

THAN = [
    "function LR_LayKhoangCap()",
    T + "-- [LOREN 28/08] Phat DUNG BO cho cong thuc RA NGUYEN MON (DES_PIECE = 0).",
    T + "-- Luat kiem o atlas.lua:verifyAtlasMaterials doi DU CA BA: dung ptc,",
    T + "-- cap >= cap trong bang, va DUNG MagicID. Menu cu phat MagicID = 0 nen",
    T + "-- khong bao gio khop - do la ly do bao thieu nguyen lieu.",
    T + "local nVer = ITEM_GetLatestItemVersion();",
    T + "local szPath = LR_DuongBang( nVer, FILE_ATLAS_TEST );",
    T + "TabFile_Load( szPath, szPath );",
    T + "local nRowCount = TabFile_GetRowCount( szPath );",
    T + "local nDong = 0;",
    T + "local j;",
    T + "for j = 2, nRowCount do",
    T*2 + 'local nPiece = tonumber( TabFile_GetCell( szPath, j, "DES_PIECE", -1 ) );',
    T*2 + 'local ag = tonumber( TabFile_GetCell( szPath, j, "ATLAS_GENRE", -1 ) );',
    T*2 + "if( nPiece == 0 and ag >= 0 ) then",
    T*3 + "nDong = j;",
    T*3 + "break;",
    T*2 + "end",
    T + "end",
    T + "if( nDong == 0 ) then",
    T*2 + 'SayEx({ "%s", "%s/LR_Root" })' % (M_LOI, NUT_VE),
    T*2 + "return",
    T + "end",
    "",
    T + 'local szMsg = format( "%s\\n", nDong );' % M_TIEU,
    T + "local nDem = 0;",
    T + "-- cuon Do pho",
    T + 'local ag = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_GENRE", -1 ) );',
    T + 'local ad = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_DETAILTYPE", -1 ) );',
    T + 'local ap = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_PARTICULAR", -1 ) );',
    T + "if( AddItemEx( nVer, 0, 0, ag, ad, ap, 1, 0, 0, 0 ) > 0 ) then",
    T*2 + "nDem = nDem + 1;",
    T*2 + 'szMsg = szMsg..format( "%s\\n", TabFile_GetCell( szPath, nDong, "ATLAS_NAME", "" ) );' % M_DOPHO,
    T + "end",
    T + "-- mot vien Huyen Tinh",
    T + "if( AddItemEx( nVer, 0, 0, 6, 1, 146, 5, 0, 0, 0 ) > 0 ) then",
    T*2 + "nDem = nDem + 1;",
    T*2 + 'szMsg = szMsg.."%s\\n";' % M_HT,
    T + "end",
    T + "-- sau o nguyen lieu: DUNG cap va DUNG MagicID",
    T + "local k;",
    T + "for k = 1, 6 do",
    T*2 + 'local g = tonumber( TabFile_GetCell( szPath, nDong, k.."_GENRE", -1 ) );',
    T*2 + "if( g >= 0 ) then",
    T*3 + 'local d = tonumber( TabFile_GetCell( szPath, nDong, k.."_DETAILTYPE", -1 ) );',
    T*3 + 'local p = tonumber( TabFile_GetCell( szPath, nDong, k.."_PARTICULAR", -1 ) );',
    T*3 + 'local lv = tonumber( TabFile_GetCell( szPath, nDong, k.."_LEVEL", -1 ) );',
    T*3 + 'local se = tonumber( TabFile_GetCell( szPath, nDong, k.."_SERIES", -1 ) );',
    T*3 + 'local mg = tonumber( TabFile_GetCell( szPath, nDong, k.."_MAGIC_ID", -1 ) );',
    T*3 + "if( d < 0 ) then d = 1; end",
    T*3 + "if( p < 0 ) then p = 0; end",
    T*3 + "if( lv < 0 ) then lv = 1; end",
    T*3 + "if( se < 0 ) then se = 0; end",
    T*3 + "if( mg < 0 ) then mg = 0; end",
    T*3 + "if( AddItemEx( nVer, 0, 0, g, d, p, lv, se, 0, mg ) > 0 ) then",
    T*4 + "nDem = nDem + 1;",
    T*4 + 'szMsg = szMsg..format( "%s\\n", TabFile_GetCell( szPath, nDong, k.."_NAME", "" ), lv, mg );' % M_NL,
    T*3 + "end",
    T*2 + "end",
    T + "end",
    T + 'szMsg = szMsg..format( "\\n%s", nDem );' % M_CUOI,
    T + 'SayEx({ szMsg, "%s/LR_MoThoRen", "%s/LR_LayKhoangCap", "%s/LR_Root" })'
        % (NUT_MO, NUT_LAI, NUT_VE),
    "end",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y6_menu_bo_dopho_dung - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"

    if "ra nguyen mon" in raw or "RA NGUYEN MON" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0
    if "function LR_LayKhoangCap()" not in raw:
        print("!!! LOI TO: chua co LR_LayKhoangCap - chay y4 truoc")
        return 1

    dong = raw.split(eol)
    # thay tron than ham cu
    i0 = None
    for i, l in enumerate(dong):
        if l.strip() == "function LR_LayKhoangCap()":
            i0 = i
            break
    i1 = None
    for j in range(i0 + 1, min(i0 + 40, len(dong))):
        if dong[j].strip() == "end":
            i1 = j
            break
    if i0 is None or i1 is None:
        print("!!! LOI TO: khong khoanh duoc than ham cu")
        return 1
    print("  ok  thay than LR_LayKhoangCap (dong %d..%d, %d dong -> %d dong)"
          % (i0 + 1, i1 + 1, i1 - i0 + 1, len(THAN)))
    dong = dong[:i0] + THAN + dong[i1 + 1:]

    nd = eol.join(dong)
    # doi nhan menu
    cu_nhan = tcvn(u"L\u1ea5y kho\u00e1ng thu\u1ed9c t\u00ednh \u0111\u1ee7 c\u1ea5p (5-8)")
    if cu_nhan in nd:
        nd = nd.replace(cu_nhan, NHAN_MENU, 1)
        print("  ok  doi nhan menu")
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1
    n_fn = nd.count("function ")
    n_end = len([l for l in nd.split(eol) if l.strip() == "end"])
    print("  function=%d | dong 'end'=%d" % (n_fn, n_end))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Nap lai script la dung duoc (KHONG phai build).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
