# -*- coding: utf-8 -*-
r"""v36 - sinh BO TEST TOAN BO HOAT DONG cho Lenh Bai Admin.

Sinh ra: script\item\test_hoatdong_admin.lua  (TCVN3, LF - dung quy uoc cua
cac tep item khac trong cay) va noi vao lenhbaiadmin.lua.

VI SAO SINH BANG CONG CU chu khong go tay: chu Viet phai la TCVN3 MOT BYTE.
Go tay bang trinh soan thao UTF-8 la hong het dau. bangtxt.uni2tcvn() chuyen
ca chuoi mot lan (KHONG chuyen tung ky tu - loi cu tung lam roi sach dau).

BA LUAT CUA MENU (rut ra tu cay nay):
  1. CAM dau "/" trong NHAN menu - ScriptFuns.cpp:717 cat o dau "/" dau tien,
     phan sau la TEN HAM. Nhan co "/" se bi hieu nham thanh ten ham.
  2. Nhan va ten ham cach nhau dung mot dau "/", khong khoang trang.
  3. lenhbaiadmin.lua main() dofile lai chinh no moi lan dung => sua tep nay
     KHONG CAN khoi dong lai GameServer.
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

# ---------------------------------------------------------------------------
# Noi dung tep - viet bang UNICODE co dau, se duoc chuyen sang TCVN3 khi ghi.
# ---------------------------------------------------------------------------
LUA = r"""
-- ============================================================================
-- TEST_HOATDONG_ADMIN.LUA - BO TEST TOAN BO HOAT DONG (Lenh Bai Admin)
-- SINH TU DONG boi ReverseTools\viemde\v36_gen_bo_test_admin.py - DUNG SUA TAY
-- (sua tay bang trinh soan thao UTF-8 se lam hong toan bo dau tieng Viet TCVN3)
--
-- Include tu lenhbaiadmin.lua; main() cua lenh bai dofile lai chinh no moi lan
-- dung => SUA TEP NAY KHONG CAN KHOI DONG LAI GameServer.
--
-- CAM dau "/" trong NHAN menu (ScriptFuns.cpp:717 cat o dau "/" dau tien).
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")

TTHD_PHIEN = "26/08/2026"

-- ---------------------------------------------------------------------------
-- Tien ich chung
-- ---------------------------------------------------------------------------

-- In mot dong bao cao ra khung chat cua chinh nguoi dang cam lenh bai.
function TTHD_In(szDong)
	Msg2Player(szDong)
end

-- Tra ve "CO" / "KHONG" theo dieu kien - cho bao cao de doc.
function TTHD_CoKhong(bDung)
	if bDung then
		return "<color=green>Có<color>"
	end
	return "<color=red>Không<color>"
end

-- Kiem mot ham Lua da nap chua. Dung de biet script/DLL da co tinh nang chua.
function TTHD_CoHam(szTen)
	return getglobal(szTen) ~= nil
end

-- Kiem mot ban do da nap vao may chu chua.
function TTHD_CoMap(nMapId)
	if SubWorldID2Idx == nil then
		return 0
	end
	if SubWorldID2Idx(nMapId) >= 0 then
		return 1
	end
	return 0
end

-- Doi 0825 thanh "08:25" cho de doc.
function TTHD_GioPhut(nHHMM)
	return format("%02d:%02d", floor(nHHMM / 100), mod(nHHMM, 100))
end

