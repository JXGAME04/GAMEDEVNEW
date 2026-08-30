# -*- coding: utf-8 -*-
"""Trich phan doi chieu cuoi cua workflow trich hang so."""
import io
import json
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"C:\Users\nguye\AppData\Local\Temp\claude"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
     r"\61d924bb-ce66-46e1-a22b-820305b4bde7\tasks\wi0pu7bu2.output")
DICH = r"D:\GAMEDEVNEW\DOICHIEU_HANGSO_3008.md"

t = io.open(P, encoding="utf-8", errors="replace").read()
nd = None
try:
    o = json.loads(t)
    nd = (o.get("result") or {}).get("kiem") or o.get("kiem")
except Exception:
    i = t.find('"kiem"')
    if i >= 0:
        j = t.rfind("{", 0, i)
        for k in range(len(t), j, -1):
            try:
                o = json.loads(t[j:k])
                nd = (o.get("kiem") or (o.get("result") or {}).get("kiem"))
                break
            except Exception:
                continue

if not nd:
    print("!!! khong trich duoc")
    sys.exit(1)

dau = """# ĐỐI CHIẾU HẰNG SỐ CÒN LẠI (30/08/2026)

> Kết quả đối chiếu của 6 nhóm khảo sát, đã lọc trùng và loại những gì đã nối dây.
> **Chưa thi công** — đây là danh sách việc cho các đợt sau.
>
> Trạng thái lúc đối chiếu: 152 khoá đã nối. Danh sách dưới đây là phần còn lại.

---

"""
with io.open(DICH, "w", encoding="utf-8", newline="") as f:
    f.write(dau + nd)
print("da ghi %s (%d ky tu)" % (DICH, len(nd)))
print()
i = nd.find("HẠNG B")
print(nd[:200])
