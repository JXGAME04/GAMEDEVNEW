# -*- coding: utf-8 -*-
r"""[25/08] SUA LOI CUA CHINH BAN VA TRAP: Include ghi MOT backslash.

Loi: fix_tinsu_trap.py ghi Include("\script\task\...") - MOT '\'. Lua 4 doc escape:
  \s -> 's' (mat '\'), \t -> TAB THAT, \m -> 'm' ...
=> duong dan thanh "script<TAB>ask..." nat, engine fopen fail + remap fail, Include
im lang bo qua => state trap van thieu ham menu => loi y nguyen sau restart 10:04.
Chuan cua moi file .lua trong du an: HAI backslash trong nguon ("\\script\\task\\...").

Sua: thay dong Include sai bang dong DUNG (2 backslash trong file = '\\' 2 ky tu).
"""
import io, os

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\task\tollgate\messenger\trap"
ENC = "latin-1"

VIEC = {
    "trap_qianbaoku.lua": [
        r"\script\task\tollgate\messenger\messenger_turerukou.lua",
        r"\script\task\tollgate\messenger\qianbaoku\messenger_turenpc.lua"],
    "trap_shanshenmiao.lua": [
        r"\script\task\tollgate\messenger\messenger_templerukou.lua",
        r"\script\task\tollgate\messenger\shanshenmiao\messenger_temnpc.lua"],
    "trap_fengzhiqi.lua": [
        r"\script\task\tollgate\messenger\messenger_flyrukou.lua",
        r"\script\task\tollgate\messenger\fengzhiqi\messenger_flynpc.lua"],
}

for ten, ds in VIEC.items():
    p = os.path.join(E, ten)
    s = io.open(p, "r", encoding=ENC, newline="").read()
    doi = 0
    for d in ds:
        sai  = 'Include("%s")' % d                       # 1 backslash (dang sai tren dia)
        dung = 'Include("%s")' % d.replace("\\", "\\\\") # 2 backslash trong nguon Lua
        if dung in s:
            continue
        if sai in s:
            s = s.replace(sai, dung, 1); doi += 1
        else:
            print("%-24s KHONG THAY dong: %s" % (ten, sai))
    if doi:
        io.open(p, "w", encoding=ENC, newline="").write(s)
    print("%-24s sua %d dong Include" % (ten, doi))

# kiem lai bang mat: in dong Include cua ca 3 file
print()
for ten in VIEC:
    p = os.path.join(E, ten)
    for l in io.open(p, "r", encoding=ENC, newline="").read().split("\n"):
        if "Include(" in l:
            print("%-24s %s" % (ten, l.strip()[:90]))