-- ---------------------------------------------------------------------------
-- MENU GOC
-- ---------------------------------------------------------------------------
function TTHD_Root()
	SayEx({"<color=yellow>Bộ test toàn bộ hoạt động<color> (bản " .. TTHD_PHIEN .. ") - chọn mục:",
	"1. Chẩn đoán nhanh tất cả hoạt động (một lượt quét)/TTHD_ChanDoan",
	"2. Viêm Đế Bảo Tàng (mới - chưa chạy lần nào)/TTHD_VD",
	"3. Tống Kim/TTHD_TK",
	"4. Ba hoạt động bản Linux: Sát Thủ, Phong Lăng Độ, Vượt ải/HD3_AdminMenu",
	"5. Bốn hoạt động PORT5: Bang Chiến, Bách Nhân, Tỷ Võ, Thành Bảo/HD_AdminMenu",
	"6. Liên Đấu/LD_AdminMenu",
	"7. Dã Tẩu/DT_AdminMenu",
	"8. Tín Sứ/HD_TS_Menu",
	"9. Tiện ích chung: thời gian, dịch chuyển, bản đồ/TTHD_TienIch",
	"10. Kiểm tra động cơ: các hàm engine mới có trong DLL chưa/TTHD_KiemEngine",
	"Kết thúc đối thoại./no"})
end

-- ---------------------------------------------------------------------------
-- 1) CHAN DOAN NHANH TAT CA
-- ---------------------------------------------------------------------------
function TTHD_ChanDoan()
	TTHD_In("<color=yellow>===== Chẩn đoán toàn bộ hoạt động =====<color>")
	local nGio, nPhut = TTHD_GioMayChu()
	TTHD_In(format("Giờ máy chủ hiện tại: <color=gold>%02d:%02d<color>", nGio, nPhut))
	TTHD_In("---")

	-- Viem De
	TTHD_In("<color=yellow>[1] Viêm Đế Bảo Tàng<color>")
	TTHD_In("    Bật trong cấu hình: " .. TTHD_CoKhong(HD_CFG("YDBZ_BAT", 1) == 1)
		.. " | Driver nạp: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_DriverInit"))
		.. " | Lịch nạp: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_Tick")))
	TTHD_In("    Hàm mở báo danh: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_OnTrigger"))
		.. " | Giờ chạy: " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("    Bản đồ NPC báo danh (" .. HD_CFG("YDBZ_NPC_MAP", 37) .. "): "
		.. TTHD_CoKhong(TTHD_CoMap(HD_CFG("YDBZ_NPC_MAP", 37)) == 1))
	local nCoMap = 0
	local i
	for i = 1003, 1017 do
		nCoMap = nCoMap + TTHD_CoMap(i)
	end
	TTHD_In("    Bản đồ phòng đấu 1003-1017 đã nạp: <color=gold>" .. nCoMap .. "/15<color>")

	-- He xuc xac
	TTHD_In("<color=yellow>[2] Hệ xúc xắc chia đồ (DICEITEM)<color>")
	TTHD_In("    ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice"))
		.. " | AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo"))
		.. " | RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem")))
	TTHD_In("    GetItemDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceItemInfo"))
		.. " | GetItemDiceRollInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceRollInfo")))
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("    <color=red>Chưa có trong DLL - cần thay CoreServer.dll mới rồi khởi động lại.<color>")
	end

	-- Ba hoat dong Linux
	TTHD_In("<color=yellow>[3] Ba hoạt động bản Linux<color>")
	TTHD_In("    Driver nạp: " .. TTHD_CoKhong(TTHD_CoHam("HD3_DriverInit"))
		.. " | Lịch nạp: " .. TTHD_CoKhong(TTHD_CoHam("HD3_Tick"))
		.. " | Menu test: " .. TTHD_CoKhong(TTHD_CoHam("HD3_AdminMenu")))

	-- Bon hoat dong PORT5
	TTHD_In("<color=yellow>[4] Bốn hoạt động PORT5<color>")
	TTHD_In("    Menu test: " .. TTHD_CoKhong(TTHD_CoHam("HD_AdminMenu"))
		.. " | Bang Chiến: " .. TTHD_CoKhong(TTHD_CoHam("HD_TW_Menu"))
		.. " | Thành Bảo: " .. TTHD_CoKhong(TTHD_CoHam("HD_TC_Menu")))

	-- Cac bo test khac
	TTHD_In("<color=yellow>[5] Các bộ test khác<color>")
	TTHD_In("    Liên Đấu: " .. TTHD_CoKhong(TTHD_CoHam("LD_AdminMenu"))
		.. " | Dã Tẩu: " .. TTHD_CoKhong(TTHD_CoHam("DT_AdminMenu"))
		.. " | Tín Sứ: " .. TTHD_CoKhong(TTHD_CoHam("HD_TS_Menu"))
		.. " | BOT: " .. TTHD_CoKhong(TTHD_CoHam("PB_Menu")))

	TTHD_In("<color=yellow>===== Hết chẩn đoán =====<color>")
	TTHD_Root()
