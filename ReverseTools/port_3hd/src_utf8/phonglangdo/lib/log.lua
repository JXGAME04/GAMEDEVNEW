IncludeLib("ITEM");

local tbFactionNumber2Name = 
{
 [-1]="Người không tên",
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
 [10]="Hoa Sơn phái",
}

function _WritePlayerLog(szTitle, szMsg)
	
	local szTime = GetLocalDate("%H:%M\t%d/%m/%Y")		
	local szLog = format("[%s]\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
		szTitle,
		szTime,
		GetServerName(),			-- 服务器名字
		GetAccount(),		-- 帐号
		GetName(),			-- 角色名
		GetLastFactionNumber(),		-- 门派
		GetLevel(),			-- 等级
		GetTong(),			-- 帮会
		szMsg)
	WriteLog(szLog)
end

function getItemInfo(nItemIdx)
	local nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck = GetItemProp(nItemIdx);
	local nQuality = GetItemQuality(nItemIdx);
	if (nQuality == 1) then
		nDetailType = GetGlodEqIndex(nItemIdx) - 1;
	end
	local szPlatinaLvl = "nil";
	if (nQuality == 4) then
		szPlatinaLvl = tostring(GetPlatinaLevel(nItemIdx));
		nDetailType = GetPlatinaEquipIndex(nItemIdx) - 1;
	end;
	local arynMagLvl = GetItemAllParams(nItemIdx);
	local szDes = format("{4,%0.0f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}:%s,GET_TIME:%0.0f,PLA_LVL:%s", 
							ITEM_GetItemRandSeed(nItemIdx), nQuality, 
							nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck, 
							arynMagLvl[1], arynMagLvl[2], arynMagLvl[3], arynMagLvl[4], arynMagLvl[5], arynMagLvl[6],
							GetItemName(nItemIdx),GetItemGenTime(nItemIdx),szPlatinaLvl);
	return szDes;
end;

tbLog = {}
function tbLog:TabFormatLog(...)
	local szLog = arg[1]
	for i=2, getn(arg) do
		szLog = format("%s\t%s", szLog, tostring(arg[i]))
	end
	WriteLog(szLog)
end

IncludeLib("SETTING")

function tbLog:PlayerAwardLog(szEventName, szAction, szAwardName, szCode, nAmount)
	local nTransLifeCount = 0
	if ST_GetTransLifeCount then
		nTransLifeCount = ST_GetTransLifeCount()
	end
	self:TabFormatLog(
		GetAccount() or "",
		GetName() or "",
		szEventName or "",
		GetLevel() or 0,
		nTransLifeCount,
		szAction or "",
		szAwardName or "",
		szCode or "",
		nAmount or 0,
		GetTongName() or "",
		GetWorldPos() or 0
	)
end


function tbLog:PlayerActionLog(szKeyLog, ...)
	self:TabFormatLog(
		GetLocalDate("%H:%M") or "",		-- 时间	
		GetLocalDate("%d/%m/%Y") or "",		-- 日期
		GetServerName(),					-- 服务器
		GetAccount() or "",					-- 帐号
		GetName() or "",					-- 名字
		%tbFactionNumber2Name[GetLastFactionNumber()] or "",				-- 门派
		GetLevel() or 0,					-- 等级
		GetTongName() or "",				-- 帮会
		szKeyLog or "",
		unpack(arg)						-- 根据不同情况需要传入的参数
	
	)
end
	
--Write log format new
tbLog.tbLogActionName = {
	[1] = "Buy",
	[2] = "Exchange",
	[3] = "Used",
	[4] = "Fight",
	[5] = "Reduce",
	[6] = "Gift",
}
tbLog.tbLogItemCount = {
	["nExp"] = 1,
	["nJxb"] = 1,
	["nRepute"] = 1,
	["nVngContribute"] = 1,
	["nVngActPoint"] = 1,
	["nVngExpPoint"] = 1,
}

function tbLog:VngGetItemID(tbItem,nAwardCount)
	for k, v in self.tbLogItemCount do
		if(tbItem[k]) then
			return format("%s-%s",k,tbItem[k]*nAwardCount)
		end
	end
	return format("%s-%s",(tbItem.szItemId or "nil"),(tbItem.nCount*nAwardCount or "nil"))
end

function tbLog:VngReduceFormatLog(tbItem,tbLogTitle)
	local szLogItemId = "nil"
	for _,v in tbItem do
		szLogItemId = format("%s,%s",self:VngGetItemID(v),szLogItemId)
	end
	tbLog:VngActionFormatLog(tbLogTitle,szLogItemId)
end

function tbLog:VngActionFormatLog(tbLogTitle,szLogItemId)
	local szAccount = GetAccount() or "nil";
	local szRoleName = GetName() or "nil"	;
	local nTransLifeCount = ST_GetTransLifeCount() or 0;
	local nLevel = GetLevel() or 0;
	local szFaction = %tbFactionNumber2Name[GetLastFactionNumber()] or "Không phái";
	local szAction = self.tbLogActionName[tonumber(tbLogTitle[2])] or "nil";
	local ItemID_Used = tbLogTitle[4] or "nil";
	local szActionLog = tbLogTitle[3] or "nil";
	local EventID = tbLogTitle[1]  or "nil";
	local ItemID_Get = szLogItemId  or "nil";
	local ItemID_Give = tbLogTitle[5]  or "nil";
	self:TabFormatLog(szAccount, szRoleName, nTransLifeCount, nLevel, szFaction, EventID, szAction, szActionLog, ItemID_Used, ItemID_Get, ItemID_Give);
end
