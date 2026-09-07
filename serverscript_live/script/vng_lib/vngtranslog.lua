-- ================================================================================================
-- [HE THONG] script/vng_lib/vngtranslog.lua
-- Muc dich  : Log giao dich VNG.
-- Duoc nap  : Include tu 6 tep (vd activitydetail.lua, partysupport.lua, simple.lua, honour.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : files_lib.lua
-- Ham (dong): tbVngTransLog:Write (19)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\vng_lib\\files_lib.lua")
IncludeLib("SETTING")
local tbVngFactionName = 
{
[-1]="Ch­a nhËp ph¸i",
 [0]="ThiÕu L©m ph¸i",
 [1]="Thiªn V­¬ng Bang",
 [2]="§­êng M«n",
 [3]="Ngò §éc Gi¸o",
 [4]="Nga My ph¸i",
 [5]="Thóy Yªn m«n",
 [6]="C¸i Bang",
 [7]="Thiªn NhÉn Gi¸o",
 [8]="Vâ §ang ph¸i",
 [9]="C«n L«n ph¸i",
}
tbVngTransLog = {}
tbVngTransLog.strFolder = "vng_data/Logs/"

function tbVngTransLog:Write(strEventFolder, nPromotionID, ...) local arg = {n = select("#", ...), ...};
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
		tbVngFactionName[GetLastFactionNumber()] or "",
		GetWorldPos() or 0,
		nPromotionID,		
	}
	for i = 1, getn(arg) do
		tinsert(tbLog2Write, arg[i])
	end
	tbVngLib_File:Table2File(strFilePath, strFileName, "a", tbLog2Write)
end