end

-- Gio may chu, tra ve hai so.
function TTHD_GioMayChu()
	local szGio = GetLocalDate("%H")
	local szPhut = GetLocalDate("%M")
	return tonumber(szGio), tonumber(szPhut)
end

-- ---------------------------------------------------------------------------
-- 2) VIEM DE BAO TANG
-- ---------------------------------------------------------------------------
function TTHD_VD()
	SayEx({"<color=yellow>Viêm Đế Bảo Tàng<color> - vượt ải theo tổ đội, 3 đường x 10 ải, boss cuối Lương Mi Nhi",
	"1. Xem trạng thái đầy đủ/TTHD_VD_TrangThai",
	"2. Đặt lại 4 NPC báo danh (gọi lặp an toàn)/TTHD_VD_DatNpc",
	"3. Bắt đầu báo danh ngay (không chờ tới giờ)/TTHD_VD_EpChay",
	"4. Dịch chuyển tới NPC báo danh (Biện Kinh)/TTHD_VD_Tele",
	"5. Kiểm 15 bản đồ phòng đấu đã nạp chưa/TTHD_VD_KiemMap",
	"6. Nhận vật phẩm liên quan (Viêm Đế Lệnh, Hình nhân)/TTHD_VD_Item",
	"7. Test hệ xúc xắc ngay (không cần đánh boss)/TTHD_VD_XucXac",
	"8. Xem cấu hình khối 7 đang hiệu lực/TTHD_VD_Cfg",
	"Quay lại/TTHD_Root"})
end

function TTHD_VD_TrangThai()
	TTHD_In("<color=yellow>===== Trạng thái Viêm Đế =====<color>")
	TTHD_In("Bật trong cấu hình: " .. TTHD_CoKhong(HD_CFG("YDBZ_BAT", 1) == 1))
	TTHD_In("Giờ mở báo danh: <color=gold>" .. HD_CFG("YDBZ_GIO", "?") .. "<color>")
	local nGio, nPhut = TTHD_GioMayChu()
	TTHD_In(format("Giờ máy chủ: <color=gold>%02d:%02d<color>", nGio, nPhut))
	TTHD_In("Cấp tối thiểu: " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. " | Tổ đội: " .. HD_CFG("YDBZ_DOI_MIN", 4) .. " tới " .. HD_CFG("YDBZ_DOI_MAX", 6) .. " người")
	TTHD_In("Trần lượt: " .. HD_CFG("YDBZ_LAN_NGAY", 4) .. " lần một ngày, "
		.. HD_CFG("YDBZ_LAN_TUAN", 10) .. " lần một tuần")
	TTHD_In("Thời gian báo danh: " .. HD_CFG("YDBZ_PHUT_BAODANH", 5) .. " phút | Thời hạn trận: "
		.. HD_CFG("YDBZ_PHUT_TRAN", 30) .. " phút")
	TTHD_In("Số phòng tối đa: " .. HD_CFG("YDBZ_PHONG_TOIDA", 15))
	TTHD_In("Hàm YDBZ_OnTrigger: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_OnTrigger")))
	TTHD_In("Hàm YDBZ_DriverInit: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_DriverInit")))
	TTHD_VD()
