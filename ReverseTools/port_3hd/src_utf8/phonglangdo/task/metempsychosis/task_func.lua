-- ====================== 文件信息 ======================

-- 剑侠情缘网络版越南版 - 多次转生头文件
-- 文件名　：task_func.lua
-- 创建者　：子非魚
-- 创建时间：2009-02-04 16:21:20

-- ======================================================

IncludeLib("LEAGUE");
IncludeLib("SETTING");
IncludeLib("FILESYS");
IncludeLib("ITEM");
Include("\\script\\task\\system\\task_string.lua");
Include("\\script\\task\\metempsychosis\\task_head.lua")
Include("\\script\\misc\\eventsys\\type\\player.lua")

-- 记录在第 n_transcount 次转生时 的等级 和所选抗性
function zhuansheng_set_gre(n_transcount, n_level, n_resist)
	local n_taskid = 0;
	local n_taskbyte = 0;
	
	local n_id = floor(n_transcount / 2) + mod(n_transcount, 2);
	n_taskid = TSK_ZHUANSHENG_GRE[n_id];
	
	if (mod(n_transcount, 2) == 0) then
		n_taskbyte = 3;
	else
		n_taskbyte = 1;
	end
	
	local n_taskvalue = GetTask(n_taskid);
	n_taskvalue = SetByte(n_taskvalue, n_taskbyte, n_level);
	n_taskvalue = SetByte(n_taskvalue, n_taskbyte+1, n_resist);
	SetTask(n_taskid, n_taskvalue);
	--WriteLog(format("zhuansheng_set_gre:%d,%d,%d,byte:%d,id:%d", n_transcount, n_level, n_resist,n_taskbyte, n_taskid))
	return 1;
end

-- 返回第n_transcount次转生 等级和所选抗性
function zhuansheng_get_gre(n_transcount)
	local n_taskid = 0;
	local n_taskbyte = 0;
	
	local n_id = floor(n_transcount / 2) + mod(n_transcount, 2);
	n_taskid = TSK_ZHUANSHENG_GRE[n_id];
	
	if (mod(n_transcount, 2) == 0) then
		n_taskbyte = 3;
	else
		n_taskbyte = 1;
	end
	
	local n_taskvalue = GetTask(n_taskid);
	
	return GetByte(n_taskvalue, n_taskbyte),GetByte(n_taskvalue, n_taskbyte+1)
end

-- 转生洗掉所有技能点，并增加转生获得的额外 npoint 技能点
function zhuansheng_clear_skill(nlevel, npoint)
	local nskill1 = HaveMagic(210);	--保留轻功
	local nskill2 = HaveMagic(400);	--保留“劫富济贫”
	local nallskill = RollbackSkill();	-- 清除投点技能，不返回技能点
	if (nskill1 ~= -1) then			--返回轻功
		nallskill = nallskill - nskill1;
		AddMagic(210, nskill1);
	end
	
	if (nskill2 ~= -1) then			--返回“劫富济贫”
		nallskill = nallskill - nskill2;
		AddMagic(400, nskill2);
	end
	--nallskill = nallskill - (nlevel -1)	--转生只扣掉升级时给与的技能点
	AddMagicPoint(npoint + nallskill);
end

-- 转生洗掉所有潜能点，并增加转生获得的额外 npoint 潜能点
function zhuansheng_clear_prop(nlevel, npoint, nseries)
	if (not nseries) then
		nseries = GetSeries();
	end
	
	nseries = nseries + 1;
	local Utask88 = GetTask(88)-- 将已分配潜能重置（task(88)是任务中直接奖励的力量、身法等）
	AddProp(100)			-- 为避免没有未分配潜能点可供修复的极端情况，暂时“借”给他100点

	AddStrg(TB_BASE_STRG[nseries] - GetStrg(1) + GetByte(Utask88,1));
	AddDex(TB_BASE_DEX[nseries] - GetDex(1) + GetByte(Utask88,2));
	AddVit(TB_BASE_VIT[nseries] - GetVit(1) + GetByte(Utask88,3));
	AddEng(TB_BASE_ENG[nseries] - GetEng(1) + GetByte(Utask88,4));
	local nallprop = GetProp();
	--nallprop = nallprop - (nlevel - 1) * 5;	--转生只扣掉升级时给与的潜能点
	AddProp(npoint-100);
end


-- 检查是否还有战队关系
function check_zhuansheng_league(ntype)
	local nlg_idx = LG_GetLeagueObjByRole(ntype,GetName());
	if (nlg_idx ~= nil and nlg_idx ~= 0) then
		return 1;			--有战队关系
	else
		return 0;			--无战队关系
	end
end

