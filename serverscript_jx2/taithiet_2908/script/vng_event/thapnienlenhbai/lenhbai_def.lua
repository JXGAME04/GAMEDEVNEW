-- [NAN5MA 30/08] nan ma vat pham theo TEN, tra bang DUNG cach may tra (theo chi so dong).
-- Xem ReverseTools/cauhinh/t23_nan_5_ma_con_lai.py va ra_soat_ma_thuong.txt
-- [MATHUONG 29/08] nan ma vat pham bang thuong theo TEN (xem ReverseTools\cauhinh\t01_nan_ma_thuong.py)
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

tbThapnienLenhbai.EXT_POINT_ID_1 = {nID = 5, nBit = 1, nVer = 2} --KÝch ho¹t m· code t¹i web

--PartySupport
tbThapnienLenhbai.nCOTBatchAward = 20 --V­ît qua ¶i 20
tbThapnienLenhbai.nYDBZBatchAward = 6 --V­ît qua ¶i 6
tbThapnienLenhbai.nMaxExpChuanguan = 20000000 --V­ît qua ¶i 1 lÇn max
tbThapnienLenhbai.nExpChuanguan = 5000000 --V­ît qua ¶i 1 lÇn


tbThapnienLenhbai.TASK_ID_MAP = {
	[4] = {
		--150-169 th«n trÊn
		[1] = {
			[42] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[43] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
		},	
		--170-199 thµnh thÞ
		[2] = {
			[11] =  {nTaskID = 2971,nStartBit = 7,nBitCount = 9,nMaxValue = 200},
			[12] =  {nTaskID = 2971,nStartBit = 16,nBitCount = 9,nMaxValue = 200},
		}
	},
	[5] = {
		--10->119 th«n
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
--PhÇn tÝnh n¨ng
tbThapnienLenhbai.TASK_INFOR_ABOUNT = {
	[6] = {
		[6] = {nTaskID = 2966,nStartBit = 1,nBitCount = 6,nMaxValue = 40}, -- Th¨m May M¾n
		[8] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Phó Quý CÈm H¹p
	},
	[7] = {
		[3] = {nTaskID = 2966,nStartBit = 1,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
	},
	[8] = {
		[3] = {nTaskID = 2966,nStartBit = 1,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
	},
	[9] = {
		[1] = {nTaskID = 2966,nStartBit = 1,nBitCount = 2,nMaxValue = 2}, -- Tèng kim
		[2] = {nTaskID = 2966,nStartBit = 3,nBitCount = 2,nMaxValue = 2}, --V­ît ¶i
		[3] = {nTaskID = 2966,nStartBit = 5,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
	},
	[10] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --Vuît ¶i
		[3] = {nTaskID = 2966,nStartBit = 9,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
	},
	[11] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 1}, --Viªm ®Õ
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --C©y thiªn léc
	},
	[12] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 2}, --Viªm ®Õ
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --C©y thiªn léc
	},
	[13] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viªm ®Õ
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 320}, --C©y thiªn léc
	},
	[14] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viªm ®Õ
		[5] = {nTaskID = 2966,nStartBit = 20,nBitCount = 2,nMaxValue = 1}, --Phong lang ®é
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 480}, --C©y thiªn léc
	},
	[15] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viªm ®Õ
		[5] = {nTaskID = 2966,nStartBit = 11,nBitCount = 2,nMaxValue = 2}, --Phong lang ®é
		[3] = {nTaskID = 2966,nStartBit = 20,nBitCount = 9,nMaxValue = 480}, --C©y thiªn léc
	},
	[16] = {	
		[1] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[2] = {nTaskID = 2966,nStartBit = 7,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[4] = {nTaskID = 2966,nStartBit = 9,nBitCount = 2,nMaxValue = 3}, --Viªm ®Õ
		[5] = {nTaskID = 2966,nStartBit = 20,nBitCount = 2,nMaxValue = 3}, --Phong lang ®é
		[3] = {nTaskID = 2966,nStartBit = 11,nBitCount = 9,nMaxValue = 480}, --C©y thiªn léc
	},
	[17] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 1}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
	},
	[18] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 2}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 1}, --V­ît ¶i
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 1}, --Phong lang ®é
	},
	[19] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 2}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 5,nMaxValue = 20}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 12,nBitCount = 9,nMaxValue = 160}, --C©y thiªn léc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --V­ît ¶i
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 1}, --Phong lang ®é
	},
	[20] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --C©y thiªn léc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --V­ît ¶i
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 2}, --Phong lang ®é
	},
	[21] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --C©y thiªn léc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 2}, --V­ît ¶i
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 2}, --Phong l¨ng ®é
		[1] = {nTaskID = 2966,nStartBit = 25,nBitCount = 2,nMaxValue = 2}, --Tèng Kim
		[4] = {nTaskID = 2966,nStartBit = 27,nBitCount = 2,nMaxValue = 2}, --Viªm ®Õ
	},
	[22] = {	
		[7] = {nTaskID = 2966,nStartBit = 5,nBitCount = 2,nMaxValue = 3}, --Vâ L©m Minh Chñ
		[6] = {nTaskID = 2966,nStartBit = 7,nBitCount = 6,nMaxValue = 40}, --Th¨m may m¾n
		[3] = {nTaskID = 2966,nStartBit = 13,nBitCount = 8,nMaxValue = 160}, --C©y thiªn léc
		[2] = {nTaskID = 2966,nStartBit = 21,nBitCount = 2,nMaxValue = 3}, --V­ît ¶i
		[5] = {nTaskID = 2966,nStartBit = 23,nBitCount = 2,nMaxValue = 3}, --Phong lang ®é
		[1] = {nTaskID = 2966,nStartBit = 25,nBitCount = 2,nMaxValue = 3}, --Tèng Kim
		[4] = {nTaskID = 2966,nStartBit = 27,nBitCount = 2,nMaxValue = 3}, --Viªm ®Õ
	},
}
tbThapnienLenhbai.TASK_NAME_TASK = {
	[1] = "Tèng Kim",
	[2] = "V­ît ¶i",
	[3] = "C©y Thiªn Léc",
	[4] = "Viªm §Õ",
	[5] = "Phong Lang §é",
	[6] = "Th¨m May M¾n",
	[7] = "Vâ L©m Minh Chñ",
	[8] = "Phó Quý CÈm H¹p",
}
tbThapnienLenhbai.TASK_NAME_NPC = {
	--150-169 th«n trÊn
	[1] = {
		[42] =  "H­¬u §èm",
		[43] =  "Heo Tr¾ng",
	},	
	--170-199 thµnh thÞ
	[2] = {
		[11] =  "Heo Rõng",
		[12] =  "NhÝm",
	},
	--10->119 th«n
	[3] = {
		[415] =  "Bao C¸t",
		[414] =  "Méc Nh©n",
		[413] =  "Cäc Gç",
	},
	--120->134  [Map] = 19
	[4] = {
		[11] =  "Heo Rõng",
		[12] =  "NhÝm",
	},
	--135->149  [Map] = 92
	[5] = {
		[37] =  "H¾c DiÖp HÇu",
		[17] =  "Tr©u Rõng",
	},
	--150->154 - [Map] = 167
	[6] = {
		[32] =  "X¸ LÞ",
		[20] =  "Nh·n Kinh Xµ",
	},
	[7] = {
		[153] =  "Phong Minh",
		[151] =  "M·ng H¸n",
	},
	[8] = {
		[24] =  "Kim §iªu",
		[26] =  "KÒn KÒn",
	},
	[9] = {
		[588] =  "B«n L«i",
		[589] =  "Ng©n Nha",
	},
	[10] = {
		[146] =  "§o¹t MÖnh Liªm",
		[156] =  "¶nh C«n",
	},
	[11] = {
		[598] =  "S­¬ng §ao",
		[599] =  "L·nh Cung",
	},
	[12] = {
		[594] =  "§ao Tr¶m",
		[596] =  "Hµn Th­¬ng",
	},
	[13] = {
		[155] =  "H¾c C©n",
		[156] =  "¶nh C«n",
	},
	[14] = {
		[559] =  "L­u Phong",
		[561] =  "L­u V©n",
	},
	[15] = {
		[535] =  "TËt Phong",
		[536] =  "TÊt Vò",
	},
	[16] = {
		[13] =  "§¹i T­îng",
		[14] =  "Voi Hoµng Hµ",
	},
}