end

function TTHD_VD_DatNpc()
	if not TTHD_CoHam("YDBZ_DriverInit") then
		TTHD_In("<color=red>Không thấy YDBZ_DriverInit - kiểm Include ydbz_driver.lua trong startgame.lua.<color>")
		TTHD_VD()
		return
	end
	local nDat = YDBZ_DriverInit()
	TTHD_In("Đã gọi YDBZ_DriverInit, đặt được <color=gold>" .. nDat .. "<color> NPC báo danh.")
	TTHD_In("Hàm này tự dọn NPC cũ của hệ trước khi đặt lại, nên gọi lặp không nhân bản NPC.")
	TTHD_VD()
end

function TTHD_VD_EpChay()
	if not TTHD_CoHam("YDBZ_OnTrigger") then
		TTHD_In("<color=red>Không thấy YDBZ_OnTrigger - kiểm Include yandibaozang_trigger.lua.<color>")
		TTHD_VD()
		return
	end
	YDBZ_OnTrigger()
	TTHD_In("<color=green>Đã mở báo danh Viêm Đế ngay bây giờ.<color>")
	TTHD_In("Tới gặp Bình Bình cô nương ở Biện Kinh để báo danh. Cần đủ "
		.. HD_CFG("YDBZ_DOI_MIN", 4) .. " người trong tổ đội.")
	TTHD_VD()
end

function TTHD_VD_Tele()
	NewWorld(HD_CFG("YDBZ_NPC_MAP", 37), 1714, 3173)
	TTHD_In("Đã dịch chuyển tới chỗ NPC báo danh thứ nhất (Biện Kinh).")
	TTHD_In("Ba chỗ còn lại: (1642,3145) (1622,3019) (1857,2968).")
end

function TTHD_VD_KiemMap()
	TTHD_In("<color=yellow>===== Bản đồ Viêm Đế =====<color>")
	local nMapNpc = HD_CFG("YDBZ_NPC_MAP", 37)
	TTHD_In("Bản đồ NPC báo danh " .. nMapNpc .. ": " .. TTHD_CoKhong(TTHD_CoMap(nMapNpc) == 1))
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
	TTHD_In("Phòng đấu 1003-1017: có <color=gold>" .. nCo .. "/15<color>")
	if szThieu ~= "" then
		TTHD_In("<color=red>Thiếu: " .. szThieu .. "<color>")
		TTHD_In("Khai bản đồ trong settings\\MapList.ini và maps\\WorldSet_GameServer.ini rồi khởi động lại.")
	end
	-- ban do neo 852 va cac ban do tran 851-862
	local szTran = ""
	for i = 851, 862 do
		if TTHD_CoMap(i) == 0 then
			szTran = szTran .. i .. " "
		end
	end
	if szTran == "" then
		TTHD_In("Bản đồ trận 851-862: <color=green>đủ<color>")
	else
		TTHD_In("<color=red>Bản đồ trận thiếu: " .. szTran .. "<color>")
	end
	TTHD_VD()
end

function TTHD_VD_Item()
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1615, 1, 0, 0, 0)
	TTHD_In("Đã nhận: 2 Hình nhân (6,1,1614) và 1 Viêm Đế Đồ Đằng (6,1,1615).")
	TTHD_In("Hình nhân là vật phẩm rơi qua hệ xúc xắc ở mọi boss Viêm Đế.")
	TTHD_VD()
end

