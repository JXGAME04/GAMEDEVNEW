#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""va_lmbj_2.py - [VANTIEU 06/09 - KIEM CHUNG] va LOI BOOT #4:
Include cua JX1 KHONG dedupe (Sources/Core/Src/ScriptFuns.cpp:2061 goi
lua_dofile MOI LAN; chi IncludeLib moi require-once, :2628-2638).

Ca 22 tep nhanh ca nhan deu mo dau bang Include("...head.lua"), ma
head.lua:3 lai gan LongMenBiaoJu = {} (RESET). Hau qua o CAY JX1: moi tep
nap sau se XOA sach nhung gi tep truoc da gan. Do bang thuc nghiem
(nap that bang Lua54Dll, Include KHONG dedupe, thu tu boot = thu tu thu
muc): sau khi nap xong 23 tep con MAT 8 thanh vien -
  TaskClass, DlgClass, TaskDataClass, BiaoCheClass, BoxClass,
  BrokenBiaoCheClass, PlayerClass, BoxAwardData
=> LongMenBiaoJu:GetTaskClass() tra nil, ca tinh nang chet.

Cach va = dung dung loi viet nha JX1 (vd script/tinhnang/3hoatdong/beidou/
bd_head.lua:8 "tbBeidou = tbBeidou or {}",
script/global/mantlesystem/mantleupgrade_npc.lua:5 co chu thich ro
"thoren.lua dofile lai moi lan bam NPC -> khong duoc reset"):
  1. head.lua:3-7  -> "X = X or {}" / "X = X or 0"
  2. lmbj_config.lua: 3 so tra cuu song (GSBiaoCheByName / GSBiaoCheByIndex /
     PlayerList) -> "or {}", va chan boc chong ham (moi lan head.lua chay lai
     se boc them mot lop OnGSTransfer/GenerateTask/AcceptRandomTask).

Chay:  python va_lmbj_2.py         (xem truoc)
       python va_lmbj_2.py --ghi   (ghi that, tu tao ban luu .truoc_lmbj2)

