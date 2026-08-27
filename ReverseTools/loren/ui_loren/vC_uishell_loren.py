# -*- coding: utf-8 -*-
# ============================================================================
# vC_uishell_loren.py - Ghi ten KUiCompoundItem (he lo ren 7 khung) vao bo
#                       dong cua so chung o S3Client\Ui\UiShell.cpp
#
# GOC LOI (da lan theo ma, xac nhan 27/08):
#   UiShell.cpp co du ba cho cho cua so anh em KUiTrembleItem (kham nam do
#   xanh) nhung KHONG co KUiCompoundItem:
#     - :529  khoi kiem visible cua UiCloseWndsInGame(bAll=false)
#     - :565  nhanh else (bAll==true) - dong dut khoat
#     - :627  duoi ham - dong theo bien bAll
#   He qua:
#     (a) ESC (ShortcutKey.cpp:249 goi UiCloseWndsInGame(false)) khong dong
#         duoc cua so lo ren, va con mo bang ESC ra menu he thong dong len tren;
#     (b) GDCNI_TRADE_START / GDCNI_GAMBLE_START
#         (GameSpaceChangedNotify.cpp:357/382) goi UiCloseWndsInGame(false)
#         de don man hinh -> don het cua so khac tru lo ren -> chong cua so;
#     (c) UiEndGame (UiShell.cpp:649) thoat ve login/disconnect goi
#         UiCloseWndsInGame(true) nhung KHONG cham toi lo ren -> con tro
#         m_pSelf + trang thai Show song sang PHIEN SAU: vao game lai cua so
#         tu hien, pad giu uId do cua phien truoc trong khi mang Item[] da
#         tai cap phat -> o hien thi DO MA / sai hinh.
#
# 4 MIENG VA (ba cho tren + mot dong #include):
#   0. Them  #include "UiCase/UiCompoundItem.h"  ngay duoi include cua
#      UiTrembleItem.h (:58). UiShell.cpp hien KHONG include header nay
#      (dem KUiCompoundItem trong tep = 0). Duong include "UiCase/..." da
#      duoc chung minh la dung: GameSpaceChangedNotify.cpp - cung thu muc
#      S3Client\Ui - dang include y het o dong 62 va bien dich duoc.
#   1. Them KUiCompoundItem::GetIfVisible() == NULL &&  ngay duoi dong :529.
#   2. Them KUiCompoundItem::CloseWindow(true);         ngay duoi dong :565.
#   3. Them KUiCompoundItem::CloseWindow(bAll);         ngay duoi dong :627.
#
# CHU KY THAT (doc that, KHONG suy doan):
#   UiCompoundItem.h:318  static KUiCompoundItem* GetIfVisible();
#   UiCompoundItem.h:319  static void   CloseWindow(bool bDestory = TRUE);
#   UiTrembleItem.h:28    static        KUiTrembleItem* GetIfVisible();
#   UiTrembleItem.h:29    static void   CloseWindow(bool bDestory = TRUE);
#   => TRUNG KHUON HOAN TOAN, chep nguyen dang bieu thuc cua tremble.
#   Than ham cung cung khuon: CloseWindow guard "if (m_pSelf)", Hide(), neu
#   bDestory thi Destroy() + m_pSelf = NULL (UiCompoundItem.cpp:150-187);
#   GetIfVisible tra m_pSelf khi IsVisible() (UiCompoundItem.cpp:110-115).
#   Nho guard + gan NULL nay ma viec goi hai lan khi bAll==true (mieng 2 roi
#   mieng 3) la vo hai - dung y het cach tremble dang chay.
#
# THUT LE - chep dung theo dong neo (da soi byte, KHONG doan):
#   :529 ba tab   (\t\t\t) - trong khoi if long hai lop
#   :565 hai tab  (\t\t)   - trong nhanh else
#   :627 mot tab  (\t)     - than ham
#
# Mac dinh DIEN TAP (khong ghi). Ghi that: python vC_uishell_loren.py --ghi
# Sao luu lan ghi dau: UiShell.cpp.truoc_uiloren2
# Doc/ghi latin-1 giu nguyen byte TCVN3; moc neo toan ASCII; giu EOL troi.
# KHONG BUILD.
# ============================================================================
import io
import os
import sys

