# -*- coding: utf-8 -*-
"""_sua_lua_5bang_b.py - sua 3 dong simcity_admin.lua bi goi_va_bot_5bang_b.py (ban dau, m.expand) pha:
literal \\n trong chuoi Lua bi doi thanh xuong dong that -> 'unfinished string'. Idempotent."""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\simcity_admin.lua"
DUNG = [
    '\tlocal t = {"<color=yellow>Tuyển thành viên<color>\\nChọn bang (mục tiêu theo botbang.txt; bot đủ cấp, chưa bang tự về xin)"}',
    '\tlocal t = {"<color=yellow>Nạp quỹ bang<color>\\nChọn bang nhận tiền"}',
    '\tSayEx({"<color=yellow>Bang bot (stt.tên tt người quỹ thành chủ)<color>\\n"..s,',
]

s = io.open(P, "r", encoding="latin-1", newline="").read()
n = 0
for d in DUNG:
    dung = vn(d)
    hong = dung.replace("\\n", "\n")
    if dung in s:
        continue
    c = s.count(hong)
    if c != 1:
        print("KHONG THAY dong hong (%d): %s" % (c, d[:50]))
        sys.exit(1)
    s = s.replace(hong, dung)
    n += 1
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("sua %d dong" % n)
