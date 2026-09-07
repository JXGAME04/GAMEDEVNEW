-- ============================================================================
-- lua4compat.lua - [LUA54 05/09] LOP TUONG THICH Lua 4.0 tren Lua 5.4.
--
-- Nap MOT LAN vao moi lua_State ngay sau luaL_openlibs (Lua54Dll: lua4_baselibopen).
-- Muc dich: 3.435 tep script viet cho Lua 4.0.1 chay tren 5.4 ma KHONG phai sua
-- ten ham thu vien (13.950 cho goi: floor, getn, format, tinsert, strfind, date, ...).
-- Moi ham tai tao DUNG ngu nghia Lua 4.0.1 (doc tu Sources\Library\LuaLib\src\baselib):
--   * getn(t): t.n neu la so, khong thi chi so so lon nhat co gia tri (lapi.c lua_getn)
--   * tinsert/tremove: LUON ghi t.n (lbaselib.c luaB_tinsert/tremove)
--   * call(f, args, "x", handler): loi -> tra nil, goi handler (mac dinh _ERRORMESSAGE)
--   * dostring/dofile: loi -> _ERRORMESSAGE roi tra nil (passresults)
--   * tag method (newtag/settag/settagmethod...) -> metatable
--   * format: %d/%i/%c/%x/%X/%o/%u nhan so thuc (Lua 4 ep (int); 5.4 se loi neu khong nguyen)
--   * random(): giong Lua 4 (random() -> [0,1), random(n) -> 1..n, random(a,b)), doi so thuc duoc ep nguyen
--   * I/O kieu Lua 4: openfile/closefile/read/write/readfrom/writeto/appendto/seek/flush ...
-- Tep nay la ASCII thuan, KHONG co chu Viet.
-- ============================================================================

local _G = _G
local string, table, math, os, io = string, table, math, os, io
local rawget, rawset, type, select, tostring, tonumber = rawget, rawset, type, select, tostring, tonumber
local pcall, xpcall, error, load, loadfile, setmetatable, getmetatable = pcall, xpcall, error, load, loadfile, setmetatable, getmetatable
local collectgarbage_54 = collectgarbage	-- [LUA54 06/09] giu ban goc: ban shim cu goi lai chinh no -> tran ngan xep (gcinfo/collectgarbage)
local unpack = table.unpack
local mfloor = math.floor

_G.LUA4COMPAT_VERSION = "1.0 (05/09/2026)"

-- ---------------------------------------------------------------------------
-- 1. Bao loi kieu Lua 4: _ERRORMESSAGE(msg) -> _ALERT(msg). Engine dang ky _ALERT (C)
--    ghi ScriptError.log. Neu script dinh nghia lai _ERRORMESSAGE thi call()/dofile ton trong.
-- ---------------------------------------------------------------------------
if _G._ALERT == nil then
	_G._ALERT = function(msg)
		io.stderr:write(tostring(msg), "\n")
	end
end
_G._ERRORMESSAGE = function(msg)
	local a = _G._ALERT
	if type(a) == "function" then
		a("error: " .. tostring(msg))
	end
end

-- [LUA54 06/09 toi] moi truong toan cuc cua SCRIPT dang goi ham shim: che do thuong = _G; che do MOT STATE
-- (LUA54_MOT_STATE=1) = bang E rieng cua script. L4_Env(lv) do Lua54Dll dang ky, lv dem tu ham goi L4_Env:
-- 1 = moiTruong, 2 = ham shim (getglobal, call, dostring...), 3 = script. Chi dung TRUC TIEP trong ham shim.
local function moiTruong()
	local f = _G.L4_Env
	if f ~= nil then return f(3) end
	return _G
end

local function baoLoi(msg, env)
	local em = (env or _G)._ERRORMESSAGE
	if type(em) == "function" then
		pcall(em, msg)
	end
end