TEP = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiShell.cpp"
SAO_LUU = TEP + ".truoc_uiloren2"


def theo_eol(s, eol):
    """Doi moc neo viet bang \\n sang EOL troi cua tep."""
    return s.replace("\n", eol)


# ---------------------------------------------------------------------------
# Danh sach mieng va: (ten, chuoi_cu, chuoi_moi, dau_hieu_da_va)
#   - chuoi_cu PHAI xuat hien dung 1 lan (dem truoc; lech -> LOI TO, khong ghi)
#   - dau_hieu_da_va: chuoi rieng cua mieng, co san -> bo qua (idempotent)
# ---------------------------------------------------------------------------
VA = []

# --- 0. Them dong #include ngay duoi UiTrembleItem.h (:58) -----------------
# Khuon include cua tremble la dong tran, khong chu thich; dong ke duoi
# (UiDiceItem.h) dung mot TAB roi "// ...". Theo khuon dice de danh dau nguon.
VA.append((
    "0. #include UiCase/UiCompoundItem.h (duoi UiTrembleItem.h :58)",
    '#include "UiCase/UiTrembleItem.h"\n',
    '#include "UiCase/UiTrembleItem.h"\n'
    '#include "UiCase/UiCompoundItem.h"\t// [UILOREN] he lo ren 7 khung\n',
    '#include "UiCase/UiCompoundItem.h"',
))

# --- 1. Khoi kiem visible cua UiCloseWndsInGame(false) (:529) --------------
# Chuoi dieu kien noi bang "&&"; dong cuoi cung (:545 KUiMiniMap) khong co
# "&&" nen chen NGAY DUOI tremble la giu nguyen cu phap.
# Tac dung: ESC dang mo cua so lo ren se tra ve true (co cua so de dong)
# thay vi rot xuong nhanh mo KUiESCDlg.
VA.append((
    "1. KUiCompoundItem::GetIfVisible() == NULL && (duoi :529)",
    "\t\t\tKUiTrembleItem::GetIfVisible() == NULL &&\n",
    "\t\t\tKUiTrembleItem::GetIfVisible() == NULL &&\n"
    "\t\t\tKUiCompoundItem::GetIfVisible() == NULL &&\n",
    "KUiCompoundItem::GetIfVisible() == NULL &&",
))

# --- 2. Nhanh else bAll==true (:565) ---------------------------------------
# Tremble truyen thang hang so true (KHONG phai bAll) o cho nay - chep y het.
VA.append((
    "2. KUiCompoundItem::CloseWindow(true); (duoi :565)",
    "\t\tKUiTrembleItem::CloseWindow(true);\n",
    "\t\tKUiTrembleItem::CloseWindow(true);\n"
    "\t\tKUiCompoundItem::CloseWindow(true);\n",
    "KUiCompoundItem::CloseWindow(true);",
))

# --- 3. Duoi ham, dong theo bien bAll (:627) -------------------------------
# Cho nay tremble truyen BIEN bAll - chep dung bien do, khong doi thanh true.
VA.append((
    "3. KUiCompoundItem::CloseWindow(bAll); (duoi :627)",
    "\tKUiTrembleItem::CloseWindow(bAll);\n",
    "\tKUiTrembleItem::CloseWindow(bAll);\n"
    "\tKUiCompoundItem::CloseWindow(bAll);\n",
    "KUiCompoundItem::CloseWindow(bAll);",
))


# ---------------------------------------------------------------------------
def dem_eol(s):
    crlf = s.count("\r\n")
    lf_don = s.count("\n") - crlf
    cr_don = s.count("\r") - crlf
    return crlf, lf_don, cr_don


