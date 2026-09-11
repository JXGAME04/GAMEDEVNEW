# -*- coding: utf-8 -*-
"""goi_va_anhnen_b_1009.py - [ANHNEN 10/09 b] sua hai loi chu bao sau khi thu ban dau:

(1) "may bam theo nguoi choi chu khong tan ra": may dang ve o TOA DO MAN HINH nen dung yen so voi camera.
    Ban 2.0 ve may trong CANH. Sua: giu vi tri may o toa do CANH (the gioi), ve voi bSinglePlaneCoord = false,
    quan vong quanh khung nhin hien tai (m_RepresentArea no ra 512 diem moi ben) de luc nao cung co may tren man.

(2) "chua co anh nen mat trang noi cay cau": do duoc trong jx_rep3.log:
    "[REP3] LoadImage FAIL type=0: \\游戏资源\\background\\背景图.jpg".
    Goc: Represent3 co MAX_TEXTURE_SIZE = 1024 (BaseInclude.h), FitTextureSize tra 0 cho anh lon hon
    -> TextureResBmp::LoadJpegDecode bo anh. Cac anh nen canh deu lon hon 1024 (背景图.jpg 1640x2176,
    mogaoku.jpg 774x1500, 背景图3.jpg 971x1300, 襄阳.jpg 1200x1024...).
    Sua GON trong duong JPEG: anh <= 1024 giu y nguyen (lam tron luy thua 2 nhu cu), anh lon hon thi tao
    texture DUNG CO (D3D11 cho NPOT), tran 4096. Khong dung toi duong SPR nen khong doi gi khac.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 b]"
F_TEX = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/TextureRes.cpp"
F_SCN = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


def rep_khoang(s, dau, cuoi, new, ten):
    if s.count(dau) != 1:
        print("FAIL neo %s: dau %d" % (ten, s.count(dau))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi, a + len(dau))
    if b < 0:
        print("FAIL neo %s: khong thay cuoi" % ten); sys.exit(1)
    return s[:a] + new + s[b + len(cuoi):]


# ---------------------------------------------------------------- 1. anh JPEG lon hon 1024
P = F_TEX
s, h0, lf0, crlf0 = doc(P)
NLx = "\r\n" if crlf0 else "\n"
if TAG not in s:
    neo = (T + "m_FrameInfo.texInfo[0].nWidth = FitTextureSize(m_nWidth);" + NLx +
           T + "m_FrameInfo.texInfo[0].nHeight = FitTextureSize(m_nHeight);" + NLx)
    them = NLx.join([
        T + "// " + TAG + " anh nen canh cua ban 2.0 lon hon MAX_TEXTURE_SIZE (1024): anh nen Hoa Son 1640x2176,",
        T + "// mogaoku.jpg 774x1500... FitTextureSize tra 0 nen truoc day bo anh (LoadImage FAIL type=0).",
        T + "// Anh <= 1024 giu Y NGUYEN duong cu; anh lon hon dung DUNG CO (D3D11 cho NPOT), tran 4096.",
        T + "if (m_FrameInfo.texInfo[0].nWidth == 0 && m_nWidth > 0 && m_nWidth <= 4096)",
        T*2 + "m_FrameInfo.texInfo[0].nWidth = m_nWidth;",
        T + "if (m_FrameInfo.texInfo[0].nHeight == 0 && m_nHeight > 0 && m_nHeight <= 4096)",
        T*2 + "m_FrameInfo.texInfo[0].nHeight = m_nHeight;",
        ""])
    s = rep(s, neo, neo + them, "TEX jpeg lon", 1)
    ghi(P, s, h0, lf0, crlf0, "TextureRes.cpp")
else:
    print("TextureRes.cpp da co")

# ---------------------------------------------------------------- 2. may ve trong CANH
P = F_SCN
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    dau = "void KScenePlaceC::VeLopMay(KLopCanh* p)" + NL + "{"
    cuoi = T + "if (nSo > 0)" + NL + T*2 + "g_pRepresent->DrawPrimitives(nSo, &aImg[0], RU_T_IMAGE, true);" + NL + "}"
    than = NL.join([
        "void KScenePlaceC::VeLopMay(KLopCanh* p)",
        "{",
        T + "// " + TAG + " may nam trong CANH (toa do the gioi) chu khong dan vao man hinh, neu khong thi",
        T + "// may bam theo nguoi choi. Quan vong quanh khung nhin hien tai de luc nao cung co may tren man.",
        T + "if (p->nSo <= 0 || p->nSoAnh <= 0)",
        T*2 + "return;",
        T + "RECT rcQuan = m_RepresentArea;",
        T + "rcQuan.left -= 512; rcQuan.top -= 512; rcQuan.right += 512; rcQuan.bottom += 512;",
        T + "int nRong = rcQuan.right - rcQuan.left;",
        T + "int nCao  = rcQuan.bottom - rcQuan.top;",
        T + "if (nRong <= 0 || nCao <= 0)",
        T*2 + "return;",
        "",
        T + "DWORD dwNay = timeGetTime();",
        T + "if (p->bDaDat == 0)",
        T + "{",
        T*2 + "p->bDaDat = 1;",
        T*2 + "p->dwMoc = dwNay;",
        T*2 + "for (int k = 0; k < p->nSo; k++)",
        T*2 + "{",
        T*3 + "p->nMayX[k] = (rcQuan.left + (rand() % nRong)) << 4;",
        T*3 + "p->nMayY[k] = (rcQuan.top  + (rand() % nCao))  << 4;",
        T*3 + "p->nMayAnh[k] = rand() % p->nSoAnh;",
        T*2 + "}",
        T + "}",
        "",
        T + "// Angle thang 64: 0 = thang len, thuan kim dong ho. Toc do: nToc * 8 diem anh moi giay.",
        T + "DWORD dwCach = dwNay - p->dwMoc;",
        T + "if (dwCach > 200) dwCach = 200;" + T + "// bo qua khung dai (doi map, nap anh)",
        T + "p->dwMoc = dwNay;",
        T + "double dGoc = (double)p->nGoc * 3.14159265358979 / 32.0;",
        T + "int nDiX = (int)(sin(dGoc) * (double)p->nToc * 128.0 * (double)dwCach / 1000.0);",
        T + "int nDiY = (int)(-cos(dGoc) * (double)p->nToc * 128.0 * (double)dwCach / 1000.0);",
        "",
        T + "KRUImage aImg[LC_MAX_MAY];",
        T + "int nSo = 0;",
        T + "for (int k = 0; k < p->nSo; k++)",
        T + "{",
        T*2 + "p->nMayX[k] += nDiX;",
        T*2 + "p->nMayY[k] += nDiY;",
        T*2 + "// quan vong theo khung nhin: may ra ngoai mot ben thi dua sang ben kia (van la toa do canh)",
        T*2 + "while (p->nMayX[k] < (rcQuan.left << 4))  p->nMayX[k] += nRong << 4;",
        T*2 + "while (p->nMayX[k] > (rcQuan.right << 4)) p->nMayX[k] -= nRong << 4;",
        T*2 + "while (p->nMayY[k] < (rcQuan.top << 4))    p->nMayY[k] += nCao << 4;",
        T*2 + "while (p->nMayY[k] > (rcQuan.bottom << 4)) p->nMayY[k] -= nCao << 4;",
        "",
        T*2 + "KRUImage* q = &aImg[nSo];",
        T*2 + "memset(q, 0, sizeof(*q));",
        T*2 + "q->nType = ISI_T_SPR;",
        T*2 + "q->bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
        T*2 + "q->bRenderFlag = 0;",
        T*2 + "q->Color.Color_dw = 0xffffffff;",
        T*2 + "q->nISPosition = IMAGE_IS_POSITION_INIT;",
        T*2 + "q->nFrame = 0;",
        T*2 + "q->uImage = 0;",
        T*2 + "strncpy(q->szImage, p->szAnh[p->nMayAnh[k]], sizeof(q->szImage) - 1);",
        T*2 + "q->oPosition.nX = p->nMayX[k] >> 4;",
        T*2 + "q->oPosition.nY = p->nMayY[k] >> 4;",
        T*2 + "q->oPosition.nZ = 0;",
        T*2 + "nSo++;",
        T + "}",
        T + "if (nSo > 0)",
        T*2 + "g_pRepresent->DrawPrimitives(nSo, &aImg[0], RU_T_IMAGE, false);" + T + "// false = toa do CANH",
        "}"])
    s = rep_khoang(s, dau, cuoi, than, "SCN VeLopMay")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")
print("XONG " + TAG)
