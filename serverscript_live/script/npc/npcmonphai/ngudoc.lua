--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö Ngò §éc Gi¸o ë th«n lµng

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "Îå¶¾½Ì"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Ngò §éc gi¸o hiÖn nay ®ang lóc h­ng thŞnh, l¹i liªn kÕt víi Thiªn NhÉn gi¸o hç trî §¹i Kim nam h¹! Ngµy sau vinh hoa phó quı sÏ cïng h­ëng!")
	elseif (player_Faction == "Îå¶¾½Ì") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: L©u l¾m kh«ng gÆp! Xem ra d¹o nµy ng­¬i ®· kh¸ h¬n tr­íc nhiÒu",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: L©u l¾m kh«ng gÆp! Xem ra d¹o nµy ng­¬i ®· kh¸ h¬n tr­íc nhiÒu",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: L©u l¾m kh«ng gÆp! Xem ra d¹o nµy ng­¬i ®· kh¸ h¬n tr­íc nhiÒu",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end


function enroll_select()
if (GetSeries() == 1) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Sao h¶? Cã muèn gia nhËp bæn gi¸o kh«ng?", 2, "Gia nhËp Ngò §éc Gi¸o/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(3)
end;

function thing()
end;