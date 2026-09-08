# -*- coding: utf-8 -*-
"""Vong 8 Engine x64: KMp3Music::Mp3Init tren x64 tra FALSE ngay dau ham (ghi log mot lan), khong toi g_MessageBox
'mp3 decode head fail' (chu gap hop thoai nay khi chay Game.exe x64 12:2x)."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Engine\Src\KMp3Music.cpp"
b = io.open(p, "rb").read()
if b"[X64 08/09] Mp3Init" in b: print("KMp3Music.cpp Mp3Init: da va"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
anchor = b"BOOL KMp3Music::Mp3Init()" + nl + b"{"
assert b.count(anchor) == 1, b.count(anchor)
new = anchor + nl + nl.join([
    b"#ifdef _WIN64",
    b"\t// [X64 08/09] Mp3Init: x64 chua co bo giai ma mp3 (mp3lib.lib chi x86) -> that bai EM, ghi log mot lan,",
    b"\t// khong bat g_MessageBox 'mp3 decode head fail' nhu duong cu; game chay khong nhac mp3.",
    b"\tstatic BOOL s_bDaBaoX64 = FALSE;",
    b"\tif (!s_bDaBaoX64)",
    b"\t{",
    b"\t\tg_DebugLog(\"KMp3Music: ban x64 chua co mp3lib, bo qua nhac mp3\");",
    b"\t\ts_bDaBaoX64 = TRUE;",
    b"\t}",
    b"\treturn FALSE;",
    b"#endif"])
nb = b.replace(anchor, new, 1)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("KMp3Music.cpp Mp3Init: OK")
