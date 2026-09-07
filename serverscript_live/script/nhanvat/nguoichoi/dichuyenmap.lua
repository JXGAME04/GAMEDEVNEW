--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Khi Player vµo game

--Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")

TAB_MAPUTNAME = { 
	{1, "Ph­îng T­êng"},
	{11, "Thµnh §«"},
	{37, "BiÖn Kinh"},
	{78, "T­¬ng D­¬ng"},
	{80, "D­¬ng Ch©u"},
	{162, "§¹i Lý"},	
	{176, "L©m an"},
	{53, "Ba Lang Huyen"},
}

function CheckMapNoFor_UT(nMap)
	for i = 1, getn(TAB_MAPUTNAME) do
		if (nMap == TAB_MAPUTNAME[i][1]) then
			return 1
		end
	end
	return 0
end

function main()
	 dofile("script/player/dichuyenmap.lua")
	CheckPK()	
	local nMap,x,y = GetWorldPos()
	if(CheckMapNoFor_UT(nMap) == 1) then
		SetFightState(0)
	end
	if(GetFightState() == 1) then
		SetProtectTime(18*6)
		AddSkillState(963, 1, 0, 18*6) 			
	end
end	

		
function CheckPK()
	local nW, nX, nY = GetWorldPos()
	if(nW== 53 and GetFightState() == 0) then
		SetPKMode(0,0)
	end		
	if (nW == 208) and (GetPK() >= 1) then
		SetTimer(2*60*60*18, 9)--2h sau se chay ham OnTimer
		SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+GetPK()))
		Msg2Player("<color=pink>Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, h·y tiÕp tôc s¸m hèi ë thiªn lao.")
		return
	end
	if GetPK() >= 5 and GetPK() < 8 then
		Talk(1,"","Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, <color=red>PK 8 trë lªn<color> sÏ bÞ tèng vµo Thiªn Lao ®Ó ¨n n¨n s¸m hèi.")
		return
	elseif (GetPK() >= 8) then
		if(NewWorld(208,1787,3058) == 1) then
		LeaveTeam()
		SetPKMode(0,0)--phuc hoi pk tu do
		SetFightState(0)--phi chien dau
		SetPunish(0)
		SetCurCamp(GetCamp())
		SetTimer(2*60*60*18, 9)--2h sau se chay ham OnTimer
		SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+GetPK()))
		Talk(1,"","<color=pink>Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, h·y vµo Thiªn Lao ®Ó ¨n n¨n s¸m hèi.")
		end
	end
end