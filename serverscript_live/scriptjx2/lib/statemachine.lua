----------------------------------------------------------------------------------------------------
--一个简单的状态机
--状态列表：
--tbStateList = {
--	"StateName1" = pStateObj1,
--	"StateName2" = pStateObj2,
--	...
--}

--状态映射表设计：
--tbMapping = {
--	"StateName" = {"szEvent1"="TargetStateName1", "szEvent2"="TargetStateName2"},
--	...
--}

Include("\\script\\activitysys\\functionlib.lua")

local StateMachine = {}
local StateBase = {}

--创建一个新的状态机
function NewStateMachine()
	local tb = lib:CopyTB1(StateMachine)
	tb.tbStateList = {}
	tb.tbMapping = {}
	return tb
end

function StateBase:New()
	local tb = lib:CopyTB1(self)
	tb.New = nil
	return tb
end

--状态激活时执行
--执行时，当前状态已经切换成自己。
function StateBase:OnStart()

end

--状态结束时执行
--执行时，当前状态还是自己。
function StateBase:OnEnd()

end

--释放状态机时触发
function StateBase:OnRelease()
	
end

--创建一个新的状态
--参数szStateName: 状态名, 必须当前状态机唯一，否则失败
--返回值：状态对象(table)
function StateMachine:NewState(szStateName)
	if self:GetState(szStateName) then
		print("[ERROR][StateMachine:NewState]", format("State:[%s] already exists.", szStateName))
		return
	end
	
	local tb = StateBase:New()
	if not tb then
		return
	end
	
	tb._nPackId = curpack()
	tb.StateMachine = self
	tb.szName = szStateName
	self:SetState(szStateName, tb)
	return tb
end

--状态机事件处理
--状态机事件是切换状态的唯一方式
--根据当前状态和传入的szEvent，查找状态映射表，决定目标状态
--目前不支持参数
function StateMachine:OnStateEvent(szEvent)
	local curState = self.CurState
	if not curState then
		return
	end
	
	local targetState = self:GetMappedState(curState.szName, szEvent)
	if not targetState then
		return
	end
	local nCurPack = usepack(curState._nPackId)
	curState:OnEnd()
	self.CurState = targetState
	
	usepack(targetState._nPackId)
	targetState:OnStart()
	
	usepack(nCurPack)
end

function StateMachine:SetStartState(szState)
	self.szStartState = szState
end

function StateMachine:SetFinalState(szState)
	self.szFinalState = szState
end

--让状态机开始工作
--参数szInitState: 初始状态
--设置当前状态为初始状态，如果没有则返回nil，如果当前状态已存在，返回nil
--否则返回1(成功)
function StateMachine:Start(szInitState)
	if self.CurState then
		return
	end
	
	if szInitState == nil then
		szInitState = self.szStartState
		if szInitState == nil then
			return
		end
	end
	
	local pInitState = self:GetState(szInitState)
	if not pInitState then
		return
	end
	
	self.CurState = pInitState
	
	local nCurPack = usepack(pInitState._nPackId)
	self.CurState:OnStart()
	usepack(nCurPack)
	return 1
end

--设置状态映射表
--在szSrcState状态下接收到szEvent事件时，切换状态到szDstState
function StateMachine:AddStateMapping(szSrcState, szEvent, szDstState)
	local tbStateMap = self.tbMapping[szSrcState]
	if not tbStateMap then
		tbStateMap = {}
		self.tbMapping[szSrcState] = tbStateMap
	end
	
	tbStateMap[szEvent] = szDstState
end

--返回当前状态
function StateMachine:GetCurrentState()
	return self.CurState
end

----------------------------------------------------------------------------------------------------
--以下的函数外部最好不要调用

function StateMachine:SetState(szStateName, state)
	self.tbStateList[szStateName] = state
end

function StateMachine:GetState(szStateName)
	return self.tbStateList[szStateName]
end

function StateMachine:Release()
	if self.CurState then
		self.CurState:OnEnd()
		self.CurState = nil
	end

	for k, v in pairs(self.tbStateList) do
		v:OnRelease()
		v.StateMachine = nil
	end
	
	if self.OnRelease then
		self:OnRelease()
	end
end

function StateMachine:GetMappedState(szCurState, szEvent)
	local tbStateMap = self.tbMapping[szCurState]
	if (not tbStateMap) or (not tbStateMap[szEvent]) then
		return
	end
	return self:GetState(tbStateMap[szEvent])
end

----------------------------------------------------------------------------------------------------
local TemplateStateMachine = NewStateMachine()

function TemplateStateMachine:OnInstance()
	return 1
end

function TemplateStateMachine:NewInstance(...) local arg = {n = select("#", ...), ...};
	local tb = lib:CopyTB1(self)
	
	local StateInstances = {}
	for k, v in pairs(self.tbStateList) do
		local state = v:Clone()
		state.StateMachine = tb
		StateInstances[k] = state
	end
	tb.tbStateList = StateInstances
	
	if tb:OnInstance(unpack(arg)) == 1 then
		return tb
	end
end

function NewTemplateStateMachine()
	local tb = lib:CopyTB1(TemplateStateMachine)
	tb.tbStateList = {}
	tb.tbMapping = {}
	return tb
end
