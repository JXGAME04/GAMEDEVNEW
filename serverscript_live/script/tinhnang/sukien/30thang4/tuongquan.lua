-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local npctalk
Include( "\\script\\event\\30thang4\\EventLib.lua");
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\worldlibrary.lua");

Exit = "Ta ph¶i rêi ®i/no";
function main(nNpcindex)
 dofile("script/event/30thang4/Tuongquan.lua");
npctalk(nNpcindex)
end

function npctalk(nNpcindex)
	SayEx({"<color=red>T­íng Qu©n<color>: Ho¹t ®éng diÔn ra ®Ó chµo mõng ngµy chiÕn th¾ng trªn c¶ n­íc. Ta chÞu tr¸ch nhiÖm ph¸t th­ëng cho c¸c ®¹i hiÖp cã c«ng lao to lín víi triÒu ®×nh",
	"Ta muèn ®æi Huy Ch­¬ng ChiÕn C«ng/ExChienCong",
	"Ta muèn ®æi Huy Ch­¬ng Ph¹t Tèng/ExPhatTong",
	"Ta muèn ®æi Huy Ch­¬ng Ng¹o ThÕ/ExNgaoThe",
	"Ta muèn nhËn mèc Event Huy Ch­¬ng Ph¹t Tèng/ExPhatTong2000",
	"Ta muèn nhËn mèc Event Huy Ch­¬ng Ng¹o ThÕ/ExNgaoThe2000",
	"Ta muèn nhËn Reset Huy Ch­¬ng Ph¹t Tèng/ResetPhatTong2000",
	"Ta muèn nhËn Reset Huy Ch­¬ng Ng¹o ThÕ/ResetNgaoThe2000",
	"Ta ®Õn ®Ó t×m hiÓu ho¹t ®éng/help",
	"Ta ph¶i rêi ®i/no"})	

end
function ResetPhatTong2000()
	if GetTask(TASK_FANCUNG_4) > 0 then  
		Talk(1,"","§¹i hiÖp ®· reset Event nµy råi") 
		return 
	end
	if GetTask(TASK_DUPHONG6) <= 0 then  
		Talk(1,"","§¹i hiÖp ch­a nhËn mèc th­ëng Event ") 
		return 
	end
	if GetTask(TASK_EVENT3004_2) < 2000 then 
	Talk(1,"","Ng­¬i ch­a sö dông ®ñ 2000 c¸i Event Huy Ch­¬ng Ph¹t Tèng kh«ng thÓ nhËn. ") 
		return 
	end 
	if (GetItemCount(0,6,1,4835,-1,-1, pos_equiproom) < 20) then
	Talk(1,"","Kh«ng mang ®ñ 20 TiÒn §ång, vui lßng kiÓm tra l¹i") 
	return end

		SetTask(TASK_DUPHONG6, 0);
		SetTask(TASK_EVENT3004_2, 0);
		SetTask(TASK_FANCUNG_4, 1);
		DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, 20) -- xo¸ sè Event trong hµnh trang
		
		Msg2Player("Ng­¬i ®· Reset Event Huy Ch­¬ng Ph¹t Tèng thµnh c«ng !");
end
function ResetNgaoThe2000()
	if GetTask(TASK_FANCUNG_3) > 0 then  
		Talk(1,"","§¹i hiÖp ®· reset Event nµy råi") 
		return 
	end
	if GetTask(TASK_MOCEVENT) <= 0 then  
		Talk(1,"","§¹i hiÖp ch­a nhËn mèc th­ëng Event ") 
		return 
	end
	if GetTask(TASK_EVENT3004_3) < 2000 then 
	Talk(1,"","Ng­¬i ch­a sö dông ®ñ 2000 c¸i event Ng¹o ThÕ kh«ng thÓ nhËn. ") 
		return 
	end 
	if (GetItemCount(0,6,1,4835,-1,-1, pos_equiproom) < 30) then
	Talk(1,"","Kh«ng mang ®ñ 30 TiÒn §ång, vui lßng kiÓm tra l¹i") 
	return end

		SetTask(TASK_MOCEVENT, 0);
		SetTask(TASK_EVENT3004_3, 0);
		SetTask(TASK_FANCUNG_3, 1);
		DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, 30) -- xo¸ sè Event trong hµnh trang
		Msg2Player("Ng­¬i ®· Reset Event Ng¹o ThÕ thµnh c«ng !");
end

