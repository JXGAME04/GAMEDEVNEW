# -*- coding: utf-8 -*-
"""Cap nhat chi muc ghi nho cuoi ngay 30/08 - phien sau doc BANGIAO_TIEPTUC_3108."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
M = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory")
p = os.path.join(M, "MEMORY.md")
d = io.open(p, encoding="utf-8").read()

DONG = (
    "- [🚩▶️ **BẮT ĐẦU PHIÊN SAU TỪ ĐÂY — đọc"
    " `D:\\GAMEDEVNEW\\BANGIAO_TIEPTUC_3108.md` TRƯỚC TIÊN.** Đang dang dở:"
    " Viêm Đế 3 triệu chứng chủ báo, đã vá 2 lỗi, **chờ khởi động lại để nghiệm"
    " thu** (bản vá `saizi.lua` 16:44 mới hơn lần khởi động 16:42 nên CHƯA"
    " sống)**](jx1-viemde-3loi-3008.md)\n"
)

if "BANGIAO_TIEPTUC_3108" in d:
    print("da co dong chi muc - bo qua")
else:
    d = DONG + d
    io.open(p, "w", encoding="utf-8", newline="").write(d)
    print("da chen dong dau MEMORY.md (%d byte)"
          % os.path.getsize(p))

# cap nhat dong 30/08 dot 3 cho khop trang thai cuoi
old = "- [🧽⚠️🟢 30/08"
i = d.find(old)
if i >= 0:
    j = d.find("\n", i)
    moi = ("- [🧽⚠️🟢 30/08 **CHỐT NGÀY: 188 tệp/~10.900 dòng ra khỏi cây (hoàn"
           " tác được) · 6 lỗi ĐANG XẢY RA đã vá · `storm_clear()` đã chặn ·"
           " DLL ĐÃ SWAP (2 bản vá C++ đã sống, `tinhnang\\datau` biến mất hẳn)."
           " 🔴 QUYẾT ĐỊNH KHÔNG quét dọn hàng loạt ~1.000 tệp — phép thử thô"
           " bạo cho thấy bộ lọc SAI ~40%**](jx1-donscript-3008.md)")
    d = d[:i] + moi + d[j:]
    io.open(p, "w", encoding="utf-8", newline="").write(d)
    print("da cap nhat dong dot 3 (%d byte)" % os.path.getsize(p))
