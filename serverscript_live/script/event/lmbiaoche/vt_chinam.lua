-- ============================================================================
-- vt_chinam.lua  [VTCN 06/09]  Ham may chu phuc vu bang CHI NAM NHIEM VU (F11), hai muc
-- Van tieu. Goi tu KProtocolProcess.cpp::UiCommandScript case 6 (danh sach trang):
--   vt_goto_canhan : dan duong nhanh ca nhan.
--       - chua nhan nhiem vu       -> thue xe toi Long Mon tran (gia HD_CFG "VT_TIEN_XE_CHINAM",
--                                     mac dinh 1000 luong - cung khuon st3_goboss cua San Boss Sat Thu)
--       - da nhan, xe chua xuat phat -> dua toi Tieu Su diem xuat phat, MIEN PHI (y het
--                                     transToBeginPos luc vua nhan nhiem vu)
--       - xe da xuat phat          -> chi nhac (xe phai di bo; lac xe thi dung Truyen tong o Tieu Su)
--     Client (CoreShell.cpp TG_VanTieu*) tu di bo toi NPC sau khi doi ban do.
--   vt_quit_canhan : nut "Bo nhiem vu" tab ca nhan -> pActivity:AbandonTask (hop xac nhan, du luat)
--   vt_quit_bang   : nut "Bo nhiem vu" tab bang    -> pActivity:CancelTongTask (npc_consigner.lua)
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\activitysys\\config\\129\\npc_consigner.lua")	-- keo theo head/extend/variables + CancelTongTask
Include("\\script\\global\\maplist.lua")

VTCN_MAP_LONGMON = 121
VTCN_X_CHUONGQUY = 1989		-- script\startgame\lmbj_addnpc.lua LMBJ_POS_ZHANGGUI
VTCN_Y_CHUONGQUY = 4476

function vt_goto_canhan()
	local nFlag = GetTask(TSK_LMBJTaskFlag)
	if nFlag ~= 0 then
		local nId = floor(nFlag / 10)
		local bFlag = mod(nFlag, 10)
		local tbLo = pActivity.tbBJPathLevel[nId]
		if not tbLo then
			Msg2Player("<color=Yellow>[ChØ nam] D÷ liÖu nhiÖm vô ¸p Tiªu kh«ng hîp lÖ, h·y gÆp ¤ng chñ Tiªu côc kiÓm tra.")
			return
		end
		if tbLo[2] >= 10 then
			Msg2Player("<color=Yellow>[ChØ nam] Ng­¬i ®ang nhËn ¸p Tiªu Bang, xem môc VËn tiªu bang héi.")
			return
		end
		if GetTask(TSK_LMBJTaskTime) ~= 0 then
			Msg2Player("<color=Yellow>[ChØ nam] Tiªu Xa ®· xuÊt ph¸t, h·y hé tèng xe tíi Tiªu S­ ®iÓm cuèi. L¹c xe th× gÆp Tiªu S­ chän TruyÒn tèng ®Õn vÞ trÝ cña Tiªu Xa.")
			return
		end
		local nSid = tbLo[1][1 + bFlag]
		local tbDiem = pActivity.tbBJPoints[nSid]
		if not tbDiem then
			return
		end
		Msg2Player(format("<color=Cyan>[ChØ nam] §­a ng­¬i tíi Tiªu S­ ®iÓm xuÊt ph¸t: %s.", tbDiem[1]))
		NewWorld(tbDiem[2][1], tbDiem[2][2], tbDiem[2][3])
		return
	end
	local nMap = GetWorldPos()
	if nMap == VTCN_MAP_LONGMON then
		return	-- client tu di bo toi Ong chu Tieu cuc
	end
	local nTien = HD_CFG("VT_TIEN_XE_CHINAM", 1000)
	if GetCash() < nTien then
		Msg2Player(format("<color=Yellow>[ChØ nam] Kh«ng ®ñ %d l­îng ®Ó thuª xe tíi Long M«n trÊn.", nTien))
		return
	end
	Pay(nTien)
	NewWorld(VTCN_MAP_LONGMON, VTCN_X_CHUONGQUY + 2, VTCN_Y_CHUONGQUY + 2)
	Msg2Player(format("<color=Cyan>[ChØ nam] §· thuª xe tíi Long M«n trÊn (%d l­îng), ®ang ch¹y tíi ¤ng chñ Tiªu côc.", nTien))
end

function vt_quit_canhan()
	pActivity:AbandonTask()
end

function vt_quit_bang()
	pActivity:CancelTongTask()
end
