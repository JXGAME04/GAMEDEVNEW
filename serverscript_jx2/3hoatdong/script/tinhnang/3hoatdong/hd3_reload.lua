-- ============================================================================
-- HD3_RELOAD.LUA - SINH TU DONG boi ReverseTools/gen_hd3_reload.py - DUNG SUA TAY
-- Nap lai NHANH toan bo script cua 3 hoat dong ma KHONG can restart server.
--
-- Vi sao khong dung lenh GM '?gm RLAS': TextGMFilter chi duoc goi khi bien dich
-- co #ifdef _CHAT_SCRIPT_OPEN - macro nay KHONG duoc dinh nghia o dau trong du an
-- => moi lenh '?gm' vo tac dung. Ham Lua ReLoadScript thi CO dang ky va dung duoc.
--
-- Dung: bam Lenh bai admin -> muc 'Nap lai toan bo script'.
-- ============================================================================

HD3_RELOAD_LIST = {
	"\\script\\task\\tollgate\\killer\\hd3_st_goboss.lua",
	"\\script\\task\\tollgate\\killer\\kill_level.lua",
	"\\script\\task\\tollgate\\killer\\lib_killlevel.lua",
	"\\script\\task\\tollgate\\killer\\mibao_head.lua",
	"\\script\\task\\tollgate\\killer\\nieshichen.lua",
	"\\script\\task\\tollgate\\killer\\shashou_mibao.lua",
	"\\script\\missions\\fengling_ferry\\bigbossdeath.lua",
	"\\script\\missions\\fengling_ferry\\boss.lua",
	"\\script\\missions\\fengling_ferry\\bossdeath.lua",
	"\\script\\missions\\fengling_ferry\\fld_death.lua",
	"\\script\\missions\\fengling_ferry\\fld_head.lua",
	"\\script\\missions\\fengling_ferry\\fld_landingtimer.lua",
	"\\script\\missions\\fengling_ferry\\fld_smalltimer.lua",
	"\\script\\missions\\fengling_ferry\\fldmap_boat1.lua",
	"\\script\\missions\\fengling_ferry\\fldmap_boat2.lua",
	"\\script\\missions\\fengling_ferry\\fldmap_boat3.lua",
	"\\script\\missions\\fengling_ferry\\hd3_baoruongthuytac.lua",
	"\\script\\missions\\fengling_ferry\\hd3_thuyenphu.lua",
	"\\script\\missions\\fengling_ferry\\hd3_thuyenphubac.lua",
	"\\script\\missions\\fengling_ferry\\lang.lua",
	"\\script\\missions\\fengling_ferry\\mission.lua",
	"\\script\\missions\\fengling_ferry\\shuizeideath.lua",
	"\\script\\missions\\challengeoftime\\award.lua",
	"\\script\\missions\\challengeoftime\\chuangguang30.lua",
	"\\script\\missions\\challengeoftime\\chuangguang30_playerdeath.lua",
	"\\script\\missions\\challengeoftime\\doubleexp.lua",
	"\\script\\missions\\challengeoftime\\include.lua",
	"\\script\\missions\\challengeoftime\\mission_match.lua",
	"\\script\\missions\\challengeoftime\\npc.lua",
	"\\script\\missions\\challengeoftime\\npc_death.lua",
	"\\script\\missions\\challengeoftime\\player_death.lua",
	"\\script\\missions\\challengeoftime\\rank_perday.lua",
	"\\script\\missions\\challengeoftime\\timer_board.lua",
	"\\script\\missions\\challengeoftime\\timer_close.lua",
	"\\script\\missions\\challengeoftime\\timer_match.lua",
	"\\script\\missions\\challengeoftime\\item\\chuangguanbaoxiang.lua",
	"\\script\\missions\\challengeoftime\\npc\\dragonboat_main.lua",
	"\\script\\missions\\challengeoftime\\npc\\transfer.lua",
	"\\script\\vng_feature\\challengeoftime\\npcNhiepThiTran.lua",
	"\\script\\tinhnang\\3hoatdong\\hd3_driver.lua",
	"\\script\\task\\tollgate\\killbosshead.lua",
	"\\script\\global\\autoexec_npc_hd3.lua",
	"\\script\\header\\cauhinh_hoatdong.lua",
	"\\script\\item\\hd3_admin.lua",
	"\\settings\\trigger_include.lua",
	"\\settings\\trigger_challengeoftime.lua",
}

function HD3_ReloadAll()
	local n = 0
	for i = 1, getn(HD3_RELOAD_LIST) do
		ReLoadScript(HD3_RELOAD_LIST[i])
		n = n + 1
	end
	if (HD_NapLaiCauHinh ~= nil) then
		HD_NapLaiCauHinh()
	end
	return n
end
