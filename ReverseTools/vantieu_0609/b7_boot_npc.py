# -*- coding: ascii -*-
"""b7_boot_npc.py -- [LMBC 06/09] BUOC B7: sinh BO KHOI TAO NPC van tieu.

VI SAO CAN: khung activitysys cua JX1 DANG CHET - G_ACTIVITY:AddActivity va LoadActivitys
co dinh nghia nhung KHONG CHO NAO GOI, va JX1 khong co autoexec.lua. Nghia la chep du
15 tep config\\129 van khong NPC nao xuat hien, va TRIEU CHUNG LA "khong loi gi ca nhung
khong thay NPC" - rat kho truy.

Cach lam theo dung loi nha JX1: mot tep addnpc rieng, duoc startgame.lua Include va goi
(khuon: script\\task\\tollgate\\tinsu_addnpc.lua, script\\missions\\bw\\bw_addnpc.lua).

Tep sinh ra: serverscript_jx2\\vantieu\\moi\\script\\startgame\\lmbj_addnpc.lua
  lmbj_addnpc()       goi ca hai nhanh
  lmbj_addnpc_bang()  NPC nhanh BANG  : 2230 (7 diem) + 2231 (23 diem)
  lmbj_addnpc_canhan() NPC nhanh CA NHAN: 2145 tieu su (14 diem) + 2157 chuong quay (1 diem)
Tach ham de bat/tat tung nhanh khong phai sua bang toa do.

Toa do chep NGUYEN VAN tu ban Linux:
  config\\129\\variables.lua:12-27 (2230, 2231) va :4, :8 (2157, 2145)
Ten NPC lay tu chinh npcs.txt vua ghi (b5a) de chac chan khop tung byte.

dung: python b7_boot_npc.py [<goc worktree>]
"""
import importlib.util
import io
import os
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_vantieu"
MOI = os.path.join(ROOT, "serverscript_jx2", "vantieu", "moi")
NPCS = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\npcs.txt"

spec = importlib.util.spec_from_file_location(
    "v2o", os.path.join(os.path.expanduser("~"), ".claude", "skills",
                        "swordonline-dev", "scripts", "vn_to_octal.py"))
v2o = importlib.util.module_from_spec(spec)
spec.loader.exec_module(v2o)


def ten_npc(nid):
    """Lay ten NPC tu npcs.txt cua cay chay that (dong 1-based = id + 2)."""
    d = io.open(NPCS, encoding="latin-1", newline="").read().split("\r\n")
    row = d[nid + 1].split("\t")
    ten = row[0]
    assert ten and ten != "0", "hang id %d con rong - chay b5a_bang_npc.py truoc" % nid
    return ten


# toa do: {mapID, x_o, y_o}
CONSIGNER = [(80, 1678, 3147), (176, 1399, 3140), (37, 1703, 3019), (11, 3205, 5078),
             (78, 1569, 3280), (1, 1559, 3258), (162, 1650, 3247)]
RECEIVER = [(116, 1741, 3031), (93, 1620, 3182), (94, 1638, 3106),
            (181, 1627, 3052), (180, 1548, 3176), (182, 2107, 3412),
            (42, 1642, 3029), (45, 1702, 3076), (204, 1621, 3375), (103, 1638, 2930),
            (10, 1778, 3065), (23, 1854, 3123), (22, 1865, 3274), (5, 1597, 3592),
            (91, 1602, 2810), (83, 1660, 2924), (205, 1587, 3221),
            (201, 1736, 3235), (2, 2406, 3713), (4, 1672, 3096),
            (168, 1653, 3138), (171, 1494, 2982), (203, 1505, 3102)]
BIAOSHI = [(176, 1448, 3335), (176, 1534, 2974), (11, 3061, 4992), (78, 1537, 3182),
           (1, 1597, 3093), (162, 1639, 3215), (37, 1690, 3150), (80, 1786, 3037),
           (20, 3516, 6164), (99, 1673, 3229), (100, 1622, 3113), (101, 1625, 3209),
           (153, 1677, 3240), (174, 1596, 3271)]
ZHANGGUI = [(121, 1989, 4476)]


def bang(name, ds):
    L = ["%s = {" % name]
    for m, x, y in ds:
        L.append("\t{%d, %d, %d}," % (m, x, y))
    L.append("}")
    return L


