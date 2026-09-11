# -*- coding: utf-8 -*-
#
# [OPHIM 12/09] Chu 18:05, bon viec quanh bon o phim 1-4:
#   (1) "item bo vao phim so 1 2 3 4 roi thi khong the lay ra duoc nua - dung phai de vai giay la lay ra ngoai
#       duoc": LOI DO TOI GAY RA sang nay. [ANDROID 11/09 OSO] da co duong rieng cho bon o do: cham = DUNG mon,
#       giu lau (chuot phai) = NHAC LEN TAY. Ban va [VATPHAM 12/09 g] doi giu lau thanh BAM TRAI kem co "giu"
#       cho moi o vat pham -> o phim so nhan bam trai = DUNG mon, mat han duong nhac ra.
#       Sua: o phim so / o ky nang KHONG nhan cau hoi WND_M_JX_CO_VATPHAM nua -> giu lau o do van la chuot phai.
#   (2) "cac nut phim 1 2 3 4 khi bam su dung khong can hien thong tin item do": bo bang chu giai khi tro chuot
#       o tren o phim so.
#   (3) "them nut gan item vao phim so 1 2 3 4": dai nut vat pham co them nut "Gan phim" - dat mon vao o phim
#       TRONG dau tien (khong con o trong thi dat vao o 1, mon cu tu doi ve tui).
#   (4) "cac item nao khong co duong dan script de su dung thi bo chu dung di": KItem::GetScript() rong thi
#       khong hien nut "Dung" (do mac duoc van hien "Trang bi").
# Chi Android.
import io
import os

os.chdir(r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df")
DAU = "[OPHIM 12/09]"


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


# ---------------- 1 + 2) o phim so: giu lau van la chuot phai, khong hien chu giai ----------------
C = "Sources/S3Client/Ui/Elem/WndObjContainer.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tcase WND_M_JX_CO_VATPHAM:\t// [VATPHAM 12/09 g] o don: co mon la duoc",
                 "\t\treturn (m_Object.uGenre != CGOG_NOTHING) ? 1 : 0;"],
                ["\tcase WND_M_JX_CO_VATPHAM:\t// [VATPHAM 12/09 g] o don: co mon la duoc",
                 "\t\t//\t%s O VAT PHAM SO 1-4 va o ky nang da co duong rieng tu [ANDROID 11/09 OSO]: cham = DUNG mon," % DAU,
                 "\t\t//\tgiu lau (chuot phai) = NHAC LEN TAY. Khong nhan cau hoi nay thi giu lau o do van la chuot",
                 "\t\t//\tphai - neu nhan, giu lau thanh bam trai = dung mon, mat han duong lay mon ra (chu 18:05).",
                 "\t\tif (m_nContainerId == UOC_IMMEDIA_ITEM || m_nContainerId == UOC_IMMEDIA_SKILL)",
                 "\t\t\treturn 0;",
                 "\t\treturn (m_Object.uGenre != CGOG_NOTHING) ? 1 : 0;"], "o phim khong nhan")
    s = thay(s, ["\tcase WM_MOUSEHOVER:",
                 "\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING && g_MouseOver.IsMoseHoverWndObj(this, 0) == 0)"],
                ["\tcase WM_MOUSEHOVER:",
                 "\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "#ifdef JX_ANDROID",
                 "\t\t//\t%s Chu: \"cac nut phim 1 2 3 4 khi bam su dung khong can hien thong tin item do\" - cham la" % DAU,
                 "\t\t//\tdung ngay, bang chu giai chi che mat man hinh.",
                 "\t\tif (m_nContainerId == UOC_IMMEDIA_ITEM)",
                 "\t\t\tbreak;",
                 "#endif",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING && g_MouseOver.IsMoseHoverWndObj(this, 0) == 0)"], "khong chu giai")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (WndObjContainer.cpp)")
    ghi(C, s)
    print("da va:", C)

# ---------------- 4) Core: mon co duong dan script de dung khong ----------------
K = "Sources/Core/Src/CoreShell.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId)",
                 "{"],
                ["// %s Mon nay co DUONG DAN SCRIPT de dung khong? Chu: \"cac item nao khong co duong dan script de su" % DAU,
                 "// dung thi bo chu dung di\". KItem::GetScript() = m_CommonAttrib.szScript; rong = khong dung duoc.",
                 "// Do mac duoc (item_equip) khong co script nhung van mac duoc - ben goi tu xet. Chi Android.",
                 "extern \"C\" int JxCore_VatPhamCoScript(unsigned int uId)",
                 "{",
                 "\tconst char* psz;",
                 "",
                 "\tif (uId == 0)",
                 "\t\treturn 0;",
                 "\tpsz = Item[uId].GetScript();",
                 "\treturn (psz && *psz) ? 1 : 0;",
                 "}",
                 "",
                 "extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId)",
                 "{"], "JxCore_VatPhamCoScript")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (CoreShell.cpp)")
    ghi(K, s)
    print("da va:", K)

# ---------------- 3) KUiPlayerBar: o phim trong dau tien ----------------
H = "Sources/S3Client/Ui/UiCase/UiPlayerBar.h"
s = doc(H)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tstatic void OnUseItem(int nIndex);"],
                ["\tstatic void OnUseItem(int nIndex);",
                 "#ifdef JX_ANDROID",
                 "\t//\t%s O phim so (1-4) dang TRONG dau tien, -1 neu day ca bon. Dai nut vat pham dung de biet" % DAU,
                 "\t//\tgan mon vao o nao (chu: \"them nut gan item vao phim so 1 2 3 4\").",
                 "\tstatic int OPhimTrong();",
                 "#endif"], "khai bao OPhimTrong")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (UiPlayerBar.h)")
    ghi(H, s)
    print("da va:", H)

