-- hb_relay.lua  (S3Relay)  [RELAYHT 06/09]
-- TAC VU NHIP TIM -- vua la vi du mau, vua la phep thu ha tang.
--
-- Moi lan chay no lam 3 viec:
--   1. Ghi mot dong vao nhat ky relay (chung minh TaskCentre goi dung gio)
--   2. Cong 1 vao mot bo dem ben vung trong ShareData (chung minh ObjBuffer +
--      MySQL thong nhau: so nay KHONG mat khi khoi dong lai relay)
--   3. Neu bat g_bThongBao thi doc mot cau len TAT CA GameServer bang
--      GlobalExecute (chung minh duong relay -> GameServer con song)
--
-- Khai bao lich giong ban Linux: TaskShedule() (dung, ban goc viet thieu chu 'c').

Include("\\script\\lib\\common_sharedata.lua")

-- 0 = khong doc thong bao trong game (mac dinh, de khong lam phien nguoi choi)
g_bThongBao = 0

-- Chu tren man hinh. LUU Y: tep nay la ASCII, muon viet tieng Viet co dau thi
-- phai ghi bang bo cong cu vn_to_octal (TCVN3), dung go truc tiep.
g_szThongBao = "May chu dang chay on dinh."

g_tbHB = nil

function TaskShedule()
	TaskName("Nhip tim relay (phep thu ha tang)")
	TaskInterval(10)		-- 10 phut mot lan
	TaskCountLimit(0)		-- 0 = chay mai
	-- khong goi TaskTime -> chay ngay tu luc relay khoi dong
	OutputMsg("[hb_relay] da khai bao lich: 10 phut/lan")
end

function TaskContent()
	local nLan = GetTaskCurCount()

	-- (2) bo dem ben vung trong ShareData
	if not g_tbHB then
		g_tbHB = NewCommonShareData("RELAY_HEARTBEAT")
	end

	local nTong = 0
	if g_tbHB then
		local tb = g_tbHB:GetData("tong")
		if type(tb) == "table" and type(tb.n) == "number" then
			nTong = tb.n
		end
		nTong = nTong + 1
		g_tbHB:SetData("tong", { n = nTong, szLan = FormatTime2String() })
	end

	-- (1) nhat ky relay
	OutputMsg(format("[hb_relay] lan %d trong phien nay, tong tu truoc toi nay %d, luc %s",
		nLan, nTong, FormatTime2String()))

	-- (3) thong bao trong game
	if g_bThongBao == 1 then
		GlobalExecute(format("dw AddLocalCountNews([[%s]], 1);", g_szThongBao))
	end
end

-- Hai ham nay relay goi khi mot GameServer vua noi / vua san sang. De trong
-- cung duoc, nhung co san de kich ban ban Linux chep sang khong bao thieu ham.
function GameSvrConnected(dwGameSvrIP)
end

function GameSvrReady(dwGameSvrIP)
end
