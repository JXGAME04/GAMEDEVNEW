# -*- coding: utf-8 -*-
"""Sinh he BAC DAU LENH BAI cho JX1 - dich nguoc tu ban Linux
script\\event\\beidoulingpai\\ (7 tep, 728 dong con song).

=== KET QUA DICH NGUOC (recon_tinhnang.py "event\\beidoulingpai") ===
* JX1 THIEU 3 ham -> KHONG can sua engine, viet duoc bang Lua:
    GetServerDate(fmt)   -> GetLocalDate(fmt)            (JX1 co san)
    GetItemLevel(idx)    -> gia tri thu 4 cua GetItemProp (KJx2WarInfra.cpp:771
    GetItemSeries(idx)   -> gia tri thu 5 cua GetItemProp  tra DU 6 gia tri)
* 6 task id 2999 / 4044..4048 deu TRONG o JX1 -> dung nguyen so cua Linux.
* 3 ma item chinh (3508 Lenh bai Bac Dau, 3522 Bac Dau Chi Bao, 3523 Bac Dau
  Huyet Linh Don) o JX1 DA BI CHIEM boi item khac -> cap ma moi 4865/4866/4867
  (ma 6,1,* lon nhat dang dung o JX1 la 4864).
* Chan Nguyen Don (Dai) 30229 -> 4847 (da doi chieu theo TEN, dot C44).

=== DIEM PHAI BIET ve ban Linux (do that, khong doan) ===
1) 13 "Lenh bai Bac Dau - <hoat dong>" (3509..3521) CHI DUOC PHAT, KHONG CHO NAO
   TIEU. SubmitToken10/13 (beidouactivity.lua:299-332) chi dem tbBD_Token =
   6,1,3508. Quet toan cay Linux: 3509..3521 chi xuat hien trong itemlist.lua.
   Truong nAwardExp cua chung cung CHET: tbAwardTemplet:Give phan loai theo khoa
   da dang ky (tbProp / nExp / nZhenYuan - awardtemplet.lua:50-55), khong co
   "nAwardExp".
   => port nay cho cac hoat dong phat THANG "Lenh bai Bac Dau" (ma dung de doi
   thuong) de vong kinh te khep kin. Khoa HD3_BD_LENHBAI_RIENG de sau nay muon
   lam 13 lenh bai rieng thi bat len.
2) Bang nhiem vu Linux = 31 nhiem vu "den <thanh> tim <NPC>" + 141 nhiem vu "thu
   thap N cai <item>" (taskhead.lua:38-48 tu sinh tu tbBD_ItemList).
   * Nhiem vu THU THAP can doi chieu 141 ma item sang JX1 (39 ma khac nghia,
     3 ma khong co) -> DE LAI DOT SAU, khong doan bua.
   * Nhiem vu DI TIM can moc doi thoai NPC (Linux moc qua activitysys config
     1046 - DA BI CHU THICH, tuc ban Linux cung khong chay). JX1 khong co moc
     doi thoai NPC toan cuc => port nay dat NPC "Bac Dau lao nhan" cua CHINH
     minh o 7 thanh, nhiem vu thanh "den <thanh> gap Bac Dau lao nhan" - giu
     dung loi choi di duong, khong dung toi NPC san co cua may chu.
3) He Bac Dau trong ban Linux DANG TAT: dang ky su kien nam o
   activitysys/config/1046/extend.lua:326-327 va bi chu thich. Port nay BAT no
   len bang lich rieng cua 3 hoat dong (hd3_driver).
"""
import io, os, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
THUMUC = r"script\tinhnang\3hoatdong\beidou"

# === MA ITEM ===
# JX1 DA CO SAN DU 16 vat pham Bac Dau o 4126..4141 (ai do da nhap bang item
# nhung chua co script nao dung). KHONG can tao item moi, khong can dung toi pak.
#   4126 Lenh bai Bac Dau            <- ma DUY NHAT SubmitToken dem (Linux 3508)
#   4127..4139 13 lenh bai theo hoat dong                     (Linux 3509..3521)
#   4140 Bac Dau Chi Bao   4141 Bac Dau Huyet Linh Don   (Linux 3522 / 3523)
ID_LENHBAI = 4126
ID_CHIBAO = 4140
ID_LINHDAN = 4141
ID_CNDAI = 4847        # Chan Nguyen Don (Dai) - da doi chieu ten dot C44

