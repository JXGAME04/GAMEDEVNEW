Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\misc\\eventsys\\type\\npc.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\vng_lib\\extpoint.lua")
Include("\\script\\lib\\log.lua")
IncludeLib("PET")
IncludeLib("SETTING");

tbThapnienLenhbai = tbThapnienLenhbai or {};
tbThapnienLenhbai.szPathFile = "\\settings\\event\\thapnienlenhbai\\taskthapnien.txt"
tbThapnienLenhbai.MINLEVEL	 = 1
tbThapnienLenhbai.MAXLEVEL	 = 200
tbThapnienLenhbai.EVENT_LOG_TITLE = "LOGEVENTJX1_ThapNienLenhBai2014"
tbThapnienLenhbai.TASK_INFOR_TN = {nTaskID = 2971,nStartBit = 1,nBitCount = 6,nMaxValue = 60}
tbThapnienLenhbai.TASK_ID_RESET_01 = 2971
tbThapnienLenhbai.TASK_ID_RESET_02 = 2966

tbThapnienLenhbai.EXT_POINT_ID_1 = {nID = 5, nBit = 1, nVer = 2} --Kích hoạt mã code tại web

--PartySupport
tbThapnienLenhbai.nCOTBatchAward = 20 --Vượt qua ải 20
tbThapnienLenhbai.nYDBZBatchAward = 6 --Vượt qua ải 6
tbThapnienLenhbai.nMaxExpChuanguan = 20000000 --Vượt qua ải 1 lần max
tbThapnienLenhbai.nExpChuanguan = 5000000 --Vượt qua ải 1 lần


