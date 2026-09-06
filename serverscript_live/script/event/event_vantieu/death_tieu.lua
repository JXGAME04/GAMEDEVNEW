--Author: Phong KiÒu
--Date 08/06/2021
--Function: khi tiªu bÞ chÕt

Include("\\script\\event\\event_vantieu\\lib_vt.lua")
Include("\\script\\lib\\lib_task.lua")

function OnDeath(nNpcIndex,nDamageIndex)
	DelNpc(nNpcIndex)
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if (nPlayerIndex == 0) then	-- neu la quai vat thi ngung		
		return 
	end		
	PlayerIndex = nPlayerIndex
	local nTask = GetTask(T_CUOPTIEU)
	if (nTask > MAX_CUOP_TIEU) then
		Talk(1,"","<sex> ®· c­íp ®­îc tiªu "..MAX_CUOP_TIEU.." råi. ")
		return 
	end
	SetTask(T_CUOPTIEU,GetTask(T_CUOPTIEU)+1)
	nTask = GetTask(T_CUOPTIEU)
	Msg2Player("Thµnh c«ng c­íp tiªu lªn "..nTask.."/"..MAX_CUOP_TIEU.."!")
end

function OnRevive(nNpcIndex)
	SetNpcResist(nNpcIndex, 90, 95, 90, 90, 90);--khang' cac loai
	SetNpcReplenish(nNpcIndex,20);--phuc hoi sinh luc
	-- SetNpcSpeed(nNpcIndex, 11)--toc do di chuyen tang len
end

function OnTimer(nNpcIndex)
	local szTieuName = GetNpcName(nNpcIndex)
	local msgLog = format("Qu¸ thêi gian <color=green> %s <color> kh«ng giao ®­îc, nhiÖm vô thÊt b¹i. ", szTieuName)
	Msg2SubWorld(msgLog)
	DelNpc(nNpcIndex)
end
