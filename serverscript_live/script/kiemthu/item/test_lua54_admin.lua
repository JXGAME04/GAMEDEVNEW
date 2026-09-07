-- kiemthu\item\test_lua54_admin.lua - THU NGHIEM LUA 5.4 qua lenh bai admin (06/09/2026, PA-4 SayWait + tien ich lib_54)
-- Duoc Include tu vatpham\lenhbaiadmin.lua; muc menu "Thu nghiem Lua 5.4/L54_MenuThu".
-- Chu y: ham goi tu menu chay trong state cua lenhbaiadmin (Say callback ve script cua vat pham).
Include("\\script\\lib\\hoithoai_54.lua")
Include("\\script\\lib\\lib_54.lua")

function L54_MenuThu()
	SayEx({"Thu nghiem Lua 5.4 (06/09 toi):",
		"Hoi thoai coroutine SayWait (3 buoc trong MOT ham)/L54_ThuHoiThoai",
		"Tien ich lib_54: Thu / GhepChuoi / ChiaChuoi / Bit/L54_ThuLib",
		"Thong tin phien ban Lua + che do mot state/L54_ThongTin",
		"Thoat/L54_Thoat"})
end

function L54_Thoat()
end

-- PA-4: mot hoi thoai 3 buoc viet tuyen tinh, khong can 3 ham callback rieng
function L54_ThuHoiThoai()
	HoiThoai(function()
		local c = SayWait("SayWait buoc 1: ban muon thu gi?", {"Chon so 1..3", "Doi thoai nhieu cau (TalkWait)", "Thoat"})
		if c == 1 then
			local n = SayWait("Buoc 2: chon mot so", {"Mot", "Hai", "Ba"})
			local m = SayWait(format("Buoc 3: ban vua chon %d. Xac nhan?", n), {"Dung", "Sai"})
			Msg2Player(format("[L54] Ket qua: buoc 2 = %d, buoc 3 = %d (coroutine ket thuc)", n, m))
		elseif c == 2 then
			TalkWait({"Cau 1: TalkWait dung Talk() roi cho bam tiep.", "Cau 2: sau cau nay quay ve SayWait."})
			local k = SayWait("Da xem xong 2 cau. Lam lai?", {"Lam lai", "Thoi"})
			if k == 1 then
				return L54_ThuHoiThoai()
			end
			Msg2Player("[L54] TalkWait xong.")
		else
			Msg2Player("[L54] Thoat hoi thoai.")
		end
	end)
end

function L54_ThuLib()
	local ok, r = Thu(function(a, b) return a + b end, 2, 3)
	local ok2 = Thu(function() error("loi co chu y") end)
	local s = GhepChuoi({1, "hai", 3.5}, "|")
	local t = ChiaChuoi("a,b,,c", ",")
	local x = Bit_Dat(0, 5)
	Msg2Player(format("[L54] Thu: ok=%s r=%s; Thu loi: ok=%s (xem ScriptError.log); GhepChuoi=%s; ChiaChuoi=%d phan; Bit_Dat(0,5)=%d Bit_Co=%d Bit_Xoa=%d",
		tostring(ok), tostring(r), tostring(ok2), s, #t, x, Bit_Co(x, 5), Bit_Xoa(x, 5)))
end

function L54_ThongTin()
	local co, chinh = coroutine.running()
	Msg2Player(format("[L54] %s; _VERSION=%s; math.maxinteger=%d; 7//2=%d; state chinh=%s; LUA4COMPAT=%s",
		tostring(LUA4COMPAT_VERSION), tostring(_VERSION), math.maxinteger, 7 // 2, tostring(chinh), tostring(rawget(_G, "L4_Env") ~= nil)))
end