FREECELL_TANTHU = 30
function ExNgaoThe2000()
		local nTaskValuel = GetTask(TASK_MOCEVENT);
		if nTaskValuel > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! §õng cã ng¸o n÷a !");
		return end;
		if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return  end;
		local nTaskValue = GetTask(TASK_EVENT3004_3);
		if nTaskValue >= 2000 then
		local nIndex = AddItem2(2,0,random(163,166),0,0,0) -- An Bang
		SetTask(TASK_MOCEVENT, 1);
			local s_name = GetItemName(nIndex)
			AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· nhËn mèc 2000 c¸i Event Ng¹o ThÕ nhËn ®­îc <color=green>"..s_name.." <color=red>! ",3)
			Msg2SubWorld("<color=green>[Sù KiÖn]<color>Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().."<color> ®· nhËn mèc 2000 c¸i Event Ng¹o ThÕ nhËn ®­îc <color=yellow>"..s_name..". <color> .")
		else
		Msg2Player("Ng­¬i ch­a sö dông ®ñ 2000 c¸i event Ng¹o ThÕ kh«ng thÓ nhËn. TiÕn ®é <color=yellow>"..nTaskValue.."/ 2000<color>");
	end
end
function ExPhatTong2000()
		local nTaskValuel = GetTask(TASK_DUPHONG6);
		if nTaskValuel > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! §õng cã ng¸o n÷a !");
		return end;
		if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return  end;
		local nTaskValue = GetTask(TASK_EVENT3004_2);
		if nTaskValue >= 2000 then
		local nIndex = AddItem2(2,0,random(158,162),0,0,0) -- dinh quoc	
		SetTask(TASK_DUPHONG6, 1);
			local s_name = GetItemName(nIndex)
			AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· nhËn mèc 2000 c¸i Huy Ch­¬ng Ph¹t Tèng nhËn ®­îc <color=green>"..s_name.." <color=red>! ",3)
			Msg2SubWorld("<color=green>[Sù KiÖn]<color>Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().."<color> ®· nhËn mèc 2000 c¸i Huy Ch­¬ng Ph¹t Tèng nhËn ®­îc <color=yellow>"..s_name..". <color> .")
		else
		Msg2Player("Ng­¬i ch­a sö dông ®ñ 2000 c¸i Huy Ch­¬ng Ph¹t Tèng kh«ng thÓ nhËn. TiÕn ®é <color=yellow>"..nTaskValue.."/ 2000<color>");
	end
end
function help()
	Tab_Help={
	"<color=red>T­íng Qu©n<color>:<color=yellow>Huy Ch­¬ng ChiÕn C«ng cÇn tèi ®a 2000:<color>\n-1 Huy Ch­¬ng Vµng\n-1 Ng«i Sao ChiÕn Th¾ng\n-1 C«ng Tr¸ng LÖnh\n-1 V¹n l­îng",
	"<color=red>T­íng Qu©n<color>:<color=yellow>Huy Ch­¬ng Ph¹t Tèng cÇn tèi ®a 2000:<color>\n-1 Huy Ch­¬ng Tèng Kim\n-1 Ng«i Sao ChiÕn Th¾ng\n-1 C«ng Tr¸ng LÖnh\n-3 V¹n l­îng",
	"<color=red>T­íng Qu©n<color>:<color=yellow>Huy Ch­¬ng Ng¹o ThÕ cÇn tèi ®a 2000:<color>\n-1 B¶n §å Chinh Ph¹t\n-1 Ng«i Sao ChiÕn Th¾ng\n-1 C«ng Tr¸ng LÖnh\n-1 V¹n l­îng",
	}
	Talk(3,"",Tab_Help[1],Tab_Help[2],Tab_Help[3]);
end


function ExChienCong()
	local msg = {
		format("Ta muèn ghÐp 1 Event ChiÕn C«ng/#ExChienCongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Event ChiÕn C«ng/#ExChienCongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Event ChiÕn C«ng/#ExChienCongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Event ChiÕn C«ng/#ExChienCongok(%d)", 50), 
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
		format("Ta muèn ghÐp 1 Event Ph¹t Tèng/#ExPhatTongok(%d)", 1), 
		format("Ta muèn ghÐp 5 Event Ph¹t Tèng/#ExPhatTongok(%d)", 5), 
		format("Ta muèn ghÐp 10 Event Ph¹t Tèng/#ExPhatTongok(%d)", 10), 
		format("Ta muèn ghÐp 50 Event Ph¹t Tèng/#ExPhatTongok(%d)", 50), 
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
		format("Ta muèn ghÐp 1 Event Ký Sù/#ExNgaoTheok(%d)", 1), 
		format("Ta muèn ghÐp 5 Event Ký Sù/#ExNgaoTheok(%d)", 5), 
		format("Ta muèn ghÐp 10 Event Ký Sù/#ExNgaoTheok(%d)", 10), 
		format("Ta muèn ghÐp 50 Event Ký Sù/#ExNgaoTheok(%d)", 50), 
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
	local nCash = 3E4;		
	
	if Event1 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[4][3].."") --Huy Ch­¬ng Tèng Kim
		return
	end
	if Event2 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[3][3].."") --C«ng Tr¸ng LÖnh 
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[2][3].."")  --Ng«i Sao ChiÕn Th¾ng
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
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[5][3].."") --B¶n §å Chinh Ph¹t
		return
	end
	if Event2 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[2][3].."") --Ng«i Sao ChiÕn Th¾ng
		return
	end
	if Event3 < EventCC then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..EventCC.." "..TAB_NEED[3][3].."")  -- C«ng Tr¸ng LÖnh
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