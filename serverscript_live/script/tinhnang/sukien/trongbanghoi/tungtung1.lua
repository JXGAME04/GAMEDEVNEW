-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Trèng bang héi

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\trongbanghoi\\lib.lua")

function main(NpcIndex)

	-- dofile("script/event/trongbanghoi/tungtung1.lua")
	
	if (0 == GetCamp()) then
		Talk(1,"","<sex> ch­a gia nhËp m«n ph¸i, kh«ng thÓ tham gia. ")
		return
	end
	
	if (GetLevel() < 80) then
		Talk(1,"","§¼ng cÊp cña c¸c h¹ kh«ng ®ñ. ")
		return
	end	

	if (1 == GetFightState()) then
		Talk(1,"","Tr¹ng th¸i chiÕn ®Êu kh«ng thÓ ®¸nh trèng. ")
		return
	end
	
	if(GetTask(T_EXP_TRONGBH) >= EXP_TBH_DAY) then
		Talk(1,"","H«m nay c¸c h¹ ®· nhËn th­ëng v­ît qu¸ "..EXP_TBH_DAY.." ®iÓm cho phÐp. ")
		return
	end
	
	local nTongIdbyNpc = GetNpcParam(NpcIndex,1)
	local nTongID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	local nTongMaster = GetTongInfo(2)
	--Msg2Player(format("%d %d", nTongID, nTongIdbyNpc))
	if(nTongID ~= nTongIdbyNpc) then
		Talk(1,"","§©y kh«ng ph¶i lµ trèng cña quý bang. ")
		return
	end
	
	PaceBar("§ang ®¸nh trèng...", 5, "FnDanhTrongOkay1("..NpcIndex..")")
	--ChonBossST()
end

function ChonBossST()
	local tab_boss_st = {}
	tinsert(tab_boss_st, "§Ó ta suy nghÜ l¹i./no")
	Say("<sex> muèn khiªu chiÕn víi ai?", getn(tab_boss_st), tab_boss_st)
end

function FnDanhTrongOkay1(NpcIndex)
	if(GetTask(T_EXP_TRONGBH) >= EXP_TBH_DAY) then
		Talk(1,"","H«m nay c¸c h¹ ®· nhËn th­ëng v­ît qu¸ "..EXP_TBH_DAY.." ®iÓm cho phÐp. ")
		return
	end
	for i = 1, 10 do
	local nExp1 = random(EXP_RAN1,EXP_RAN2)
	AddOwnExp(nExp1)
	local logMgs = format("Ng­êi ch¬i <color=yellow>%s<color> ®¸nh trèng nhËn ®­îc ®iÓm kinh nghiÖm.", GetName())
	--Msg2SubWorld(logMgs)
	logHoatDong(logMgs)
	SetTask(T_EXP_TRONGBH, GetTask(T_EXP_TRONGBH) + nExp1)
	Msg2Player(format("C¸c h¹ ®­îc tæng <color=yellow>%d<color> ®iÓm. ", GetTask(T_EXP_TRONGBH)))
	end
end

function OnTimer(nIndex)
	DelNpc(nIndex)
end