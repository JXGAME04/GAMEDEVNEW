-------------------------------------------------------------------
-- Filename	: Hπt Huy Hoµng
-- Author	: DzungDolby
-------------------------------------------------------------------

Include("\\script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")

function main(nNpcIdx)
	
	--dofile("script/event/event_huyhoang_dungdb/hathuyhoang.lua")
	
	local nTimeLeft = GetNpcParam(nNpcIdx, 3) - GetTimeMinutesNow()
	local nMinutes = floor(nTimeLeft/(60*18))
	local nSecond = ceil(mod(nTimeLeft,60*18)/18)
	Talk(1,"",format("<color=green>Hπt Huy Hoµng<color> : ßang cﬂn r t non vµ xanh ch≠a th” thu hoπch h∑y quay lπi sau <color=red>%d<color> phÛt  <color=red>%d <color> gi©y!",  nMinutes, nSecond))
end

function OnTimer(nNpcIdx)
	local nType = GetNpcParam(nNpcIdx, 1)
	local x,y,w = GetNpcPos(nNpcIdx)
	Add_QuaHuyHoang(nType,x,y,w)
	DelNpc(nNpcIdx)
end
