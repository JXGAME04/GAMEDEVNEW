-- SINH TU DONG [BDH 27/08] - COMPAT TASKSYS toi thieu cho he Ban Dong Hanh.
--
-- BAN GOC VNG (region "vn") TAT he nhiem vu ngau nhien dong hanh:
--   D:\ServerLinux\server1\script\task\system\random.lua:39
--   if (szRegion == "vn") then do return end   -- "Viet Nam khong co nhiem vu ngau nhien"
-- nen tren server VN goc, cac bien 1301..1309 mai bang 0 va nhanh nhiem vu
-- trong partner_talk.lua khong bao gio kich hoat.
--
-- Compat nay giu DUNG hanh vi VN do: dinh nghia du ham khung de
-- partner_talk.lua (doi thoai tam su theo do than) chay tron ven ma khong can
-- C-API TASKSYS (TaskName/TaskNo/... tren Linux la lib C, JX1 chi co stub).
-- TASKSYS day du (nhiem vu ngau nhien + Mat do 1304 + bang F12) = GIAI DOAN 2,
-- xem BANGIAO_BANDONGHANH_2708.md muc 11.

-- bien task (dung so ban goc: task\random\task_head.lua:50-66)
ID_RANDOMTASK      = 1301
ID_RANDOMTASK_WAIT = 1302
ID_RANDOMSTATE     = 1303
ID_RANDOMBOOK      = 1304
ID_RANDOMBOOKDATE  = 1305
ID_RANDOMBOOKNUM   = 1306
ID_RANDOMBOOKTEMP  = 1307
ID_RANDOMTASKDATE  = 1308
ID_RANDOMTASKNUM   = 1309

-- trang thai nhiem vu ngau nhien (THAT - luu o 1303 nhu ban goc)
function setRandomTaskState(nState)
	SetTask(ID_RANDOMSTATE, nState)
end

function getRandomTaskState()
	return GetTask(ID_RANDOMSTATE)
end

-- khung TASKSYS: tra "khong co nhiem vu" (region vn goc khong bao gio tao)
function TaskName(nId)
	return ""
end

function TaskNo(szName)
	return 0
end

function GetTaskStatus(szName)
	return 0
end

function SetTaskStatus(szName, nState)
	return 0
end

function CloseTask(szName)
	return 1
end

function ApplyTask(szName)
	return 0
end

function GetTaskText(szName)
	return ""
end

function checkTaskBookEnable()
	return 0
end

function checkRandomTaskEnable()
	return 0
end

function initRandomTask()
	return 0
end

function _confirmRandomTask()
	return
end

function TaskTip(szText)
	if (szText ~= nil) then
		Msg2Player(szText)
	end
end

function SyncTaskValue(nId)
	-- JX1: SetTask tu dong bo moi id >= 256 xuong client (UI_TASKVALUE)
	return
end

function WriteTaskLog(szText)
	return
end
