# -*- coding: utf-8 -*-
"""C15 - 3 viec chu bao 25/08 trua:
(1) NPC len thuyen PLD van la ban VN cu: 6 NPC cu ("Thuyen phu Nam/Bac ...") dung
    TRUNG 6 toa do voi NPC moi (toi tai dung TAB_NPCCHUCNANG) -> user click trung
    NPC cu. addnpcphonglangdo() da comment => nguon = map tinh/ton du. Xoa luc
    boot theo TEN + GIOI HAN MAP 336 (template 240-242/394-396 "Thuyen phu *"
    con dung cho ben do THON - khong duoc xoa toan cuc).
(2) "Toi gio xuat hien NPC khong danh duoc": fld_head:135 SetFightState(0) khi
    len thuyen (JX1 = THU VU KHI) + user len bang NPC CU nen KHONG o trong
    mission => khong duoc mission.lua:34 SetFightState(1) khi thuyen roi ben =>
    client mask (kind_normal + fight_none -> relation_none, KNpcSet.cpp:1558)
    => khong click-chon duoc quai. Xoa NPC cu + len dung luong Linux la het
    (mission tu keo fight=1 cho moi thanh vien).
(3) Thong bao chua giong Linux:
    - Bao danh Vuot Ai: dung chuoi goc relay challengeoftime.lua TaskContent.
    - DailyRank: dung chuoi goc challegeoftime-dailyrank.lua (co dau, "da hoan
      thanh", "phut/giay") + AddGlobalNews (JX1 co, giong Linux) thay
      AddGlobalCountNews.
    - HD3_PLD_GIO mac dinh -> DU 24 GIO (TaskList.ini dang ky ban goc interval
      60 = moi gio; cac tep _NN00 gio chan KHONG duoc dang ky).
C++ kem: HD3_DelNpcByName(szTen [, nMapID]) - tham so 2 tuy chon loc theo map.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
T = "\t"
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


def sync(rel):
    import shutil, os
    src = SRV + "\\" + rel
    dst = MIRROR + "\\" + rel
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)


# ---------- 1) C++: them loc map ----------
def f_infra(d):
    if "nLocMapID" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "const char* pTen = Lua_ValueToString(L, 1);"
    assert d.count(a) == 1
    d = d.replace(a, a + NL + NL.join([
        T + "// tham so 2 (tuy chon): CHI xoa NPC dang o map co SubWorldID nay (vd 336",
        T + "// ben Phong Lang Do) - tranh xoa nham NPC trung ten o he khac (ben do thon).",
        T + "int nLocMapID = 0;",
        T + "if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))",
        T + T + "nLocMapID = (int)Lua_ValueToNumber(L, 2);",
    ]))
    a2 = T + T + "if (strstr(Npc[nIdx].Name, pTen) == NULL)"
    assert d.count(a2) == 1
    d = d.replace(a2, NL.join([
        T + T + "if (nLocMapID != 0 && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID != nLocMapID)",
        T + T + T + "continue;",
    ]) + NL + a2)
    return d
rw(SRC + r"\KJx2WarInfra.cpp", f_infra)


# ---------- 2) hd3_driver.lua ----------
TEN_TP = V("Thuyền phu")
MSG_VA = V("Thời gian báo danh 'Thách thức thời gian' đã bắt đầu, các đội trưởng hãy mau đến Nhiếp Thí Trần ở các thành thị báo danh. Thời gian báo danh là 10 phút.")
MSG_RANK = V("Chúc mừng đội <%s> đã hoàn thành <thách đấu thời gian> với thời gian ngắn nhất <%s>")
FMT_TIME = V("%s phút %s giây")

def f_drv(d):
    NL = "\r\n" if "\r\n" in d else "\n"
    # (a) xoa NPC thuyen phu cu map 336 truoc khi sinh
    if TEN_TP not in d:
        a = T + "-- (B) Phong Lang Do: 6 thuyen phu (Linux fld_head.lua fld_wanttakeboat)."
        assert d.count(a) == 1
        d = d.replace(a, NL.join([
            T + "-- [3HD 25/08 C15] xoa 6 NPC thuyen phu CU cua ban Viet (dung TRUNG toa do",
            T + "-- voi NPC moi -> user click trung NPC cu, vao PLD ngoai mission => khong",
            T + "-- danh duoc quai vi thieu SetFightState(1) cua mission). GIOI HAN map 336:",
            T + "-- template \"Thuyen phu *\" con dung cho ben do thon o map khac.",
            T + "if (HD3_DelNpcByName ~= nil) then",
            T + T + 'local nXoaTP = HD3_DelNpcByName("' + TEN_TP + '", 336)',
            T + T + 'print("[3HD] Da xoa "..nXoaTP.." NPC thuyen phu cu (ban Viet, map 336).")',
            T + "end",
            a,
        ]))
    # (b) ten NPC moi: TCVN3
    old_add = 'HD3_AddNpc(240, 1, nIdx, t[1]*32, t[2]*32, 1, "Thuyen phu")'
    if old_add in d:
        d = d.replace(old_add, 'HD3_AddNpc(240, 1, nIdx, t[1]*32, t[2]*32, 1, "' + TEN_TP + '")')
    # (c) thong bao bao danh Vuot Ai: dung chuoi goc relay
    old_msg = 'AddLocalCountNews("Nhiem vu \'Thach thuc thoi gian\' bat dau bao danh. Cac doi truong hay den gap Nhiep Thi Tran o that dai thanh thi!", 2)'
    if old_msg in d:
        d = d.replace(old_msg, 'AddLocalCountNews("' + MSG_VA + '", 2)\t-- chuoi goc relay challengeoftime.lua')
    # (d) DailyRank: chuoi goc + AddGlobalNews
    old_t = 'local szTime = format("%s phut %s giay", floor(value/60), floor(mod(value, 60)))'
    if old_t in d:
        d = d.replace(old_t, 'local szTime = format("' + FMT_TIME + '", floor(value/60), floor(mod(value, 60)))')
    old_m = 'local szMsg = format("Chuc mung doi <%s> hoan thanh <Thach thuc thoi gian> voi thoi gian ngan nhat <%s>", name, szTime)'
    if old_m in d:
        d = d.replace(old_m, 'local szMsg = format("' + MSG_RANK + '", name, szTime)\t-- chuoi goc relay dailyrank')
    d = d.replace("AddGlobalCountNews(szMsg, 10)", "AddGlobalNews(szMsg, 10)\t-- dung ham goc Linux (JX1 co san)")
    return d
rw(SRV + r"\script\tinhnang\3hoatdong\hd3_driver.lua", f_drv)
sync(r"script\tinhnang\3hoatdong\hd3_driver.lua")


# ---------- 3) cauhinh: PLD du 24 gio ----------
def f_cfg(d):
    if "1900, 2000, 2100, 2200, 2300}," in d and "HD3_PLD_GIO = {0, 100" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    old = "HD3_PLD_GIO = {0, 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200},"
    assert d.count(old) == 1, "khong thay HD3_PLD_GIO cu"
    new = NL.join([
        "HD3_PLD_GIO = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,",
        "               1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300},",
    ])
    d = d.replace(old, new)
    # sua note cu neu khop
    d = d.replace("-- MOI GIO dung phut :00 (24 luot/ngay); mac dinh o day 12 gio chan = LECH CO",
                  "-- MOI GIO dung phut :00 (24 luot/ngay) - DUNG 100% Linux (TaskList.ini Task_86")
    d = d.replace("-- CHU DICH de nhe server - muon dung 100% Linux thi liet ke du 24 gio. [RESTART]",
                  "-- dang ky ban interval 60; cac tep _NN00 gio chan KHONG duoc dang ky). [RESTART]")
    return d
rw(SRV + r"\script\header\cauhinh_hoatdong.lua", f_cfg)
sync(r"script\header\cauhinh_hoatdong.lua")
print("xong C15 (nguon) - can build lai CoreServer x64")
