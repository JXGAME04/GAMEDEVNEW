# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Dot 8 - LP64 tiep (theo bao cao so_layout_lp64.py): KScenePlaceRegionC.h m_ObstacleInfo/m_TrapInfo (doc thang tu tep map),
LoginDef.h KLoginAccountInfo.nLeftTime (goi dang nhap s2c_login) -> JX_LONG / JX_ULONG. Byte-an-toan, idempotent.
"""
import io, os, re
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
S = os.path.join(ROOT, "Sources")
def doc(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def ghi(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def cao(s): return sum(1 for c in s if ord(c) >= 128)
def eol(s): return "\r\n" if "\r\n" in s else "\n"
TYPEDEF = ["#ifndef JX_LP64_TYPES\t// [ANDROID 08/09] long tren dia/goi: Windows 4 byte (giu nguyen long), Android LP64 phai la 32 bit",
           "#define JX_LP64_TYPES", "#if defined(JX_POSIX)", "typedef int JX_LONG; typedef unsigned int JX_ULONG;",
           "#else", "typedef long JX_LONG; typedef unsigned long JX_ULONG;", "#endif", "#endif"]
def chen_typedef(s):
    E = eol(s)
    if "JX_LP64_TYPES" in s: return s
    blk = E.join(TYPEDEF) + E
    m = re.search(r"^#define\s+\w+\s*\r?\n", s, re.M)
    if m and s[:m.start()].count("#ifndef") >= 1: return s[:m.end()] + blk + s[m.end():]
    m = re.search(r"^#pragma once[^\r\n]*\r?\n", s, re.M)
    if m: return s[:m.end()] + blk + s[m.end():]
    m = re.search(r"^#include[^\r\n]*\r?\n", s, re.M)
    return s[:m.end()] + blk + s[m.end():] if m else blk + s
def doi_thanh_vien(s, dong_ke):
    n = 0; out = []
    for l in s.split("\n"):
        l2 = l
        if any(re.search(p, l) for p in dong_ke):
            l2 = re.sub(r"\bunsigned\s+long\b", "JX_ULONG", l2)
            l2 = re.sub(r"(?<![\w])long\b(?!\s+long)", "JX_LONG", l2)
            if l2 != l: n += 1
        out.append(l2)
    return "\n".join(out), n
def sua(rel, pats):
    p = os.path.join(S, rel); s = doc(p); n0 = cao(s)
    s2 = chen_typedef(s); s2, n = doi_thanh_vien(s2, pats)
    if s2 == s: print("  =", rel, "(khong doi)"); return
    assert cao(s2) == n0, "byte cao doi o " + rel
    ghi(p, s2); print("  +", rel, "%d thanh vien" % n)
sua("Core/Src/Scene/KScenePlaceRegionC.h", [r"^\s*long\s+m_(ObstacleInfo|TrapInfo)\["])
sua("S3Client/Login/LoginDef.h", [r"^\s*unsigned long\s+nLeftTime;"])
print("xong dot 8")