LUAT: doc/ghi bang latin-1, newline="" - KHONG dung Edit/Write tool.
"""
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

GOC = os.path.join("D:\\", "GAMEDEVNEW_wt_vantieu", "serverscript_jx2", "vantieu")
HEAD = os.path.join(GOC, "lua54", "script", "event", "longmenbiaoju", "head.lua")
CFG = os.path.join(GOC, "moi", "script", "event", "longmenbiaoju", "lmbj_config.lua")

GHI = "--ghi" in sys.argv

# (tep, chuoi cu, chuoi moi)  - chuoi cu phai xuat hien DUNG MOT lan
VA = [
    (HEAD,
     "LongMenBiaoJu = {}\r\n"
     "LongMenBiaoJu.GSCallBackList = {}\r\n"
     "LongMenBiaoJu.RelayCallBackId = 0\r\n"
     "LongMenBiaoJu.BiaoCheList = {}\r\n"
     "LongMenBiaoJu.GenData = {}\r\n",
     "-- [VANTIEU 06/09 va#4] Include cua JX1 KHONG dedupe (ScriptFuns.cpp:2061\r\n"
     "-- lua_dofile MOI lan): 22 tep deu Include tep nay o dong 1, nen \"= {}\"\r\n"
     "-- se XOA sach nhung gi tep nap truoc da gan (do that: mat 8 lop, ke ca\r\n"
     "-- TaskClass/BiaoCheClass). Dung loi viet nha JX1: \"X = X or {}\".\r\n"
     "LongMenBiaoJu = LongMenBiaoJu or {}\r\n"
     "LongMenBiaoJu.GSCallBackList = LongMenBiaoJu.GSCallBackList or {}\r\n"
     "LongMenBiaoJu.RelayCallBackId = LongMenBiaoJu.RelayCallBackId or 0\r\n"
     "LongMenBiaoJu.BiaoCheList = LongMenBiaoJu.BiaoCheList or {}\r\n"
     "LongMenBiaoJu.GenData = LongMenBiaoJu.GenData or {}\r\n"),

    (CFG,
     "LongMenBiaoJu.GSBiaoCheByName = {}\nLongMenBiaoJu.GSBiaoCheByIndex = {}\n",
     "-- [VANTIEU 06/09 va#4] PHAI \"or {}\": head.lua Include tep nay o dong cuoi,\n"
     "-- ma head.lua bi Include lai moi lan mot tep khac trong thu muc duoc nap\n"
     "-- (JX1 khong dedupe Include) - \"= {}\" se xoa sach Tieu Xa dang song.\n"
     "LongMenBiaoJu.GSBiaoCheByName = LongMenBiaoJu.GSBiaoCheByName or {}\n"
     "LongMenBiaoJu.GSBiaoCheByIndex = LongMenBiaoJu.GSBiaoCheByIndex or {}\n"),

    (CFG,
     "LongMenBiaoJu.PlayerList = {}\n",
     "LongMenBiaoJu.PlayerList = LongMenBiaoJu.PlayerList or {}   -- [va#4] xem ghi chu muc 12\n"),

    (CFG,
     "-- Boc 3 cua ngo ben Relay: vao bang duong nao cung tu nap so lieu truoc.\n"
     "local _lmbj_OnGSTransfer = LongMenBiaoJu.OnGSTransfer\n",
     "-- Boc 3 cua ngo ben Relay: vao bang duong nao cung tu nap so lieu truoc.\n"
     "-- [VANTIEU 06/09 va#4] CO CHAN BOC CHONG: head.lua (va tep nay theo no)\n"
     "-- chay lai moi lan mot tep trong thu muc duoc nap, khong chan thi moi lan\n"
     "-- lai boc them mot lop len ba ham nay.\n"
     "if LongMenBiaoJu.bDaBoc ~= 1 then\n"
     "LongMenBiaoJu.bDaBoc = 1\n"
     "local _lmbj_OnGSTransfer = LongMenBiaoJu.OnGSTransfer\n"),

    (CFG,
     "local _lmbj_AcceptRandomTask = LongMenBiaoJu.AcceptRandomTask\n"
     "function LongMenBiaoJu:AcceptRandomTask(ParamHandle)\n"
     "\tself:EnsureLoaded()\n"
     "\treturn _lmbj_AcceptRandomTask(self, ParamHandle)\n"
     "end\n",
     "local _lmbj_AcceptRandomTask = LongMenBiaoJu.AcceptRandomTask\n"
     "function LongMenBiaoJu:AcceptRandomTask(ParamHandle)\n"
     "\tself:EnsureLoaded()\n"
     "\treturn _lmbj_AcceptRandomTask(self, ParamHandle)\n"
     "end\n"
     "end   -- [va#4] het khoi chan boc chong\n"),
]


def main():
    doc = {}
    for p, _, _ in VA:
        if p not in doc:
            with open(p, "rb") as f:
                doc[p] = f.read().decode("latin-1")
    ok = True
    for p, cu, moi in VA:
        n = doc[p].count(cu)
        print("%-14s  tim thay %d lan  (%s...)" % (os.path.basename(p), n, cu.split("\n")[0][:52]))
        if n != 1:
            ok = False
            continue
        doc[p] = doc[p].replace(cu, moi, 1)
    if not ok:
        print("\n!! CO MUC KHONG KHOP DUNG 1 LAN - KHONG GHI GI CA.")
        return 1
    if not GHI:
        print("\n(xem truoc - chua ghi. Them --ghi de ghi that)")
        return 0
    for p, noi in doc.items():
        luu = p + ".truoc_lmbj2"
        if not os.path.exists(luu):
            shutil.copy2(p, luu)
        with open(p, "wb") as f:
            f.write(noi.encode("latin-1"))
        print("DA GHI %s (ban luu %s)" % (p, os.path.basename(luu)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
