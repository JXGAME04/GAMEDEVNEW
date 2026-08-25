Include("\\script\\activitysys\\config\\32\\dailytask.lua")

tbTalkDailyTask = tbDailyTask:new()

tbTalkDailyTask.TSK_DAILY_TASK_COMPLETE_STATE = TSK_DAILY_TASK_COMPLETE_COUNT;
tbTalkDailyTask.nStateBit = 3;
tbTalkDailyTask.szConfigPath = {"\\settings\\task\\dailytask\\talk.txt" ,"\\settings\\task\\dailytask\\talk_pos.txt"};
tbTalkDailyTask.szTaskName = "Nhiệm vụ Đối thoại ";
tbTalkDailyTask.tbNpcPos = {}
tbTalkDailyTask.TSK_TALK_TASK_ID = TSK_TALK_TASK_ID;
tbTalkDailyTask.TSK_TALK_TASK_STATE = TSK_TALK_TASK_STATE;

function tbTalkDailyTask:Init()
	local nRet = self:_init();
	if not nRet then
		return nil;
	end
	
	return nRet;
end

function tbTalkDailyTask:AddTalkNpc(nMapTemplate, nMapId)
	if (self.tbNpcPos[nMapTemplate] ~= nil) then
		local nRandIdx = random(1,getn(self.tbNpcPos[nMapTemplate]));
		local tbPos = self.tbNpcPos[nMapTemplate][nRandIdx];
		if (tbPos ~= nil) then
			local nNpcIdx = AddNpc(tbPos.nNpcRes, 1, SubWorldID2Idx(nMapId), tbPos.nX * 32, tbPos.nY * 32, 0, tbPos.szNpcName);
			if (nNpcIdx ~= nil and nNpcIdx > 0) then
				SetNpcScript(nNpcIdx, tbPos.szNpcScript);
				--SetNpcParam(nNpcIdx, 1, tbPos.nTaskId);
			end
		end
	end

end

function tbTalkDailyTask:LoadDataFromConfig()
	local nTotalRow = TabFile_GetRowCount(self.szConfigPath[1])
	self.tbTaskList = {}
	for nRow = 2, nTotalRow do
		local tbTask = {}
		tbTask.nTaskId = tonumber(TabFile_GetCell(self.szConfigPath[1], nRow, "TaskId"));
		tbTask.szTaskName = TabFile_GetCell(self.szConfigPath[1], nRow, "TaskName");
		tbTask.szMapName = TabFile_GetCell(self.szConfigPath[1], nRow, "MapName");
		tbTask.szNpcName = TabFile_GetCell(self.szConfigPath[1], nRow, "NpcName");
		tbTask.nSex = tonumber(TabFile_GetCell(self.szConfigPath[1], nRow, "NpcGender"));
		tinsert(self.tbTaskList, tbTask)
	end
	
	nTotalRow = TabFile_GetRowCount(self.szConfigPath[2])
	self.tbNpcPos = {}
	for nRow = 2, nTotalRow do
		local tbPos = {}	
		tbPos.nMapId = tonumber(TabFile_GetCell(self.szConfigPath[2], nRow, "MapId"));
		tbPos.nX = tonumber(TabFile_GetCell(self.szConfigPath[2], nRow, "X"));
		tbPos.nY = tonumber(TabFile_GetCell(self.szConfigPath[2], nRow, "Y"));
		tbPos.nNpcRes = tonumber(TabFile_GetCell(self.szConfigPath[2], nRow, "NpcRes"));
		tbPos.szNpcName = TabFile_GetCell(self.szConfigPath[2], nRow, "NpcName");
		tbPos.szNpcScript = TabFile_GetCell(self.szConfigPath[2], nRow, "NpcScript");
		tbPos.nTaskId = tonumber(TabFile_GetCell(self.szConfigPath[2], nRow, "TaskId"));
		if not self.tbNpcPos[tbPos.nMapId] then
			self.tbNpcPos[tbPos.nMapId] = {}
		end
		
		tinsert(self.tbNpcPos[tbPos.nMapId], tbPos)
	end
	return 1;
end

function tbTalkDailyTask:CheckCanAccept()
	if (self:_CheckCanAccept() ~= 1) then
		Talk(1,"",format("Ông ta đã noận được thư của ta rồi, đa tạ %s ngươi giúp đỡ.",GetSex() == 1 and "Chị " or "Ca Ca"))
		return nil;
	end
	return 1;
end

