-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
-- ================================================================================================
-- [HE THONG] script/lib/lib_lmbiaoche.lua
-- Muc dich  : VAN TIEU Long Mon: ham xe tieu (engine KBiaoChe.cpp).
-- Duoc nap  : Include tu 7 tep (vd extend.lua, npc_lmbiaoche.lua, npc_lmbiaowu.lua, variables.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): WriteYunBiaoLog (32), VT_TestBoQua (67), VT_TestDat (85)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- =========================================================================
-- lib_lmbiaoche.lua   [LMBC 06/09]  thu vien Long Mon Tieu Cuc cho JX1
--
-- WriteYunBiaoLog(szMsg)
--   Ban Linux day la HAM ENGINE (chuoi "WriteYunBiaoLog" tai offset 2179155
--   trong D:\ServerLinux\server1\jx_linux_y). Engine JX1 KHONG co ham nay
--   nen boc lai THUAN LUA o day.
--
--   Bang chung dinh dang, doc thang tu jx_linux_y:
--     offset 2137119  "Logs/KSG_YunBiaoLog"                     <- goc ten tep
--     offset 2184420  "[%04d-%02d-%02d %02d:%02d:%02d]\t%s\r\n"   <- mau mot dong
--   Tep that ben Linux: D:\ServerLinux\server1\Logs\KSG_YunBiaoLog_20260806.txt
--     -> engine noi them "_YYYYMMDD.txt", tuc XOAY THEO NGAY. Ta lam y het.
--
-- HAI HAM ENGINE JX1 DUNG O DAY (da doc nguon xac nhan chu ky):
--   GetLocalDate(szFormat) -> szKetQua
--     Sources\Core\Src\ScriptFuns.cpp:3465. NHAN MOT CHUOI dinh dang
--     strftime va tra ve MOT CHUOI. KHONG PHAI bang 6 so nhu ban Linux.
--     Khi strftime that bai ham tra ve 0 gia tri (=> nil ben Lua), nen moi
--     cho goi deu phai co duong lui - loi Lua trong timer la CHET VINH VIEN.
--   WriteStringToFile(szDuongDan, szNoiDung) -> 1 / 0
--     Sources\Core\Src\KTongJX2.cpp:4077. Mo che do "ab" (noi duoi), TU TAO
--     thu muc cha, dau '\' o dau duong dan = GOC MAY CHU (khong phai goc o dia).
-- =========================================================================

-- Duong dan goc cua tep log (dau '\' = goc may chu). Tep that se la
--   <goc may chu>\Logs\KSG_YunBiaoLog_YYYYMMDD.txt
LMBC_LOG_PREFIX   = "\\Logs\\KSG_YunBiaoLog_"
LMBC_LOG_SUFFIX   = ".txt"
LMBC_LOG_FMT_DAY  = "%Y%m%d"                -- strftime: ngay dat vao ten tep
LMBC_LOG_FMT_TIME = "%Y-%m-%d %H:%M:%S"     -- strftime: dau dong, = %04d-%02d-...

function WriteYunBiaoLog(szMsg)
	if szMsg == nil then
		return 0;
	end
	if type(szMsg) ~= "string" then
		szMsg = tostring(szMsg);
	end

	-- GetLocalDate tra ve MOT CHUOI, hoac nil khi strftime that bai.
	local szDay = GetLocalDate(LMBC_LOG_FMT_DAY);
	local szTime = GetLocalDate(LMBC_LOG_FMT_TIME);
	if type(szDay) ~= "string" or szDay == "" then
		szDay = "00000000";
	end
	if type(szTime) ~= "string" or szTime == "" then
		szTime = "0000-00-00 00:00:00";
	end

	-- Mau ban Linux: "[<thoi gian>]<TAB><noi dung><CR><LF>"
	local szLine = "[" .. szTime .. "]\t" .. szMsg .. "\r\n";
	return WriteStringToFile(LMBC_LOG_PREFIX .. szDay .. LMBC_LOG_SUFFIX, szLine);
end

-- =========================================================================
-- [VTCN 06/09] CO TEST van tieu - 3 bit trong bien nhiem vu 4169 CUA NGUOI CHOI
-- (chi admin dat qua Lenh bai admin > Bo test van tieu > Co test). Vi JX1 moi
-- tep .lua la mot lua_State rieng nen KHONG dung bien toan cuc de bat co; bien
-- nhiem vu thi moi state deu doc duoc va chi anh huong nguoi bat co.
--   bit 1 (1): bo qua khung gio / thu   (ca nhan 10-23h; bang thu 7, CN 12-23h)
--   bit 2 (2): bo qua "vao bang du 7 ngay"
--   bit 3 (4): coi bang minh la bang dang chiem thanh dang dung
-- 4169 nam ngoai moi dai dang dung (4160-4168 nhanh cu, 4178-4187 nhanh bang).
-- =========================================================================
VT_TEST_TASK = 4169

function VT_TestBoQua(nBit)
	if (GetTask == nil or PlayerIndex == nil or PlayerIndex <= 0) then
		return 0
	end
	local v = GetTask(VT_TEST_TASK)
	if (v == nil or v == 0) then
		return 0
	end
	local nMask = 1
	for i = 2, nBit do
		nMask = nMask * 2
	end
	if (mod(floor(v / nMask), 2) == 1) then
		return 1
	end
	return 0
end

function VT_TestDat(nBit, bOn)
	local v = GetTask(VT_TEST_TASK)
	if (v == nil) then
		v = 0
	end
	local nMask = 1
	for i = 2, nBit do
		nMask = nMask * 2
	end
	local bCo = (mod(floor(v / nMask), 2) == 1)
	if (bOn == 1 and not bCo) then
		v = v + nMask
	elseif (bOn == 0 and bCo) then
		v = v - nMask
	end
	SetTask(VT_TEST_TASK, v)
end
