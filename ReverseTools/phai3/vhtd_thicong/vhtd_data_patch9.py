# -*- coding: utf-8 -*-
"""vhtd_data_patch9.py [VHTD 02/09r] - dong bo NOT bang thuoc tinh cua 3 ky nang Hoa Son con lech VLTK (dot 6 chi dong bo 13 hang):
 1349 Kiem Tong Tong Quyet : thieu attackspeed_yan_v, addcolddamage_v, anti_hitrecover; lifemax_p -> lifemax_yan_p
 1358 Huyen Nhan Van Yen   : thieu me2firedamage_p, fire2medamage_p, anti_block_rate, fasthitrecover_yan_v, fatallystrikeres_p;
                             lifemax_p -> lifemax_yan_p (walkrunshadow o cot 1 DA co - phan hinh anh do engine, xem patch11)
 1385 Than Quang Toan Nhieu 2: sorbdamage_p -> sorbdamage_yan_p; lifemax_p -> lifemax_yan_p
Chi doi cac cot LvlSetting*/LvlData* (giu ten, mo ta, moi cot khac). Bo qua 2119 (gia dinh chu duyet), 1386/1392/1393/1418 (chieu BOSS /
danh hieu, khong phai chieu nguoi choi 3 phai). Doc/ghi latin-1, giu NL, idempotent. DUNG: python vhtd_data_patch9.py [--kiem]
"""
import io, os, sys, shutil

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw\settings__skills.txt"
BAK = ".truoc_vhtd_patch9_0209"
IDS = [1349, 1358, 1385]

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

def load(s):
    nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
    lines = s.split(nl); hdr = lines[0].split("\t")
    return nl, lines, hdr

def main():
    vnl, vlines, vhdr = load(rd(VLTK))
    vid = vhdr.index("SkillId"); vrows = {}
    for l in vlines[1:]:
        c = l.split("\t")
        if len(c) > vid and c[vid].strip().isdigit(): vrows[int(c[vid])] = dict(zip(vhdr, c))
    for side in ("server", "client"):
        p = os.path.join(BIN, side, "settings", "skills.txt")
        s = rd(p); nl, lines, hdr = load(s)
        sid_col = hdr.index("SkillId"); n = 0
        for i in range(1, len(lines)):
            c = lines[i].split("\t")
            if len(c) <= sid_col or not c[sid_col].strip().isdigit(): continue
            k = int(c[sid_col])
            if k not in IDS: continue
            v = vrows[k]; ch = []
            for j, hname in enumerate(hdr):
                if not (hname.startswith("LvlSetting") or hname.startswith("LvlData")): continue
                if hname in v and c[j] != v[hname]:
                    ch.append("%s %s->%s" % (hname, c[j] or "-", v[hname] or "-")); c[j] = v[hname]
            if ch:
                n += 1; lines[i] = "\t".join(c)
                print("  [+] skills %d: %s" % (k, "; ".join(ch)))
        if not n:
            print("  [=] %s skills.txt da khop 3 hang" % side); continue
        out = nl.join(lines)
        if "\xef\xbf\xbd" in out: raise SystemExit("EF BF BD " + p)
        if not KIEM:
            if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
            io.open(p, "w", encoding="latin-1", newline="").write(out)
        print("  => %s %s (%d hang)" % ("KIEM" if KIEM else "ghi", p, n))

if __name__ == "__main__":
    print("vhtd_data_patch9 [VHTD 02/09r]%s" % (" (KIEM)" if KIEM else "")); main(); print("XONG.")
