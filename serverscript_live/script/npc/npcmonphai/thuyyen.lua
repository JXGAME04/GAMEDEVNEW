--Author: Fong KiÒu
--Date: 2021
--Fution: §Ö tö Ph¸i Thuı Yªn ë th«n

Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "´äÑÌÃÅ"

function main(nNpcIndex)
	-- if(nvthegioi(nNpcIndex) == 1) then
	-- return end;
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Thóy Yªn m«n chóng ta tuy toµn lµ n÷, nh­ng vâ nghÖ lÊy 'Khinh, Kho¸i, Kú, Mü' mµ næi tiÕng giang hå. Tû muéi tÒ t©m!  Trong giang hå kh«ng ai d¸m coi khinh")
	elseif (player_Faction == "´äÑÌÃÅ") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Tû tû t¹i sao xuÊt s­ vËy!  Cø ë ®©y tù t¹i ch¼ng ph¶i vui vÎ l¾m sao!?",4,"XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Tû tû t¹i sao xuÊt s­ vËy!  Cø ë ®©y tù t¹i ch¼ng ph¶i vui vÎ l¾m sao!?",3,"T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Tû tû t¹i sao xuÊt s­ vËy!  Cø ë ®©y tù t¹i ch¼ng ph¶i vui vÎ l¾m sao!?",4,"Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end

	end
end


function enroll_select()
if (GetSeries() == 2) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say(10188, 2, "Gia nhËp Thóy Yªn/go", "§Ó ta suy nghÜ kü l¹i xem/thing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kĞm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
gianhapmonphai(5)
end;

function thing()
	Talk(1,"",10187)
end;