tbThapnienLenhbai.TASK_ID_MAP = {
	[4] = {
		--150-169 thôn trấn
		[1] = {
			[42] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[43] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
		},	
		--170-199 thành thị
		[2] = {
			[11] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[12] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
		}
	},
	[5] = {
		--10->119 thôn
		[3] = {
			[415] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[414] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
			[413] =  {nTaskID = 2966,nStartBit = 1,nBitCount = 9,nMaxValue = 200},
		},
		--120->134  [Map] = 19
		[4] = {
			[11] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[12] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
		},
		--135->149  [Map] = 92
		[5] = {
			[37] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 300},
			[17] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 300},
		},
		--150->154 - [Map] = 167
		[6] = {
			[32] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 300},
			[20] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 300},
		},
		[7] = {
			[153] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 300},
			[151] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 300},
		},
		[8] = {
			[24] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 400},
			[26] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 400},
		},
		[9] = {
			[588] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[589] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[10] = {
			[146] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[156] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[11] = {
			[598] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[599] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[12] = {
			[594] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[596] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[13] = {
			[155] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[156] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[14] = {
			[559] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[561] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[15] = {
			[535] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[536] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
		[16] = {
			[13] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 500},
			[14] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 500},
		},
	}
}
--Phần tính năng
tbThapnienLenhbai.TASK_INFOR_ABOUNT = {
	[6] = {
		[6] = {nTaskID = 2966,nStartBit = 1,nBitCount = 6,nMaxValue = 40}, -- Thăm May Mắn
		[8] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Phú Quý Cẩm Hạp
	},
	[7] = {
		[3] = {nTaskID = 2966,nStartBit = 1,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
	},
	[8] = {
		[3] = {nTaskID = 2966,nStartBit = 1,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
	},
	[9] = {
		[1] = {nTaskID = 2966,nStartBit = 1,nBitCount = 2,nMaxValue = 2}, -- Tống kim
		[2] = {nTaskID = 2966,nStartBit = 3,nBitCount = 2,nMaxValue = 2}, --Vượt ải
		[3] = {nTaskID = 2966,nStartBit = 5,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
	},
	[10] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vuợt ải
		[3] = {nTaskID = 2966,nStartBit = 9,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
	},
	[11] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 1}, --Viêm đế
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --Cây thiên lộc
	},
	[12] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 2}, --Viêm đế
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --Cây thiên lộc
	},
	[13] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viêm đế
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --Cây thiên lộc
	},
	[14] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viêm đế
		[5] = {nTaskID = 2966,nStartBit = 20,nBitCount = 2,nMaxValue = 1}, --Phong lang độ
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 480}, --Cây thiên lộc
	},
	[15] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viêm đế
		[5] = {nTaskID = 2966,nStartBit = 11,nBitCount = 2,nMaxValue = 2}, --Phong lang độ
		[3] = {nTaskID = 2966,nStartBit = 20,nBitCount = 9,nMaxValue = 480}, --Cây thiên lộc
	},
	[16] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viêm đế
		[5] = {nTaskID = 2966,nStartBit = 20,nBitCount = 2,nMaxValue = 3}, --Phong lang độ
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 480}, --Cây thiên lộc
	},
	[17] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 1}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
	},
	[18] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 2}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 1}, --Vượt ải
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 1}, --Phong lang độ
	},
	[19] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 2}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --Cây thiên lộc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --Vượt ải
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 1}, --Phong lang độ
	},
	[20] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --Cây thiên lộc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --Vượt ải
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 2}, --Phong lang độ
	},
	[21] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --Cây thiên lộc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --Vượt ải
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 2}, --Phong lăng độ
		[1] = {nTaskID = 2966,nStartBit = 25,nBitCount = 2,nMaxValue = 2}, --Tống Kim
		[4] = {nTaskID = 2966,nStartBit = 27,nBitCount = 2,nMaxValue = 2}, --Viêm đế
	},
	[22] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Võ Lâm Minh Chủ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Thăm may mắn
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --Cây thiên lộc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 3}, --Vượt ải
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 3}, --Phong lang độ
		[1] = {nTaskID = 2966,nStartBit = 25,nBitCount = 2,nMaxValue = 3}, --Tống Kim
		[4] = {nTaskID = 2966,nStartBit = 27,nBitCount = 2,nMaxValue = 3}, --Viêm đế
	},
}
tbThapnienLenhbai.TASK_NAME_TASK = {
	[1] = "Tống Kim",
	[2] = "Vượt ải",
	[3] = "Cây Thiên Lộc",
	[4] = "Viêm Đế",
	[5] = "Phong Lang Độ",
	[6] = "Thăm May Mắn",
	[7] = "Võ Lâm Minh Chủ",
	[8] = "Phú Quý Cẩm Hạp",
}
tbThapnienLenhbai.TASK_NAME_NPC = {
	--150-169 thôn trấn
	[1] = {
		[42] =  "Hươu Đốm",
		[43] =  "Heo Trắng",
	},	
	--170-199 thành thị
	[2] = {
		[11] =  "Heo Rừng",
		[12] =  "Nhím",
	},
	--10->119 thôn
	[3] = {
		[415] =  "Bao Cát",
		[414] =  "Mộc Nhân",
		[413] =  "Cọc Gỗ",
	},
	--120->134  [Map] = 19
	[4] = {
		[11] =  "Heo Rừng",
		[12] =  "Nhím",
	},
	--135->149  [Map] = 92
	[5] = {
		[37] =  "Hắc Diệp Hầu",
		[17] =  "Trâu Rừng",
	},
	--150->154 - [Map] = 167
	[6] = {
		[32] =  "Xá Lị",
		[20] =  "Nhãn Kinh Xà",
	},
	[7] = {
		[153] =  "Phong Minh",
		[151] =  "Mãng Hán",
	},
	[8] = {
		[24] =  "Kim Điêu",
		[26] =  "Kền Kền",
	},
	[9] = {
		[588] =  "Bôn Lôi",
		[589] =  "Ngân Nha",
	},
	[10] = {
		[146] =  "Đoạt Mệnh Liêm",
		[156] =  "ảnh Côn",
	},
	[11] = {
		[598] =  "Sương Đao",
		[599] =  "Lãnh Cung",
	},
	[12] = {
		[594] =  "Đao Trảm",
		[596] =  "Hàn Thương",
	},
	[13] = {
		[155] =  "Hắc Cân",
		[156] =  "ảnh Côn",
	},
	[14] = {
		[559] =  "Lưu Phong",
		[561] =  "Lưu Vân",
	},
	[15] = {
		[535] =  "Tật Phong",
		[536] =  "Tất Vũ",
	},
	[16] = {
		[13] =  "Đại Tượng",
		[14] =  "Voi Hoàng Hà",
	},
}

--Trả về thông tin của nhiệm vụ theo ID
function tbThapnienLenhbai:GetAwardInfo(nEventId)
	if(not nEventId or not self.tbAllAwards[nEventId]) then
		return
	end
	return self.tbAllAwards[nEventId]
end

