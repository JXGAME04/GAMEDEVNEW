--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö Thiªn V­¬ng ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName="ÌìÍõ°ï"

function main()
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(3, "enroll_select", "Bang chñ D­¬ng Anh vâ nghÖ cao c­êng, gan d¹ h¬n ng­êi, kh«ng kĞm g× tu mi nam tö!", "Bæn m«n quy luËt uy nghiªm, ng­êi ®«ng thÕ m¹nh! Kh«ng ai trong thiªn h¹ d¸m coi khinh!", "Bæn bang võa chèng Kim võa kh¸ng Tèng! Huynh ®Ö ®Òu xuÊt th©n lµ d©n nghÌo, phiªu b¹t giang hå lµ v× kÕ sinh nhai! Ai lµm vua bän ta ch¶ cÇn quan t©m")
	elseif (player_Faction == "ÌìÍõ°ï") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: H¶o huynh ®Ö! Chóng ta l¹i gÆp nhau råi! Cã c¬ héi ta sÏ uèng r­îu cïng nhau!",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: H¶o huynh ®Ö! Chóng ta l¹i gÆp nhau råi! Cã c¬ héi ta sÏ uèng r­îu cïng nhau!",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: H¶o huynh ®Ö! Chóng ta l¹i gÆp nhau råi! Cã c¬ héi ta sÏ uèng r­îu cïng nhau!",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
end
end

function enroll_select()
if (GetSeries() == 0) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then					--µÈ¼¶´ïµ½Ê®¼¶
			Say("Gia nhËp bæn bang, chóng ta sÏ lµ huynh ®Ö mét nhµ, häa phóc cïng h­ëng!", 2, "Gia nhËp Thiªn V­¬ng Bang/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn <color=Red>cÊp 10<color> l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(1)
end;

function thing()
	Talk(1,"",10366)
end;
function return_yes()
	-- NewWorld(59, 1425, 3472)
end

function go1()
	-- NewWorld(59, 1425, 3472)				--Ve mon phai
	--SetFightState(0)					--Set trang thai luyen cong
	--SetRevPos(59,21) 					--Thiet lap diem luu ruong
	SetTask(TSK_NV_THIENVUONG, 10)
	SetFaction(FactionName)      				--Set mon phai
	SetCamp(3)
	SetCurCamp(3)
	SetRank(43)
	hockynang(2)
	Msg2Player("Hoan nghªnh b¹n gia nhËp Thiªn V­¬ng bang! H·y khëi ®Çu tõ mét ng­êi ThŞ vÖ!")
	Msg2Player("Häc ®­îc vâ c«ng Kinh L«i Tr¶m, Håi Phong L¹c Nh¹n, Tr¶m Long QuyÕt")
end

function no()
end