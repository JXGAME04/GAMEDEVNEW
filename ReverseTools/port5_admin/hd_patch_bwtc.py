# -*- coding: utf-8 -*-
r"""[HD CONFIG 24/08] Phan 2: noi BW + TONGCASTLE + item thuong vao config trung tam.
Kem va bug tiem an: treedeath.lua thieu JOIN_TONG_TIME (dung %JOIN_TONG_TIME = nil -> loi khi cay chet).
Latin-1, giu CRLF/LF, idempotent.
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MBW = r"D:\GAMEDEVNEW\serverscript_jx2\bw\jx1_edits"
MTC = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle\jx1_edits"
MTW = r"D:\GAMEDEVNEW\serverscript_jx2\tongwar\jx1_edits"

INC = 'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")'

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def nl(s): return "\r\n" if "\r\n" in s else "\n"
def mir(p_abs, mroot, rel):
    d = os.path.dirname(os.path.join(mroot, rel))
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(p_abs, os.path.join(mroot, rel))

def prepend_inc(s, tag):
    if "cauhinh_hoatdong" in s: return s
    return INC + "\t-- [HD CONFIG 24/08] " + tag + nl(s) + s

def sub1(s, old, new, tag):
    assert s.count(old) == 1, "anchor '%s' = %d" % (tag, s.count(old))
    return s.replace(old, new, 1)

def suball(s, old, new, n, tag):
    assert s.count(old) == n, "anchor '%s' = %d (mong %d)" % (tag, s.count(old), n)
    return s.replace(old, new)

def bsub(s, oldb, newb, n, tag):
    b = s.encode("latin-1")
    assert b.count(oldb) == n, "byte-anchor '%s' = %d (mong %d)" % (tag, b.count(oldb), n)
    return b.replace(oldb, newb).decode("latin-1")

# ============ 1) bw\bwhead.lua ============
p = os.path.join(E, r"script\missions\bw\bwhead.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "nhip tran Ty Vo chinh duoc (RESTART)")
    s = sub1(s, "TIMER_2 = 12 * 3 * TIMER_1 ;",
             'TIMER_2 = HD_CFG("BW_PHUT_TRAN", 12) * 3 * TIMER_1 ;', "timer2")
    s = sub1(s, "GO_TIME = 6;",
             'GO_TIME = HD_CFG("BW_PHUT_CHO", 2) * 3;', "gotime")
    wr(p, s); print("bwhead.lua OK")
else:
    print("bwhead.lua da co")
mir(p, MBW, r"script\missions\bw\bwhead.lua")

# ============ 2) bw\bwmanager.lua ============
p = os.path.join(E, r"script\missions\bw\bwmanager.lua")
s = rd(p)
if "HD_CFG" not in s:
    # khong prepend Include: bwmanager Include bwhead (da co cauhinh)
    s = suball(s, "GetLevel() < 90", 'GetLevel() < HD_CFG("BW_CAP_TOITHIEU", 90)', 3, "lv3")
    s = bsub(s, "p 90 tr".encode("latin-1"),
             ('p "..HD_CFG("BW_CAP_TOITHIEU", 90).." tr').encode("latin-1"), 3, "text3")
    s = sub1(s, "if (MemberCount <= 0 or MemberCount > 8) then",
             'if (MemberCount <= 0 or MemberCount > HD_CFG("BW_DOI_TOIDA", 8)) then', "doitoida")
    wr(p, s); print("bwmanager.lua OK")
else:
    print("bwmanager.lua da co")
mir(p, MBW, r"script\missions\bw\bwmanager.lua")

# ============ 3) tongcastle\tongcastle.lua (ban chinh) ============
p = os.path.join(E, r"script\missions\tongcastle\tongcastle.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "lich / diem Thanh Bao chinh duoc")
    s = suball(s, "tbOpenDay = {[0]=1},", "tbOpenDay = HD_TC_Ngay(1),", 3, "openday1")
    s = sub1(s, "tbOpenDay = {[0]=176},", "tbOpenDay = HD_TC_Ngay(176),", "opendayc")
    s = suball(s, "tbStartTime = {17,00},", 'tbStartTime = HD_CFG("TC_GIO_MO", {17,00}),', 2, "start17")
    s = sub1(s, "tbStartTime = {18,00},", 'tbStartTime = HD_CFG("TC_GIO_CAY2", {18,00}),', "start18")
    s = sub1(s, "tbStartTime = {18,30},", 'tbStartTime = HD_CFG("TC_GIO_CAY3", {18,30}),', "start1830")
    s = suball(s, "tbEndTime = {19,00},", 'tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),', 4, "end19")
    s = sub1(s, "TongCastle.GUARDLIMIT = 100",
             'TongCastle.GUARDLIMIT = HD_CFG("TC_TRAN_THUVE", 100)', "guardlimit")
    s = sub1(s, "TC_JX1_JOINTONGTIME = 1440",
             'TC_JX1_JOINTONGTIME = HD_CFG("TC_VAO_BANG_PHUT", 1440)', "join")
    s = sub1(s, "local tbNpc, nCount = GetAroundNpcList(15)",
             'local tbNpc, nCount = GetAroundNpcList(HD_CFG("TC_BANKINH_BUA", 15))', "radius")
    wr(p, s); print("tongcastle.lua (chinh) OK")
else:
    print("tongcastle.lua (chinh) da co")
mir(p, MTC, r"script\missions\tongcastle\tongcastle.lua")

# ============ 4) mission\tongcastle\tongcastle.lua (relay) ============
p = os.path.join(E, r"script\mission\tongcastle\tongcastle.lua")
s = rd(p)
if "HD_CFG" not in s:
    NL = nl(s)
    s = prepend_inc(s, "lich cay / diem cay chinh duoc (relay)")
    s = suball(s, "tbOpenDay = {[0]=1},", "tbOpenDay = HD_TC_Ngay(1),", 3, "r-openday")
    s = sub1(s, "tbStartTime = {17,00},", 'tbStartTime = HD_CFG("TC_GIO_MO", {17,00}),', "r-17")
    s = sub1(s, "tbStartTime = {18,00},", 'tbStartTime = HD_CFG("TC_GIO_CAY2", {18,00}),', "r-18")
    s = sub1(s, "tbStartTime = {18,30},", 'tbStartTime = HD_CFG("TC_GIO_CAY3", {18,30}),', "r-1830")
    s = suball(s, "tbEndTime = {19,00},", 'tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),', 3, "r-end")
    old = "tbS3TongCastle.SAVE_KEY"
    ins = ("-- [HD CONFIG 24/08] de bang diem cay tu config (thay ca bang neu co)" + NL +
           "if (tbCHD and tbCHD.TC_DIEM_CAY) then" + NL +
           "\ttbS3TongCastle.tbTreePoint = tbCHD.TC_DIEM_CAY" + NL +
           "end" + NL)
    i = s.index(old)
    s = s[:i] + ins + s[i:]
    wr(p, s); print("tongcastle.lua (relay) OK")
else:
    print("tongcastle.lua (relay) da co")
mir(p, MTC, r"script\mission\tongcastle\tongcastle.lua")

# ============ 5) tongcastle\guideperson.lua ============
p = os.path.join(E, r"script\missions\tongcastle\guideperson.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "gate / gia doi Thanh Bao chinh duoc")
    s = sub1(s, "JOIN_TONG_TIME = 60*24",
             'JOIN_TONG_TIME = HD_CFG("TC_VAO_BANG_PHUT", 1440)', "join")
    s = sub1(s, "MIN_STAY_TIME = 45",
             'MIN_STAY_TIME = HD_CFG("TC_PHUT_TOITHIEU", 45)', "stay")
    s = sub1(s, "AWARD_STATE_TIME = 1900",
             'AWARD_STATE_TIME = HD_CFG("TC_GIO_NHANDIEM_TU", 1900)', "awtu")
    s = sub1(s, "AWARD_END_TIME = 2400",
             'AWARD_END_TIME = HD_CFG("TC_GIO_NHANDIEM_DEN", 2400)', "awden")
    s = sub1(s, "if (GetLevel() < 90) then",
             'if (GetLevel() < HD_CFG("TC_CAP_TOITHIEU", 90)) then', "lv")
    s = bsub(s, "p 90 tr".encode("latin-1"),
             ('p "..HD_CFG("TC_CAP_TOITHIEU", 90).." tr').encode("latin-1"), 1, "lvtext")
    s = sub1(s, "nId = 3205, nPrice = 10,", 'nId = 3205, nPrice = HD_CFG("TC_GIA_LENH_1", 10),', "gia1")
    s = sub1(s, "nId = 3206, nPrice = 20,", 'nId = 3206, nPrice = HD_CFG("TC_GIA_LENH_2", 20),', "gia2")
    s = sub1(s, "nId = 3207, nPrice = 120,", 'nId = 3207, nPrice = HD_CFG("TC_GIA_LENH_3", 120),', "gia3")
    s = sub1(s, "local nPrice = 200",
             'local nPrice = HD_CFG("TC_GIA_BUA", 200)', "giabua")
    s = sub1(s, 'g_AskClientNumberEx(1, 100, "Nh',
             'g_AskClientNumberEx(1, HD_CFG("TC_MUA_BUA_TOIDA", 100), "Nh', "muabua")
    s = sub1(s, "local nMaxCount = 100",
             'local nMaxCount = HD_CFG("TC_DOI_LENH_TOIDA", 100)', "doilenh")
    s = bsub(s, "c 19:00 ".encode("latin-1"),
             ('c "..HD_GioPhut(HD_CFG("TC_GIO_NHANDIEM_TU", 1900)).." ').encode("latin-1"), 1, "awtext-tu")
    # anchor an ca dau cham cuoi cau; chuoi thay ket thuc bang ..". de dau ") goc dong chuoi
    s = bsub(s, "n 24:00.".encode("latin-1"),
             ('n "..HD_GioPhut(HD_CFG("TC_GIO_NHANDIEM_DEN", 2400))..".').encode("latin-1"), 1, "awtext-den")
    wr(p, s); print("guideperson.lua OK")
else:
    print("guideperson.lua da co")
mir(p, MTC, r"script\missions\tongcastle\guideperson.lua")

# ============ 6) tongcastle\treedeath.lua (kem VA BUG %JOIN_TONG_TIME nil) ============
p = os.path.join(E, r"script\missions\tongcastle\treedeath.lua")
s = rd(p)
if "JOIN_TONG_TIME =" not in s:
    NL = nl(s)
    s = prepend_inc(s, "radius + fix thieu JOIN_TONG_TIME (%upvalue nil -> loi khi cay chet)")
    old = "local tbTreePrice = {"
    assert s.count(old) == 1
    s = s.replace(old,
        'JOIN_TONG_TIME = HD_CFG("TC_VAO_BANG_PHUT", 1440)\t-- [HD CONFIG 24/08] FIX: file goc thieu -> %JOIN_TONG_TIME capture nil' + NL + NL + old, 1)
    s = sub1(s, "GetNpcAroundNpcList(nNpcIndex, 15)",
             'GetNpcAroundNpcList(nNpcIndex, HD_CFG("TC_BANKINH_BUA", 15))', "radius")
    wr(p, s); print("treedeath.lua OK (kem fix JOIN_TONG_TIME)")
else:
    print("treedeath.lua da co")
mir(p, MTC, r"script\missions\tongcastle\treedeath.lua")

# ============ 7) tongcastle\guard.lua ============
p = os.path.join(E, r"script\missions\tongcastle\guard.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "radius bua chinh duoc")
    s = sub1(s, "local tbNpc, nCount = GetAroundNpcList(15)",
             'local tbNpc, nCount = GetAroundNpcList(HD_CFG("TC_BANKINH_BUA", 15))', "radius")
    wr(p, s); print("guard.lua OK")
else:
    print("guard.lua da co")
mir(p, MTC, r"script\missions\tongcastle\guard.lua")

# ============ 8) tongcastle\shenmuling.lua ============
p = os.path.join(E, r"script\missions\tongcastle\shenmuling.lua")
s = rd(p)
if "HD_CFG" not in s:
    NL = nl(s)
    s = prepend_inc(s, "thuong Than Moc Lenh chinh duoc")
    s = sub1(s, "MAX_COUNT_PER_WEEK = 5",
             'MAX_COUNT_PER_WEEK = HD_CFG("TC_LENH_TUAN", 5)', "tuan")
    old = "function main(nItemIndex)"
    ins = ("-- [HD CONFIG 24/08] de bang thuong tu config (thay ca bang neu co)" + NL +
           "if (tbCHD and tbCHD.TC_THUONG_LENH) then" + NL +
           "\ttbAward = tbCHD.TC_THUONG_LENH" + NL +
           "end" + NL + NL)
    i = s.index(old)
    s = s[:i] + ins + s[i:]
    wr(p, s); print("shenmuling.lua OK")
else:
    print("shenmuling.lua da co")
mir(p, MTC, r"script\missions\tongcastle\shenmuling.lua")

# ============ 9) item\huangzhendan.lua ============
p = os.path.join(E, r"script\item\huangzhendan.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "exp Hoang Chan Don chinh duoc")
    s = suball(s, "2000000000", 'HD_CFG("HCD_EXP", 2000000000)', 3, "hcd")
    wr(p, s); print("huangzhendan.lua OK")
else:
    print("huangzhendan.lua da co")
mir(p, MTW, r"script\item\huangzhendan.lua")

# ============ 10) vng_event\item\biggoldenseed.lua ============
p = os.path.join(E, r"script\vng_event\item\biggoldenseed.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "exp Qua Dai Hoang Kim chinh duoc")
    s = bsub(s, '"200 tri'.encode("latin-1"),
             'floor(HD_CFG("QDHK_EXP", 200000000)/1e6).." tri'.encode("latin-1"), 1, "qdhk-ten")
    s = sub1(s, "nExp = 200000000}",
             'nExp = HD_CFG("QDHK_EXP", 200000000)}', "qdhk-exp")
    wr(p, s); print("biggoldenseed.lua OK")
else:
    print("biggoldenseed.lua da co")
mir(p, MTW, r"script\vng_event\item\biggoldenseed.lua")

# ============ 11) event\tongwar\headinfo.lua (bang thuong tongwar) ============
p = os.path.join(E, r"script\event\tongwar\headinfo.lua")
s = rd(p)
if "TW_THUONG" not in s:
    NL = nl(s)
    s = prepend_inc(s, "bang thuong Bang Chien chinh duoc")
    key = "TAB_AWORD_GOOD"
    i = s.index(key)
    j = s.index("{", i)
    depth = 0
    k = j
    while k < len(s):
        if s[k] == "{": depth = depth + 1
        elif s[k] == "}":
            depth = depth - 1
            if depth == 0: break
        k = k + 1
    assert depth == 0, "khong dong bang TAB_AWORD_GOOD"
    ins = (NL + "-- [HD CONFIG 24/08] de bang thuong tu config (thay ca bang neu co)" + NL +
           "if (tbCHD and tbCHD.TW_THUONG) then" + NL +
           "\tTAB_AWORD_GOOD = tbCHD.TW_THUONG" + NL +
           "end")
    s = s[:k+1] + ins + s[k+1:]
    wr(p, s); print("headinfo.lua OK")
else:
    print("headinfo.lua da co")
mir(p, MTW, r"script\event\tongwar\headinfo.lua")

print("XONG PHAN 2 (bw + tongcastle + item)")
