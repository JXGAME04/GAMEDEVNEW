# -*- coding: utf-8 -*-
"""goi_va_anhnen_g_1009.py - [ANHNEN 10/09 g] anh nen nho hon khung ve -> CO GIAN cho phu kin.

Chu bao Mac Cao Quat "con den": anh nen da hien nhung con vien den ben phai. Do duoc:
anh mogaoku.jpg cua DU AN chi 774x1500 trong khi ban 2.0 la 1161x1500 (yueyaquan.jpg: 776x1014 vs 1164x1014),
ma khung ve la 1024x768 -> anh hep hon man hinh nen khong the phu kin.
Sua khong dung toi du lieu: anh nho hon khung ve thi ve CO GIAN theo ti le lon nhat (giu ti le khung anh)
cho phu kin, anh lon hon khung ve (vd Hoa Son 1648x2176) van ve nguyen co nhu truoc.
Duong ve co gian cua Represent3 truoc day chi nhan SPR; them nhanh ISI_T_BITMAP16 (DrawBitmap16 da co san
tham so kich thuoc va co bStretch = loc tuyen tinh).
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 g]"
F_REP = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"
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


# ---------------------------------------------------------------- Represent3: co gian anh bitmap
P = F_REP
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = T*2 + "if (pTemp->nType == ISI_T_SPR)" + NL
    them = NL.join([
        T*2 + "if (pTemp->nType == ISI_T_BITMAP16)" + T + "// " + TAG + " anh nen canh co gian cho phu kin khung ve",
        T*2 + "{",
        T*3 + "TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(",
        T*4 + "pTemp->szImage, pTemp->uImage, pTemp->nISPosition, pTemp->nFrame, pTemp->nType);",
        T*3 + "if (!pBitmap)",
        T*4 + "continue;",
        T*3 + "int nW = pTemp->oEndPos.nX - pTemp->oPosition.nX;",
        T*3 + "int nH = pTemp->oEndPos.nY - pTemp->oPosition.nY;",
        T*3 + "if (nW <= 0) nW = pBitmap->GetWidth();",
        T*3 + "if (nH <= 0) nH = pBitmap->GetHeight();",
        T*3 + "DrawBitmap16(pTemp->oPosition.nX, pTemp->oPosition.nY, nW, nH, pBitmap, true);",
        T*3 + "continue;",
        T*2 + "}",
        ""])
    s = rep(s, neo, them + neo, "REP stretch bitmap")
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ---------------------------------------------------------------- Core: tinh co gian khi anh nho hon khung
P = F_SCN
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    old = NL.join([
        T + "KImageParam Param;",
        T + "memset(&Param, 0, sizeof(Param));",
        T + "if (g_pRepresent->GetImageParam(Img.szImage, &Param, ISI_T_BITMAP16) && Param.nWidth > 0 && Param.nHeight > 0)",
        T + "{",
        T*2 + "int nRong = p->rcMan.right - p->rcMan.left;",
        T*2 + "int nCao  = p->rcMan.bottom - p->rcMan.top;",
        T*2 + "if ((int)Param.nWidth >= nRong)",
        T*2 + "{",
        T*3 + "if (x > p->rcMan.left) x = p->rcMan.left;",
        T*3 + "if (x + (int)Param.nWidth < p->rcMan.right) x = p->rcMan.right - (int)Param.nWidth;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "x = p->rcMan.left + (nRong - (int)Param.nWidth) / 2;",
        T*2 + "if ((int)Param.nHeight >= nCao)",
        T*2 + "{",
        T*3 + "if (y > p->rcMan.top) y = p->rcMan.top;",
        T*3 + "if (y + (int)Param.nHeight < p->rcMan.bottom) y = p->rcMan.bottom - (int)Param.nHeight;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "y = p->rcMan.top + (nCao - (int)Param.nHeight) / 2;",
        T + "}",
        ""])
    new = NL.join([
        T + "KImageParam Param;",
        T + "memset(&Param, 0, sizeof(Param));",
        T + "int nRongVe = 0, nCaoVe = 0;" + T + "// " + TAG + " kich thuoc ve (co gian khi anh nho hon khung)",
        T + "if (g_pRepresent->GetImageParam(Img.szImage, &Param, ISI_T_BITMAP16) && Param.nWidth > 0 && Param.nHeight > 0)",
        T + "{",
        T*2 + "int nRong = p->rcMan.right - p->rcMan.left;",
        T*2 + "int nCao  = p->rcMan.bottom - p->rcMan.top;",
        T*2 + "nRongVe = (int)Param.nWidth;",
        T*2 + "nCaoVe  = (int)Param.nHeight;",
        T*2 + "// " + TAG + " anh cua du an co the hep hon ban 2.0 (mogaoku 774 vs 1161) -> phong cho phu kin, giu ti le",
        T*2 + "if (nRongVe < nRong || nCaoVe < nCao)",
        T*2 + "{",
        T*3 + "double dTiLe = (double)nRong / (double)nRongVe;",
        T*3 + "double dTiLe2 = (double)nCao / (double)nCaoVe;",
        T*3 + "if (dTiLe2 > dTiLe) dTiLe = dTiLe2;",
        T*3 + "nRongVe = (int)(nRongVe * dTiLe + 0.5);",
        T*3 + "nCaoVe  = (int)(nCaoVe * dTiLe + 0.5);",
        T*2 + "}",
        T*2 + "if (nRongVe >= nRong)",
        T*2 + "{",
        T*3 + "if (x > p->rcMan.left) x = p->rcMan.left;",
        T*3 + "if (x + nRongVe < p->rcMan.right) x = p->rcMan.right - nRongVe;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "x = p->rcMan.left + (nRong - nRongVe) / 2;",
        T*2 + "if (nCaoVe >= nCao)",
        T*2 + "{",
        T*3 + "if (y > p->rcMan.top) y = p->rcMan.top;",
        T*3 + "if (y + nCaoVe < p->rcMan.bottom) y = p->rcMan.bottom - nCaoVe;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "y = p->rcMan.top + (nCao - nCaoVe) / 2;",
        T + "}",
        ""])
    s = rep(s, old, new, "SCN co gian")
    old2 = NL.join([
        T + "Img.oPosition.nX = x;",
        T + "Img.oPosition.nY = y;",
        T + "Img.oPosition.nZ = 0;",
        ""])
    new2 = NL.join([
        T + "Img.oPosition.nX = x;",
        T + "Img.oPosition.nY = y;",
        T + "Img.oPosition.nZ = 0;",
        T + "Img.oEndPos.nX = x + nRongVe;" + T + "// " + TAG,
        T + "Img.oEndPos.nY = y + nCaoVe;",
        T + "Img.oEndPos.nZ = 0;",
        ""])
    s = rep(s, old2, new2, "SCN oEndPos")
    old3 = T + "g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);" + T + "// true = toa do man hinh" + NL
    new3 = NL.join([
        T + "// " + TAG + " anh du to thi ve nguyen co (nhanh hon); anh phai phong thi di duong co gian",
        T + "if (nRongVe > (int)Param.nWidth || nCaoVe > (int)Param.nHeight)",
        T*2 + "g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_STRETCH, true);",
        T + "else",
        T*2 + "g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);" + T + "// true = toa do man hinh",
        ""])
    s = rep(s, old3, new3, "SCN chon duong ve")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")
print("XONG " + TAG)