--Tr¶ vÒ th«ng tin cña nhiÖm vô theo ID
function tbThapnienLenhbai:GetAwardInfo(nEventId)
	if(not nEventId or not self.tbAllAwards[nEventId]) then
		return
	end
	return self.tbAllAwards[nEventId]
end

--Tr¶ vÒ th«ng tin gammer cã thÓ nhËn, ®ang trong qu¸ tr×nh xö lý hoÆc lµ ®· hoµn thµnh ®­îc nhiÖm vô theo ID
--1: ®· nhËn nhiÖm vô
--2: ®· hoµn thµnh nhiÖm vô nh­ng ch­a nhËn th­ëng
--3: ®· nhËn th­ëng nhiÖm vô
--4: Ch­a nhËn nhiÖm vô
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
--Function kiÓm tra ng­êi ch¬i míi
--Cã thËp niªn lÖnh bµi & KÝch ho¹t m· code
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
	--§Õm sè l­îng t©n thñ trong tæ ®éi
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
		local tbExp = {szName = "§iÓm Kinh NghiÖm", nExp = nExpAward}
		tbAwardTemplet:Give(tbExp, 1, {self.EVENT_LOG_TITLE, strLog})		
		Msg2Player(format("<color=green>Tæ ®éi cã <color><color=yellow>%d<color><color=green> t©n thñ, nhËn ®­îc phÇn th­ëng bæ sung: <color><color=yellow>%d ®iÓm kinh nghiÖm.<color>", nNewPlayersCount, nExpAward))
	end
	PlayerIndex = nOldPlayer
