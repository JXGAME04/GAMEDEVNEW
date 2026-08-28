# -*- coding: utf-8 -*-
"""vT_soi_o_loren.py - Them ham SOI O LO REN vao compound_ui.lua + muc menu.

VI SAO: the Kham nam bao "Nguyen lieu khong dung luat ghep" (FOUNDRY_RESULT 8).
Luat da doi chieu voi ban Linux va DUNG:
  - `Equip(enchasable)` doi Quality = 1 (NATURE_VIOLET) - ban Linux ghi 2, JX1
    nan sang 1 qua g_QualityNatureToLinux; DetailType [0,1,2,5,6,7,8] TRUNG KHIT
    ban Linux (nhan/day chuyen von KHONG kham nam duoc).
  - 3 nhom bat buoc: Equip(enchasable) + Cryolite(146) + PropMine(199..204).
  - `s_anSoO[5] = 3` (toi da 3 mon CHINH) va `s_anKhoaThua[5] = -1`
    (KHONG cho phep mon thua nao trong vector chinh).
  - O 3..10 di vao vector TU CHON, kiem bang `Enchase_EnhanceItem` = ConsumeItem;
    bo mon khac vao do cung ra RULE_ERROR (KItemCompound.cpp:537-540).

Nen thay vi doan nguoi choi bo gi, ham nay IN RA noi dung tung o cua ca 6 phong
(genre / detailtype / particular / level / series), kem nhan xet o nao la CHINH
o nao la PHU. Nhin mot cai la biet mon nao khong khop luat.

KHONG PHAI BUILD: chi la kich ban Lua, nap lai script la chay.
Dung xong go bang `--go`.

Mac dinh DIEN TAP; --ghi de them; --go de bo.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

UI_LUA = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\compound\compound_ui.lua"
TEST_LUA = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\test_loren_admin.lua"
HAU_TO = ".truoc_soi"

CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! LOI TO: chu HOA co dau: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! LOI TO: cham vong tron that bai: %r" % u)
    return t


TIEU_DE = tcvn(u"Soi \u00f4 l\u00f2 r\u00e8n (xem m\u00f3n n\u00e0o kh\u00f4ng h\u1ee3p lu\u1eadt)")
DONG_TRONG = tcvn(u"(tr\u1ed1ng)")
NHAN_CHINH = tcvn(u"ch\u00ednh")
NHAN_PHU = tcvn(u"ph\u1ee5")

HAM = """
-- [LOREN 27/08] SOI O LO REN - in noi dung tung o de biet mon nao khong hop luat.
-- Bang phong: nPos / so o / o dau tien la PHU (-1 = khong co o phu)
LR_UI_PHONG = {
\t{ 4,  8, -1, "Tinh luyen (hop thanh Huyen Tinh)" },
\t{ 5,  8, -1, "Nang cap Huyen Tinh" },
\t{ 6,  8, -1, "Nang cap khoang thuoc tinh" },
\t{ 7, 11,  3, "Trich lay" },
\t{ 8,  2, -1, "Duc trang bi tim" },
\t{ 9, 11,  3, "Kham nam" },
};

