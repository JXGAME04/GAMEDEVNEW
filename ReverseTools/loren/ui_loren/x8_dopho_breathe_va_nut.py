# -*- coding: utf-8 -*-
"""x8_dopho_breathe_va_nut.py - VA "bam Ket hop khong hien gi" + dat 2 anh nut.

GOC: `KUiCompoundItem::Breathe()` (UiCompoundItem.cpp:272) THIEU case
WINDOWS_ATLAS. Ham nay chay NHIP cua trang dang mo:
    WINDOWS_COMP / COMP2 / COMP3 / DISTILL / FORGE / ENCHASE  -> 6 case
    WINDOWS_ATLAS                                             -> KHONG CO
Khi bam nut Ket hop, `KUiAtlas::WndProc` dat m_nStatus = STATUS_BEGIN_ATLAS roi
tra ve, va viec con lai (hien hieu ung -> chay het 25 khung -> ProcessAtlas gui
lenh) nam trong `KUiAtlas::Breathe()`. Breathe khong bao gio duoc goi => trang
thai ket vinh vien o BEGIN_ATLAS, KHONG BAO GIO gui lenh => bam nut khong hien gi.

Day la nhanh THU TU bi bo sot cung mot kieu:
    w9  KItemList.cpp:3754            duong dat vat pham (keo-tha)
    w6  KUiCompoundItem::UpdateItem   duong phan phoi trong cua so
    x6  GameSpaceChangedNotify        duong bao tin "o vua doi"
    x8  KUiCompoundItem::Breathe      duong chay NHIP cua trang
=> LUAT: them mot trang lo ren moi thi phai ra soat DU BON cho tren.

KEM THEO: dat hai anh nut con thieu (da rut o x7, doi chieu TRUNG BYTE):
    <dung hop>.spr  cho [AtlasBtn]   (nut "Che tao")
    <xem truoc>.spr cho [PreviewBtn] (nut "Xem truoc ti le thanh cong")

Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_breathe lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")

T = "\t"
NHAN = "[LOREN 28/08] Do pho: chay nhip trang"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_breathe"

CU = [
    T*2 + "case WINDOWS_ENCHASE:",
    T*3 + "m_EnchasePad.Breathe();",
    T*3 + "break;",
    T*2 + "default:",
]

MOI = [
    T*2 + "case WINDOWS_ENCHASE:",
    T*3 + "m_EnchasePad.Breathe();",
    T*3 + "break;",
    T*2 + "// " + NHAN + " - THIEU case nay thi bam nut Ket hop",
    T*2 + "// chi dat m_nStatus = STATUS_BEGIN_ATLAS roi KET vinh vien: viec hien",
    T*2 + "// hieu ung va goi ProcessAtlas (gui lenh) deu nam trong KUiAtlas::Breathe.",
    T*2 + "case WINDOWS_ATLAS:",
    T*3 + "m_AtlasPad.Breathe();",
    T*3 + "break;",
    T*2 + "default:",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x8_dopho_breathe_va_nut - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
    else:
        dong = raw.split(eol)
        vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
        if len(vt) != 1:
            print("!!! LOI TO: moc neo khop %d lan (can 1)" % len(vt))
            return 1
        i = vt[0]
        print("  ok  them case WINDOWS_ATLAS vao Breathe (dong %d)" % (i + 1))
        dong = dong[:i] + MOI + dong[i + len(CU):]
        nd = eol.join(dong)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi")
            return 1
        if nd.count("{") != raw.count("{") or nd.count("}") != raw.count("}"):
            print("!!! LOI TO: ngoac doi")
            return 1
        n_br = nd.count("Pad.Breathe();")
        print("  chot: so trang duoc chay nhip = %d (truoc 6, nay phai 7)" % n_br)
        if n_br != 7:
            print("!!! LOI TO: mong 7")
            return 1
        print("  byte cao %d (khong doi) | ngoac can bang" % hi0)
        if ghi:
            sao = DICH + HAU_TO
            if not os.path.isfile(sao):
                shutil.copy2(DICH, sao)
            with io.open(DICH, "wb") as f:
                f.write(nd.encode("latin-1"))
            if io.open(DICH, "rb").read().decode("latin-1") != nd:
                print("!!! LOI TO: doc lai KHONG khop")
                return 1
            print("  DA GHI UiCompoundItem.cpp")

    # ---------- hai anh nut ----------
    import pakdump as P
    import ucl
    ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
    DST = os.path.join(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client",
                       "spr", "Ui3", "\xd6\xfd\xd4\xec\xd7\xb0\xb1\xb8")
    BS = chr(92)
    THUMUC = "\xd6\xfd\xd4\xec\xd7\xb0\xb1\xb8"
    CAN = [("nut Che tao", "\xc8\xda\xba\xcf.spr"),
           ("nut Xem truoc", "\xd4\xa4\xc0\xc0.spr")]
    print("\n--- hai anh nut ---")
    ids = {}
    for nhan, ten in CAN:
        ids[P.name2id(BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + ten)] = (nhan, ten)
    thay = {}
    for tenpak in sorted(p for p in os.listdir(ROOT) if p.lower().endswith(".pak")):
        try:
            f, es = P.entries(os.path.join(ROOT, tenpak))
        except Exception:
            continue
        try:
            for e in es:
                if e[0] in ids:
                    u, off, size, cf = e
                    cs = cf & 0xFFFFFF
                    f.seek(off)
                    r = f.read(cs)
                    thay[e[0]] = r if (cf >> 24) == 0 else ucl.nrv2b_decompress_8(r, size)
        finally:
            f.close()
    for nhan, ten in CAN:
        b = thay.get(P.name2id(BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + ten))
        dst = os.path.join(DST, ten)
        if b is None:
            print("   !!! %s: khong thay trong pak" % nhan)
            continue
        if os.path.isfile(dst):
            print("   %-14s DA CO trong client" % nhan)
            continue
        if ghi:
            with io.open(dst, "wb") as g:
                g.write(b)
            print("   %-14s DAT (%d byte)" % (nhan, len(b)))
        else:
            print("   %-14s se dat (%d byte)" % (nhan, len(b)))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    print("\n  => build S3Client, dat lai Game.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
