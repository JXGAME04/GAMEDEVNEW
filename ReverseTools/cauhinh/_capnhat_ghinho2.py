# -*- coding: utf-8 -*-
"""Bo sung ket qua phep thu thu bao vao ghi nho."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
M = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory")

p = os.path.join(M, "jx1-donscript-3008.md")
d = io.open(p, encoding="utf-8").read()
neo = ("Lợi ích gần bằng không (tệp nằm im), rủi ro là một bẫy dịch chuyển chết"
       " lặng. ⇒ Chỉ dọn **từng cụm có tên tuổi**, đọc thật xem là tính năng gì.")
if "PHÉP THỬ DỨT ĐIỂM" not in d and neo in d:
    them = (
        "### PHÉP THỬ DỨT ĐIỂM (làm sau hai lần sai)\n"
        "Không tin bộ lọc nào nữa: lấy 10 tệp ngẫu nhiên trong nhóm \"mồ côi\","
        " tìm **nguyên byte tên tệp** trên toàn bộ máy chủ — `script\\`,"
        " `settings\\`, `Maps\\` (5.601 tệp), mã nguồn C++, và **cả 5 pak đã"
        " giải nén** (154.607 mục).\n\n"
        "**4/10 tệp mẫu THẬT SỰ có trong `maps.pak`** (`ÄÏÔÀÕò-¸æÊ¾ÅÆ1.lua`,"
        " `Â·±ê_ÁúÈª´åtoÂÞÏüÉ½.lua`, `¼ñÊ°_cyl20_...lua`,"
        " `sign_tolongmenzhen.lua`). Bộ lọc sai **~40%** trên nhóm đó — dời"
        " 1.064 tệp thì có thể đã dời ~400 tệp đang sống.\n\n"
        "Công cụ phép thử: `t69_kiem_mau_thobao.py`. **Lần sau muốn dọn cụm nào"
        " thì chạy nó trước.**\n\n"
    )
    d = d.replace(neo, them + neo, 1)
    io.open(p, "w", encoding="utf-8", newline="").write(d)
    print("da bo sung phep thu vao ghi nho")
else:
    print("da co hoac khong tim thay neo")