-- ---------------------------------------------------------------------------
-- 7) TEST HE XUC XAC NGAY - khong can danh boss
--
-- Day la duong test end-to-end cua CA HE: engine tao phien, tao vat pham that,
-- gui goi s2c_diceitem sang client, client mo cua so, bam nut gui c2s_diceitem
-- ve, engine gieo diem roi goi lai hai ham duoi day.
--
-- LUU Y ve tham so szFile: phai la tep ma engine THUC SU NAP va la noi hai ham
-- goi lai ton tai. O day dung LENHBAIADMIN chu khong phai chinh tep nay, vi:
--   * lenhbaiadmin.lua CHAC CHAN da duoc nap (no dang chay khi ban bam lenh bai)
--   * no Include tep nay, ma Include cua JX1 = lua_dofile vao trang thai cua tep
--     GOI (ScriptFuns.cpp:1969), nen TTHD_XX_Goi / TTHD_XX_Chot nam trong trang
--     thai Lua cua lenhbaiadmin.lua
-- Neu truyen duong dan chinh tep nay ma engine chua tung nap no thanh mot script
-- rieng thi g_GetScript tra NULL, goi lai im lang, chi con lai mot dong
-- "KItemDice: khong tim thay kich ban" trong nhat ky may chu.
-- ---------------------------------------------------------------------------
TTHD_XX_TEP = "\\script\\item\\lenhbaiadmin.lua"

function TTHD_VD_XucXac()
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("<color=red>Động cơ chưa có hàm ApplyItemDice.<color>")
		TTHD_In("Phải thay CoreServer.dll mới rồi khởi động lại máy chủ.")
		TTHD_VD()
		return
	end

	-- 1 nguoi (chinh minh), gieo 1..100, cho 30 giay
	local nId = ApplyItemDice(1, 100, 30, TTHD_XX_TEP, "TTHD_XX_Chot", "TTHD_XX_Goi", 1)
	if nId == nil or nId <= 0 then
		TTHD_In("<color=red>ApplyItemDice trả về " .. tostring(nId) .. " - không tạo được phiên.<color>")
		TTHD_In("Xem nhật ký máy chủ tìm chuỗi KItemDice: để biết lý do.")
		TTHD_VD()
		return
	end

	-- Vat pham dem ra chia: Hinh nhan (6,1,1614) - dung thu roi o Viem De.
	-- Tham so: (nId, phamChat, chungLoai, loai, chiTiet, cap, nguHanh, hatGiong, mayMan)
	local nItem = AddDiceItemInfo(nId, 0, 6, 1, 1614, 1, 1, 1, 1)
	if nItem == nil or nItem <= 0 then
		TTHD_In("<color=red>AddDiceItemInfo trả về " .. tostring(nItem) .. " - không tạo được vật phẩm.<color>")
		TTHD_VD()
		return
	end

	RollItem(nId)
	TTHD_In("<color=green>Đã mở cửa sổ xúc xắc, mã phiên " .. nId .. ", vật phẩm " .. nItem .. ".<color>")
	TTHD_In("Hãy soi bốn thứ: ô vật phẩm có hiện hình không, rê chuột có ra chú giải không,")
	TTHD_In("đồng hồ có đếm lùi không, và hai nút có bấm được không.")
	TTHD_In("Bấm <color=yellow>Tham dự nhận<color> để gieo điểm, hoặc <color=yellow>Hủy bỏ nhận<color> để bỏ qua.")
end

-- Goi moi lan MOT nguoi gieo xong. Chay voi PlayerIndex la nguoi vua gieo.
function TTHD_XX_Goi(nId, nSo)
	if nSo > 0 then
		TTHD_In("<color=gold>[Xúc xắc] Phiên " .. nId .. ": bạn gieo được " .. nSo .. " điểm.<color>")
	else
		TTHD_In("<color=gold>[Xúc xắc] Phiên " .. nId .. ": bạn đã hủy bỏ nhận.<color>")
	end
end