# lenh bai rieng tung hoat dong - dung thu tu bang item JX1, khop 1-1 voi
# tbBD_ItemList[129..141] cua Linux (itemlist.lua:132-144)
LB_HD = [
    ("phongvan1",    4127, "Tống Kim"),
    ("phongvan2",    4128, "Tống Kim"),
    ("phongvan3",    4129, "Tống Kim"),
    ("phongvan4",    4130, "Tống Kim"),
    ("vuotai1",      4131, "Vượt ải"),
    ("vuotai2",      4132, "Vượt ải"),
    ("viemde1",      4133, "Viêm Đế"),
    ("viemde2",      4134, "Viêm Đế"),
    ("phonglangdo1", 4135, "Phong Lăng Độ"),
    ("phonglangdo2", 4136, "Phong Lăng Độ"),
    ("tinsu",        4137, "Tín Sứ"),
    ("satthu",       4138, "Boss sát thủ"),
    ("thienloc",     4139, "Thiên Lộc Phúc"),
]
ID_TRUYCONG = 2024     # Truy Cong Lenh cua JX1 (Linux 2015)

# 7 thanh da co NPC 769 Nhiep Thi Tran (autoexec_npc_hd3.lua) - dung lai toa do,
# doi 6 o de hai NPC khong chong nhau.
THANH = [
    ("Thành Đô",    11, 3210 + 6, 4974),
    ("Phượng Tường", 1, 1506 + 6, 3198),
    ("Biện Kinh",   37, 1647 + 6, 3050),
    ("Lâm An",     176, 1372 + 6, 3010),
    ("Đại Lý",     162, 1573 + 6, 3227),
    ("Tương Dương", 78, 1512 + 6, 3206),
    ("Dương Châu",  80, 1700 + 6, 2963),
]


def ghi(rel, dong):
    body = "\r\n".join(dong)
    p = os.path.join(SRV, rel)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(body)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    print("   sinh %-56s %4d dong" % (rel, len(dong)))


BANNER = [
    "-- ============================================================================",
    "-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.",
    "-- Dich nguoc tu ban Linux script\\event\\beidoulingpai\\ (xem dau tep gen).",
    "-- ============================================================================",
]

# ---------------------------------------------------------------- bd_lang.lua
# nguyen van lang.lua ban Linux
LANG = [
    ("MSG_FINISH_TASK", "Hoàn thành nhiệm vụ Bắc Đẩu lão nhân giao cho."),
    ("MSG_WANT_TASK_OPT", "Nhận nhiệm vụ"),
    ("MSG_SUBMIT_TASK_OPT", "Nộp nhiệm vụ"),
    ("MSG_CANCEL_TASK_OPT", "Hủy bỏ nhiệm vụ"),
    ("MSG_QUERY_TASK_OPT", "Kiểm tra nhiệm vụ đã nhận"),
    ("MSG_END_DIALOG", " Kết thúc đối thoại!"),
    ("MSG_TITLE", "Xin chào!"),
    ("MSG_ERR_NO_TASK", "Hiện tại bạn vẫn chưa nhận nhiệm vụ"),
    ("MSG_CURR_TASK", "Nhiệm vụ hiện tại là: %s"),
    ("MSG_DOUBLE_AWARD", "Phần thưởng gấp đôi"),
    ("MSG_ALREADY_HAVE_TASK", "Đã nhận nhiệm vụ Bắc Đẩu lão nhân rồi"),
    ("MSG_ERR_TASK_DAY_LIMIT", "Mỗi ngày nhận nhiều nhất %d lần"),
    ("MSG_GET_TASK", "Nhận được nhiệm vụ: %s"),
    ("MSG_TASK_FAIL", "Thời gian hoàn thành nhiệm vụ đã hết, nhiệm vụ thất bại!"),
    ("MSG_NO_FINISH_TASK", "Bạn dường như vẫn chưa hoàn thành nhiệm vụ."),
    ("MSG_CANCEL_TASK", "Hủy nhiệm vụ Bắc Đẩu lão nhân: %s."),
    ("MSG_ERR_NO_ENOUGH_COUNT", "Số lượng %s trên người bạn không đủ"),
    ("MSG_ALREADY_DOUBLE_AWARD", "Bạn đã có hiệu ứng phần thưởng gấp đôi rồi, không nên lãng phí Linh Đơn"),
    ("MSG_DOUBLE_AWARD_TIP", "Lần sau nhận nhiệm vụ hưởng hiệu ứng phần thưởng gấp đôi"),
    ("MSG_NO_OPEN", "Chưa đến thời gian mở hoạt động"),
    ("MSG_BEIDOUZHIBAO", "Đại hiệp sử dụng Bắc Đẩu Chi Bảo phút chốc hoàn thành nhiệm vụ!"),
    ("MSG_LEVEL_LIMIT", "Chỉ những bằng hữu cấp 150 trở lên hoặc đã trùng sinh mới được tham gia"),
]
L = list(BANNER)
L.append("-- lang.lua ban Linux - giu nguyen tung cau chu.")
for k, v in LANG:
    L.append('%s = "%s"' % (k, V(v)))
