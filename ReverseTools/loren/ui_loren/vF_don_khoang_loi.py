# -*- coding: utf-8 -*-
"""vF_don_khoang_loi.py - THEM nut "Don sach khoang loi trong tui" vao bo test lo ren.

VI SAO CAN: bo test cu (test_loren_admin.lua:155) goi
    AddItemEx( nVer, 0, 0, 6, 1, <ptc>, lv, 2, 0, 1 )
Tham so cuoi KHONG phai ma phep nhu chu thich cu tuong, ma la nMagLvl1 = cap thuoc
tinh cua O KHAM THU NHAT (chu ky that: KItemCompound.cpp:1240). Vien khoang sinh ra
mang o thuoc tinh "co cap nhung khong co noi dung" - trang thai duong choi binh
thuong khong tao ra duoc - nen RE CHUOT vao no la client SAP khi ve mo ta.

Da sua dong 155 thanh 0 (da ra trong, dung ban goc: thuoc tinh chi co khi RUT tu
trang bi xanh qua the Trich lay). Nhung nhung vien DA SINH RA truoc do van nam
trong tui, va CHU GAME KHONG THE ban/vut chung: cham chuot vao la sap ngay.

=> Can mot duong xoa CHUNG TU PHIA MAY CHU, khong dung toi chuot:
   ConsumeItem(-1, n, 6, 1, ptc) - dang JX2 cua LuaConsumeItem (ScriptFuns.cpp:3491),
   xoa theo ma o pos_equiproom + pos_equiproomex + pos_hand. Khong doc mo ta nen
   khong cham vao duong ve dang loi.

Chi xoa 6 KHOANG THUOC TINH 199..204 (thu duy nhat bo test sinh sai). KHONG dung
den Huyen Tinh 146 vi no duoc add voi nMagLvl = 0 tu dau, khong loi.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_uiloren2 lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\test_loren_admin.lua"
HAU_TO = ".truoc_uiloren2"

CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    """Doi tieng Viet co dau -> TCVN3, co cham vong tron + cam chu HOA co dau."""
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! LOI TO: chu HOA co dau khong ma hoa duoc TCVN3: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! LOI TO: cham vong tron that bai voi %r" % u)
    return t


# --- chuoi hien cho nguoi choi ---
MUC_MENU = tcvn(u"D\u1ecdn s\u1ea1ch kho\u00e1ng l\u1ed7i trong t\u00fai")
MSG_XONG = tcvn(u"\u0110\u00e3 d\u1ecdn <color=yellow>%d<color> vi\u00ean kho\u00e1ng "
                u"thu\u1ed9c t\u00ednh kh\u1ecfi t\u00fai.\\nGi\u1edd l\u1ea5y l\u1ea1i "
                u"nguy\u00ean li\u1ec7u l\u00e0 s\u1ea1ch, kh\u00f4ng c\u00f2n s\u1eadp n\u1eefa.")
MSG_LAY = tcvn(u"L\u1ea5y nguy\u00ean li\u1ec7u m\u1edbi")
MSG_QUAY = tcvn(u"Quay l\u1ea1i")
MSG_HET = tcvn(u"K\u1ebft th\u00fac")

# Moc neo phai SINH TU tieng Viet qua uni2tcvn, TUYET DOI khong go byte \xNN bang
# tay (bay da ghi trong skill: doan byte sai thi moc neo im lang khong khop).
NEO_MENU = '\t\t"%s/LR_MoThoRen",' % tcvn(
    u"Mở thợ rèn ngay tại đây")

HAM_MOI = """
--------------------------------------------------------------------------------
-- Don sach khoang thuoc tinh dang nam trong tui.
-- Bo test cu sinh khoang voi nMagLvl1 = 1 (o kham co cap nhung rong) -> re chuot
-- vao la client sap khi ve mo ta, nen KHONG the ban/vut bang tay. Xoa thang bang
-- ConsumeItem dang JX2 (khong doc mo ta, khong dung toi chuot).
--------------------------------------------------------------------------------
function LR_DonKhoangLoi()
\tlocal nDem = 0;
\tfor i = 1, getn( TBKHOANG_TEST ) do
\t\tlocal nPtc = TBKHOANG_TEST[i][1];
\t\tlocal nCon = 1;
\t\twhile nCon == 1 do
\t\t\tif( ConsumeItem( -1, 1, 6, 1, nPtc ) == 1 ) then
\t\t\t\tnDem = nDem + 1;
\t\t\telse
\t\t\t\tnCon = 0;
\t\t\tend
\t\tend
\tend
\tSayEx({ format( "@MSG_XONG@", nDem ),
\t\t"@MSG_LAY@/LR_LayChung", "@MSG_QUAY@/LR_Root", "@MSG_HET@/no" })
end
"""


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vF_don_khoang_loi - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "LR_DonKhoangLoi" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    # --- mieng 1: them muc menu vao LR_Root ---
    if raw.count(NEO_MENU) != 1:
        print("!!! LOI TO: moc neo menu xuat hien %d lan (can 1)" % raw.count(NEO_MENU))
        return 1
    dong_menu = '\t\t"%s/LR_DonKhoangLoi",' % MUC_MENU
    moi = raw.replace(NEO_MENU, NEO_MENU + eol + dong_menu, 1)
    print("  moc neo menu trung 1 lan")

    # --- mieng 2: them ham vao cuoi tep ---
    than = (HAM_MOI.replace("@MSG_XONG@", MSG_XONG)
                   .replace("@MSG_LAY@", MSG_LAY)
                   .replace("@MSG_QUAY@", MSG_QUAY)
                   .replace("@MSG_HET@", MSG_HET))
    than = than.replace("\n", eol)
    if not moi.endswith(eol):
        moi += eol
    moi += than
    print("  them ham LR_DonKhoangLoi vao cuoi tep")

    them_hi = sum(1 for c in moi if ord(c) > 127) - hi_truoc
    print("  byte cao %d -> %d (+%d, deu la chu Viet TCVN3 moi)"
          % (hi_truoc, hi_truoc + them_hi, them_hi))
    try:
        moi.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: co ky tu ngoai latin-1: %s" % e)
        return 1

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai tu dia KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
