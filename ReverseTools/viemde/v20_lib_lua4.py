# -*- coding: utf-8 -*-
"""v20_lib_lua4.py - VA "attempt to index global 'lib' (a nil value)" ready.lua:324.

GOC (do tan ma, khong doan):
  1. ready.lua goi lib:DoFunInWorld/lib:ShuffleTable (9 cho) nhung KHONG Include
     file dinh nghia 'lib'. Ben Linux, 'lib' nam trong
     script/activitysys/functionlib.lua va duoc engine nap san vao state dung
     chung; JX1 Include = dofile vao state DANG GOI (moi state rieng) nen phai
     Include tuong minh. Thieu => ScriptError.log 26-27/08:
        attempt to index global `lib' (a nil value)  (ready.lua:324)
  2. functionlib.lua ben JX1 la ban Linux chep NGUYEN VAN (da so khop byte),
     trong do DoFunByPlayer/DoFunInWorld ket thuc bang `return unpack(re)`.
     unpack la ham Lua 5 - Lua 4.0.1 cua JX1 KHONG co (grep lbaselib.c: chi co
     call/getn) => sua xong loi 1 se sap tiep "attempt to call global `unpack'".

CO CHE DoFunInWorld dung duoc tren JX1 (da kiem tan ScriptFuns.cpp):
  - SCRIPT_SUBWORLDINDEX = chuoi "SubWorld" (KPlayerDef.h:13); OpenMission/
    SetMissionV/SetMissionS/AddMSPlayer deu doc bien toan cuc Lua nay qua
    GetSubWorldIndex(L) (ScriptFuns.cpp:513-515) => doi tam bien 'SubWorld'
    roi goi ham la du, y het co che ban Linux.
  - call(fun, arg) la builtin co that (lbaselib.c:610), tien le trong du an:
    lib/baseclass.lua:46, lib/composeclass.lua:272.

MIENG VA (toi thieu):
  A. functionlib.lua: 2 cho `return unpack(re)` -> `return re[1]`.
     Moi cho goi trong toan cay JX1 (ready.lua, challengeoftime) chi dung
     <=1 gia tri tra ve (GetMissionV/GetMSPlayerCount tra 1 so) - da grep het.
     Them 1 comment canh bao truoc DoFunByTeam (van con unpack(arg) :335/:338,
     ngoai duong Viem De, chua ai dung nen KHONG sua - dung la sap).
  B. ready.lua: them Include functionlib.lua sau Include log.lua.
     (functionlib.lua:3 Include \\script\\lib\\string.lua KHONG ton tai ben
     JX1 - vo hai: LuaIncludeFile chi ghi DebugLog "[script] Include HONG"
     roi di tiep, ScriptFuns.cpp:2051-2052; IncludeLib SETTING/FILESYS da co.)

KHONG PHAI BUILD (chi Lua). Script NPC bi cache theo state => can KHOI DONG LAI
GameServer (hoac doi chu game) de an - CAM tu restart (luat 28/08).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_lua4lib lan dau).
"""
import io
import os
import re
import shutil
import sys

FL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\activitysys\functionlib.lua"
RD = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\yandibaozang\readymap\ready.lua"
HAU_TO = ".truoc_lua4lib"
NHAN = "[LUA4 28/08]"