L.append('BD_MSG_GET_AWARD = "%s"' % V("Nộp %d lệnh bài Bắc Đẩu nhận thưởng"))
L.append('BD_MSG_TASK_CITY = "%s"' % V("Đến %s gặp Bắc Đẩu lão nhân"))
L.append('BD_MSG_WRONG_CITY = "%s"' % V("Nhiệm vụ của ngươi là %s, đây không phải nơi đó."))
L.append("")
ghi(os.path.join(THUMUC, "bd_lang.lua"), L)

# ---------------------------------------------------------------- bd_head.lua
L = list(BANNER)
L += [
    'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")',
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_lang.lua")',
    "",
    "tbBeidou = tbBeidou or {}",
    'tbBeidou.LOG_TITLE = "beidou Activity"',
    "",
    "-- head.lua ban Linux - GIU NGUYEN moi so:",
    "tbBeidou.nStartHour = 8",
    "tbBeidou.nEndHour = 22",
    "tbBeidou.MIN_LEVEL = 150",
    "tbBeidou.PER_DAY_MAX_TSK_COUNT = 30",
    "tbBeidou.ONE_HOUR = 3600",
    "tbBeidou.TOKEN_NUM_AWARD10 = 15",
    "tbBeidou.TOKEN_NUM_AWARD13 = 20",
    "",
    "-- task id cua Linux - da kiem tra CA 6 deu trong o JX1",
    "tbBeidou.TSK_ACCEPT_TIME = 4044",
    "tbBeidou.TSK_TASK_STATE  = 4045",
    "tbBeidou.TSK_BIT_TASK_ID = 1",
    "tbBeidou.TSK_BIT_ID_LEN = 29",
    "tbBeidou.TSK_BIT_TASK_DONE = 30",
    "tbBeidou.TSK_AWARD_STATE = 4046",
    "tbBeidou.TSK_BIT_DOUBLE_AWARD = 1",
    "tbBeidou.TSK_BIT_XUELINGDAN = 2",
    "tbBeidou.TSK_HOUR_COUNT = 4047",
    "tbBeidou.TSK_FINISH_COUNT = 4048",
    "tbBeidou.TSK_LIMIT_TIMES = 2999",
    "",
    "-- 3 item MOI cua JX1 (ma Linux 3508/3522/3523 da bi item khac chiem)",
    "tbBeidou.ITEM_LENHBAI = {szName = \"%s\", tbProp = {6,1,%d,1,0,0}, nBindState = -2}" % (V("Lệnh bài Bắc Đẩu"), ID_LENHBAI),
    "tbBeidou.ITEM_CNDAI   = {szName = \"%s\", tbProp = {6,1,%d,0,0,0}, nCount = 1, nBindState = -2}" % (V("Chân Nguyên Đơn (Đại)"), ID_CNDAI),
    "",
    "-- thuong doi lenh bai - head.lua ban Linux (tbAward10 / tbAward13)",
    "tbBeidou.tbAward10 =",
    "{",
    "\t{nExp = 50000000},",
    "\t{tbProp = {6,1,%d,1,0,0}, nBindState = -2, nCount = 15}," % ID_CNDAI,
    "}",
    "tbBeidou.tbAward13 =",
    "{",
    "\t{nExp = 80000000},",
    "\t{tbProp = {6,1,%d,1,0,0}, nBindState = -2, nCount = 20}," % ID_CNDAI,
    "}",
    "",
    "-- 7 thanh dat NPC Bac Dau lao nhan: {ten thanh, mapId, x, y}",
    "tbBeidou.THANH =",
    "{",
]
for ten, mid, x, y in THANH:
    L.append('\t{"%s", %d, %d, %d},' % (V(ten), mid, x, y))
L += [
    "}",
    "",
    "-- 13 lenh bai RIENG cua tung hoat dong (Linux itemlist.lua:132-144).",
    "-- {ma item JX1, ten hoat dong de hien thong bao}",
    "tbBeidou.LENHBAI_HD =",
    "{",
]
for k, pid, t in LB_HD:
    L.append('\t["%s"] = {%d, "%s"},' % (k, pid, V(t)))
L += [
    "}",
    "",
    "-- Truy Cong Lenh: Linux beidouactivity.lua:411-418 doi nguoi choi PHAI MANG",
    "-- theo (CalcEquiproomItemCount >= 1) moi duoc lenh bai Phong Lang Do. KHONG TRU.",
    "tbBeidou.ITEM_TRUYCONG = {6, 1, %d}" % ID_TRUYCONG,
    "",
]
ghi(os.path.join(THUMUC, "bd_head.lua"), L)

