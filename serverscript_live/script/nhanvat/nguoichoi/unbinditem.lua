-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local motrangbi_f
--Author: Fong KiÒu
--Date: 2021
--Function: Th¸o trang bÞ --GetPlayerItemIsLock tr¹ng th¸i kho¸ 0 kh«ng 1 kho¸ -2 kho¸ vv
-- GetPlayerItemSecondLock thêi gian kho¸ cßn l¹i tÝnh b»ng gi©y

Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_ham.lua")

function main(itemidx)
	
	-- dofile("script/player/unbinditem.lua")
	
	if(itemidx <= 0) then return end
	local szItemName = GetItemName(itemidx)
	local StrTab = {"<sex> cã muèn më kho¸ b¶o hiÓm "..szItemName.." ? "}
	
	if(GetPlayerItemIsLock(itemidx) == 1 and GetPlayerItemSecondLock(itemidx) == 0) then
		tinsert(StrTab,format("KÝch ho¹t më./#motrangbi(%d)",itemidx))
	elseif(GetPlayerItemIsLock(itemidx) == 1 and GetPlayerItemSecondLock(itemidx) < 0) then
		Msg2Player("GetPlayerItemIsLock"..GetPlayerItemIsLock(itemidx))
		motrangbi_f(itemidx)
	else
		return
	end
	tinsert(StrTab,"Tho¸t/no")
	SayEx(StrTab)
end

function motrangbi_f(itemidx2)
	if(itemidx2 <= 0) then return end
	local szItemName = GetItemName(itemidx2)
	if (GetPlayerItemSecondLock(itemidx2) > 0) then
		Talk(1,"","VËt phÈm ®ang trong thêi gian chê më kho¸!")
		return
	end
	local res = SetPlayerItemUnLockF(itemidx2)
	if(res==1) then
		--Talk(1,"","§· më kho¸ b¶o hiÓm.")
		Msg2Player("§· më kho¸ b¶o hiÓm "..szItemName..". ")
		logWriteBind(format("Account [%s] Player [%s] ®· më kho¸ b¶o hiÓm vËt phÈm [%s] thµnh c«ng",GetAccount(),GetName(),szItemName))
	end
end

function no()
end

function motrangbi(itemidx2)
	if(itemidx2 <= 0) then return end
	local szItemName = GetItemName(itemidx2)
	if (GetPlayerItemSecondLock(itemidx2) > 0) then
		--Talk(1,"","VËt phÈm "..szItemName.." ®ang trong thêi gian chê më kho¸!")
		return
	end
	local res = SetPlayerItemUnLock(itemidx2)
	if(res==1) then
		Talk(1,"","§· më kho¸ b¶o hiÓm "..szItemName.." thêi gian chê më kho¸ 186 giê.")
		logWriteBind(format("Account [%s] Player [%s] ®· më kho¸ b¶o hiÓm vËt phÈm [%s] thêi gian chê më 186 giê",GetAccount(),GetName(),szItemName))
	end
end

function no()
end