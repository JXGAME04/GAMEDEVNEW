# -*- coding: utf-8 -*-
"""Sua dong chi muc cho dung pham vi - CO NHIEU PHIEN cung lam du an nay."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
p = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory\MEMORY.md")
d = io.open(p, encoding="utf-8").read()

CU = "- [🚩▶️ **BẮT ĐẦU PHIÊN SAU TỪ ĐÂY"
i = d.find(CU)
if i < 0:
    print("khong tim thay dong cu")
    raise SystemExit
j = d.find("\n", i)

MOI = ("- [🔥⏸🔴 30/08 **VIÊM ĐẾ — ĐANG DANG DỞ, CHỜ NGHIỆM THU.** Chủ báo 3"
       " triệu chứng (quái không mất máu / không bu vào / xúc xắc không add đồ)."
       " Gốc: `tbReady` bị bảng rỗng đè mất `InitMatchMission` nên **mission"
       " KHÔNG BAO GIỜ được mở**. Đã vá 2 lỗi — nhưng bản vá `saizi.lua` (16:44)"
       " **mới hơn** lần khởi động gần nhất (16:42) nên **CHƯA sống**, phải khởi"
       " động lại mới test được**](jx1-viemde-3loi-3008.md) — đọc"
       " `BANGIAO_TIEPTUC_3108.md` mục 1.1.")

d = d[:i] + MOI + d[j:]
io.open(p, "w", encoding="utf-8", newline="").write(d)
print("da sua dong chi muc (%d byte)" % os.path.getsize(p))
