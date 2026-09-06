--Author: Fong KiÒu
--Date: 2021
--Function: §Ö tö §­êng M«n ë th«n lµng

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "ÌÆÃÅ"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(2, "enroll_select", "§­êng m«n hïng cø Xuyªn Thôc, ¸m khİ vµ háa khİ ®· lõng danh thiªn h¹. Ng­êi trong giang hå nghe ®Õn §­êng m«n th¶y ®Òu biÕn s¾c, ®ñ thÊy sù lîi h¹i cña bæn m«n", "C¸i g× lµ Vâ l©m chİnh ®¹o, d©n téc ®¹i nghÜa? Toµn lµ bän l¸o toĞt. Bæn m«n kh«ng ­a bän chİnh ®¹o, còng ch¼ng kÕt giao víi tµ ®¹o. Muèn gia nhËp bæn m«n kh«ng ph¶i lµ chuyÖn ®¬n gi¶n!")
	elseif (player_Faction == "ÌÆÃÅ") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: L©u qu¸ kh«ng gÆp! Sau khi xuÊt s­ ®· lËp nªn kú tİch g× ch­a? KÓ nghe thö!",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: L©u qu¸ kh«ng gÆp! Sau khi xuÊt s­ ®· lËp nªn kú tİch g× ch­a? KÓ nghe thö!",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: L©u qu¸ kh«ng gÆp! Sau khi xuÊt s­ ®· lËp nªn kú tİch g× ch­a? KÓ nghe thö!",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
end

end
function enroll_select()
if (GetSeries() == 1) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Bæn m«n tinh th«ng ®¹o chó kiÕm thuËt. Bän Vâ §ang  lµ c¸i thø g× chø!  Cã muèn gia nhËp bæn ph¸i kh«ng?", 2, "Gia nhËp §­êng M«n/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(2)
end;

function no()
	Talk(1,"",10310)
end;