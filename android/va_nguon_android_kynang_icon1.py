# -*- coding: utf-8 -*-
#
# [KYNANG 12/09 ICON] Chu: "lam cac icon ky nang khi bo vao o ky nang to len bang o tron".
# KSkill::DrawSkillIcon BO QUA Width/Height - no ve anh o co THAT, lay (x, y) lam goc trai-tren. Vi vay dat co
# to bao nhieu cung khong doi. Nay (chi Android): neu ben goi cho mot khung (Width/Height > 0) LON HON khung anh
# thi ve bang RU_T_IMAGE_STRETCH cho VUA khung do - giu ti le, can giua theo khung anh that (nhu [VEVATPHAM 12/09]).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[KYNANG 12/09 ICON]"
TEP = "Sources/Core/Src/KSkills.cpp"


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
s = thay(s, ["\tm_RUIconImage.oPosition.nX = x;",
             "\tm_RUIconImage.oPosition.nY = y;",
             "\tm_RUIconImage.oPosition.nZ = 0;",
             "\tm_RUIconImage.nFrame = 0;",
             "\tg_pRepresent->DrawPrimitives(1, &m_RUIconImage, RU_T_IMAGE, 1);"],
            ["\tm_RUIconImage.oPosition.nX = x;",
             "\tm_RUIconImage.oPosition.nY = y;",
             "\tm_RUIconImage.oPosition.nZ = 0;",
             "\tm_RUIconImage.nFrame = 0;",
             "#ifdef JX_ANDROID",
             "\t{",
             "\t\t//\t%s Ben goi cho khung to hon anh (o ky nang tron cua ban mobile) thi KEO anh cho vua khung:" % DAU,
             "\t\t//\tgiu ti le, can giua theo KHUNG ANH THAT (hoi Represent bang GetImageFrameParam - khung khai bao",
             "\t\t//\ttrong .spr co the nho hon hinh ve that, xem [VEVATPHAM 12/09]). Khung nho hon anh thi ve nhu cu.",
             "\t\tKRPosition2 oLech, oCo;",
             "\t\toLech.nX = oLech.nY = 0;",
             "\t\toCo.nX = oCo.nY = 0;",
             "\t\tif (Width > 0 && Height > 0 && g_pRepresent",
             "\t\t\t&& g_pRepresent->GetImageFrameParam(m_RUIconImage.szImage, 0, &oLech, &oCo, m_RUIconImage.nType)",
             "\t\t\t&& oCo.nX > 0 && oCo.nY > 0)",
             "\t\t{",
             "\t\t\tint nX1 = (oLech.nX < 0) ? oLech.nX : 0;",
             "\t\t\tint nY1 = (oLech.nY < 0) ? oLech.nY : 0;",
             "\t\t\tint nX2 = (oLech.nX + oCo.nX > oCo.nX) ? (oLech.nX + oCo.nX) : oCo.nX;",
             "\t\t\tint nY2 = (oLech.nY + oCo.nY > oCo.nY) ? (oLech.nY + oCo.nY) : oCo.nY;",
             "\t\t\tint nRong = nX2 - nX1, nCao = nY2 - nY1;",
             "\t\t\tif (nRong > 0 && nCao > 0 && (Width > nRong || Height > nCao))",
             "\t\t\t{",
             "\t\t\t\tint nA = Width * 1000 / nRong;",
             "\t\t\t\tint nB = Height * 1000 / nCao;",
             "\t\t\t\tint nTiLe = (nA < nB) ? nA : nB;",
             "\t\t\t\tint nVeW = oCo.nX * nTiLe / 1000;",
             "\t\t\t\tint nVeH = oCo.nY * nTiLe / 1000;",
             "\t\t\t\tif (nVeW > 0 && nVeH > 0)",
             "\t\t\t\t{",
             "\t\t\t\t\tKRUImageStretch a;",
             "\t\t\t\t\tmemset(&a, 0, sizeof(a));",
             "\t\t\t\t\ta.nType = m_RUIconImage.nType;",
             "\t\t\t\t\ta.bRenderStyle = m_RUIconImage.bRenderStyle;",
             "\t\t\t\t\ta.Color.Color_dw = 0xffffffff;",
             "\t\t\t\t\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
             "\t\t\t\t\ta.nFrame = 0;",
             "\t\t\t\t\tstrncpy(a.szImage, m_RUIconImage.szImage, sizeof(a.szImage) - 1);",
             "\t\t\t\t\ta.szImage[sizeof(a.szImage) - 1] = 0;",
             "\t\t\t\t\ta.oPosition.nX = x + (Width  - (nX2 - nX1) * nTiLe / 1000) / 2 - nX1 * nTiLe / 1000;",
             "\t\t\t\t\ta.oPosition.nY = y + (Height - (nY2 - nY1) * nTiLe / 1000) / 2 - nY1 * nTiLe / 1000;",
             "\t\t\t\t\ta.oPosition.nZ = 0;",
             "\t\t\t\t\ta.oEndPos.nX = a.oPosition.nX + nVeW;",
             "\t\t\t\t\ta.oEndPos.nY = a.oPosition.nY + nVeH;",
             "\t\t\t\t\ta.oEndPos.nZ = 0;",
             "\t\t\t\t\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, 1);",
             "\t\t\t\t\treturn;",
             "\t\t\t\t}",
             "\t\t\t}",
             "\t\t}",
             "\t}",
             "#endif",
             "\tg_pRepresent->DrawPrimitives(1, &m_RUIconImage, RU_T_IMAGE, 1);"], "keo anh ky nang")
if cao(s) != cao(s0):
    raise SystemExit("byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
