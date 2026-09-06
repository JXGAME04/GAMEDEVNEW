--------------------------------------------------------------------------------
-- weeklyrank.lua  (S3Relay)  [RELAYHT 06/09]
--
-- Nguon: D:\ServerLinux\gateway\s3relay\script\battle\weeklyrank.lua  (ban Linux)
-- Doi chieu hop dong voi: E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server
--                         \script\battles\weeklyrank.lua  (phia GameServer)
-- Ngay port: 06/09/2026.
--
-- CONG DUNG
--   Xep hang Tong Kim theo TUAN, luu ben vung tren ShareData cua relay (MySQL),
--   khoa "FUNC_SJ_WEEKLY_RANK". Ba nhom ban ghi:
--       (KEY, 0, 0)  = so nguoi co diem         (KEY, 0, i) = ten + diem tho
--       (KEY, 1, 0)  = so hang trong bang top   (KEY, 1, i) = ten + diem + co da nhan
--       (KEY, 2, 0)  = so tuan da chot bang (date("%W"))
--   GameServer cong diem tung nguoi (task 751) len day; relay tich luy, tu chot
--   bang top 10 khi sang tuan moi, tra top 10 de hien thi va cap co "da nhan thuong".
--
-- HOP DONG RPC - GIU NGUYEN, SAI MOT O LA HONG
--   Relay duoc goi bang lua_dostring "tbSJ_WeeklyRank:s_XXX(hParam,hRes,dwGameSvrId)"
--   (RelayRpc.cpp) nen moi ham s_* nhan (self, hParam, hResult) va bo qua tham so 3.
--     s_AddScore     hParam: [1] szName, [2] nScore.  hResult: KHONG day gi.
--     s_GetRank      hParam: [1] szName.
--                    hResult: [1] szName, [2] nCount, roi nCount cap (szName, nScore).
--     s_GetRankAward hParam: [1] szName.
--                    hResult: [1] szName, [2] nFound (1..10 = lan dau nhan,
--                             0 = da nhan roi, nil = khong co trong bang).
--
-- DA SUA SO VOI BAN LINUX (chuyen cu phap Lua 4 -> Lua 5.4)
--   1. Bo han Include("\\script\\lib\\common.lua"): cay relay JX1 khong co tep do.
--      Ban Linux chi muon no de lay unpack; shim lua4compat da cap san unpack.
--   2. SaveValue(...): Lua 5.4 khong tu tao bang "arg" -> them dong dau than ham
--      local arg = {n = select("#", ...), ...}  (dung n = select de getn dung ca
--      khi co tham so nil).
--   3. LoadValue: return unpack(tbReturn) -> unpack(tbReturn, 1, getn(tbReturn)).
--      [SOAT 06/09] Lua 4.0.1 KHONG co unpack san; ban Linux tu viet mot ham de quy
--      trong script\lib\common.lua. Chi ro can tren de ket qua xac dinh, khong phu
--      thuoc cho nao trong bang co lo.
--   4. UpdateRank: "for szName, tbData in self.tbScores do" -> them pairs().
--   5. UpdateRank: sort(tbNewRank, %_pCompare) -> bo dau % (upvalue Lua 4).
--      _pCompare la local khai bao TRUOC ham nen van thay duoc.
--   6. DebugMsg(...): bo "..." vi than ham khong dung toi (chi la ham go roi).
--   7. DebugMsg: "for szName, tbData in self.tbScores do" -> them pairs().
--   8. THEM (khong co o ban Linux): chan so vong lap trong LoadValue, phong ban
--      ghi ShareData hong lam treo luong chinh cua relay (RelayRpc_Tick chay
--      tren WM_TIMER 100 ms).
--   9. THEM (khong co o ban Linux): s_AddScore kiem szName phai la chuoi va
--      nScore phai la so truoc khi ghi, tranh "table index is nil" khi goi toi hong.
--      Khong doi hop dong: s_AddScore von khong day gi vao hResult.
--   Giu nguyen date("%W") - shim cap date = os.date, %W la dinh dang C89, MSVC co.
--   Khong co cho nao gan lai bien dieu khien for (da soi tung vong).
--
-- CHO DAT: phai la  <thu muc chay relay>\script\battle\weeklyrank.lua  ('battle'
--   SO IT - dung duong dan GameServer goi).
--   [SOAT 06/09] Ly do cu ghi o day la SAI va da bo: RelayScript_Init() chi dang ky
--   bang ham, KHONG nap san tep .lua nao; moi tep nap luoi qua RelayScript_Get.
--   Thu tu that (Global.cpp): RelayScript_Init -> AddFuncTable -> RelayRpc_Init ->
--   ShareData_Init -> TaskCentre_Init, nen khi tep nay duoc nap thi ShareData da mo.
--
-- LUU Y: tep .utf8.lua nay la BAN DUNG TAM, ma UTF-8. Ban chinh se chuyen chu
--   Viet sang TCVN3 sau.
--------------------------------------------------------------------------------