function tbTalkDailyTask:AcceptTask()
	if (self:CheckCanAccept() ~= 1) then
		return nil;
	end
	
	if (self:IsHaveTask() == 1) then
		local tbTask = self:GetTaskData(GetTask(self.TSK_TALK_TASK_ID));
		if (tbTask ~= nil) then
			Talk(1,"",format("Vị này %s, Ngươi giúp ta mang thư gửi cho<color=yellow>%s<color> chưa? Ngươi nhanh chóng đi<color=red>%s<color>tìm%sđi, bức thư này của ta rất gấp đấy.", GetSex() == 1 and "Chị " or "Ca Ca", tbTask.szNpcName, tbTask.szMapName, tbTask.nSex == 1 and "Cụ " or "Anh "));
		end
		return nil;
	end
	
	if (PlayerFunLib:CheckFreeBagCell(1, "default") ~= 1) then
		return nil;
	end
	
	local nTaskCount = getn(self.tbTaskList)
	
	local nId = random(1,nTaskCount)
	local tbTask = self.tbTaskList[nId]
	if tbTask == nil then
		return nil;
	end
	
	SetTask(self.TSK_TALK_TASK_ID, nId);
	SetTask(self.TSK_TALK_TASK_STATE, 0);
	
	Talk(1,"",format("Vị này %s, ngươi có quen<color=yellow>%s<color> của <color=red>%s<color> không? Đã lâu lắm rồi ta chưa gặp %s rồi, ngươi có thể giúp ta mang thư đi không? %sđang ở<color=red>%s<color>.", GetSex() == 1 and "Chị " or "Ca Ca", tbTask.szMapName, tbTask.szNpcName, tbTask.nSex == 1 and "Cụ " or "Anh ", tbTask.nSex == 1 and "Cụ " or "Anh ", tbTask.szMapName));

	tbAwardTemplet:GiveAwardByList({szName="Thư của Long Nhi",tbProp={6,1,2571,1,0,0}}, "Tiếp nhận nhiệm vụ đối thoại nhận được thư của Long Nhi");

	WriteLog(format("[%s]\t%s\t%s\t%s","Nhiệm vụ hàng ngày",GetName(), GetAccount(),format("Nhận nhiệm vụ đối thoại: đối thoại với%s",tbTask.szNpcName)))

	return 1;
end

function tbTalkDailyTask:IsHaveTask()
	if GetTask(self.TSK_TALK_TASK_ID) <= 0 then
		return nil;
	else
		return 1;
	end
end

function tbTalkDailyTask:CancelTask()
	SetTask(self.TSK_TALK_TASK_ID, 0)
	SetTask(self.TSK_TALK_TASK_STATE, 0)
	return 1;
end

function tbTalkDailyTask:CheckCompleteTask()
	local nTskId = GetTask(self.TSK_TALK_TASK_ID);
	local nState = GetTask(self.TSK_TALK_TASK_STATE);
	local tbTask = self:GetTaskData(nTskId);
	
	if (self:IsHaveTask() ~= 1 or tbTask == nil) then
		Talk(1,"",format("Vị này %s, tìm ta có chuyện gì không?", GetSex() == 1 and "Chị " or "Ca Ca"));
		return nil;
	end
	
	if (nState ~= 1) then
		Talk(1,"",format("Vị này %s,ngươi vẫn chưa giúp ta mang thư cho<color=yellow>%s<color>. Ngươi nhanh nhanh đi<color=red>%s<color> tìm%s đi, bức thư này của ta rất gấp", GetSex() == 1 and "Chị " or "Ca Ca", tbTask.szNpcName, tbTask.szMapName, tbTask.nSex == 1 and "Cụ " or "Anh "));
		return nil;
	end
	
	return 1;
end

function tbTalkDailyTask:CompleteTask()
	if (self:CheckCompleteTask() ~= 1) then
		return nil;
	end
	
	local tbTask = self:GetTaskData(GetTask(self.TSK_TALK_TASK_ID));
	
	SetTask(self.TSK_TALK_TASK_ID, 0)
	SetTask(self.TSK_TALK_TASK_STATE, 0)
	self:_CompleteTask();
	
	WriteLog(format("[%s]\t%s\t%s\t%s","Nhiệm vụ hàng ngày",GetName(), GetAccount(),format("Nhận nhiệm vụ đối thoại: đối thoại với%s",tbTask.szNpcName)))
	AddStatData("richangrenwu_songxincishu")
	return 1;
end

function tbTalkDailyTask:TalkToNpc()
	local szNpc = GetNpcName(GetLastDiagNpc());
	local nTskId = GetTask(self.TSK_TALK_TASK_ID);
	local nState = GetTask(self.TSK_TALK_TASK_STATE);
	local tbTask = self:GetTaskData(nTskId);
	
	if (tbTask == nil or tbTask.szNpcName ~= szNpc or nState == 1) then
		Talk(1,"",format("Đại hiệp ngươi có nhầm người không."));
		return nil;
	end
	
	if (PlayerFunLib:CheckItemInBag({tbProp={6,1,2571,-1,-1,0}}, 1, "Đại hiệp thư của ngươi đâu?") ~= 1) then
		return nil;
	end
	
	PlayerFunLib:ConsumeEquiproomItem({tbProp={6,1,2571,-1,-1,0}}, 1)

	SetTask(self.TSK_TALK_TASK_STATE, 1);
	
	Talk(1,"",format("Đa tạ đại hiệp."));
	return 1;
end
