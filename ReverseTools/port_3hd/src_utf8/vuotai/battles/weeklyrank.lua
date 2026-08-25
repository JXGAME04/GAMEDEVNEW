Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\dailogsys\\dailogsay.lua")

function sj_UpdateScore(tbPlayerList)
	for i=1,getn(tbPlayerList) do
		local tbPlayer = tbPlayerList[i]
		if tbPlayer.nIndex ~= nil and tbPlayer.nIndex > 0 then
			local nScore = CallPlayerFunction(tbPlayer.nIndex, GetTask, 751)
			if nScore >= 0 then
				local szName = CallPlayerFunction(tbPlayer.nIndex, GetName)
				local hParam = OB_Create()
				ObjBuffer:PushObject(hParam, szName)
				ObjBuffer:PushObject(hParam, nScore)
				RemoteExecute("\\script\\battle\\weeklyrank.lua", "tbSJ_WeeklyRank:s_AddScore", hParam)
				OB_Release(hParam)
			end
		end
		
	end
end

function sj_weekly_rank_update(tbPlayerList)
	local nTime = tonumber(GetLocalDate("%H%M"))
	if nTime > 2100 and nTime < 2300 then
		sj_UpdateScore(tbPlayerList)
	end
end

function sj_GetRankTop10()
	local hParam = OB_Create()
	local szName = GetName()
	ObjBuffer:PushObject(hParam, szName)
	RemoteExecute("\\script\\battle\\weeklyrank.lua", "tbSJ_WeeklyRank:s_GetRank", hParam, "sj_GetRankTop10_CallBack", 0)
	OB_Release(hParam)
end

function sj_GetRankTop10_CallBack(Param, hResult)

	if OB_IsEmpty(hResult) == 1 then
		return
	end

	local szName = ObjBuffer:PopObject(hResult)
	local nPlayerIndex = SearchPlayer(szName)
	
	if nPlayerIndex <= 0 then
		OB_Release(hResult)
		return
	end
	
	local nCount = ObjBuffer:PopObject(hResult)
	local szTitle = "Danh sách xếp hạng Tống Kim tuần trước như sau:"
	local szText, nScore
	
	for i=1, nCount do
		szName = ObjBuffer:PopObject(hResult)
		nScore = ObjBuffer:PopObject(hResult)
		szText = format("<enter>\thạng%d: \t%16s\t%d điểm", i, szName, nScore)
		szTitle = szTitle .. szText
	end
	
	tbOpt = {
		[1]={"Ta muốn nhận phần thưởng Long Hổ Tống Kim tuần trước", sj_GetWeeklyRank},
		--Bổ sung phần thưởng chỉ nhận exp, không nhận hào quang - modified by DinhHQ - 20130701
		[2]={"Ta muốn nhận phần thưởng Long Hổ Tống Kim tuần trước (chỉ nhận kinh nghiệm)", sj_GetWeeklyRank_2},
		[3]={" Đóng",}
	}
	CallPlayerFunction(nPlayerIndex, CreateNewSayEx, szTitle, tbOpt)
	
end

function sj_close()
end

function sj_GetWeeklyRank()
	local nTime = tonumber(GetLocalDate("%H%M"))
	local nWeekDay = tonumber(GetLocalDate("%w"))
	
	if nWeekDay ~= 1 or nTime < 0100 then
		Talk(1, "", "Hiện tại không phải thời gian nhận, ngươi hãy đợi đến 01:00 đến 24:00 ngày thứ 2 nhận phần thưởng nhé.")
		return
	end
	
	local szName = GetName()
	local hParam = OB_Create()
	ObjBuffer:PushObject(hParam, szName)
	
	RemoteExecute("\\script\\battle\\weeklyrank.lua", "tbSJ_WeeklyRank:s_GetRankAward", hParam, "sj_GetWeeklyRank_CallBack", 0)
	
	OB_Release(hParam)

end

