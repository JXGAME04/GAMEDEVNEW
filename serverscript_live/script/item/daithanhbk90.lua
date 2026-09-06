--Author: Fong KiÒu

Include("\\script\\task\\system\\task_string.lua")
Include("\\script\\log_game\\save_log.lua")

function main(nItemIdx) 
	
	-- dofile("script/item/daithanhbk90.lua")
	
	local n_fac = GetFirstAddFaction()

	if (n_fac < 0) then 
		Talk(1, "","Gièng nh­ quyÓn s¸ch nµy miªu t¶ chÝnh lµ c¸c m«n ph¸i cao cÊp vâ c«ng , ng­¬i kh«ng biÕt bän hä huyÒn c¬ ")
		return 1
	end 
	local player_Faction = GetFaction();
	if (player_Faction == "") then
		Talk(1,"","Ng­êi ch¬i ch­a vµo ph¸i ch­a thÓ sö dông.")
		return
	end
	if (GetLevel() < 90) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 90 trë lªn míi cã thÓ sö dông.")
		return
	end
	local tb_90skill = { 
		[0] = {318, 319, 321}, 
		[1] = {322, 325, 323}, 
		[2] = {339, 302, 342}, 
		[3] = {353, 355}, 
		[4] = {380, 328}, 
		[5] = {336, 337}, 
		[6] = {357, 359}, 
		[7] = {361, 362}, 
		[8] = {365, 368}, 
		[9] = {372, 375}, 
		[10] = {1364, 1382}, -- [HOASON 01/09c]
		[11] = {1967, 1983}, -- [VHTD 02/09] Vu Hon
		[12] = {2124, 2141}, -- [VHTD 02/09] Tieu Dao
	}; 

	local tb_Desc = {}
	for i = 1, getn(tb_90skill[n_fac]) do 
		local skill = HaveMagic(tb_90skill[n_fac][i])
		if (skill ~= -1 and skill ~= 20) then 
			tinsert(tb_Desc, format("Th¨ng cÊp ".."%s/#upgrade_skilllevel(%d)", GetSkillName(tb_90skill[n_fac][i]), tb_90skill[n_fac][i]))
		end 
	end 

	if (getn(tb_Desc) == 0) then 
		Talk(1, "","Kü n¨ng ®¹t tíi cao cÊp nhÊt hoÆc lµ cßn ch­a häc .")
		return 1
	end 
	tinsert(tb_Desc,"KÕt thóc ®èi tho¹i /OnCancel()")
	local DOITHOAI = "Lùa chän cÇn th¨ng cÊp ®Ých kü n¨ng :"
	Say(DOITHOAI,getn(tb_Desc),tb_Desc)
	return 1
end 

function upgrade_skilllevel(n_skillid) 
	if (HaveMagic(n_skillid) == -1 or HaveMagic(n_skillid) >= 20) then 
		return
	end 
	if (ConsumeItem(1,0,6,1,2433) == 1) then
		AddMagic(n_skillid, 20); 
		Msg2Player("§· tu luyÖn ®¹i thµnh kü n¨ng")
		logHoatDong(format("\n[%s]\t%s\tAccount:%s\tName:%s\tUpGrade Skill:%d", " ®¹i thµnh bÝ tÞch 90", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), n_skillid ))
	else
		Msg2Player("Kh«ng cã ®¹i thµnh trong hµnh trang")
	end 
end 


function OnCancel() 
end 