# ---------------------------------------------------------------- bd_lib.lua
L = list(BANNER)
L += [
    "-- 3 ham ban Linux dung ma JX1 khong dang ky - viet lai bang ham co san.",
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_head.lua")',
    "",
    "-- Linux: GetServerDate(fmt). JX1 co GetLocalDate cung y nghia.",
    "function BD_ServerDate(fmt)",
    "\treturn GetLocalDate(fmt)",
    "end",
    "",
    "-- Linux: GetItemLevel(idx) / GetItemSeries(idx).",
    "-- JX1: GetItemProp tra DU 6 gia tri genre,detail,particular,LEVEL,SERIES,luck",
    "-- (KJx2WarInfra.cpp:771-793) nen lay thang khong can them ham engine.",
    "function BD_ItemLevel(nItemIdx)",
    "\tlocal g, d, p, lv = GetItemProp(nItemIdx)",
    "\treturn lv",
    "end",
    "function BD_ItemSeries(nItemIdx)",
    "\tlocal g, d, p, lv, se = GetItemProp(nItemIdx)",
    "\treturn se",
    "end",
    "",
    "-- so lenh bai Bac Dau dang mang (hanh trang)",
    "function BD_DemLenhBai()",
    "\treturn CalcItemCount(3, 6, 1, %d, -1)" % ID_LENHBAI,
    "end",
    "",
]
ghi(os.path.join(THUMUC, "bd_lib.lua"), L)

# ---------------------------------------------------------------- bd_activity.lua
def T(s):
    return '"' + V(s) + '"'


