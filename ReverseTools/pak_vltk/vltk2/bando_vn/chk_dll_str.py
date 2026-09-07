# -*- coding: utf-8 -*-
"""Kiem chuoi TCVN3 moi (BANDO20) co nam trong nhi phan khong. In ASCII de khong vuong console cp1252."""
import sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
p = sys.argv[1]
d = open(p, "rb").read()
tests = {
    "CORE: [Ban do] Dang tu chay toi": "[Bản đồ] Đang tự chạy tới",
    "CORE: [Ban do] Da toi":           "[Bản đồ] Đã tới",
    "CORE: Khong tim duoc duong bo":   "Không tìm được đường bộ tới",
    "CORE: Khong qua duoc cua map":    "Không qua được cửa map",
    "UI  : bam de chay toi":           "bấm để chạy tới",
    "UI  : Kinh do":                   "Kinh đô",
    "UI  : Son dong":                  "Sơn động",
}
ok = 0
for lab, s in tests.items():
    hit = unicode_to_tcvn3_bytes(s) in d
    ok += hit
    print("  %-34s %s" % (lab, "CO" if hit else "khong"))
print("%s: %d/%d chuoi co mat (%d B)" % (p.split("\\")[-1].split("/")[-1], ok, len(tests), len(d)))
