# -*- coding: utf-8 -*-
r"""v39 - sinh BO TEST VIEM DE cho Lenh Bai Admin.  (thay han v36)

Chu game da noi ro: "bo test toan bo hoat dong" nghia la test TOAN BO cua
HOAT DONG VIEM DE, khong phai gom moi hoat dong vao mot cho. v36 hieu sai,
tep nay viet lai cho dung.

BA THU v36 LAM SAI, da sua o day:

  1. THIEU Include ydbz_driver.lua  -> bam "bat dau bao danh" bao
     "Khong thay YDBZ_OnTrigger". Nap driver la co ca YDBZ_OnTrigger LAN tbReady
     (driver -> trigger -> readymap\ready.lua -> head.lua).

  2. MENU QUA 6 NUT. Client CHI VE 6 NUT mot luc, nut thu 7 tro di phai keo
     thanh truot (xem 10 luat o script\test\bangthanh_f.lua:5-24). Moi man o day
     deu <= 6 nut, ke ca nut Quay lai.

  3. KHONG CO CACH TEST MOT MINH. Xem ghi chu o TTHD_MM_GiaiThich.

CACH TEST MOT MINH - vi sao phai lam rieng:
    tbReady:Group(tbParty) nhan danh sach TO DOI (khong phai nguoi choi) va co
    chot `if nGroupCount == 1 then return {} end` (readymap\ready.lua). Tuc CHI
    MOT to doi bao danh la tra ve bang RONG, khong ai vao duoc tran. Cong them
    yandibaozang_main.lua:72 doi to doi >= YDBZ_TEAM_COUNT_LIMIT (=4, ghi cung o
    readymap\include.lua:25).
    => Duong bao danh binh thuong KHONG THE test bang mot nguoi.
    Nen che do nay di THANG vao ruot, dung dung hai ham chinh chu goc dung:
        tbReady:InitMatchMission(nMapId, tbGroup)   -- mo mission 50 tren ban do
        tbReady:ToMatch(nMapId, tbGroup)            -- AddMSPlayer + PlayerEnterMatch
    voi tbGroup = { { <ten minh> } }. PlayerEnterMatch lo tron goi: dich chuyen,
    dat co chien dau, phe, kich ban chet, SetTaskTemp(200,1)...
    Thoat thi goi YDBZ_restore(PlayerIndex, ...) - dung ham don dep CUA CHINH
    tinh nang (include.lua:34), khong tu bia lai.
"""
import io
import os
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("bangtxt", os.path.join(HERE, "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bangtxt)

LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
KHO = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\script\item"
TEN = "test_hoatdong_admin.lua"
F_OUT = os.path.join(LIVE, "item", TEN)
F_LB = os.path.join(LIVE, "item", "lenhbaiadmin.lua")
HAU_TO = ".truoc_botest_2608"

LUA = r"""
-- ============================================================================
-- TEST_HOATDONG_ADMIN.LUA - BO TEST HOAT DONG VIEM DE BAO TANG
-- SINH TU DONG boi ReverseTools\viemde\v39_gen_test_viemde.py - DUNG SUA TAY
-- (sua tay bang trinh soan thao UTF-8 se lam hong toan bo dau tieng Viet TCVN3)
--
-- Include tu lenhbaiadmin.lua; main() cua lenh bai dofile lai chinh no moi lan
-- dung => SUA TEP NAY KHONG CAN KHOI DONG LAI GameServer.
--
-- HAI LUAT CUA MENU:
--   * CAM dau "/" trong NHAN (ScriptFuns.cpp:717 cat o dau "/" dau tien)
--   * Client CHI VE 6 NUT mot luc - moi man duoi day deu <= 6 nut
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
-- Mot dong nay keo theo ca chuoi: driver -> yandibaozang_trigger (YDBZ_OnTrigger)
-- -> readymap\ready.lua (tbReady) -> head.lua (YDBZ_MAP_MAP, YDBZ_BOAT_POS...).
-- Thieu no la bam "bat dau bao danh" se bao "Khong thay YDBZ_OnTrigger".
Include("\\script\\tinhnang\\viemde\\ydbz_driver.lua")

TTHD_PHIEN = "26/08/2026"

-- ---------------------------------------------------------------------------
-- Tien ich
-- ---------------------------------------------------------------------------
function TTHD_In(szDong)
	Msg2Player(szDong)
end

function TTHD_CoKhong(bDung)
	if bDung then
		return "<color=green>Có<color>"
	end
	return "<color=red>Không<color>"
end

function TTHD_CoHam(szTen)
	return getglobal(szTen) ~= nil
end

function TTHD_CoMap(nMapId)
	if SubWorldID2Idx == nil then
		return 0
	end
	if SubWorldID2Idx(nMapId) >= 0 then
		return 1
	end
	return 0
end

-- ---------------------------------------------------------------------------
-- MENU GOC  (5 muc + thoat = 6 nut)
-- ---------------------------------------------------------------------------
function TTHD_Root()
	SayEx({"<color=yellow>Bộ test Viêm Đế Bảo Tàng<color> (bản " .. TTHD_PHIEN .. ") - chọn mục:",
	"1. Chẩn đoán Viêm Đế (một lượt quét)/TTHD_ChanDoan",
	"2. Báo danh và vào trận/TTHD_BaoDanh",
	"3. Test một mình, không cần tổ đội/TTHD_MotMinh",
	"4. Vật phẩm và hệ xúc xắc/TTHD_VatPham",
	"5. Bản đồ, NPC và cấu hình/TTHD_BanDo",
	"Kết thúc đối thoại./no"})
end

-- ---------------------------------------------------------------------------
-- 1) CHAN DOAN
-- ---------------------------------------------------------------------------
function TTHD_ChanDoan()
	TTHD_In("<color=yellow>===== Chẩn đoán Viêm Đế =====<color>")
	TTHD_In("Giờ máy chủ: <color=gold>" .. GetLocalDate("%y-%m-%d %H:%M:%S") .. "<color>")

	TTHD_In("<color=yellow>[1] Kịch bản<color>")
	TTHD_In("    Bật trong cấu hình: " .. TTHD_CoKhong(HD_CFG("YDBZ_BAT", 1) == 1)
		.. " | Giờ chạy: " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("    YDBZ_DriverInit: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_DriverInit"))
		.. " | YDBZ_Tick: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_Tick"))
		.. " | YDBZ_OnTrigger: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_OnTrigger")))
	TTHD_In("    tbReady (ruột báo danh): " .. TTHD_CoKhong(tbReady ~= nil)
		.. " | YDBZ_restore: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_restore")))

	TTHD_In("<color=yellow>[2] Bản đồ<color>")
	local nMapNpc = HD_CFG("YDBZ_NPC_MAP", 37)
	TTHD_In("    Bản đồ NPC báo danh " .. nMapNpc .. ": " .. TTHD_CoKhong(TTHD_CoMap(nMapNpc) == 1))
	local nCo = 0
	local i
	for i = 1003, 1017 do
		nCo = nCo + TTHD_CoMap(i)
	end
	TTHD_In("    Phòng chờ 1003-1017: <color=gold>" .. nCo .. "/15<color>")
	nCo = 0
	for i = 853, 862 do
		nCo = nCo + TTHD_CoMap(i)
	end
	TTHD_In("    Bản đồ trận 853-862: <color=gold>" .. nCo .. "/10<color>")

	TTHD_In("<color=yellow>[3] Hệ xúc xắc trong động cơ<color>")
	TTHD_In("    ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice"))
		.. " | AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo"))
		.. " | RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem")))
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("    <color=red>Chưa có trong DLL - thay CoreServer.dll rồi khởi động lại.<color>")
	end

	TTHD_In("<color=yellow>[4] Điều kiện tham gia<color>")
	TTHD_In("    Cấp tối thiểu " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. " | Tổ đội cần " .. YDBZ_TEAM_COUNT_LIMIT .. " tới " .. YDBZ_TEAM_COUNT_MAXLIMIT .. " người")
	TTHD_In("    Cấp của bạn: <color=gold>" .. GetLevel() .. "<color>"
		.. " | Tổ đội hiện tại: <color=gold>" .. GetTeamSize() .. "<color> người")
	TTHD_In("<color=yellow>===== Hết chẩn đoán =====<color>")
	TTHD_Root()
end

-- ---------------------------------------------------------------------------
-- 2) BAO DANH  (4 muc + quay lai = 5 nut)
-- ---------------------------------------------------------------------------
function TTHD_BaoDanh()
	SayEx({"<color=yellow>Báo danh và vào trận<color> - đường đi bình thường của người chơi",
	"1. Mở đợt báo danh ngay, không chờ tới giờ/TTHD_BD_Mo",
	"2. Dịch chuyển tới NPC báo danh ở Biện Kinh/TTHD_BD_Tele",
	"3. Xem điều kiện tham gia/TTHD_BD_YeuCau",
	"4. Vì sao một tổ đội chưa đủ để mở trận/TTHD_BD_ViSao",
	"Quay lại/TTHD_Root"})
end

function TTHD_BD_Mo()
	if not TTHD_CoHam("YDBZ_OnTrigger") then
		TTHD_In("<color=red>Không thấy YDBZ_OnTrigger.<color>")
		TTHD_In("Kiểm dòng Include ydbz_driver.lua ở đầu tệp test_hoatdong_admin.lua.")
		TTHD_BaoDanh()
		return
	end
	YDBZ_OnTrigger()
	TTHD_In("<color=green>Đã mở đợt báo danh Viêm Đế.<color>")
	TTHD_In("Thời gian báo danh " .. HD_CFG("YDBZ_PHUT_BAODANH", 5)
		.. " phút. Tới gặp Bình Bình cô nương ở Biện Kinh.")
	TTHD_In("Nhớ là cần <color=yellow>ít nhất hai tổ đội<color> mới mở được trận -")
	TTHD_In("muốn thử một mình thì dùng mục 3 ở menu gốc.")
	TTHD_BaoDanh()
end

function TTHD_BD_Tele()
	NewWorld(HD_CFG("YDBZ_NPC_MAP", 37), 1714, 3173)
	TTHD_In("Đã dịch chuyển tới NPC báo danh thứ nhất.")
	TTHD_In("Ba chỗ còn lại: (1642,3145) (1622,3019) (1857,2968).")
end

function TTHD_BD_YeuCau()
	TTHD_In("<color=yellow>===== Điều kiện tham gia =====<color>")
	TTHD_In("Cấp tối thiểu: <color=gold>" .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. "<color> - cấp của bạn: <color=gold>" .. GetLevel() .. "<color>")
	TTHD_In("Tổ đội: <color=gold>" .. YDBZ_TEAM_COUNT_LIMIT .. " tới "
		.. YDBZ_TEAM_COUNT_MAXLIMIT .. "<color> người - hiện có: <color=gold>"
		.. GetTeamSize() .. "<color>")
	TTHD_In("Trần lượt: " .. HD_CFG("YDBZ_LAN_NGAY", 4) .. " lần một ngày, "
		.. HD_CFG("YDBZ_LAN_TUAN", 10) .. " lần một tuần.")
	TTHD_In("Mang Viêm Đế Lệnh thì được miễn trần lượt.")
	TTHD_BaoDanh()
end

function TTHD_BD_ViSao()
	TTHD_In("<color=yellow>===== Vì sao một tổ đội chưa đủ =====<color>")
	TTHD_In("Hàm ghép nhóm tbReady:Group nhận danh sách <color=yellow>tổ đội<color>,")
	TTHD_In("không phải người chơi. Nó có chốt: nếu chỉ có <color=yellow>một<color> tổ đội")
	TTHD_In("thì trả về bảng rỗng, nên không ai vào được trận.")
	TTHD_In("Một bản đồ trận chứa <color=gold>ba<color> tổ đội tranh nhau, nên cần ít nhất hai.")
	TTHD_In("Muốn thử một mình thì dùng mục 3 ở menu gốc - nó đi thẳng vào ruột,")
	TTHD_In("bỏ qua khâu ghép nhóm.")
	TTHD_BaoDanh()
end

-- ---------------------------------------------------------------------------
-- 3) TEST MOT MINH  (4 muc + quay lai = 5 nut)
-- ---------------------------------------------------------------------------
function TTHD_MotMinh()
	SayEx({"<color=yellow>Test một mình<color> - đi thẳng vào trận, bỏ qua báo danh và ghép nhóm",
	"1. Vào thẳng trận một mình, bản đồ 853/TTHD_MM_Vao853",
	"2. Chọn bản đồ trận khác/TTHD_MM_ChonMap",
	"3. Thoát trận và phục hồi trạng thái/TTHD_MM_Thoat",
	"4. Chế độ này làm gì, có an toàn không/TTHD_MM_GiaiThich",
	"Quay lại/TTHD_Root"})
end

-- Ruot cua che do mot minh. Dung DUNG hai ham ma duong bao danh that dung,
-- chi khac la tu dung san mot "nhom" gom mot nguoi.
function TTHD_MM_Vao(nMapId)
	if tbReady == nil then
		TTHD_In("<color=red>Không thấy tbReady - kiểm Include ydbz_driver.lua.<color>")
		return
	end
	if TTHD_CoMap(nMapId) == 0 then
		TTHD_In("<color=red>Bản đồ " .. nMapId .. " chưa nạp vào máy chủ.<color>")
		return
	end
	local tbGroup = {}
	tbGroup[1] = {}
	tbGroup[1][1] = GetName()

	tbReady:InitMatchMission(nMapId, tbGroup)
	tbReady:ToMatch(nMapId, tbGroup)

	TTHD_In("<color=green>Đã vào trận Viêm Đế một mình, bản đồ " .. nMapId .. ".<color>")
	TTHD_In("Bạn đang ở phe 1. Đi tìm ải để đánh; boss cuối là Lương Mi Nhi.")
	TTHD_In("Hình nhân rơi qua <color=yellow>hệ xúc xắc<color> ở mọi boss - đó là chỗ cần soi.")
	TTHD_In("Xong thì quay lại lệnh bài, mục 3 rồi mục 3, để thoát cho sạch trạng thái.")
end

function TTHD_MM_Vao853() TTHD_MM_Vao(853) end
function TTHD_MM_Vao854() TTHD_MM_Vao(854) end
function TTHD_MM_Vao855() TTHD_MM_Vao(855) end
function TTHD_MM_Vao856() TTHD_MM_Vao(856) end
function TTHD_MM_Vao857() TTHD_MM_Vao(857) end

function TTHD_MM_ChonMap()
	SayEx({"<color=yellow>Chọn bản đồ trận<color> - mười bản đồ 853 tới 862 đều dùng được",
	"Bản đồ 854/TTHD_MM_Vao854",
	"Bản đồ 855/TTHD_MM_Vao855",
	"Bản đồ 856/TTHD_MM_Vao856",
	"Bản đồ 857/TTHD_MM_Vao857",
	"Quay lại/TTHD_MotMinh"})
end

function TTHD_MM_Thoat()
	if not TTHD_CoHam("YDBZ_restore") then
		TTHD_In("<color=red>Không thấy YDBZ_restore - kiểm Include ydbz_driver.lua.<color>")
		TTHD_MotMinh()
		return
	end
	-- Dung ham don dep CUA CHINH tinh nang (include.lua:34), khong tu bia lai:
	-- no go co chien dau, phe, kich ban chet, cam giao dich, SetTaskTemp(200,0)...
	YDBZ_restore(PlayerIndex, YDBZ_MISSION_MATCH, 0)
	NewWorld(HD_CFG("YDBZ_NPC_MAP", 37), 1714, 3173)
	TTHD_In("<color=green>Đã thoát trận và phục hồi trạng thái, đưa về Biện Kinh.<color>")
	TTHD_In("Nếu còn kẹt cờ chiến đấu thì thoát ra vào lại nhân vật.")
end

function TTHD_MM_GiaiThich()
	TTHD_In("<color=yellow>===== Chế độ một mình làm gì =====<color>")
	TTHD_In("Nó gọi đúng hai hàm mà đường báo danh thật vẫn gọi:")
	TTHD_In("   <color=gold>tbReady:InitMatchMission<color> - mở nhiệm vụ 50 trên bản đồ trận")
	TTHD_In("   <color=gold>tbReady:ToMatch<color> - ghi bạn vào nhiệm vụ rồi đưa vào trận")
	TTHD_In("Chỉ khác một chỗ: nó tự dựng sẵn một nhóm gồm mình bạn, nên không")
	TTHD_In("phải qua khâu ghép nhóm vốn đòi ít nhất hai tổ đội.")
	TTHD_In("Mọi thứ khác giữ nguyên: cờ chiến đấu, phe, kịch bản chết, giới hạn lượt.")
	TTHD_In("<color=red>Chỉ nên dùng trên máy thử.<color> Nó có tính một lượt tham gia trong ngày.")
	TTHD_MotMinh()
end

-- ---------------------------------------------------------------------------
-- 4) VAT PHAM VA XUC XAC  (3 muc + quay lai = 4 nut)
-- ---------------------------------------------------------------------------
function TTHD_VatPham()
	SayEx({"<color=yellow>Vật phẩm và hệ xúc xắc<color>",
	"1. Nhận vật phẩm Viêm Đế để thử/TTHD_VP_Nhan",
	"2. Mở cửa sổ xúc xắc ngay, không cần đánh boss/TTHD_VP_XucXac",
	"3. Kiểm tám hàm xúc xắc trong động cơ/TTHD_VP_KiemDLL",
	"Quay lại/TTHD_Root"})
end

function TTHD_VP_Nhan()
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1615, 1, 0, 0, 0)
	AddItem(6, 1, 1626, 1, 0, 0, 0)
	TTHD_In("Đã nhận: 2 Hình nhân, 1 Viêm Đế Đồ Đằng, 1 Viêm Đế Lệnh.")
	TTHD_In("Hình nhân là món rơi qua hệ xúc xắc ở mọi boss.")
	TTHD_In("Viêm Đế Lệnh cho miễn trần lượt tham gia.")
	TTHD_VatPham()
end

-- Test end-to-end ca he xuc xac. szFile phai la tep ma engine DA NAP va la noi
-- hai ham goi lai ton tai: lenhbaiadmin.lua (no Include tep nay, ma Include cua
-- JX1 = lua_dofile vao trang thai cua tep GOI - ScriptFuns.cpp:1969).
TTHD_XX_TEP = "\\script\\item\\lenhbaiadmin.lua"

function TTHD_VP_XucXac()
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("<color=red>Động cơ chưa có hàm ApplyItemDice.<color>")
		TTHD_In("Phải thay CoreServer.dll mới rồi khởi động lại máy chủ.")
		TTHD_VatPham()
		return
	end
	local nId = ApplyItemDice(1, 100, 30, TTHD_XX_TEP, "TTHD_XX_Chot", "TTHD_XX_Goi", 1)
	if nId == nil or nId <= 0 then
		TTHD_In("<color=red>ApplyItemDice trả về " .. tostring(nId) .. ".<color>")
		TTHD_VatPham()
		return
	end
	-- Hinh nhan {6,1,1614} - ma cua JX1. Ban Linux la 1605, ma do o JX1 lai la
	-- "Thiep chuc su de" nen truyen nham la hien ra cai phong thu.
	local nItem = AddDiceItemInfo(nId, 0, 6, 1, 1614, 1, 1, 1, 1)
	if nItem == nil or nItem <= 0 then
		TTHD_In("<color=red>AddDiceItemInfo trả về " .. tostring(nItem) .. ".<color>")
		TTHD_VatPham()
		return
	end
	RollItem(nId)
	TTHD_In("<color=green>Đã mở cửa sổ xúc xắc - mã phiên " .. nId .. ".<color>")
	TTHD_In("Soi bốn thứ: biểu tượng Hình nhân, chú giải khi rê chuột,")
	TTHD_In("đồng hồ đếm lùi, và hai nút Cần với Bỏ qua.")
end

function TTHD_XX_Goi(nId, nSo)
	if nSo > 0 then
		TTHD_In("<color=gold>[Xúc xắc] Phiên " .. nId .. ": bạn gieo được " .. nSo .. " điểm.<color>")
	else
		TTHD_In("<color=gold>[Xúc xắc] Phiên " .. nId .. ": bạn đã bỏ qua.<color>")
	end
end

function TTHD_XX_Chot(nId, nThang, nSo)
	TTHD_In("<color=yellow>===== Xúc xắc đã chốt =====<color>")
	TTHD_In("Mã phiên " .. nId .. " | Người thắng (chỉ số) " .. nThang .. " | Điểm " .. nSo)
	if nThang > 0 then
		TTHD_In("<color=green>Vật phẩm đã trao cho người thắng - kiểm túi đồ.<color>")
	else
		TTHD_In("Không ai nhận, vật phẩm đã được thu hồi (đúng thiết kế, không rò).")
	end
end

function TTHD_VP_KiemDLL()
	TTHD_In("<color=yellow>===== Tám hàm xúc xắc trong động cơ =====<color>")
	TTHD_In("   ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice"))
		.. " | AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo")))
	TTHD_In("   RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem"))
		.. " | GetItemDiceState: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceState")))
	TTHD_In("   DiceLootItem: " .. TTHD_CoKhong(TTHD_CoHam("DiceLootItem"))
		.. " | GetItemDiceRollInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceRollInfo")))
	TTHD_In("   GetItemDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceItemInfo"))
		.. " | GetItemDicePlayerList: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDicePlayerList")))
	TTHD_In("Có hàm nào ghi Không tức DLL đang chạy là bản cũ.")
	TTHD_VatPham()
end

-- ---------------------------------------------------------------------------
-- 5) BAN DO, NPC, CAU HINH  (4 muc + quay lai = 5 nut)
-- ---------------------------------------------------------------------------
function TTHD_BanDo()
	SayEx({"<color=yellow>Bản đồ, NPC và cấu hình<color>",
	"1. Đặt lại bốn NPC báo danh, gọi lặp an toàn/TTHD_BanDo_Npc",
	"2. Kiểm mọi bản đồ Viêm Đế đã nạp chưa/TTHD_BanDo_Kiem",
	"3. Xem cấu hình khối 7 đang hiệu lực/TTHD_BanDo_Cfg",
	"4. Xem vị trí hiện tại của bạn/TTHD_BanDo_ViTri",
	"Quay lại/TTHD_Root"})
end

function TTHD_BanDo_Npc()
	if not TTHD_CoHam("YDBZ_DriverInit") then
		TTHD_In("<color=red>Không thấy YDBZ_DriverInit.<color>")
		TTHD_BanDo()
		return
	end
	local nDat = YDBZ_DriverInit()
	TTHD_In("Đã đặt <color=gold>" .. nDat .. "<color> NPC báo danh.")
	TTHD_In("Hàm này tự dọn NPC cũ trước khi đặt lại nên gọi lặp không nhân bản.")
	TTHD_BanDo()
end

function TTHD_BanDo_Kiem()
	TTHD_In("<color=yellow>===== Bản đồ Viêm Đế =====<color>")
	local nMapNpc = HD_CFG("YDBZ_NPC_MAP", 37)
	TTHD_In("NPC báo danh, bản đồ " .. nMapNpc .. ": " .. TTHD_CoKhong(TTHD_CoMap(nMapNpc) == 1))
	local szThieu = ""
	local nCo = 0
	local i
	for i = 1003, 1017 do
		if TTHD_CoMap(i) == 1 then
			nCo = nCo + 1
		else
			szThieu = szThieu .. i .. " "
		end
	end
	TTHD_In("Phòng chờ 1003-1017: <color=gold>" .. nCo .. "/15<color>")
	if szThieu ~= "" then
		TTHD_In("<color=red>Thiếu: " .. szThieu .. "<color>")
	end
	szThieu = ""
	nCo = 0
	for i = 853, 862 do
		if TTHD_CoMap(i) == 1 then
			nCo = nCo + 1
		else
			szThieu = szThieu .. i .. " "
		end
	end
	TTHD_In("Bản đồ trận 853-862: <color=gold>" .. nCo .. "/10<color>")
	if szThieu ~= "" then
		TTHD_In("<color=red>Thiếu: " .. szThieu .. "<color>")
		TTHD_In("Khai trong settings\\MapList.ini và maps\\WorldSet_GameServer.ini rồi khởi động lại.")
	end
	TTHD_BanDo()
end

function TTHD_BanDo_Cfg()
	TTHD_In("<color=yellow>===== Cấu hình khối 7 =====<color>")
	TTHD_In("Sửa ở script\\header\\cauhinh_hoatdong.lua")
	TTHD_In("YDBZ_BAT = " .. HD_CFG("YDBZ_BAT", 1) .. " | YDBZ_GIO = " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("YDBZ_CAP_TOITHIEU = " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. " | YDBZ_PHONG_TOIDA = " .. HD_CFG("YDBZ_PHONG_TOIDA", 15))
	TTHD_In("YDBZ_LAN_NGAY = " .. HD_CFG("YDBZ_LAN_NGAY", 4)
		.. " | YDBZ_LAN_TUAN = " .. HD_CFG("YDBZ_LAN_TUAN", 10))
	TTHD_In("YDBZ_PHUT_BAODANH = " .. HD_CFG("YDBZ_PHUT_BAODANH", 5)
		.. " | YDBZ_PHUT_TRAN = " .. HD_CFG("YDBZ_PHUT_TRAN", 30))
	TTHD_In("<color=red>Lưu ý<color>: số người tổ đội (" .. YDBZ_TEAM_COUNT_LIMIT .. " tới "
		.. YDBZ_TEAM_COUNT_MAXLIMIT .. ") là số ghi cứng")
	TTHD_In("ở readymap\\include.lua dòng 25-26, không đọc từ cấu hình.")
	TTHD_BanDo()
end

function TTHD_BanDo_ViTri()
	local nW, nX, nY = GetWorldPos()
	TTHD_In("Bản đồ: <color=gold>" .. nW .. "<color> (" .. GetMapName(nW) .. ")")
	TTHD_In("Tọa độ ô: <color=gold>" .. nX .. ", " .. nY .. "<color> | Chỉ số người chơi: " .. PlayerIndex)
	TTHD_In("Tổ đội: <color=gold>" .. GetTeamSize() .. "<color> người | Cấp: <color=gold>"
		.. GetLevel() .. "<color>")
	TTHD_BanDo()
end
"""


def kiem_mat_dau(s):
    """TCVN3 chi co 7/67 nguyen am HOA co dau; hoa mang THANH DIEU thi MAT khi ghi."""
    mat = []
    for k, dong in enumerate(s.split("\n")):
        for c in dong:
            if ord(c) < 128:
                continue
            if bangtxt.uni2tcvn(c) == "":
                mat.append((k + 1, c, dong.strip()[:70]))
    return mat


def kiem_so_nut(s):
    """Client chi ve 6 nut mot luc - dem so muc trong moi SayEx."""
    ra = []
    import re
    for m in re.finditer(r"SayEx\(\{(.*?)\}\)", s, re.S):
        than = m.group(1)
        n = than.count('",') + (1 if than.rstrip().endswith('"') else 0)
        # tru dong dau (cau hoi, khong phai nut)
        nut = n - 1
        dong = s[:m.start()].count("\n") + 1
        if nut > 6:
            ra.append((dong, nut))
    return ra


def main():
    goc = LUA.lstrip("\n")

    mat = kiem_mat_dau(goc)
    if mat:
        print("!! %d ky tu KHONG ma hoa duoc sang TCVN3:" % len(mat))
        for dong, c, ngucanh in mat[:20]:
            print("   dong %-4d %r  trong: %s" % (dong, c, ngucanh))
        return 2
    print("   chot mat dau: khong ky tu nao bi roi")

    qua = kiem_so_nut(goc)
    if qua:
        print("!! menu qua 6 nut (client khong ve het):")
        for dong, n in qua:
            print("   dong %-4d co %d nut" % (dong, n))
        return 2
    print("   chot so nut: moi menu deu <= 6 nut")

    b = bangtxt.uni2tcvn(goc).replace("\r\n", "\n").encode("latin-1")
    for thumuc in (os.path.dirname(F_OUT), KHO):
        os.makedirs(thumuc, exist_ok=True)
    for p in (F_OUT, os.path.join(KHO, TEN)):
        if os.path.isfile(p):
            bak = p + HAU_TO
            if not os.path.isfile(bak):
                shutil.copy2(p, bak)
        tmp = p + ".tmp"
        io.open(tmp, "wb").write(b)
        os.replace(tmp, p)
    print("   > sinh %s: %d byte, %d dong, %d byte co dau"
          % (TEN, len(b), b.count(b"\n"), sum(1 for x in b if x > 127)))

    # doi lai nhan muc o menu goc cua lenh bai cho dung noi dung
    d = io.open(F_LB, "rb").read().decode("latin-1")
    cu = None
    for ln in d.split("\r\n"):
        if "TTHD_Root" in ln:
            cu = ln
            break
    NHAN = bangtxt.uni2tcvn("Bộ test Viêm Đế Bảo Tàng")
    moi = '\t\t"' + NHAN + '/TTHD_Root",'
    if cu is None:
        print("!! khong thay muc TTHD_Root trong lenhbaiadmin.lua")
        return 2
    if cu != moi:
        d = d.replace(cu, moi, 1)
        tmp = F_LB + ".tmp"
        io.open(tmp, "wb").write(d.encode("latin-1"))
        os.replace(tmp, F_LB)
        print("   > doi nhan muc menu goc thanh 'Bo test Viem De Bao Tang'")
    else:
        print("   = nhan menu goc da dung")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
