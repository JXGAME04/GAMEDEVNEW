--------------------------------------------------------------------------------
-- weeklyrank.lua  (S3Relay)  [RELAYHT 06/09]
--
-- Nguồn: D:\ServerLinux\gateway\s3relay\script\battle\weeklyrank.lua  (bản Linux)
-- Đối chiếu hợp đồng với: E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server
--                         \script\battles\weeklyrank.lua  (phía GameServer)
-- Ngày port: 06/09/2026.
--
-- CÔNG DỤNG
--   Xếp hạng Tống Kim theo TUẦN, lưu bền vững trên ShareData của relay (MySQL),
--   khóa "FUNC_SJ_WEEKLY_RANK". Ba nhóm bản ghi:
--       (KEY, 0, 0)  = số người có điểm         (KEY, 0, i) = tên + điểm thô
--       (KEY, 1, 0)  = số hàng trong bảng top   (KEY, 1, i) = tên + điểm + cờ đã nhận
--       (KEY, 2, 0)  = số tuần đã chốt bảng (date("%W"))
--   GameServer cộng điểm từng người (task 751) lên đây; relay tích lũy, tự chốt
--   bảng top 10 khi sang tuần mới, trả top 10 để hiển thị và cấp cờ "đã nhận thưởng".
--
-- HỢP ĐỒNG RPC — GIỮ NGUYÊN, SAI MỘT Ô LÀ HỎNG
--   Relay được gọi bằng lua_dostring "tbSJ_WeeklyRank:s_XXX(hParam,hRes,dwGameSvrId)"
--   (RelayRpc.cpp) nên mỗi hàm s_* nhận (self, hParam, hResult) và bỏ qua tham số 3.
--     s_AddScore     hParam: [1] szName, [2] nScore.  hResult: KHÔNG đẩy gì.
--     s_GetRank      hParam: [1] szName.
--                    hResult: [1] szName, [2] nCount, rồi nCount cặp (szName, nScore).
--     s_GetRankAward hParam: [1] szName.
--                    hResult: [1] szName, [2] nFound (1..10 = lần đầu nhận,
--                             0 = đã nhận rồi, nil = không có trong bảng).
--
-- ĐÃ SỬA SO VỚI BẢN LINUX (chuyển cú pháp Lua 4 -> Lua 5.4)
--   1. Bỏ hẳn Include("\\script\\lib\\common.lua"): cây relay JX1 không có tệp đó.
--      Bản Linux chỉ mượn nó để lấy unpack; shim lua4compat đã cấp sẵn unpack.
--   2. SaveValue(...): Lua 5.4 không tự tạo bảng "arg" -> thêm dòng đầu thân hàm
--      local arg = {n = select("#", ...), ...}  (dùng n = select để getn đúng cả
--      khi có tham số nil).
--   3. LoadValue: return unpack(tbReturn) -> unpack(tbReturn, 1, getn(tbReturn)).
--      [SOAT 06/09] Lua 4.0.1 KHÔNG có unpack sẵn; bản Linux tự viết một hàm đệ quy
--      trong script\lib\common.lua. Chỉ rõ cận trên để kết quả xác định, không phụ
--      thuộc chỗ nào trong bảng có lỗ.
--   4. UpdateRank: "for szName, tbData in self.tbScores do" -> thêm pairs().
--   5. UpdateRank: sort(tbNewRank, %_pCompare) -> bỏ dấu % (upvalue Lua 4).
--      _pCompare là local khai báo TRƯỚC hàm nên vẫn thấy được.
--   6. DebugMsg(...): bỏ "..." vì thân hàm không dùng tới (chỉ là hàm gỡ rối).
--   7. DebugMsg: "for szName, tbData in self.tbScores do" -> thêm pairs().
--   8. THÊM (không có ở bản Linux): chặn số vòng lặp trong LoadValue, phòng bản
--      ghi ShareData hỏng làm treo luồng chính của relay (RelayRpc_Tick chạy
--      trên WM_TIMER 100 ms).
--   9. THÊM (không có ở bản Linux): s_AddScore kiểm szName phải là chuỗi và
--      nScore phải là số trước khi ghi, tránh "table index is nil" khi gói tới hỏng.
--      Không đổi hợp đồng: s_AddScore vốn không đẩy gì vào hResult.
--   Giữ nguyên date("%W") — shim cấp date = os.date, %W là định dạng C89, MSVC có.
--   Không có chỗ nào gán lại biến điều khiển for (đã soi từng vòng).
--
-- CHỖ ĐẶT: phải là  <thư mục chạy relay>\script\battle\weeklyrank.lua  ('battle'
--   SỐ ÍT — đúng đường dẫn GameServer gọi).
--   [SOAT 06/09] Lý do cũ ghi ở đây là SAI và đã bỏ: RelayScript_Init() chỉ đăng ký
--   bảng hàm, KHÔNG nạp sẵn tệp .lua nào; mọi tệp nạp lười qua RelayScript_Get.
--   Thứ tự thật (Global.cpp): RelayScript_Init -> AddFuncTable -> RelayRpc_Init ->
--   ShareData_Init -> TaskCentre_Init, nên khi tệp này được nạp thì ShareData đã mở.
--
-- LƯU Ý: tệp .utf8.lua này là BẢN DÙNG TẠM, mã UTF-8. Bản chính sẽ chuyển chữ
--   Việt sang TCVN3 sau.
--------------------------------------------------------------------------------

Include("\\script\\lib\\objbuffer_head.lua")

tbSJ_WeeklyRank = {}
tbSJ_WeeklyRank.SAVE_KEY = "FUNC_SJ_WEEKLY_RANK"

tbSJ_WeeklyRank.tbRank = {}
tbSJ_WeeklyRank.nRankDate = 0
tbSJ_WeeklyRank.tbScores = {}
tbSJ_WeeklyRank.nPlayerCount = 0

-- Một bản ghi lành lặn nhiều nhất chỉ có 3 ô (tên, điểm, cờ). Lấy 64 cho rộng.
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

	-- chặn thêm khi chuyển sang relay: gói hỏng thì bỏ, đừng để nổ giữa luồng chính
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