--Trả về thông tin gammer có thể nhận, đang trong quá trình xử lý hoặc là đã hoàn thành được nhiệm vụ theo ID
--1: đã nhận nhiệm vụ
--2: đã hoàn thành nhiệm vụ nhưng chưa nhận thưởng
--3: đã nhận thưởng nhiệm vụ
--4: Chưa nhận nhiệm vụ
function tbThapnienLenhbai:GetEventIdInfo(nID)
	for i=1, self.nMaxEventId do
		local tbAward = self:GetAwardInfo(i)
		if(self:IsShowDlg(i,tbAward[1].nTaskIDStart,tbAward[1].nBitStart) == nID) then
			return i
		end
	end
end

function tbThapnienLenhbai:IsShowDlg(nEventId,nTaskID,nBit)
	local tbAward = self:GetAwardInfo(nEventId)
	if(not tbAward) then
		return 0
	end
	local nTransLife = ST_GetTransLifeCount()
	if(nTransLife ~= tbAward[1].nTransLife) then
		return 0
	end
	local nPlayerLevel = GetLevel()
	if(nPlayerLevel < tbAward[1].nMinLevel or nPlayerLevel > tbAward[1].nMaxLevel) then
		return 0
	end
	local tbBitTSK = {nTaskID = nTaskID,nStartBit = nBit,nBitCount = 2,nMaxValue = 3}
	local nCurTskVal = tbVNG_BitTask_Lib:getBitTask(tbBitTSK)
	if(nCurTskVal ~= 0) then
		return nCurTskVal
	end
	return 4
end
--Function kiểm tra người chơi mới
--Có thập niên lệnh bài & Kích hoạt mã code
function tbThapnienLenhbai:IsNewPlayer()
	if (tbExtPointLib:GetBitValueByVersion(self.EXT_POINT_ID_1) == 0)then
		return 0
	end
	if(CalcItemCount(-1, 6, 1, 30458, -1) < 1) then
		return 2
	end
	return 1
end

function tbThapnienLenhbai:Award(tbPlayers, strLog)
	local nNewPlayersCount = 0
	local nOldPlayer = PlayerIndex
	--Đếm số lượng tân thủ trong tổ đội
	for i = 1, getn(tbPlayers) do
		PlayerIndex = tbPlayers[i]
		if self:IsNewPlayer() == 1 and GetLife(0) > 0 then
			nNewPlayersCount = nNewPlayersCount + 1
		end
	end
	if nNewPlayersCount <= 0 then
		return
	end
	local nExpAward = nNewPlayersCount*self.nExpChuanguan
	if nExpAward > self.nMaxExpChuanguan then
		nExpAward = self.nMaxExpChuanguan
	end
	for i = 1, getn(tbPlayers) do
		PlayerIndex = tbPlayers[i]
		local tbExp = {szName = "Điểm Kinh Nghiệm", nExp = nExpAward}
		tbAwardTemplet:Give(tbExp, 1, {self.EVENT_LOG_TITLE, strLog})		
		Msg2Player(format("<color=green>Tổ đội có <color><color=yellow>%d<color><color=green> tân thủ, nhận được phần thưởng bổ sung: <color><color=yellow>%d điểm kinh nghiệm.<color>", nNewPlayersCount, nExpAward))
	end
	PlayerIndex = nOldPlayer
end
--Function tăng đẳng cấp của nhân vật
_AddLevel = function (nId,nLevel,szLog)
	local szMsg = "Tăng lên cấp %d"
	if(nId ==  1) then	
		ST_LevelUp(nLevel - GetLevel())
	else
		SetTask(4000, GetTask(4000) + nLevel)
		szMsg = "Nhận được %d điểm Chân Nguyên"
	end
	if(not szLog) then szLog = "NhanLevel" end
	Msg2Player(format(szMsg,nLevel))
	tbLog:PlayerActionLog(tbThapnienLenhbai.EVENT_LOG_TITLE,szLog,format(szMsg,nLevel))
end

--Function tăng nguyên liệu cho pet
_AwardTask = function (nUpgrade,nGrown,nTame,szLog)
	PET_SetUpgradePoint(PET_GetUpgradePoint() + nUpgrade)
	PET_SetGrownPoint(PET_GetGrownPoint() + nGrown)
	PET_SetTamePoint(PET_GetTamePoint() + nTame)
	Msg2Player(format("Nhận được %d điểm Thăng Cấp, %d điểm Tăng Trưởng, %d điểm Tu Luyện",nUpgrade,nGrown,nTame))
	if(not szLog) then szLog = "NhanNguyenLieu" end
	tbLog:PlayerActionLog(tbThapnienLenhbai.EVENT_LOG_TITLE,szLog,format("Nhận được %d điểm Thăng Cấp, %d điểm Tăng Trưởng, %d điểm Tu Luyện",nUpgrade,nGrown,nTame))
