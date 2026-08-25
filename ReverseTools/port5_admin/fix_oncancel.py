# -*- coding: utf-8 -*-
r"""[24/08] npc_enter.lua (Bach Nhan Loi Dai) dung `oncancel` o 2 lua chon
"Ket thuc doi thoai" nhung KHONG dinh nghia ham do trong state nay.
Ban Linux dung 1 lua_State chung nen `oncancel` (ham RONG, xem
missions\tong\collectgoods\award.lua:157) luon co san; JX1 moi tep mot state nen mat.
=> nguoi choi bam "Ket thuc doi thoai" o NPC loi vao co the nem loi script.
Va: dinh nghia lai dung khuon goc (ham rong).
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
p = os.path.join(E, r"script\missions\bairenleitai\npc_enter.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()

if "function oncancel" in s:
    print("da co oncancel - bo qua")
    raise SystemExit(0)

NL = "\r\n" if "\r\n" in s else "\n"
anchor = "function bairen_dialog()"
assert s.count(anchor) == 1, "anchor = %d" % s.count(anchor)

ins = ("-- [KIEM CHUNG 24/08] ban Linux dung MOT lua_State chung nen `oncancel` (ham RONG," + NL +
       "-- khuon missions\\tong\\collectgoods\\award.lua:157) luon co san. JX1 moi tep mot state" + NL +
       "-- nen o day `oncancel` = nil => bam \"Ket thuc doi thoai\" co the nem loi script." + NL +
       "function oncancel()" + NL +
       "end" + NL + NL)

s = s.replace(anchor, ins + anchor, 1)
shutil.copyfile(p, p + ".truoc_oncancel_2408")
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("DA VA npc_enter.lua: them function oncancel() rong")

d = r"D:\GAMEDEVNEW\serverscript_jx2\bairenleitai\jx1_edits\script\missions\bairenleitai"
if not os.path.isdir(d): os.makedirs(d)
shutil.copyfile(p, os.path.join(d, "npc_enter.lua"))
print("mirror ok")
