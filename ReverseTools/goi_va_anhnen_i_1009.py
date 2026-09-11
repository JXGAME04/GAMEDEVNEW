# -*- coding: utf-8 -*-
"""goi_va_anhnen_i_1009.py - [ANHNEN 10/09 i] GOC that cua "van con den": khung ve trong ini cua DU AN
la 800x600 (doi may cu), ban 2.0 da sua thanh 1024x768.

So do (dong [ANHNEN] ve nen o Mac Cao Quat, map 340):
  anh 784x1504 -> ve 800x1535 (co gian) | khung ini 0,0..800,600
Doi chieu tep _Scroll.ini hai ben:
  id 340: 2.0 PaintRect 1024x768 ; du an 800x600
  id 2  : 2.0 PaintRect 1024x768 (lop nen) ; du an 800x600
=> Anh nen chi duoc ve trong 800x600 nen ben phai (800..1024) va duoi (600..768) con den.

Sua (khong dung toi du lieu): voi lop NEN, neu khung ve trong ini nho hon do phan giai that thi noi ra cho
bang man hinh - dung y ban 2.0 da lam trong tep ini moi cua ho. Do phan giai doc tu config.ini nhu game van doc.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[ANHNEN 10/09 i]"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/KScenePlaceC.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)


def rep(old, new, ten, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FAIL neo %s: %d (mong %d)" % (ten, c, n)); sys.exit(1)
    s = s.replace(old, new)


# 1. dau ham VeLopNen: tinh khung ve that
rep("void KScenePlaceC::VeLopNen(KLopCanh* p)" + NL + "{" + NL,
    NL.join([
        "void KScenePlaceC::VeLopNen(KLopCanh* p)",
        "{",
        T + "// " + TAG + " tep _Scroll.ini cua du an con la ban 800x600 (2.0 da sua thanh 1024x768).",
        T + "// Voi lop NEN: neu khung ve trong ini nho hon do phan giai that thi noi ra cho bang man hinh,",
        T + "// khong thi ben phai va phia duoi con vien den.",
        T + "static int s_nManRong = 0, s_nManCao = 0;",
        T + "if (s_nManRong <= 0)",
        T + "{",
        T*2 + "s_nManRong = GetPrivateProfileIntA(\"Resolution\", \"Width\",  1024, \".\\\\config.ini\");",
        T*2 + "s_nManCao  = GetPrivateProfileIntA(\"Resolution\", \"Height\", 768,  \".\\\\config.ini\");",
        T*2 + "if (s_nManRong <= 0) s_nManRong = 1024;",
        T*2 + "if (s_nManCao <= 0) s_nManCao = 768;",
        T + "}",
        T + "RECT rcVe = p->rcMan;",
        T + "if (rcVe.right - rcVe.left < s_nManRong) rcVe.right = rcVe.left + s_nManRong;",
        T + "if (rcVe.bottom - rcVe.top < s_nManCao)  rcVe.bottom = rcVe.top + s_nManCao;",
        ""]), "dau ham")

# 2. thay moi cho dung p->rcMan trong VeLopNen bang rcVe
i = s.find("void KScenePlaceC::VeLopNen(KLopCanh* p)")
j = s.find(NL + "}" + NL, s.find("g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);", i))
than = s[i:j]
n = than.count("p->rcMan")
if n < 8:
    print("FAIL: chi thay %d cho p->rcMan trong VeLopNen" % n); sys.exit(1)
s = s[:i] + than.replace("p->rcMan", "rcVe") + s[j:]
print("da doi %d cho p->rcMan -> rcVe" % n)

if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KScenePlaceC.cpp " + TAG)
