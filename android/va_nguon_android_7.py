# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 7 - LP64: struct TREN DIA / GOI MANG co thanh vien 'long' / 'unsigned long' (Windows 4 byte, Android 8 byte)
-> doi sang JX_LONG / JX_ULONG (typedef: Windows = long / unsigned long y nguyen -> nhi phan va kieu KHONG doi; JX_POSIX = int / unsigned int).
Khoi typedef (co guard JX_LP64_TYPES) chen vao dau moi header duoc va. Chi doi THANH VIEN struct, khong doi chu ky ham.
Byte-an-toan, idempotent. Danh sach: XPackFile.cpp/.h, ZSPRPackFile.h, KProtocol.h (4 truong), GameDataDef.h (KLightInfo), KRegion.h (m_Obstacle).
"""
import io, os, re, sys
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
S = os.path.join(ROOT, "Sources")
def doc(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def ghi(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def cao(s): return sum(1 for c in s if ord(c) >= 128)
def eol(s): return "\r\n" if "\r\n" in s else "\n"
TYPEDEF = ["#ifndef JX_LP64_TYPES\t// [ANDROID 08/09] long tren dia/goi: Windows 4 byte (giu nguyen long), Android LP64 phai la 32 bit",
           "#define JX_LP64_TYPES", "#if defined(JX_POSIX)", "typedef int JX_LONG; typedef unsigned int JX_ULONG;",
           "#else", "typedef long JX_LONG; typedef unsigned long JX_ULONG;", "#endif", "#endif"]

def chen_typedef(s, sau=None):
    """chen khoi typedef sau dong 'sau' (neu co) hoac sau #define guard dau tien / #pragma once / dong include dau"""
    E = eol(s)
    if "JX_LP64_TYPES" in s: return s
    blk = E.join(TYPEDEF) + E
    if sau and sau in s:
        i = s.find(sau); j = s.find(E, i) + len(E); return s[:j] + blk + s[j:]
    m = re.search(r"^#define\s+\w+\s*\r?\n", s, re.M)   # guard
    if m and s[:m.start()].count("#ifndef") >= 1:
        return s[:m.end()] + blk + s[m.end():]
    m = re.search(r"^#pragma once[^\r\n]*\r?\n", s, re.M)
    if m: return s[:m.end()] + blk + s[m.end():]
    m = re.search(r"^#include[^\r\n]*\r?\n", s, re.M)
    return s[:m.end()] + blk + s[m.end():] if m else blk + s

def doi_thanh_vien(s, dong_ke):
    """dong_ke: danh sach (regex dong) - doi 'unsigned long' -> JX_ULONG, 'long' -> JX_LONG o nhung dong khop"""
    n = 0
    lines = s.split("\n"); out = []
    for l in lines:
        l2 = l
        if any(re.search(p, l) for p in dong_ke):
            l2 = re.sub(r"\bunsigned\s+long\b", "JX_ULONG", l2)
            l2 = re.sub(r"(?<![\w])long\b(?!\s+long)", "JX_LONG", l2)
            if l2 != l: n += 1
        out.append(l2)
    return "\n".join(out), n

def sua(rel, fn):
    p = os.path.join(S, rel); s = doc(p); n0 = cao(s)
    s2, msg = fn(s)
    if s2 == s: print("  =", rel, "(khong doi)", msg); return
    assert cao(s2) == n0, "byte cao doi o " + rel
    ghi(p, s2); print("  +", rel, msg)

# XPackFile.h : uId (dong 20), uId/lSize (81-82); .cpp: XPackFileHeader (uCount..uCrc32), XPackIndexInfo (uId,uOffset,lSize,lCompressSizeFlag), 73-74 (lCompressSize,lSize)
def xph(s):
    s = chen_typedef(s)
    s, n = doi_thanh_vien(s, [r"^\s*unsigned long\s+uId;", r"^\s*long\s+lSize;"])
    return s, "%d thanh vien" % n
sua("Engine/Src/XPackFile.h", xph)
def xpc(s):
    s, n = doi_thanh_vien(s, [r"^\s*unsigned long\s+(uCount|uIndexTableOffset|uDataOffset|uCrc32|uId|uOffset);", r"^\s*long\s+(lSize|lCompressSizeFlag|lCompressSize);"])
    return s, "%d thanh vien" % n
sua("Engine/Src/XPackFile.cpp", xpc)

# ZSPRPackFile.h : item_info, ZCache (cache_size, last_items), m_Size, index_info, z_pack_header, frame_info, frame_index_info
def zspr(s):
    s = chen_typedef(s, sau="#include <windows.h>")
    s, n = doi_thanh_vien(s, [r"^\s*unsigned long\s+(index_high|index_low|last_items\[|m_Size|id|offset|count|index_offset|data_offset|crc32)\b",
                              r"^\s*long\s+(offset|size|cache_size|compress_size);"])
    return s, "%d thanh vien" % n
sua("Engine/Src/ZSPRPackFile.h", zspr)

# KProtocol.h : 4 truong goi
def proto(s):
    s = chen_typedef(s)
    s, n = doi_thanh_vien(s, [r"^\s*unsigned long\s+(lnID|ulIdentity);", r"^\s*long\s+pValue;"])
    return s, "%d thanh vien" % n
sua("Core/Src/KProtocol.h", proto)

# GameDataDef.h : KLightInfo.nRadius
def gdd(s):
    s = chen_typedef(s)
    s, n = doi_thanh_vien(s, [r"^\s*long\s+nRadius;"])
    return s, "%d thanh vien" % n
sua("Core/Src/GameDataDef.h", gdd)

# KRegion.h : m_Obstacle[][] doc thang tu tep map (cObstacle.Read(m_Obstacle, sizeof))
def region(s):
    s = chen_typedef(s)
    s, n = doi_thanh_vien(s, [r"^\s*long\s+m_Obstacle\["])
    return s, "%d thanh vien" % n
sua("Core/Src/KRegion.h", region)
print("xong dot 7")
