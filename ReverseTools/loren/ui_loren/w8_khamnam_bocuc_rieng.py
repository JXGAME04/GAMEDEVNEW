# -*- coding: utf-8 -*-
"""w8_khamnam_bocuc_rieng.py - The KHAM NAM dung BO CUC RIENG (thoi muon Lay.ini).

Tu dau the Kham nam phai muon bo cuc cua the Trich lay (`Lay.ini`) vi client
JX1 THIEU anh nen `\\spr\\Ui3\\<lo ren>\\<phan trang kham nam>.spr`. Anh do da
duoc rut tu `update01.pak` cua client VLTK va dat vao client (xem w2), nen gio
co the dung dung bo cuc goc.

DOI CHIEU HAI BO CUC (doc that, khong doan):
  Khamnam.ini (goc VLTK)  27 muc: Main, BigBox, SmallBox1, SmallBox2,
      Consume1..8, GuideList, GuideList_Scroll(_Btn), EnchaseBtn, DistillBtn,
      EquipEffect, ConsumeEffect, ...Pos, TextColor, ReturnInfoVLTK, ReturnInfo
  Lay.ini (dang muon)     26 muc: ... DistillBtn, CancleBtn, NewMineBox ...

Ba diem dang chu y, deu da kiem bang toa do:
  1. `EnchaseBtn` va `DistillBtn` trong Khamnam.ini TRUNG toa do (285,208) va
     TRUNG anh (<kham nam>.spr) - la CUNG MOT NUT khai hai ten. Nen giu nguyen
     `m_Enchase.Init(&Ini, "DistillBtn")` van chay dung, va anh nut se la
     "kham nam" thay vi "trich lay" => sua luon loi "nut Kham nam mang hinh
     Trich lay" ghi trong ho so.
  2. Khamnam.ini KHONG co `CancleBtn`: ban goc the nay KHONG CO NUT HUY.
     `m_Cancle.Init` se khong tim thay muc -> nut khong hien. Dung nhu ban goc;
     nguoi choi van thu hoi nguyen lieu bang cach dong cua so (duong
     GOI_RECOVER_ITEM da co san).
  3. Khamnam.ini khong co `NewMineBox` - CtrlEnchaseItemMap khong dung o do
     (chi BigBox, SmallBox1/2, Consume1..8) nen khong anh huong.

MIENG VA:
  + `#define SCHEME_INI_KHAMNAM "khamnam/Khamnam.ini"`
  * `KUiEnchase::LoadScheme` doc SCHEME_INI_KHAMNAM thay SCHEME_INI_OUTIN
  * `KUiEnchase::ProcessEnchase` doc ReturnInfo tu dung tep do
  + dat `Khamnam.ini` vao bin\\client\\Ui\\Ui3\\khamnam\\
CHI dong toi KUiEnchase; cac the khac van doc Lay.ini nhu cu.

Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_bocuckn lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] Kham nam dung bo cuc rieng"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_bocuckn"
SRC_INI = r"D:\GAMEDEVNEW\ReverseTools\loren\ui_loren\ra_khamnam2\Khamnam.ini"
DST_INI = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
           r"\Ui\Ui3\khamnam\Khamnam.ini")


def K(*d):
    return list(d)


MIENG = [
    ("them SCHEME_INI_KHAMNAM",
     K("#define 	SCHEME_INI_ATLAS			\"khamnam/Dopho.ini\"	// [LOREN 27/08] THE DO PHO"),
     K("#define 	SCHEME_INI_ATLAS			\"khamnam/Dopho.ini\"	// [LOREN 27/08] THE DO PHO",
       "#define 	SCHEME_INI_KHAMNAM			\"khamnam/Khamnam.ini\"	// " + NHAN)),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w8_khamnam_bocuc_rieng - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

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
    for ten, cu, moi in MIENG:
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-30s khop %d lan (can 1)" % (ten, len(vt)))
            return 1
        print("  ok  %-30s dong %d" % (ten, vt[0] + 1))
        dong = dong[:vt[0]] + moi + dong[vt[0] + len(cu):]

    # doi HAI cho dung SCHEME_INI_OUTIN nam TRONG than KUiEnchase
    i_bat = None
    i_het = None
    for i, l in enumerate(dong):
        if l.startswith("void KUiEnchase::LoadScheme"):
            i_bat = i
        if i_bat is not None and l.startswith("void KUiEnchase::UpdateData"):
            i_het = i
            break
    if i_bat is None or i_het is None:
        print("!!! LOI TO: khong khoanh duoc vung KUiEnchase")
        return 1
    n = 0
    for i in range(i_bat, i_het):
        if "SCHEME_INI_OUTIN" in dong[i]:
            dong[i] = dong[i].replace("SCHEME_INI_OUTIN", "SCHEME_INI_KHAMNAM")
            n += 1
    print("  ok  %-30s doi %d cho trong than KUiEnchase (dong %d..%d)"
          % ("SCHEME_INI_OUTIN -> KHAMNAM", n, i_bat + 1, i_het))
    if n < 2:
        print("!!! LOI TO: mong it nhat 2 cho (LoadScheme + ProcessEnchase), thay %d" % n)
        return 1

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") != raw.count("{") or nd.count("}") != raw.count("}"):
        print("!!! LOI TO: ngoac doi")
        return 1
    # chot: cac lop KHAC van dung SCHEME_INI_OUTIN
    print("  chot: SCHEME_INI_OUTIN con %d cho (cac the khac), SCHEME_INI_KHAMNAM %d cho"
          % (nd.count("SCHEME_INI_OUTIN") - 1, nd.count("SCHEME_INI_KHAMNAM") - 1))
    if nd.count("SCHEME_INI_OUTIN") < 2:
        print("!!! LOI TO: da doi nham ca cac the khac")
        return 1
    print("  byte cao %d (khong doi)" % hi0)

    print("--- Khamnam.ini ---")
    if os.path.isfile(DST_INI):
        print("   DA CO - bo qua")
        dat = False
    elif not os.path.isfile(SRC_INI):
        print("   !!! khong thay ban goc %s" % SRC_INI)
        return 1
    else:
        print("   se dat (%d byte)" % os.path.getsize(SRC_INI))
        dat = True

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
    print("  DA GHI UiCompoundItem.cpp")
    if dat:
        shutil.copy2(SRC_INI, DST_INI)
        print("  DA DAT Khamnam.ini")
    print("\n  => build S3Client, dat lai Game.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
