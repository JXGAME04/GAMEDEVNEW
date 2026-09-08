# -*- coding: utf-8 -*-
"""Bon mieng va nguon cho ban x64 (byte-safe, giu nguyen kieu xuong dong TUNG DONG, chi chen ASCII):
 1. Engine/Src/KWin32.h      : typedef KUPARAM/KNPARAM (Win32 = unsigned int/int -> nhi phan khong doi)
 2. S3Client/Ui/FilterTextLib.h : pragma lib theo _WIN64 -> Lib/release64
 3. Represent/iRepresent/Font/KFontRes.cpp : KFontRes::Update() ban C cho _WIN64, giu __asm cho Win32
 4. Represent/Represent3/TextureRes.cpp   : RenderToA4R4G4B4 (asm, KHONG duoc goi) bo qua khi _WIN64
Moi mieng va deu kiem tra neo truoc khi ghi; chay lai lan 2 se bao 'da va'.
"""
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"

def rd(p):
    return io.open(p, "rb").read()

def wr(p, b_old, b_new):
    assert sum(1 for c in b_old if c > 127) == sum(1 for c in b_new if c > 127), "byte cao doi: " + p
    io.open(p, "wb").write(b_new)

def split_lines(b):
    """tra ve danh sach (noi_dung_khong_co_CR, duoi) voi duoi la b'\\r\\n' / b'\\n' / b'' (dong cuoi khong xuong dong)"""
    out = []
    parts = b.split(b"\n")
    for i, part in enumerate(parts):
        last = (i == len(parts) - 1)
        if last:
            out.append((part, b""))
        elif part.endswith(b"\r"):
            out.append((part[:-1], b"\r\n"))
        else:
            out.append((part, b"\n"))
    return out

def join_lines(ls):
    return b"".join(c + e for c, e in ls)

def find_line(ls, start, pred):
    for i in range(start, len(ls)):
        if pred(ls[i][0]): return i
    raise KeyError("khong thay neo")

# ---------- 1. KWin32.h
p = os.path.join(ROOT, "Sources", "Engine", "Src", "KWin32.h")
b = rd(p)
if b"KUPARAM" in b:
    print("1 KWin32.h: da va")
else:
    ls = split_lines(b)
    i = find_line(ls, 0, lambda l: l.startswith(b"#define WIN32_LEAN_AND_MEAN"))
    e = ls[i][1] or b"\r\n"
    block = [
        b"// [X64 08/09] Tham so mang con tro cua he cua so KWnd (WndProc uParam/nParam) va CoreDataChanged:",
        b"// Win32 giu nguyen unsigned int / int (kieu va ten xao tron y het -> nhi phan khong doi),",
        b"// x64 rong bang con tro de khong cat cut dia chi. Dung KUPARAM/KNPARAM thay cho (unsigned int)/(int) khi ep con tro.",
        b"#include <stdint.h>",
        b"typedef uintptr_t KUPARAM;",
        b"typedef intptr_t  KNPARAM;",
        b"",
    ]
    ls = ls[:i + 1] + [(x, e) for x in block] + ls[i + 1:]
    wr(p, b, join_lines(ls)); print("1 KWin32.h: OK")

# ---------- 2. FilterTextLib.h
p = os.path.join(ROOT, "Sources", "S3Client", "Ui", "FilterTextLib.h")
b = rd(p)
if b"release64/FilterText_StaticLib.lib" in b:
    print("2 FilterTextLib.h: da va")
else:
    ls = split_lines(b)
    i = find_line(ls, 0, lambda l: l == b"#ifdef _DEBUG")
    assert ls[i + 1][0] == b'#pragma comment(lib, "../../Lib/Debug/FilterText_StaticLib.lib")' and ls[i + 2][0] == b"#else", ls[i + 1][0]
    e = ls[i][1] or b"\r\n"
    ls = ls[:i] + [(b"#if defined(_WIN64)", e), (b'#pragma comment(lib, "../../Lib/release64/FilterText_StaticLib.lib")	// [X64 08/09]', e), (b"#elif defined(_DEBUG)", e)] + ls[i + 1:]
    wr(p, b, join_lines(ls)); print("2 FilterTextLib.h: OK")

