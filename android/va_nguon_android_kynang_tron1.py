# -*- coding: utf-8 -*-
#
# [KYNANG 12/09 TRON] Chu 15:05: "cho ky nang nam trong o ky nang TRON chu hien tai vuong xau".
# Bieu tuong ky nang la anh VUONG, dat trong nut tron nen 4 goc thua ra. Represent chi cat duoc theo HINH CHU NHAT
# (DrawSpritePartAlpha co tham so rect) nen cach lam: ve anh thanh nhieu DAI NGANG, moi dai cat theo day cung cua
# duong tron o do -> nhin ra hinh tron. Chi Android:
#   - KRepresentShell3::DrawImage2DStretch: neu bRenderFlag co bit RUIMAGE_RENDER_FLAG_CAT_KHUNG thi cat theo
#     oImgLTPos / oImgRBPos cua chinh muc do (cau truc KRUImagePart) thay vi ca man hinh.
#   - KSkill::DrawSkillIcon: khi ben goi cho khung (o ky nang tron) thi ve theo dai, cat tron.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[KYNANG 12/09 TRON]"


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


# ---------------- 1) co cat khung cho anh keo ----------------
U = "Sources/Represent/iRepresent/KRepresentUnit.h"
s = doc(U)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tRUIMAGE_RENDER_FLAG_FRAME_DRAW = 2,"],
                ["\tRUIMAGE_RENDER_FLAG_FRAME_DRAW = 2,",
                 "#ifdef JX_ANDROID",
                 "\tRUIMAGE_RENDER_FLAG_CAT_KHUNG = 64,\t// %s anh KEO (RU_T_IMAGE_STRETCH): cat theo oImgLTPos/oImgRBPos" % DAU,
                 "\t\t\t\t\t\t\t\t\t\t\t// cua muc (cau truc KRUImagePart) - dung de cat anh vuong thanh hinh tron",
                 "#endif"], "co cat")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KRepresentUnit.h)")
    ghi(U, s); print("da va:", U)

# ---------------- 2) Represent3: cat theo khung khi ve anh keo ----------------
R = "Sources/Represent/Represent3/KRepresentShell3.cpp"
s = doc(R)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\t\tRECT rcFull;",
                 "\t\t\trcFull.left = 0;",
                 "\t\t\trcFull.top = 0;",
                 "\t\t\trcFull.right = g_nScreenWidth;",
                 "\t\t\trcFull.bottom = g_nScreenHeight;"],
                ["\t\t\tRECT rcFull;",
                 "\t\t\trcFull.left = 0;",
                 "\t\t\trcFull.top = 0;",
                 "\t\t\trcFull.right = g_nScreenWidth;",
                 "\t\t\trcFull.bottom = g_nScreenHeight;",
                 "#ifdef JX_ANDROID",
                 "\t\t\tif (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_CAT_KHUNG)",
                 "\t\t\t{\t// %s ben goi cho khung cat rieng (KRUImagePart): dung de ve anh vuong thanh hinh tron" % DAU,
                 "\t\t\t\tKRUImagePart* pCat = (KRUImagePart*)pTemp;",
                 "\t\t\t\trcFull.left   = pCat->oImgLTPos.nX;",
                 "\t\t\t\trcFull.top    = pCat->oImgLTPos.nY;",
                 "\t\t\t\trcFull.right  = pCat->oImgRBPos.nX;",
                 "\t\t\t\trcFull.bottom = pCat->oImgRBPos.nY;",
                 "\t\t\t}",
                 "#endif"], "cat khung")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KRepresentShell3.cpp)")
    ghi(R, s); print("da va:", R)

