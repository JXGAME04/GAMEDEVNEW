Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\vng_lib\\bittask_lib.lua")
tbvng_ChestExpAward = {}
tbvng_ChestExpAward.TSK_DAY = 2744
--tbvng_ChestExpAward.TSK_EXP = 2742
tbvng_ChestExpAward.tbBitTask = {
	["Tống Kim Bí Bảo"] = {
		nTaskID = 2705,
		nStartBit = 1,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Bảo rương vượt ải"] = {
		nTaskID = 2705,
		nStartBit = 8,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Bảo Rương Thủy Tặc"] = {
		nTaskID = 2705,
		nStartBit = 15,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Tín Sứ Bảo Rương"] = {
		nTaskID = 2705,
		nStartBit = 22,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Viêm Đế Bí Bảo"] = {
		nTaskID = 2706,
		nStartBit = 1,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Vệ Trụ Lễ Bao"] = {
		nTaskID = 2706,
		nStartBit = 8,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Công Thành Lễ Bao (mới)"] = {
		nTaskID = 2706,
		nStartBit = 15,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Thiên Trì Bí Bảo"] = {
		nTaskID = 3080,
		nStartBit = 1,
		nBitCount = 7,
		nMaxValue = 100,
	},
	["Bảo Rương Kiếm Gia"] = {
		nTaskID = 3080,
		nStartBit = 9,
		nBitCount = 7,
		nMaxValue = 100,
	},
}
function tbvng_ChestExpAward:ExpAward(nValue, strItemName)
	local nTskDayValue = GetTask(self.TSK_DAY)
	local now = tonumber(GetLocalDate("%Y%m%d"))
	local tbBitTSK_Exp = self.tbBitTask[strItemName]
	if not tbBitTSK_Exp then
		return
	end
	local MAX_EXP_PER_DAY
	local nTranslife = ST_GetTransLifeCount()
	if nTranslife <= 4 then
		MAX_EXP_PER_DAY = 50
	elseif nTranslife == 5 then
		MAX_EXP_PER_DAY = 80
	else
		MAX_EXP_PER_DAY = 100
	end
	if (now ~= nTskDayValue) then
		SetTask(self.TSK_DAY, now)
		for key, val in self.tbBitTask do
			%tbVNG_BitTask_Lib:setBitTask(val, 0)
		end
	else
		local nTskExpValue = %tbVNG_BitTask_Lib:getBitTask(tbBitTSK_Exp)
		if nTskExpValue >= MAX_EXP_PER_DAY then
			Msg2Player(format("Hôm nay đã nhận đủ <color=yellow>%d <color>triệu kinh nghiệm, không thể nhận thêm.", MAX_EXP_PER_DAY))
			return
		end
	end
	
	local nTskExpValue = %tbVNG_BitTask_Lib:getBitTask(tbBitTSK_Exp)
	if (nTskExpValue + nValue/1e6) > MAX_EXP_PER_DAY then
		nValue = (MAX_EXP_PER_DAY - nTskExpValue)*1e6
	end
	--SetTask(self.TSK_EXP, GetTask(self.TSK_EXP) + nValue/1e6)
	local nNextValue = nTskExpValue + nValue/1e6
	%tbVNG_BitTask_Lib:setBitTask(tbBitTSK_Exp, nNextValue)
	AddOwnExp(nValue)
	Msg2Player(format("Bạn đã nhận được %d điểm kinh nghiệm không thể cộng dồn", nValue))
	local strTongName, nResult = GetTong()
	if nResult == 0 then
		strTongName = "Không"
	end
	local nMapID, _, _ = GetWorldPos()
	local strItemProp = nValue.." exp"
	local nDaylyExp = %tbVNG_BitTask_Lib:getBitTask(tbBitTSK_Exp) * 1e6
	local strExtraInfo = "Kinh nghiệm đã nhận trong ngày: "..nDaylyExp
	local strLog = GetLocalDate("%Y-%m-%d %H:%M:%S").."\t".."Sử dụng "..strItemName.."\t"..GetAccount().."\t"..GetName().."\t"..GetLevel().."\t"..ST_GetTransLifeCount().."\t".."Nhận kinh nghiệm".."\t".."Kinh nghiệm".."\t"..strItemProp.."\t".."1".."\t"..strTongName.."\t"..nMapID.."\t"..strExtraInfo
	WriteLog(strLog);	
end