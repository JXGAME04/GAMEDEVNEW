# -*- coding: utf-8 -*-
r"""Sua 2 dong hong trong muc 15 BANGIAO (bay heredoc rut backslash:
\x64 -> 'd', \b -> 0x08)."""
import io

BS = chr(92)
p = r"D:\GAMEDEVNEW\BANGIAO_BANDONGHANH_2708.md"
s = io.open(p, "r", encoding="latin-1", newline="").read()

hong1 = "`Sources" + BS + "Cored" + BS + "ServerRelease" + BS + "`"
dung1 = ("`Sources" + BS + "Core" + BS + "x64" + BS + "ServerRelease" + BS + "`")
assert hong1 in s, "khong thay hong1"
s = s.replace(hong1, dung1, 1)

hong2 = "ReverseTools" + chr(8) + "andonghanh" + BS + "p27_goc_include_reset.py"
dung2 = "ReverseTools" + BS + "bandonghanh" + BS + "p27_goc_include_reset.py"
assert hong2 in s, "khong thay hong2 (0x08)"
s = s.replace(hong2, dung2, 1)

# muc cu (phien truoc cung dinh bay): script\item + 0x08 + dh_admin.lua
hong3 = "item" + chr(8) + "dh_admin.lua"
if hong3 in s:
    s = s.replace(hong3, "item" + BS + "bdh_admin.lua")
    print("sua ca cho bdh_admin cu")

assert chr(8) not in s, "van con byte 0x08"
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("da sua 2 dong + het 0x08")
