-- [LOCAL54 06/09 toi] 4 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local wlls_add_exp, wlls_award_pl_Respect, wlls_award_pl_exp, LeagueMatchResult
Include("\\script\\missions\\leaguematch\\head.lua")

--¿ÉÉı¼¶µÄ¼Ó¾­Ñé
function wlls_add_exp(n_exp)
	local n_switch	= GetTask(WLLS_TASKID_SWITCH)
	local b_expswt	= GetBit(n_switch, 1)
	local b_expsub	= GetBit(n_switch, 2)
	if (b_expswt ~= 0) then
		Msg2Player("B¹n ®· thiÕt lËp ®ãng phÇn th­ëng kinh nghiÖm liªn ®Êu, phÇn th­ëng kinh nghiÖm cña b¹n ®· bŞ hñy bá. NÕu cÇn cã thÓ ®Õn gÆp Quan viªn liªn ®Êu thay ®æi l¹i thiÕt lËp nµy.")
		return
	end
	if (b_expsub ~= 0) then
		n_exp	= n_exp - GetTaskTemp(WLLS_TEMPTASK)
		Msg2Player("B¹n chØ nhËn ®­îc trong sè ®ã"..n_exp.."®iÓm kinh nghiÖm")
	end
	tl_addPlayerExp(n_exp)
end

function wlls_award_pl_Respect(nRespect)
	AddRespect(nRespect)
	Msg2Player(format("Uy danh t¨ng %d", nRespect))
	wlls_award_log(format("NhËn ®­îc phÇn th­ëng 1 trËn liªn ®Êu——uy danh %d ®iÓm, tæng céng cã %d ®iÓm uy danh", nRespect, GetRespect()))
end

function wlls_award_pl_exp(nExp)
	wlls_add_exp(nExp)
	local str = "NhËn ®­îc phÇn th­ëng thi ®Êu——<color=yellow>"..nExp.." <color>®iÓm kinh nghiÖm"
	wlls_award_log(str)
	Msg2Player(str)
end

--Ã¿³¡½áÊø¼Ó¾­Ñé
function LeagueMatchResult(nMatchType, nMatchLevel, nResult)
	if (GetUUID() == 0) then	--ÏÂÏßµÄ²»¸ø¾­ÑéÁË
		wlls_award_log("Bëi v× ng­êi ch¬i ®· tho¸t m¹ng, kh«ng thÓ nhËn phÇn th­ëng kinh nghiÖm.")
		return
	end
	if (nResult == 1) then	--Ê¤
		wlls_award_pl_exp(10000000)
		wlls_award_pl_Respect(3)
	elseif (nResult == 0) then	--Æ½
		wlls_award_pl_exp(6000000)
		wlls_award_pl_Respect(2)
	else --¸º
		wlls_award_pl_exp(2000000)
		wlls_award_pl_Respect(1)
	end
end

EventSys:GetType("LeagueMatch"):Reg("MatchResult", LeagueMatchResult)