Include("\\script\\lib\\objbuffer_head.lua")

tbSJ_WeeklyRank = {}
tbSJ_WeeklyRank.SAVE_KEY = "FUNC_SJ_WEEKLY_RANK"

tbSJ_WeeklyRank.tbRank = {}
tbSJ_WeeklyRank.nRankDate = 0
tbSJ_WeeklyRank.tbScores = {}
tbSJ_WeeklyRank.nPlayerCount = 0

-- Mot ban ghi lanh lan nhieu nhat chi co 3 o (ten, diem, co). Lay 64 cho rong.
local MAX_POP_LOOP = 64

function tbSJ_WeeklyRank:SaveValue(nKey1, nKey2, ...)
	local arg = {n = select("#", ...), ...}
	local nArgs = getn(arg)
	if nArgs <= 0 then
		return
	end

	local handle = OB_Create()
	for i=1,nArgs do
		ObjBuffer:PushObject(handle, arg[i])
	end
	OB_SaveShareData(handle, self.SAVE_KEY, nKey1, nKey2)
	OB_Release(handle)
end

function tbSJ_WeeklyRank:LoadValue(nKey1, nKey2)
	local handle = OB_Create()
	local tbReturn = {}
	OB_LoadShareData(handle, self.SAVE_KEY, nKey1, nKey2)

	local nLoop = 0
	while OB_IsEmpty(handle) ~= 1 do
		nLoop = nLoop + 1
		if nLoop > MAX_POP_LOOP then
			OutputMsg(format("[WeeklyRank] ban ghi hong [%s] (%d,%d): doc qua %d o, bo do",
				self.SAVE_KEY, nKey1, nKey2, MAX_POP_LOOP))
			break
		end
		tinsert(tbReturn, ObjBuffer:PopObject(handle))
	end

	OB_Release(handle)
	return unpack(tbReturn, 1, getn(tbReturn))
end

function tbSJ_WeeklyRank:ClearScore()
	self.tbScores = {}
	self.nPlayerCount = 0
	self:SaveValue(0, 0, 0)
end

function tbSJ_WeeklyRank:GetRankWeek()
	return tonumber(date("%W"))
end

local _pCompare = function(tbA, tbB)
	-- [SOAT 06/09] ban ghi ShareData hong co the cho nScore = nil -> so sanh
	-- so voi nil la loi thoi ca UpdateRank (chot tuan hong, lap lai moi lan goi).
	return (tbA.nScore or 0) > (tbB.nScore or 0)
end

function tbSJ_WeeklyRank:UpdateRank()

	local tbNewRank = {}
	local tbTemp
	for szName, tbData in pairs(self.tbScores) do
		if type(szName) == "string" then
			tbTemp = {}
			tbTemp.szName = szName
			tbTemp.nScore = tbData.nScore
			tinsert(tbNewRank, tbTemp)
		end
	end

	sort(tbNewRank, _pCompare)

	self.tbRank = {}
	for i=1,10 do
		if tbNewRank[i] ~= nil then
			tinsert(self.tbRank, {szName=tbNewRank[i].szName, nScore=tbNewRank[i].nScore, nFlag=0})
			self:SaveValue(1, i, tbNewRank[i].szName, tbNewRank[i].nScore, 0)
		end
	end

	self.nRankDate = self:GetRankWeek()

	self:SaveValue(1, 0, getn(self.tbRank))
	self:SaveValue(2, 0, self.nRankDate)
end

function tbSJ_WeeklyRank:LoadRank()
	local nCount = self:LoadValue(1, 0) or 0
	local tbTemp
	self.tbRank = self.tbRank or {}

	for i=1,nCount do
		tbTemp = {}
		tbTemp.szName, tbTemp.nScore, tbTemp.nFlag = self:LoadValue(1, i)
		tinsert(self.tbRank, tbTemp)
	end

	self.nRankDate = self:LoadValue(2, 0) or 0;

end