def main():
    ghi = ("--ghi" in sys.argv)

    if not os.path.isfile(TEP):
        print("LOI: khong thay tep dich: %s" % TEP)
        sys.exit(2)

    f = io.open(TEP, "rb")
    goc = f.read().decode("latin-1")
    f.close()

    eol = "\r\n" if goc.count("\r\n") * 2 >= goc.count("\n") else "\n"
    crlf0, lf0, cr0 = dem_eol(goc)

    print("Tep dich : %s" % TEP)
    print("Kich thuoc TRUOC: %d byte" % len(goc))
    print("EOL TRUOC: CRLF=%d  LF_don=%d  CR_don=%d  (EOL troi: %s)"
          % (crlf0, lf0, cr0, "CRLF" if eol == "\r\n" else "LF"))
    print("So mieng va: %d" % len(VA))
    print("Che do   : %s"
          % ("GHI THAT" if ghi else "DIEN TAP (khong ghi; muon ghi them --ghi)"))
    print("-" * 74)

    data = goc
    loi = []
    so_ap = 0
    so_bo = 0

    for ten, cu, moi, dau in VA:
        cu2 = theo_eol(cu, eol)
        moi2 = theo_eol(moi, eol)
        dau2 = theo_eol(dau, eol)

        if dau2 in data:
            print("BO QUA (da va tu truoc): %s" % ten)
            so_bo += 1
            continue

        n = data.count(cu2)
        print("moc neo '%s' -> trung %d lan" % (ten, n))
        if n != 1:
            loi.append("  - '%s': moc neo xuat hien %d lan (can dung 1 lan)"
                       % (ten, n))
            continue

        data = data.replace(cu2, moi2, 1)
        so_ap += 1
        print("AP: %s" % ten)

    print("-" * 74)

    if loi:
        print("!" * 74)
        print("LOI TO - MOC NEO KHONG KHOP, KHONG GHI GI CA:")
        for d in loi:
            print(d)
        print("!" * 74)
        sys.exit(2)

    crlf1, lf1, cr1 = dem_eol(data)
    print("Kich thuoc SAU  : %d byte (lech %+d)" % (len(data), len(data) - len(goc)))
    print("EOL SAU  : CRLF=%d  LF_don=%d  CR_don=%d" % (crlf1, lf1, cr1))
    if lf1 != lf0 or cr1 != cr0:
        print("!" * 74)
        print("LOI TO - EOL LE BI DOI (LF_don %d->%d, CR_don %d->%d)."
              " KHONG GHI GI CA." % (lf0, lf1, cr0, cr1))
        print("!" * 74)
        sys.exit(2)

    print("Tong ket: ap %d, bo qua %d, loi 0." % (so_ap, so_bo))

    if not ghi:
        print("DIEN TAP xong - tep dich CHUA bi dong den.")
        return

    if so_ap == 0:
        print("Khong co gi moi de ghi.")
        return

    if not os.path.exists(SAO_LUU):
        f = io.open(SAO_LUU, "wb")
        f.write(goc.encode("latin-1"))
        f.close()
        print("Da sao luu: %s" % SAO_LUU)

    f = io.open(TEP, "wb")
    f.write(data.encode("latin-1"))
    f.close()
    print("DA GHI %d mieng va vao %s" % (so_ap, TEP))

    # Doc lai tu dia de chac chan khong bi ghi nua chung (bay driver UTF-16)
    f = io.open(TEP, "rb")
    lai = f.read().decode("latin-1")
    f.close()
    if lai != data:
        print("!" * 74)
        print("LOI TO - DOC LAI TU DIA KHONG KHOP BAN VUA GHI. Khoi phuc tu %s"
              % SAO_LUU)
        print("!" * 74)
        sys.exit(3)
    crlf2, lf2, cr2 = dem_eol(lai)
    print("KIEM LAI TU DIA: %d byte, CRLF=%d LF_don=%d CR_don=%d - KHOP."
          % (len(lai), crlf2, lf2, cr2))


if __name__ == "__main__":
    main()
