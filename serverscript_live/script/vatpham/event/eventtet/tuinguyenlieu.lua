Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nItemIndex)
	local szEvent ={
		"C«u",
		"Dıa",
		"ßÒ",
		"Xoµi",
	}
	local nRanEvent = random(4836,4839);	
	-- if (nRanEvent == 4836) then
		-- Msg2Player("ßπi Hi÷p nhÀn Æ≠Óc <color=green>C«u<color> ");	
	-- elseif (nRanEvent == 4837) then
		-- Msg2Player("ßπi Hi÷p nhÀn Æ≠Óc <color=green>Dıa<color> ");	
	-- elseif (nRanEvent == 4838) then
		-- Msg2Player("ßπi Hi÷p nhÀn Æ≠Óc <color=green>ßÒ<color> ");	
	-- elseif (nRanEvent == 4839) then
		-- Msg2Player("ßπi Hi÷p nhÀn Æ≠Óc <color=green>Xoµi<color> ");	
	-- end;
	AddItem( 6, 1,nRanEvent,0,0,0,0)
	RemoveItem(nItemIndex,1);
end
