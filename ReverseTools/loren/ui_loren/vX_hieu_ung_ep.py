# -*- coding: utf-8 -*-
"""vX_hieu_ung_ep.py - KHOI PHUC HIEU UNG CHUYEN DONG khi bam nut ep (dung ban goc).

CHU GAME: "khi bam ep dung ban linux se co he thong hieu ung chuyen dong 2-3s roi
moi thuc hien ep (hien tai toi chua thay hieu ung)".

DUNG - VA DAY LA LOI DO TOI GAY RA O DOT TRUOC.
May trang thai GOC (KUiCompoundOne::Breathe, UiCompoundItem.cpp:664-691):
    bam nut  -> m_nStatus = STATUS_BEGIN_COMPOUND; return;   (CHUA gui)
    Breathe: BEGIN_COMPOUND  -> Show 3 anh hieu ung + SetFrame(0) -> COMPOUNDING
    Breathe: COMPOUNDING     -> if(!PlayEffect()) { Hide; -> CHANGING_ITEM }
    Breathe: CHANGING_ITEM   -> ProcessComp();  <-- LUC NAY MOI GUI LENH EP
=> hieu ung chay het MAX_SPR_FRAME (25) khung roi moi gui. Dung "2-3 giay".

Loi that nam o `PlayEffect()`: than bi chu thich `/* ... */`, chi con `return 1;`
nen KHONG BAO GIO tra 0 => ket vinh vien o STATUS_COMPOUNDING => nhanh
CHANGING_ITEM (noi GUI) khong bao gio toi => cu bam DAU bi nuot va anh hieu ung
treo mai. Dot truoc toi chua bang cach BO nhip "nap" (mieng L2 trong
vB_client_ui_loren.py) - het treo va bam an ngay, nhung MAT hieu ung, khong con
giong ban goc. Day la cach chua SAI HUONG; chua dung la khoi phuc PlayEffect.

MIENG VA (6 lop x 2 viec):
  A. Bo chu thich than PlayEffect va xoa `return 1;` thua o cuoi:
        int KUiXxx::PlayEffect()
        {
            /*if(...GetCurrentFrame() >= MAX_SPR_FRAME)
            {   ...SetFrame(0);  return 0; }
            else
            {   ...NextFrame();  return 1; }*/
            return 1;      <-- xoa
        }
     Bien anh hieu ung tung lop (doc that):
        One/Two/Three : m_CompEffect1/2/3
        Distill       : m_DistillEffect1/2
        Forge         : m_ForgeEffect
        Enchase       : m_EnchaseEffect1/2
  B. Bo chu thich nhanh "nap" ma dot truoc da tat (nhan [UILOREN-L2-*]):
        if(m_nStatus == STATUS_WAITING_MATERIALS)
        { m_nStatus = STATUS_BEGIN_COMPOUND; return; }

Sau mieng nay: bam nut -> hien hieu ung -> chay 25 khung -> TU DONG gui lenh ep.
Dung nhu ban goc, va van "1 cu bam = 1 lan ep" vi trang thai tu ve WAITING sau khi
ProcessComp chay.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_hieuung lan dau).
"""
import io
import os
import re
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_hieuung"


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vX_hieu_ung_ep - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] hieu ung chay lai" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    moi_dong = list(dong)
    so_A = 0
    so_B = 0

    # ---------- A: bo chu thich than PlayEffect ----------
    for i, l in enumerate(dong):
        if "::PlayEffect()" not in l or "int " not in l:
            continue
        lop = l.split("::")[0].split()[-1]
        # tim `/*if(` va `}*/` trong than
        i_mo = None
        i_dong = None
        for j in range(i, min(i + 30, len(dong))):
            if i_mo is None and dong[j].lstrip().startswith("/*if("):
                i_mo = j
            elif i_mo is not None and dong[j].rstrip().endswith("}*/"):
                i_dong = j
                break
        if i_mo is None or i_dong is None:
            print("  ! %s: khong thay khoi chu thich - bo qua" % lop)
            continue
        # dong `return 1;` ngay sau `}*/`
        i_thua = None
        for j in range(i_dong + 1, min(i_dong + 4, len(dong))):
            if dong[j].strip() == "return 1;":
                i_thua = j
                break
        moi_dong[i_mo] = dong[i_mo].replace("/*if(", "if(", 1)
        moi_dong[i_dong] = dong[i_dong].rstrip()[:-2]          # bo `*/`
        if i_thua is not None:
            moi_dong[i_thua] = None                            # xoa dong thua
        so_A += 1
        print("  A %-18s dong %d..%d %s"
              % (lop, i_mo + 1, i_dong + 1,
                 "(+ xoa `return 1;` dong %d)" % (i_thua + 1) if i_thua else ""))

    # ---------- B: bo chu thich nhanh "nap" ----------
    for i, l in enumerate(dong):
        if "[UILOREN-L2-" not in l:
            continue
        ten = l.split("[UILOREN-L2-")[1].split("]")[0]
        n = 0
        # Tim dong mo `//if(m_nStatus ...)` roi bo `//` cho CAC DONG LIEN TIEP
        # bat dau bang `//`. Khong so sanh chinh xac tung dong: dong `//{` that
        # su la '	//{	' (co TAB THUA cuoi) nen so khop kieu `s in ("//{",...)`
        # bo sot no -> mat ngoac mo -> hong cu phap.
        i_mo = None
        for j in range(i, min(i + 16, len(dong))):
            if dong[j].lstrip().startswith("//if(m_nStatus"):
                i_mo = j
                break
        if i_mo is None:
            continue
        for j in range(i_mo, min(i_mo + 8, len(dong))):
            s = dong[j].lstrip()
            if not s.startswith("//"):
                break
            moi_dong[j] = dong[j].replace("//", "", 1)
            n += 1
            if s.rstrip().endswith("}"):      # het khoi
                break
        if n:
            so_B += 1
            print("  B %-18s bo chu thich %d dong nhanh \"nap\"" % (ten, n))

    if so_A != 6 or so_B != 6:
        print("!!! LOI TO: mong 6 A va 6 B, thay A=%d B=%d - KHONG ghi gi" % (so_A, so_B))
        return 1

    # nhan idempotent
    for i, l in enumerate(moi_dong):
        if l is not None and "::PlayEffect()" in l and "int " in l:
            moi_dong[i] = ("// [LOREN 27/08] hieu ung chay lai: bo chu thich than ham nay va" + eol +
                           "// khoi phuc nhip \"nap\" - bam nut -> hien hieu ung -> het 25 khung" + eol +
                           "// -> moi gui lenh ep, dung may trang thai goc (Breathe:664-691)." + eol + l)
            break

    moi = eol.join(x for x in moi_dong if x is not None)

    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1))
        return 1
    if moi.count("{") != moi.count("}"):
        print("!!! LOI TO: ngoac nhon lech {%d }%d" % (moi.count("{"), moi.count("}")))
        return 1
    print("  byte cao %d (khong doi) | ngoac {%d }%d can bang | CRLF %d -> %d"
          % (hi0, moi.count("{"), moi.count("}"), raw.count("\r\n"), moi.count("\r\n")))

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
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Core \"Client Release|Win32\" roi dat CoreClient.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
