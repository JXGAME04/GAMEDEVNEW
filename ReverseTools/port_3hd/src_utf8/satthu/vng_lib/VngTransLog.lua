Include("\\script\\vng_lib\\files_lib.lua")
IncludeLib("SETTING")
local tbVngFactionName = 
{
[-1]="Chưa nhập phái",
 [0]="Thiếu Lâm phái",
 [1]="Thiên Vương Bang",
 [2]="Đường Môn",
 [3]="Ngũ Độc Giáo",
 [4]="Nga My phái",
 [5]="Thúy Yên môn",
 [6]="Cái Bang",
 [7]="Thiên Nhẫn Giáo",
 [8]="Võ Đang phái",
 [9]="Côn Lôn phái",
}
tbVngTransLog = {}
tbVngTransLog.strFolder = "vng_data/Logs/"

function tbVngTransLog:Write(strEventFolder, nPromotionID, ...)
--Close
do return end
	local strFilePath = self.strFolder..strEventFolder
	local strFileName = GetLocalDate("%Y_%m_%d_").."TransLog.txt"
	local tbLog2Write = {
		GetLocalDate("%Y-%m-%d %H:%M:%S"),
		GetAccount() or "",
		GetName() or "",
		GetLevel() or 0,
		ST_GetTransLifeCount(),
		%tbVngFactionName[GetLastFactionNumber()] or "",
		GetWorldPos() or 0,
		nPromotionID,		
	}
	for i = 1, getn(arg) do
		tinsert(tbLog2Write, arg[i])
	end
	%tbVngLib_File:Table2File(strFilePath, strFileName, "a", tbLog2Write)
end