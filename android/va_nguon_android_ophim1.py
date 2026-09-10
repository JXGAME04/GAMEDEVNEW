# -*- coding: utf-8 -*-
#
# [VATPHAM 12/09 e] Chu 15:30: "tren dien thoai vao 4 o den phim 1 2 3 4 bo item van lech".
# Bon o den la hinh ve san trong anh nen thanh duoi (khung_chat_mobile.spr) - anh do ve theo cua so 800x600
# nen tren khung ve rong bi keo lech so voi cua so o phim (do duoc: o den 1099,241 / cua so 1098,219).
# Sua: bo bon o khoi anh nen (android/anh_ophim_nen.py) va cho TUNG O PHIM tu ve nen cua no
# (\spr\ui3\uivatpham\o_phim_nen.spr) vao dung khung cua so -> o den va vat pham luon trung nhau.
# Chi Android.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VATPHAM 12/09 e]"
TEP = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"


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


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
s0 = s
B = chr(92)
ANH = B + B + "spr" + B + B + "ui3" + B + B + "uivatpham" + B + B + "o_phim_nen.spr"
s = thay(s, ["void KImmediaItem::PaintWindow()",
             "{",
             "\tKWndObjectBox::PaintWindow();"],
            ["void KImmediaItem::PaintWindow()",
             "{",
             "#ifdef JX_ANDROID",
             "\t{",
             "\t\t//\t%s O phim tu ve NEN cua no (anh cat tu chinh anh nen thanh duoi, da bo khoi anh do)." % DAU,
             "\t\t//\tTruoc day bon o den nam trong anh nen 1040x604 ve theo cua so 800x600 nen tren khung ve rong",
             "\t\t//\tno lech han so voi cua so o phim -> bo item vao thi anh mot noi, o den mot noi (chu 15:30).",
             "\t\tKRUImage a;",
             "\t\tmemset(&a, 0, sizeof(a));",
             "\t\ta.nType = ISI_T_SPR;",
             "\t\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
             "\t\ta.Color.Color_dw = 0xffffffff;",
             "\t\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
             "\t\ta.nFrame = 0;",
             "\t\tstrncpy(a.szImage, \"" + ANH + "\", sizeof(a.szImage) - 1);",
             "\t\ta.szImage[sizeof(a.szImage) - 1] = 0;",
             "\t\ta.oPosition.nX = m_nAbsoluteLeft;",
             "\t\ta.oPosition.nY = m_nAbsoluteTop;",
             "\t\ta.oPosition.nZ = 0;",
             "\t\ta.oEndPos.nX = m_nAbsoluteLeft + m_Width;",
             "\t\ta.oEndPos.nY = m_nAbsoluteTop + m_Height;",
             "\t\ta.oEndPos.nZ = 0;",
             "\t\tif (g_pRepresentShell)",
             "\t\t\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
             "\t}",
             "#endif",
             "\tKWndObjectBox::PaintWindow();"], "ve nen o phim")
if cao(s) != cao(s0):
    raise SystemExit("byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
