Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\lib\\lib_server.lua")

function main(sel)
	local nTaskValue = GetTask(TASK_RESET5)
	local nTask = GetNumber(1,nTaskValue,8)
	if (nTask >= 4) then
		Msg2Player("MÁi ngµy chÿ c„ th” nh∆t Æ≠Óc 4 tÛi")
	return end
	local nNpcValue = GetNpcValue(sel)
	if (nNpcValue == 1) then
	return end
	-- DoProcess(5*18, "Pickup("..sel..")")
	PaceBar("ßang thu hoπch...", 5, "Pickup("..sel..")")
end

function Pickup(sel)
	-- if(IsProgressSuccess() == 0) then
		-- Msg2Player("Thu thÀp bﬁ gi∏n Æoπn!")
	-- return end
	local nTaskValue = GetTask(TASK_RESET5)
	local nTask = GetNumber(1,nTaskValue,8)
	if (nTask >= 4) then
		Msg2Player("MÁi ngµy chÿ c„ th” nh∆t Æ≠Óc 4 r≠¨ng")
	return end
	local nNpcValue = GetNpcValue(sel)
	if (nNpcValue == 1) then
	return end
	SetNpcValue(sel,1)
	DelNpc(sel)
	local nExp = GetLevel()^3*2
	AddSumExp(nExp)
	local randomT = random(5,30);
	for i = 1, randomT do 
	AddItem(6,1,4844,0,0,0,0) --- HMD
	end
	Msg2Player("Bπn nhÀn Æ≠Óc "..nExp.." Æi”m kinh nghi÷m")
	SetTask(TASK_RESET5, SetNumber(1,nTaskValue,8,nTask+1))
end

function OnTimer(sel)
	DelNpc(sel)
end
