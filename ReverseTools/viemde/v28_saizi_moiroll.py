# -*- coding: utf-8 -*-
"""v28_saizi_moiroll.py - VA "Hinh nhan roi vao hu khong": khong ai duoc moi gieo.

GOC (doc tan ma, chua cho chu game bao):
  saizi.lua:11-14 moi nguoi gieo xuc xac bang
        for i = 1, GetTeamSize() do  PlayerIndex = GetTeamMember(i); RollItem(nRet)  end
  va truyen GetTeamSize() lam "so nguoi tham gia" cho ApplyItemDice (:6).
  Nhung TRONG TRAN nguoi choi DA ROI TO DOI:
        readymap\\ready.lua:251  LeaveTeam()   (PlayerEnterMatch)
        readymap\\include.lua:102 va :223       LeaveTeam()
  LuaGetTeamSize (ScriptFuns.cpp:2853-2861): khong con co doi -> nTeamId = -1
  -> tra 0. Vay vong lap chay 0 lan => KHONG AI duoc moi gieo => het 20 giay,
  YDBZ_OnTimeOver chot voi nWinner = 0 => bao "Tat ca nguoi choi deu huy bo
  roll" va **Hinh nhan bi thu hoi**, khong ai nhan duoc.
  Hinh nhan la vat pham xuong song cua tinh nang (hoi sinh tai cho o
  player_death.lua:46-63, va la nguyen lieu doi thuong), nen loi nay lam hong
  toan bo phan thuong cua moi boss.

MIENG VA (giu dung y do "chia do trong noi bo doi cua minh"):
  gom nguoi nhan theo NGUOI CUNG PHE TRONG MISSION - dung khuon co san cua
  chinh tinh nang (npc_death.lua:62 / :86 / :186:
      idx, pidx = GetNextPlayer(YDBZ_MISSION_MATCH, idx, camp)  ),
  phe lay bang GetTmpCamp() cua nguoi vua ket lieu (YDBZ_award da dat
  PlayerIndex = nguoi do truoc khi goi - npc_death.lua:34).
  Neu vi ly do nao do khong gom duoc ai -> lui ve chinh nguoi ket lieu, de mon
  do KHONG BAO GIO roi vao hu khong.

KHONG build. Can KHOI DONG LAI GameServer de nap (bao chu).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_moiroll lan dau).
"""
import io
import os
import re
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\missions\yandibaozang\saizi.lua")
HAU_TO = ".truoc_moiroll"
NHAN = "[XX 29/08]"

# neo: 3 dong goc (dong 11-14 hien tai), so bang strip de khong ke le
CU = [
    "local nPlayerOld = PlayerIndex",
    "for i=1, GetTeamSize() do",
    "PlayerIndex = GetTeamMember(i)",
    "RollItem(nRet)",
    "end",
    "PlayerIndex = nPlayerOld",
]

MOI = [
    "\tlocal nPlayerOld = PlayerIndex",
    "\t-- " + NHAN + " TRONG TRAN NGUOI CHOI DA ROI TO DOI (readymap\\ready.lua:251",
    "\t-- LeaveTeam) nen GetTeamSize() = 0 (ScriptFuns.cpp:2860) => vong cu khong",
    "\t-- moi duoc AI gieo => het gio chot 0 nguoi => Hinh nhan bi thu hoi, khong",
    "\t-- ai nhan. Gom nguoi CUNG PHE trong mission - dung khuon cua chinh tinh",
    "\t-- nang (npc_death.lua:62/:86/:186). PlayerIndex luc nay la nguoi vua ket",
    "\t-- lieu (npc_death.lua:34 dat truoc khi goi) nen GetTmpCamp() ra dung phe.",
    "\tfor i=1, getn(tbNhan) do",
    "\t\tPlayerIndex = tbNhan[i]",
    "\t\tRollItem(nRet)",
    "\tend",
    "\tPlayerIndex = nPlayerOld",
]