-- Goi khi chot phien: moi nguoi da chon xong, hoac het gio.
function TTHD_XX_Chot(nId, nThang, nSo)
	TTHD_In("<color=yellow>===== Xúc xắc đã chốt =====<color>")
	TTHD_In("Mã phiên: " .. nId .. " | Người thắng (chỉ số): " .. nThang .. " | Điểm: " .. nSo)
	if nThang > 0 then
		TTHD_In("<color=green>Vật phẩm đã được trao cho người thắng - kiểm túi đồ.<color>")
	else
		TTHD_In("Không ai nhận, vật phẩm đã được thu hồi (đúng thiết kế, không rò).")
	end
	-- Doc lai thong tin de kiem ba ham truy van
	if TTHD_CoHam("GetItemDiceItemInfo") then
		local a, b, c, d, e, f, g, h = GetItemDiceItemInfo(nId)
		if a ~= nil then
			TTHD_In("GetItemDiceItemInfo: chỉ số " .. a .. ", tên " .. tostring(b))
		else
			TTHD_In("GetItemDiceItemInfo trả về rỗng - phiên đã đóng, đúng như mong đợi.")
		end
	end
end

function TTHD_VD_Cfg()
	TTHD_In("<color=yellow>===== Cấu hình khối 7 (Viêm Đế) =====<color>")
	TTHD_In("Sửa ở: script\\header\\cauhinh_hoatdong.lua")
	TTHD_In("YDBZ_BAT = " .. HD_CFG("YDBZ_BAT", 1) .. "  (0 là tắt hẳn, cần khởi động lại)")
	TTHD_In("YDBZ_GIO = " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("YDBZ_CAP_TOITHIEU = " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120))
	TTHD_In("YDBZ_LAN_NGAY = " .. HD_CFG("YDBZ_LAN_NGAY", 4)
		.. " | YDBZ_LAN_TUAN = " .. HD_CFG("YDBZ_LAN_TUAN", 10))
	TTHD_In("YDBZ_DOI_MIN = " .. HD_CFG("YDBZ_DOI_MIN", 4)
		.. " | YDBZ_DOI_MAX = " .. HD_CFG("YDBZ_DOI_MAX", 6))
	TTHD_In("YDBZ_PHONG_TOIDA = " .. HD_CFG("YDBZ_PHONG_TOIDA", 15) .. "  (cần khởi động lại)")
	TTHD_In("YDBZ_PHUT_BAODANH = " .. HD_CFG("YDBZ_PHUT_BAODANH", 5) .. "  (cần khởi động lại)")
	TTHD_In("YDBZ_PHUT_TRAN = " .. HD_CFG("YDBZ_PHUT_TRAN", 30))
	TTHD_VD()
end

-- ---------------------------------------------------------------------------
-- 3) TONG KIM
-- ---------------------------------------------------------------------------
function TTHD_TK()
	SayEx({"<color=yellow>Tống Kim<color> - chiến trường 324, bot tự tham gia",
	"1. Dịch chuyển vào chiến trường Tống Kim (bản đồ 324)/TTHD_TK_Tele",
	"2. Nhận Chiêu Thư Tống Kim (vé vào)/TTHD_TK_Ve",
	"3. Bật BOT Tống Kim/HD_TK_Bat",
	"4. Tắt BOT Tống Kim/HD_TK_Tat",
	"5. Xem bản đồ 324 và 380 đã nạp chưa/TTHD_TK_KiemMap",
	"Quay lại/TTHD_Root"})
end

function TTHD_TK_Tele()
	if TTHD_CoMap(324) == 0 then
		TTHD_In("<color=red>Bản đồ 324 chưa nạp.<color>")
		return
	end
	NewWorld(324, 1566, 3084)
	TTHD_In("Đã dịch chuyển vào bản đồ 324 (điểm vòng Kim).")
end

function TTHD_TK_Ve()
	AddItem(6, 1, 154, 1, 0, 0, 0)
	TTHD_In("Đã nhận Chiêu Thư Tống Kim (6,1,154).")
	TTHD_TK()
end

function TTHD_TK_KiemMap()
	TTHD_In("Bản đồ 324: " .. TTHD_CoKhong(TTHD_CoMap(324) == 1)
		.. " | Bản đồ 380: " .. TTHD_CoKhong(TTHD_CoMap(380) == 1))
	TTHD_TK()
