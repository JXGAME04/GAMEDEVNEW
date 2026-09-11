# -*- coding: utf-8 -*-
r"""[VE 11/09 f] SUA LOI cua [VE 11/09 d] (chu bao 10:55 tren ban 109111021: spr luc dang nhap luc hien luc khong, map loi, dau/duoi ngua an hien):
vung 0 (to trang atlas moi / o chua co ban CPU) duoc ghi lenh SAU cac lenh tai noi dung texture trong cung copy pass, ma trang moi va noi dung
cua no nam trong CUNG mot khung -> lenh to 0 ghi DE len anh vua tai (lenh copy chay theo thu tu) -> texture trong (bo ban CPU roi nen khong
tai lai) cho toi khi cache bo va nap lai. Truoc d, vung 0 nam chung hang m_texUploads nen luon di truoc noi dung. Sua: chuyen khoi vung 0
len DAU copy pass (truoc bang mau + texture). Chi doi cho, khong doi noi dung.
Doc/ghi latin-1 (CRLF), moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_ve4.py   (sau ve1, ve2, ve3)
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VE 11/09 f]"
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")

p = os.path.join(R3, "D3D9onGPUDev.cpp")
s = io.open(p, encoding="latin-1", newline="").read()
if s.count("\r\n") != s.count("\n"):
    raise SystemExit("tep co xuong dong lan lon, khong va")
cao = sum(1 for c in s if ord(c) >= 0x80)
s = s.replace("\r\n", "\n")
if "[VE 11/09 e]" not in s:
    raise SystemExit("chua ap ve3")
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    # 1. cat khoi vung 0 (tu 'if (!m_jxZeroUploads.empty())' den 'jxK.dChepZero = ...; }')
    rx = re.compile(r"\t\tif \(!m_jxZeroUploads\.empty\(\)\)\n\t\t\{.*?\t\t\tjxK\.dChepZero = JxVeMs\(uZ0, SDL_GetPerformanceCounter\(\)\);\n\t\t\}\n", re.S)
    m = list(rx.finditer(s))
    if len(m) != 1:
        raise SystemExit("khong tim thay dung 1 khoi vung 0 (%d)" % len(m))
    khoi = m[0].group(0)
    s = s[:m[0].start()] + s[m[0].end():]
    # 2. chen len dau copy pass cua SubmitFrame
    moc = "\t// ---- copy pass: bang mau, texture, ring dinh\n\t{\n\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);\n"
    if s.count(moc) != 1:
        raise SystemExit("khong tim thay dung 1 moc dau copy pass (%d)" % s.count(moc))
    them = ("#ifdef JX_ANDROID\n"
            "\t\t// " + DAU + " vung 0 (trang atlas moi / o chua co ban CPU) PHAI ghi lenh TRUOC noi dung texture: trang moi va anh dau tien tren no\n"
            "\t\t// nam trong cung khung; [VE 11/09 d] tung dat khoi nay SAU tex -> lenh to 0 ghi de anh vua tai (chu thay spr an hien, map loi, ngua mat dau duoi).\n"
            + khoi +
            "#endif\n")
    s = s.replace(moc, moc + them)
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi")
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", "\r\n"))
    print("da va:", os.path.relpath(p, GOC))
print("xong")
