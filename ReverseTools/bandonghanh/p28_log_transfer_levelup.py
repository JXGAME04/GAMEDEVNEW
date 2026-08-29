# -*- coding: utf-8 -*-
r"""[PETSYS 28/08 toi] v2 (strip-CR): cay PLOG nhanh vao TransferExp + LevelUpDlg.
GO SAU NGHIEM THU."""
import io
import os

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CR = chr(13)


def va(duong, cap, dau):
    p = os.path.join(SV, duong)
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    if dau in raw:
        print("da co:", duong)
        return
    lf = raw.replace(CR + "\n", "\n")
    for cu, moi in cap:
        if lf.count(cu) != 1:
            print("!! anchor (%d): %r" % (lf.count(cu), cu[:60]))
            return
        lf = lf.replace(cu, moi, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("VA:", duong)


va(r"script\petsys\transferexp.lua", [
    ("\tif (GetExp() < nNeedExp) then\n"
     '\t\tTalk(1,"",format(%NOT_ENOUGH_EXP, nNeedExp, nTransferCount))\n'
     "\t\treturn\n",
     '\tPLOG("TransferExp: exp=" .. GetExp() .. " daily=" .. (PlayerFunLib:GetTaskDaily(TSK_COUNT_TRANSFER_EXP) or -1))\n'
     "\tif (GetExp() < nNeedExp) then\n"
     '\t\tTalk(1,"",format(%NOT_ENOUGH_EXP, nNeedExp, nTransferCount))\n'
     '\t\tPLOG("TransferExp: da goi Talk THIEU EXP")\n'
     "\t\treturn\n"),
    ("\t\t\treturn\n\tend\n\tReduceOwnExp(nReducExp)\n",
     '\t\t\tPLOG("TransferExp: chan daily 25 lan/ngay")\n'
     "\t\t\treturn\n\tend\n\tReduceOwnExp(nReducExp)\n"
     '\tPLOG("TransferExp: THANH CONG +1 TamePoint (tru 50tr exp)")\n'),
], 'PLOG("TransferExp:')

va(r"script\petsys\levelup.lua", [
    ("function PetSys:LevelUpDlg()\n",
     "function PetSys:LevelUpDlg()\n"
     '\tPLOG("LevelUpDlg: lv=" .. PET_GetLevel() .. " diem=" .. PET_GetUpgradePoint() .. "/" .. PET_GetGrownPoint() .. "/" .. PET_GetTamePoint())\n'),
    ("\tif not tbNextLeveldata then \n\t\treturn \n\tend\n",
     "\tif not tbNextLeveldata then \n"
     '\t\tPLOG("LevelUpDlg: tbLevelUp[" .. (PET_GetLevel() + 1) .. "] NIL - dataload chua nap?")\n'
     "\t\treturn \n\tend\n"
     '\tPLOG("LevelUpDlg: can " .. tbNextLeveldata[1] .. "/" .. tbNextLeveldata[2] .. "/" .. tbNextLeveldata[3] .. " thuoc=" .. tbNextLeveldata[4])\n'),
], 'PLOG("LevelUpDlg:')
print("XONG p28 v2")
