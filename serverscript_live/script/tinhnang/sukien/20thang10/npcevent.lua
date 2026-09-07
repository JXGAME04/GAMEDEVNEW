-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local npcgame
Include( "\\script\\event\\20thang10\\EventLib2010.lua");
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\worldlibrary.lua");

Exit = "Ta ph¶i rêi ®i/no";
function main(nNpcindex)
 dofile("script/event/20thang10/npcevent.lua");
npcgame(nNpcindex)
end

function npcgame(nNpcindex)
	SayEx({"<color=red>T­íng Qu©n<color>: Ho¹t ®éng diÔn ra ®Ó chµo mõng ngµy 20 th¸ng 11",
	"Ta muèn ®æi Bã Hång Tam S¾c /ExChienCong",
	"Ta muèn ®æi Bã Hång Tø S¾c /ExPhatTong",
	"Ta muèn ®æi Giá Hoa Hång VÜnh Cöu /ExNgaoThe",
	"Ta muèn nhËn mèc Giá Hoa Hång VÜnh Cöu /ExNgaoThe2000",
	"Ta ®Õn ®Ó t×m hiÓu ho¹t ®éng/help",
	"Ta ph¶i rêi ®i/no"})	
end

FREECELL_TANTHU = 30
function ExNgaoThe2000()
		local nTaskValuel = GetTask(TASK_MOCEVENT);
		if GetNumber(1,nTaskValuel,2) > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! §õng cã ng¸o n÷a !");
		return end;
		if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return  end;
		local nTaskValue = GetTask(TASK_EVENT2011_3);
		if GetNumber(4,nTaskValue,1) >= 2000 then
		local nIndex = AddGoldItem(random(164, 167),0)
		AddItem(0,10,6,10,0,0,0)
		SetTask(TASK_MOCEVENT, SetNumber(1,nTaskValue,2,1));
		Msg2SubWorld("<color=green>[Sù KiÖn]<color>Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().."<color> ®· nhËn mèc 2000 c¸i Giá Hoa Hång VÜnh Cöu nhËn ®­îc <color=yellow>Trang BÞ An Bang NgÉu Nhiªn vµ Ngùa B«n Tiªu<color> .")
		thoigian = tonumber(date("%H%M%d%m"))
		LoginLog = openfile("dulieu/NhanMocEvent.txt", "a");
		if LoginLog then
		write(LoginLog,"Tµi kho¶n: "..GetAccount().." - Tªn nh©n vËt: "..GetName().." - CÊp: "..GetLevel().." - nhËn mèc thµnh c«ng \n");
		end
		closefile(LoginLog)
		else
		Msg2Player("Ng­¬i ch­a sö dông ®ñ 2000 c¸i event Giá Hoa Hång VÜnh Cöu kh«ng thÓ nhËn.");
	end
end

function help()
	Tab_Help={
	"<color=red>NPC EVENT<color>:<color=yellow>Bã Hång Tam S¾c tèi ®a 2000:<color>\n-1 Hoa Hång Tr¾ng\n-1 Hoa Hång Xanh\n-1 Hoa Hång Vµng\n-1 V¹n l­îng",
	"<color=red>NPC EVENT<color>:<color=yellow>Bã Hång Tø S¾c tèi ®a 2000:<color>\n-1 Hoa hång ®á\n-1 Hoa Hång Xanh\n-1 Hoa Hång Vµng\n-5 V¹n l­îng",
	"<color=red>NPC EVENT<color>:<color=yellow>Giá Hoa Hång VÜnh Cöu tèi ®a 2000:<color>\n-1 Giá §ùng Hoa\n-1 Hoa Hång Xanh\n-1 Hoa Hång Vµng\n-3 V¹n l­îng",
	}
	Talk(3,"",Tab_Help[1],Tab_Help[2],Tab_Help[3]);
end


function ExChienCong()
	local msg = {
		format("Ta muèn ghÐp 1 Bã Hång Tam S¾c/#ExChienCongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Bã Hång Tam S¾c/#ExChienCongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Bã Hång Tam S¾c/#ExChienCongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Bã Hång Tam S¾c/#ExChienCongok(%d)", 50), 
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
		format("Ta muèn ghÐp 1 Bã Hång Tø S¾c/#ExPhatTongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Bã Hång Tø S¾c/#ExPhatTongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Bã Hång Tø S¾c/#ExPhatTongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Bã Hång Tø S¾c/#ExPhatTongok(%d)", 50), 
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
		format("Ta muèn ghÐp 1 Giá Hoa Hång VÜnh Cöu /#ExNgaoTheok(%d)", 1), 
		format("Ta muèn ghÐp 5 Giá Hoa Hång VÜnh Cöu /#ExNgaoTheok(%d)", 5), 
		format("Ta muèn ghÐp 10 Giá Hoa Hång VÜnh Cöu /#ExNgaoTheok(%d)", 10), 
		format("Ta muèn ghÐp 50 Giá Hoa Hång VÜnh Cöu /#ExNgaoTheok(%d)", 50), 
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
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[3][3].."")
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[2][3].."")
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