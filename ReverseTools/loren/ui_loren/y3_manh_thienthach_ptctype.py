# -*- coding: utf-8 -*-
"""y3_manh_thienthach_ptctype.py - VA NOT: Manh thien thach bi tu choi o o tu chon.

DO THAT tu `loren_kiem.log` cua chu game:
    [LOREN-KIEM] type=6 O TU CHON n=1 -> tu choi (ma 8) tai o thu 0
    [LOREN-KIEM]   khoa so 18: co=1, 18 tiet doan
    [LOREN-KIEM]   TUCHON o0: genre=4 detail=1317 ptc=0 cap=1 he=-1 chong=1
Manh thien thach co genre=4 detail=1317 nhung ptc=0, trong khi
`[ConsumeItem_10] PtcType=1` => KhopTietDoan truot => RULE_ERROR.

DAY LA Y HET LOI THUY TINH DA VA O w7, va la CHO TOI VA SOT:
genre 4 = item_task, bang `questkey.txt` cua JX1 co 11 cot va KHONG HE CO cot
ParticularType (Linux 9 cot, cot 8 la ParticularType). Nen MOI vat pham questkey
ben JX1 deu co nParticular = 0, khong the bang 1.
w7 chi go `PtcType=1` cho ba muc Thuy Tinh (ConsumeItem_2/3/4) ma BO SOT chin
muc Manh thien thach (ConsumeItem_10..18) - cung genre 4, cung dinh y het.

MIENG VA: go dong `PtcType=1` o chin muc ConsumeItem_10..18.
Truong vang mat = -1 = KHONG KIEM. `Genre=4` + `DetailType=1317..1325` da dinh
danh DUY NHAT chin manh thien thach (DetailType la khoa rieng trong questkey.txt)
nen khong noi long bo loc.

KIEM CHEO: quet TOAN BO ini tim moi muc con `Genre=4` kem `PtcType`, de lan nay
khong sot muc nao nua.

KHONG PHAI BUILD: chi la tep du lieu.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_thienthach lan dau).
"""
import io
import os
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\settings\item\foundryresdemand.ini")
HAU_TO = ".truoc_thienthach"
NHAN = "; [LOREN 28/08] questkey (genre 4) khong co cot ParticularType"


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y3_manh_thienthach_ptctype - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    dong = raw.split(eol)

    # --- quet TOAN BO: muc nao co Genre=4 va con PtcType dang hoat dong ---
    cur = None
    muc = {}
    for i, l in enumerate(dong):
        s = l.strip()
        if s.startswith("["):
            cur = s.strip("[]")
            muc[cur] = {"dong": {}, "bd": i}
        elif cur and "=" in s and not s.startswith(";"):
            k, v = s.split("=", 1)
            muc[cur]["dong"][k.strip()] = (i, v.strip())

    can = []
    for ten, d in muc.items():
        g = d["dong"].get("Genre")
        p = d["dong"].get("PtcType")
        if g and g[1] == "4" and p:
            can.append((ten, d["dong"]["DetailType"][1] if "DetailType" in d["dong"] else "?",
                        p[0], p[1]))
    can.sort(key=lambda x: x[2])
    print("  quet toan bo ini: %d muc co Genre=4 ma VAN con PtcType" % len(can))
    for ten, dt, i, v in can:
        print("     [%-16s] DetailType=%-6s PtcType=%s (dong %d)" % (ten, dt, v, i + 1))
    if not can:
        print("  Khong con muc nao - DA VA HET.")
        return 0

    moi = list(dong)
    for ten, dt, i, v in can:
        moi[i] = NHAN + eol + "; " + dong[i].strip() + "   ; bo dieu kien nay"
    nd = eol.join(moi)

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    # chot lai
    cur = None
    con = 0
    for l in nd.split(eol):
        s = l.strip()
        if s.startswith("["):
            cur = s.strip("[]")
        elif cur and "=" in s and not s.startswith(";"):
            k, v = s.split("=", 1)
            if k.strip() == "PtcType":
                # xem muc nay co Genre=4 khong
                pass
    # kiem lai bang cach phan tich lai
    cur = None
    m2 = {}
    for l in nd.split(eol):
        s = l.strip()
        if s.startswith("["):
            cur = s.strip("[]")
            m2[cur] = {}
        elif cur and "=" in s and not s.startswith(";"):
            k, v = s.split("=", 1)
            m2[cur][k.strip()] = v.strip()
    for ten, d in m2.items():
        if d.get("Genre") == "4" and "PtcType" in d:
            con += 1
    print("  chot: sau khi va con %d muc Genre=4 kem PtcType (mong 0)" % con)
    if con:
        print("!!! LOI TO: van con sot")
        return 1
    print("  byte cao %d (khong doi)" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
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
    print("  DA GHI. => KHONG phai build; khoi dong lai may chu de nap lai du lieu.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
