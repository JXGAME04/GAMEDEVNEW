-- [HE THONG] lib\hoithoai_54.lua - HOI THOAI NPC NHIEU BUOC BANG COROUTINE (Lua 5.4) - 06/09/2026 (PA-4, PHUONGAN_LUA54_SCRIPT_0609.md C7)
-- ============================================================================================================
-- Cach cu (Lua 4): moi buoc hoi thoai = mot ham toan cuc rieng + bien tam toan cuc de giu trang thai:
--     function main()   Say("Chao", 2, "Mua/mua", "Thoat/thoat") end
--     function mua()    Say("Mua gi?", 2, "A/mua_a", "B/mua_b") end
--     function mua_a()  ... end
-- Cach moi: viet TUYEN TINH trong mot ham, SayWait tam dung (coroutine.yield) cho toi khi nguoi choi chon:
--     Include("\\script\\lib\\hoithoai_54.lua")
--     function main()
--         HoiThoai(function()
--             local c = SayWait("Chao", {"Mua", "Thoat"})       -- c = 1 / 2; 0 = dong hop thoai
--             if c == 1 then
--                 local m = SayWait("Mua gi?", {"A", "B"})
--                 ...
--             end
--         end)
--     end
-- Co che: SayWait goi Say() voi cac lua chon tro toi HT_Chon1..HT_Chon12 (ham toan cuc trong state cua script goi);
-- engine (KPlayer::ProcessPlayerSelectFromUI) goi ExecuteScript(script NPC, "HT_ChonK", k) trong CUNG state ->
-- HT_ChonK resume coroutine dang cho cua nguoi choi (PlayerIndex do engine dat truoc khi goi).
-- Gioi han: 12 lua chon / buoc; moi nguoi choi mot coroutine dang cho (buoc moi thay buoc cu); nguoi choi dong hop
-- thoai thi coroutine nam cho toi khi bi thay the (khong tich luy qua MAX_PLAYER). Loi trong than -> _ERRORMESSAGE (ScriptError.log).
-- Ham C goi tu trong coroutine nhan lua_State cua coroutine: Lua54Dll (lua4_owner) + KSortScript da xu ly (06/09 toi).
-- ============================================================================================================
HT_MAX_CHON = 12
HT_Co = HT_Co or {}          -- PlayerIndex -> coroutine dang cho

local function HT_BaoLoi(msg)
	local em = _ERRORMESSAGE
	if type(em) == "function" then
		em("[HoiThoai] " .. tostring(msg) .. "\n" .. (debug and debug.traceback and debug.traceback("", 2) or ""))
	end
	return msg
end

-- chay tiep coroutine cua nguoi choi hien tai voi gia tri tra ve cua SayWait
local function HT_Tiep(co, ...)
	local ok, err = coroutine.resume(co, ...)
	if not ok then
		HT_BaoLoi(err)
		HT_Co[PlayerIndex] = nil
		return
	end
	if coroutine.status(co) == "suspended" then
		HT_Co[PlayerIndex] = co
	else
		if HT_Co[PlayerIndex] == co then HT_Co[PlayerIndex] = nil end
	end
end

-- Bat dau mot hoi thoai: f chay trong coroutine; ben trong dung SayWait / TalkWait
function HoiThoai(f, ...)
	if type(f) ~= "function" then
		HT_BaoLoi("HoiThoai: can ham")
		return
	end
	local co = coroutine.create(f)
	HT_Co[PlayerIndex] = nil
	HT_Tiep(co, ...)
end

-- SayWait(noi_dung, {lua chon 1, lua chon 2, ...}) -> chi so da chon (1..n). Goi duoc CHI trong HoiThoai.
-- noi_dung co the la chuoi hoac so (id cau noi nhu Say).
function SayWait(szText, tbChon)
	local co, chinh = coroutine.running()
	if chinh or co == nil then
		HT_BaoLoi("SayWait: phai goi trong HoiThoai(function() ... end)")
		return 0
	end
	tbChon = tbChon or {}
	local n = tbChon.n or #tbChon
	if n > HT_MAX_CHON then n = HT_MAX_CHON end
	local tbOpt = {}
	for i = 1, n do
		tbOpt[i] = tostring(tbChon[i]) .. "/HT_Chon" .. i
	end
	HT_Co[PlayerIndex] = co
	Say(szText, n, table.unpack(tbOpt, 1, n))
	return coroutine.yield()
end

-- TalkWait({"cau 1", "cau 2", ...}) : noi nhieu cau roi cho nguoi choi bam tiep (Talk voi callback)
function TalkWait(tbCau)
	local co, chinh = coroutine.running()
	if chinh or co == nil then
		HT_BaoLoi("TalkWait: phai goi trong HoiThoai(function() ... end)")
		return 0
	end
	tbCau = tbCau or {}
	local n = tbCau.n or #tbCau
	HT_Co[PlayerIndex] = co
	Talk(n, "HT_Chon1", table.unpack(tbCau, 1, n))
	return coroutine.yield()
end

-- cac callback engine goi theo ten (phai la ham toan cuc trong state cua script NPC)
local function HT_Chon(k)
	local co = HT_Co[PlayerIndex]
	if co == nil then return end
	HT_Co[PlayerIndex] = nil
	if coroutine.status(co) ~= "suspended" then return end
	HT_Tiep(co, k)
end
function HT_Chon1() HT_Chon(1) end
function HT_Chon2() HT_Chon(2) end
function HT_Chon3() HT_Chon(3) end
function HT_Chon4() HT_Chon(4) end
function HT_Chon5() HT_Chon(5) end
function HT_Chon6() HT_Chon(6) end
function HT_Chon7() HT_Chon(7) end
function HT_Chon8() HT_Chon(8) end
function HT_Chon9() HT_Chon(9) end
function HT_Chon10() HT_Chon(10) end
function HT_Chon11() HT_Chon(11) end
function HT_Chon12() HT_Chon(12) end
