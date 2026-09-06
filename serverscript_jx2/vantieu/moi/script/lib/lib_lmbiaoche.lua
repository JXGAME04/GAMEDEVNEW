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
