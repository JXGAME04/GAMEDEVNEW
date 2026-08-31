# -*- coding: utf-8 -*-
"""Cap nhat ghi nho cuoi ngay 30/08."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
M = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory")

# --- 1. bo sung vao jx1-donscript-3008.md ---
p = os.path.join(M, "jx1-donscript-3008.md")
d = io.open(p, encoding="utf-8").read()
neo = "## CHƯA đụng — chờ chủ"
if "## Đã làm nốt (cuối ngày)" not in d and neo in d:
    them = (
        "## Đã làm nốt (cuối ngày)\n"
        "- **`storm_clear()` ĐÃ CHẶN**: gọi thiếu tham số nay báo và thoát;"
        " muốn xoá thật phải gọi rõ `storm_clear(-1)`.\n"
        "- **Bảng \"bù hoạt động\" ĐÃ TẮT 5 dòng chết** (`lib_ham.lua:296`)."
        " Giữ đúng dòng Vận Tiêu và **đánh số lại thành `[1]`** — bắt buộc, vì"
        " hai hàm duyệt bằng `while TB_BU_HD[i]`, hổng một số là dừng ngay.\n"
        "- Dời `New folder` (17 tệp `.lua.bak` rác).\n"
        "\n"
        "## 🔴 QUYẾT ĐỊNH: KHÔNG quét dọn hàng loạt ~1.000 tệp bẫy/obj bản đồ"
        " Trung Quốc\n"
        "Trong **đúng một buổi**, công cụ của tôi cho **hai câu trả lời sai** về"
        " chính câu hỏi này, và **cả hai lần đều sai theo hướng \"tuyên bố tệp"
        " đang sống là chết\"**:\n"
        "1. Thiếu đường nạp thứ 8 (dữ liệu bản đồ trong pak đã nén) → báo"
        " 1.675/3.065 tệp chết.\n"
        "2. **Biểu thức `[\\w\\-]+\\.lua` CẮT CỤT tên tiếng Trung** — ký tự như"
        " `·` (byte GBK) không phải word-char nên `Â·ÈË_³Ì´óÎª.lua` bị đọc thành"
        " `ÈË_³Ì´óÎª.lua`, tra không khớp. Sửa thành"
        " `[^\\s\"'\\\\/<>()\\[\\],;=]+\\.lua` thì bắt thêm **1.639 tên**.\n"
        "\n"
        "Bộ khảo sát nền cũng xếp 9 cây thư mục TQ (1.285 tệp) là \"an toàn\" —"
        " đối chiếu thì **454 tệp đang được bản đồ dùng**.\n"
        "\n"
        "Lợi ích gần bằng không (tệp nằm im), rủi ro là một bẫy dịch chuyển chết"
        " lặng. ⇒ Chỉ dọn **từng cụm có tên tuổi**, đọc thật xem là tính năng gì.\n"
        "\n"
    )
    d = d.replace(neo, them + neo, 1)
    io.open(p, "w", encoding="utf-8", newline="").write(d)
    print("da bo sung jx1-donscript-3008.md")
else:
    print("jx1-donscript-3008.md da co - bo qua")

# --- 2. thay dong chi muc ---
p = os.path.join(M, "MEMORY.md")
d = io.open(p, encoding="utf-8").read()
i = d.find("- [🧽⚠️")
if i >= 0:
    j = d.find("\n", i)
    moi = ("- [🧽⚠️🟢 30/08 **DỌN SCRIPT + CHỐT NGÀY: 186 tệp/10.807 dòng ra"
           " khỏi cây (hoàn tác được) · 4 lỗi ĐANG XẢY RA đã vá ·"
           " `storm_clear()` đã chặn · bảng bù hoạt động tắt 5 dòng chết."
           " 🔴 QUYẾT ĐỊNH KHÔNG quét dọn hàng loạt ~1.000 tệp — trong một buổi"
           " công cụ tôi SAI HAI LẦN, cả hai đều theo hướng tuyên bố tệp sống là"
           " chết**](jx1-donscript-3008.md) — đọc `BANGIAO_HOANTAT_3008.md`"
           " trước.")
    d = d[:i] + moi + d[j:]
    io.open(p, "w", encoding="utf-8", newline="").write(d)
    print("da cap nhat MEMORY.md (%d byte)" % len(d.encode("utf-8")))
else:
    print("!! khong tim thay dong chi muc")
