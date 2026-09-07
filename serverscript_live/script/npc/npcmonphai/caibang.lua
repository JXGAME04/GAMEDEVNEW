--Author: Fong KiÒu
--Date: 2021
--Fuction: §Ö tö C¸i Bang Th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName 		= "Ø¤°ï"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Bæn bang ®· cã danh x­ng Thiªn h¹ ®Ö nhÊt bang, danh chÊn giang hå.Nh©n tµi ngäa Hæ tµng Long, thêi nµo còng cã ")
	elseif (player_Faction == "Ø¤°ï") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: §· l©u kh«ng gÆp! Mäi ng­êi ®Òu rÊt mong nhí! §¹i hiÖp d¹o nµy cã kháe kh«ng?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: §· l©u kh«ng gÆp! Mäi ng­êi ®Òu rÊt mong nhí! §¹i hiÖp d¹o nµy cã kháe kh«ng?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: §· l©u kh«ng gÆp! Mäi ng­êi ®Òu rÊt mong nhí! §¹i hiÖp d¹o nµy cã kháe kh«ng?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end


function enroll_select()
if (GetSeries() == 3) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Cã høng thó gia nhËp C¸i bang kh«ng? Mäi ng­êi cã c¬m cïng ¨n, cã r­îu cïng uèng!", 2, "Gia NhËp C¸i bang/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(6)
end;

function thing()
	Talk(1,"",10452)
end;