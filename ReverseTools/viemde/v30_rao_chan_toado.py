# -*- coding: utf-8 -*-
"""v30_rao_chan_toado.py - RAO CHAN: thieu tep toa do thi bo qua 1 ai, KHONG sap ca tran.

CAN CU (loi CO THAT trong ScriptError.log luc 2026/08/29 11:46:24):
    error: bad argument #2 to `random' (interval is empty)
    stack traceback:
       1: function `random' [C]
       2: function `YDBZ_add_npc' at line 285 [npc.lua]     <- ban truoc [CAMP], = :298 nay
       3: function `YDBZ_create_all_npc' at line 453        <- = :466 nay
       4: function `YDBZ_start_match' at line 48
       5: function `OnTimer' at line 86

CHUOI NHAN QUA (da doc tan noi tung mat):
  npc.lua:289  npc_create_point = YDBZ_GetTabFileHeight(file_name)
  include.lua:167-173  ham do: TabFile_Load that bai -> `return 0`
     (TabFile_Load -> ScriptFuns.cpp:3125 -> KTabFile.cpp:58 File.Open;
      KPakFile::Open thu DIA truoc, thu muc settings khong nam trong pak
      => THIEU TEP TREN DIA = Load that bai)
  npc.lua:291-295  0 < 60 => nlistsum = npc_count = 60
  npc.lua:296-297  i=1 > 0 dung
  npc.lua:298      random(1, 0)  => NO

VI SAO LUC DO THIEU TEP: thu muc settings\\maps\\yandibaozang duoc chep sang cay
chay luc 11:58:37 (v22) - SAU khi loi xay ra 12 phut. Bay gio da du 31 tep toa do
+ 62 tep bay, nen loi KHONG con tai dien. Nhung con nguyen NGUY CO: chi can mot
tep thieu la SAP CA TRAN (huy sach YDBZ_create_all_npc, khong to nao co quai,
4 boss trung tam :469-472 cung khong chay) - dung nhu trieu chung chu bao
"vao map khong hien npc".

MIENG VA (toi thieu, KHONG doi can bang):
 A) npc.lua sau :289 - npc_create_point <= 0 thi ghi log roi `return`, bo qua
    DUNG MOT ai. Khi du lieu dung, npc_create_point = 50 (hoac 10 o wayfinial)
    nen dieu kien LUON SAI => luong chay y het truoc khi va.
 B) npc.lua sau :283 (`item = YDBZ_map_npcBossEx[nteams][nway]`) - item nil thi
    return. Bang do chi khai [nteams][3] va [nteams][6]; hien moi noi goi deu
    co rao (npc_death.lua:242, test_hoatdong_admin.lua:312) nen KHONG doi hanh
    vi, chi chan bay cho nguoi sua sau.

KHONG dong toi include.lua:170 (`return 0`): doi thanh 1 se khien
YDBZ_bt_addtrap/addZhangai doc o rong va dat bay o toa do nil, lai con che mat
trieu chung thieu du lieu.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_raochan lan dau).
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
     r"\script\missions\yandibaozang\npc.lua")
HAU_TO = ".truoc_raochan"
NHAN = "[RAOCHAN 29/08]"
T = "\t"

VA = [
    dict(
        ten="A - thieu tep toa do thi bo qua 1 ai",
        cu=[T*2 + "local npc_create_point = YDBZ_GetTabFileHeight(file_name)"],
        moi=[
            T*2 + "local npc_create_point = YDBZ_GetTabFileHeight(file_name)",
            T*2 + "-- " + NHAN + " YDBZ_GetTabFileHeight tra 0 khi KHONG mo duoc tep",
            T*2 + "-- (include.lua:167-173). Truoc day roi thang xuong random(1,0)",
            T*2 + "-- ben duoi va NEM LOI, huy sach ca tran: khong to nao co quai,",
            T*2 + "-- 4 boss trung tam cung khong sinh. Nay bo qua DUNG MOT ai.",
            T*2 + "-- Du lieu dung thi so nay la 50 (hoac 10 o wayfinial) nen nhanh",
            T*2 + "-- duoi day KHONG BAO GIO chay - ti le/vi tri giu nguyen.",
            T*2 + "if (npc_create_point <= 0) then",
            T*3 + 'print("[VIEMDE] thieu hoac khong doc duoc tep toa do: "'
                  '..file_name)',
            T*3 + "return",
            T*2 + "end",
        ],
    ),
    dict(
        ten="B - boss ai nil thi bo qua",
        cu=[
            T*2 + "if nstate == 21 then",
            T*3 + "item = YDBZ_map_npcBossEx[nteams][nway]",
            T*2 + "end",
        ],
        moi=[
            T*2 + "if nstate == 21 then",
            T*3 + "item = YDBZ_map_npcBossEx[nteams][nway]",
            T*3 + "-- " + NHAN + " bang do chi khai [nteams][3] va [nteams][6];",
            T*3 + "-- goi voi nway khac se cho nil roi no o YDBZ_get_npc_count.",
            T*3 + "-- Moi noi goi hien nay deu da co rao nen KHONG doi hanh vi.",
            T*3 + "if (item == nil) then",
            T*4 + 'print("[VIEMDE] khong co boss ai cho to "..nteams..'
                  '" ai "..nway)',
            T*4 + "return",
            T*3 + "end",
            T*2 + "end",
        ],
    ),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang_tu_khoa(s):
    """Lua 4: so `end` phai bang function+then+do - elseif."""
    import re
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v30_rao_chan_toado - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    if not os.path.isfile(P):
        print("!!! LOI TO: thieu %s" % P)
        return 1
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang_tu_khoa(raw)

    nd = raw
    for v in VA:
        kcu = eol.join(v["cu"])
        n = nd.count(kcu)
        if n != 1:
            print("!!! LOI TO: %s - moc khop %d lan (can 1)" % (v["ten"], n))
            print("    moc:\n%s" % kcu.replace("\t", "->"))
            return 1
        nd = nd.replace(kcu, eol.join(v["moi"]))
        print("  %s" % v["ten"])

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    cb1 = can_bang_tu_khoa(nd)
    if cb1 != cb0:
        print("!!! LOI TO: can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return 1
    print("  can bang tu khoa Lua: %d (khong doi)" % cb1)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI npc.lua")
    print("\nScript nap luc chay - can KHOI DONG LAI GameServer (bao chu).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