function LR_UI_Soi()
\tlocal i;
\tfor i = 1, getn( LR_UI_PHONG ) do
\t\tlocal nPos   = LR_UI_PHONG[i][1];
\t\tlocal nSoO   = LR_UI_PHONG[i][2];
\t\tlocal nPhuTu = LR_UI_PHONG[i][3];
\t\tlocal szTen  = LR_UI_PHONG[i][4];
\t\tlocal nCo = 0;
\t\tlocal j;
\t\tfor j = 0, nSoO - 1 do
\t\t\tlocal nIdx = GetIdItem( nPos, j );
\t\t\tif( nIdx ~= nil and nIdx > 0 ) then
\t\t\t\tif( nCo == 0 ) then
\t\t\t\t\tMsg2Player( "<color=yellow>--- "..szTen.." ---" );
\t\t\t\tend
\t\t\t\tnCo = nCo + 1;
\t\t\t\tlocal g, d, p, lv, se, lu = GetItemProp( nIdx );
\t\t\t\tlocal szVai = "@CHINH@";
\t\t\t\tif( nPhuTu >= 0 and j >= nPhuTu ) then
\t\t\t\t\tszVai = "@PHU@";
\t\t\t\tend
\t\t\t\tMsg2Player( "<color=green>o "..j.." ("..szVai..") <color=white>genre="..g
\t\t\t\t\t.." detail="..d.." ptc="..p.." cap="..lv.." nguhanh="..se );
\t\t\tend
\t\tend
\tend
\tMsg2Player( "<color=yellow>--- het ---" );
end
"""


def main():
    ghi = "--ghi" in sys.argv[1:]
    go = "--go" in sys.argv[1:]
    print("=== vT_soi_o_loren - %s ==="
          % ("GO BO" if go else ("GHI THAT" if ghi else "DIEN TAP")))

    ke_hoach = []

    # ---- compound_ui.lua: them ham ----
    raw = io.open(UI_LUA, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    than = (HAM.replace("@CHINH@", NHAN_CHINH).replace("@PHU@", NHAN_PHU)).replace("\n", eol)
    co = "function LR_UI_Soi" in raw
    if go:
        if not co:
            print("  compound_ui.lua: khong co ham - bo qua")
        else:
            i = raw.find(than)
            if i < 0:
                print("!!! LOI TO: khong khop nguyen van khoi ham - KHONG go"); return 1
            ke_hoach.append((UI_LUA, raw[:i] + raw[i + len(than):]))
            print("  compound_ui.lua: se go khoi ham")
    elif co:
        print("  compound_ui.lua: DA CO - bo qua")
    else:
        if not raw.endswith(eol):
            raw += eol
        ke_hoach.append((UI_LUA, raw + than))
        print("  compound_ui.lua: se them ham LR_UI_Soi")

    # ---- test_loren_admin.lua: them muc menu ----
    raw2 = io.open(TEST_LUA, "rb").read().decode("latin-1")
    eol2 = "\r\n" if "\r\n" in raw2 else "\n"
    dong_menu = '\t\t"%s/LR_UI_Soi",' % TIEU_DE
    neo = '\t\t"%s/LR_UI_Mo",' % tcvn(u"M\u1edf c\u1eeda s\u1ed5 l\u00f2 r\u00e8n th\u1eadt")
    co2 = "LR_UI_Soi" in raw2
    if go:
        if co2:
            ke_hoach.append((TEST_LUA, raw2.replace(dong_menu.replace("\n", eol2) + eol2, "", 1)))
            print("  test_loren_admin.lua: se go muc menu")
        else:
            print("  test_loren_admin.lua: khong co muc menu - bo qua")
    elif co2:
        print("  test_loren_admin.lua: DA CO - bo qua")
    else:
        neo_e = neo.replace("\n", eol2)
        if raw2.count(neo_e) != 1:
            print("!!! LOI TO: moc neo menu %d lan (can 1)" % raw2.count(neo_e)); return 1
        ke_hoach.append((TEST_LUA, raw2.replace(neo_e, neo_e + eol2 + dong_menu, 1)))
        print("  test_loren_admin.lua: se them muc menu (moc neo trung 1 lan)")

    if not ke_hoach:
        print("\nKhong co gi de lam."); return 0

    for p, nd in ke_hoach:
        try:
            nd.encode("latin-1")
        except UnicodeEncodeError as e:
            print("!!! LOI TO: ky tu ngoai latin-1 trong %s: %s" % (os.path.basename(p), e))
            return 1

    if not (ghi or go):
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd in ke_hoach:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao); print("   sao luu -> %s" % os.path.basename(sao))
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p); return 1
        print("   DA GHI %s" % os.path.basename(p))
    print("\n  => Nap lai script (Lenh bai Admin) la dung duoc ngay, KHONG phai build.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
