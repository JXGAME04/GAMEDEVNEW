# -*- coding: utf-8 -*-
"""Them dong chi muc cho ghi nho bay 'nap san moi script'."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
p = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory\MEMORY.md")
d = io.open(p, encoding="utf-8").read()
if "jx1-nap-san-moi-script.md" in d:
    print("da co - bo qua")
else:
    neo = "- [🧽⚠️🟢 30/08"
    i = d.index(neo)
    moi = ("- [🔴📜 30/08 **BẪY: engine NẠP SẴN TỪNG tệp `.lua` lúc khởi động"
           " (dòng `Total ScriptLoaded`), nên THÂN CHUNK chạy ĐỘC LẬP — gọi hàm"
           " của tệp khác ở thân chunk là `ScriptError 4`, tệp đó không nạp"
           " được**](jx1-nap-san-moi-script.md) — đã vấp thật với `BDH_CFG` ở"
           " `partner_reward2.lua`; chạy `t71_quet_goi_nil_thanchunk.py` sau mỗi"
           " đợt sửa script.\n")
    io.open(p, "w", encoding="utf-8", newline="").write(d[:i] + moi + d[i:])
    print("da them dong chi muc (%d byte)"
          % os.path.getsize(p))