end

-- ---------------------------------------------------------------------------
-- 9) TIEN ICH CHUNG
-- ---------------------------------------------------------------------------
function TTHD_TienIch()
	SayEx({"<color=yellow>Tiện ích chung<color>",
	"1. Xem giờ máy chủ và ngày tháng/TTHD_TI_Gio",
	"2. Xem vị trí hiện tại (bản đồ, tọa độ)/TTHD_TI_ViTri",
	"3. Xem tổ đội hiện tại/TTHD_TI_ToDoi",
	"4. Về Biện Kinh/TTHD_TI_VeBienKinh",
	"Quay lại/TTHD_Root"})
end

function TTHD_TI_Gio()
	TTHD_In("Ngày giờ máy chủ: <color=gold>" .. GetLocalDate("%y-%m-%d %H:%M:%S") .. "<color>")
	TTHD_In("Lưu ý: lịch các hoạt động tính theo giờ này, không phải giờ máy của bạn.")
	TTHD_TienIch()
end

function TTHD_TI_ViTri()
	local nW, nX, nY = GetWorldPos()
	TTHD_In("Bản đồ: <color=gold>" .. nW .. "<color> (" .. GetMapName(nW) .. ")")
	TTHD_In("Tọa độ ô: <color=gold>" .. nX .. ", " .. nY .. "<color>")
	TTHD_In("Chỉ số người chơi: " .. PlayerIndex)
	TTHD_TienIch()
end

function TTHD_TI_ToDoi()
	local nSo = GetTeamSize()
	TTHD_In("Số người trong tổ đội: <color=gold>" .. nSo .. "<color>")
	if nSo > 0 then
		local nCu = PlayerIndex
		local i
		for i = 1, nSo do
			PlayerIndex = GetTeamMember(i)
			TTHD_In("   " .. i .. ". " .. GetName() .. " (chỉ số " .. PlayerIndex .. ")")
		end
		PlayerIndex = nCu
	end
	TTHD_TienIch()
end

function TTHD_TI_VeBienKinh()
	NewWorld(37, 1714, 3173)
	TTHD_In("Đã về Biện Kinh.")
end

-- ---------------------------------------------------------------------------
-- 10) KIEM DONG CO - cac ham engine moi da co trong DLL chua
-- ---------------------------------------------------------------------------
function TTHD_KiemEngine()
	TTHD_In("<color=yellow>===== Kiểm động cơ (CoreServer.dll) =====<color>")
	TTHD_In("Hệ xúc xắc chia đồ, 8 hàm:")
	TTHD_In("   ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice")))
	TTHD_In("   AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo")))
	TTHD_In("   RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem")))
	TTHD_In("   GetItemDiceState: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceState")))
	TTHD_In("   DiceLootItem: " .. TTHD_CoKhong(TTHD_CoHam("DiceLootItem")))
	TTHD_In("   GetItemDiceRollInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceRollInfo")))
	TTHD_In("   GetItemDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceItemInfo")))
	TTHD_In("   GetItemDicePlayerList: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDicePlayerList")))
	TTHD_In("Hệ hẹn giờ JX2 (PORT5):")
	TTHD_In("   AddTimer: " .. TTHD_CoKhong(TTHD_CoHam("AddTimer"))
		.. " | DelTimer: " .. TTHD_CoKhong(TTHD_CoHam("DelTimer")))
	TTHD_In("Nếu có hàm nào ghi Không, tức DLL đang chạy là bản cũ - thay CoreServer.dll rồi khởi động lại.")
	TTHD_Root()
