# -*- coding: utf-8 -*-
"""Kiem nhi phan moi CO chuoi cua ban va 08/09 khong (in ASCII cho khoi vuong console)."""
import sys, hashlib
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

CASES = {
    "CoreClient.dll": [
        ("B: nguoi tu di chuyen -> huy", "Ngươi tự di chuyển - đã huỷ tự chạy."),
        ("A: cua map ghi sai toa do",    "Cửa map ghi sai toạ độ - dừng tự chạy."),
        ("cu: dang tu chay toi",         "Đang tự chạy tới"),
    ],
    "Game.exe": [
        ("C: bam de chay toi",  "bấm để chạy tới"),
        ("C: Kinh do",          "Kinh đô"),
    ],
}
for p in sys.argv[1:]:
    name = p.replace("\\", "/").split("/")[-1]
    d = open(p, "rb").read()
    print("\n%s  %d B  md5 %s" % (name, len(d), hashlib.md5(d).hexdigest()[:8]))
    for lab, s in CASES.get(name, []):
        print("   %-32s %s" % (lab, "CO" if unicode_to_tcvn3_bytes(s) in d else "*** KHONG ***"))
