-- [HE THONG] lib\lib_54.lua - TIEN ICH LUA 5.4 CHO SCRIPT (06/09/2026, PHANTICH_NANG_LUA54_0509.md muc 14.3)
-- @IncludeOnce  (tep toan ham, khong bien cap tep -> Lua54Dll chi chay than mot lan moi state)
-- ============================================================================================================
-- Thu(f, ...)          : goi f co bao ve; loi -> _ERRORMESSAGE (ScriptError.log) kem traceback; tra (ok, ket qua...)
--                        thay cho call(f, {args}, "x") cua Lua 4 (nhanh ~20 lan, khong tao bang doi so).
-- GhepChuoi(tb, sep)   : table.concat co ep tostring (thay s = s .. x trong vong lap: O(n) thay O(n^2)).
-- ChiaChuoi(s, sep)    : tach chuoi theo ky tu/chuoi sep -> bang (thay vong strfind/strsub).
-- Bit_Co(x, k) / Bit_Dat(x, k) / Bit_Xoa(x, k): bit thu k (0..62) bang toan tu bit 5.4.
-- ============================================================================================================
function L54_BaoLoi(msg)
	local em = _ERRORMESSAGE
	if type(em) == "function" then
		em(tostring(msg) .. "\n" .. (debug and debug.traceback and debug.traceback("", 2) or ""))
	end
	return msg
end

function Thu(f, ...)
	return xpcall(f, L54_BaoLoi, ...)
end

function GhepChuoi(tb, sep, i, j)
	local n = j or tb.n or #tb
	local t = {}
	for k = (i or 1), n do
		t[#t + 1] = tostring(tb[k])
	end
	return table.concat(t, sep or "")
end

function ChiaChuoi(s, sep)
	local t = {}
	if s == nil then return t end
	s = tostring(s)
	sep = sep or ","
	local pos = 1
	while true do
		local a, b = string.find(s, sep, pos, true)
		if not a then
			t[#t + 1] = string.sub(s, pos)
			break
		end
		t[#t + 1] = string.sub(s, pos, a - 1)
		pos = b + 1
	end
	return t
end

function Bit_Co(x, k)
	return ((math.tointeger(x) or 0) >> k) & 1
end

function Bit_Dat(x, k)
	return (math.tointeger(x) or 0) | (1 << k)
end

function Bit_Xoa(x, k)
	return (math.tointeger(x) or 0) & ~(1 << k)
end
