# -*- coding: utf-8 -*-
r"""[25/08] Va loi [C]: chu TCVN3 (byte >0x80) dung NGAY TRUOC '<' -> TEncodeText
(Text.cpp:468) nuot '<' lam vo the ("<co lor>"). Chen 1 SPACE truoc '<' - chi trong
CHUOI literal "..." cua cac file cum Tin Su, khong dung code."""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
FILES = [
    r"task\tollgate\messenger\posthouse.lua",
    r"task\tollgate\messenger\messenger_turerukou.lua",
    r"task\tollgate\messenger\fengzhiqi\messenger_flynpc.lua",
    r"task\tollgate\messenger\qianbaoku\90\turebug90.lua",
    r"task\tollgate\messenger\qianbaoku\90\tureboss90.lua",
    r"task\tollgate\messenger\shanshenmiao\messenger_temnpc.lua",
    r"task\tollgate\messenger\qianbaoku\messenger_turenpc.lua",
    r"item\xinshirenwu\xinshibaoxiang.lua",
    r"item\xinshirenwu\wuxingfu.lua",
    r"item\xinshirenwu\qianbaokuling.lua",
]

def va_dong(b):
    """chen space truoc '<' neu byte truoc >0x80, CHI trong chuoi literal"""
    out = bytearray(); in_str = False; doi = 0
    i = 0
    while i < len(b):
        c = b[i]
        if c == 0x22:  # dau nhay kep
            in_str = not in_str
            out.append(c)
        elif in_str and c == 0x3C and out and out[-1] > 0x80:  # '<' sau byte dau
            out.append(0x20); out.append(c); doi += 1
        else:
            out.append(c)
        i += 1
    return bytes(out), doi

tong = 0
for rel in FILES:
    p = os.path.join(E, rel)
    if not os.path.isfile(p): continue
    raw = io.open(p, "rb").read()
    NL = b"\r\n" if b"\r\n" in raw else b"\n"
    out = []; doi = 0
    for l in raw.split(NL):
        code = l.split(b"--")[0]
        if b'"' in code and b"<" in code:
            l2, d = va_dong(l)
            out.append(l2); doi += d
        else:
            out.append(l)
    if doi:
        q = p + ".truoc_cspace_2508"
        if not os.path.isfile(q): shutil.copyfile(p, q)
        io.open(p, "wb").write(NL.join(out))
    print("%-56s chen %d space" % (rel, doi))
    tong += doi
print("TONG:", tong)
