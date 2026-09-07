--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö Nga My Ph¸i ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "¶ëáÒÅÉ"


function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Trong thiªn h¹ c¸c m«n ph¸i do n÷ nhi s¸ng lËp th× bæn m«n ®øng ®Çu!  §Ö tö bæn m«n kh«ng nh÷ng vâ nghÖ cao c­êng, cÇm kú thi häa ®Òu tinh th«ng!  Ai nãi giang hå lµ thÕ giíi cña nam nh©n th× cø nh×n bæn ph¸i mµ lµm g­¬ng")
	elseif (player_Faction == "¶ëáÒÅÉ") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: TiÓu s­ muéi l¹i xuèng nói ch¬i µ? Cã cÇn ta ®­a vÒ nói kh«ng?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: TiÓu s­ muéi l¹i xuèng nói ch¬i µ? Cã cÇn ta ®­a vÒ nói kh«ng?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: TiÓu s­ muéi l¹i xuèng nói ch¬i µ? Cã cÇn ta ®­a vÒ nói kh«ng?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
end
end


function enroll_select()
if (GetSeries() == 2) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Gia nhËp Nga My ph¸i, chóng ta sÏ lµ tû muéi 1 nhµ!  Häa phóc cïng h­ëng! ", 2, "Gia nhËp Nga Mi ph¸i/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Say("Tr­íc tiªn muéi h·y luyÖn tËp l¹i c¨n b¶n, ®¹t ®Õn <color=Red>cÊp 10<color> chóng ta sÏ lµ tû muéi 1 nhµ! ", 0)		
		end
	end
end;

function go()
gianhapmonphai(4)
end;

function thing()
	Talk(1,"",10213)
end;