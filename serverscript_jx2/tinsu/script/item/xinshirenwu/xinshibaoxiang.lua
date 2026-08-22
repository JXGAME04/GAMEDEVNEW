Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\vng_event\\change_request_baoruong\\exp_award.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")
function GetDesc(nItem)
	return ""
end
-- Thªm phÇn sö dông ch×a khãa vµng  -Modifiled by ThanhLD - 20130417	
tbXS_Key_Require = {
	["chiakhoanhuy"] = {6, 1, 3362},
	["chiakhoavang"] = {6, 1, 2953},
}
tbXS_Box_Award = 
{
	["chiakhoanhuy"] = 
	{
    		{szName="§iÓm kinh nghiÖm 1", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(1000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 52,
		},
		{szName="§iÓm kinh nghiÖm 2", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(2000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 30,
		},
		{szName="§iÓm kinh nghiÖm 3", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(3000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 10,
		},
		{szName="§iÓm kinh nghiÖm 4", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(4000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 5,
		},
		{szName="§iÓm kinh nghiÖm 5", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(5000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 3,
		},
    	},	
    	-- Thay ®æi phÇn th­ëng sö dông TÝn Sø B¶o R­¬ng  -Modifiled by ThanhLD - 20140226 
	["chiakhoavang"] = 
	{
		{szName="Ch©n Nguyªn §an",tbProp={6,1,4752,1,0,0},nCount=10,nRate=5},
		{szName="Ch©n Nguyªn §¬n (trung)",tbProp={6,1,4846,1,0,0},nCount=7,nRate=10,nBindState=-2},
		{szName="Ch©n Nguyªn §¬n (®¹i)",tbProp={6,1,4847,1,0,0},nCount=7,nRate=5,nBindState=-2},
		{szName="Hçn nguyªn ch©n ®¬n",tbProp={6,1,4857,1,0,0},nCount=1,nRate=0.01},
		{szName="Hé M¹ch §¬n",tbProp={6,1,4844,1,0,0},nCount=50,nRate=10},
		{szName="HuyÕt Long §»ng CÊp 9",tbProp={6,1,4848,9,0,0},nCount=5,nRate=1.2},
		{szName="HuyÕt Long §»ng CÊp 11",tbProp={6,1,4848,11,0,0},nCount=5,nRate=1.1},
		{szName="HuyÕt Long §»ng CÊp 12",tbProp={6,1,4848,12,0,0},nCount=5,nRate=0.5},
		{szName="Tinh Tinh Kho¸ng",tbProp={6,1,4429,1,0,0},nCount=1,nRate=1},
		{szName="Tinh ThiÕt Kho¸ng",tbProp={6,1,4428,1,0,0},nCount=1,nRate=0.5},
		{szName = "§å Phæ §»ng Long Y", tbProp = {6, 1, 4858,1,0,0}, nRate = 0.002,},
		{szName = "§å Phæ §»ng Long KhÝ Giíi", tbProp = {6, 1, 4859,1,0,0}, nRate = 0.001,},
		{szName = "Tinh S­¬ng LÖnh", tbProp = {6, 1, 4860,1,0,0}, nRate = 0.007,},
		{szName = "HuyÒn ThiÕt", tbProp = {6, 1, 4861,1,0,0}, nRate = 0.03,},
		{szName = "§å Phæ Tinh S­¬ng Y", tbProp = {6, 1, 4862,1,0,0}, nRate = 0.007,},
		{szName = "§å Phæ Tinh S­¬ng KhÝ Giíi", tbProp = {6, 1, 4863,1,0,0}, nRate = 0.006,},
		{szName="§iÓm kinh nghiÖm 1", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(2000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 29.637,
		},
		{szName="§iÓm kinh nghiÖm 2", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(4000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 15,
		},
		{szName="§iÓm kinh nghiÖm 3", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(5000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 10,
		},
		{szName="§iÓm kinh nghiÖm 4", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(6000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 5,
		},
		{szName="§iÓm kinh nghiÖm 5", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(8000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 4,
		},
		{szName="§iÓm kinh nghiÖm 6", 
					pFun = function (tbItem, nItemCount, szLogTitle)
						%tbvng_ChestExpAward:ExpAward(10000000, "TÝn Sø B¶o R­¬ng")
					end,
					nRate = 2,
		},	
	},
}

nWidth = 1
nHeight = 1
nFreeItemCellLimit = 1

function main(nIndexItem)	
	local tbKey1 = tbXS_Key_Require["chiakhoanhuy"]
	local tbKey2 = tbXS_Key_Require["chiakhoavang"]
	local nCount1 = CalcItemCount(3, tbKey1[1], tbKey1[2], tbKey1[3], -1) 
	local nCount2 = CalcItemCount(3, tbKey2[1], tbKey2[2], tbKey2[3], -1) 
	if nCount1 == 0 and nCount2 == 0 then
		Say("CÇn ph¶i cã Ch×a Khãa Vµng hoÆc Ch×a Khãa Nh­ ý míi cã thÓ më ®­îc B¶o R­¬ng TÝn Sø", 1, "§ãng/no")
		return 1
	end

	if CountFreeRoomByWH(nWidth, nHeight, nFreeItemCellLimit) < nFreeItemCellLimit then
		Say(format("§Ó b¶o ®¶m tµi s¶n cña ®¹i hiÖp, xin h·y ®Ó trèng %d %dx%d hµnh trang", nFreeItemCellLimit, nWidth, nHeight))
		return 1
	end	
	local tbOpt = {}
	if nCount1 ~= 0 then
		tinsert(tbOpt,format("Sö dông Ch×a khãa nh­ ý/#VnXSBoxNewAward(%d, '%s')", nIndexItem, "chiakhoanhuy"))
	end
	if nCount2 ~= 0 then
		tinsert(tbOpt,format("Sö dông Ch×a khãa vµng/#VnXSBoxNewAward(%d, '%s')", nIndexItem, "chiakhoavang"))
	end
	if getn(tbOpt) > 0 then
		tinsert(tbOpt, "§ãng/Oncancel")
		Say("Sö dông ch×a khãa ®Ó më r­¬ng:", getn(tbOpt), tbOpt)
	end
	return 1
end

function Oncancel()end

function VnXSBoxNewAward(nItemIdx, strKeyType)
	local tbKey = tbXS_Key_Require[strKeyType]
	local tbAward = tbXS_Box_Award[strKeyType]
	if not tbKey or not tbAward then
		return
	end
	if ConsumeItem(3, 1, tbKey[1], tbKey[2], tbKey[3], -1) ~= 1 then
		Say("CÇn ph¶i cã Ch×a Khãa Vµng hoÆc Ch×a Khãa Nh­ ý míi cã thÓ më ®­îc B¶o R­¬ng TÝn Sø", 1, "§ãng/no")
		return
	end
	
	if ConsumeItem(3, 1, 6, 1, 3430, -1) ~= 1 then
		Say("Kh«ng t×m thÊy B¶o R­¬ng TÝn Sø", 1, "§ãng/no")
		return
	end		
	tbAwardTemplet:Give(tbAward, 1, {"TinSu", "SuDungBaoRuongTinSu"})
	EventSys:GetType("OpenFuncAwardBox"):OnPlayerEvent("OpenAwardBoxEvent", PlayerIndex)	
end 
