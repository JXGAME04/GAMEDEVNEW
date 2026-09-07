--[VHTD 02/09] Wuhuntang o thon lang - khuon theo hoason.lua (he Hoa, id 11); nguon: client VLTK (Linux khong co phai nay)
Include("\\script\\header\\factionhead.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\npcthon\\npcmonphaiFactionHelper.lua")

FactionName = "Îä»êÌÃ"

function main(nNpcIndex)
local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"enroll_select","Vò Hån §­êng do Nh¹c Phi lËp nªn, quy tô tinh binh Nh¹c gia qu©n, lÊy ®ao thuÉn lµm vò khÝ, lÊy ch÷ trung nghÜa lµm gèc. Ng­¬i cã muèn cïng ta b¶o vÖ biªn c­¬ng kh«ng?")
	elseif (player_Faction == "Îä»êÌÃ") or (player_Faction == "wuhuntang") then
			if (GetTask(TASK_DUNGCHUNG2) == 0) then
				if GetLevel() >= 60 then
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",5,"§Õn Vò Hån §­êng/go_vuhon","XuÊt s­ xuèng nói/xuatsu","Ta muèn nhËn kü n¨ng 9x/hotrokn","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",4,"§Õn Vò Hån §­êng/go_vuhon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
				end
			else
				Say("<npc>: Nghe nãi sau khi ng­¬i xuèng nói ®· lËp chót c«ng danh, cã nhí ®Õn s­ ®Ö s­ muéi chóng ta kh«ng?",5,"§Õn Vò Hån §­êng/go_vuhon","Trïng ph¶n m«n ph¸i/trungphansumon","T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Xin chuyÓn dïm lêi hái th¨m s­ phô /no")
			end
	end
end

function enroll_select()
if (GetSeries() == 3) and (GetCamp() == 0) then
		if (GetLevel() >= 10) then
			Say("B¶n m«n chóng ta lÊy Vò Hån ThuÉn Ph¸p lµm gèc, Vò Hån §ao Ph¸p c«ng thñ toµn diÖn. Ng­¬i cã muèn gia nhËp Vò Hån §­êng kh«ng?", 3, "Gia nhËp Vò Hån §­êng/go", "§Õn Vò Hån §­êng/go_vuhon", "§Ó ta suy nghÜ kü l¹i xem/nothing")
		else
			Talk(1,"","C¨n b¶n cña ng­¬i cßn kÐm l¾m! H·y ®i luyÖn tËp thªm, bao giê ®Õn cÊp 10 l¹i ®Õn t×m ta!")
		end
	end
end;

function go()
	gianhapmonphai(11)	-- [VHTD 02/09] nhu hoason.lua: SetFaction/Camp/Rank + hockynang (SKILLNORMAL[12])
end;

function thing()
	Talk(1,"",10239)
end;


function go_vuhon()	-- [VHTD 02/09d] Linux: NewWorld toa do mon phai
	Msg2Player("Ngåi yªn, chóng ta ®Õn Vò Hån §­êng")
	NewWorld(1042, 1625, 3130)
end

function OnRevive()	-- [VHTD 02/09e] engine goi khi spawn (KNpc.cpp:9352 / ScriptFuns.cpp:7346); thieu -> ScriptError 4 + m_ActionScriptID = 0
end