-- SINH TU DONG [BDH-G5] bdh_compat.lua - lop tuong thich JX1 cho script partner Linux
-- (Msg2player viet thuong + he AddPlayerEvent ma JX1 khong co trong engine)
Msg2player = Msg2Player

IncludeLib("FILESYS")
TabFile_Load("\\settings\\npc\\player\\event_killnpc.txt", "bdh_killevent")

-- he dem giet quai lam bang hook danhquai.lua (BDH_OnKillNpc trong bdh_killhook.lua)
function AddPlayerEvent(nEventId)
	return 1
end
function RemovePlayerEvent(nEventId)
	SetTask(1236, 0)
	return 1
end
