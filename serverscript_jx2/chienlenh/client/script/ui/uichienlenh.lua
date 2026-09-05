-- uichienlenh.lua - [CL 04/09 DOT2] Lua phia CLIENT cua cua so Chien Lenh (khuon y het uiauction_house.lua).
-- May chu day du lieu bang 7 goi CL_* (script_protocol/protocol_def_c.lua) -> cac ham CLUI_On* duoi day
-- -> ham C++ CLUi_* (KChienLenhClient.cpp) -> cua so UiChienLenh.cpp ve. Chieu nguoc: nguoi choi bam nut
-- -> CoreShell GOI_CHIENLENH_UI -> C++ goi UIChienLenh:xxx() o day -> gui 3 yeu cau CL_REQUEST_* len may chu.
-- Client KHONG tu tinh gi ca (khong doc duoc bien nhiem vu, trang thai o MySQL) - chi ve dung nhung gi may chu dua.

-- [CL 04/09 DOT2d] MOI script client chay trong state Lua RIENG (KScriptProtocol.cpp sClientLoad): phai tu Include
-- bang ten giao thuc + ObjBuffer nhu uiauction_house.lua, khong thi ScriptProtocol/ObjBuffer la nil khi bam nut
-- (jx_mail.log 14:25 "[SP] RunClientLua loi ... UIChienLenh:RequestOpen()"). Chieu nhan van chay vi chi goi ham C++.
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")

UIChienLenh = UIChienLenh or {}

function CLUI_Gui(szEnum, ...)
	local h = OB_Create()
	for i = 1, arg.n do
		if type(arg[i]) == "string" then
			ObjBuffer:PushByType(h, OBJTYPE_STRING, arg[i])
		else
			ObjBuffer:PushByType(h, OBJTYPE_NUMBER, arg[i] or 0)
		end
	end
	ScriptProtocol:SendData(szEnum, h)
	OB_Release(h)
end

-- ============================ nguoi choi bam -> may chu ============================
-- bam bieu tuong: xin may chu day tron bo (thong tin + moc + nhiem vu) roi mo cua so
function UIChienLenh:RequestOpen()
	CLUI_Gui("emSCRIPT_PROTOCOL_CL_REQUEST_OPEN", 1)
end

function UIChienLenh:OnWindowClosed()
end

-- bam o sang: nhan moc thuong (nIdx = so thu tu moc, nBranch 0 Thuong / 1 Hao Hoa)
function UIChienLenh:RequestGetAward(nIdx, nBranch)
	CLUI_Gui("emSCRIPT_PROTOCOL_CL_REQUEST_GETAWARD", nIdx, nBranch)
end

-- bam "Nhan" tren dong nhiem vu da xong
function UIChienLenh:RequestGetMission(nId)
	CLUI_Gui("emSCRIPT_PROTOCOL_CL_REQUEST_GETMISSION", nId)
end

-- bam "Den": C++ da hien goi y (tips) ngay tren cua so; khong can hoi may chu
function UIChienLenh:OnGotoMission(nId)
end

-- nut "Mua Chien Lenh": the ban o Ky Tran Cac (goods.txt 6/1/4977, 500 xu), dung the -> CL_MuaVip
function UIChienLenh:OnBuyVip()
	-- [CL 04/09 MUA] client khong tu mo Ky Tran Cac duoc -> xin may chu (CLUI_OnOpen(2) trong cl_ui.lua)
	CLUI_Gui("emSCRIPT_PROTOCOL_CL_REQUEST_OPEN", 2)
	CLUi_Msg("Mua ChiÕn LÖnh Hµo Hoa t¹i Kú Tr©n C¸c (500 xu) råi dïng thÎ")
end

function UIChienLenh:OnHelp()
	CLUI_Gui("emSCRIPT_PROTOCOL_CL_REQUEST_OPEN", 3)
	CLUi_Msg("Lµm nhiÖm vô lÊy ®iÓm, ®ñ ®iÓm bÊm « s¸ng ®Ó nhËn th­ëng qua th­")
end

function UIChienLenh:Reset()
end

-- ============================ may chu day xuong -> ve ============================
function CLUI_OnShowIcon(nShow)
	CLUi_SetIconVisible(nShow or 0)
end

function CLUI_OnInfo(nScore, nCap, nVip, nGotLow, nGotVip, nSoMoc, nLevelScore, nCloseTime, nDangMo, nVipPrice)
	CLUi_SetInfo(nScore or 0, nCap or 0, nVip or 0, nGotLow or 0, nGotVip or 0, nSoMoc or 0,
		nLevelScore or 0, nCloseTime or 0, nDangMo or 0, nVipPrice or 0)
end

function CLUI_OnAward(nIdx, nNeed, nCap, nBranch, nCount, szName, szItemInfo)
	CLUi_SetAward(nIdx or 0, nNeed or 0, nCap or 0, nBranch or 0, nCount or 0, szName or "", szItemInfo or "")
end

function CLUI_OnMission(nId, nKind, nScore, nTarget, nProg, nState, szTitle, szTips)
	CLUi_SetMission(nId or 0, nKind or 0, nScore or 0, nTarget or 0, nProg or 0, nState or 0, szTitle or "", szTips or "")
end

function CLUI_OnClear(n)
	CLUi_Clear()
end

-- nOpen = 1: day xong tron bo -> mo cua so; 0: chi ve lai (cua so dang mo)
function CLUI_OnRefresh(nOpen)
	if nOpen == 1 then
		CLUi_Open()
	else
		CLUi_Refresh()
	end
end

function CLUI_OnMsg(szMsg)
	CLUi_Msg(szMsg or "")
end