L = list(BANNER)
L += [
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_lib.lua")',
    'Include("\\\\script\\\\lib\\\\awardtemplet.lua")',
    "",
    "-- ---------------------------------------------------------------- gio mo",
    "function tbBeidou:IsOpenTime()",
    '\tlocal nHour = tonumber(BD_ServerDate("%H"))',
    '\tlocal nBatDau = HD_CFG("HD3_BD_GIO_MO", self.nStartHour)',
    '\tlocal nKetThuc = HD_CFG("HD3_BD_GIO_DONG", self.nEndHour)',
    "\tif (nHour < nBatDau or nHour > nKetThuc) then",
    "\t\treturn 0",
    "\tend",
    "\treturn 1",
    "end",
    "",
    "function tbBeidou:CheckCondition()",
    '\tlocal nCap = HD_CFG("HD3_BD_CAP_TOITHIEU", self.MIN_LEVEL)',
    "\tif (ST_GetTransLifeCount ~= nil and ST_GetTransLifeCount() >= 1) then",
    "\t\treturn 1",
    "\tend",
    "\tif (GetLevel() < nCap) then",
    "\t\treturn 0",
    "\tend",
    "\treturn 1",
    "end",
    "",
    "-- ------------------------------------------------- doi ngay / doi gio",
    "function tbBeidou:UpdateTask()",
    "\tlocal nLast = GetTask(self.TSK_ACCEPT_TIME)",
    "\tlocal nNow = GetCurServerTime()",
    "\tif (FormatTime2Date(nLast) ~= FormatTime2Date(nNow)) then",
    "\t\tSetTask(self.TSK_TASK_STATE, 0)",
    "\t\tSetTask(self.TSK_ACCEPT_TIME, nNow)",
    "\t\tSetTask(self.TSK_FINISH_COUNT, 0)",
    "\t\tSetTask(self.TSK_HOUR_COUNT, 0)",
    "\t\treturn",
    "\tend",
    '\tlocal nGioNay = tonumber(BD_ServerDate("%H"))',
    '\tlocal nGioTruoc = tonumber(FormatTime2String("%H", nLast))',
    "\tif (nGioNay > nGioTruoc) then",
    "\t\tSetTask(self.TSK_TASK_STATE, 0)",
    "\t\tSetTask(self.TSK_ACCEPT_TIME, nNow)",
    "\t\tSetTask(self.TSK_HOUR_COUNT, 0)",
    "\tend",
    "end",
    "",
    "function tbBeidou:CheckOverTime()",
    "\tif (GetTask(self.TSK_ACCEPT_TIME) + self.ONE_HOUR < GetCurServerTime()) then",
    "\t\treturn 0",
    "\tend",
    "\treturn 1",
    "end",
    "",
    "-- ------------------------------------------------------------- menu NPC",
    "-- nThanh = so thu tu thanh cua NPC dang noi chuyen (1..7)",
    "function tbBeidou:Menu(nThanh)",
    "\tBD_THANH_HIENTAI = nThanh",
    "\tif (self:IsOpenTime() ~= 1) then",
    "\t\treturn Talk(1, \"\", MSG_NO_OPEN)",
    "\tend",
    "\tif (self:CheckCondition() ~= 1) then",
    "\t\treturn Talk(1, \"\", MSG_LEVEL_LIMIT)",
    "\tend",
    "\tself:UpdateTask()",
    "\tlocal tb = {",
    '\t\tMSG_WANT_TASK_OPT.."/BD_Want",',
    '\t\tMSG_SUBMIT_TASK_OPT.."/BD_Submit",',
    '\t\tMSG_CANCEL_TASK_OPT.."/BD_Cancel",',
    '\t\tMSG_QUERY_TASK_OPT.."/BD_Query",',
    '\t\tformat(BD_MSG_GET_AWARD, self.TOKEN_NUM_AWARD10).."/BD_Doi15",',
    '\t\tformat(BD_MSG_GET_AWARD, self.TOKEN_NUM_AWARD13).."/BD_Doi20",',
    '\t\tMSG_END_DIALOG.."/BD_No",',
    "\t}",
    '\tSay(" "..MSG_TITLE.." '
    + V("Ta là Bắc Đẩu lão nhân. Ngươi đang giữ ")
    + '<color=yellow>"..BD_DemLenhBai().."<color> '
    + V("lệnh bài Bắc Đẩu.") + '", getn(tb), tb)',
    "end",
    "",
    "-- --------------------------------------------------------- nhan nhiem vu",
    "function tbBeidou:Want()",
    "\tself:UpdateTask()",
    "\tif (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN) ~= 0) then",
    '\t\treturn Talk(1, "", MSG_ALREADY_HAVE_TASK)',
    "\tend",
    '\tlocal nMax = HD_CFG("HD3_BD_SO_LAN_NGAY", self.PER_DAY_MAX_TSK_COUNT)',
    "\tif (GetTask(self.TSK_FINISH_COUNT) >= nMax) then",
    '\t\treturn Talk(1, "", format(MSG_ERR_TASK_DAY_LIMIT, nMax))',
    "\tend",
    "\t-- Linux: random trong bang nhiem vu. Port nay = di toi 1 thanh khac.",
    "\tlocal nSo = getn(self.THANH)",
    "\tlocal nId = random(1, nSo)",
    "\tif (BD_THANH_HIENTAI ~= nil and nId == BD_THANH_HIENTAI and nSo > 1) then",
    "\t\tnId = mod(nId, nSo) + 1",
    "\tend",
    "\tSetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN, nId)",
    "\tSetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1, 0)",
    "\tSetTask(self.TSK_ACCEPT_TIME, GetCurServerTime())",
    "\tif (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_XUELINGDAN, 1) == 1) then",
    "\t\tSetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1, 1)",
    "\t\tSetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_XUELINGDAN, 1, 0)",
    "\tend",
    "\tMsg2Player(format(MSG_GET_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1])))",
    "end",
    "",
    "function tbBeidou:Query()",
    "\tlocal nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)",
    "\tif (nId == 0) then",
    '\t\treturn Talk(1, "", MSG_ERR_NO_TASK)',
    "\tend",
    "\tlocal sz = format(MSG_CURR_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1]))",
    "\tif (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1) == 1) then",
    '\t\tsz = sz.." <color=yellow>"..MSG_DOUBLE_AWARD.."<color>"',
    "\tend",
    '\tTalk(1, "", sz)',
    "end",
    "",
    "-- danh dau hoan thanh khi NOI CHUYEN dung NPC o thanh muc tieu",
    "function tbBeidou:CheckWithCity(nThanh)",
    "\tlocal nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)",
    "\tif (nId == 0 or nId ~= nThanh) then",
    "\t\treturn 0",
    "\tend",
    "\tif (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1) == 1) then",
    "\t\treturn 0",
    "\tend",
    "\tif (self:CheckOverTime() ~= 1) then",
    "\t\treturn 0",
    "\tend",
    "\tSetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1, 1)",
    "\tMsg2Player(MSG_FINISH_TASK)",
    "\treturn 1",
    "end",
    "",
    "-- ----------------------------------------------------------- nop nhiem vu",
    "function tbBeidou:Submit()",
    "\tlocal nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)",
    "\tif (nId == 0) then",
    '\t\treturn Talk(1, "", MSG_ERR_NO_TASK)',
    "\tend",
    "\tif (self:CheckOverTime() ~= 1) then",
    "\t\tSetTask(self.TSK_AWARD_STATE, 0)",
    "\t\tSetTask(self.TSK_TASK_STATE, 0)",
    '\t\treturn Talk(1, "", MSG_TASK_FAIL)',
    "\tend",
    "\tif (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1) == 0) then",
    '\t\treturn Talk(1, "", MSG_NO_FINISH_TASK)',
    "\tend",
    "\tif (self:GiveAward() == 1) then",
    "\t\tSetTask(self.TSK_TASK_STATE, 0)",
    "\t\tSetTask(self.TSK_FINISH_COUNT, GetTask(self.TSK_FINISH_COUNT) + 1)",
    "\t\tSetTask(self.TSK_HOUR_COUNT, GetTask(self.TSK_HOUR_COUNT) + 1)",
    "\t\tSetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1, 0)",
    "\tend",
    "end",
    "",
    "function tbBeidou:GiveAward()",
    "\tlocal nLan = 1",
    "\tif (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1) == 1) then",
    "\t\tnLan = 2",
    "\tend",
    "\tif (CountFreeRoomByWH(1, 1, 5) < 5) then",
    '\t\tTalk(1, "", "' + V("Hãy chừa trống ít nhất 5 ô hành trang.") + '")',
    "\t\treturn 0",
    "\tend",
    "\t-- Linux beidouactivity.lua:248 - nhiem vu di tim NPC thuong 8 trieu exp",
    '\tlocal nExp = HD_CFG("HD3_BD_EXP_NHIEMVU", 8000000) * nLan',
    '\ttbAwardTemplet:Give({nExp = nExp}, 1, {self.LOG_TITLE, "get exp"})',
    "\t-- Linux:273-277 - 1 Lenh bai Bac Dau (han 1 ngay) + 1 Chan Nguyen Don (Dai)",
    "\tlocal tbLB = {szName = self.ITEM_LENHBAI.szName, tbProp = self.ITEM_LENHBAI.tbProp,",
    '\t\tnBindState = -2, nExpiredTime = HD_CFG("HD3_BD_HAN_LENHBAI", 1440)}',
    '\ttbAwardTemplet:Give(tbLB, nLan, {self.LOG_TITLE, "get lenh bai"})',
    '\ttbAwardTemplet:Give(self.ITEM_CNDAI, 1, {self.LOG_TITLE, "get CND dai"})',
    "\treturn 1",
    "end",
    "",
    "function tbBeidou:Cancel()",
    "\tlocal nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)",
    "\tif (nId == 0) then",
    '\t\treturn Talk(1, "", MSG_ERR_NO_TASK)',
    "\tend",
    "\tSetTask(self.TSK_TASK_STATE, 0)",
    "\tSetTask(self.TSK_AWARD_STATE, 0)",
    "\tSetTask(self.TSK_FINISH_COUNT, GetTask(self.TSK_FINISH_COUNT) + 1)",
    "\tSetTask(self.TSK_HOUR_COUNT, GetTask(self.TSK_HOUR_COUNT) + 1)",
    "\tMsg2Player(format(MSG_CANCEL_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1])))",
    "end",
    "",
    "-- --------------------------------------------------------- doi lenh bai",
    "function tbBeidou:DoiThuong(nCan, tbThuong)",
    "\tif (self:IsOpenTime() ~= 1) then",
    '\t\treturn Talk(1, "", MSG_NO_OPEN)',
    "\tend",
    "\tlocal nCo = BD_DemLenhBai()",
    "\tif (nCo < nCan) then",
    '\t\treturn Talk(1, "", format(MSG_ERR_NO_ENOUGH_COUNT, self.ITEM_LENHBAI.szName))',
    "\tend",
    "\tif (CountFreeRoomByWH(1, 1, 10) < 10) then",
    '\t\treturn Talk(1, "", "' + V("Hãy chừa trống ít nhất 10 ô hành trang.") + '")',
    "\tend",
    "\tlocal tp = self.ITEM_LENHBAI.tbProp",
    "\tif (ConsumeItem(3, nCan, tp[1], tp[2], tp[3], -1) ~= 1) then",
    '\t\treturn Talk(1, "", format(MSG_ERR_NO_ENOUGH_COUNT, self.ITEM_LENHBAI.szName))',
    "\tend",
    '\ttbAwardTemplet:Give(tbThuong, 1, {self.LOG_TITLE, format("doi %d lenh bai", nCan)})',
    "end",
    "",
    "-- ------------------------------------------- phat lenh bai tu hoat dong",
    "-- Ban Linux (beidouactivity.lua:334-340 GiveTokens) phat lenh bai RIENG cua",
    "-- tung hoat dong. Port nay giu nguyen. NHUNG: quet toan cay Linux cho thay",
    "-- 13 ma do KHONG CHO NAO TIEU (SubmitToken10/13 chi dem ma 'Lenh bai Bac Dau'),",
    "-- va truong nAwardExp cua chung cung chet (awardtemplet chi biet tbProp/nExp).",
    "-- => them khoa HD3_BD_QUYDOI_LENHBAI (mac dinh 1) phat KEM 1 Lenh bai Bac Dau",
    "-- de hoat dong thuc su nuoi duoc moc doi thuong 15/20. Dat 0 = y het Linux.",
    "function tbBeidou:PhatLenhBai(szKhoa)",
    '\tif (HD_CFG("HD3_BD_BAT", 1) ~= 1) then',
    "\t\treturn 0",
    "\tend",
    "\tif (self:CheckCondition() ~= 1) then",
    "\t\treturn 0",
    "\tend",
    "\tlocal tb = self.LENHBAI_HD[szKhoa]",
    "\tif (tb == nil) then",
    "\t\treturn 0",
    "\tend",
    "\tif (CountFreeRoomByWH(1, 1, 2) < 2) then",
    '\t\tMsg2Player("' + V("Hành trang đầy nên không nhận được lệnh bài Bắc Đẩu.") + '")',
    "\t\treturn 0",
    "\tend",
    '\tlocal nHan = HD_CFG("HD3_BD_HAN_LENHBAI", 1440)',
    "\ttbAwardTemplet:Give({tbProp = {6, 1, tb[1], 1, 0, 0}, nCount = 1, nBindState = -2,",
    '\t\tnExpiredTime = nHan}, 1, {self.LOG_TITLE, "token "..szKhoa})',
    '\tif (HD_CFG("HD3_BD_QUYDOI_LENHBAI", 1) == 1) then',
    "\t\ttbAwardTemplet:Give({szName = self.ITEM_LENHBAI.szName, tbProp = self.ITEM_LENHBAI.tbProp,",
    '\t\t\tnCount = 1, nBindState = -2, nExpiredTime = nHan}, 1, {self.LOG_TITLE, "lenh bai "..szKhoa})',
    "\tend",
    '\tMsg2Player("' + V("Nhận được lệnh bài Bắc Đẩu từ hoạt động ")
    + '<color=yellow>"..tb[2].."<color>")',
    "\treturn 1",
    "end",
    "",
    "-- ============================================================================",
    "-- MOC TU CAC HOAT DONG - moi ham deu an toan khi he Bac Dau chua nap.",
    "-- ============================================================================",
    "function HD3_BD_Co()",
    "\treturn (tbBeidou ~= nil and tbBeidou.PhatLenhBai ~= nil)",
    "end",
    "",
    "-- Tong Kim: Linux OnFinishSongJin - theo diem tich luy (task 751)",
    "function HD3_BD_TongKim(nDiem)",
    "\tif (not HD3_BD_Co()) then return 0 end",
    "\tnDiem = nDiem or 0",
    "\tlocal szKhoa = nil",
    "\tif (nDiem >= 20000) then szKhoa = \"phongvan4\"",
    "\telseif (nDiem >= 10000) then szKhoa = \"phongvan3\"",
    "\telseif (nDiem >= 5000) then szKhoa = \"phongvan2\"",
    "\telseif (nDiem >= 2000) then szKhoa = \"phongvan1\"",
    "\tend",
    "\tif (szKhoa == nil) then return 0 end",
    "\treturn tbBeidou:PhatLenhBai(szKhoa)",
    "end",
    "",
    "-- Vuot ai: Linux OnPassChuanGuan - qua ai thu 10 va thu 28",
    "function HD3_BD_VuotAi(nSoAi)",
    "\tif (not HD3_BD_Co()) then return 0 end",
    "\tif (nSoAi == 28) then return tbBeidou:PhatLenhBai(\"vuotai2\") end",
    "\tif (nSoAi == 10) then return tbBeidou:PhatLenhBai(\"vuotai1\") end",
    "\treturn 0",
    "end",
    "",
    "-- Phong Lang Do: Linux OnLanding - PHAI DANG MANG Truy Cong Lenh (khong tru).",
    "-- bNhiemVuThuyTac = 1 (khung gio ton phi) thi duoc lenh bai cap 2.",
    "function HD3_BD_PhongLangDo(bNhiemVuThuyTac)",
    "\tif (not HD3_BD_Co()) then return 0 end",
    "\tlocal tp = tbBeidou.ITEM_TRUYCONG",
    "\tif (CalcItemCount(3, tp[1], tp[2], tp[3], -1) < 1) then",
    "\t\treturn 0",
    "\tend",
    "\tif (bNhiemVuThuyTac == 1) then",
    "\t\treturn tbBeidou:PhatLenhBai(\"phonglangdo2\")",
    "\tend",
    "\treturn tbBeidou:PhatLenhBai(\"phonglangdo1\")",
    "end",
    "",
    "-- Tin Su: Linux OnFinishMessenger",
    "function HD3_BD_TinSu()",
    "\tif (not HD3_BD_Co()) then return 0 end",
    "\treturn tbBeidou:PhatLenhBai(\"tinsu\")",
    "end",
    "",
    "-- Boss sat thu: Linux OnFinishKillerBoss - CHI nhom boss cap 90",
    "function HD3_BD_SatThu(nCapBoss)",
    "\tif (not HD3_BD_Co()) then return 0 end",
    "\tif (nCapBoss ~= 90) then return 0 end",
    "\treturn tbBeidou:PhatLenhBai(\"satthu\")",
    "end",
    "",
    "-- --------------------------------------------------- cau noi cho menu Say",
    "function BD_Want()    tbBeidou:Want()   end",
    "function BD_Submit()  tbBeidou:Submit() end",
    "function BD_Cancel()  tbBeidou:Cancel() end",
    "function BD_Query()   tbBeidou:Query()  end",
    "function BD_Doi15()   tbBeidou:DoiThuong(tbBeidou.TOKEN_NUM_AWARD10, tbBeidou.tbAward10) end",
    "function BD_Doi20()   tbBeidou:DoiThuong(tbBeidou.TOKEN_NUM_AWARD13, tbBeidou.tbAward13) end",
    "function BD_No()      end",
    "",
]
ghi(os.path.join(THUMUC, "bd_activity.lua"), L)

