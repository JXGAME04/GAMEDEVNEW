# -*- coding: utf-8 -*-
"""Trich truong `tomtat` tu ket qua workflow khao sat lich/thuong ra tep .md."""
import io
import json
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
NGUON = (r"C:\Users\nguye\AppData\Local\Temp\claude"
         r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
         r"\61d924bb-ce66-46e1-a22b-820305b4bde7\tasks\w6cgx6qq8.output")
DICH = r"D:\GAMEDEVNEW\KHAOSAT_LICH_THUONG_2908.md"

t = io.open(NGUON, encoding="utf-8", errors="replace").read()
print("kich thuoc nguon: %d ky tu" % len(t))

nd = None
# thu doc ca tep nhu JSON
try:
    o = json.loads(t)
    nd = o.get("tomtat") or (o.get("result") or {}).get("tomtat")
except Exception:
    # tim doan JSON dau tien co khoa tomtat
    i = t.find('"tomtat"')
    if i >= 0:
        j = t.rfind("{", 0, i)
        for k in range(len(t), j, -1):
            try:
                o = json.loads(t[j:k])
                nd = o.get("tomtat")
                break
            except Exception:
                continue

if not nd:
    print("!!! khong trich duoc truong tomtat")
    sys.exit(1)

dau = """# KHẢO SÁT LỊCH + THƯỞNG TOÀN GAME (29/08/2026)

> Tài liệu này là **kết quả khảo sát**, chưa phải bản đã thi công.
> Đợt 29/08 mới nối dây phần **20 công tắc bật/tắt hoạt động**
> (`script\\cauhinh\\ch_lich.lua`). Phần còn lại là danh sách việc cho đợt sau.
>
> Sáu nhóm quét song song: trận chiến lớn · bang hội · boss/rơi đồ ·
> sự kiện theo thời gian · nhiệm vụ hằng ngày · settings + C++.

---

"""
with io.open(DICH, "w", encoding="utf-8", newline="") as f:
    f.write(dau + nd)
print("da ghi %s (%d ky tu)" % (DICH, len(nd)))
