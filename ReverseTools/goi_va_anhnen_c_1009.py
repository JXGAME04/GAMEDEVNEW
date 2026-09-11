# -*- coding: utf-8 -*-
"""goi_va_anhnen_c_1009.py - [ANHNEN 10/09 c] DAU DO cho "thieu anh nen phia sau".

Da biet: anh JPEG nap duoc (jx_rep3.log het dong LoadImage FAIL cho background, co "jpeg 1 lan 16.3 ms"),
may ve dung trong canh, nhung anh nen van khong hien.
Do tiep hai dau, moi dau ghi toi da 10 dong roi thoi (khong lam ban log):
  - Ben Core (KScenePlaceC::VeLopNen): tieu diem, vung kich hoat, vi tri tinh duoc, co lay duoc co anh khong.
  - Ben Represent3 (DrawImage2DFlat nhanh ISI_T_BITMAP16): lay duoc texture khong, ve tai dau, co bao nhieu.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 c]"
F_SCN = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.cpp"
F_REP = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"


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


# ---------------------------------------------------------------- Core
P = F_SCN
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = (T + "Img.oPosition.nX = x;" + NL + T + "Img.oPosition.nY = y;" + NL +
           T + "Img.oPosition.nZ = 0;" + NL)
    them = NL.join([
        T + "{" + T + "// " + TAG + " ghi 10 dong dau de biet ve o dau, co anh chua",
        T*2 + "extern int g_nCorePaintLog;",
        T*2 + "static int s_nGhi = 0;",
        T*2 + "if (g_nCorePaintLog > 0 && s_nGhi < 10)",
        T*2 + "{",
        T*3 + "s_nGhi++;",
        T*3 + "FILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        T*3 + "if (pLog)",
        T*3 + "{",
        T*4 + "fprintf(pLog, \"[ANHNEN] ve nen: tieu diem %d,%d | vung %d,%d..%d,%d | tile %d | dat tai %d,%d | co anh %dx%d | %s\\n\",",
        T*5 + "m_FocusPosition.x, m_FocusPosition.y, p->rcVung.left, p->rcVung.top, p->rcVung.right, p->rcVung.bottom,",
        T*5 + "p->nTiLe, x, y, (int)Param.nWidth, (int)Param.nHeight, Img.szImage);",
        T*4 + "fclose(pLog);",
        T*3 + "}",
        T*2 + "}",
        T + "}",
        ""])
    s = rep(s, neo, neo + them, "SCN log nen")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")

# ---------------------------------------------------------------- Represent3
P = F_REP
s, h0, lf0, crlf0 = doc(P)
NLr = "\r\n" if crlf0 else "\n"
if TAG not in s:
    neo = NLr.join([
        T*3 + "TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(",
        T*4 + "pTemp->szImage,\tpTemp->uImage,",
        T*4 + "pTemp->nISPosition, pTemp->nFrame, pTemp->nType);",
        T*3 + "if (!pBitmap)",
        T*4 + "break;",
        ""])
    them = NLr.join([
        T*3 + "TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(",
        T*4 + "pTemp->szImage,\tpTemp->uImage,",
        T*4 + "pTemp->nISPosition, pTemp->nFrame, pTemp->nType);",
        T*3 + "{" + T + "// " + TAG + " 10 dong dau: lay duoc texture khong, co bao nhieu, ve tai dau",
        T*4 + "static int s_nGhiBm = 0;",
        T*4 + "if (s_nGhiBm < 10)",
        T*4 + "{",
        T*5 + "s_nGhiBm++;",
        T*5 + "Rep3Log(\"[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p\",",
        T*6 + "pTemp->szImage, pBitmap ? \"CO\" : \"NULL\", pTemp->oPosition.nX, pTemp->oPosition.nY,",
        T*6 + "pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,",
        T*6 + "pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,",
        T*6 + "pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL);",
        T*4 + "}",
        T*3 + "}",
        T*3 + "if (!pBitmap)",
        T*4 + "break;",
        ""])
    s = rep(s, neo, them, "REP log bitmap")
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG " + TAG)
