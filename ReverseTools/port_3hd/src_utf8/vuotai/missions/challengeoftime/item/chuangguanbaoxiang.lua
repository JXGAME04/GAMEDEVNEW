-- 闯关宝箱物品（随机获得一样物品）
-- By: Wangjingjun(2011-03-02)

Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\vng_event\\change_request_baoruong\\exp_award.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\task\\metempsychosis\\translife_6.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")
local  _Message =  function (nItemIndex)
	local handle = OB_Create()
	local msg = format("<color=green>Chúc mừng cao thủ <color=yellow>%s<color=green> đã nhận được <color=yellow><%s><color=green> từ <color=yellow><Bảo Rương Vượt ải><color>" ,GetName(),GetItemName(nItemIndex))
	ObjBuffer:PushObject(handle, msg)
	RemoteExecute("\\script\\event\\msg2allworld.lua", "broadcast", handle)
	OB_Release(handle)
end

tbCOT_Key_Require = {
	["chiakhoanhuy"] = {6, 1, 2744},
	["chiakhoavang"] = {6, 1, 30191},
}
tbCOT_Box_Award = 
{
	["chiakhoanhuy"] = 
	{
		{szName="Điểm kinh nghiệm 1", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(1000000, "Bảo rương vượt ải")
					end,
					nRate = 52,
		},
		{szName="Điểm kinh nghiệm 2", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(2000000, "Bảo rương vượt ải")
					end,
					nRate = 30,
		},
		{szName="Điểm kinh nghiệm 3", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(3000000, "Bảo rương vượt ải")
					end,
					nRate = 10,
		},
		{szName="Điểm kinh nghiệm 4", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(4000000, "Bảo rương vượt ải")
					end,
					nRate = 5,
		},
		{szName="Điểm kinh nghiệm 5", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(5000000, "Bảo rương vượt ải")
					end,
					nRate = 3,
		},		
	},	
	-- Thay đổi phần thưởng sử dụng Bảo rương vượt ải  -Modifiled by ThanhLD - 20140226
	["chiakhoavang"] = 
	{
		{szName="Chân Nguyên Đan",tbProp={6,1,4134,1,0,0},nCount=10,nRate=5},
		{szName="Chân Nguyên Đơn (trung)",tbProp={6,1,30228,1,0,0},nCount=7,nRate=10,nBindState=-2},
		{szName="Chân Nguyên Đơn (đại)",tbProp={6,1,30229,1,0,0},nCount=7,nRate=5,nBindState=-2},
		{szName="Hỗn nguyên chân đơn",tbProp={6,1,30301,1,0,0},nCount=1,nRate=0.01},
		{szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nCount=50,nRate=10},
		{szName="Huyết Long Đằng Cấp 9",tbProp={6,1,30289,9,0,0},nCount=5,nRate=1.2},
		{szName="Huyết Long Đằng Cấp 11",tbProp={6,1,30289,11,0,0},nCount=5,nRate=1.1},
		{szName="Huyết Long Đằng Cấp 12",tbProp={6,1,30289,12,0,0},nCount=5,nRate=0.5},
		{szName="Tinh Tinh Khoáng",tbProp={6,1,3811,1,0,0},nCount=1,nRate=1},
		{szName="Tinh Thiết Khoáng",tbProp={6,1,3810,1,0,0},nCount=1,nRate=0.5},
		{szName = "Đồ Phổ Đằng Long Y", tbProp = {6, 1, 30529,1,0,0}, nRate = 0.002,},
		{szName = "Đồ Phổ Đằng Long Khí Giới", tbProp = {6, 1, 30537,1,0,0}, nRate = 0.001,},
		{szName = "Tinh Sương Lệnh", tbProp = {6, 1, 30506,1,0,0}, nRate = 0.007,},
		{szName = "Huyền Thiết", tbProp = {6, 1, 30507,1,0,0}, nRate = 0.03,},
		{szName = "Đồ Phổ Tinh Sương Y", tbProp = {6, 1, 30006,1,0,0}, nRate = 0.007,},
		{szName = "Đồ Phổ Tinh Sương Khí Giới", tbProp = {6, 1, 30505,1,0,0}, nRate = 0.006,},
		{szName="Điểm kinh nghiệm 1", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(2000000,"Bảo rương vượt ải")
					end,
					nRate = 29.637,
		},
		{szName="Điểm kinh nghiệm 2", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(4000000,"Bảo rương vượt ải")
					end,
					nRate = 15,
		},
		{szName="Điểm kinh nghiệm 3", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(5000000,"Bảo rương vượt ải")
					end,
					nRate = 10,
		},
		{szName="Điểm kinh nghiệm 4", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(6000000,"Bảo rương vượt ải")
					end,
					nRate = 5,
		},
		{szName="Điểm kinh nghiệm 5", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(8000000,"Bảo rương vượt ải")
					end,
					nRate = 4,
		},	
		{szName="Điểm kinh nghiệm 6", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(10000000,"Bảo rương vượt ải")
					end,
					nRate = 2,
		},
	},
}

