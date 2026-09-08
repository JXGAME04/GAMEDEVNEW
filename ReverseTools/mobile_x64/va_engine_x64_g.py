# -*- coding: utf-8 -*-
"""Vong 7 Engine x64: KMp3Music.cpp - mp3lib.lib chi co ban x86 (khong co nguon) -> tren _WIN64 stub 4 ham mp3_decode_* tra that bai
(Mp3Init that bai em, khong phat mp3, khong sap). Win32 khong doi."""
import io, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\Engine\Src\KMp3Music.cpp"
b = io.open(p, "rb").read()
if b"[X64 08/09] mp3lib stub" in b: print("KMp3Music.cpp stub: da va"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
anchor = b'#include "mp3lib.h"'
assert b.count(anchor) == 1
stub = nl.join([anchor,
    b"#ifdef _WIN64",
    b"// [X64 08/09] mp3lib stub: mp3lib.lib chi co ban x86 va khong co ma nguon -> tren x64 chua co bo giai ma MP3.",
    b"// Bon ham tra that bai de KMp3Music::Mp3Init that bai em (khong phat nhac mp3), moi thu khac cua Engine van chay.",
    b"extern \"C\" {",
    b"int    mp3_decode_head(unsigned char* buf, MPEG_HEAD* head) { (void)buf; (void)head; return 0; }",
    b"int    mp3_decode_init(MPEG_HEAD* head, int framebytes_arg, int reduction_code, int transform_code, int convert_code, int freq_limit)",
    b"       { (void)head; (void)framebytes_arg; (void)reduction_code; (void)transform_code; (void)convert_code; (void)freq_limit; return 0; }",
    b"void   mp3_decode_info(DEC_INFO* info) { if (info) { info->channels = 0; info->outvalues = 0; info->samprate = 0; info->bits = 0; info->framebytes = 0; } }",
    b"IN_OUT mp3_decode_frame(unsigned char* mp3, unsigned char* pcm) { IN_OUT r; (void)mp3; (void)pcm; r.in_bytes = 0; r.out_bytes = 0; return r; }",
    b"}",
    b"#endif"])
nb = b.replace(anchor, stub, 1)
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb); print("KMp3Music.cpp stub: OK")