function tbSJ_WeeklyRank:LoadScore()
	local nCount = self:LoadValue(0, 0) or 0
	local szName, nScore, nPos
	local tbTemp
	self.tbScores = self.tbScores or {}

	for i=1, nCount do
		szName, nScore = self:LoadValue(0, i)
		-- [SOAT 06/09] mot ban ghi (0,i) thieu/hong thi szName = nil ->
		-- "table index is nil" no NGAY trong Init(), chunk chet nhung van bi
		-- cache la "da nap" (RelayScript.cpp dat con tro truoc khi chay chunk)
		-- -> nPlayerCount ket o 0 -> nguoi ke tiep GHI DE ban ghi nguoi dau.
		if type(szName) == "string" and szName ~= "" then
			tbTemp = {}
			tbTemp.nScore = (type(nScore) == "number") and nScore or 0
			tbTemp.nPos = i
			self.tbScores[szName] = tbTemp
		else
			OutputMsg(format("[WeeklyRank] ban ghi (0,%d) hong, bo qua", i))
		end
	end

	self.nPlayerCount = nCount
end


function tbSJ_WeeklyRank:s_AddScore(hParam, hResult)
	local szName, nScore
	if OB_IsEmpty(hParam) == 1 then
		return
	end

	self:CheckUpdateRank()

	szName = ObjBuffer:PopObject(hParam)
	nScore = ObjBuffer:PopObject(hParam)

	-- chan them khi chuyen sang relay: goi hong thi bo, dung de no giua luong chinh
	if type(szName) ~= "string" or szName == "" or type(nScore) ~= "number" then
		OutputMsg("[WeeklyRank] s_AddScore: tham so hong, bo qua")
		return
	end

	if not self.tbScores[szName] then
		self.nPlayerCount = self.nPlayerCount + 1
		self.tbScores[szName] = {nScore=0, nPos = self.nPlayerCount}
		self:SaveValue(0, 0, self.nPlayerCount)
	end

	-- [SOAT 06/09] khong chi kiem tham so DEN, phai kiem ca gia tri CU: ban ghi
	-- da ton tai nhung nScore = nil (doc len tu ShareData hong) van lam no.
	if type(self.tbScores[szName].nScore) ~= "number" then
		self.tbScores[szName].nScore = 0
	end
	self.tbScores[szName].nScore = self.tbScores[szName].nScore + nScore
	self:SaveValue(0, self.tbScores[szName].nPos, szName, self.tbScores[szName].nScore)
end

function tbSJ_WeeklyRank:s_GetRank(hParam, hResult)
	if OB_IsEmpty(hParam) == 1 then
		return
	end

	self:CheckUpdateRank()

	local szName = ObjBuffer:PopObject(hParam)
	ObjBuffer:PushObject(hResult, szName)

	local nCount = getn(self.tbRank)
	ObjBuffer:PushObject(hResult, nCount)

	for i=1, nCount do
		ObjBuffer:PushObject(hResult, self.tbRank[i].szName)
		ObjBuffer:PushObject(hResult, self.tbRank[i].nScore)
	end
end

function tbSJ_WeeklyRank:s_GetRankAward(hParam, hResult)
	if OB_IsEmpty(hParam) == 1 then
		return
	end

	self:CheckUpdateRank()

	local szName = ObjBuffer:PopObject(hParam)
	local nFound = nil

	for i=1,getn(self.tbRank) do
		if (self.tbRank[i].szName == szName) then
			if self.tbRank[i].nFlag == 0 then
				nFound = i
				self.tbRank[i].nFlag = 1
				self:SaveValue(1, i, self.tbRank[i].szName, self.tbRank[i].nScore, self.tbRank[i].nFlag)
			else
				nFound = 0
			end
			break
		end
	end

	ObjBuffer:PushObject(hResult, szName)
	ObjBuffer:PushObject(hResult, nFound)
end

function tbSJ_WeeklyRank:CheckUpdateRank()
	local nWeek = tonumber(date("%W"))

	if nWeek == self.nRankDate or nWeek == 0 then
		return
	end

	self:UpdateRank()
	self:ClearScore()

end

function tbSJ_WeeklyRank:Init()
	self:LoadRank()
	self:LoadScore()
end

function tbSJ_WeeklyRank:DebugMsg(...)	-- [SOAT 06/09] giu "..." nhu ban goc
	local szMsg

	OutputMsg("---------------------------------------------------------------------------------------")

	OutputMsg(format("Rank: %d", self.nRankDate))
	for i=1,getn(self.tbRank) do
		szMsg = format("%2d:  %s  %d", i, self.tbRank[i].szName, self.tbRank[i].nScore)
		OutputMsg(szMsg)
	end

	OutputMsg(format("Score: %d", self.nPlayerCount))
	for szName, tbData in pairs(self.tbScores) do
		if type(szName) == "string" then
			szMsg = format("%s  %s  %d", szName, tbData.nScore, tbData.nPos)
			OutputMsg(szMsg)
		end
	end
	OutputMsg("---------------------------------------------------------------------------------------")
end


tbSJ_WeeklyRank:Init()