function sj_GetWeeklyRank_CallBack(Param, hResult)
	if OB_IsEmpty(hResult) == 1 then
		return
	end
	
	local szName = ObjBuffer:PopObject(hResult)
	local nRank = ObjBuffer:PopObject(hResult)
	local nPlayerIndex = SearchPlayer(szName)
	
	if nPlayerIndex <= 0 then
		OB_Release(hResult)
		return
	end
	
	CallPlayerFunction(nPlayerIndex, sj_GiveRankAward, nRank)
	
end

local tbRankAward = {
	[1]={Exp=500000000, nTitle=255},
	[2]={Exp=400000000, nTitle=256},
	[3]={Exp=300000000, nTitle=257},
	[4]={Exp=200000000, nTitle=258},

}

function sj_GiveRankAward(nRank)
	if not nRank then
		Talk(1, "", "Xin lượng thứ, ngươi không có trong bảng xếp hạng tuần trước.")
		return
	end
	
	if nRank == 0 then
		Talk(1, "", "Đại hiệp đã nhận phần thưởng rồi, không thể nhận thêm.")
		return
	end
	
	Msg2Player(format("Chúc mừng đại hiệp đã nhận được điểm Tống Kim xếp hạng %d.", nRank))
	
	if nRank > 4 then
		nRank = 4
	end
	
	local nDate = mod(FormatTime2Number(GetCurServerTime() + 7*24*60*60), 100000000)

	Title_RemoveTitle(%tbRankAward[nRank].nTitle)
	Title_AddTitle(%tbRankAward[nRank].nTitle, 2, nDate) --持续一周
	Title_ActiveTitle(%tbRankAward[nRank].nTitle)

	SetTask(1122, %tbRankAward[nRank].nTitle)
	tbAwardTemplet:GiveAwardByList({nExp_tl = %tbRankAward[nRank].Exp}, "battle rank top10")

end


--------Bổ sung phần thưởng chỉ nhận exp, không nhận hào quang - modified by DinhHQ - 20130701--------

function sj_GetWeeklyRank_2()
	local nTime = tonumber(GetLocalDate("%H%M"))
	local nWeekDay = tonumber(GetLocalDate("%w"))
	
	if nWeekDay ~= 1 or nTime < 0100 then
		Talk(1, "", "Hiện tại không phải thời gian nhận, ngươi hãy đợi đến 01:00 đến 24:00 ngày thứ 2 nhận phần thưởng nhé.")
		return
	end
	
	local szName = GetName()
	local hParam = OB_Create()
	ObjBuffer:PushObject(hParam, szName)
	
	RemoteExecute("\\script\\battle\\weeklyrank.lua", "tbSJ_WeeklyRank:s_GetRankAward", hParam, "sj_GetWeeklyRank_CallBack_2", 0)
	
	OB_Release(hParam)

end

function sj_GetWeeklyRank_CallBack_2(Param, hResult)
	if OB_IsEmpty(hResult) == 1 then
		return
	end
	
	local szName = ObjBuffer:PopObject(hResult)
	local nRank = ObjBuffer:PopObject(hResult)
	local nPlayerIndex = SearchPlayer(szName)
	
	if nPlayerIndex <= 0 then
		OB_Release(hResult)
		return
	end
	
	CallPlayerFunction(nPlayerIndex, sj_GiveRankAward_2, nRank)
	
end

function sj_GiveRankAward_2(nRank)
	if not nRank then
		Talk(1, "", "Xin lượng thứ, ngươi không có trong bảng xếp hạng tuần trước.")
		return
	end
	
	if nRank == 0 then
		Talk(1, "", "Đại hiệp đã nhận phần thưởng rồi, không thể nhận thêm.")
		return
	end
	
	Msg2Player(format("Chúc mừng đại hiệp đã nhận được điểm Tống Kim xếp hạng %d.", nRank))
	
	if nRank > 4 then
		nRank = 4
	end	
	
	tbAwardTemplet:GiveAwardByList({nExp_tl = %tbRankAward[nRank].Exp}, "battle rank top10 only exp")

end
