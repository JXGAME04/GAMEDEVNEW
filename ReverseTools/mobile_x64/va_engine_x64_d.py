# -*- coding: utf-8 -*-
"""Vong 4 Engine x64: mo lai JPEG tren x64 (KJpgFile.cpp:14, KPakFile.cpp:121 '#ifndef _WIN64' -> '#if 1') vi JpgLib x64 (WIC) da co."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + r"\Sources\Engine\Src"
for f, ln in (("KJpgFile.cpp", 14), ("KPakFile.cpp", 121)):
    p = E + "\\" + f; b = io.open(p, "rb").read()
    if b"[X64 08/09] JpgLib x64" in b: print(f, "da va"); continue
    parts = b.split(b"\n")
    line = parts[ln - 1].rstrip(b"\r")
    assert line == b"#ifndef _WIN64", (f, line)
    cr = b"\r" if parts[ln - 1].endswith(b"\r") else b""
    parts[ln - 1] = b"#if 1	// [X64 08/09] JpgLib x64 (WIC) da co -> mo lai JPEG tren x64 (truoc la #ifndef _WIN64)" + cr
    nb = b"\n".join(parts)
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
    io.open(p, "wb").write(nb); print(f, "OK")
