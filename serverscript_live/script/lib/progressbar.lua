-- [HOASON 01/09] port tu Linux script\lib\progressbar.lua (bo IncludeLib TIMER/FILESYS, cau hinh settings\progressconfig.txt nhung thang vao bang)
-- JX1 OpenProgressBar (KJx2WarInfra.cpp LuaHD3_OpenProgressBar) = hop dem gio TimeBox: chi dung szTitle, nFrame va ten ham OnTime.
local tbData = {}
tbProgressBar = {}
tbProgressBar.tbConfig = {
	[1] = {nId = 1, szTitle = "§ang thu thËp…", nTime = 3, nEvent = 65535, bDesc = 0},
	[2] = {nId = 2, szTitle = "§ang thu thËp…", nTime = 8, nEvent = 65535, bDesc = 0},
	[3] = {nId = 3, szTitle = "§ang thu thËp…", nTime = 2, nEvent = 65535, bDesc = 0},
	[4] = {nId = 4, szTitle = "§ang nhÆt…", nTime = 2, nEvent = 65535, bDesc = 0},
	[5] = {nId = 5, szTitle = "§ang më…", nTime = 10, nEvent = 65535, bDesc = 0},
	[6] = {nId = 6, szTitle = "B¾t ®Çu lÊy n­íc...", nTime = 8, nEvent = 65535, bDesc = 0},
	[7] = {nId = 7, szTitle = "§ang thu thËp…", nTime = 5, nEvent = 65535, bDesc = 0},
	[8] = {nId = 8, szTitle = "§ang ¨n...", nTime = 3, nEvent = 65535, bDesc = 0},
	[9] = {nId = 9, szTitle = "§ang trÞ liÖu", nTime = 5, nEvent = 65535, bDesc = 0},
	[10] = {nId = 10, szTitle = "§ang l·nh ngé", nTime = 5, nEvent = 65535, bDesc = 0},
	[11] = {nId = 11, szTitle = "§ang më…", nTime = 6, nEvent = 6157, bDesc = 0},
	[12] = {nId = 12, szTitle = "§ang më…", nTime = 0, nEvent = 6157, bDesc = 0},
	[13] = {nId = 13, szTitle = "§ang nhÆt…", nTime = 3, nEvent = 65535, bDesc = 0},
	[14] = {nId = 14, szTitle = "§ang th¨ng cÊp…", nTime = 2, nEvent = 32769, bDesc = 0},
	[15] = {nId = 15, szTitle = "§ang C©u C¸", nTime = 10, nEvent = 65535, bDesc = 0},
	[16] = {nId = 16, szTitle = "§Æt BÉy", nTime = 1, nEvent = 65535, bDesc = 0},
}
function tbProgressBar:OpenByConfig(nId, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)
	if not self.tbConfig or not self.tbConfig[nId] then
		return
	end
	local pTemp = self.tbConfig[nId]
	self:Start(pTemp.szTitle, pTemp.nTime * 18, pTemp.nEvent, pTemp.bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)
end
function tbProgressBar:Open(szTitle, nFrame, tbEvent, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)
	self:Start(szTitle, nFrame, 0, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)
end
function tbProgressBar:Start(szTitle, nFrame, nEvent, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)
	OpenProgressBar(szTitle, nFrame, nEvent, bDesc, "tbProgressBar_OnTime", "tbProgressBar_OnBreak")
	tbData[PlayerIndex] = {}
	tbData[PlayerIndex].fnOnTime = fnOnTime
	tbData[PlayerIndex].tbOnTimeParam = tbOnTimeParam
	tbData[PlayerIndex].fnOnBreak = fnOnBreak
	tbData[PlayerIndex].tbOnBreakParam = tbOnBreakParam
end
function tbProgressBar_OnTime()
	local tbPlayerProgressData = tbData[PlayerIndex] or {}
	if not tbPlayerProgressData.fnOnTime then
		return
	end
	return call(tbPlayerProgressData.fnOnTime, tbPlayerProgressData.tbOnTimeParam or {})
end
function tbProgressBar_OnBreak()
	local tbPlayerProgressData = tbData[PlayerIndex] or {}
	if not tbPlayerProgressData.fnOnBreak then
		return
	end
	return call(tbPlayerProgressData.fnOnBreak, tbPlayerProgressData.tbOnBreakParam or {})
end
