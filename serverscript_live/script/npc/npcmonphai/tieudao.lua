--[VHTD 02/09] Xiaoyao o thon lang - khuon theo hoason.lua (he Tho, id 12); nguon: client VLTK (Linux khong co phai nay)
Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "åÐÒ£ÅÉ"

function main(nNpcIndex)
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Tiªu Dao ph¸i ta Èn c­ gi÷a nói rõng, lÊy kiÕm vµ cÇm lµm b¹n, tiªu dao tù t¹i ngoµi vßng danh lîi. Ng­¬i cã muèn cïng ta ng¾m c¶nh th­ëng nh¹c kh«ng?")
	elseif (player_Faction == "åÐÒ£ÅÉ") or (player_Faction == "xiaoyao") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",5,"§Õn Tiªu Dao ph¸i/go_tieudao","XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",4,"§Õn Tiªu Dao ph¸i/go_tieudao","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",5,"§Õn Tiªu Dao ph¸i/go_tieudao","Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end

function enroll_select()
if (GetSeries() == 4) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("B¶n m«n chóng ta lÊy Tiªu Dao KiÕm Ph¸p lµm gèc, Tiªu Dao CÇm Ph¸p ©m luËt s¸t ®Þch. Ng­¬i cã muèn gia nhËp Tiªu Dao ph¸i kh«ng?", 3, "Gia nhËp Tiªu Dao ph¸i/go", "§Õn Tiªu Dao ph¸i/go_tieudao", "§Ó ta suy nghÜ kü l¹i xem/nothing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kÐm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
	gianhapmonphai(12)	-- [VHTD 02/09] nhu hoason.lua: SetFaction/Camp/Rank + hockynang (SKILLNORMAL[13])
end;

function thing()
	Talk(1,"",10239)
end;


function go_tieudao()	-- [VHTD 02/09d] Linux: NewWorld toa do mon phai
	Msg2Player("Ngåi yªn, chóng ta ®Õn Tiªu Dao ph¸i")
	NewWorld(1057, 1641, 3288)
end

function OnRevive()	-- [VHTD 02/09e] engine goi khi spawn (KNpc.cpp:9352 / ScriptFuns.cpp:7346); thieu -> ScriptError 4 + m_ActionScriptID = 0
end