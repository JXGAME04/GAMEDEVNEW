# -*- coding: ascii -*-
"""C5 - YEU CAU 2 (A7-N3/N4/N5): noi 6 khoa "chet" vao script that + them khoa
PHAN THUONG cho ca 3 hoat dong + sua nhan/ghi chu config + thu hep tbRangeId.

Khuon override: <bien> = HD_CFG("KHOA", <mac dinh goc Linux>)  (nil = dung bang goc)
Moi tep duoc them Include cauhinh_hoatdong.lua (ASCII, re) neu chua co.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
INC = 'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")'


def edit(rel, fn):
    p = os.path.join(JX1, rel.replace("/", os.sep))
    d = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in d else "\n"
    d2 = fn(d, nl)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        m = os.path.join(MIRROR, rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(m), exist_ok=True)
        io.open(m, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", rel.rsplit("/", 1)[-1])
    else:
        print("  (khong doi)", rel)


def need_inc(d, nl, after_line_sub):
    """chen Include cauhinh sau dong chua after_line_sub (neu chua co)"""
    if "cauhinh_hoatdong" in d:
        return d
    i = d.index(after_line_sub)
    eol = d.index(nl, i) + len(nl)
    return d[:eol] + INC + "\t-- [3HD] noi cauhinh" + nl + d[eol:]


# ---------- (A) include.lua: 4 khoa VA + tbRangeId ----------
def f_include(d, nl):
    d = need_inc(d, nl, 'IncludeLib("RELAYLADDER")')
    a = "COUNT_LIMIT \t\t= 1"
    assert d.count(a) == 1
    d = d.replace(a, a + nl + nl.join([
        "-- [3HD 25/08] noi cauhinh_hoatdong.lua (khoa HD3_VA_*) - mac dinh = so goc Linux",
        'TIME_SIGNUP = HD_CFG("HD3_VA_PHUT_BAODANH", TIME_SIGNUP)',
        "LIMIT_SIGNUP = TIME_SIGNUP * 60",
        'LIMIT_FINISH = HD_CFG("HD3_VA_PHUT_NHIEMVU", 30) * 60',
        'LIMIT_PLAYER_COUNT = HD_CFG("HD3_VA_NGUOI_TOIDA", LIMIT_PLAYER_COUNT)',
        'COUNT_LIMIT = HD_CFG("HD3_VA_LUOT_NGAY", COUNT_LIMIT)',
    ]))
    # tbRangeId cao cap: JX1 da de 1032/1033/1034 = "Boss New Dragon" (he khac chiem cho),
    # khong con "(cao cap) tieu Boss nam 7/8, nu 1" -> thu hep dai
    a2 = "\t\t{1026, 1033},"
    assert d.count(a2) == 1
    d = d.replace(a2, "\t\t-- [3HD 25/08] npcs.txt JX1: 1032-1034 = Boss New Dragon (he khac da chiem" + nl +
                      "\t\t-- cho); nam 7/8 + nu 1 khong ton tai -> thu hep dai (6 nam + 3 nu)." + nl +
                      "\t\t{1026, 1031},")
    a3 = "\t\t{1034, 1037}"
    assert d.count(a3) == 1
    d = d.replace(a3, "\t\t{1035, 1037}")
    return d
edit("script/missions/challengeoftime/include.lua", f_include)

# ---------- (B) nieshichen: ST_MAX_NGAY ----------
def f_nie(d, nl):
    d = need_inc(d, nl, "npcNhiepThiTran.lua")
    a = INC + "\t-- [3HD] noi cauhinh"
    assert a in d
    d = d.replace(a, a + nl + 'KILLER_MAXCOUNT = HD_CFG("HD3_ST_MAX_NGAY", KILLER_MAXCOUNT)\t-- tran giet boss/ngay (goc Linux = 8, newtask_head.lua:20)')
    return d
edit("script/task/tollgate/killer/nieshichen.lua", f_nie)

# ---------- (C) fld_head: suc chua + gio ton phi ----------
def f_fld(d, nl):
    d = need_inc(d, nl, "misc\\\\eventsys\\\\type\\\\func.lua")
    a = "GetMSPlayerCount(MISSIONID, 1) >= 100 )"
    assert d.count(a) == 1
    d = d.replace(a, 'GetMSPlayerCount(MISSIONID, 1) >= HD_CFG("HD3_PLD_SUC_CHUA", 100) )')
    old = nl.join(["\tlocal tb_sptime = {", "\t\t[10] = 1,", "\t\t[14] = 1,",
                   "\t\t[16] = 1,", "\t\t[18] = 1,", "\t\t[20] = 1,", "\t};"])
    assert d.count(old) == 1, "tb_sptime khong khop"
    new = nl.join([
        "\t-- [3HD 25/08] gio ton phi lay tu cauhinh (HHMM) - goc Linux 10/14/16/18/20h",
        "\tlocal tb_sptime = {}",
        '\tlocal tbGioTP = HD_CFG("HD3_PLD_GIO_TONPHI", {1000, 1400, 1600, 1800, 2000})',
        "\tfor i = 1, getn(tbGioTP) do",
        "\t\ttb_sptime[floor(tbGioTP[i] / 100)] = 1",
        "\tend",
    ])
    d = d.replace(old, new)
    return d
edit("script/missions/fengling_ferry/fld_head.lua", f_fld)

# ---------- (D) kill_level: bang thuong lv90 ----------
def f_kill(d, nl):
    d = need_inc(d, nl, "awardtemplet.lua")
    a = "\t\ttbAwardTemplet:Give(tbAward, 1, {\"KillerTask\", \"FinishLevel90\"})"
    assert d.count(a) == 1
    d = d.replace(a, '\t\ttbAward = HD_CFG("HD3_ST_THUONG", nil) or tbAward\t-- nil = bang goc Linux o tren' + nl + a)
    return d
edit("script/task/tollgate/killer/kill_level.lua", f_kill)

# ---------- (E) bossdeath: so an + ti le hai long ----------
def f_bossdeath(d, nl):
    d = need_inc(d, nl, "misc\\\\eventsys\\\\type\\\\func.lua")
    a = "SIGNET_DROPCOUNT = 2"
    assert d.count(a) == 1
    d = d.replace(a, a + nl + 'SIGNET_DROPCOUNT = HD_CFG("HD3_PLD_SO_AN_BOSS", SIGNET_DROPCOUNT)\t-- so Thi Gia Chi An roi tu boss dau linh')
    a2 = "\t\tlocal nRate = 0.005;"
    assert d.count(a2) == 1
    d = d.replace(a2, '\t\tlocal nRate = HD_CFG("HD3_PLD_TILE_HAILONG", 0.005);')
    return d
edit("script/missions/fengling_ferry/bossdeath.lua", f_bossdeath)

# ---------- (F) shuizeideath: ti le truy cong ----------
def f_shuizei(d, nl):
    d = need_inc(d, nl, "misc\\\\eventsys\\\\type\\\\func.lua")
    a = "\tif nCurRate < 50 then"
    assert d.count(a) == 1
    d = d.replace(a, '\tif nCurRate < HD_CFG("HD3_PLD_TILE_TRUYCONG", 50) then')
    return d
edit("script/missions/fengling_ferry/shuizeideath.lua", f_shuizei)

# ---------- (G) mission.lua PLD: thuong cap ben ----------
def f_mission(d, nl):
    a = '"fenglingdu_shuizeicaibao", 2)'
    assert d.count(a) == 1
    d = d.replace(a, '"fenglingdu_shuizeicaibao", HD_CFG("HD3_PLD_THUONG_CAPBEN", 2))')
    return d
edit("script/missions/fengling_ferry/mission.lua", f_mission)

# ---------- (H) award.lua: bang thuong hoan thanh ----------
def f_award(d, nl):
    d = need_inc(d, nl, 'Include("\\\\script')
    a = "\t},-- Level 2" + nl + "}"
    assert d.count(a) == 1
    d = d.replace(a, a + nl + 'tbAward_Success = HD_CFG("HD3_VA_THUONG_HOANTHANH", nil) or tbAward_Success\t-- [3HD] nil = bang goc')
    return d
edit("script/missions/challengeoftime/award.lua", f_award)

# ---------- (I) rank_perday: thuong hang ngay ----------
def f_rank(d, nl):
    d = need_inc(d, nl, "awardtemplet.lua")
    a = "tbQiannianlingyao = {"
    i = d.index(a)
    eol = d.index(nl, i) + len(nl)
    d = d[:eol] + 'tbQiannianlingyao = HD_CFG("HD3_VA_THUONG_HANG_NGAY", nil) or tbQiannianlingyao\t-- [3HD] nil = Thien Nien Linh Duoc goc' + nl + d[eol:]
    return d
edit("script/missions/challengeoftime/rank_perday.lua", f_rank)

# ---------- (J) chuangguanbaoxiang: bang ruong ----------
def f_box(d, nl):
    d = need_inc(d, nl, 'Include("\\\\script')
    a = "\tlocal tbAward = tbCOT_Box_Award[strKeyType]"
    assert d.count(a) == 1
    d = d.replace(a, '\ttbCOT_Box_Award = HD_CFG("HD3_VA_THUONG_RUONG", nil) or tbCOT_Box_Award\t-- [3HD] nil = bang goc' + nl + a)
    return d
edit("script/missions/challengeoftime/item/chuangguanbaoxiang.lua", f_box)

print("C5 script hooks: xong")
