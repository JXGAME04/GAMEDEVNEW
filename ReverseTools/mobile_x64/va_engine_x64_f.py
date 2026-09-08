# -*- coding: utf-8 -*-
"""Vong 6 Engine x64: mo lai KMp3Music tren x64 (chi LoadLibrary/GetProcAddress mp3lib.dll, khong can lib tinh); S3Client::KMyApp co thanh vien KMp3Music."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Engine\Src\KMp3Music.cpp"
b = io.open(p, "rb").read()
if b"[X64 08/09] KMp3Music" in b: print("KMp3Music.cpp: da va"); sys.exit(0)
parts = b.split(b"\n"); ln = 13
line = parts[ln - 1].rstrip(b"\r"); assert line == b"#ifndef _WIN64", line
cr = b"\r" if parts[ln - 1].endswith(b"\r") else b""
parts[ln - 1] = b"#if 1	// [X64 08/09] KMp3Music mo lai tren x64: chi nap mp3lib.dll dong qua GetProcAddress (truoc la #ifndef _WIN64)" + cr
nb = b"\n".join(parts)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("KMp3Music.cpp: OK")
