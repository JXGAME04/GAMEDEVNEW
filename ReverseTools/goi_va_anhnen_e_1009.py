# -*- coding: utf-8 -*-
"""goi_va_anhnen_e_1009.py - [ANHNEN 10/09 e] dau do lan hai o Represent3.

Lan truoc 10 dong dau bi man dang nhap (Init_Login1024.jpg) dung het, khong thay dong nao trong game.
Lan nay: bo qua anh co chu "Login", va ghi them TRANG THAI VE luc do (cull, tron mau, alpha test, tang mau)
vi DrawBitmap16 khong tu dat trang thai - no dung y nguyen trang thai cua lenh ve truoc do.
Ben Core da chung minh: anh co that (1648x2176), dat tai -50,-344, tuc PHAI nam kin man hinh.
"""
import io
import sys

T = "\t"
TAG = "[ANHNEN 10/09 e]"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")
NL = "\r\n"
if TAG in s:
    print("da co"); sys.exit(0)

cu = NL.join([
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
    ""])

moi = NL.join([
    T*5 + "static int s_nGhiBm = 0;" + T + "// " + TAG,
    T*5 + "if (s_nGhiBm < 10 && !strstr(pTemp->szImage, \"Login\") && !strstr(pTemp->szImage, \"login\"))",
    T*5 + "{",
    T*6 + "s_nGhiBm++;",
    T*6 + "DWORD dwCull = 0, dwBlend = 0, dwSrc = 0, dwDst = 0, dwAT = 0, dwARef = 0, dwAFunc = 0, dwCOp = 0, dwCA1 = 0, dwCA2 = 0;",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_CULLMODE, &dwCull);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwBlend);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_SRCBLEND, &dwSrc);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_DESTBLEND, &dwDst);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAT);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_ALPHAREF, &dwARef);",
    T*6 + "PD3DDEVICE->GetRenderState(D3DRS_ALPHAFUNC, &dwAFunc);",
    T*6 + "PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOp);",
    T*6 + "PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);",
    T*6 + "PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG2, &dwCA2);",
    T*6 + "Rep3Log(\"[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p | cull %u tron %u (%u/%u) at %u ham %u ref %u | op %u a1 %u a2 %u\",",
    T*7 + "pTemp->szImage, pBitmap ? \"CO\" : \"NULL\", pTemp->oPosition.nX, pTemp->oPosition.nY,",
    T*7 + "pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,",
    T*7 + "pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,",
    T*7 + "pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL,",
    T*7 + "(unsigned)dwCull, (unsigned)dwBlend, (unsigned)dwSrc, (unsigned)dwDst, (unsigned)dwAT, (unsigned)dwAFunc, (unsigned)dwARef,",
    T*7 + "(unsigned)dwCOp, (unsigned)dwCA1, (unsigned)dwCA2);",
    T*5 + "}",
    ""])

n = s.count(cu)
if n != 3:
    print("FAIL neo: %d (mong 3)" % n); sys.exit(1)
s = s.replace(cu, moi)
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp " + TAG + " (3 nhanh)")