-- Ham xu ly loi cho xpcall: them traceback nhu Lua 4 (_ALERT nhan ca stack)
local function themTraceback(msg)
	if type(msg) ~= "string" then
		msg = tostring(msg)
	end
	return debug and debug.traceback and debug.traceback(msg, 2) or msg
end

-- ---------------------------------------------------------------------------
-- 2. Bang: getn / tinsert / tremove / sort / foreach / foreachi / next
-- ---------------------------------------------------------------------------
local function getn(t)
	if type(t) ~= "table" then
		error("bad argument #1 to `getn' (table expected)", 2)
	end
	local n = rawget(t, "n")
	if type(n) == "number" then
		return n
	end
	local max = 0
	for k, v in next, t do
		if type(k) == "number" and v ~= nil and k > max then
			max = k
		end
	end
	return max
end
_G.getn = getn

_G.tinsert = function(t, ...)
	if type(t) ~= "table" then
		error("bad argument #1 to `tinsert' (table expected)", 2)
	end
	local n = getn(t)
	local pos, v
	if select("#", ...) <= 1 then
		-- tinsert(t, v): luaB_tinsert dem so doi so (lua_gettop == 2) -> chen cuoi
		pos, v = n + 1, (...)
	else
		local p, x = ...
		pos, v = mfloor(p), x
	end
	rawset(t, "n", n + 1)
	for i = n, pos, -1 do
		rawset(t, i + 1, rawget(t, i))
	end
	rawset(t, pos, v)
end

_G.tremove = function(t, pos)
	if type(t) ~= "table" then
		error("bad argument #1 to `tremove' (table expected)", 2)
	end
	local n = getn(t)
	pos = pos and mfloor(pos) or n
	if n <= 0 then
		return nil
	end
	local r = rawget(t, pos)
	for i = pos, n - 1 do
		rawset(t, i, rawget(t, i + 1))
	end
	rawset(t, "n", n - 1)
	rawset(t, n, nil)
	return r
end

_G.sort = function(t, f)
	-- Lua 4 sort dung getn (ke ca t.n)
	local n = getn(t)
	if n ~= #t then
		local tmp = {}
		for i = 1, n do tmp[i] = t[i] end
		if f then table.sort(tmp, f) else table.sort(tmp) end
		for i = 1, n do t[i] = tmp[i] end
		return
	end
	if f then table.sort(t, f) else table.sort(t) end
end

_G.foreach = function(t, f)
	for k, v in next, t do
		local r = f(k, v)
		if r ~= nil then
			return r
		end
	end
end

_G.foreachi = function(t, f)
	for i = 1, getn(t) do
		local r = f(i, t[i])
		if r ~= nil then
			return r
		end
	end
end
-- next, rawget, rawset, type, tostring, print, assert, error: giu ban 5.4 (cung ngu nghia)
_G.rawgettable = rawget
_G.rawsettable = rawset

-- ---------------------------------------------------------------------------
-- 3. Bien toan cuc kieu Lua 4
-- ---------------------------------------------------------------------------
_G.globals = function(t)
	if t ~= nil then
		error("globals(t): thay bang toan cuc khong ho tro tren Lua 5.4", 2)
	end
	return moiTruong()
end
_G.getglobal = function(name) return moiTruong()[name] end
_G.setglobal = function(name, v) moiTruong()[name] = v end
_G.rawgetglobal = function(name) return rawget(moiTruong(), name) end
_G.rawsetglobal = function(name, v) rawset(moiTruong(), name, v) end
_G.nextvar = function(k) return next(moiTruong(), k) end
_G.foreachvar = function(f)
	for k, v in next, moiTruong() do
		local r = f(k, v)
		if r ~= nil then return r end
	end
end
-- load / loadfile: mac dinh chay trong moi truong cua script goi (mot state); che do thuong = _G nhu 5.4
local load54, loadfile54 = load, loadfile
_G.load = function(chunk, name, mode, env)
	if env == nil then env = moiTruong() end
	return load54(chunk, name, mode, env)
