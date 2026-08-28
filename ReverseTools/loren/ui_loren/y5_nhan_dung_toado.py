# -*- coding: utf-8 -*-
"""y5_nhan_dung_toado.py - Nhan cua trang Do pho dat DUNG toa do goc.

TRIEU CHUNG: anh chu game gui chi thay SAU nhan nguyen lieu, KHONG thay ba nhan
tinh "Do pho" / "Huyen tinh" / "Nguyen lieu". Vi khong thay nhan "Huyen tinh"
nen khong biet bo Huyen Tinh Khoang Thach vao o nao - va log may chu xac nhan
dung the:
    [LOREN-KIEM] type=6 O CHINH n=7 -> thieu NHOM BAT BUOC thu 0 (ma 4)
    (bay mon, KHONG co ptc=146)

GOC: khi viet KUiAtlas::LoadScheme toi BE NGUYEN phep tru toa do cua
KUiEnchase::LoadScheme:
    m_Pos2.SetPosition(nX - 28, nY - 4);
Ben Lay.ini/Khamnam.ini thi `<ten>Pos` la toa do CUA O, nen phai lui sang trai
28px de nhan nam CANH o. Nhung ben `Dopho.ini` thi cac muc *Pos va Material_*
DA LA toa do CUA NHAN san roi - tru them 28 la day nhan lech vao trong.
Cu the:
    CryolitePos = 156,110  ->  tru 28 = x 128
    [ItemBox]   Left=84 Width=55  =>  chiem ngang 84..139
=> nhan "Huyen tinh" roi vao 128, tuc NAM DUOI O LON giua man hinh, bi che.
(AtlasPos 66-28=38 thi con thay, nhung cung lech khoi o cua no.)

MIENG VA: dung DUNG toa do doc tu ini, khong tru gi - cho ca 3 nhan tinh lan 6
nhan nguyen lieu.

CHI doi hai so hang trong pheps SetPosition; khong dong logic nao khac.
Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_toado lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] nhan Do pho dung toa do goc"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_toado"

# (cu, moi) - chi trong than KUiAtlas::LoadScheme
DOI = [
    ("m_PosAtlas.SetPosition(nX - 28, nY - 4);",
     "m_PosAtlas.SetPosition(nX, nY);"),
    ("m_PosCryolite.SetPosition(nX - 28, nY - 4);",
     "m_PosCryolite.SetPosition(nX, nY);"),
    ("m_PosConsume.SetPosition(nX - 28, nY - 4);",
     "m_PosConsume.SetPosition(nX, nY);"),
    ("m_Material[m].SetPosition(nX - 28, nY - 4);",
     "m_Material[m].SetPosition(nX, nY);"),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y5_nhan_dung_toado - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    nd = raw
    for cu, moi in DOI:
        if nd.count(cu) != 1:
            print("!!! LOI TO: %r khop %d lan (can 1)" % (cu[:40], nd.count(cu)))
            return 1
        nd = nd.replace(cu, moi, 1)
        print("  ok  %-34s -> %s" % (cu.split(".SetPosition")[0], moi.split("SetPosition")[1]))

    # nhan idempotent
    neo = "\t\t// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang - khuon y het KUiEnchase::LoadScheme."
    if neo in nd:
        nd = nd.replace(neo, neo + eol + "\t\t// " + NHAN + ": Dopho.ini luu SAN toa do NHAN"
                        + eol + "\t\t// (khac Lay.ini luu toa do O), nen KHONG tru 28 - tru vao la nhan"
                        + eol + "\t\t// \"Huyen tinh\" roi xuong duoi o lon giua (ItemBox chiem ngang 84..139).", 1)
    else:
        # tim dong chu thich dat nhan
        for l in nd.split(eol):
            if "khuon y het KUiEnchase::LoadScheme" in l:
                nd = nd.replace(l, l + eol + T*2 + "// " + NHAN + ": Dopho.ini luu SAN toa do NHAN,"
                                + eol + T*2 + "// khac Lay.ini luu toa do O - nen KHONG tru 28.", 1)
                break

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") != raw.count("{") or nd.count("}") != raw.count("}"):
        print("!!! LOI TO: ngoac doi")
        return 1
    if "nX - 28" in nd.split("KUiAtlas::LoadScheme")[1].split("void KUiAtlas::Breathe")[0]:
        print("!!! LOI TO: van con phep tru trong KUiAtlas::LoadScheme")
        return 1
    print("  chot: khong con `nX - 28` trong KUiAtlas::LoadScheme")
    print("  byte cao %d (khong doi)" % hi0)

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
    print("  DA GHI. => build S3Client, dat Game.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
