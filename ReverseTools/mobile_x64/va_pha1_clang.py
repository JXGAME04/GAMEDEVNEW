# -*- coding: utf-8 -*-
"""[PHA 1 08/09] 9 sua nho cho clang (BANGIAO_PHA1_CLANG_0809.md muc 1), deu trung tinh voi MSVC, KHONG doi export cua Engine.dll:
 1 KMemBase.h: them overload inline g_MemComp(LPCVOID, LPCVOID, DWORD) (khong doi ham xuat)
 2 KPlayerChat.cpp: cFile.Write("..", n) -> (LPVOID)"..";  KItemGenerator.CPP: (LPSTR) cho tf.Load(?:)
 3 KNpcRes.cpp: ?: co ve void -> if/else
 6 KUiMsgCentrePad::CloseWindow (static, che virtual KWndWindow::CloseWindow) -> CloseWnd (h, cpp, 3 cho goi)
 7 WndObjContainer.cpp: bien 'or' (tu khoa thay the C++) -> rcOr, chi dong 820-890
 8 UiToaDo.cpp: "UserData\\UiToaDo.ini" (\\U la escape) -> "UserData\\\\UiToaDo.ini" trong 2 chuoi
10 AntiHack: hwnd > 0 -> != NULL (3 tep)
Byte-safe (chi doi ASCII), dem byte >127 khong doi, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + "\\Sources\\"
TAG = b"[CLANG 08/09]"
def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
def nlof(b): return b"\r\n" if b"\r\n" in b else b"\n"

# 1
p = S + "Engine\\Src\\KMemBase.h"; b = rd(p)
if TAG not in b:
    m = re.search(rb"(ENGINE_API[ \t]+BOOL[ \t]+g_MemComp\(LPVOID lpDest, LPVOID lpSrc, DWORD dwLen\);[ \t]*\r?\n)", b); assert m
    ins = b"inline BOOL g_MemComp(LPCVOID lpDest, LPCVOID lpSrc, DWORD dwLen) { return g_MemComp((LPVOID)lpDest, (LPVOID)lpSrc, dwLen); }\t// " + TAG + b" goi voi chuoi hang (const) - clang khong cho const->non-const; overload inline, khong doi ham xuat" + nlof(b)
    wr(p, b, b[:m.end()] + ins + b[m.end():]); print("1 KMemBase.h OK")
else: print("1 KMemBase.h da va")

# 2a KPlayerChat.cpp
p = S + "Core\\Src\\KPlayerChat.cpp"; b = rd(p)
if TAG not in b:
    nb, n = re.subn(rb'cFile\.Write\("([^"\r\n]*)", ', rb'cFile.Write((LPVOID)"\1", ', b)
    assert n >= 7, n
    nl = nlof(nb); i = nb.find(b"cFile.Write((LPVOID)"); j = nb.rfind(nl, 0, i) + len(nl)
    nb = nb[:j] + b"\t// " + TAG + b" (LPVOID) cho chuoi hang: KFile::Write(LPVOID,...) - clang khong cho const->non-const" + nl + nb[j:]
    wr(p, b, nb); print("2a KPlayerChat.cpp OK:", n)
else: print("2a da va")
# 2b KItemGenerator.CPP
p = S + "Core\\Src\\KItemGenerator.CPP"; b = rd(p)
if TAG not in b:
    a = b"tf.Load(bPlatina ? PLATINA_EQUIP_FILE : TABFILE_GOLDITEM_FULL_N)"; assert b.count(a) == 1
    wr(p, b, b.replace(a, b"tf.Load((LPSTR)(bPlatina ? PLATINA_EQUIP_FILE : TABFILE_GOLDITEM_FULL_N))\t/* " + TAG + b" LPSTR cho const char* */")); print("2b KItemGenerator.CPP OK")
else: print("2b da va")

# 3 KNpcRes.cpp
p = S + "Core\\Src\\KNpcRes.cpp"; b = rd(p)
if TAG not in b:
    m = re.search(rb"([ \t]+)m_cNpcEffectImage\[i\]\.m_nTotalDir > 1 \? m_cNpcEffectImage\[i\]\.SetCurFrame\(nCurFrameNo\) : m_cNpcEffectImage\[i\]\.GetNextFrame\(\);", b); assert m
    ind = m.group(1); nl = nlof(b)
    new = (ind + b"if (m_cNpcEffectImage[i].m_nTotalDir > 1)\t// " + TAG + b" ?: co ve void (clang loi) -> if/else" + nl +
           ind + b"\tm_cNpcEffectImage[i].SetCurFrame(nCurFrameNo);" + nl + ind + b"else" + nl + ind + b"\tm_cNpcEffectImage[i].GetNextFrame();")
    wr(p, b, b[:m.start()] + new + b[m.end():]); print("3 KNpcRes.cpp OK")
else: print("3 da va")

# 6 CloseWindow -> CloseWnd
p = S + "S3Client\\Ui\\UiCase\\UiMsgCentrePad.h"; b = rd(p)
if b"CloseWnd(bool bDestroy)" not in b:
    a = b"CloseWindow(bool bDestroy);"; assert b.count(a) == 1
    wr(p, b, b.replace(a, b"CloseWnd(bool bDestroy);\t// " + TAG + b" doi ten: ham static che virtual KWndWindow::CloseWindow (clang loi)")); print("6 UiMsgCentrePad.h OK")
for f in ("S3Client\\Ui\\UiCase\\UiMsgCentrePad.cpp", "S3Client\\Ui\\UiCase\\UiConnectInfo.cpp", "S3Client\\Ui\\UiCase\\UiNewPlayer2.cpp", "S3Client\\Ui\\UiShell.cpp"):
    p = S + f; b = rd(p); a = b"KUiMsgCentrePad::CloseWindow("
    n = b.count(a)
    if n: wr(p, b, b.replace(a, b"KUiMsgCentrePad::CloseWnd(")); print("6", f, "doi", n)
    else: print("6", f, "khong co / da doi")

# 7 'or' -> rcOr (dong 820-890)
p = S + "S3Client\\Ui\\Elem\\WndObjContainer.cpp"; b = rd(p)
if b"rcOr" not in b:
    nl = nlof(b); L = b.split(nl); n = 0
    for i in range(819, min(890, len(L))):
        L2, k = re.subn(rb"\bor\b", b"rcOr", L[i]); n += k; L[i] = L2
    assert n >= 20, n
    i = next(i for i in range(819, 890) if b"RECT" in L[i] and b"rcOr" in L[i]); L[i] += b"\t// " + TAG + b" 'or' la tu khoa thay the C++ (clang), doi ten"
    wr(p, b, nl.join(L)); print("7 WndObjContainer.cpp OK:", n)
else: print("7 da va")

# 8 UiToaDo.cpp
p = S + "S3Client\\Ui\\Elem\\UiToaDo.cpp"; b = rd(p)
a = b'UserData\\UiToaDo.ini"'
n = b.count(a)
if n: wr(p, b, b.replace(a, b'UserData\\\\UiToaDo.ini"')); print("8 UiToaDo.cpp OK:", n)
else: print("8 da va")

# 10 AntiHack
for f, ln in (("S3Client\\AntiHack\\DetectHide\\DetectHide.cpp", 25), ("S3Client\\AntiHack\\DetectWindowsClassName\\DetectWindowsClassName.cpp", 21), ("S3Client\\AntiHack\\DetectWindowsTitle\\DetectWindowsTitle.cpp", 20)):
    p = S + f; b = rd(p); nl = nlof(b); L = b.split(nl)
    l = L[ln - 1]
    if b"!= NULL" in l: print("10", f, "da va"); continue
    l2, k = re.subn(rb"(\w+)\s*>\s*0\s*\)", rb"\1 != NULL)", l)
    assert k == 1, (f, l)
    L[ln - 1] = l2 + b"\t// " + TAG + b" so sanh con tro voi 0 bang > (clang loi)"
    wr(p, b, nl.join(L)); print("10", f, "OK")
print("XONG")