# khoi gom nguoi, chen ngay sau dong "function YDBZ_DiceDice(...)"
GOM = [
    "\t-- " + NHAN + " gom danh sach nguoi duoc moi gieo (xem chu thich duoi).",
    "\tlocal tbNhan = {}",
    "\tlocal nCampXX = GetTmpCamp()",
    "\tlocal nIdxXX = 0",
    "\tlocal nPidXX",
    "\tif nCampXX ~= nil and nCampXX > 0 then",
    "\t\tfor i=1, 10 do",
    "\t\t\tnIdxXX, nPidXX = GetNextPlayer(YDBZ_MISSION_MATCH, nIdxXX, nCampXX)",
    "\t\t\tif nPidXX ~= nil and nPidXX > 0 then",
    "\t\t\t\ttinsert(tbNhan, nPidXX)",
    "\t\t\tend",
    "\t\t\tif nIdxXX == 0 then",
    "\t\t\t\tbreak",
    "\t\t\tend",
    "\t\tend",
    "\tend",
    "\tif getn(tbNhan) == 0 then",
    "\t\ttbNhan[1] = PlayerIndex",
    "\tend",
]


def can_bang(nd):
    dem = {"function": 0, "then": 0, "elseif": 0, "do": 0, "end": 0}
    for raw in nd.replace("\r\n", "\n").split("\n"):
        l = re.sub(r'"[^"]*"', '""', raw)
        l = re.sub(r"'[^']*'", "''", l)
        j = l.find("--")
        if j >= 0:
            l = l[:j]
        for k in dem:
            dem[k] += len(re.findall(r"\b%s\b" % k, l))
    return dem["function"] + dem["then"] - dem["elseif"] + dem["do"], dem["end"]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v28_saizi_moiroll - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA VA - bo qua (idempotent)")
        return 0

    d = raw.split(eol)
    vt = [i for i in range(len(d) - len(CU) + 1)
          if [x.strip() for x in d[i:i + len(CU)]] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: neo vong roll khop %d lan (can 1)" % len(vt))
        return 1
    i = vt[0]
    d[i:i + len(CU)] = MOI
    print("  ok  thay vong roll theo to doi -> theo phe trong mission (dong %d)" % (i + 1))

    # chen khoi gom ngay sau dong khai bao ham
    vt2 = [k for k, l in enumerate(d) if l.strip().startswith("function YDBZ_DiceDice(")]
    if len(vt2) != 1:
        print("!!! LOI TO: khong tim thay ham YDBZ_DiceDice (khop %d)" % len(vt2))
        return 1
    d[vt2[0] + 1:vt2[0] + 1] = GOM
    print("  ok  chen khoi gom nguoi sau dong %d" % (vt2[0] + 1))

    # ApplyItemDice: so nguoi = getn(tbNhan) thay cho GetTeamSize()
    n_ap = 0
    for k, l in enumerate(d):
        if "ApplyItemDice(" in l and "GetTeamSize()" in l:
            d[k] = l.replace("GetTeamSize()", "getn(tbNhan)")
            n_ap += 1
    if n_ap != 1:
        print("!!! LOI TO: dong ApplyItemDice co GetTeamSize khop %d lan (can 1)" % n_ap)
        return 1
    print("  ok  ApplyItemDice: so nguoi = getn(tbNhan)")

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    mo, dong = can_bang(nd)
    mo0, dong0 = can_bang(raw)
    if (mo - dong) != (mo0 - dong0):
        print("!!! LOI TO: can bang token doi: truoc %+d sau %+d" % (mo0 - dong0, mo - dong))
        return 1
    # dem tren MA THAT (bo chu thich - comment cua toi co nhac ten ham)
    con = 0
    for l in nd.split(eol):
        j = l.find("--")
        if j >= 0:
            l = l[:j]
        con += l.count("GetTeamSize()")
    if con:
        print("!!! LOI TO: van con %d loi goi GetTeamSize() that" % con)
        return 1
    print("  chot: token %d/%d, byte cao %d, khong con GetTeamSize" % (mo, dong, hi0))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. Can KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
