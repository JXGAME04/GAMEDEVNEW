-- ============================================================================
-- vt_chinam.lua  [VTCN 06/09 v2]  Ham may chu phuc vu bang CHI NAM NHIEM VU (F11), hai muc
-- Van tieu. Goi tu KProtocolProcess.cpp::UiCommandScript case 6 (danh sach trang):
--   vt_quit_canhan : nut "Bo nhiem vu" tab ca nhan -> pActivity:AbandonTask (hop xac nhan, du luat)
--   vt_quit_bang   : nut "Bo nhiem vu" tab bang    -> pActivity:CancelTongTask (npc_consigner.lua)
-- v2: KHONG con vt_goto_canhan (thue xe / NewWorld) - chu game 06/09: "khong duoc nhay map";
-- dan duong hoan toan o client (CoreShell.cpp TG_VanTieu*: phu ve thanh -> Xa Phu -> di bo).
-- ============================================================================
Include("\\script\\activitysys\\config\\129\\npc_consigner.lua")	-- keo theo head/extend/variables + CancelTongTask

function vt_quit_canhan()
	pActivity:AbandonTask()
end

function vt_quit_bang()
	pActivity:CancelTongTask()
end
