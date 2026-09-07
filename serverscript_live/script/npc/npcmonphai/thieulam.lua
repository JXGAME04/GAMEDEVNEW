--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö ThiÕu L©m ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "ÉÙÁÖÅÉ"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","ThiÕu l©m ph¸i lu«n ®­îc xem lµ Vâ l©m ®Ö nhÊt m«n ph¸i, mÖnh danh 'Th¸i s¬n b¾c ®Èu' cña Trung nguyªn vâ l©m")
	elseif (player_Faction == "ÉÙÁÖÅÉ") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: S­ ®Ö! S­ phô l¹i ph¸i ®Ö xuèng nói ­?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin thay ta vÊn an s­ phô /no")
				else
				Say("<npc>: S­ ®Ö! S­ phô l¹i ph¸i ®Ö xuèng nói ­?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin thay ta vÊn an s­ phô /no")
				end
			else
				Say("<npc>: S­ ®Ö! S­ phô l¹i ph¸i ®Ö xuèng nói ­?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin thay ta vÊn an s­ phô /no")
			end
	end
end


function enroll_select()
	if (GetSeries() == 0) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("ThiÕu L©m kh«ng hæ danh lµ danh m«n chİnh ph¸i! Huynh ®Ö cã muèn gia nhËp kh«ng?", 2, "Gia nhËp ThiÕu L©m ph¸i/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end

function go()
gianhapmonphai(0)
end;

function thing()
	Talk(1,"",10283)
end;