# -*- coding: utf-8 -*-
"""z1_menu_va_xemtruoc.py - (1) menu cu phat DUNG bo; (2) them ham Xem truoc.

DA MO PHONG BANG SO (khong doan) tren bang atlas_compound.txt that:

  `isAtlas` (atlas.lua:110-126) quet tu dong 2 va LAY DONG DAU TIEN khop ma do
  pho (break ngay khi gap). Voi do pho 243, dong dau tien la HANG 6 - cong thuc
  RA NGUYEN MON, doi cap >= 8/7/6/7/7 + MagicID 126/166/137/115/121.

  Menu cu `LR_LayBoDoPho` chon dong dau tien co DES_PIECE 1..9 = HANG 134 (bo
  MANH, cap 7/6/5/5/5). Doi chieu bo nay voi yeu cau hang 6:
      yc1 Huyen Thiet cap>=8  -> co c7  TRUOT
      yc2 Mat Ngan   cap>=7  -> co c6  TRUOT
      yc3 Chu Sa     cap>=6  -> co c5  TRUOT
      yc4 Huyen Thiet cap>=7 -> co c5 (mg115)  TRUOT
      yc5 Phu Dung   cap>=7  -> co c5  TRUOT
  => verifyAtlasMaterials tra 0 => RESULT_LACK_RESOURCE = "Thieu nguyen lieu".
  DUNG Y trieu chung chu game gap, va khong co dong loren_kiem.log nao (C++ da
  cho qua, rot o Lua).

VA (1): doi bo loc chon dong cua LR_LayBoDoPho tu `DES_PIECE 1..9` sang
`DES_PIECE == 0` - phat bo cua chinh dong ma isAtlas se kiem. Kem chan: chi
phat Manh thien thach khi nPiece >= 1 (piece 0 thi 1316+0 = detail 1316 khong
ton tai).

VA (2): them `LR_UI_AtlasPreview` vao test_loren_admin.lua - doi chieu 8 o
CHINH cua phong 10 voi cong thuc that (cung 6 dieu kien nhu verifyAtlasMaterials
+ dem so mon), in tung dong Khop/Thieu kem cap va ma phep. Nut Xem truoc tren
trang Do pho (z4) se goi ham nay. Dat trong test_loren_admin.lua vi no can
LR_DuongBang + FILE_ATLAS_TEST + TabFile_* (cung mot the hien script voi
compound_ui.lua qua Include).

Sau khi ghi PHAI qua bo kiem can bang khoi Lua (kiem_lua.py) - luat moi sau
su co y6.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_xemtruoc lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\item\test_loren_admin.lua")
HAU_TO = ".truoc_xemtruoc"
T = "\t"
CHO_PHEP_HOA = set(u"\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! chu HOA co dau: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! cham vong tron: %r" % u)
    return t


S_TIEUDE = tcvn(u"Xem tr\u01b0\u1edbc \u0111\u1ed3 ph\u1ed5:")
S_CHUACO = tcvn(u"Ch\u01b0a \u0111\u1eb7t \u0110\u1ed3 ph\u1ed5 v\u00e0o \u00f4.")
S_THIEUHT = tcvn(u"Thi\u1ebfu Huy\u1ec1n Tinh Kho\u00e1ng Th\u1ea1ch (\u00f4 nh\u1ecf b\u00ean ph\u1ea3i).")
S_KHOP = tcvn(u"Kh\u1edbp")
S_TRUOT = tcvn(u"Thi\u1ebfu")
S_CAP = tcvn(u"c\u1ea5p")
S_PHEP = tcvn(u"ph\u00e9p")
S_SOMON = tcvn(u"S\u1ed1 m\u00f3n nguy\u00ean li\u1ec7u: c\u1ea7n %d, \u0111ang c\u00f3 %d.")
S_DU = tcvn(u"\u0110\u1ee7 nguy\u00ean li\u1ec7u - b\u1ea5m K\u1ebft h\u1ee3p \u0111\u01b0\u1ee3c.")
S_CHUA = tcvn(u"Ch\u01b0a \u0111\u1ee7 - xem c\u00e1c d\u00f2ng \u0111\u1ecf.")

# ---------------- mieng 1: bo loc chon dong ----------------
CU_LOC = [
    T*2 + 'local nPiece = tonumber( TabFile_GetCell( szPath, j, "DES_PIECE", -1 ) );',
    T*2 + 'local nGenre = tonumber( TabFile_GetCell( szPath, j, "ATLAS_GENRE", -1 ) );',
    T*2 + "if( nPiece >= 1 and nPiece <= 9 and nGenre >= 0 ) then",
]
MOI_LOC = [
    T*2 + 'local nPiece = tonumber( TabFile_GetCell( szPath, j, "DES_PIECE", -1 ) );',
    T*2 + 'local nGenre = tonumber( TabFile_GetCell( szPath, j, "ATLAS_GENRE", -1 ) );',
    T*2 + "-- [LOREN 28/08] DOI 1..9 -> == 0: isAtlas (atlas.lua:110) LAY DONG DAU",
    T*2 + "-- TIEN khop ma do pho, tuc voi moi do pho chi cong thuc o dong dau",
    T*2 + "-- (ra nguyen mon, DES_PIECE=0) la duoc kiem that. Bo MANH (1..9) o cac",
    T*2 + "-- dong sau khong bao gio toi luot -> phat bo do la luon 'thieu nguyen",
    T*2 + "-- lieu' (cap thap hon yeu cau dong dau).",
    T*2 + "if( nPiece == 0 and nGenre >= 0 ) then",
]

# ---------------- mieng 2: chan manh khi piece == 0 ----------------
CU_MANH = [
    T + 'local nPiece = tonumber( TabFile_GetCell( szPath, nDong, "DES_PIECE", -1 ) );',
    T + "if( AddItemEx( nVer, 0, 0, 4, 1316 + nPiece, 0, 1, 0, 0, 0 ) > 0 ) then",
]
MOI_MANH = [
    T + 'local nPiece = tonumber( TabFile_GetCell( szPath, nDong, "DES_PIECE", -1 ) );',
    T + "-- [LOREN 28/08] piece 0 thi 1316+0 = detail 1316 khong ton tai - chi phat",
    T + "-- manh khi cong thuc that su ra manh.",
    T + "if( nPiece >= 1 and AddItemEx( nVer, 0, 0, 4, 1316 + nPiece, 0, 1, 0, 0, 0 ) > 0 ) then",
]

# ---------------- mieng 3: ham Xem truoc ----------------
HAM = [
    "",
    "--------------------------------------------------------------------------------",
    "-- [LOREN 28/08] XEM TRUOC DO PHO - nut PreviewBtn cua trang Do pho goi ham nay.",
    "-- Doi chieu 8 o CHINH cua phong 10 voi cong thuc that: cung 6 dieu kien nhu",
    "-- atlas.lua:verifyAtlasMaterials (genre/detail/ptc khop, cap >=, ngu hanh khop,",
    "-- MagicID khop) + dem so mon phai dung. In tung dong Khop/Thieu de nguoi choi",
    "-- biet chinh xac con thieu gi truoc khi bam Ket hop.",
    "--------------------------------------------------------------------------------",
    "function LR_UI_AtlasPreview()",
    T + "local nVer = ITEM_GetLatestItemVersion();",
    T + "local szPath = LR_DuongBang( nVer, FILE_ATLAS_TEST );",
    T + "TabFile_Load( szPath, szPath );",
    T + "local nRowCount = TabFile_GetRowCount( szPath );",
    T + "local nAtlasRow = 0;",
    T + "local bXJ = 0;",
    T + "local aryMat = {};",
    T + "local nMat = 0;",
    T + "local i;",
    T + "for i = 0, 7 do",
    T*2 + "local nIdx = GetIdItem( 10, i );",
    T*2 + "if( nIdx ~= nil and nIdx > 0 ) then",
    T*3 + "local g, d, p, lv, se, lu = GetItemProp( nIdx );",
    T*3 + "if( g == 6 and d == 1 and p == 146 ) then",
    T*4 + "bXJ = 1;",
    T*3 + "else",
    T*4 + "local nDong = 0;",
    T*4 + "local r;",
    T*4 + "for r = 2, nRowCount do",
    T*5 + 'if( tonumber( TabFile_GetCell( szPath, r, "ATLAS_GENRE", -1 ) ) == g and',
    T*6 + 'tonumber( TabFile_GetCell( szPath, r, "ATLAS_DETAILTYPE", -1 ) ) == d and',
    T*6 + 'tonumber( TabFile_GetCell( szPath, r, "ATLAS_PARTICULAR", -1 ) ) == p ) then',
    T*5 + "nDong = r;",
    T*5 + "break;",
    T*5 + "end",
    T*4 + "end",
    T*4 + "if( nDong > 0 and nAtlasRow == 0 ) then",
    T*5 + "nAtlasRow = nDong;",
    T*4 + "else",
    T*5 + "nMat = nMat + 1;",
    T*5 + "aryMat[nMat] = { g = g, d = d, p = p, lv = lv, se = se, mg = GetItemParam( nIdx, 1 ) };",
    T*4 + "end",
    T*3 + "end",
    T*2 + "end",
    T + "end",
    "",
    T + 'Msg2Player( "<color=yellow>%s" );' % S_TIEUDE,
    T + "if( nAtlasRow == 0 ) then",
    T*2 + 'Msg2Player( "<color=red>%s" );' % S_CHUACO,
    T*2 + "return;",
    T + "end",
    T + "local bOk = 1;",
    T + "if( bXJ ~= 1 ) then",
    T*2 + 'Msg2Player( "<color=red>%s" );' % S_THIEUHT,
    T*2 + "bOk = 0;",
    T + "end",
    T + "local nReq = 0;",
    T + "local k;",
    T + "for k = 1, 6 do",
    T*2 + 'local qg = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_GENRE", -1 ) );',
    T*2 + "if( qg >= 0 ) then",
    T*3 + "nReq = nReq + 1;",
    T*3 + 'local qd = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_DETAILTYPE", -1 ) );',
    T*3 + 'local qp = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_PARTICULAR", -1 ) );',
    T*3 + 'local ql = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_LEVEL", -1 ) );',
    T*3 + 'local qs = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_SERIES", -1 ) );',
    T*3 + 'local qm = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_MAGIC_ID", -1 ) );',
    T*3 + 'local szTen = TabFile_GetCell( szPath, nAtlasRow, k.."_NAME", "" );',
    T*3 + "local bCo = 0;",
    T*3 + "local j;",
    T*3 + "for j = 1, nMat do",
    T*4 + "if( aryMat[j].g == qg and",
    T*5 + "( qd < 0 or aryMat[j].d == qd ) and",
    T*5 + "( qp < 0 or aryMat[j].p == qp ) and",
    T*5 + "( ql < 0 or aryMat[j].lv >= ql ) and",
    T*5 + "( qs < 0 or aryMat[j].se == qs ) and",
    T*5 + "( qm < 0 or aryMat[j].mg == qm ) ) then",
    T*4 + "bCo = 1;",
    T*4 + "break;",
    T*4 + "end",
    T*3 + "end",
    T*3 + "local szDong;",
    T*3 + "if( bCo == 1 ) then",
    T*4 + 'szDong = format( "<color=green>[%s] ", 1 ).."%s"..szTen;' % ("%s", ""),
    T*3 + "else",
    T*4 + 'szDong = format( "<color=red>[%s] ", 1 ).."%s"..szTen;' % ("%s", ""),
    T*4 + "bOk = 0;",
    T*3 + "end",
    T*3 + "if( ql >= 0 ) then",
    T*4 + 'szDong = szDong..format( " - %s ", 1 )..ql;' % "%s",
    T*3 + "end",
    T*3 + "if( qm >= 0 ) then",
    T*4 + 'szDong = szDong..format( " - %s ", 1 )..qm;' % "%s",
    T*3 + "end",
    T*3 + "Msg2Player( szDong );",
    T*2 + "end",
    T + "end",
    T + "if( nReq ~= nMat ) then",
    T*2 + 'Msg2Player( format( "<color=red>%s", nReq, nMat ) );' % S_SOMON,
    T*2 + "bOk = 0;",
    T + "end",
    T + "if( bOk == 1 ) then",
    T*2 + 'Msg2Player( "<color=green>%s" );' % S_DU,
    T + "else",
    T*2 + 'Msg2Player( "<color=red>%s" );' % S_CHUA,
    T + "end",
    "end",
    "",
]
# don gian hoa cac dong ghep chuoi o tren (tranh format lồng %s kho doc):
for i, l in enumerate(HAM):
    if 'format( "<color=green>[%s] ", 1 )' in l:
        HAM[i] = T*4 + 'szDong = "<color=green>[' + S_KHOP + '] "..szTen;'
    elif 'format( "<color=red>[%s] ", 1 )' in l:
        HAM[i] = T*4 + 'szDong = "<color=red>[' + S_TRUOT + '] "..szTen;'
    elif 'szDong = szDong..format( " - ' in l and "ql" in l:
        HAM[i] = T*4 + 'szDong = szDong.." - ' + S_CAP + ' "..ql;'
    elif 'szDong = szDong..format( " - ' in l and "qm" in l:
        HAM[i] = T*4 + 'szDong = szDong.." - ' + S_PHEP + ' "..qm;'


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z1_menu_va_xemtruoc - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"

    if "LR_UI_AtlasPreview" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)

    def ap(dong, cu, moi, ten):
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-28s khop %d lan (can 1)" % (ten, len(vt)))
            return None
        print("  ok  %-28s dong %d (+%d)" % (ten, vt[0] + 1, len(moi) - len(cu)))
        return dong[:vt[0]] + moi + dong[vt[0] + len(cu):]

    dong = ap(dong, CU_LOC, MOI_LOC, "bo loc chon dong (piece==0)")
    if dong is None:
        return 1
    dong = ap(dong, CU_MANH, MOI_MANH, "chan manh khi piece==0")
    if dong is None:
        return 1

    # them ham vao CUOI tep
    while dong and dong[-1].strip() == "":
        dong.pop()
    dong = dong + HAM
    print("  ok  them LR_UI_AtlasPreview (%d dong) vao cuoi tep" % len(HAM))

    nd = eol.join(dong) + eol
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ngoai latin-1: %s" % e)
        return 1

    # chot can bang: do lech function/end cot 0 truoc-sau + 1 ham moi
    def dem(s):
        d = s.split(eol)
        return (len([l for l in d if l.startswith("function ")]),
                len([l for l in d if l == "end"]))
    f0, e0 = dem(raw)
    f1, e1 = dem(nd)
    print("  chot: truoc fn/end = %d/%d (lech %d) | sau = %d/%d (lech %d)"
          % (f0, e0, f0 - e0, f1, e1, f1 - e1))
    if (f1 - e1) != (f0 - e0):
        print("!!! LOI TO: do lech doi - KHONG ghi")
        return 1

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
    print("  DA GHI. Chay tiep kiem_lua.py de nghiem thu can bang.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
