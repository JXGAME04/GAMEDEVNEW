-- Author: Fong KiÒu
-- Date: 28/11/2020
--Function: Qu©n quan c«ng thµnh chiÕn

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_vatpham.lua")

NUM_XU_MUA_DC = 100

function main()
	AskDeal()
end

function AskDeal()
	Say("Dông cô hç trî c«ng thµnh chiÕn cùc kú hiÖu dông, chØ cã thÓ dïng TiÒn xu ®Ó mua! Kh¸ch quan nÕu bá lì kh«ng mua th× thËt v« cïng ®¸ng tiÕc!", 4, 
	"NghiÖt Long Xung Xa/#DealBuy(1)", 
	"V©n Kú Binh phï /#DealBuy(2)", 
	"Ngoan Cæ Binh phï /#DealBuy(3)", 
	"Kh«ng cÇn ®©u/OnCancelBuy")
end

function DealBuy(ItemID)
	if (ItemID == 0) then
		SetTaskTemp(TMP_BUY_CTC, 29)
	elseif (ItemID == 1) then
		SetTaskTemp(TMP_BUY_CTC, 30)
	elseif (ItemID == 2) then
		SetTaskTemp(TMP_BUY_CTC, 31)
	elseif (ItemID == 3) then
		SetTaskTemp(TMP_BUY_CTC, 32)
	else
		return
	end
	Say("Do qu¸ nhiÒu ng­êi mua! Kh¸ch quan chØ cã thÓ mua mét lÇn mµ th«i!Cã ®ång ý kh«ng?", 2, "Muèn/DoBuy", "Ta nghÜ l¹i xem /OnCancelBuy")
end

function DoBuy()
	local TheItem = GetTaskTemp(TMP_BUY_CTC)
	if (TheItem >= 29 and TheItem <= 32) then
		if(GetItemCount(0,4,ID_XU_TIENDONG,0) < NUM_XU_MUA_DC) then
			Talk(1,"","CÇn cã "..NUM_XU_MUA_DC.." tiÒn xu trong hµnh trang")
			return
		end		
		if (ConsumeItem(NUM_XU_MUA_DC,0,4,ID_XU_TIENDONG,0) == NUM_XU_MUA_DC) then			
			AddItem(6,1,TheItem,0,0,0,0)
			logHoatDong(format("[%s]\t%s\tAccount:%s\tName:%s\t Mua Vat Pham CTC :%d", " Mua Vat Pham CTC", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), NUM_XU_MUA_DC ))
			Say("Kh¸ch quan thËt biÕt xem hµng!", 0)
		else
			Say("Kh¸ch quan ®ang trªu chäc tiÓu nh©n hay thËt sù kh«ng ®ñ TiÒn xu?", 0)
		end
	end
end

function OnCancelBuy()
	Say("Chóc nghÜa sÜ kú khai ®¾c th¾ng, sím ca khóc kh¶i hoµn!", 0)
end