# ---------------------------------------------------------------- NPC script
L = list(BANNER)
L += [
    "-- NPC Bac Dau lao nhan. GetNpcValue tra so thu tu thanh (1..7).",
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")',
    "",
    "function main(NpcIndex)",
    "\tlocal nThanh = GetNpcValue(NpcIndex)",
    "\tif (nThanh == nil or nThanh < 1 or nThanh > getn(tbBeidou.THANH)) then",
    "\t\tnThanh = 1",
    "\tend",
    "\t-- den dung thanh muc tieu = hoan thanh nhiem vu di duong",
    "\ttbBeidou:CheckWithCity(nThanh)",
    "\ttbBeidou:Menu(nThanh)",
    "end",
    "",
]
ghi(os.path.join(THUMUC, "bd_npc.lua"), L)

# ---------------------------------------------------------------- 2 item script
L = list(BANNER)
L += [
    "-- Bac Dau Chi Bao (Linux beidouzhibao.lua): dung de HOAN THANH ngay nhiem vu.",
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")',
    "",
    "function main(nItemIndex)",
    "\tlocal nId = GetBitTask(tbBeidou.TSK_TASK_STATE, tbBeidou.TSK_BIT_TASK_ID, tbBeidou.TSK_BIT_ID_LEN)",
    "\tif (nId == 0) then",
    '\t\tTalk(1, "", MSG_ERR_NO_TASK)',
    "\t\treturn 1",
    "\tend",
    "\tif (tbBeidou:CheckOverTime() ~= 1) then",
    "\t\tSetTask(tbBeidou.TSK_AWARD_STATE, 0)",
    "\t\tSetTask(tbBeidou.TSK_TASK_STATE, 0)",
    '\t\tTalk(1, "", MSG_TASK_FAIL)',
    "\t\treturn 1",
    "\tend",
    "\tif (tbBeidou:GiveAward() ~= 1) then",
    "\t\treturn 1",
    "\tend",
    "\tif (ConsumeItem(3, 1, 6, 1, %d, -1) ~= 1) then" % ID_CHIBAO,
    "\t\treturn 1",
    "\tend",
    "\tSetTask(tbBeidou.TSK_TASK_STATE, 0)",
    "\tSetTask(tbBeidou.TSK_FINISH_COUNT, GetTask(tbBeidou.TSK_FINISH_COUNT) + 1)",
    "\tSetTask(tbBeidou.TSK_HOUR_COUNT, GetTask(tbBeidou.TSK_HOUR_COUNT) + 1)",
    "\tSetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_DOUBLE_AWARD, 1, 0)",
    "\tMsg2Player(MSG_BEIDOUZHIBAO)",
    "end",
    "",
]
ghi(r"script\item\bd_chibao.lua", L)

