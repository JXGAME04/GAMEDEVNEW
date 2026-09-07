--Author: Fong KiÒu
--Date:13/07/2021
--Funtion: §Ö tö Vâ §ang Ph¸i ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "Îäµ±ÅÉ"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(3, "enroll_select", "Thiªn h¹ vâ häc, B¾c t«n ThiÕu l©m, nam sïng Vâ §ang. Ch¾c ng­¬i ®· tõng nghe qua?", "Vâ c«ng bæn ph¸i lÊy tÜnh chÕ ®éng, lÊy nhu kh¾c c­¬ng, lÊy ng¾n th¾ng dµi, lÊy chËm ®¸nh nhanh, lÊy ı vËn khİ, lÊy khİ vËn th©n, ®¸nh sau tíi tr­íc. Khiªm tèn ®iÒm ®¹m, lÊy v« h×nh th¾ng h÷u h×nh, ®ã lµ c¶nh giíi vâ häc tèi cao!", "Vâ nghÖ bæn m«n cã 'ngò bÊt truyÒn': yÕu ®uèi, hiÓm ®éc, h¸o th¾ng, cuång töu, gian d©m sÏ kh«ng truyÒn vâ c«ng")
	elseif (player_Faction == "Îäµ±ÅÉ") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Tuy ng­¬i ®· xuÊt s­ nh­ng chóng ta vÉn th­êng nhí ®Õn! D¹o nµy cã kháe kh«ng?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Tuy ng­¬i ®· xuÊt s­ nh­ng chóng ta vÉn th­êng nhí ®Õn! D¹o nµy cã kháe kh«ng?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Tuy ng­¬i ®· xuÊt s­ nh­ng chóng ta vÉn th­êng nhí ®Õn! D¹o nµy cã kháe kh«ng?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end

	end
end


function enroll_select()
if (GetSeries() == 4) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then						--µÈ¼¶´ïµ½Ê®¼¶
			Say("NÕu muèn gia nhËp bæn ph¸i ph¶i thay ®æi t©m tİnh, chuyÓn t©m tu hµnh, t­¬ng lai cã rÊt nhiÒu c¬ héi chê ®ãn ng­¬i!", 2, "Gia nhËp Vâ §ang/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Say("C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn <color=Red>cÊp 10<color> l¹i ®Õn t×m ta", 0)
		end
	end
end;

function go()
gianhapmonphai(8)
end;

function thing()
	Talk(1,"",10394)
end;