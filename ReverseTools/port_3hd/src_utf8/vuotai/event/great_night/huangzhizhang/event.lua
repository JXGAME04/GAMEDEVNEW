if not GREATNIGHT_HUANGZHIZHANG  then
GREATNIGHT_HUANGZHIZHANG = 1

Include("\\script\\lib\\gb_taskfuncs.lua")
GN_EVENTNAME = "Hoạt động Hoàng chi chương"
TB_HUANGZHIZHANG_EVENT = {
		"Chiến trường Tống Kim",
		"Nhiệm vụ Tín Sứ ",
		"Thách thức thời gian",
		"Chuỗi nhiệm vụ Dã Tẩu",
		--"同伴剧情任务修炼篇，同伴修炼任务",
	}
GN_HUANG_EVENTID = 1
GN_HUANG_SWITH = 2
GN_HUANG_DATE = 3
function greatnight_huang_event(event)
	--closed
--	--活动还没开始
--	if (gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID) == 0) then
--		return 1
--	end
	local award_times = gn_getaward_times()	--当天活动奖励翻倍倍数
--	--礼官查询当晚活动
--	if (event == nil) then
--		--非1表示返回3个参数： 0，活动ID，奖励翻倍倍数
--		return 0, gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID), award_times
--	end
--
--	--活动还未开始
--	if (gb_GetTask(GN_EVENTNAME, GN_HUANG_SWITH) == 0) then
--		return 1
--	end
--	open it for x2 TK -by Thanhld 20150120
	if (gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID) == event) then
		return award_times
	end
	
	return 1
end

function gn_getaward_times()
--	local nWeek = tonumber(GetLocalDate("%w"))
--	if (nWeek == 6 or nWeek == 0) then
--		return 3	--周末3倍
--	else
		return 2	--平时2倍
--	end
end

function huang_event_hint()
	--closed
--	local swith, eventid, awardtimes = greatnight_huang_event()
--	if (swith == 1) then
--		return format("%s%s%s%s%s",
--				"煌之章活动的活动时间为每晚<color=yellow>9点整至11点整<color>。此时间段内，大侠可以获得在某个活动中奖励翻倍的好处。<enter>目前奖励翻倍的活动包括：<enter>",
--				"宋金大战<enter>",
--				"闯关任务<enter>",
--				"野叟任务链<enter>",
--				"至于具体在当天是哪个活动翻倍，请在每晚<color=yellow>8点<color>后礼官处查询。请各位尽情享受独孤盟主的馈赠。")
--		
--	else
--		return format("煌之章活动的时间为每晚<color=yellow>9点整至11点整<color>。今晚活动是：<enter><color=yellow>%s<color><enter><enter>请各位尽情享受独孤盟主的馈赠。",
--					TB_HUANGZHIZHANG_EVENT[eventid])
--	end
	return ""
end

function huang_event_name()
	local swith, eventid, awardtimes = greatnight_huang_event()
	if (swith == 1) then
		return "Chưa biết"
	else
		return TB_HUANGZHIZHANG_EVENT[eventid]
	end
end

function huang_event_msg()
	local name = huang_event_name()
	return format("Hoạt động Hoàng chi chương sẽ diễn ra lúc 21h00 đêm nay, hoạt động nhân đôi phần thưởng liên quan là: <color=yellow>%s<color>",name)
end

function seed_birth_city()
	local MapStr = "Đông môn Dương Châu";
	if (1 == gb_GetTask("hạt Huy Hoàng", 1)) then
		MapStr = "Bắc môn Đại Lý ";
	end;
	return format("12h00 trưa nay sẽ bắt đầu hoạt động Huy chi chương, quả Hoàng Kim sẽ xuất hiện tại <color=yellow>%s<color>",MapStr)
end

if EVENTS_TB then 
EVENTS_TB[getn(EVENTS_TB)+1] = {	--辉之章
			name = "Huy Hoàng",
			describe = seed_birth_city
		}
--EVENTS_TB[getn(EVENTS_TB)+1] = {	--煌之章
--			name = "煌之章",
--			describe = huang_event_msg
--		}
end
end