-- 检查当前转生次和等级是否能够再转生
function check_zhuansheng_level()
	local nlevel = GetLevel();
	local ntranscount = ST_GetTransLifeCount();
	if (ntranscount >= NTRANSLIFE_MAX) then
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[7], "Được rồi./OnCancel"});
		return 0;
	end
	
	
	if (not TB_LEVEL_REMAIN_PROP[nlevel] or not TB_LEVEL_LIMIT[ntranscount + 1]) then
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[5], "Được rồi./OnCancel"});
		return
	end
	
	if (nlevel < TB_LEVEL_LIMIT[ntranscount + 1]) then
		CreateTaskSay({"<dec><npc>"..format("Tại lần chuyển sinh thứ (%d), đẳng cấp chí ít cũng phải %d!", (ntranscount + 1), TB_LEVEL_LIMIT[ntranscount + 1]), " Kết thúc đối thoại!/OnCancel"});
		return 0;
	end
	
	return 1;
end

function OnCancel()
end

function zhuansheng_check_common()

	local ntranscount = ST_GetTransLifeCount();
	if (ntranscount ~= 0) then
		local bResult = DynamicExecuteByPlayer(PlayerIndex, "\\script\\vng_event\\20130107_den_bu_chinh_do\\head.lua", "tbZhengtuCompensate:CheckGetTransLife")
		if bResult == 1 and ntranscount == 4 then
		
		else
			if (GetTask(TSK_ZHUANSHENG_LASTTIME) + TB_TRANSTIME_LIMIT[ntranscount]*24*60*60 >= GetCurServerTime()) then
				CreateTaskSay({format(TB_TRANSLIFE_ERRORMSG[12], TB_TRANSTIME_LIMIT[ntranscount]), "Được rồi./OnCancel"});
				return 0;
			end
		end
	end

	local nTaskValue = GetTask(2885)
	local nTmpRes = floor(nTaskValue/100)
	if nTaskValue > 0 and mod(nTmpRes, 100) < 14 then
		Msg2Player("Ngươi đã nhận nhiệm vụ kỹ năng 150, xin hãy hoàn thành nhiệm vụ rồi hãy đến gặp ta")
		return  0
	end

	--type=2 好像是身上
	--type=3 应该是背包
	if (CalcItemCount(2,0,-1,-1,-1) > 0) then
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[1], "Được rồi./OnCancel"});
		return 0
	end

--	if ntranscount < 3 and (check_zhuansheng_league(LG_WLLSLEAGUE) == 1) then	--战队关系
--		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[6], "Được rồi./OnCancel"});
--		return 0
--	end
	if (GetTask(TSK_KILLER_ID) ~= 0) then	--杀手任务完成
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[8], "Được rồi./OnCancel"});
		return 0
	end
	if (GetTask(TSK_MESSENGER_FENG) ~= 0 or GetTask(TSK_MESSENGER_SHAN) ~= 0 or GetTask(TSK_MESSENGER_QIAN) ~= 0 ) then	--信使任务完成
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[9], "Được rồi./OnCancel"});
		return 0
	end
--close check quest Da Tau
--	if (GetTask(TSK_TASKLINK_STATE) ~= 3 and GetTask(TSK_TASKLINK_STATE) ~= 0) then	--野叟任务完成
--		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[10], "Được rồi./OnCancel"});
--		return 0
--	end
--	
--	if GetTask(TSK_TASKLINK_CancelTaskLevel) ~= 0 or GetTask(TSK_TASKLINK_CancelTaskExp1) ~= 0 or GetTask(TSK_TASKLINK_CancelTaskExp2) ~= 0 then
--		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[11], "Được rồi./OnCancel"});
--		return 0
--	end
	
	if (GetTask(TSK_ZHUANSHENG_FLAG) ~= 1) then							--基础篇的学习
		CreateTaskSay({TB_TRANSLIFE_ERRORMSG[4], "Được rồi./OnCancel"});
		return 0;
	end
	
	return 1
end

--特殊处理玩家的任务变量，用于修正玩家转生所使用的任务变量
--TSK_ZHUANSHENG_GRE = {2577, 2578, 2579, 2579}	-- 每次转生所选等级和抗性
--TSK_ZHUANSHENG_GRE = {2577, 2578, 2579, 4107}	--修改后，每次转生所选等级和抗性
tbTransLifeSpeProcess = {}
--tbTransLifeSpeProcess.SetGreInfo = zhuansheng_set_gre
--tbTransLifeSpeProcess.GetGreInfo = zhuansheng_get_gre

function tbTransLifeSpeProcess:PlayerLogin()
	local n_transcount = ST_GetTransLifeCount()
	local nlevelL6, nresistidL6 = self:GetGreInfo(6);
	local nlevelL7, nresistidL7 = self:GetGreInfo(7);
	print("PlayerLoginProcessSome:",nlevelL7, nresistidL7)
	if n_transcount == 7 and nlevelL7 == 0 and nresistidL7 == 0 then
		self:SetGreInfo(7, nlevelL6, nresistidL6)
	end   
end

function tbTransLifeSpeProcess:SetGreInfo(nTranLife, nParam1, nParam2)
	zhuansheng_set_gre(nTranLife, nParam1, nParam2)
end

function tbTransLifeSpeProcess:GetGreInfo(nTranLife)
	return zhuansheng_get_gre(nTranLife)
end

if login_add then login_add(PlayerLoginProcessSome, 0) end

EventSys:GetType("OnLogin"):Reg(1, tbTransLifeSpeProcess.PlayerLogin, tbTransLifeSpeProcess)