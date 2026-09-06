-- ÎÄ¼þÃû¡¡£ºdoubleexp.lua
-- ´´½¨Õß¡¡£ºwangjingjun
-- ÄÚÈÝ¡¡¡¡£ºÔÚ´³¹Ø¹ý¹ØµÄÊ±ºò£¬ÅÐ¶ÏÊÇ·ñÐèÒª·­±¶¾­Ñé
-- ´´½¨Ê±¼ä£º2012-03-02 13:08:16

local tbDouble = {
	{"\\script\\activitysys\\config\\41\\extend.lua", "pActivity:DoubleExp_ChuangGuan",},
	}
function Chuangguan_checkdoubleexp(nExp)
	for i=1, getn(tbDouble) do
		local tbfunc = tbDouble[i]
		-- [FIX 25/08] phong nil nhu battles\doubleexp.lua (hoat dong 41 da tat)
		local nRet = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)
		if nRet then
			nExp = nRet
		end
	end
	--Nh©n ®«i ®iÓm tÝch luü V­ît ¶i - Modified By DinhHQ - 20130305
	nExp = DynamicExecuteByPlayer(PlayerIndex, "\\script\\vng_feature\\double_mission_award.lua", "tbVnX2Award:X2ChallengeOfTime", nExp)
	return nExp
end