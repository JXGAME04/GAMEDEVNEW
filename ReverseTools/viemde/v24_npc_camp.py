# -*- coding: utf-8 -*-
"""v24_npc_camp.py - VA "vao tran co quai nhung QUAI KHONG DANH nguoi choi".

GOC (khong doan - loi giai DA CO SAN trong chinh du an):
  - npc.lua goi `AddNpcEx(id, level, series, SubWorld, px, py, 1, name, isboss)`.
    So 1 o vi tri 7: ben LINUX nghia la **bNoRevive** (chet la bien mat) - moi
    script Linux khac trong du an deu chu thich dung the (basemission/lib.lua:40,
    bairenleitai/hundred_arena.lua:642, tongcastle:944 - "²»ÖØÉú").
  - Ben JX1, LuaAddNpcEx (ScriptFuns.cpp:7055-7060) lay tham so 7 lam **CAMP**
    va SetCurrentCamp GHI DE camp tu npcs.txt.
  - Quai Viem De (npcs.txt id 1289-1319) von Camp=5 (thu/quai - thu dich tat ca),
    bi ep thanh camp 1 = chinh phai. Nguoi choi trong tran mang tmpcamp 1/2/3
    (ready.lua:252 SetTmpCamp(nCamp) theo so doi) => doi 1 CUNG PHE voi quai =>
    GenOneRelation (KNpcSet.cpp:143) coi la dong minh => quai khong tan cong.
    (Chu game test mot minh = doi 1 => "npc khong danh toi" - khop 100%.)
  - Doi port 3 hoat dong 25/08 DA gap va giai dung bai nay: KJx2WarInfra.cpp:1686
    ghi ro cung trieu chung, va lam ham rieng **HD3_AddNpcEx** - KHONG dung den
    camp (giu camp npcs.txt) + tham so 7 = bNoRevive. challengeoftime/npc.lua:431
    la khuon mau dang chay tot.

MIENG VA: dat mot bi danh o dau npc.lua
    YDBZ_AddNpcEx = HD3_AddNpcEx (neu DLL co) hoac AddNpcEx (lui ve, nhu driver)
roi doi CA 4 cho goi trong tep sang bi danh do. Khong dung C++ (khong dung cham
cac he JX1 khac dang dua vao ngu nghia camp cua LuaAddNpcEx).

4 cho: YDBZ_add_npc (quai ai) / YDBZ_add_npcboss (boss nhanh) /
YDBZ_add_final_npc (Luong Mi Nhi) / YDBZ_AddNpc_YuanJun (vien quan - cho nay
camp duoc SetTmpCamp dat lai ngay sau nen khong doi hanh vi).

KHONG build (DLL dang chay DA co HD3_AddNpcEx - da kiem). Can KHOI DONG LAI
GameServer de nap lai script (bao chu, CAM tu restart).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_camp lan dau).
"""
import io
import os
import re
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\missions\yandibaozang\npc.lua")
HAU_TO = ".truoc_camp"
NHAN = "[CAMP 29/08]"

# chen sau dong Include cuoi cung o dau tep
KHOI = [
    "",
    "-- " + NHAN + " QUAI PHAI THU DICH NGUOI CHOI.",
    "-- Ban Linux: tham so 7 cua AddNpcEx = bNoRevive (chet la bien mat).",
    "-- Ban JX1: LuaAddNpcEx (ScriptFuns.cpp:7055) lay tham so 7 lam CAMP va ghi de",
    "-- camp cua npcs.txt => quai Viem De (Camp=5 thu dich) bi ep ve camp 1 = chinh",
    "-- phai = CUNG PHE voi to doi 1 (ready.lua SetTmpCamp theo so doi) => quai dung",
    "-- yen khong danh. Doi port 3 hoat dong 25/08 da lam ham HD3_AddNpcEx giu dung",
    "-- ngu nghia Linux (khong dung camp, tham so 7 = bNoRevive) - dung lai no.",
    "if HD3_AddNpcEx ~= nil then",
    "\tYDBZ_AddNpcEx = HD3_AddNpcEx",
    "else",
    "\tYDBZ_AddNpcEx = AddNpcEx",
    "end",
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
    print("=== v24_npc_camp - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA VA - bo qua (idempotent)")
        return 0

    d = raw.split(eol)

    # 1) doi 4 cho goi THAT (dong ket thuc bang "AddNpcEx(" - khong dinh chuoi log)
    vt = [i for i, l in enumerate(d) if l.rstrip().endswith("= AddNpcEx(")]
    if len(vt) != 4:
        print("!!! LOI TO: thay %d cho goi AddNpcEx( (can 4)" % len(vt))
        for i in vt:
            print("    dong %d: %r" % (i + 1, d[i][:80]))
        return 1
    for i in vt:
        d[i] = d[i].replace("= AddNpcEx(", "= YDBZ_AddNpcEx(")
    print("  ok  doi 4 cho goi -> YDBZ_AddNpcEx (dong %s)"
          % ", ".join(str(i + 1) for i in vt))

    # 2) chen khoi bi danh sau dong Include CUOI CUNG o dau tep
    inc = [i for i, l in enumerate(d[:40]) if l.strip().startswith("Include(")]
    if not inc:
        print("!!! LOI TO: khong thay dong Include nao o dau tep")
        return 1
    chen = inc[-1] + 1
    d[chen:chen] = KHOI
    print("  ok  chen khoi bi danh sau dong Include cuoi (dong %d)" % (inc[-1] + 1))

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    mo, dong = can_bang_token(nd)
    mo0, dong0 = can_bang_token(raw)
    if (mo - dong) != (mo0 - dong0):
        print("!!! LOI TO: can bang token doi: truoc %+d sau %+d" % (mo0 - dong0, mo - dong))
        return 1
    # chot: khong con loi goi AddNpcEx( TRUC TIEP nao (tru trong khoi bi danh + log)
    con = 0
    for l in nd.split(eol):
        s = re.sub(r'"[^"]*"', '""', l)   # bo CHUOI (3 cho log 'Failed to AddNpcEx(...)')
        j = s.find("--")                   # bo chu thich
        if j >= 0:
            s = s[:j]
        if re.search(r"(?<!YDBZ_)(?<!HD3_)\bAddNpcEx\s*\(", s) and "YDBZ_AddNpcEx =" not in l:
            con += 1
    if con:
        print("!!! LOI TO: con %d cho goi AddNpcEx truc tiep" % con)
        return 1
    print("  chot: token %d/%d, byte cao %d, 0 cho goi AddNpcEx truc tiep" % (mo, dong, hi0))

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
