--[HOASON 01/09] De Tu Hoa Son phai o thon lang - khuon theo conlon.lua (he Thuy, id 10)
Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "»ªÉ½ÅÉ"

function main(nNpcIndex)
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Ph¸i Hoa S¬n ta ®· kh«ng tham gia giang hå h¬n 10 n¨m nay, lÇn nµy t¸i xuÊt ¾t sÏ khiÕn bän giang hå tµ ph¸i ph¶i khiÕp sî! Vâ c«ng bæn ph¸i chia hai nh¸nh: KiÕm T«ng vµ Khİ T«ng.")
	elseif (player_Faction == "»ªÉ½ÅÉ") or (player_Faction == "huashan") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end

function enroll_select()
if (GetSeries() == 2) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("Bæn m«n KiÕm T«ng tu th©n, Khİ T«ng tu t©m, dung hîp trong vâ c«ng. Cã muèn gia nhËp ph¸i Hoa S¬n kh«ng?", 2, "Gia nhËp Hoa S¬n/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
	gianhapmonphai(10)	-- nhu conlon.lua: SetFaction/Camp/Rank + hockynang (SKILLNORMAL[11] 15 chieu 10-70)
end;

function thing()
	Talk(1,"",10239)
end;
