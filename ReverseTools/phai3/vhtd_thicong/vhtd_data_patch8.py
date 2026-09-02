# -*- coding: utf-8 -*-
"""vhtd_data_patch8.py [VHTD 02/09k] - dot 7 du lieu:
 D1 missles.txt (server+client): dan 426 (Phach Thach Pha Ngoc 1382/1383, Hoa Son) lay DUNG bang client VLTK theo ten cot
    (dot 1 lay Linux: CollidRange 1, LifeTime 15, Speed 25, ColVanish 0 -> VLTK 3 / 24 / 30 / 1). Cac dan Hoa Son khac (418-428) da trung.
    23 dan JX1 co dien (34 57 94 284 291.. 413) VLTK cung doi -> KHONG dong (can bang cu), liet ke trong bangiao cho chu quyet.
 D2 ui\\Ui3\\UiHeaderControlBar.ini (client): go 'Button6=Shield' (2 muc [Main]/[Main1024]) - thanh khien chuyen xuong thanh mau duoi ten
    (engine patch7 KNpc::PaintLife). Cac muc [Shield*] giu lai (khong dung).
Doc/ghi latin-1, giu NL, idempotent. DUNG: python vhtd_data_patch8.py [--kiem]
"""
import io, os, re, sys, shutil

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw\settings__missles.txt"
BAK = ".truoc_vhtd_patch8_0209"
IDS = [426]

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s, tag):
    if "\xef\xbf\xbd" in s: raise SystemExit("EF BF BD " + p)
    if not KIEM:
        if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
        io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  => %s %s (%s)" % ("KIEM" if KIEM else "ghi", p, tag))

def d1_missles():
    vs = rd(VLTK); vl = vs.split("\r\n") if "\r\n" in vs else vs.split("\n"); vh = vl[0].split("\t")
    vrows = {}
    for l in vl[1:]:
        c = l.split("\t")
        if c and c[0].strip().isdigit(): vrows[int(c[0])] = dict(zip(vh, c))
    for side in ("server", "client"):
        p = os.path.join(BIN, side, "settings", "missles.txt")
        s = rd(p); nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
        lines = s.split(nl); hdr = lines[0].split("\t"); n = 0
        for i in range(1, len(lines)):
            c = lines[i].split("\t")
            if not c or not c[0].strip().isdigit() or int(c[0]) not in IDS: continue
            v = vrows[int(c[0])]; ch = []
            for j, h in enumerate(hdr):
                if h == "MissleName": continue   # giu ten Viet (VLTK la ten Trung)
                if h in v and j < len(c) and c[j] != v[h]:
                    ch.append("%s %s->%s" % (h, c[j] or "-", v[h] or "-")); c[j] = v[h]
            if ch:
                n += 1; lines[i] = "\t".join(c); print("  [+] missle %s: %s" % (c[0], "; ".join(ch)))
        if n: wr(p, nl.join(lines), "missles.txt %d dong" % n)
        else: print("  [=] %s missles.txt da khop" % side)

def d2_header():
    p = os.path.join(BIN, "client", "ui", "Ui3", "UiHeaderControlBar.ini")
    s = rd(p); nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    lines = s.split(nl); n = 0; out = []
    for l in lines:
        if l.strip() == "Button6=Shield":
            n += 1; continue
        out.append(l)
    if n: wr(p, nl.join(out), "go %d dong Button6=Shield" % n)
    else: print("  [=] UiHeaderControlBar.ini khong con Button6=Shield")

if __name__ == "__main__":
    print("vhtd_data_patch8 [VHTD 02/09k]%s" % (" (KIEM)" if KIEM else ""))
    d1_missles(); d2_header()
    print("XONG.")