def doc(p):
    raw = io.open(p, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    return raw, eol


def can_bang_token(duongdan, nd):
    """Kiem can bang khoi Lua theo token (bai hoc su co y6)."""
    dem = {"function": 0, "then": 0, "elseif": 0, "do": 0, "end": 0}
    for raw in nd.replace("\r\n", "\n").split("\n"):
        l = re.sub(r'"[^"]*"', '""', raw)
        l = re.sub(r"'[^']*'", "''", l)
        j = l.find("--")
        if j >= 0:
            l = l[:j]
        for k in dem:
            dem[k] += len(re.findall(r"\b%s\b" % k, l))
    mo = dem["function"] + dem["then"] - dem["elseif"] + dem["do"]
    return mo, dem["end"]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v20_lib_lua4 - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    viec = []  # (duong dan, noi dung moi)

    # ---------- A. functionlib.lua ----------
    raw, eol = doc(FL)
    if NHAN in raw:
        print("  functionlib: DA VA - bo qua (idempotent)")
    else:
        d = raw.split(eol)
        hi0 = sum(1 for c in raw if ord(c) > 127)

        # A1+A2: hai cho `return unpack(re)` - phan biet bang dong lien truoc
        sua = 0
        for truoc in ("\tPlayerIndex = nOldPlayer;", "\tSubWorld = nOldSubWorld;"):
            vt = [i for i in range(1, len(d))
                  if d[i].rstrip() == "\treturn unpack(re)" and d[i - 1].rstrip() == truoc]
            if len(vt) != 1:
                print("!!! LOI TO: neo %r khop %d lan (can 1)" % (truoc.strip(), len(vt)))
                return 1
            i = vt[0]
            d[i:i + 1] = [
                "\t-- %s unpack la ham Lua 5, Lua 4.0.1 cua JX1 khong co (goi la sap" % NHAN,
                "\t-- 'attempt to call nil'). Moi cho goi DoFun* trong du an chi dung",
                "\t-- <=1 gia tri tra ve (da ra soat het) => tra re[1] la du.",
                "\treturn re[1]",
            ]
            sua = sua + 1
        print("  functionlib: doi %d cho `return unpack(re)` -> `return re[1]`" % sua)

        # A3: comment canh bao truoc DoFunByTeam
        vt = [i for i, l in enumerate(d) if l.rstrip() == "function lib:DoFunByTeam(nPlayerIndex, fun, ...)"]
        if len(vt) != 1:
            print("!!! LOI TO: neo DoFunByTeam khop %d lan (can 1)" % len(vt))
            return 1
        d[vt[0]:vt[0]] = [
            "-- %s CANH BAO: ham duoi van dung unpack(arg) (Lua 5) - tren JX1 goi la" % NHAN,
            "-- sap. Chua co ai dung nen de nguyen; muon dung phai doi sang call(...).",
        ]
        print("  functionlib: them canh bao truoc DoFunByTeam")

        nd = eol.join(d)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (functionlib)")
            return 1
        mo, dong = can_bang_token(FL, nd)
        mo0, dong0 = can_bang_token(FL, raw)
        if (mo - dong) != (mo0 - dong0):
            print("!!! LOI TO: can bang token doi: truoc %+d, sau %+d" % (mo0 - dong0, mo - dong))
            return 1
        print("  functionlib: token %d/%d (lech %+d, giu nguyen nhu truoc)" % (mo, dong, mo - dong))
        viec.append((FL, nd))

    # ---------- B. ready.lua ----------
    raw, eol = doc(RD)
    if NHAN in raw:
        print("  ready: DA VA - bo qua (idempotent)")
    else:
        d = raw.split(eol)
        hi0 = sum(1 for c in raw if ord(c) > 127)
        neo = 'Include("\\\\script\\\\lib\\\\log.lua")'
        vt = [i for i, l in enumerate(d) if l.strip() == neo]
        if len(vt) != 1:
            print("!!! LOI TO: neo Include log.lua khop %d lan (can 1)" % len(vt))
            return 1
        i = vt[0] + 1
        d[i:i] = [
            "-- %s JX1 Include = dofile vao state dang goi (moi state Lua rieng, khong" % NHAN,
            "-- co state dung chung nhu Linux) => 'lib' (DoFunInWorld/ShuffleTable) phai",
            "-- Include tuong minh; thieu no la \"index global 'lib' (nil)\" tai :324.",
            'Include("\\\\script\\\\activitysys\\\\functionlib.lua")',
        ]
        print("  ready: them Include functionlib sau dong %d" % (vt[0] + 1))

        nd = eol.join(d)
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (ready)")
            return 1
        mo, dong = can_bang_token(RD, nd)
        mo0, dong0 = can_bang_token(RD, raw)
        if (mo - dong) != (mo0 - dong0):
            print("!!! LOI TO: can bang token doi: truoc %+d, sau %+d" % (mo0 - dong0, mo - dong))
            return 1
        print("  ready: token %d/%d (lech %+d, giu nguyen nhu truoc)" % (mo, dong, mo - dong))
        viec.append((RD, nd))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd in viec:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
            print("  sao luu -> %s" % os.path.basename(sao))
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nXong. KHONG can build; can KHOI DONG LAI GameServer de script nap lai")
    print("(CAM tu restart - bao chu game).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
