# -*- coding: utf-8 -*-
#
# [VATPHAM 12/09 d] Chu 12:40:
#  (1) "nem do ra ngoai no nem cung loai toan bo item - dung la nem 1 vien thi ra 1 vien thoi": bo GDI_THROW_ALL_ITEM
#      (nem CA LOAI), doi sang nhac mon dang cham len tay (GOI_SWITCH_OBJECT chi co Pick) roi ThrowAwayItem()
#      (nem DUNG mon dang cam). Nem hong (do khoa / hoang kim / nhiem vu) thi tra mon ve dung o cu.
#  (2) "kich ra ngoai man hinh thi thong tin item tat nhung cac nut chuc nang van con": them JxVatPham_ChamNgoai(x, y)
#      goi tu KSdlApp moi lan dat ngon xuong - cham ra ngoai dai nut thi dong dai nut (cu cham di tiep nhu cu).
#  (3) "cac nut chuc nang qua to lam nho lai": 100x34 -> 76x26.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VATPHAM 12/09 d]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


# ---------------- UiVatPham.cpp ----------------
C = "Sources/S3Client/Ui/UiCase/UiVatPham.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    #   (3) nut nho lai
    s = thay(s, ["#define VP_NUT_RONG\t\t100\t\t// dung co anh nut kho VNKU (android/anh_vatpham_vnku.py: nut ngoc UiAutoNew)",
                 "#define VP_NUT_CAO\t\t34"],
                ["#define VP_NUT_RONG\t\t76\t\t// %s chu: \"cac nut chuc nang qua to lam nho lai\"" % DAU,
                 "#define VP_NUT_CAO\t\t26"], "co nut")
    #   (1) nem dung mot mon
    s = thay(s, ["\tcase VP_NEM:\t\t// nem bo (loi tu chan do khoa / vang bac)",
                 "\t\tg_pCoreShell->OperationRequest(GDI_THROW_ALL_ITEM, (KUPARAM)(&Obj), 0);",
                 "\t\tbreak;"],
                ["\tcase VP_NEM:",
                 "\t\t//\t%s chu: \"nem 1 vien thi ra 1 vien thoi chu khong nem toan bo\"." % DAU,
                 "\t\t//\tGDI_THROW_ALL_ITEM nem CA LOAI -> thay bang: nhac DUNG mon nay len tay",
                 "\t\t//\t(GOI_SWITCH_OBJECT chi co Pick) roi ThrowAwayItem() nem mon dang cam.",
                 "\t\t//\tLoi tu chan do khoa / hoang kim / nhiem vu: nem hong thi tra mon ve dung o cu.",
                 "\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (KUPARAM)(&Obj), 0);",
                 "\t\tif (g_pCoreShell->ThrowAwayItem() == 0)",
                 "\t\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 0, (KNPARAM)(&Obj));",
                 "\t\tbreak;"], "nem mot mon")
    #   (2) cham ra ngoai thi dong
    s = thay(s, ["int KUiVatPham::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)"],
                ["//\t%s Cham ra NGOAI dai nut thi dong no (KSdlApp goi moi lan dat ngon xuong)." % DAU,
                 "//\tTra 0 de cu cham van di tiep nhu cu; cham TRUNG dai nut thi de he cua so lo (nut tu nhan).",
                 "extern \"C\" int JxVatPham_ChamNgoai(int x, int y)",
                 "{",
                 "\tKUiVatPham* p = KUiVatPham::GetIfVisible();",
                 "\tint nL = 0, nT = 0, nW = 0, nH = 0;",
                 "",
                 "\tif (p == NULL)",
                 "\t\treturn 0;",
                 "\tp->GetAbsolutePos(&nL, &nT);",
                 "\tp->GetSize(&nW, &nH);",
                 "\tif (x < nL || x >= nL + nW || y < nT || y >= nT + nH)",
                 "\t\tKUiVatPham::Dong();",
                 "\treturn 0;",
                 "}",
                 "",
                 "int KUiVatPham::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)"], "cham ngoai")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (cpp)")
    ghi(C, s); print("da va:", C)

# ---------------- KSdlApp.cpp: goi luc dat ngon xuong ----------------
K = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\t\tif (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))",
                 "\t\t\t{",
                 "\t\t\t\tm_nCham = CHAM_KHONG;",
                 "\t\t\t\treturn true;",
                 "\t\t\t}"],
                ["\t\t\tif (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))",
                 "\t\t\t{",
                 "\t\t\t\tm_nCham = CHAM_KHONG;",
                 "\t\t\t\treturn true;",
                 "\t\t\t}",
                 "\t\t\t{\t// %s dai nut vat pham dang mo: cham ra ngoai no thi dong (nhu bang chu giai tu tat)" % DAU,
                 "\t\t\t\textern \"C\" int JxVatPham_ChamNgoai(int x, int y);",
                 "\t\t\t\tJxVatPham_ChamNgoai(m_nChamX0, m_nChamY0);",
                 "\t\t\t}"], "goi cham ngoai")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSdlApp)")
    ghi(K, s); print("da va:", K)
print("xong")
