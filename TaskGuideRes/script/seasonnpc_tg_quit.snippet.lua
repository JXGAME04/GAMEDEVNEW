-- Snippet nay DA duoc chen vao CUOI:
--   E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\global\seasonnpc.lua
-- (seasonnpc.lua khong nam trong git; giu ban sao o day de tra cuu / chen lai)

-- ============================================================
-- [TaskGuide 19/08/2026] Nut 'Bo nhiem vu' tren bang Chi nam nhiem vu (F11).
-- Mo dung hop xac nhan huy CHUAN (Task_CancelConfirm): du luat tru luot huy,
-- phat huy lau, huy bang 100 manh SHXT - y het dung truoc NPC chon muc huy.
-- Duoc goi tu xa qua UI_CMD case 6 (KProtocolProcess.cpp::UiCommandScript).
-- ============================================================
function tg_quit()
	if tl_gettaskcourse() ~= 1 then
		return
	end
	nt_setTask(1045, 1)
	Task_CancelConfirm()
end
