-- ================================================================================================
-- [TKFIX 11/09] script/maps/tongkim/newworld_tk.lua - SCRIPT BAN DO 379 (chien truong Tong Kim trung cap)
-- Muc dich  : MapList.ini 379_NewWorldScript tro toi day. Engine goi OnNewWorld(szParam) khi vao map va
--             OnLeaveWorld(szParam) khi roi map (KNpc::ChangeWorld) VA khi thoat game (KPlayerSet::PrepareRemove);
--             bien toan cuc SubWorld = chi so map 379, PlayerIndex = nguoi choi.
--             Nguoi that roi 379 bang bat ky cach nao (lenh bai admin, GM, Tong Kim Chieu thu...) deu duoc go khoi
--             mission MS_TONGKIM (OnLeave cua tinhnang\phuban\mission01.lua bao "da roi", tha co, luu diem vao
--             T_SAVE_TK_*) -> khong con nhan bang diem / xep hang / dong chat, khong bi keo ve 324 luc tong ket.
--             Truoc day KNpc::ChangeWorld khong go mission va \script\maps\newworldscript.lua (495 map tro toi)
--             KHONG TON TAI nen moc nay chet. Tham so PARTNER_OFF|TISHENZHIREN cua map: KHONG xu ly (giu hanh vi cu).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng); MapList.ini doc luc boot -> can restart GameServer
-- Include   : lib_task.lua, lib_tktc.lua (MS_TONGKIM, MAP_TK_TC)
-- Ham (dong): OnNewWorld, OnLeaveWorld
-- Sua nong  : KHONG - can restart GameServer
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat
-- ================================================================================================
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function OnNewWorld(szParam)
end

function OnLeaveWorld(szParam)
	if (PlayerIndex == nil or PlayerIndex <= 0) then
		return
	end
	if (IsMission(MS_TONGKIM) ~= 1) then
		return
	end
	local nIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
	if (nIdx == nil or nIdx <= 0) then
		return
	end
	-- param 0 = 0: da duoc xu ly (bi day dung im tongtu/kimtu, tong ket task03, bot) -> khong go them
	if (GetPMParam(MS_TONGKIM, nIdx, 0) ~= 1) then
		return
	end
	-- thoat game: nguoi con dung tren 379 (PrepareRemove goi OnLeaveWorld truoc khi tu go mission) -> de engine go,
	-- GIU SetLogoutRV(1) de dang nhap lai vao diem bao danh va vao lai tran (nhu Linux battles\marshal\mission.lua OnLeave)
	local nW = GetWorldPos()
	if (nW == MAP_TK_TC) then
		return
	end
	-- roi map bang cach khac: go khoi tran (OnLeave: bao "da roi", tha co, luu T_SAVE_TK_*, tra camp/PK/punish)
	DelMSPlayer(MS_TONGKIM, 0)
	SetLogoutRV(0)
	SetFightState(0)
end
