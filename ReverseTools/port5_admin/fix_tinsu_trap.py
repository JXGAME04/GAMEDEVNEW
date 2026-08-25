# -*- coding: utf-8 -*-
r"""[25/08] VA 2 LOI PORT cua TIN SU (loi kien truc per-file-state, ban Linux khong dinh).

LOI 1 - TRAP NUOT MENU (ScriptError 08:52 + 09:00 hom nay):
  Nguoi choi dung canh Dich Quan (1412,3203), trap o ngay canh (SetPos ve 1414,3197).
  Bam NPC -> menu mo; nhich chan giam trap -> engine doi "current script" cua player
  sang trap_*.lua; bam lua chon -> engine goi ham menu TRONG STATE TRAP:
      ScriptError: (trap_qianbaoku.lua) cFuncName:(ture_try_starttask) - nil
      ScriptError: (trap_qianbaoku.lua) cFuncName:(ture_movecity)      - nil
  Ban Linux Y HET script nay van chay vi MOI script chung MOT lua_State (ham nao cung
  thay). JX1 moi file mot state -> trap khong co ham menu -> "bam khong phan ung".
  VA: Include 2 script NPC cua ai vao trap => state trap co du moi ham menu (mo phong
  single-state Linux cho cum nay). Include dat TRUOC function main() nen main cua trap
  van de len main cua file duoc Include (Lua: dinh nghia sau thang).

LOI 2 - OnRevive NIL (36 loi luc boot):
  Engine JX1 goi ExecuteScript(ActionScript, "OnRevive", idx) khi npc hoi sinh
  (KNpc.cpp:8676) VO DIEU KIEN. turebug90.lua (ca ban Linux) KHONG co ham nay -
  engine Linux khong goi kieu do. Them ham RONG - khong doi hanh vi.
  (kill_level.lua cung dinh 160 loi nhung phien song song dang sua file do - KHONG dung.)
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
ENC = "latin-1"

TRAP = [
    (r"task\tollgate\messenger\trap\trap_qianbaoku.lua",
     ["\\\\script\\\\task\\\\tollgate\\\\messenger\\\\messenger_turerukou.lua",
      "\\\\script\\\\task\\\\tollgate\\\\messenger\\\\qianbaoku\\\\messenger_turenpc.lua"]),
    (r"task\tollgate\messenger\trap\trap_shanshenmiao.lua",
     ["\\\\script\\\\task\\\\tollgate\\\\messenger\\\\messenger_templerukou.lua",
      "\\\\script\\\\task\\\\tollgate\\\\messenger\\\\shanshenmiao\\\\messenger_temnpc.lua"]),
    (r"task\tollgate\messenger\trap\trap_fengzhiqi.lua",
     ["\\\\script\\\\task\\\\tollgate\\\\messenger\\\\messenger_flyrukou.lua",
      "\\\\script\\\\task\\\\tollgate\\\\messenger\\\\fengzhiqi\\\\messenger_flynpc.lua"]),
]

def rd(p): return io.open(p, "r", encoding=ENC, newline="").read()
def wr(p, s): io.open(p, "w", encoding=ENC, newline="").write(s)
def bak(p, t):
    q = p + ".truoc_" + t
    if not os.path.isfile(q): shutil.copyfile(p, q)

for rel, incs in TRAP:
    p = os.path.join(E, rel)
    s = rd(p)
    NL = "\r\n" if "\r\n" in s else "\n"
    if "FIX 25/08" in s:
        print("da va:", rel); continue
    dau = (
        "-- [FIX 25/08] JX1 moi file mot lua_State (Linux: MOT state chung). Trap chay se" + NL +
        "-- chiem 'current script' cua nguoi choi; menu NPC dang mo bam vao se goi ham" + NL +
        "-- trong STATE TRAP -> nil (ScriptError: ture_try_starttask / ture_movecity)." + NL +
        "-- Include 2 script NPC cua ai de state trap co DU ham menu - mo phong dung" + NL +
        "-- hanh vi single-state cua ban Linux. main() cua trap o cuoi file nen van de len." + NL)
    for i in incs:
        dau += 'Include("' + i.replace("\\\\", "\\") + '")' + NL
    dau += NL
    bak(p, "trapfix_2508")
    wr(p, dau + s)
    print("va xong:", rel)

# ---- OnRevive rong cho turebug90 ----
p = os.path.join(E, r"task\tollgate\messenger\qianbaoku\90\turebug90.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "function OnRevive" in s:
    print("turebug90: da co OnRevive")
else:
    bak(p, "trapfix_2508")
    s += NL + "-- [FIX 25/08] engine JX1 goi OnRevive khi npc hoi sinh (KNpc.cpp:8676) VO DIEU KIEN;" + NL
    s += "-- ban Linux khong co ham nay (engine Linux khong goi kieu do) -> 36 ScriptError moi" + NL
    s += "-- lan boot. Ham RONG - khong doi hanh vi." + NL
    s += "function OnRevive(npcindex)" + NL + "end" + NL
    wr(p, s)
    print("turebug90: da them OnRevive rong")
print("XONG")