end
_G.loadfile = function(path, mode, env)
	if env == nil then env = moiTruong() end
	return loadfile54(path, mode, env)
end
_G.gcinfo = function() return mfloor(collectgarbage_54("count")), 0 end
_G.collectgarbage = function(n)
	if n ~= nil then
		collectgarbage_54("step", n)
	else
		collectgarbage_54("collect")
	end
	return 0
end

-- ---------------------------------------------------------------------------
-- 4. call / dostring / dofile (ngu nghia luaB_call, passresults)
-- ---------------------------------------------------------------------------
_G.call = function(f, args, mode, handler)
	if type(args) ~= "table" then
		error("bad argument #2 to `call' (table expected)", 2)
	end
	local env = moiTruong()
	local n = getn(args)
	mode = mode or ""
	local cu
	if handler ~= nil then
		cu = env._ERRORMESSAGE
		env._ERRORMESSAGE = handler
	end
	local function goi()
		return f(unpack(args, 1, n))
	end
	local r = { xpcall(goi, function(msg)
		-- Lua 4: loi -> _ERRORMESSAGE(msg) (handler neu co) ngay tai cho loi
		local em = env._ERRORMESSAGE
		if type(em) == "function" then
			pcall(em, themTraceback(msg))
		end
		return msg
	end) }
	if handler ~= nil then
		env._ERRORMESSAGE = cu
	end
	if r[1] then
		return unpack(r, 2, #r)
	end
	if string.find(mode, "x", 1, true) then
		return nil
	end
	error(r[2], 0)			-- lan loi ra ngoai, khong them thong diep (lua_error(L, NULL))
end

local function chayChunk(f, err, env)
	if not f then
		baoLoi(err, env)
		return nil
	end
	local r = { xpcall(f, function(msg)
		baoLoi(themTraceback(msg), env)
		return msg
	end) }
	if r[1] then
		return unpack(r, 2, #r)
	end
	return nil
end

_G.dostring = function(s, name)
	if type(s) ~= "string" then
		error("bad argument #1 to `dostring' (string expected)", 2)
	end
	if string.byte(s, 1) == 27 then
		error("`dostring' cannot run pre-compiled code", 2)
	end
	local env = moiTruong()
	local f, e = load54(s, name or s, "t", env)
	return chayChunk(f, e, env)
end

_G.dofile = function(path)
	local env = moiTruong()
	local f, e = loadfile54(path, "t", env)
	if f == nil and type(path) == "string" and type(_G.L4_DuongDanMoi) == "function" then
		local moi = _G.L4_DuongDanMoi(path)	-- [SAPXEP 06/09] duong dan cu -> moi (script\_duongdan_cu.txt)
		if moi ~= nil then f, e = loadfile54(moi, "t", env) end
	end
	return chayChunk(f, e, env)
end

-- ---------------------------------------------------------------------------
-- 5. Tag method -> metatable. Su kien Lua 4: gettable settable index function gc
--    add sub mul div pow unm lt le concat getglobal setglobal
-- ---------------------------------------------------------------------------
local tagMt = {}         -- tag id -> metatable
local tagKe = 100        -- id dau (tranh trung cac tag co ban cua Lua 4: 0..5)
local TAG_CO_BAN = { ["nil"] = 1, number = 2, string = 3, table = 4, ["function"] = 5, userdata = 0, boolean = 2, thread = 0 }

local SU_KIEN = { gettable = "__index", index = "__index", settable = "__newindex", ["function"] = "__call", gc = "__gc",
	add = "__add", sub = "__sub", mul = "__mul", div = "__div", pow = "__pow", unm = "__unm",
	lt = "__lt", le = "__le", concat = "__concat" }

_G.newtag = function()
	tagKe = tagKe + 1
	tagMt[tagKe] = { __lua4tag = tagKe }
	return tagKe
end

_G.settag = function(t, tg)
	if type(t) ~= "table" then
		error("bad argument #1 to `settag' (table expected)", 2)
	end
	local mt = tagMt[tg]
	if mt == nil then
		error("`settag': tag " .. tostring(tg) .. " chua duoc tao bang newtag()", 2)
	end
	setmetatable(t, mt)
	return t
end

_G.tag = function(v)
	local mt = getmetatable(v)
	if type(mt) == "table" and mt.__lua4tag then
		return mt.__lua4tag
	end
	return TAG_CO_BAN[type(v)] or 0
end

_G.settagmethod = function(tg, event, f)
	local mt = tagMt[tg]
	if mt == nil then
		error("`settagmethod': tag " .. tostring(tg) .. " chua duoc tao bang newtag()", 2)
	end
	local key = SU_KIEN[event]
	if key == nil then
		error("`settagmethod': su kien '" .. tostring(event) .. "' khong ho tro", 2)
	end
	local cu = mt[key]
	if event == "gettable" and f ~= nil then
		-- Lua 4 goi f(t, k) cho MOI truy cap; 5.4 __index chi khi thieu khoa. Ket qua nhu nhau
		-- khi f tu rawget (mau baseclass.lua). Giu nguyen f.
		mt[key] = f
	elseif event == "settable" and f ~= nil then
		mt[key] = f
	else
		mt[key] = f
	end
	return cu
end

_G.gettagmethod = function(tg, event)
	local mt = tagMt[tg]
	if mt == nil then return nil end
	return mt[SU_KIEN[event] or event]
end

_G.copytagmethods = function(tagTo, tagFrom)
	local a, b = tagMt[tagTo], tagMt[tagFrom]
	if a and b then
		for k, v in next, b do
			if k ~= "__lua4tag" then a[k] = v end
		end
	end
	return tagTo
end

-- ---------------------------------------------------------------------------
-- 6. Chuoi (Lua 4 dung ten toan cuc, khong co bang string.)
-- ---------------------------------------------------------------------------
_G.strlen = string.len
_G.strsub = string.sub
_G.strlower = string.lower
_G.strupper = string.upper
_G.strrep = string.rep
_G.strchar = string.char
_G.strbyte = string.byte
_G.ascii = string.byte
_G.strfind = string.find
_G.gsub = string.gsub

-- format: Lua 4 ep (int) cho %d/%i/%c va (unsigned) cho %o/%u/%x/%X; 5.4 loi neu so thuc khong nguyen.
-- Dong thoi so > 2^31 khong con tran (5.4 dung 64 bit).
local function epNguyen(v)
	if type(v) == "number" then
		local i = math.tointeger(v)
		if i == nil then
			i = mfloor(v)
			i = math.tointeger(i) or i
		end
		return i
	end
	return v
end
_G.format = function(fmt, ...)
	local n = select("#", ...)
	if n == 0 then
		return string.format(fmt)
	end
	local args = { ... }
	local i = 0
	for spec in string.gmatch(fmt, "%%[-+ #0-9.]*([a-zA-Z%%])") do
		if spec ~= "%" then
			i = i + 1
			if i > n then break end
			if spec == "d" or spec == "i" or spec == "c" or spec == "o" or spec == "u" or spec == "x" or spec == "X" then
				args[i] = epNguyen(args[i])
			end
		end
	end
	return string.format(fmt, unpack(args, 1, n))
end

-- ---------------------------------------------------------------------------
-- 7. Toan (ten toan cuc)
-- ---------------------------------------------------------------------------
_G.abs = math.abs
_G.ceil = math.ceil
_G.floor = math.floor
_G.sqrt = math.sqrt
_G.min = math.min
_G.max = math.max
_G.log = math.log
_G.log10 = function(x) return math.log(x, 10) end
_G.exp = math.exp
_G.deg = math.deg
_G.rad = math.rad
_G.sin = math.sin
_G.cos = math.cos
_G.tan = math.tan
_G.asin = math.asin
_G.acos = math.acos
_G.atan = math.atan
_G.atan2 = function(y, x) return math.atan(y, x) end
_G.frexp = math.frexp or function(x) error("frexp khong co tren 5.4") end
_G.ldexp = math.ldexp or function(m, e) return m * 2.0 ^ e end
_G.PI = math.pi
-- [LUA54 05/09] mod(x, 0): Lua 4 = C fmod -> nan (im lang); 5.4 math.fmod voi so nguyen bao loi
-- "bad argument #2 (zero)" va cat ngang script. Giu ngu nghia Lua 4: chia 0 tra nan.
local __l4_fmod = math.fmod
_G.mod = function(a, b)
    if b == 0 then return 0 / 0 end
    return __l4_fmod(a, b)
end
_G.randomseed = function(s) math.randomseed(epNguyen(s or os.time())) end
_G.random = function(a, b)
	if a == nil then
		return math.random()
	end
	if b == nil then
		local n = epNguyen(a)
		if n < 1 then return 1 end				-- Lua 4: random(0) tra 1 (luaL_arg_check(1<=u))... giu an toan
		return math.random(n)
	end
	local lo, hi = epNguyen(a), epNguyen(b)
	if hi < lo then lo, hi = hi, lo end
	return math.random(lo, hi)
end

-- ---------------------------------------------------------------------------
-- 8. Vao/ra kieu Lua 4 (liolib 4.0): mot tep vao mac dinh, mot tep ra mac dinh
-- ---------------------------------------------------------------------------
local defIn, defOut = io.stdin, io.stdout
_G.openfile = function(name, mode)
	local f, e = io.open(name, mode or "r")
	if not f then return nil, e end
	return f
end
_G.closefile = function(f)
	if f == nil then return nil end
	local ok = pcall(function() f:close() end)
	return ok and 1 or nil
end
_G.readfrom = function(name)
	if name == nil then
		if defIn ~= io.stdin then pcall(function() defIn:close() end) end
		defIn = io.stdin
		return defIn
	end
	local f = io.open(name, "r")
	if not f then return nil end
	defIn = f
	return f
end
_G.writeto = function(name)
	if name == nil then
		if defOut ~= io.stdout then pcall(function() defOut:close() end) end
		defOut = io.stdout
		return defOut
	end
	local f = io.open(name, "w")
	if not f then return nil end
	defOut = f
	return f
end
_G.appendto = function(name)
	local f = io.open(name, "a")
	if not f then return nil end
	defOut = f
	return f
end
local function laTep(x)
	return io.type(x) ~= nil
end
_G.read = function(a, ...)
	if laTep(a) then
		return a:read(...)
	end
	return defIn:read(a, ...)
end
_G.write = function(a, ...)
	if laTep(a) then
		local ok = a:write(...)
		return ok and 1 or nil
	end
	local ok = defOut:write(a, ...)
	return ok and 1 or nil
end
_G.seek = function(f, whence, offset) return f:seek(whence, offset) end
_G.flush = function(f) if f then f:flush() else defOut:flush() end return 1 end
_G.remove = function(name) local ok = os.remove(name) return ok and 1 or nil end
_G.rename = function(a, b) local ok = os.rename(a, b) return ok and 1 or nil end
_G.tmpname = os.tmpname
_G.getenv = os.getenv
_G.exit = os.exit
_G.execute = function(cmd) local ok, _, code = os.execute(cmd) return code or (ok and 0 or 1) end
_G.setlocale = os.setlocale
_G.date = os.date
_G.clock = os.clock
_G.time = os.time

-- ---------------------------------------------------------------------------
-- 9. tonumber: giu ban 5.4 (da doi chung: Lua 4.0.1 tren MSVC cung nhan "0x10" = 16 qua strtod)
-- ---------------------------------------------------------------------------

-- ---------------------------------------------------------------------------
-- 10. unpack (Lua 4 khong co, nhung baseclass.lua dung unpack(arg) -> co san o 5.4 la table.unpack)
-- ---------------------------------------------------------------------------
_G.unpack = _G.unpack or table.unpack