end
"""


def kiem_mat_dau(s):
    """TCVN3 chi co 7/67 nguyen am HOA co dau (Â Ê Ô Ă Đ Ơ Ư); hoa mang THANH DIEU
    thi MAT KHI GHI. Chot nay so vong tron uni -> tcvn -> uni, bao dung ky tu nao
    bi roi va o dong nao, de khong bao gio lang le mat dau nua."""
    mat = []
    for k, dong in enumerate(s.split("\n")):
        for c in dong:
            if ord(c) < 128:
                continue
            if bangtxt.uni2tcvn(c) == "":
                mat.append((k + 1, c, dong.strip()[:70]))
    return mat


def main():
    goc = LUA.lstrip("\n")
    mat = kiem_mat_dau(goc)
    if mat:
        print("!! %d ky tu KHONG ma hoa duoc sang TCVN3 - se bi mat dau:" % len(mat))
        for dong, c, ngucanh in mat[:20]:
            print("   dong %-4d ky tu %r  trong: %s" % (dong, c, ngucanh))
        print("   (TCVN3 chi co 7/67 nguyen am HOA co dau - viet chu thuong di)")
        return 2
    print("   chot mat dau: khong ky tu nao bi roi")
    tcvn = bangtxt.uni2tcvn(goc)
    # tep item trong cay nay dung LF (giong hoatdong_admin.lua)
    b = tcvn.replace("\r\n", "\n").encode("latin-1")

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
    print("   > sinh %s: %d byte, %d dong LF, %d byte co dau"
          % (TEN, len(b), b.count(b"\n"), sum(1 for x in b if x > 127)))

    # noi vao lenhbaiadmin.lua
    d = io.open(F_LB, "rb").read().decode("latin-1")
    NEO = 'Include("\\\\script\\\\item\\\\hd3_admin.lua")'
    if TEN in d:
        print("   = lenhbaiadmin.lua da Include san")
    else:
        i = d.find(NEO)
        if i < 0:
            print("!! khong tim thay mo neo Include hd3_admin.lua")
            return 2
        cuoi = d.find("\r\n", i)
        if cuoi < 0:
            print("!! lenhbaiadmin.lua khong phai CRLF?")
            return 2
        dong = d[i:cuoi]
        moi = (dong + "\r\n"
               + 'Include("\\\\script\\\\item\\\\test_hoatdong_admin.lua")'
               + "\t-- [26/08] BO TEST TOAN BO HOAT DONG")
        bak = F_LB + HAU_TO
        if not os.path.isfile(bak):
            shutil.copy2(F_LB, bak)
        d2 = d.replace(dong, moi, 1)
        tmp = F_LB + ".tmp"
        io.open(tmp, "wb").write(d2.encode("latin-1"))
        os.replace(tmp, F_LB)
        print("   > lenhbaiadmin.lua: them Include")

    # them muc vao menu goc cua lenh bai
    d = io.open(F_LB, "rb").read().decode("latin-1")
    NHAN = bangtxt.uni2tcvn("Bộ test toàn bộ hoạt động (26.08)")
    if "TTHD_Root" in d:
        print("   = menu goc da co muc TTHD_Root")
    else:
        NEO2 = '\t\t"K'
        i = d.find('/HD3_AdminMenu"')
        if i < 0:
            print("!! khong tim thay muc HD3_AdminMenu trong menu goc")
            return 2
        cuoi = d.find("\r\n", i)
        dong = d[d.rfind("\r\n", 0, i) + 2:cuoi]
        moi = dong + "\r\n\t\t\"" + NHAN + "/TTHD_Root\","
        d2 = d.replace(dong, moi, 1)
        tmp = F_LB + ".tmp"
        io.open(tmp, "wb").write(d2.encode("latin-1"))
        os.replace(tmp, F_LB)
        print("   > lenhbaiadmin.lua: them muc menu goc")

    print()
    d = io.open(F_LB, "rb").read().decode("latin-1")
    print("   lenhbaiadmin.lua Include: %s | muc menu: %s"
          % ("CO" if TEN in d else "KHONG", "CO" if "TTHD_Root" in d else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