# ---------- 3. KFontRes.cpp
p = os.path.join(ROOT, "Sources", "Represent", "iRepresent", "Font", "KFontRes.cpp")
b = rd(p)
if b"[X64 08/09] KFontRes::Update" in b:
    print("3 KFontRes.cpp: da va")
else:
    ls = split_lines(b)
    s = find_line(ls, 0, lambda l: l.startswith(b"void KFontRes::Update(unsigned char* pCharImage"))
    e = find_line(ls, s, lambda l: l == b"}")
    a = find_line(ls, s, lambda l: l == b"\t__asm")
    c = find_line(ls, a + 1, lambda l: l == b"\t}")   # dau '}' dong khoi __asm (sau do con lenh khac truoc khi ham ket thuc)
    assert a < c < e and ls[a + 1][0] == b"\t{", (ls[a + 1][0], ls[c][0])
    e = c + 1
    nl = ls[a][1] or b"\n"
    cblock = [
        b"#ifdef _WIN64",
        b"\t// [X64 08/09] KFontRes::Update ban C (x64 khong co __asm), cung ngu nghia voi khoi hop ngu ben duoi:",
        b"\t// moi byte RLE = (alpha 3 bit << 5) | so diem (5 bit); alpha 7 -> 0xFFFF, co vien: alpha 1..6 -> 0xEFFF, alpha 0 -> 0;",
        b"\t// khong vien: chi alpha 7 -> 0xFFFF, con lai 0. Moi dong ghi nWidth diem roi nhay TextureOffset byte.",
        b"\t{",
        b"\t\tunsigned char*  s = pCharImage;",
        b"\t\tunsigned short* d = (unsigned short*)pTexData;",
        b"\t\tfor (int y = 0; y < nHeight; y++)",
        b"\t\t{",
        b"\t\t\tint nRemain = nWidth;",
        b"\t\t\twhile (nRemain > 0)",
        b"\t\t\t{",
        b"\t\t\t\tunsigned int  uRle   = *s++;",
        b"\t\t\t\tint           nCount = (int)(uRle & 0x1f);",
        b"\t\t\t\tunsigned int  uAlpha = uRle >> 5;",
        b"\t\t\t\tunsigned short wPix;",
        b"\t\t\t\tif (bEnableTextBorder)",
        b"\t\t\t\t\twPix = (uAlpha == 0) ? 0 : ((uAlpha == 7) ? 0xffff : 0xefff);",
        b"\t\t\t\telse",
        b"\t\t\t\t\twPix = (uAlpha == 7) ? 0xffff : 0;",
        b"\t\t\t\tfor (int k = 0; k < nCount; k++)",
        b"\t\t\t\t\t*d++ = wPix;",
        b"\t\t\t\tnRemain -= nCount;",
        b"\t\t\t}",
        b"\t\t\td = (unsigned short*)((unsigned char*)d + TextureOffset);",
        b"\t\t}",
        b"\t}",
        b"#else",
    ]
    ls = ls[:a] + [(x, nl) for x in cblock] + ls[a:e] + [(b"#endif", nl)] + ls[e:]
    wr(p, b, join_lines(ls)); print("3 KFontRes.cpp: OK (chen %d dong)" % (len(cblock) + 1))

# ---------- 4. TextureRes.cpp
p = os.path.join(ROOT, "Sources", "Represent", "Represent3", "TextureRes.cpp")
b = rd(p)
if b"[X64 08/09] RenderToA4R4G4B4" in b:
    print("4 TextureRes.cpp: da va")
else:
    ls = split_lines(b)
    s = find_line(ls, 0, lambda l: l.startswith(b"inline void RenderToA4R4G4B4("))
    e = find_line(ls, s, lambda l: l == b"}")
    assert any(l.startswith(b"loc_DrawSprite_exit:") for l, _ in ls[s:e])
    nl = ls[s][1] or b"\n"
    ls = ls[:s] + [(b"#ifndef _WIN64	// [X64 08/09] RenderToA4R4G4B4 la hop ngu x86 va KHONG con duoc goi (dung RenderToA4R4G4B4Safe); x64 bo qua", nl)] + ls[s:e + 1] + [(b"#endif", nl)] + ls[e + 1:]
    wr(p, b, join_lines(ls)); print("4 TextureRes.cpp: OK")