P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = doc(P)
if DAU not in s:
    s0 = s
    s = thay(s, ["void KUiPlayerBar::OnUseItem(int nIndex)",
                 "{"],
                ["#ifdef JX_ANDROID",
                 "//\t%s O phim so dang TRONG dau tien trong bon o hien tren man (0..3), -1 neu day." % DAU,
                 "int KUiPlayerBar::OPhimTrong()",
                 "{",
                 "\tint i;",
                 "",
                 "\tif (m_pSelf == NULL)",
                 "\t\treturn -1;",
                 "\tfor (i = 0; i < 4 && i < UPB_IMMEDIA_ITEM_COUNT; i++)",
                 "\t{",
                 "\t\tKUiDraggedObject o;",
                 "\t\tm_pSelf->m_ImmediaItem[i].GetObject(o);",
                 "\t\tif (o.uGenre == CGOG_NOTHING)",
                 "\t\t\treturn i;",
                 "\t}",
                 "\treturn -1;",
                 "}",
                 "#endif",
                 "",
                 "void KUiPlayerBar::OnUseItem(int nIndex)",
                 "{"], "OPhimTrong")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (UiPlayerBar.cpp)")
    ghi(P, s)
    print("da va:", P)

# ---------------- 3 + 4) dai nut: them "Gan phim", bo "Dung" khi khong co script ----------------
U = "Sources/S3Client/Ui/UiCase/UiVatPham.cpp"
s = doc(U)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tVP_CHUYEN,\t\t\t// chuyen tiep cai bam cu cho cua so chu (mua / ban / them vao / bo ra)",
                 "\tVP_DONG,"],
                ["\tVP_CHUYEN,\t\t\t// chuyen tiep cai bam cu cho cua so chu (mua / ban / them vao / bo ra)",
                 "\tVP_GANPHIM,\t\t\t// %s gan mon vao o phim so 1-4" % DAU,
                 "\tVP_DONG,"], "enum gan phim")
    s = thay(s, ["\telse\t// hanh trang",
                 "\t{",
                 "\t\tThemNut(VP_DUNG, (m_nGenre == item_equip) ? \"trang_bi\" : \"dung\");",
                 "\t\tThemNut(VP_RAO, \"rao\");"],
                ["\telse\t// hanh trang",
                 "\t{",
                 "\t\t//\t%s Chu: \"item nao khong co duong dan script de su dung thi bo chu dung di\"." % DAU,
                 "\t\t//\tDo mac duoc van hien \"Trang bi\" (mac duoc la nho loai mon, khong can script).",
                 "\t\tif (m_nGenre == item_equip)",
                 "\t\t\tThemNut(VP_DUNG, \"trang_bi\");",
                 "\t\telse if (JxCore_VatPhamCoScript(m_Obj.uId))",
                 "\t\t\tThemNut(VP_DUNG, \"dung\");",
                 "\t\tif (m_nGenre != item_equip && KUiPlayerBar::OPhimTrong() >= -1)",
                 "\t\t\tThemNut(VP_GANPHIM, \"gan_phim\");\t// %s gan vao o phim so 1-4" % DAU,
                 "\t\tThemNut(VP_RAO, \"rao\");"], "nut gan phim + bo dung")
    s = thay(s, ["\tcase VP_CHUYEN:\t\t// cua hang / giao dich: tra ve dung cai bam cu cua cua so chu"],
                ["\tcase VP_GANPHIM:",
                 "\t\t//\t%s Gan mon vao o phim so: o TRONG dau tien; day ca bon thi dat vao o 1 - mon cu tu doi ve" % DAU,
                 "\t\t//\ttui (GOI_SWITCH_OBJECT la lenh DOI CHO, khong lam mat mon nao).",
                 "\t\t{",
                 "\t\t\tKUiObjAtContRegion Dich;",
                 "\t\t\tint nO = KUiPlayerBar::OPhimTrong();",
                 "",
                 "\t\t\tif (nO < 0)",
                 "\t\t\t\tnO = 0;",
                 "\t\t\tmemset(&Dich, 0, sizeof(Dich));",
                 "\t\t\tDich.Obj            = m_Obj;",
                 "\t\t\tDich.Region.h       = nO;",
                 "\t\t\tDich.Region.v       = 0;",
                 "\t\t\tDich.Region.Width   = m_nDataW;",
                 "\t\t\tDich.Region.Height  = m_nDataH;",
                 "\t\t\tDich.eContainer     = UOC_IMMEDIA_ITEM;",
                 "\t\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (KUPARAM)(&Obj), (KNPARAM)(&Dich));",
                 "\t\t}",
                 "\t\tbreak;",
                 "",
                 "\tcase VP_CHUYEN:\t\t// cua hang / giao dich: tra ve dung cai bam cu cua cua so chu"], "lenh gan phim")
    s = thay(s, ["extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId);\t// CoreShell.cpp"],
                ["extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId);\t// CoreShell.cpp",
                 "extern \"C\" int JxCore_VatPhamCoScript(unsigned int uId);\t// %s CoreShell.cpp" % DAU], "khai bao script")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (UiVatPham.cpp)")
    ghi(U, s)
    print("da va:", U)
print("xong")
