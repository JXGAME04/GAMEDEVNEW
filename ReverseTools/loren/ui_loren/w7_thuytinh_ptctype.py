# -*- coding: utf-8 -*-
"""w7_thuytinh_ptctype.py - VA "bo THUY TINH vao o tu chon bao sai nguyen lieu".

CHU GAME: "kham nam bo phuc duyen va thuy tinh vao se duoc tang ti le opt cao,
nhung toi bo thuy tinh va phuc duyen vao bao SAI NGUYEN LIEU".

GOC THAT - JX1 KHONG CO cot ParticularType cho vat pham questkey.

    settings/item/questkey.txt
        LINUX :  9 cot, cot 8 = ParticularType     -> Thuy Tinh ghi 1
        JX1   : 11 cot: Name Genre DetailType Image ObjIdx Width Height
                        Intro Price ShortKey MaxStack
                => KHONG HE CO cot ParticularType
    (KItem.cpp:3116 da ghi nhan san: "questkey 11 cot ... khong he co".)

`foundryresdemand.ini` duoc nan tu ban Linux nen giu nguyen dieu kien:
    [ConsumeItem_2] Genre=4  DetailType=238  PtcType=1  Stackable=1   (Lam Thuy Tinh)
    [ConsumeItem_3] ... 239 ...                                        (Tu Thuy Tinh)
    [ConsumeItem_4] ... 240 ...                                        (Luc Thuy Tinh)
Nhung ben JX1 vien Thuy Tinh KHONG THE co nParticular == 1 (khong co cho luu),
nen `KFoundryResDemand::KhopTietDoan` truot ngay o dieu kien PtcType
=> `CheckTuChon` tra FOUNDRY_RESULT_RULE_ERROR (ma 8) = "nguyen lieu khong dung
   luat ghep".

KIEM CHEO khop bang chung cu: log `KSG_CompoundLog_20260827.txt` co
`SRC_EX: {0, 6, 1, 123, ...} x3` - tuc BA VIEN PHUC DUYEN DA VAO DUOC o tu chon.
Phuc Duyen la genre 6 (bang magicscript CO cot ParticularType) nen khop binh
thuong. Chi Thuy Tinh (genre 4 = questkey) moi dinh. Dung nhu chu game ta:
"bo thuy tinh va phuc duyen vao bao sai" - thu pham la Thuy Tinh.

MIENG VA (sua DU LIEU, khong dong toi ma):
Bo dong `PtcType=1` o ba muc ConsumeItem_2/3/4. Truong VANG MAT = -1 = KHONG
KIEM (dung co che ky tu dai dien co san cua KFoundryResDemand, xem chu thich
"*** MOI TRUONG MAC DINH -1 = KHONG KIEM ***" trong KItemCompound.cpp).
`Genre=4` + `DetailType=238/239/240` da dinh danh DUY NHAT ba vien Thuy Tinh
(DetailType la khoa rieng trong questkey.txt), nen khong he noi long bo loc.

KHONG chon cach them cot ParticularType vao questkey.txt: do la doi CAU TRUC
BANG DU LIEU dung chung, rui ro hon nhieu ma khong duoc them gi.

KHONG PHAI BUILD: chi la tep du lieu, nap lai la chay.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_thuytinh lan dau).
"""
import io
import os
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\settings\item\foundryresdemand.ini")
HAU_TO = ".truoc_thuytinh"
NHAN = "; [LOREN 28/08] JX1 khong co cot ParticularType cho questkey"

# ba muc can go dieu kien PtcType, kem DetailType de doi chieu cho chac
CAN = [("ConsumeItem_2", "238", "Lam Thuy Tinh"),
       ("ConsumeItem_3", "239", "Tu Thuy Tinh"),
       ("ConsumeItem_4", "240", "Luc Thuy Tinh")]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w7_thuytinh_ptctype - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

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
    moi = list(dong)
    so = 0

    for ten, detail, nhan in CAN:
        # tim muc
        i0 = None
        for i, l in enumerate(dong):
            if l.strip() == "[%s]" % ten:
                i0 = i
                break
        if i0 is None:
            print("!!! LOI TO: khong thay muc [%s]" % ten)
            return 1
        # doc cac dong cua muc
        gia = {}
        vt_ptc = None
        for j in range(i0 + 1, min(i0 + 12, len(dong))):
            s = dong[j].strip()
            if s.startswith("["):
                break
            if "=" in s:
                k, v = s.split("=", 1)
                gia[k.strip()] = v.strip()
                if k.strip() == "PtcType":
                    vt_ptc = j
        # doi chieu: dung muc chua Thuy Tinh
        if gia.get("Genre") != "4" or gia.get("DetailType") != detail:
            print("!!! LOI TO: [%s] khong phai %s (Genre=%s DetailType=%s)"
                  % (ten, nhan, gia.get("Genre"), gia.get("DetailType")))
            return 1
        if vt_ptc is None:
            print("  [%s] %s: khong co PtcType - bo qua" % (ten, nhan))
            continue
        if gia.get("PtcType") != "1":
            print("!!! LOI TO: [%s] PtcType=%s (mong 1)" % (ten, gia.get("PtcType")))
            return 1
        moi[vt_ptc] = NHAN + eol + "; " + dong[vt_ptc].strip() + "   ; bo dieu kien nay"
        so += 1
        print("  ok  [%s] %-16s Genre=4 DetailType=%s -> go `PtcType=1` (dong %d)"
              % (ten, nhan, detail, vt_ptc + 1))

    if so != 3:
        print("!!! LOI TO: mong go 3 muc, thay %d" % so)
        return 1

    nd = eol.join(moi)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    # chot: khong con muc ConsumeItem nao con PtcType=1 voi Genre=4
    for ten, detail, _ in CAN:
        i0 = [i for i, l in enumerate(nd.split(eol)) if l.strip() == "[%s]" % ten][0]
        for j in range(i0 + 1, i0 + 12):
            s = nd.split(eol)[j].strip()
            if s.startswith("["):
                break
            if s.startswith("PtcType"):
                print("!!! LOI TO: [%s] van con %s" % (ten, s))
                return 1
    print("  chot: ca ba muc khong con dong PtcType dang hoat dong")
    print("  byte cao %d (khong doi)" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => KHONG phai build; nap lai du lieu (khoi dong lai may chu) la chay.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