end
--Function t¨ng ®¼ng cÊp cña nh©n vËt
_AddLevel = function (nId,nLevel,szLog)
	local szMsg = "T¨ng lªn cÊp %d"
	if(nId ==  1) then	
		ST_LevelUp(nLevel - GetLevel())
	else
		SetTask(4000, GetTask(4000) + nLevel)
		szMsg = "NhËn ®­îc %d ®iÓm Ch©n Nguyªn"
	end
	if(not szLog) then szLog = "NhanLevel" end
	Msg2Player(format(szMsg,nLevel))
	tbLog:PlayerActionLog(tbThapnienLenhbai.EVENT_LOG_TITLE,szLog,format(szMsg,nLevel))
end

--Function t¨ng nguyªn liÖu cho pet
_AwardTask = function (nUpgrade,nGrown,nTame,szLog)
	PET_SetUpgradePoint(PET_GetUpgradePoint() + nUpgrade)
	PET_SetGrownPoint(PET_GetGrownPoint() + nGrown)
	PET_SetTamePoint(PET_GetTamePoint() + nTame)
	Msg2Player(format("NhËn ®­îc %d ®iÓm Th¨ng CÊp, %d ®iÓm T¨ng Tr­ëng, %d ®iÓm Tu LuyÖn",nUpgrade,nGrown,nTame))
	if(not szLog) then szLog = "NhanNguyenLieu" end
	tbLog:PlayerActionLog(tbThapnienLenhbai.EVENT_LOG_TITLE,szLog,format("NhËn ®­îc %d ®iÓm Th¨ng CÊp, %d ®iÓm T¨ng Tr­ëng, %d ®iÓm Tu LuyÖn",nUpgrade,nGrown,nTame))
end

