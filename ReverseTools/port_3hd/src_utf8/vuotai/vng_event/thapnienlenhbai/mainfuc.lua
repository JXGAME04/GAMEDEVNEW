Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\misc\\eventsys\\type\\npc.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\vng_event\\thapnienlenhbai\\lenhbai_def.lua")

tbThapnienLenhbai = tbThapnienLenhbai or {};
function tbThapnienLenhbai:GiveNameTaskEvent(nEventId)
	local nTransLife = ST_GetTransLifeCount()
	local szMsg = ""
	if(self.TASK_ID_MAP[nTransLife] and self.TASK_ID_MAP[nTransLife][nEventId]) then
		for i,v in self.TASK_ID_MAP[nTransLife][nEventId] do
			local nNum = tbVNG_BitTask_Lib:getBitTask(v)
			local szNameTask = self.TASK_NAME_NPC[nEventId][i]
			local szActive = tbVNG_BitTask_Lib:isMaxBitTaskValue(v) ~= 1 and "red" or "yellow"
			szMsg = szMsg..format("<color=%s>%s:%d<color>\n",szActive,szNameTask,nNum)
		end
	end
	if(self.TASK_INFOR_ABOUNT[nEventId]) then
		for i,v in self.TASK_INFOR_ABOUNT[nEventId] do
			local nNum = tbVNG_BitTask_Lib:getBitTask(v)
			local szNameTask = self.TASK_NAME_TASK[i]
			local szActive = tbVNG_BitTask_Lib:isMaxBitTaskValue(v) ~= 1 and "red" or "yellow"
			szMsg = szMsg..format("<color=%s>%s:%d<color>\n",szActive,szNameTask,nNum)
		end
	end
	return szMsg
end

--Kết thúc mỗi nhiệm vụ
function tbThapnienLenhbai:CheckFinishEvent(nEventId)
	local nTransLife = ST_GetTransLifeCount()
	--Kiểm tra id tiêu diệt quái
	if(self.TASK_ID_MAP[nTransLife] and self.TASK_ID_MAP[nTransLife][nEventId]) then
		for _,v in self.TASK_ID_MAP[nTransLife][nEventId] do
			if(tbVNG_BitTask_Lib:isMaxBitTaskValue(v) ~= 1) then
				return 0
			end
		end
	end
	--Kiểm tra id các sự kiện
	if(self.TASK_INFOR_ABOUNT[nEventId]) then
		for _,v in self.TASK_INFOR_ABOUNT[nEventId] do
			if(type(v) == "table" and tbVNG_BitTask_Lib:isMaxBitTaskValue(v) ~= 1) then
				return 0
			end
			if(type(v) == "number" and GetTask(mod(v,1000)) < floor(v/1000)) then
				return 0
			end
		end
	end
	
	return 1
end

--Hiện thị các dòng chọn game có thể nhận
function tbThapnienLenhbai:onDialogNpcGiveTask()
	local szMsg = "Thập niên lệnh bài tăng tốc độ thăng cấp lên 10000\nlần! \nHoàn thành nhiệm vụ có thể nhận thưởng:\n"
	local tbOpt = {}
	local nEventId = self:GetEventIdInfo(4)
	if(nEventId) then
		local tbAward = self:GetAwardInfo(nEventId)
		szMsg = szMsg..format("<color=yellow>%s:\n<color>",tbAward[1].szDlg)
		tinsert(tbOpt, {format("%s",tbAward[1].szSelect), self.onSelectOptGiveTask, {self,nEventId}})	
	else
		szMsg = szMsg.."<color=red>Không có nhiệm vụ để nhận, hãy luyện tập thêm<color>"		
	end
	tinsert(tbOpt, {"Hủy bỏ "})
	CreateNewSayEx(szMsg, tbOpt)
end

function tbThapnienLenhbai:onSelectOptGiveTask(nEventId)
	local tbAward = self:GetAwardInfo(nEventId)
	if(not tbAward) then
		return 0
	end
	local tbBitTSK = {nTaskID = tbAward[1].nTaskIDStart, nStartBit = tbAward[1].nBitStart, nBitCount = 2, nMaxValue = 3}
	tbVNG_BitTask_Lib:addTask(tbBitTSK,1)
	tbVNG_BitTask_Lib:addTask(self.TASK_INFOR_TN,nEventId -  tbVNG_BitTask_Lib:getBitTask(self.TASK_INFOR_TN))
	Msg2Player("Chúc mừng đại hiệp nhận được "..tbAward[1].szSelect)
	tbLog:PlayerActionLog(self.EVENT_LOG_TITLE,"NhanNhiemVu","["..tbAward[1].szSelect.."]")
end

