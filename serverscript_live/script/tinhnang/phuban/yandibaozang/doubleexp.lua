-- doubleexp.lua
-- wangjingjun
-- 
-- 2012-03-02 13:08:16

local tbDouble = {
	{"\\script\\activitysys\\config\\41\\extend.lua", "pActivity:DoubleExp_YDBZ",},
	}
function YDBZ_checkdoubleexp(nExp)
	-- [X2NIL 29/08] JX1: LuaDynamicExecuteByPlayer (ScriptFuns.cpp:2429) LUON
	-- `return 0` = khong tra gia tri nao => gan thang vao nExp la nil, keo theo
	-- AddOwnExp(nil) + format(nil) SAP ngay ai dau (npc_death :96/:106/:190,
	-- player_death :37). Nay chi nhan ket qua khi no THAT SU la so > 0:
	-- hom nay giu exp muc thuong (dung), mai kia C tra gia tri thi tu dong an.
	for i=1, getn(tbDouble) do
		local tbfunc = tbDouble[i]
		local nRa = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)
		if nRa ~= nil and nRa > 0 then
			nExp = nRa
		end
	end
	--Nh©n ®«i ®iÓm tÝch luü V­ît ¶i Viªm §Õ - Modified By DinhHQ - 20130305
	local nRa2 = DynamicExecuteByPlayer(PlayerIndex, "\\script\\vng_feature\\double_mission_award.lua", "tbVnX2Award:X2YDBZ", nExp)
	if nRa2 ~= nil and nRa2 > 0 then
		nExp = nRa2
	end
	return nExp
end