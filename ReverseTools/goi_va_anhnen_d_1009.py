# -*- coding: utf-8 -*-
"""goi_va_anhnen_d_1009.py - [ANHNEN 10/09 d] dau do ben Represent3 cho anh nen canh:
ghi 10 dong dau o nhanh ve ISI_T_BITMAP16 (ca DrawImage2D lan DrawImage2DFlat) de biet
lay duoc texture khong, co bao nhieu, ve tai dau.
"""
import io
import sys

T = "\t"
TAG = "[ANHNEN 10/09 d]"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")
NL = "\r\n"
if TAG in s:
    print("da co"); sys.exit(0)

neo = (T*2 + "case ISI_T_BITMAP16:" + NL + T*3 + "{" + NL +
       T*4 + "TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(" + NL +
       T*5 + "pTemp->szImage," + T + "pTemp->uImage," + NL +
       T*5 + "pTemp->nISPosition, pTemp->nFrame, pTemp->nType);" + NL +
       T*4 + "if (!pBitmap)" + NL + T*5 + "break;" + NL)

log = NL.join([
    T*4 + "{" + T + "// " + TAG,
    T*5 + "static int s_nGhiBm = 0;",
    T*5 + "if (s_nGhiBm < 10)",
    T*5 + "{",
    T*6 + "s_nGhiBm++;",
    T*6 + "Rep3Log(\"[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p\",",
    T*7 + "pTemp->szImage, pBitmap ? \"CO\" : \"NULL\", pTemp->oPosition.nX, pTemp->oPosition.nY,",
    T*7 + "pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,",
    T*7 + "pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,",
    T*7 + "pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL);",
    T*5 + "}",
    T*4 + "}",
    ""])

n = s.count(neo)
if n != 3:
    print("FAIL neo: %d (mong 3)" % n); sys.exit(1)
dau = neo[:-len(T*4 + "if (!pBitmap)" + NL + T*5 + "break;" + NL)]
moi = dau + log + T*4 + "if (!pBitmap)" + NL + T*5 + "break;" + NL
s = s.replace(neo, moi)
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp " + TAG + " (3 nhanh)")