--Hiện thị các dòng chọn game đã nhận
function tbThapnienLenhbai:onDialogNpcGiveAward()
	local szMsg = "Hoàn thành nhiệm vụ có thể nhận thưởng:\n"
	local tbOpt = {}
	local nEventId = self:GetEventIdInfo(1)
	if(nEventId) then
		local tbAward = self:GetAwardInfo(nEventId)
		szMsg = szMsg..format("<color=yellow>%s:\n<color>",tbAward[1].szDlg)
		szMsg = szMsg.."\n\n"..tbThapnienLenhbai:GiveNameTaskEvent(nEventId)
		tinsert(tbOpt, {format("Hoàn thành %s",tbAward[1].szSelect), self.onSelectOptGiveAward, {self,nEventId}})	
	else
		szMsg = szMsg.."<color=red>Không có nhiệm vụ để hoàn thành<color>"	
	end
	tinsert(tbOpt, {"Hủy bỏ "})
	CreateNewSayEx(szMsg, tbOpt)
end

--Phần thưởng hỗ trợ tân thủ
function tbThapnienLenhbai:onSelectOptGiveAward(nEventId)
	local tbAward = self:GetAwardInfo(nEventId)
	if(not tbAward) then
		return 0
	end
	if(self:CheckFinishEvent(nEventId) ~= 1) then
		Msg2Player("Đại hiệp chưa hoàn thành nhiệm vụ, không thể nhận thưởng")
		return 0
	end
	if PlayerFunLib:CheckFreeBagCell(30,"default") ~= 1 then
		return
	end
	local tbBitTSK = {nTaskID = tbAward[1].nTaskIDStart, nStartBit = tbAward[1].nBitStart, nBitCount = 2, nMaxValue = 3}
	tbVNG_BitTask_Lib:addTask(tbBitTSK,1)
	SetTask(self.TASK_ID_RESET_01,0)
	SetTask(self.TASK_ID_RESET_02,0)
	tbAwardTemplet:Give(self.tbAward_List[nEventId],1,{self.EVENT_LOG_TITLE,"Phanthuonghoanthanhnhiemvu_"..nEventId})
	tbLog:PlayerActionLog(self.EVENT_LOG_TITLE,"HoanThanhNhiemVu","["..tbAward[1].szSelect.."]")
end


--Kiểm tra hỗ trợ vượt ải
function tbThapnienLenhbai:ChuanguanAward(nBatch, tbPlayers)
	if nBatch ~= self.nCOTBatchAward then		
		return
	end
	self:Award(tbPlayers, "ToDoiHoTroTanThuVuotAi")
end
--Kiểm tra hỗ trợ vượt Viêm Đế
function tbThapnienLenhbai:YDBZAward(nBatch, tbPlayers)
	if nBatch ~= self.nYDBZBatchAward then		
		return
	end
	self:Award(tbPlayers, "ToDoiHoTroTanThuVuotViemDe")
end

function tbThapnienLenhbai:LoadFile()
	self.nMaxEventId = 0
	tbThapnienLenhbai.tbAllAwards = {}
	if TabFile_Load(self.szPathFile, self.szPathFile) == 0 then
		return 0
	end
	local nCount = TabFile_GetRowCount(self.szPathFile)
	for i = 2, nCount do
		local tbItem = {}
		local nEventId	= tonumber(TabFile_GetCell(self.szPathFile, i, "nEventId")) or 0
		tbItem.nEventId	= nEventId
		tbItem.nMinLevel	= tonumber(TabFile_GetCell(self.szPathFile, i, "nMinLevel")) or 0
		tbItem.nMaxLevel	= tonumber(TabFile_GetCell(self.szPathFile, i, "nMaxLevel")) or 0
		tbItem.nTransLife	= tonumber(TabFile_GetCell(self.szPathFile, i, "nTranslife")) or 0
		tbItem.szMapList	= TabFile_GetCell(self.szPathFile, i, "szMapList") or ""
		tbItem.szSelect	= TabFile_GetCell(self.szPathFile, i, "szSelect") or ""
		tbItem.szDlg	= TabFile_GetCell(self.szPathFile, i, "szDlg") or ""
		tbItem.nTaskIDStart	= tonumber(TabFile_GetCell(self.szPathFile, i, "nTaskIDStart")) or 0
		tbItem.nBitStart	= tonumber(TabFile_GetCell(self.szPathFile, i, "nBitStart")) or 0
		self.nMaxEventId = (self.nMaxEventId >nEventId) and self.nMaxEventId or nEventId
		self.tbAllAwards[nEventId] = self.tbAllAwards[nEventId] or {}
		tinsert(self.tbAllAwards[nEventId],tbItem)
	end
end

--Khởi tạo nhiệm vụ Thập Niên Lệnh Bài -- NgaVN 20140611
tbThapnienLenhbai:LoadFile();