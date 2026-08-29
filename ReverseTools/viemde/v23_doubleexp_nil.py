# -*- coding: utf-8 -*-
"""v23_doubleexp_nil.py - VA "giet ai xong sap / khong duoc exp" (chan TRUOC khi no).

GOC (doc tan ma, chua doi chu game bao):
  - doubleexp.lua:12 va :15 viet `nExp = DynamicExecuteByPlayer(...)`.
  - Ben JX1, LuaDynamicExecuteByPlayer (ScriptFuns.cpp:2375-2429) chay xong
    LUON `return 0` => KHONG tra ve gia tri nao cho Lua (ban Linux tra gia tri).
    => nExp = nil O MOI TRUONG HOP, ke ca khi tep dich ton tai.
  - YDBZ_checkdoubleexp tra nil; moi cho goi deu lam:
        nexp = YDBZ_checkdoubleexp(nexp) ; AddOwnExp(nexp)
    (npc_death.lua:96/106/190 - vuot ai, boss no khi, hoan thanh;
     player_death.lua:37 - giet nguoi choi)
    => AddOwnExp(nil) + format("%s", nil) => SAP ngay ai dau tien.
  - Them: `\\script\\activitysys\\config\\41\\extend.lua` (su kien x2 cua ban
    Linux) KHONG co ben JX1 - khong sao, vi ket qua von da khong dung duoc.

MIENG VA (toi thieu, khong dung C++ vi ham do dung chung nhieu he):
  giu nExp goc; chi NHAN gia tri tra ve khi no THAT SU la so > 0. Nhu vay:
    * hom nay (ham C tra rong)      -> exp giu nguyen muc thuong: DUNG.
    * mai kia neu C tra gia tri     -> tu dong an su kien x2, khong phai sua lai.
  Van GOI DynamicExecuteByPlayer nhu cu de giu tac dung phu (ghi log su kien).

KHONG build. Script nay duoc Include boi npc_death/player_death - can KHOI DONG
LAI GameServer de nap (bao chu, CAM tu restart).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_x2nil lan dau).
"""
import io
import os
import re
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\missions\yandibaozang\doubleexp.lua")
HAU_TO = ".truoc_x2nil"
NHAN = "[X2NIL 29/08]"

CU = [
    "\tfor i=1, getn(%tbDouble) do",
    "\t\tlocal tbfunc = %tbDouble[i]",
    "\t\tnExp = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)",
    "\tend",
]
MOI = [
    "\t-- " + NHAN + " JX1: LuaDynamicExecuteByPlayer (ScriptFuns.cpp:2429) LUON",
    "\t-- `return 0` = khong tra gia tri nao => gan thang vao nExp la nil, keo theo",
    "\t-- AddOwnExp(nil) + format(nil) SAP ngay ai dau (npc_death :96/:106/:190,",
    "\t-- player_death :37). Nay chi nhan ket qua khi no THAT SU la so > 0:",
    "\t-- hom nay giu exp muc thuong (dung), mai kia C tra gia tri thi tu dong an.",
    "\tfor i=1, getn(%tbDouble) do",
    "\t\tlocal tbfunc = %tbDouble[i]",
    "\t\tlocal nRa = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)",
    "\t\tif nRa ~= nil and nRa > 0 then",
    "\t\t\tnExp = nRa",
    "\t\tend",
    "\tend",
]

CU2 = '\tnExp = DynamicExecuteByPlayer(PlayerIndex, "\\\\script\\\\vng_feature\\\\double_mission_award.lua", "tbVnX2Award:X2YDBZ", nExp)'
MOI2 = [
    '\tlocal nRa2 = DynamicExecuteByPlayer(PlayerIndex, "\\\\script\\\\vng_feature\\\\double_mission_award.lua", "tbVnX2Award:X2YDBZ", nExp)',
    "\tif nRa2 ~= nil and nRa2 > 0 then",
    "\t\tnExp = nRa2",
    "\tend",
]


def can_bang_token(nd):
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
    print("=== v23_doubleexp_nil - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA VA - bo qua (idempotent)")
        return 0

    d = raw.split(eol)
    vt = [i for i in range(len(d) - len(CU) + 1)
          if [x.rstrip() for x in d[i:i + len(CU)]] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: neo vong for khop %d lan (can 1)" % len(vt))
        return 1
    i = vt[0]
    d[i:i + len(CU)] = MOI

    vt2 = [k for k, l in enumerate(d) if l.rstrip() == CU2]
    if len(vt2) != 1:
        print("!!! LOI TO: neo dong X2YDBZ khop %d lan (can 1)" % len(vt2))
        return 1
    d[vt2[0]:vt2[0] + 1] = MOI2
    print("  ok  va 2 cho gan-tu-DynamicExecuteByPlayer (dong %d va %d)"
          % (i + 1, vt2[0] + 1))

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    mo, dong = can_bang_token(nd)
    mo0, dong0 = can_bang_token(raw)
    if (mo - dong) != (mo0 - dong0):
        print("!!! LOI TO: can bang token doi: truoc %+d sau %+d" % (mo0 - dong0, mo - dong))
        return 1
    def dem_goi(s):
        """dem loi goi THAT (bo dong chu thich - comment cua toi co nhac ten ham)."""
        n = 0
        for l in s.replace("\r\n", "\n").split("\n"):
            j = l.find("--")
            if j >= 0:
                l = l[:j]
            n += l.count("DynamicExecuteByPlayer(")
        return n
    if dem_goi(nd) != dem_goi(raw):
        print("!!! LOI TO: so loi goi that thay doi: %d -> %d" % (dem_goi(raw), dem_goi(nd)))
        return 1
    print("  chot: token %d/%d, byte cao %d, so loi goi giu nguyen" % (mo, dong, hi0))

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
    print("  DA GHI. Can KHOI DONG LAI GameServer de nap (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
