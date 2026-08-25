# -*- coding: utf-8 -*-
"""Tao ban c26 cho cum TIN SU tu tool goc cua phien 3HD (thay THUMUC/TEPLE/OUT)."""
import io, re

SRC = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong\c26_soat_thoai.py"
DST = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\667a822b-6945-44b9-8b84-5bf2eac05ce5\scratchpad\c26_tinsu.py"

s = io.open(SRC, "r", encoding="utf-8", errors="replace").read()

thumuc_moi = "THUMUC = [\n" + "".join(
    '    r"%s",\n' % d for d in (
        "script\\task\\tollgate\\messenger",
        "script\\task\\tollgate\\messenger\\qianbaoku",
        "script\\task\\tollgate\\messenger\\qianbaoku\\90",
        "script\\task\\tollgate\\messenger\\shanshenmiao",
        "script\\task\\tollgate\\messenger\\fengzhiqi",
        "script\\task\\tollgate\\messenger\\trap",
        "script\\item\\xinshirenwu",
        "script\\item\\messenger",
    )) + "]"
teple_moi = "TEPLE = [\n    r\"script\\task\\tollgate\\tinsu_addnpc.lua\",\n]"

s = re.sub(r"THUMUC = \[.*?\]", lambda m: thumuc_moi, s, count=1, flags=re.S)
s = re.sub(r"TEPLE = \[.*?\]", lambda m: teple_moi, s, count=1, flags=re.S)
s = s.replace("port_3hd\\audit\\C26_soat_thoai.md", "port5_admin\\C26_soat_thoai_TINSU.md")

io.open(DST, "w", encoding="utf-8").write(s)
print("da tao", DST)
