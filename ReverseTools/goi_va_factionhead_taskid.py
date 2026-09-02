# -*- coding: ascii -*-
r"""goi_va_factionhead_taskid.py - [BOTBANG 02/09] (chu game: "fix loi vao bang truoc")

GOC: factionhead.lua:38 xuatsu() goi SetTask(TASK_DUNGCHUNG2, 1) nhung hang TASK_DUNGCHUNG2
chi dinh nghia o script\header\taskid.lua (= 4134); 13 script NPC mon phai + factionhead /
lib_task / FactionHelper khong Include taskid.lua. Moi tep .lua la MOT Lua state rieng
(Engine KLuaScript.cpp:445 Lua_Create) nen hang la nil -> Lua 4.0 lua_tonumber(nil) = 0 ->
LuaSetTaskValue ghi TASK 0. Bot (KPlayerBot.cpp:7671) doc task 4134 = 0 -> HUY xin vao bang.
Nguoi choi that khong thay vi GetTask(TASK_DUNGCHUNG2) cung lech ve o 0 y het.

VA: them 1 dong Include("\\script\\header\\taskid.lua") o DONG 1 cua factionhead.lua, dat
TRUOC lib_task.lua: 3 hang trung ten khac gia tri (TASK_NVST 21/355, TASK_TANTHU 22/320,
TASK_THOIGIAN9 52/321) -> Include sau ghi de nen gia tri cua lib_task.lua van thang nhu cu;
chi TASK_DUNGCHUNG2 (chi taskid.lua co) thanh 4134. Ca 13 script mon phai deu Include
factionhead.lua nen sua mot cho la du. Can restart GameServer (script NPC nap luc boot).

Idempotent: chay lai khong ap hai lan. Doc/ghi latin-1, giu CRLF, kiem so byte cao khong doi.
Mirror sang D:\GAMEDEVNEW\serverscript_jx2\jx1_edits\factionhead.lua.
"""
import io
import os
import shutil
import sys

SRC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\header\factionhead.lua"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\jx1_edits\factionhead.lua"

# trong tep Lua la HAI dau gach cheo nguoc: Include("\\script\\...") -> python '\\\\'
ANCHOR = 'Include("\\\\script\\\\lib\\\\lib_task.lua")\r\n'
MARK = 'header\\\\taskid.lua'
LINE = ('Include("\\\\script\\\\header\\\\taskid.lua")'
        '\t-- [BOTBANG 02/09] TASK_DUNGCHUNG2 (4134) truoc day nil trong state script mon phai'
        ' -> xuatsu() ghi nham task 0; dat TRUOC lib_task.lua de 3 hang trung ten'
        ' (TASK_NVST/TASK_TANTHU/TASK_THOIGIAN9) giu gia tri cua lib_task nhu cu\r\n')


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def main():
    if not os.path.isfile(SRC):
        print("KHONG THAY: " + SRC)
        return 2
    s = io.open(SRC, "r", encoding="latin-1", newline="").read()
    if MARK in s:
        print("DA AP TU TRUOC (co Include taskid.lua) - khong doi gi")
    else:
        if not s.startswith(ANCHOR):
            print("NEO DONG 1 KHONG KHOP - dung, khong ghi")
            print(repr(s[:80]))
            return 3
        s2 = LINE + s
        if hb(s2) != hb(s):
            print("SO BYTE CAO DOI - dung, khong ghi")
            return 4
        io.open(SRC, "w", encoding="latin-1", newline="").write(s2)
        print("AP XONG: +1 dong, high-byte %d -> %d, %d -> %d byte" % (hb(s), hb(s2), len(s), len(s2)))
    d = os.path.dirname(MIR)
    if os.path.isdir(d):
        shutil.copyfile(SRC, MIR)
        print("MIRROR: " + MIR)
    else:
        print("KHONG THAY THU MUC MIRROR: " + d)
    return 0


if __name__ == "__main__":
    sys.exit(main())