nWidth = 1
nHeight = 1
nFreeItemCellLimit = 1

function main(nIndexItem)	
	local tbKey1 = tbCOT_Key_Require["chiakhoanhuy"]
	local tbKey2 = tbCOT_Key_Require["chiakhoavang"]
	local nCount1 = CalcItemCount(3, tbKey1[1], tbKey1[2], tbKey1[3], -1) 
	local nCount2 = CalcItemCount(3, tbKey2[1], tbKey2[2], tbKey2[3], -1) 
	if nCount1 == 0 and nCount2 == 0 then
		Say("Cần phải có Chìa Khóa Vàng hoặc Chìa Khóa Như ý mới có thể mở được Bảo Rương Vượt ải", 1, "Đóng/no")
		return 1
	end

	if CountFreeRoomByWH(nWidth, nHeight, nFreeItemCellLimit) < nFreeItemCellLimit then
		Say(format("Để bảo đảm tài sản của đại hiệp, xin hãy để trống %d %dx%d hành trang", nFreeItemCellLimit, nWidth, nHeight))
		return 1
	end	
	local tbOpt = {}
	if nCount1 ~= 0 then
		tinsert(tbOpt,format("Sử dụng Chìa khóa như ý/#VnCOTBoxNewAward(%d, '%s')", nIndexItem, "chiakhoanhuy"))
	end
	if nCount2 ~= 0 then
		tinsert(tbOpt,format("Sử dụng Chìa khóa vàng/#VnCOTBoxNewAward(%d, '%s')", nIndexItem, "chiakhoavang"))
	end
	if getn(tbOpt) > 0 then
		tinsert(tbOpt, "Đóng/Oncancel")
		Say("Sử dụng chìa khóa để mở rương:", getn(tbOpt), tbOpt)
	end
	return 1
end

function Oncancel()end

function VnCOTBoxNewAward(nItemIdx, strKeyType)
	local tbKey = tbCOT_Key_Require[strKeyType]
	local tbAward = tbCOT_Box_Award[strKeyType]
	if not tbKey or not tbAward then
		return
	end
	if ConsumeItem(3, 1, tbKey[1], tbKey[2], tbKey[3], -1) ~= 1 then
		Say("Cần phải có Chìa Khóa Vàng hoặc Chìa Khóa Như ý mới có thể mở được Bảo Rương Vượt ải", 1, "Đóng/no")
		return
	end
	
	if ConsumeItem(3, 1, 6, 1, 2742, -1) ~= 1 then
		Say("Không tìm thấy Bảo Rương Vượt ải", 1, "Đóng/no")
		return
	end
	
	if strKeyType == "chiakhoavang" then
		%TransLife6:OnFinishEvent(%TASK_ID_BOX)
	end
	
	tbAwardTemplet:Give(tbAward, 1, {"chuangguan", "use chuangguanbaoxiang"})
	AddStatData("baoxiangxiaohao_kaichuangguanbaoxiang", 1)	--数据埋点第一期
	EventSys:GetType("OpenFuncAwardBox"):OnPlayerEvent("OpenAwardBoxEvent", PlayerIndex)
end