end

tbThapnienLenhbai.tbAward_List = {
	[1] = {
		[1] = {szName="Đại Thành Bí Kíp cấp 90", tbProp={6,1,2424,1,0,0},nBindState = -2,nCount=1,nBindState=-2},
		[2] = {szName="Hồi thành phù (lớn)",tbProp={6,1,1083,1,0,0},nBindState = -2,nCount=1,nBindState=-2},
		[3] = {szName="Bát Thuật Chân Kinh",tbProp={6,1,30126,1,0,0},nBindState = -2,nCount=1,nBindState=-2},	
		[4] = {szName = "450 điểm danh vọng", nRepute = 450},
		[5] = {szName = "Tăng cấp lên 170", pFun = function()  _AddLevel(1,170,"NhiemVu_1") end },
	},
	[2] = {
		[1] = {szName="Chuyển sinh thuật(cấp 5)", tbProp={6,1,30246,1,0,0},nBindState = -2,nCount=2,nExpiredTime=20160},
		[2] = {szName="Bắc đẩu trường sinh thuật-Cơ sở thiên(7 ngày)", tbProp={6,1,1390,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[3] = {szName="Hộ Mạch Bao",tbProp={6,1,4323,1,0,0},nBindState = -2,nCount=2},
		[4] = {szName = "Tăng cấp lên 200", pFun = function()  _AddLevel(1,200,"NhiemVu_2") end },
		[5] = {szName = "50.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,50000,"NhiemVu_2") end },
	},
	[3] = {
		[1] = {szName="Đại Thành Bí Kíp 120(7 ngày)", tbProp={6,1,2425,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[2] = {szName = "Tăng cấp lên 120", pFun = function()  _AddLevel(1,120,"NhiemVu_3") end },
		[3] = {szName = "30.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,30000,"NhiemVu_3") end },
		[4] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=12000},
	},
	[4] = {
		[1] = {szName = "Tăng cấp lên 135", pFun = function()  _AddLevel(1,135,"NhiemVu_4") end },
		[2] = {szName = "30.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,30000,"NhiemVu_4") end },
		[3] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=12000},
	},
	[5] = {
		[1] = {szName="Võ Học Kinh Nhu(14 ngày)", tbProp={6,1,2921,1,0,0},nBindState = -2,nCount=1,nExpiredTime=20160},
		[2] = {szName="Hộ Mạch Bao",tbProp={6,1,4323,1,0,0},nBindState = -2,nCount=2},
		[3] = {szName = "Tăng cấp lên 150", pFun = function()  _AddLevel(1,150,"NhiemVu_5") end },
		[4] = {szName = "50.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,50000,"NhiemVu_5") end },
	},
	[6] = {
		[1] = {szName="Bộ Xích Lân - Tự chọn hệ phái",tbProp={6,1,30386,1,0,0},nCount=1,nBindState = -2,tbParam={11,1,86400,0,0,0}},
		[2] = {szName="Đại thành bí kíp(150)",tbProp={6,1,30446,1,0,0},nBindState = -2,nCount=1},
		[3] = {szName="Mặt na-Anh hùng chiến trường(60 ngày)",tbProp={0,11,482,1,0,0},nBindState = -2,nCount=1,nExpiredTime=86400},
		[4] = {szName="Phi phong ngự phong(Hóa giải, 60 ngày)",tbProp={0,3474},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[5] = {szName="Phi phong ngự phong(Trọng kích, 60 ngày)",tbProp={0,3475},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[6] = {szName="Ngựa phiên vũ(90 ngày)",tbProp={0,10,7,1,0,0},nBindState = -2,nCount=1,nExpiredTime=129600},
		[7] = {szName="Cuồng lan 3%(60 ngày, không thể gia hạn)",tbProp={0,4490},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[8] = {szName="Hoàng Kim ấn cấp 6(Nhược hóa, 60 ngày)",tbProp={0,3220},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[9] = {szName="Hoàng Kim ấn cấp 6(Cường hóa, 60 ngày)",tbProp={0,3210},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[10] = {szName="Càn khôn song tuyệt bội(7 ngày)",tbProp={6,1,2219,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[11] = {szName="Hộ Mạch Bao",tbProp={6,1,4323,1,0,0},nBindState = -2,nCount=5},
		[12] = {szName = "Tăng cấp lên 154", pFun = function()  _AddLevel(1,154,"NhiemVu_6") end },
		[13] = {szName = "100.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,100000,"NhiemVu_6") end },
		[14] = {szName = "7.000.000.000 Điểm Kinh Nghiệm", nExp=7000e6},
	},
	[7] = {
		[1] = {szName="Thiệp đồng hành", tbProp={6,1,3453,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName = "Tăng điểm bạn đồng hành", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_7") end },
		[3] = {szName="Thuốc tăng trưởng", tbProp={6,1,3454,1,0,0},nBindState = -2,nCount=8},
		[4] = {szName="Huyết long đằng cấp 5", tbProp={6,1,30289,5,0,0},nBindState = -2,nCount=50},
		[6] = {szName = "Tăng cấp lên 159", pFun = function()  _AddLevel(1,159,"NhiemVu_7") end },
		[7] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_7") end },
		[8] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[9] = {szName = "11.000.000.000 Điểm Kinh Nghiệm", nExp=11000e6},
	},
	[8] = {
		[1] = {szName="Thiên sơn thánh thủy(đại)", tbProp={6,1,30449,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName = "Tăng cấp lên 164", pFun = function()  _AddLevel(1,164,"NhiemVu_8") end },
		[3] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_8") end },
		[4] = {szName="2000 Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[5] = {szName = "17.000.000.000 Điểm Kinh Nghiệm", nExp=17000e6},
	},
	[9] = {
		[1] = {szName="Huyết long đằng 6", tbProp={6,1,30289,6,0,0},nBindState = -2,nCount=50},
		[2] = {szName="100 vạn Ngân lượng",nJxb=1000000,nCount=1},
		[3] = {szName = "Tăng cấp lên 169", pFun = function()  _AddLevel(1,169,"NhiemVu_9") end },
		[5] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_9") end },
		[6] = {szName="2000 Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[7] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[10] = {
		[1] = {szName = "Tăng điểm bạn đồng hành", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_10") end },
		[2] = {szName="Thuốc tăng trưởng", tbProp={6,1,3454,1,0,0},nBindState = -2,nCount=8},
		[3] = {szName="Thiên sơn thánh thủy(đại)", tbProp={6,1,30449,1,0,0},nBindState = -2,nCount=1},
		[4] = {szName="200 vạn Ngân lượng",nJxb=2000000,nCount=1},
		[5] = {szName = "Tăng cấp lên 174", pFun = function()  _AddLevel(1,174,"NhiemVu_10") end },
		[6] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_10") end },
		[7] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[8] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[11] = {
		[1] = {szName="Huyết long đằng cấp 8", tbProp={6,1,30289,8,0,0},nBindState = -2,nCount=50},
		[2] = {szName="200 vạn Ngân lượng",nJxb=2000000,nCount=1},
		[3] = {szName = "Tăng cấp lên 179", pFun = function()  _AddLevel(1,179,"NhiemVu_11") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_11") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},		
	},
	[12] = {
		[1] = {szName="Thiên sơn thánh thủy(đại)", tbProp={6,1,30449,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName="300 vạn Ngân lượng",nJxb=3000000,nCount=1},
		[3] = {szName = "Tăng cấp lên 184", pFun = function()  _AddLevel(1,184,"NhiemVu_12") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_12") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[13] = {
		[1] = {szName = "Tăng cấp lên 189", pFun = function()  _AddLevel(1,189,"NhiemVu_13") end },		
		[2] = {szName = "Tăng điểm bạn đồng hành", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_13") end },
		[3] = {szName="Thuốc tăng trưởng", tbProp={6,1,3454,1,0,0},nBindState = -2,nCount=8},
		[4] = {szName="Huyết long đằng cấp 9", tbProp={6,1,30289,9,0,0},nBindState = -2,nCount=50},
		[5] = {szName="300 vạn Ngân lượng",nJxb=3000000,nCount=1},
		[6] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[7] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_13") end },
		[8] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[14] = {
		[1] = {szName="Thiên sơn thánh thủy(đại)", tbProp={6,1,30449,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName="400 vạn Ngân lượng",nJxb=4000000,nCount=1},
		[3] = {szName = "Tăng cấp lên 194", pFun = function()  _AddLevel(1,194,"NhiemVu_14") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_14") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[15] = {
		[1] = {szName="Huyết long đằng cấp 12", tbProp={6,1,30289,12,0,0},nBindState = -2,nCount=50},
		[2] = {szName="400 vạn Ngân lượng",nJxb=4000000,nCount=1},
		[3] = {szName = "Tăng cấp lên 198", pFun = function()  _AddLevel(1,198,"NhiemVu_15") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_15") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 Điểm Kinh Nghiệm", nExp=28000e6},
	},
	[16] = {
		[1] = {szName="Bắc đẩu trường sinh thuật-Cơ sở thiên(7 ngày)", tbProp={6,1,1390,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[2] = {szName="Ngọc chuyển sinh(7 ngày)", tbProp={6,1,3557,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[3] = {szName="Phục sinh hoàn(7 ngày)", tbProp={6,1,3558,1,0,0},nBindState = -2,nCount=2330,nExpiredTime=10080},
		[4] = {szName="500 vạn Ngân lượng",nJxb=5000000,nCount=1},
		[5] = {szName = "Tăng điểm bạn đồng hành", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_16") end },
		[6] = {szName="Thuốc tăng trưởng", tbProp={6,1,3454,1,0,0},nBindState = -2,nCount=10},
		[7] = {szName = "Tăng cấp lên 200", pFun = function()  _AddLevel(1,200,"NhiemVu_16") end },
		[8] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_16") end },
		[9] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[10] = {szName = "55.000.000.000 Điểm Kinh Nghiệm", nExp=55000e6},
	},
	[17] = {
		[1] = {szName="Bộ Trang Bị Minh Phượng - Tự chọn hệ phái",tbProp={6,1,30474,1,0,0},nBindState = -2,nCount=1,tbParam={11,1,86400,0,0,0}},
		[2] = {szName="Bát thuật chân kinh", tbProp={6,1,30126,1,0,0},nBindState = -2,nCount=3},
		[3] = {szName="1000 vạn Ngân lượng",nJxb=10000000,nCount=1},
		[4] = {szName = "Tăng cấp lên 179", pFun = function()  _AddLevel(1,120,"NhiemVu_17") end },
		[5] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,10000,"NhiemVu_17") end },
		[6] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=4000},
	},
	[18] = {
		[1] = {szName="Hoàng kim ấn cấp 7(Nhược hóa, 60 ngày)", tbProp={0,3221},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=86400},
		[2] = {szName="Hoàng kim ấn cấp 7(Cường hóa, 60 ngày)", tbProp={0,3211},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=86400},
		[3] = {szName = "Tăng cấp lên 130", pFun = function()  _AddLevel(1,130,"NhiemVu_18") end },
		[5] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_18") end },
		[6] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
	},
	[19] = {
		[1] = {szName="Cuồng lan 5%(Không gia hạn)", tbProp={0,4492},nBindState = -2,nCount=1,nQuality=1,nExpiredTime=43200},
		[2] = {szName = "Tăng cấp lên 140", pFun = function()  _AddLevel(1,140,"NhiemVu_19") end },
		[3] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_19") end },
		[4] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
	},
	[20] = {
		[1] = {szName="Phi phong phệ quang(Hóa giải)", tbProp={0,3477},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=43200},
		[2] = {szName="Phi phong phệ quang(Trọng kích)", tbProp={0,3478},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=43200},
		[3] = {szName = "Tăng cấp lên 150", pFun = function()  _AddLevel(1,150,"NhiemVu_20") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_20") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
	},
	[21] = {
		[1] = {szName = "Tăng điểm bạn đồng hành", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_21") end },
		[2] = {szName="Thuốc tăng trưởng", tbProp={6,1,3454,1,0,0},nBindState = -2,nCount=11},
		[3] = {szName = "Tăng cấp lên 154", pFun = function()  _AddLevel(1,154,"NhiemVu_21") end },
		[4] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_21") end },
		[5] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "6.000.000.000 Điểm Kinh Nghiệm", nExp=6000e6},
	},
	[22] = {
		[1] = {szName="Mặt nạ-Chiến trường vương giả(30 ngày)", tbProp={0,11,647,1,0,0},nBindState = -2,nCount=1,nExpiredTime=43200},
		[2] = {szName = "Tăng cấp lên 159", pFun = function()  _AddLevel(1,159,"NhiemVu_22") end },
		[3] = {szName = "5.000 Điểm Chân Nguyên", pFun = function()  _AddLevel(2,5000,"NhiemVu_22") end },
		[4] = {szName="Hộ Mạch Đơn",tbProp={6,1,3203,1,0,0},nBindState = -2,nCount=2000},
	},
}