# -*- coding: utf-8 -*-
"""[PHA 1 08/09 clang b] (1) KMemBase.h: overload inline g_MemCopy(LPVOID, LPCVOID, DWORD);
(2) MultiServer/Common/OpaqueUserData.h:43: GetUserPtr() lay dia chi cua rvalue (MSVC cho, clang khong) -> doc thang;
(3) WndWindow.h: KWndWindow::CloseWindow bo 'virtual' (khong lop nao override khong-static, khong cho nao goi ao; 123 lop dan xuat
    khai bao 'static void CloseWindow(bool)' che no - clang/GCC: static khong duoc trung ten+tham so voi ham ao lop cha).
Byte-safe, chay lai an toan."""
import io, re, sys, glob
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
if b"g_MemCopy(LPVOID lpDest, LPCVOID lpSrc" not in b:
    m = re.search(rb"(ENGINE_API[ \t]+void[ \t]+g_MemCopy\(LPVOID lpDest, LPVOID lpSrc, DWORD dwLen\);[ \t]*\r?\n)", b); assert m
    ins = b"inline void g_MemCopy(LPVOID lpDest, LPCVOID lpSrc, DWORD dwLen) { g_MemCopy(lpDest, (LPVOID)lpSrc, dwLen); }\t// " + TAG + b" nguon const (clang); overload inline, khong doi ham xuat" + nlof(b)
    wr(p, b, b[:m.end()] + ins + b[m.end():]); print("1 KMemBase.h g_MemCopy OK")
else: print("1 da va")

# 2
p = S + "MultiServer\\Common\\OpaqueUserData.h"; b = rd(p)
if TAG not in b:
    m = re.search(rb"([ \t]+)return InterlockedExchangePointer\( &\(const_cast<void \*>\(m_pUserData\)\), m_pUserData \);", b); assert m, "khong thay GetUserPtr"
    ind = m.group(1)
    new = ind + b"return const_cast<void *>(m_pUserData);\t// " + TAG + b" truoc: InterlockedExchangePointer(&(const_cast<>(x)), x) = doc x qua dia chi cua rvalue (MSVC cho, clang khong); doc con tro can le la nguyen tu"
    wr(p, b, b[:m.start()] + new + b[m.end():]); print("2 OpaqueUserData.h OK")
else: print("2 da va")

# 3 - kiem truoc: khong co override khong-static, khong co goi ao qua con tro lop cha trong Elem/*.cpp
bad = []
for f in glob.glob(S + "S3Client\\Ui\\Elem\\*.cpp") + glob.glob(S + "S3Client\\Ui\\Elem\\*.h"):
    t = rd(f)
    for mm in re.finditer(rb"(?<![:>.\w])CloseWindow\s*\(", t):
        line = t[t.rfind(b"\n", 0, mm.start()) + 1: t.find(b"\n", mm.end())]
        if re.search(rb"^\s*(virtual\s+)?(int|void|static)\b", line): continue   # khai bao/dinh nghia
        if line.strip().startswith(b"//"): continue
        bad.append((f.split("\\")[-1], line.strip()[:80]))
print("3 goi CloseWindow khong dinh danh trong Elem:", bad[:5])
p = S + "S3Client\\Ui\\Elem\\WndWindow.h"; b = rd(p)
if TAG not in b:
    if bad:
        print("3 BO QUA (co cho goi can xem)")
    else:
        m = re.search(rb"virtual int([ \t]+)CloseWindow\(bool bDestory\)\{return 0;\};", b); assert m, "khong thay khai bao CloseWindow"
        new = b"int" + m.group(1) + b"\tCloseWindow(bool bDestory){return 0;};\t// " + TAG + b" bo virtual: khong lop nao override khong-static, khong goi ao; 123 lop KUi* khai bao static CloseWindow(bool) che no (clang/GCC cam static trung ham ao lop cha)"
        wr(p, b, b[:m.start()] + new + b[m.end():]); print("3 WndWindow.h OK")
else: print("3 da va")
print("XONG")
