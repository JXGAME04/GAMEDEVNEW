--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö Thiªn NhÉn Gi¸o ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "ÌìÈÌ½Ì"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Gi¸o chñ cña bæn gi¸o lµ Hoµn Nhan Hïng LiÖt. Danh chÊn thiªn h¹! Hoµng ®Õ Kim quèc còng ph¶i nh­êng 3 phÇn. Cao thñ cña bæn gi¸o nhiÒu v« sè! Nh©n tµi ngäa hæ tµng long")
	elseif (player_Faction == "ÌìÈÌ½Ì") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Nghe nãi ng­¬i sau khi xuÊt s­ ®· lËp nªn nhiÒu c«ng danh! Qu¶ kh«ng hæ danh lµ ®Ö tö cña bæn ph¸i",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Nghe nãi ng­¬i sau khi xuÊt s­ ®· lËp nªn nhiÒu c«ng danh! Qu¶ kh«ng hæ danh lµ ®Ö tö cña bæn ph¸i",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Nghe nãi ng­¬i sau khi xuÊt s­ ®· lËp nªn nhiÒu c«ng danh! Qu¶ kh«ng hæ danh lµ ®Ö tö cña bæn ph¸i",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end


function enroll_select()
if (GetSeries() == 3) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Cã muèn gia nhËp bæn gi¸o kh«ng?", 2, "Gia nhËp Thiªn NhÉn gi¸o/go", "Kh«ng gia nhËp/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(7)
end;

function thing()
	Talk(1,"",10424)
end;