def main():
    bs = chr(92)          # dau gach cheo nguoc - KHONG go truc tiep (bay heredoc)
    dd = bs * 2           # trong chuoi Lua: "\\script\\..."

    t2230 = ten_npc(2230)
    t2231 = ten_npc(2231)
    t2145 = ten_npc(2145)
    t2157 = ten_npc(2157)
    print("ten doc tu npcs.txt: 2230=%r 2231=%r 2145=%r 2157=%r"
          % (t2230, t2231, t2145, t2157))

    P_CONSIGNER = '"' + dd + 'script' + dd + 'activitysys' + dd + 'config' + dd + '129' + dd + 'npc_consigner.lua"'
    P_RECEIVER = '"' + dd + 'script' + dd + 'activitysys' + dd + 'config' + dd + '129' + dd + 'npc_receiver.lua"'
    P_TASKNPC = '"' + dd + 'script' + dd + 'event' + dd + 'longmenbiaoju' + dd + 'tasknpc.lua"'
    I_NPCFUN = 'Include("' + dd + 'script' + dd + 'activitysys' + dd + 'npcfunlib.lua")'

    L = []
    A = L.append
    A("-- ============================================================")
    A("-- [LMBC 06/09] KHOI TAO NPC LONG MON TIEU CUC (van tieu).")
    A("--")
    A("-- VI SAO CAN TEP NAY: khung activitysys cua JX1 dang CHET - G_ACTIVITY:AddActivity")
    A("-- va LoadActivitys co dinh nghia nhung KHONG CHO NAO GOI, va JX1 khong co autoexec.lua.")
    A("-- Chep du 15 tep config" + bs + "129 van se KHONG NPC NAO XUAT HIEN, va trieu chung la")
    A("-- 'khong loi gi ca nhung khong thay NPC' - rat kho truy. Nen sinh NPC o day, theo dung")
    A("-- loi nha JX1 (khuon script" + bs + "task" + bs + "tollgate" + bs + "tinsu_addnpc.lua).")
    A("--")
    A("-- Toa do chep nguyen van tu ban Linux config" + bs + "129" + bs + "variables.lua:4,8,12-27.")
    A("-- Ten NPC doc tu chinh npcs.txt nen khop tung byte voi bang.")
    A("-- startgame.lua goi lmbj_addnpc().")
    A("-- ============================================================")
    A(I_NPCFUN)
    A("")
    A('LMBJ_TEN_CONSIGNER = "%s"' % t2230)
    A('LMBJ_TEN_RECEIVER  = "%s"' % t2231)
    A('LMBJ_TEN_BIAOSHI   = "%s"' % t2145)
    A('LMBJ_TEN_ZHANGGUI  = "%s"' % t2157)
    A("")
    A("-- {mapID, o x, o y}")
    L += bang("LMBJ_POS_CONSIGNER", CONSIGNER)
    A("")
    L += bang("LMBJ_POS_RECEIVER", RECEIVER)
    A("")
    L += bang("LMBJ_POS_BIAOSHI", BIAOSHI)
    A("")
    L += bang("LMBJ_POS_ZHANGGUI", ZHANGGUI)
    A("")
    A("-- Nhanh BANG (ap Tieu Bang 10 sao): NPC tiep dan + NPC nhan hang.")
    A("function lmbj_addnpc_bang()")
    A("\tNpcFunLib:AddObjNpc(LMBJ_TEN_CONSIGNER, 2230, LMBJ_POS_CONSIGNER, %s)" % P_CONSIGNER)
    A("\tNpcFunLib:AddObjNpc(LMBJ_TEN_RECEIVER,  2231, LMBJ_POS_RECEIVER,  %s)" % P_RECEIVER)
    A("end")
    A("")
    A("-- Nhanh CA NHAN (1-9 sao): Long Mon Tieu Su + ong chu tieu cuc.")
    A("-- Ca hai deu dung chung tasknpc.lua cua ban Linux.")
    A("function lmbj_addnpc_canhan()")
    A("\tNpcFunLib:AddObjNpc(LMBJ_TEN_BIAOSHI,  2145, LMBJ_POS_BIAOSHI,  %s)" % P_TASKNPC)
    A("\tNpcFunLib:AddObjNpc(LMBJ_TEN_ZHANGGUI, 2157, LMBJ_POS_ZHANGGUI, %s)" % P_TASKNPC)
    A("end")
    A("")
    A("-- Bat/tat tung nhanh o day. De 0 la nhanh do khong sinh NPC nao.")
    A("LMBJ_BAT_BANG   = 1")
    A("LMBJ_BAT_CANHAN = 1")
    A("")
    A("function lmbj_addnpc()")
    A("\tif (LMBJ_BAT_BANG == 1) then")
    A("\t\tlmbj_addnpc_bang()")
    A("\tend")
    A("\tif (LMBJ_BAT_CANHAN == 1) then")
    A("\t\tlmbj_addnpc_canhan()")
    A("\tend")
    A("end")
    A("")

    out = os.path.join(MOI, "script", "startgame", "lmbj_addnpc.lua")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    io.open(out, "w", encoding="latin-1", newline="").write("\r\n".join(L))
    d = io.open(out, encoding="latin-1", newline="").read()
    print("da tao:", out, os.path.getsize(out), "byte | byte cao:",
          sum(1 for c in d if ord(c) >= 0x80), "| FFFD:", d.count("\ufffd"))
    print("so NPC se sinh: bang=%d, ca nhan=%d"
          % (len(CONSIGNER) + len(RECEIVER), len(BIAOSHI) + len(ZHANGGUI)))


if __name__ == "__main__":
    main()