tbThapnienLenhbai.tbAward_List = {
	[1] = {
		[1] = {szName="§¹i Thµnh BÝ KÝp cÊp 90", tbProp={6,1,2433,1,0,0},nBindState = -2,nCount=1,nBindState=-2},
		[2] = {szName="Håi thµnh phï (lín)",tbProp={6,1,1084,1,0,0},nBindState = -2,nCount=1,nBindState=-2},
		[3] = {szName="B¸t ThuËt Ch©n Kinh",tbProp={6,1,2888,1,0,0},nBindState = -2,nCount=1,nBindState=-2},	
		[4] = {szName = "450 ®iÓm danh väng", nRepute = 450},
		[5] = {szName = "T¨ng cÊp lªn 170", pFun = function()  _AddLevel(1,170,"NhiemVu_1") end },
	},
	[2] = {
		[1] = {szName="ChuyÓn sinh thuËt(cÊp 5)", tbProp={6,1,30246,1,0,0},nBindState = -2,nCount=2,nExpiredTime=20160},
		[2] = {szName="B¾c ®Èu tr­êng sinh thuËt-C¬ së thiªn(7 ngµy)", tbProp={6,1,1399,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[3] = {szName="Hé M¹ch Bao",tbProp={6,1,4793,1,0,0},nBindState = -2,nCount=2},
		[4] = {szName = "T¨ng cÊp lªn 200", pFun = function()  _AddLevel(1,200,"NhiemVu_2") end },
		[5] = {szName = "50.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,50000,"NhiemVu_2") end },
	},
	[3] = {
		[1] = {szName="§¹i Thµnh BÝ KÝp 120(7 ngµy)", tbProp={6,1,2434,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[2] = {szName = "T¨ng cÊp lªn 120", pFun = function()  _AddLevel(1,120,"NhiemVu_3") end },
		[3] = {szName = "30.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,30000,"NhiemVu_3") end },
		[4] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=12000},
	},
	[4] = {
		[1] = {szName = "T¨ng cÊp lªn 135", pFun = function()  _AddLevel(1,135,"NhiemVu_4") end },
		[2] = {szName = "30.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,30000,"NhiemVu_4") end },
		[3] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=12000},
	},
	[5] = {
		[1] = {szName="Vâ Häc Kinh Nhu(14 ngµy)", tbProp={6,1,3539,1,0,0},nBindState = -2,nCount=1,nExpiredTime=20160},
		[2] = {szName="Hé M¹ch Bao",tbProp={6,1,4793,1,0,0},nBindState = -2,nCount=2},
		[3] = {szName = "T¨ng cÊp lªn 150", pFun = function()  _AddLevel(1,150,"NhiemVu_5") end },
		[4] = {szName = "50.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,50000,"NhiemVu_5") end },
	},
	[6] = {
		[1] = {szName="Bé XÝch L©n - Tù chän hÖ ph¸i",tbProp={6,1,30386,1,0,0},nCount=1,nBindState = -2,tbParam={11,1,86400,0,0,0}},
		[2] = {szName="§¹i thµnh bÝ kÝp(150)",tbProp={6,1,3208,1,0,0},nBindState = -2,nCount=1},
		[3] = {szName="MÆt na-Anh hïng chiÕn tr­êng(60 ngµy)",tbProp={0,11,482,1,0,0},nBindState = -2,nCount=1,nExpiredTime=86400},
		[4] = {szName="Phi phong ngù phong(Hãa gi¶i, 60 ngµy)",tbProp={0,3474},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[5] = {szName="Phi phong ngù phong(Träng kÝch, 60 ngµy)",tbProp={0,3475},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[6] = {szName="Ngùa phiªn vò(90 ngµy)",tbProp={0,10,7,1,0,0},nBindState = -2,nCount=1,nExpiredTime=129600},
		[7] = {szName="Cuång lan 3%(60 ngµy, kh«ng thÓ gia h¹n)",tbProp={0,4490},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[8] = {szName="Hoµng Kim Ên cÊp 6(Nh­îc hãa, 60 ngµy)",tbProp={0,3220},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[9] = {szName="Hoµng Kim Ên cÊp 6(C­êng hãa, 60 ngµy)",tbProp={0,3210},nQuality=1,nBindState = -2,nCount=1,nExpiredTime=86400},
		[10] = {szName="Cµn kh«n song tuyÖt béi(7 ngµy)",tbProp={6,1,2228,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[11] = {szName="Hé M¹ch Bao",tbProp={6,1,4793,1,0,0},nBindState = -2,nCount=5},
		[12] = {szName = "T¨ng cÊp lªn 154", pFun = function()  _AddLevel(1,154,"NhiemVu_6") end },
		[13] = {szName = "100.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,100000,"NhiemVu_6") end },
		[14] = {szName = "7.000.000.000 §iÓm Kinh NghiÖm", nExp=7000e6},
	},
	[7] = {
		[1] = {szName="ThiÖp ®ång hµnh", tbProp={6,1,4071,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName = "T¨ng ®iÓm b¹n ®ång hµnh", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_7") end },
		[3] = {szName="Thuèc t¨ng tr­ëng", tbProp={6,1,4072,1,0,0},nBindState = -2,nCount=8},
		[4] = {szName="HuyÕt long ®»ng cÊp 5", tbProp={6,1,3051,5,0,0},nBindState = -2,nCount=50},
		[6] = {szName = "T¨ng cÊp lªn 159", pFun = function()  _AddLevel(1,159,"NhiemVu_7") end },
		[7] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_7") end },
		[8] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[9] = {szName = "11.000.000.000 §iÓm Kinh NghiÖm", nExp=11000e6},
	},
	[8] = {
		[1] = {szName="Thiªn s¬n th¸nh thñy(®¹i)", tbProp={6,1,3211,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName = "T¨ng cÊp lªn 164", pFun = function()  _AddLevel(1,164,"NhiemVu_8") end },
		[3] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_8") end },
		[4] = {szName="2000 Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[5] = {szName = "17.000.000.000 §iÓm Kinh NghiÖm", nExp=17000e6},
	},
	[9] = {
		[1] = {szName="HuyÕt long ®»ng 6", tbProp={6,1,3051,6,0,0},nBindState = -2,nCount=50},
		[2] = {szName="100 v¹n Ng©n l­îng",nJxb=1000000,nCount=1},
		[3] = {szName = "T¨ng cÊp lªn 169", pFun = function()  _AddLevel(1,169,"NhiemVu_9") end },
		[5] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_9") end },
		[6] = {szName="2000 Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[7] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[10] = {
		[1] = {szName = "T¨ng ®iÓm b¹n ®ång hµnh", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_10") end },
		[2] = {szName="Thuèc t¨ng tr­ëng", tbProp={6,1,4072,1,0,0},nBindState = -2,nCount=8},
		[3] = {szName="Thiªn s¬n th¸nh thñy(®¹i)", tbProp={6,1,3211,1,0,0},nBindState = -2,nCount=1},
		[4] = {szName="200 v¹n Ng©n l­îng",nJxb=2000000,nCount=1},
		[5] = {szName = "T¨ng cÊp lªn 174", pFun = function()  _AddLevel(1,174,"NhiemVu_10") end },
		[6] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_10") end },
		[7] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[8] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[11] = {
		[1] = {szName="HuyÕt long ®»ng cÊp 8", tbProp={6,1,3051,8,0,0},nBindState = -2,nCount=50},
		[2] = {szName="200 v¹n Ng©n l­îng",nJxb=2000000,nCount=1},
		[3] = {szName = "T¨ng cÊp lªn 179", pFun = function()  _AddLevel(1,179,"NhiemVu_11") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_11") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},		
	},
	[12] = {
		[1] = {szName="Thiªn s¬n th¸nh thñy(®¹i)", tbProp={6,1,3211,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName="300 v¹n Ng©n l­îng",nJxb=3000000,nCount=1},
		[3] = {szName = "T¨ng cÊp lªn 184", pFun = function()  _AddLevel(1,184,"NhiemVu_12") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_12") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[13] = {
		[1] = {szName = "T¨ng cÊp lªn 189", pFun = function()  _AddLevel(1,189,"NhiemVu_13") end },		
		[2] = {szName = "T¨ng ®iÓm b¹n ®ång hµnh", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_13") end },
		[3] = {szName="Thuèc t¨ng tr­ëng", tbProp={6,1,4072,1,0,0},nBindState = -2,nCount=8},
		[4] = {szName="HuyÕt long ®»ng cÊp 9", tbProp={6,1,3051,9,0,0},nBindState = -2,nCount=50},
		[5] = {szName="300 v¹n Ng©n l­îng",nJxb=3000000,nCount=1},
		[6] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[7] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_13") end },
		[8] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[14] = {
		[1] = {szName="Thiªn s¬n th¸nh thñy(®¹i)", tbProp={6,1,3211,1,0,0},nBindState = -2,nCount=1},
		[2] = {szName="400 v¹n Ng©n l­îng",nJxb=4000000,nCount=1},
		[3] = {szName = "T¨ng cÊp lªn 194", pFun = function()  _AddLevel(1,194,"NhiemVu_14") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_14") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[15] = {
		[1] = {szName="HuyÕt long ®»ng cÊp 12", tbProp={6,1,3051,12,0,0},nBindState = -2,nCount=50},
		[2] = {szName="400 v¹n Ng©n l­îng",nJxb=4000000,nCount=1},
		[3] = {szName = "T¨ng cÊp lªn 198", pFun = function()  _AddLevel(1,198,"NhiemVu_15") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_15") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "28.000.000.000 §iÓm Kinh NghiÖm", nExp=28000e6},
	},
	[16] = {
		[1] = {szName="B¾c ®Èu tr­êng sinh thuËt-C¬ së thiªn(7 ngµy)", tbProp={6,1,1399,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[2] = {szName="Ngäc chuyÓn sinh(7 ngµy)", tbProp={6,1,4175,1,0,0},nBindState = -2,nCount=1,nExpiredTime=10080},
		[3] = {szName="Phôc sinh hoµn(7 ngµy)", tbProp={6,1,4176,1,0,0},nBindState = -2,nCount=2330,nExpiredTime=10080},
		[4] = {szName="500 v¹n Ng©n l­îng",nJxb=5000000,nCount=1},
		[5] = {szName = "T¨ng ®iÓm b¹n ®ång hµnh", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_16") end },
		[6] = {szName="Thuèc t¨ng tr­ëng", tbProp={6,1,4072,1,0,0},nBindState = -2,nCount=10},
		[7] = {szName = "T¨ng cÊp lªn 200", pFun = function()  _AddLevel(1,200,"NhiemVu_16") end },
		[8] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_16") end },
		[9] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[10] = {szName = "55.000.000.000 §iÓm Kinh NghiÖm", nExp=55000e6},
	},
	[17] = {
		[1] = {szName="Bé Trang BÞ Minh Ph­îng - Tù chän hÖ ph¸i",tbProp={6,1,30474,1,0,0},nBindState = -2,nCount=1,tbParam={11,1,86400,0,0,0}},
		[2] = {szName="B¸t thuËt ch©n kinh", tbProp={6,1,2888,1,0,0},nBindState = -2,nCount=3},
		[3] = {szName="1000 v¹n Ng©n l­îng",nJxb=10000000,nCount=1},
		[4] = {szName = "T¨ng cÊp lªn 179", pFun = function()  _AddLevel(1,120,"NhiemVu_17") end },
		[5] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,10000,"NhiemVu_17") end },
		[6] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=4000},
	},
	[18] = {
		[1] = {szName="Hoµng kim Ên cÊp 7(Nh­îc hãa, 60 ngµy)", tbProp={0,3221},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=86400},
		[2] = {szName="Hoµng kim Ên cÊp 7(C­êng hãa, 60 ngµy)", tbProp={0,3211},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=86400},
		[3] = {szName = "T¨ng cÊp lªn 130", pFun = function()  _AddLevel(1,130,"NhiemVu_18") end },
		[5] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_18") end },
		[6] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
	},
	[19] = {
		[1] = {szName="Cuång lan 5%(Kh«ng gia h¹n)", tbProp={0,4492},nBindState = -2,nCount=1,nQuality=1,nExpiredTime=43200},
		[2] = {szName = "T¨ng cÊp lªn 140", pFun = function()  _AddLevel(1,140,"NhiemVu_19") end },
		[3] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_19") end },
		[4] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
	},
	[20] = {
		[1] = {szName="Phi phong phÖ quang(Hãa gi¶i)", tbProp={0,3477},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=43200},
		[2] = {szName="Phi phong phÖ quang(Träng kÝch)", tbProp={0,3478},nBindState = -2,nQuality=1,nCount=1,nExpiredTime=43200},
		[3] = {szName = "T¨ng cÊp lªn 150", pFun = function()  _AddLevel(1,150,"NhiemVu_20") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_20") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
	},
	[21] = {
		[1] = {szName = "T¨ng ®iÓm b¹n ®ång hµnh", pFun = function()  _AwardTask(160, 320, 500,"NhiemVu_21") end },
		[2] = {szName="Thuèc t¨ng tr­ëng", tbProp={6,1,4072,1,0,0},nBindState = -2,nCount=11},
		[3] = {szName = "T¨ng cÊp lªn 154", pFun = function()  _AddLevel(1,154,"NhiemVu_21") end },
		[4] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_21") end },
		[5] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
		[6] = {szName = "6.000.000.000 §iÓm Kinh NghiÖm", nExp=6000e6},
	},
	[22] = {
		[1] = {szName="MÆt n¹-ChiÕn tr­êng v­¬ng gi¶(30 ngµy)", tbProp={0,11,647,1,0,0},nBindState = -2,nCount=1,nExpiredTime=43200},
		[2] = {szName = "T¨ng cÊp lªn 159", pFun = function()  _AddLevel(1,159,"NhiemVu_22") end },
		[3] = {szName = "5.000 §iÓm Ch©n Nguyªn", pFun = function()  _AddLevel(2,5000,"NhiemVu_22") end },
		[4] = {szName="Hé M¹ch §¬n",tbProp={6,1,3821,1,0,0},nBindState = -2,nCount=2000},
	},
}