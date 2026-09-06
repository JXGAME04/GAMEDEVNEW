-- [FIX 25/08] JX1 moi file mot lua_State (Linux: MOT state chung). Trap chay se
-- chiem 'current script' cua nguoi choi; menu NPC dang mo bam vao se goi ham
-- trong STATE TRAP -> nil (ScriptError: ture_try_starttask / ture_movecity).
-- Include 2 script NPC cua ai de state trap co DU ham menu - mo phong dung
-- hanh vi single-state cua ban Linux. main() cua trap o cuoi file nen van de len.
Include("\\script\\task\\tollgate\\messenger\\messenger_templerukou.lua")
Include("\\script\\task\\tollgate\\messenger\\shanshenmiao\\messenger_temnpc.lua")

function main()
	if ( GetTask(1202) == 10 ) then
		SetPos(1337,3154);
		Msg2Player("Tr­íc tiªn ph¶i ®èi tho¹i tr­íc víi DÞch Quan trong khu vùc")
	elseif ( GetTask(1202) == 20 ) then
		SetPos(1340,3148);
	elseif ( GetTask(1202) == 30 or GetTask(1202) == 25 or   GetTask(1202) == 0 ) then
		SetPos(1337,3154);
		Msg2Player("Xin lçi! HiÖn t¹i b¹n kh«ng thÓ vµo ¶i ®­îc.")
	end
end;