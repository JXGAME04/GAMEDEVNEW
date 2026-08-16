if not GREATNIGHT_HUANGZHIZHANG  then
GREATNIGHT_HUANGZHIZHANG = 1

Include("\\script\\lib\\gb_taskfuncs.lua")
GN_EVENTNAME = "Ho¹t ®éng Hoµng chi ch­¬ng"
TB_HUANGZHIZHANG_EVENT = {
		"ChiÕn tr­êng Tèng Kim",
		"NhiÖm vô Tİn Sø ",
		"Th¸ch thøc thêi gian",
		"Chuçi nhiÖm vô D· TÈu",
		--"Í¬°é¾çÇéÈÎÎñĞŞÁ¶Æª£¬Í¬°éĞŞÁ¶ÈÎÎñ",
	}
GN_HUANG_EVENTID = 1
GN_HUANG_SWITH = 2
GN_HUANG_DATE = 3
function greatnight_huang_event(event)
	--closed
--	--»î¶¯»¹Ã»¿ªÊ¼
--	if (gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID) == 0) then
--		return 1
--	end
	local award_times = gn_getaward_times()	--µ±Ìì»î¶¯½±Àø·­±¶±¶Êı
--	--Àñ¹Ù²éÑ¯µ±Íí»î¶¯
--	if (event == nil) then
--		--·Ç1±íÊ¾·µ»Ø3¸ö²ÎÊı£º 0£¬»î¶¯ID£¬½±Àø·­±¶±¶Êı
--		return 0, gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID), award_times
--	end
--
--	--»î¶¯»¹Î´¿ªÊ¼
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
--		return 3	--ÖÜÄ©3±¶
--	else
		return 2	--Æ½Ê±2±¶
--	end
end

function huang_event_hint()
	--closed
--	local swith, eventid, awardtimes = greatnight_huang_event()
--	if (swith == 1) then
--		return format("%s%s%s%s%s",
--				"»ÍÖ®ÕÂ»î¶¯µÄ»î¶¯Ê±¼äÎªÃ¿Íí<color=yellow>9µãÕûÖÁ11µãÕû<color>¡£´ËÊ±¼ä¶ÎÄÚ£¬´óÏÀ¿ÉÒÔ»ñµÃÔÚÄ³¸ö»î¶¯ÖĞ½±Àø·­±¶µÄºÃ´¦¡£<enter>Ä¿Ç°½±Àø·­±¶µÄ»î¶¯°üÀ¨£º<enter>",
--				"ËÎ½ğ´óÕ½<enter>",
--				"´³¹ØÈÎÎñ<enter>",
--				"Ò°ÛÅÈÎÎñÁ´<enter>",
--				"ÖÁÓÚ¾ßÌåÔÚµ±ÌìÊÇÄÄ¸ö»î¶¯·­±¶£¬ÇëÔÚÃ¿Íí<color=yellow>8µã<color>ºóÀñ¹Ù´¦²éÑ¯¡£Çë¸÷Î»¾¡ÇéÏíÊÜ¶À¹ÂÃËÖ÷µÄÀ¡Ôù¡£")
--		
--	else
--		return format("»ÍÖ®ÕÂ»î¶¯µÄÊ±¼äÎªÃ¿Íí<color=yellow>9µãÕûÖÁ11µãÕû<color>¡£½ñÍí»î¶¯ÊÇ£º<enter><color=yellow>%s<color><enter><enter>Çë¸÷Î»¾¡ÇéÏíÊÜ¶À¹ÂÃËÖ÷µÄÀ¡Ôù¡£",
--					TB_HUANGZHIZHANG_EVENT[eventid])
--	end
	return ""
end

function huang_event_name()
	local swith, eventid, awardtimes = greatnight_huang_event()
	if (swith == 1) then
		return "Ch­a biÕt"
	else
		return TB_HUANGZHIZHANG_EVENT[eventid]
	end
end

function huang_event_msg()
	local name = huang_event_name()
	return format("Ho¹t ®éng Hoµng chi ch­¬ng sÏ diÔn ra lóc 21h00 ®ªm nay, ho¹t ®éng nh©n ®«i phÇn th­ëng liªn quan lµ: <color=yellow>%s<color>",name)
end

function seed_birth_city()
	local MapStr = "§«ng m«n D­¬ng Ch©u";
	if (1 == gb_GetTask("h¹t Huy Hoµng", 1)) then
		MapStr = "B¾c m«n §¹i Lı ";
	end;
	return format("12h00 tr­a nay sÏ b¾t ®Çu ho¹t ®éng Huy chi ch­¬ng, qu¶ Hoµng Kim sÏ xuÊt hiÖn t¹i <color=yellow>%s<color>",MapStr)
end

if EVENTS_TB then 
EVENTS_TB[getn(EVENTS_TB)+1] = {	--»ÔÖ®ÕÂ
			name = "Huy Hoµng",
			describe = seed_birth_city
		}
--EVENTS_TB[getn(EVENTS_TB)+1] = {	--»ÍÖ®ÕÂ
--			name = "»ÍÖ®ÕÂ",
--			describe = huang_event_msg
--		}
end
end