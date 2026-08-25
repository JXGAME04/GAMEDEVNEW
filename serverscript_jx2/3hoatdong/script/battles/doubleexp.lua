-- ÎÄ¼þÃû¡¡£ºdoubleexp.lua
-- ´´½¨Õß¡¡£ºwangjingjun
-- ÄÚÈÝ¡¡¡¡£ºÔÚËÎ½ðµÄÊ±ºò£¬ÅÐ¶ÏÊÇ·ñÐèÒª·­±¶¾­Ñé
-- ´´½¨Ê±¼ä£º2012-03-02 13:08:16

local tbDouble = {
	{"\\script\\activitysys\\config\\41\\extend.lua", "pActivity:DoubleExp_Songjin",},
	}
function Songjin_checkdoubleexp(nExp)
	for i=1, getn(%tbDouble) do
		local tbfunc = %tbDouble[i]
		nExp = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)
	end
	--Nh©n ®«i ®iÓm tÝch luü Tèng Kim - Modified By DinhHQ - 20130305
	nExp = DynamicExecuteByPlayer(PlayerIndex, "\\script\\vng_feature\\double_mission_award.lua", "tbVnX2Award:X2SongJin", nExp)
	return nExp
end