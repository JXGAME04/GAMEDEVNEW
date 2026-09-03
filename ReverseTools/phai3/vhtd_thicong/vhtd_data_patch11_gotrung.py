# -*- coding: utf-8 -*-
"""vhtd_data_patch11_gotrung.py [VHTD 02/09t] - GO THUOC TINH TRUNG (bien the "Duong") - SUA HOI QUY DOT 6 + DOT 9 CUA TOI.

GOC: engine cua ta anh xa bien the _yan_ VA ban thuong vao CUNG MOT HAM XU LY (KNpcAttribModify.cpp:36-84:
ProcessFunc[magic_attackspeed_yan_v] = &AttackSpeedV, [magic_lifemax_yan_p] = &LifeMaxP, ...). VLTK co he AM/DUONG rieng
nen khai ca hai la hai kho khac nhau; ben ta thi khai ca hai = CONG DOI cung mot chi so, va tooltip hien HAI DONG y het nhau
(dot 6 toi da bo hau to "(Duong)" theo quyet dinh cua chu o dot g nen hai dong khong con phan biet duoc).

Dot 6 va dot 9 toi dong bo skills.txt theo VLTK da THEM cac bien the _yan_ vao 8 hang von chi co ban thuong -> tu do
cong doi. Chu phat hien qua tooltip Kiem Tong Tong Quyet: hai dong "Toc do danh - ngoai cong: +7%" giong het nhau.

VA: go bo bien the _yan_ o nhung hang co CA HAI, tra ve dung MOT lan cong. Danh sach (chi hang do dot 6/9 cua toi them vao):
  1349 Kiem Tong Tong Quyet      : attackspeed_yan_v
  1376 Long Huyen Kiem Khi       : lifemax_yan_p
  1381 Kiem Khi Vo Cap           : lifemax_yan_p
  1385 Than Quang Toan Nhieu 2   : lifemax_yan_p
  1968 Hieu qua Kinh Dao Phach Ngan : attackspeed_yan_v, castspeed_yan_v
  1970 Kinh Dao Phach Ngan       : attackspeed_yan_v, castspeed_yan_v
  1971 Tri Dung Sieu Luan        : lifemax_yan_p
  1980 Co Tuong Bach Chien Ham   : attackspeed_yan_v
KHONG dung den 92 / 208 Phat Tam Tu Huu (chieu CO DIEN, von da khai ca hai tu truoc - khong phai do toi, doi la doi can bang cu).
KHONG dung den cac hang chi co MOT bien the (vd 1980 allres_yan_p, 2122 lifemax_yan_v, 2123 ...) - nhung cai do dung 1 lan, khong sao.

Doc/ghi latin-1, giu NL, idempotent. DUNG: python vhtd_data_patch11_gotrung.py [--kiem]
"""
import io, os, sys, shutil

KIEM = "--kiem" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
BAK = ".truoc_gotrung_0209"

GO = {
    1349: ["attackspeed_yan_v"],
    1376: ["lifemax_yan_p"],
    1381: ["lifemax_yan_p"],
    1385: ["lifemax_yan_p"],
    1968: ["attackspeed_yan_v", "castspeed_yan_v"],
    1970: ["attackspeed_yan_v", "castspeed_yan_v"],
    1971: ["lifemax_yan_p"],
    1980: ["attackspeed_yan_v"],
}

def main():
    for side in ("server", "client"):
        p = os.path.join(BIN, side, "settings", "skills.txt")
        s = io.open(p, "r", encoding="latin-1", newline="").read()
        nl = "\r\n" if s.count("\r\n") * 2 > s.count("\n") else "\n"
        lines = s.split(nl); hdr = lines[0].split("\t")
        sid_col = hdr.index("SkillId"); n = 0
        for i in range(1, len(lines)):
            c = lines[i].split("\t")
            if len(c) <= sid_col or not c[sid_col].strip().isdigit(): continue
            sid = int(c[sid_col])
            if sid not in GO: continue
            at = {}
            for j, hn in enumerate(hdr):
                if hn.startswith("LvlSetting") and j < len(c): at[c[j].strip()] = j
            ch = []
            for ten in GO[sid]:
                base = ten.replace("_yan_", "_")
                if ten not in at: continue
                if base not in at:
                    print("  [!] %d: %s khong co ban thuong %s -> GIU (khong phai trung)" % (sid, ten, base)); continue
                j = at[ten]
                jd = hdr.index("LvlData" + hdr[j][len("LvlSetting"):])
                ch.append("%s(+%s)" % (ten, c[jd].strip()))
                c[j] = ""
                if jd < len(c): c[jd] = ""
            if ch:
                n += 1; lines[i] = "\t".join(c)
                print("  [+] skills %d: go %s" % (sid, "; ".join(ch)))
        if not n:
            print("  [=] %s skills.txt khong con thuoc tinh trung" % side); continue
        out = nl.join(lines)
        if "\xef\xbf\xbd" in out: raise SystemExit("EF BF BD " + p)
        if not KIEM:
            if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
            io.open(p, "w", encoding="latin-1", newline="").write(out)
        print("  => %s %s (%d hang)" % ("KIEM" if KIEM else "ghi", p, n))

if __name__ == "__main__":
    print("vhtd_data_patch11_gotrung [VHTD 02/09t]%s" % (" (KIEM)" if KIEM else "")); main(); print("XONG.")
