--Author: Fong Ki“u
--Date: 2021
--Function: ß›nh trang bﬁ

Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_ham.lua")

function main(itemidx)
	
	dofile("script/player/binditem.lua")
	
	if(itemidx <= 0) then 
		return
	end
	
	if (GetPlayerItemIsLock(itemidx) == 1 and GetPlayerItemSecondLock(itemidx) == 0) then 
		return
	end
	
	if (GetPlayerItemIsLock(itemidx) == -2) then
		return
	end
	
	local szItemName = GetItemName(itemidx)	
	SayEx(
		{"<sex> c„ muËn kho∏ b∂o hi”m "..szItemName.."?",
		format("Kho∏ lπi./#khoatrangbi(%d)", itemidx),
		"Kh´ng./no"}
		)
end

function khoatrangbi(itemidx2)
	local typelock = 1 --ki”u kho∏ trang bﬁ 1 kho∏ c„ th” mÎ, -2 kho∏ v‹nh vi‘n
	local res = SetPlayerItemLock(itemidx2, typelock)
	local szItemName = GetItemName(itemidx2)
	if(res == 1) then
		Talk(1,"","ß∑ kho∏ b∂o hi”m "..szItemName..".")
		logWriteBind(format("Account [%s] Player [%s] Æ∑ kho∏ b∂o hi”m vÀt ph»m [%s] thµnh c´ng",GetAccount(),GetName(),szItemName))
	end
end

function no()
end