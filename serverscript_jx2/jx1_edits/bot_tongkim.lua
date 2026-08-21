-- Author: phien bot 21/08/2026
-- Function: ham phu tro cho BOT KPlayer tham gia Tong Kim.
--
-- TRIET LY: bot phai di DUNG duong cua nguoi choi. Toan bo khau bao danh chay
-- bang chinh go_tong()/go_kim() cua script\tinhnang\tong_kim_tcap\mobinhtk.lua
-- (C++ dat TaskTemp TMP_INDEX_NPC roi ExecuteScript), khau ra tran chay bang
-- chinh tong_ratran()/kim_ratran() cua script\timertask\task02.lua. Tep NAY chi
-- chua DUY NHAT thu ma C++ khong tu lam duoc: TRA LAI TRANG THAI CU khi bot roi
-- tran SOM (truoc khi task03 chay), va mot ham tra trang thai tran cho C++ doc.
--
-- VI SAO CAN: khi het gio, task03.lua PlayerEndTongKim() tra lai trang thai cho
-- moi nguoi - nhung vong do kep boi GetNextPlayer(MS_TONGKIM, idx, 0) nen chi
-- cham toi nguoi CON trong mission. Bot roi som (chu game tat cong tac, bot bi
-- goi ve, tran bi dong bat thuong) se GIU NGUYEN camp 1/2 + SetPKMode(1,1) khoa
-- co PK + SetPunish(1) + SetLogoutRV(1) + diem hoi sinh tro vao map 379 - tuc
-- mang trang thai chien truong ra the gioi thuong va thanh DICH cua nguoi choi
-- that khac camp o giua thanh (KNpcSet::GetRelation khong he kiem cung map).

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

-- Tra lai trang thai truoc khi vao Tong Kim.
-- Chep Y NGUYEN khoi phuc hoi cua PlayerEndTongKim (timertask\task03.lua:172-216),
-- BO phan thuong / tich luy / thong bao (bot khong nhan thuong khi thoat som) va
-- BO NewWorld (C++ tu quyet dinh dua bot di dau sau do - thuong la Than Hanh Phu).
--
-- Goi tu C++: Player[n].ExecuteScript("\\script\\global\\bot_tongkim.lua",
--                                     "bot_tk_thoat", 0, false)
function bot_tk_thoat()
	-- Cac ham mission doc bien toan cuc SubWorld (SCRIPT_SUBWORLDINDEX), KHONG
	-- doc map bot dang dung - mobinhtk.lua:21-26 cung lam dung the nay.
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId >= 0 then
		SubWorld = nSubWorldId
		local nDataIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
		if nDataIdx > 0 then
			-- param 0 = 0: task03 se KHONG con dem con bot nay khi tong ket
			-- (vong duyet cua no kep boi param 0 == 1), khoi bi NewWorld ve
			-- map 324 luc het tran trong khi no dang luyen cong o bai.
			SetPMParam(MS_TONGKIM, nDataIdx, 0, 0)
			-- Go han khoi mission: tra lai o cho nguoi choi that va khong con
			-- bi tinh vao GetMSPlayerCount khi can bang quan so 2 phe.
			DelMSPlayer(MS_TONGKIM, PlayerIndex)
		end
	end

	SetPKMode(0, 0)          -- bo khoa co PK, tra ve PK tu do
	SetFightState(0)         -- ve trang thai phi chien dau
	SetPunish(0)             -- tra lai luat chet mat do binh thuong
	SetCreateTeam(1)         -- cho lap to doi lai
	ReSetMask()
	RestoreRunSpeed()        -- ham nay dinh nghia phia LUA (lib_tktc.lua:687)
	SetCurCamp(GetCamp())    -- QUAN TRONG NHAT: tra camp ve camp goc theo mon phai
	SetDeathScript("")       -- bo script chet rieng cua Tong Kim
	RemoveRankBattle()
	IgnoreState()            -- xoa het trang thai skill dinh tren nguoi
	SetLogoutRV(0)
	SetTempRevPos(53, 51904, 102048)  -- diem hoi sinh ve Ba Lang Huyen nhu task03
	SetNpcTimeIdle(0)        -- bo luat 5 phut dung im bi day ra
	SetTask(T_CHECKPHETK, 0) -- xoa phe da gia nhap
	StopTimer()
end
