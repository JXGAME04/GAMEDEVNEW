-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_lang.lua")

tbBeidou = tbBeidou or {}
tbBeidou.LOG_TITLE = "beidou Activity"

-- head.lua ban Linux - GIU NGUYEN moi so:
tbBeidou.nStartHour = 8
tbBeidou.nEndHour = 22
tbBeidou.MIN_LEVEL = 150
tbBeidou.PER_DAY_MAX_TSK_COUNT = 30
tbBeidou.ONE_HOUR = 3600
tbBeidou.TOKEN_NUM_AWARD10 = 15
tbBeidou.TOKEN_NUM_AWARD13 = 20

-- task id cua Linux - da kiem tra CA 6 deu trong o JX1
tbBeidou.TSK_ACCEPT_TIME = 4044
tbBeidou.TSK_TASK_STATE  = 4045
tbBeidou.TSK_BIT_TASK_ID = 1
tbBeidou.TSK_BIT_ID_LEN = 29
tbBeidou.TSK_BIT_TASK_DONE = 30
tbBeidou.TSK_AWARD_STATE = 4046
tbBeidou.TSK_BIT_DOUBLE_AWARD = 1
tbBeidou.TSK_BIT_XUELINGDAN = 2
tbBeidou.TSK_HOUR_COUNT = 4047
tbBeidou.TSK_FINISH_COUNT = 4048
tbBeidou.TSK_LIMIT_TIMES = 2999

-- 3 item MOI cua JX1 (ma Linux 3508/3522/3523 da bi item khac chiem)
tbBeidou.ITEM_LENHBAI = {szName = "LÖnh bµi B¾c §Èu", tbProp = {6,1,4126,1,0,0}, nBindState = -2}
tbBeidou.ITEM_CNDAI   = {szName = "Ch©n Nguyªn §¬n (§¹i)", tbProp = {6,1,4847,0,0,0}, nCount = 1, nBindState = -2}

-- thuong doi lenh bai - head.lua ban Linux (tbAward10 / tbAward13)
tbBeidou.tbAward10 =
{
	{nExp = 50000000},
	{tbProp = {6,1,4847,1,0,0}, nBindState = -2, nCount = 15},
}
tbBeidou.tbAward13 =
{
	{nExp = 80000000},
	{tbProp = {6,1,4847,1,0,0}, nBindState = -2, nCount = 20},
}

-- 7 thanh dat NPC Bac Dau lao nhan: {ten thanh, mapId, x, y}
tbBeidou.THANH =
{
	{"Thµnh §«", 11, 3216, 4974},
	{"Ph­îng T­êng", 1, 1512, 3198},
	{"BiÖn Kinh", 37, 1653, 3050},
	{"L©m An", 176, 1378, 3010},
	{"§¹i Lý", 162, 1579, 3227},
	{"T­¬ng D­¬ng", 78, 1518, 3206},
	{"D­¬ng Ch©u", 80, 1706, 2963},
}

-- 13 lenh bai RIENG cua tung hoat dong (Linux itemlist.lua:132-144).
-- {ma item JX1, ten hoat dong de hien thong bao}
tbBeidou.LENHBAI_HD =
{
	["phongvan1"] = {4127, "Tèng Kim"},
	["phongvan2"] = {4128, "Tèng Kim"},
	["phongvan3"] = {4129, "Tèng Kim"},
	["phongvan4"] = {4130, "Tèng Kim"},
	["vuotai1"] = {4131, "V­ît ¶i"},
	["vuotai2"] = {4132, "V­ît ¶i"},
	["viemde1"] = {4133, "Viªm §Õ"},
	["viemde2"] = {4134, "Viªm §Õ"},
	["phonglangdo1"] = {4135, "Phong L¨ng §é"},
	["phonglangdo2"] = {4136, "Phong L¨ng §é"},
	["tinsu"] = {4137, "TÝn Sø"},
	["satthu"] = {4138, "Boss s¸t thñ"},
	["thienloc"] = {4139, "Thiªn Léc Phóc"},
}

-- Truy Cong Lenh: Linux beidouactivity.lua:411-418 doi nguoi choi PHAI MANG
-- theo (CalcEquiproomItemCount >= 1) moi duoc lenh bai Phong Lang Do. KHONG TRU.
tbBeidou.ITEM_TRUYCONG = {6, 1, 2024}
