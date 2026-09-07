--ÎäÁÖÈÙÓþÁî

Include("\\script\\missions\\leaguematch\\head.lua")

WLLS_HONOUR_ITEM_TB	= {
	[1259]	= {500, "LÖnh bµi vinh dù Hoµng Kim"},
	[1260]	= {100, "LÖnh bµi vinh dù B¹ch Ng©n"},
	[1261]	= {50, "LÖnh bµi vinh dù Thanh ®ång"},
	[1262]	= {10, "LÖnh bµi vinh dù Hµn thiÕt"},
	-- [WLLS port 20/08/2026] 3 muc bang goc bo sot (magicscript van tro ve day):
	[1299]	= {20, "LÖnh bµi vinh dù"},
	[1339]	= {1000, "B¹ch Kim bµi"},
	[1340]	= {3000, "ChÝ t«n lÖnh bµi"},
}

--Ö÷¶Ô»°¿ò
function main(nItemIndex)
	local nG, nD, nP	= GetItemProp(nItemIndex)
	local tbItem	= WLLS_HONOUR_ITEM_TB[nP]
	if (not tbItem) then
		Say("CÇn th­¬ng d­îc g× kh«ng?",0)
		return 1
	end
	SetTask(WLLS_TASKID_HONOUR, GetTask(WLLS_TASKID_HONOUR) + tbItem[1])
	SyncTaskValue(WLLS_TASKID_HONOUR)
	local szlog = format("§· sö dông %s, ®iÓm vinh dù t¨ng thªm <color=yellow>%d<color> ®iÓm.", 
									tbItem[2], tbItem[1])
	Msg2Player(szlog)
	wlls_award_log(format("%s, tæng céng cã %d ®iÓm", szlog, GetTask(WLLS_TASKID_HONOUR)))
end
