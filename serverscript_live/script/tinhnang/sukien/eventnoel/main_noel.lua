Include("\\script\\event\\eventnoel\\EventLibNoel.lua");
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\worldlibrary.lua");

Exit = "Ta ph¶i rêi ®i/no";
function main(nNpcindex)
 dofile("script/event/eventnoel/main_noel.lua");
npcgamenoel(nNpcindex)
end

function npcgamenoel(nNpcindex)
	SayEx({"<color=red>T­íng Qu©n<color>: Ho¹t ®éng diÔn ra ®Ó chµo mõng ngµy 20 th¸ng 11",
	"Ta muèn ®æi Ng­êi tuyÕt Th­êng /ExChienCong",
	"Ta muèn ®æi Ng­êi tuyÕt choµng kh¨n xanh  /ExPhatTong",
	"Ta muèn ®æi Ng­êi tuyÕt choµng kh¨n ®á /ExNgaoThe",
	"Ta muèn nhËn mèc Ng­êi tuyÕt choµng kh¨n ®á /ExNgaoThe2000",
	"Ta ®Õn ®Ó t×m hiÓu ho¹t ®éng/help",
	"Ta ph¶i rêi ®i/no"})	
end

FREECELL_TANTHU = 30
function ExNgaoThe2000()
		--local nTaskValuel = GetTask(TASK_MOCEVENT);
		if GetTask(TASK_MOCEVENT) > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! §õng cã ng¸o n÷a !");
		return end;
		if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return  end;
		--local nTaskValue = GetTask(TASK_EVENT2011_3);
		if GetTask(TASK_EVENT2011_3) >= 2000 then
		local nIndex = AddGoldItem(random(164, 167),0)
		--AddItem(0,10,6,10,0,0,0)
		SetTask(TASK_MOCEVENT, 1);
			Msg2SubWorld("<color=green>[Sù KiÖn]<color>Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().."<color> ®· nhËn mèc 2000 c¸i Ng­êi tuyÕt choµng kh¨n ®á nhËn ®­îc <color=yellow>Trang BÞ An Bang NgÉu Nhiªn<color> .")
		else
		Msg2Player("Ng­¬i ch­a sö dông ®ñ 2000 c¸i event Ng­êi tuyÕt choµng kh¨n ®á kh«ng thÓ nhËn.");
	end
end

function help()
	Tab_Help={
	"<color=red>NPC EVENT<color>:<color=yellow>Ng­êi tuyÕt Th­êng tèi ®a 2000:<color>\n-1 Hoa tuyÕt\n-1 Cµ rèt\n-1 Nãn gi¸ng sinh\n-1 V¹n l­îng",
	"<color=red>NPC EVENT<color>:<color=yellow>Ng­êi tuyÕt choµng kh¨n xanh tèi ®a 2000:<color>\n-1 Kh¨n choµng (xanh)\n-1 Cµ rèt\n-1 Nãn gi¸ng sinh\n-5 V¹n l­îng",
	"<color=red>NPC EVENT<color>:<color=yellow>Ng­êi tuyÕt choµng kh¨n ®á tèi ®a 2000:<color>\n-1 Kh¨n choµng (®á)\n-1 Cµ rèt\n-1 Nãn gi¸ng sinh\n-3 V¹n l­îng",
	}
	Talk(3,"",Tab_Help[1],Tab_Help[2],Tab_Help[3]);
end


function ExChienCong()
	local msg = {
		format("Ta muèn ghÐp 1 Ng­êi tuyÕt Th­êng/#ExChienCongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Ng­êi tuyÕt Th­êng/#ExChienCongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Ng­êi tuyÕt Th­êng/#ExChienCongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Ng­êi tuyÕt Th­êng/#ExChienCongok(%d)", 50), 
		"Nh©n tiÖn ghÐ qua th«i/no"
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end
function ExPhatTong()
	local msg = {
		format("Ta muèn ghÐp 1 Ng­êi tuyÕt choµng kh¨n xanh/#ExPhatTongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Ng­êi tuyÕt choµng kh¨n xanh/#ExPhatTongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Ng­êi tuyÕt choµng kh¨n xanh/#ExPhatTongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Ng­êi tuyÕt choµng kh¨n xanh/#ExPhatTongok(%d)", 50), 
		"Nh©n tiÖn ghÐ qua th«i/no"
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end
function ExNgaoThe()
	local msg = {
		format("Ta muèn ghÐp 1 Ng­êi tuyÕt choµng kh¨n ®á /#ExNgaoTheok(%d)", 1), 
		format("Ta muèn ghÐp 5 Ng­êi tuyÕt choµng kh¨n ®á /#ExNgaoTheok(%d)", 5), 
		format("Ta muèn ghÐp 10 Ng­êi tuyÕt choµng kh¨n ®á /#ExNgaoTheok(%d)", 10), 
		format("Ta muèn ghÐp 50 Ng­êi tuyÕt choµng kh¨n ®á/#ExNgaoTheok(%d)", 50), 
		"Nh©n tiÖn ghÐ qua th«i/no"
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end
function ExEvent(nSel)
 local nSel = nSel +1;
	if (nSel == 1) then		
		Input("ExChienCong",20);
	elseif (nSel == 2) then
		Input("ExPhatTong",20);
	elseif (nSel == 3) then
		Input("ExThienDieu",20);
 else
 MsgPlayer("Hack h¶ mµy?");
 return
 end
end
function ExChienCongok(EventCC)
	--local nNum = tonumber(GetInput());
	local Event1 = GetItemCount(0,6,1,TAB_NEED[1][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event2 = GetItemCount(0,6,1,TAB_NEED[2][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event3 = GetItemCount(0,6,1, TAB_NEED[3][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local nCash = 1E4;	
	
	if Event1 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[1][3].."")
		return
	end
	if Event2 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[2][3].."")
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[3][3].."")
		return
	end
	if (GetCash() < nCash*EventCC) then
		Msg2Player("Ng­¬i kh«ng cã ®ñ "..nCash*EventCC.." l­îng");
		return
	end
	
	if (GetItemCount(0,6,1,TAB_NEED[1][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[2][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[3][2]) >= EventCC) then
		DelItem(0, -1, 6,1 ,TAB_NEED[1][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[2][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[3][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		
	end
	Pay(nCash*EventCC);
	for i=1,EventCC do
	AddItem(6,1,TAB_NEED[6][2],0,0,0,0) --- x2 ky nang
	end
	
	Msg2Player("§¹i hiÖp nhËn ®­îc "..EventCC.." "..TAB_NEED[6][3].."");
end
function ExPhatTongok(EventCC)
	--local nNum = tonumber(GetInput());
	local Event1 = GetItemCount(0,6,1,TAB_NEED[4][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event2 = GetItemCount(0,6,1,TAB_NEED[3][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event3 = GetItemCount(0,6,1, TAB_NEED[2][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local nCash = 5E4;		
	
	if Event1 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[4][3].."")
		return
	end
	if Event2 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[5][3].."")
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[6][3].."")
		return
	end
	if (GetCash() < nCash*EventCC) then
		Msg2Player("Ng­¬i kh«ng cã ®ñ "..nCash*EventCC.." l­îng");
		return
	end
	
	if (GetItemCount(0,6,1,TAB_NEED[4][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[3][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[2][2]) >= EventCC) then
		DelItem(0, -1, 6,1 ,TAB_NEED[4][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[3][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[2][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		
	end
	Pay(nCash*EventCC);
	for i=1,EventCC do
	AddItem(6,1,TAB_NEED[7][2],0,0,0,0) --- x2 ky nang
	end
	
	Msg2Player("§¹i hiÖp nhËn ®­îc "..EventCC.." "..TAB_NEED[7][3].."");
end



function ExNgaoTheok(EventCC)
	local nCash = 1E4;	
	local Event1 = GetItemCount(0,6,1,TAB_NEED[5][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event2 = GetItemCount(0,6,1,TAB_NEED[2][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	local Event3 = GetItemCount(0,6,1, TAB_NEED[3][2],-1,-1, pos_equiproom) -- ®Õm sè event trong hµnh trang
	
	if Event1 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[5][3].."")
		return
	end
	if Event2 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[2][3].."")
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[3][3].."")
		return
	end
	if (GetCash() < nCash*EventCC) then
		Msg2Player("Ng­¬i kh«ng cã ®ñ "..nCash*EventCC.." l­îng");
		return
	end
	
	if (GetItemCount(0,6,1,TAB_NEED[5][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[2][2]) >= EventCC) and (GetItemCount(0,6,1,TAB_NEED[3][2]) >= EventCC) then
		DelItem(0, -1, 6,1 ,TAB_NEED[5][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[2][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		DelItem(0, -1, 6,1, TAB_NEED[3][2], -1, -1, ROOME, EventCC) -- xo¸ sè Event trong hµnh trang
		
	end
	Pay(nCash*EventCC);
	for i=1,EventCC do
	AddItem(6,1,TAB_NEED[8][2],0,0,0,0) --- x2 ky nang
	end
	
	Msg2Player("§¹i hiÖp nhËn ®­îc "..EventCC.." "..TAB_NEED[8][3].."");
end


function no()
end