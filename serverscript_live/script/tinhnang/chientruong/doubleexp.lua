-- ÎÄ¼þÃû¡¡£ºdoubleexp.lua
-- ´´½¨Õß¡¡£ºwangjingjun
-- ÄÚÈÝ¡¡¡¡£ºÔÚËÎ½ðµÄÊ±ºò£¬ÅÐ¶ÏÊÇ·ñÐèÒª·­±¶¾­Ñé
-- ´´½¨Ê±¼ä£º2012-03-02 13:08:16

local tbDouble = {
	{"\\script\\activitysys\\config\\41\\extend.lua", "pActivity:DoubleExp_Songjin",},
	}
function Songjin_checkdoubleexp(nExp)
	for i=1, getn(tbDouble) do
		local tbfunc = tbDouble[i]
		-- [FIX 25/08] extend.lua cua hoat dong 41 da bi tat (doi ten thu muc) => DynamicExecuteByPlayer
		-- tra NIL (engine bo qua script chua nap, ScriptFuns.cpp:2354). Khong phong thi nExp = nil
		-- chay tiep vao X2SongJin/battlehead:711 lam MAT DIEM tran Tong Kim. Phong nil, giu nguyen
		-- hanh vi khi extend con nap duoc.
		local nRet = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)
		if nRet then
			nExp = nRet
		end
	end
	--Nh©n ®«i ®iÓm tÝch luü Tèng Kim - Modified By DinhHQ - 20130305
	nExp = DynamicExecuteByPlayer(PlayerIndex, "\\script\\vng_feature\\double_mission_award.lua", "tbVnX2Award:X2SongJin", nExp)
	return nExp
end