L = list(BANNER)
L += [
    "-- Bac Dau Huyet Linh Don (Linux beidouxuelingdan.lua): lan nhan nhiem vu ke",
    "-- tiep duoc thuong GAP DOI. Gioi han 15 lan/ngay (task 2999) - y het Linux.",
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")',
    "",
    "function main(nItemIndex)",
    "\tif (GetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_XUELINGDAN, 1) == 1) then",
    '\t\tTalk(1, "", MSG_ALREADY_DOUBLE_AWARD)',
    "\t\treturn 1",
    "\tend",
    "\t-- dem theo NGAY: task 2999 luu ngay + so lan",
    "\tlocal nNgay = tonumber(FormatTime2Date(GetCurServerTime()))",
    "\tlocal nLuu = GetTask(tbBeidou.TSK_LIMIT_TIMES)",
    "\tlocal nNgayLuu = floor(nLuu / 100)",
    "\tlocal nLan = mod(nLuu, 100)",
    "\tif (nNgayLuu ~= mod(nNgay, 100000)) then",
    "\t\tnLan = 0",
    "\tend",
    '\tlocal nMax = HD_CFG("HD3_BD_LINHDAN_NGAY", 15)',
    "\tif (nLan >= nMax) then",
    '\t\tTalk(1, "", format("' + V("Hôm nay đã dùng %d lần, không thể dùng thêm.") + '", nMax))',
    "\t\treturn 1",
    "\tend",
    "\tif (ConsumeItem(3, 1, 6, 1, %d, -1) ~= 1) then" % ID_LINHDAN,
    "\t\treturn 1",
    "\tend",
    "\tSetTask(tbBeidou.TSK_LIMIT_TIMES, mod(nNgay, 100000) * 100 + nLan + 1)",
    "\tSetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_XUELINGDAN, 1, 1)",
    "\tMsg2Player(MSG_DOUBLE_AWARD_TIP)",
    "end",
    "",
]
ghi(r"script\item\bd_linhdan.lua", L)
print("da sinh xong 7 tep script Bac Dau")
