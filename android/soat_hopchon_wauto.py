# -*- coding: utf-8 -*-
"""Soat: hop chon nao co dong dai hon be ngang o -> chu bi cat."""
import io
import os
import re

os.chdir(r"D:\GAMEDEVNEW\.claude\worktrees\kind-bassi-b0f319")

H = io.open("Sources/S3Client/Ui/UiCase/UiWAutoBang.h", encoding="latin-1", newline="").read()

# s_LuaChon_<tab>_<i>[] = { "...", ... };
lc = {}
for m in re.finditer(r'static const char\* const (s_LuaChon_\d+_\d+)\[\] = \{(.*?)\};', H, re.S):
    ten, than = m.group(1), m.group(2)
    lc[ten] = re.findall(r'"((?:[^"\\]|\\.)*)"', than)

# s_Muc_<tab>[] rows
tong = 0
xau = []
for m in re.finditer(r'static const WAUiMuc (s_Muc_(\d+))\[\] = \{(.*?)\n\};', H, re.S):
    tab = int(m.group(2))
    ini = io.open("android/du_lieu_ghi_de/ui/ui3/uiwauto_tab%d.ini" % tab,
                  encoding="latin-1", newline="").read()
    rong = {}
    cur = None
    for ln in ini.split("\n"):
        ln = ln.strip()
        if ln.startswith("["):
            cur = ln[1:-1]
        elif ln.startswith("Width=") and cur:
            rong[cur] = int(ln.split("=", 1)[1])
    for r in m.group(3).split("\n"):
        r = r.strip()
        if not r.startswith("{ 3,"):
            continue
        c = [x.strip() for x in r.strip("{},").split(",")]
        khe = int(c[1])
        ten = c[7]
        idc = c[-1].strip('"')
        w = rong.get("Chon%d" % khe, 0)
        if ten == "NULL":
            dai, chu = 0, "(nguon dong)"
        else:
            ds = lc.get(ten, [])
            chu = max(ds, key=len) if ds else ""
            dai = len(chu)
        tong += 1
        can = dai * 6 + 34
        if can > w:
            xau.append((tab, "Chon%d" % khe, w, can, dai, chu, idc))

print("tong hop chon:", tong, " bi cat:", len(xau))
for t, k, w, can, dai, chu, idc in xau:
    print("  tab %-2d %-7s W=%-4d can=%-4d (%2d byte)  %-28s %s" % (t, k, w, can, dai, chu[:28], idc))