# ---------------- 3) KSkills: ve bieu tuong theo hinh TRON ----------------
K = "Sources/Core/Src/KSkills.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["\t\t\ta.oPosition.nX = x;",
                 "\t\t\ta.oPosition.nY = y;",
                 "\t\t\ta.oPosition.nZ = 0;",
                 "\t\t\ta.oEndPos.nX = x + Width;",
                 "\t\t\ta.oEndPos.nY = y + Height;",
                 "\t\t\ta.oEndPos.nZ = 0;",
                 "\t\t\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, 1);",
                 "\t\t\treturn;"],
                ["\t\t\ta.oPosition.nX = x;",
                 "\t\t\ta.oPosition.nY = y;",
                 "\t\t\ta.oPosition.nZ = 0;",
                 "\t\t\ta.oEndPos.nX = x + Width;",
                 "\t\t\ta.oEndPos.nY = y + Height;",
                 "\t\t\ta.oEndPos.nZ = 0;",
                 "\t\t\tif (g_nJxKyNangTron)",
                 "\t\t\t{",
                 "\t\t\t\t//\t%s Chu: \"cho ky nang nam trong o ky nang TRON chu hien tai vuong xau\"." % DAU,
                 "\t\t\t\t//\tRepresent chi cat duoc theo hinh chu nhat -> ve anh thanh tung DAI NGANG, moi dai cat",
                 "\t\t\t\t//\ttheo day cung cua duong tron o do. Dai cao 2 px: bac thang o vien gan nhu khong thay,",
                 "\t\t\t\t//\tma so lan ve chi bang nua chieu cao anh.",
                 "\t\t\t\tKRUImagePart b;",
                 "\t\t\t\tint nBK = (Width < Height ? Width : Height) / 2;\t// ban kinh",
                 "\t\t\t\tint nTX = x + Width / 2, nTY = y + Height / 2;\t// tam",
                 "\t\t\t\tint nDai = 2, nY0;",
                 "",
                 "\t\t\t\tmemset(&b, 0, sizeof(b));",
                 "\t\t\t\t*(KRUImage*)&b = a;",
                 "\t\t\t\tb.bRenderFlag |= RUIMAGE_RENDER_FLAG_CAT_KHUNG;",
                 "\t\t\t\tfor (nY0 = y; nY0 < y + Height; nY0 += nDai)",
                 "\t\t\t\t{",
                 "\t\t\t\t\tint nY1 = nY0 + nDai;",
                 "\t\t\t\t\tint nGiua = (nY0 + nY1) / 2 - nTY;\t// khoang cach tu tam theo truc doc",
                 "\t\t\t\t\tint nNua, nBP;",
                 "\t\t\t\t\tif (nGiua < 0)",
                 "\t\t\t\t\t\tnGiua = -nGiua;",
                 "\t\t\t\t\tnBP = nBK * nBK - nGiua * nGiua;\t// nua day cung = can bac hai",
                 "\t\t\t\t\tif (nBP <= 0)",
                 "\t\t\t\t\t\tcontinue;",
                 "\t\t\t\t\tfor (nNua = nBK; nNua > 0 && nNua * nNua > nBP; nNua--)",
                 "\t\t\t\t\t\t;",
                 "\t\t\t\t\tif (nNua <= 0)",
                 "\t\t\t\t\t\tcontinue;",
                 "\t\t\t\t\tb.oImgLTPos.nX = nTX - nNua;",
                 "\t\t\t\t\tb.oImgLTPos.nY = nY0;",
                 "\t\t\t\t\tb.oImgRBPos.nX = nTX + nNua;",
                 "\t\t\t\t\tb.oImgRBPos.nY = nY1;",
                 "\t\t\t\t\tg_pRepresent->DrawPrimitives(1, &b, RU_T_IMAGE_STRETCH, 1);",
                 "\t\t\t\t}",
                 "\t\t\t\treturn;",
                 "\t\t\t}",
                 "\t\t\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, 1);",
                 "\t\t\treturn;"], "ve tron")
    #   co bat / tat (JxCanDieuKhien dat theo config [Cham] KyNangTron, mac dinh 1)
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\t{",
                 "\t\t//\t[KYNANG 12/09 ICON] Ben goi cho mot KHUNG (Width/Height) to hon anh - o ky nang tron cua ban mobile -"],
                ["#ifdef JX_ANDROID",
                 "\t{",
                 "\t\textern int g_nJxKyNangTron;\t// %s 1 = cat bieu tuong thanh hinh tron ([Cham] KyNangTron)" % DAU,
                 "\t\t//\t[KYNANG 12/09 ICON] Ben goi cho mot KHUNG (Width/Height) to hon anh - o ky nang tron cua ban mobile -"], "extern co")
    #   dinh nghia bien (dat canh g_nJxVeVatPham* trong KItem.cpp de cung mot noi)
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSkills.cpp)")
    ghi(K, s); print("da va:", K)

I = "Sources/Core/Src/KItem.cpp"
s = doc(I)
if DAU not in s:
    s0 = s
    s = thay(s, ["int g_nJxKeoAnhVatPham = 0;\t// [VEVATPHAM 12/09 d] 1 = keo anh vat pham cho vua o (config [Ui] KeoAnhVatPham); 0 = ve nguyen co, can giua o nhu ban PC"],
                ["int g_nJxKeoAnhVatPham = 0;\t// [VEVATPHAM 12/09 d] 1 = keo anh vat pham cho vua o (config [Ui] KeoAnhVatPham); 0 = ve nguyen co, can giua o nhu ban PC",
                 "int g_nJxKyNangTron = 1;\t// %s 1 = cat bieu tuong ky nang thanh hinh TRON khi ve vao o tron ([Cham] KyNangTron)" % DAU], "bien tron")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KItem.cpp)")
    ghi(I, s); print("da va:", I)

J = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(J)
if DAU not in s:
    s0 = s
    s = thay(s, ["\ts_nKNIconPT  = GetPrivateProfileInt(\"Cham\", \"KyNangIconPhanTram\", 78, szCfg);\t// [KYNANG 12/09 O]"],
                ["\ts_nKNIconPT  = GetPrivateProfileInt(\"Cham\", \"KyNangIconPhanTram\", 78, szCfg);\t// [KYNANG 12/09 O]",
                 "\t{\t// %s cat bieu tuong ky nang thanh hinh tron cho vua o tron (0 = de vuong nhu cu)" % DAU,
                 "\t\textern int g_nJxKyNangTron;",
                 "\t\tg_nJxKyNangTron = GetPrivateProfileInt(\"Cham\", \"KyNangTron\", 1, szCfg) ? 1 : 0;",
                 "\t}"], "doc co tron")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (JxCanDieuKhien.cpp)")
    ghi(J, s); print("da va:", J)
print("xong")
