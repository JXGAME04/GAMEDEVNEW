# -*- coding: utf-8 -*-
"""goi_va_anhnen_h_1009.py - [ANHNEN 10/09 h] ghi them so do cho dong [ANHNEN] ve nen:
kich thuoc VE that (sau khi phong), duong ve (nguyen co hay co gian), khung ve trong ini va khung nhin that.
Chu bao "van vay" sau khi da co gian -> can biet chinh xac ve rong bao nhieu, tu dau toi dau.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 h]"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)

cu = NL.join([
    T*4 + "fprintf(pLog, \"[ANHNEN] ve nen: tieu diem %d,%d | vung %d,%d..%d,%d | tile %d | dat tai %d,%d | co anh %dx%d | %s\\n\",",
    T*5 + "m_FocusPosition.x, m_FocusPosition.y, p->rcVung.left, p->rcVung.top, p->rcVung.right, p->rcVung.bottom,",
    T*5 + "p->nTiLe, x, y, (int)Param.nWidth, (int)Param.nHeight, Img.szImage);",
    ""])
moi = NL.join([
    T*4 + "fprintf(pLog, \"[ANHNEN] ve nen: tieu diem %d,%d | vung %d,%d..%d,%d | tile %d | dat tai %d,%d | anh %dx%d -> ve %dx%d (%s) | khung ini %d,%d..%d,%d | khung nhin %d,%d..%d,%d | %s\\n\",",
    T*5 + "m_FocusPosition.x, m_FocusPosition.y, p->rcVung.left, p->rcVung.top, p->rcVung.right, p->rcVung.bottom,",
    T*5 + "p->nTiLe, x, y, (int)Param.nWidth, (int)Param.nHeight, nRongVe, nCaoVe,",
    T*5 + "(nRongVe > (int)Param.nWidth || nCaoVe > (int)Param.nHeight) ? \"co gian\" : \"nguyen co\",",
    T*5 + "p->rcMan.left, p->rcMan.top, p->rcMan.right, p->rcMan.bottom,",
    T*5 + "m_RepresentArea.left, m_RepresentArea.top, m_RepresentArea.right, m_RepresentArea.bottom, Img.szImage);",
    ""])
n = s.count(cu)
if n != 1:
    print("FAIL neo: %d" % n); sys.exit(1)
s = s.replace(cu, moi)
# ghi lai moi 2 giay (thay vi 10 dong roi thoi) de lan chay nao cung co so lieu moi
cu2 = NL.join([
    T*2 + "static int s_nGhi = 0;",
    T*2 + "if (g_nCorePaintLog > 0 && s_nGhi < 10)",
    T*2 + "{",
    T*3 + "s_nGhi++;",
    ""])
moi2 = NL.join([
    T*2 + "static int s_nGhi = 0; static DWORD s_dwMocGhi = 0;" + T + "// " + TAG + " moi 2 giay mot dong, toi da 20",
    T*2 + "DWORD dwNayGhi = timeGetTime();",
    T*2 + "if (g_nCorePaintLog > 0 && s_nGhi < 20 && (s_dwMocGhi == 0 || dwNayGhi - s_dwMocGhi >= 2000))",
    T*2 + "{",
    T*3 + "s_nGhi++; s_dwMocGhi = dwNayGhi;",
    ""])
if s.count(cu2) != 1:
    print("FAIL neo 2: %d" % s.count(cu2)); sys.exit(1)
s = s.replace(cu2, moi2)
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KScenePlaceC.cpp " + TAG)
