-- dump_cfg.lua (viet bang CU PHAP LUA 4): nap toan bo bang cau hinh roi in tung khoa da sap xep.
-- Chay tren Lua 4 (lua4.exe) voi ban goc, va tren Lua 5.4 + lua4compat voi ban da qua bo chuyen
-- (chinh tep nay cung duoc bo chuyen doi: for-in -> pairs, %x -> x). Hai ket qua phai GIONG HET.
-- GOC = thu muc script (ban goc hoac ban da chuyen), do trinh chay dat truoc.

function Include(p)
	local q = gsub(p, strchar(92), "/")
	q = gsub(q, "^/script", "")
	dofile(GOC .. q)
end

Include("\\script\\cauhinh\\ch_all.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\cauhinh_web\\cfgw_meta.lua")

DS = {}
function them(ten, tb)
	for k, v in tb do
		if (type(v) == "table") then
			for k2, v2 in v do
				if (type(v2) ~= "table") then
					tinsert(DS, ten .. "." .. tostring(k) .. "." .. tostring(k2) .. "=" .. tostring(v2))
				end
			end
		else
			tinsert(DS, ten .. "." .. tostring(k) .. "=" .. tostring(v))
		end
	end
end

them("CHUNG", tbCFG_CHUNG)
them("LICH", tbCFG_LICH)
them("THUONG", tbCFG_THUONG)
them("EXP", tbCFG_EXP)
them("DROP", tbCFG_DROP)
them("CHD", tbCHD)
them("META", tbCFGW_META)
sort(DS)
for i = 1, getn(DS) do
	print(DS[i])
end

-- vai ham thu vien hay dung: ket qua phai giong nhau tren hai ban
print("TONG", getn(DS))
print("G_CFG", G_CFG("GLB_TILE_EXP", 0), G_CFG("KHONG_CO", 7))
print("HD_CFG", HD_CFG("TW_GIO_KHAICHIEN", 0), HD_GioPhut(1730), HD_GioPhut(805))
print("format", format("%d|%s|%5.2f|%03d", 7/2, "x", 1/3, 5))
print("floor/mod/ceil", floor(7/2), mod(7, 3), ceil(2.1), floor(-2.5))
print("chia", 10/2, 7/2, 1e3, 2^10, 100000 * 100000)
print("chuoi", strsub("abcdef", 2, 4), strlen("abc"), strfind("hello world", "wor"), strupper("abc"), strrep("ab", 3))
print("gsub", gsub("a-b-c", "-", "+"))
print("tostring/tonumber", tostring(12), tostring(12.5), tonumber("15"), tonumber("0x10"), tonumber("abc"))
print("tinsert n", getn({1, 2, 3}), getn({n = 5}))
local tb = {}
tinsert(tb, "a") tinsert(tb, "b") tinsert(tb, 1, "z")
print("tinsert", getn(tb), tb.n, tb[1], tb[2], tb[3], tremove(tb), getn(tb), tb.n)
print("call", call(function(a, b) return a + b end, {2, 3}))
print("call x", call(function() error("boom") end, {}, "x", function(m) end))
print("date", date("%Y") ~= nil)
