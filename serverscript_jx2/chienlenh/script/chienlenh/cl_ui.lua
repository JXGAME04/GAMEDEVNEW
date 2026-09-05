-- cl_ui.lua - [CL 04/09 DOT2] MAY CHU day du lieu cua so Chien Lenh xuong client + nhan 3 yeu cau tu cua so.
-- Kenh ScriptProtocol (protocol.lua / script_protocol/protocol_def_gs.lua), khuon y het auction_manager.lua:
-- bo phan phoi dat san PlayerIndex roi goi CLUI_OnOpen / CLUI_OnGetAward / CLUI_OnGetMission voi tham so da giai.
-- Client CHI VE. Moi tinh toan (diem, moc, ledger, thu) nam o KChienLenh.cpp: CL_TrangThai / CL_Award / CL_Mission /
-- CL_Nhan / CL_LinhNhiemVu. Vat pham thuong: chuoi 6 so (genre,detail,particular,level,series,luck) de client dung
-- Auc_FillChatItemInfo dung lai bieu tuong + chu giai THAT (nhu hop thu). Chi mon DAU TIEN cua chuoi award duoc ve;
-- thu van giao du.
Include("\\script\\chienlenh\\cl_def.lua")
Include("\\script\\mail\\mailmanager.lua")
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")

CLUI_SO_NV = 20

function CLUI_GuiXuong(szEnum, ...)
	local h = OB_Create()
	for i = 1, arg.n do
		if type(arg[i]) == "string" then
			ObjBuffer:PushByType(h, OBJTYPE_STRING, arg[i])
		else
			ObjBuffer:PushByType(h, OBJTYPE_NUMBER, arg[i] or 0)
		end
	end
	SendScriptDataToPlayer(PlayerIndex, ScriptProtocol[szEnum], h)
	OB_Release(h)
end

-- thong bao ngan: hien tren cua so (neu dang mo) + khung thoai
function CLUI_Bao(sz)
	CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_MSG", sz)
	Msg2Player(sz)
end

-- mot moc thuong -> (chuoi 6 so cho bieu tuong, so luong, ten)
function CLUI_ThongTinMoc(a)
	local szInfo, nCount, szName = "", (a.award_count or 1), (a.title or "")
	local tb = MailManager_ParseAward(a.award or "")
	local e = tb[1]
	if e then
		nCount = e.nCount or nCount
		if e.szKind == "item" then
			szInfo = format("%d,%d,%d,%d,%d,%d", e.nGenre or 0, e.nDetail or 0, e.nParticular or 0,
				e.nLevel or 0, e.nSeries or 0, e.nLuck or 0)
		elseif e.szKind == "aucitem" and e.bLoi ~= 1 and e.szInfo and e.szInfo ~= "" then
			szInfo = e.szInfo
		end
	end
	return szInfo, nCount, szName
end

-- day TRON BO: CLEAR -> INFO -> AWARD x N -> MISSION x N -> REFRESH(nMo: 1 mo cua so, 0 chi ve lai)
function CLUI_DayTronBo(nMo)
	if CL_Ready() ~= 1 then
		CLUI_Bao("Chi’n L÷nh ch≠a sΩn sµng, thˆ lπi sau")
		return 0
	end
	local inf = CL_Info()
	local tt = CL_TrangThai()
	if not tt then
		CL_Load()
		tt = CL_TrangThai()
	end
	if not inf or not tt then
		CLUI_Bao("Kh´ng t∂i Æ≠Óc trπng th∏i Chi’n L÷nh")
		return 0
	end
	CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_CLEAR", 1)
	CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_INFO", tt.score, tt.cap, tt.vip, tt.got_low, tt.got_vip, tt.so_moc,
		inf.level_score, inf.close_time, tt.dangmo, inf.vip_price_xu)
	for i = 1, (tt.so_moc or 0) do
		local a = CL_Award(i)
		if a then
			local szInfo, nCount, szName = CLUI_ThongTinMoc(a)
			CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_AWARD", a.idx, a.need_score, a.cap, a.branch, nCount, szName, szInfo)
		end
	end
	for i = 1, CLUI_SO_NV do
		local m = CL_Mission(i)
		if m and m.enabled == 1 then
			CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_MISSION", m.id, m.kind, m.score, m.target,
				tt["td"..i] or 0, tt["nv"..i] or 0, m.title or "", m.tips or "")
		end
	end
	CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_REFRESH", nMo)
	return 1
end

-- ============================ 3 yeu cau tu cua so ============================
function CLUI_OnOpen(n)
	CLUI_DayTronBo(1)
end

-- CL_Nhan: 1 = da gui thu, 2 = moc nay da nhan, 3 = moc chua cau hinh thuong (award rong), 0 = chua du dieu kien
function CLUI_OnGetAward(nIdx, nBranch)
	local kq = CL_Nhan(nIdx or 0, nBranch or 0) or 0
	if kq == 1 then
		CLUI_Bao("ß∑ gˆi th≠Îng mËc vµo hÈp th≠")
		CLUI_DayTronBo(0)
	elseif kq == 2 then
		CLUI_Bao("MËc nµy Æ∑ nhÀn rÂi")
	elseif kq == 3 then
		CLUI_Bao("MËc nµy ch≠a Æ≠Óc c u h◊nh th≠Îng tr™n web")
	else
		CLUI_Bao("Ch≠a ÆÒ Æi”m ho∆c ch≠a mÎ nh∏nh Hµo Hoa")
	end
end

-- CL_LinhNhiemVu -> so diem cong (0 = chua xong / da linh)
function CLUI_OnGetMission(nId)
	local nDiem = CL_LinhNhiemVu(nId or 0) or 0
	if nDiem > 0 then
		CLUI_Bao(format("NhÀn %d Æi”m Chi’n L÷nh", nDiem))
		CLUI_DayTronBo(0)
	else
		CLUI_Bao("Nhi÷m vÙ ch≠a hoµn thµnh ho∆c Æ∑ nhÀn")
	end
end

-- playerlogin.lua goi ngay sau CL_Load(): hien bieu tuong Chien Lenh tren HUD (client giu lai neu chua vao game)
function CLUI_OnLogin()
	if CL_Ready() == 1 then
		CLUI_GuiXuong("emSCRIPT_PROTOCOL_CL_SHOWICON", 1)
	end
end

-- cl_gm.lua (menu Lenh Bai Admin) co the goi de mo cua so ma khong can bieu tuong
function CLUI_MoCuaSo()
	CLUI_DayTronBo(1)
end
