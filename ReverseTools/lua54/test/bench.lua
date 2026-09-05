-- bench.lua (cu phap Lua 4): do toc do thong dich - chay tren lua4.exe va tren Lua 5.4 + lua4compat.
-- Mo phong kieu tai that cua script game: vong lap long, truy cap bang, goi ham, ghep chuoi, format.

function do_thoi_gian(ten, f)
	local t0 = clock()
	local r = f()
	local t1 = clock()
	print(format("%-28s %8.1f ms   (kq %s)", ten, (t1 - t0) * 1000, tostring(r)))
end

-- 1. Vong lap long 600 x 600 tim trung (kieu TK_GuiDiemPhe FindSame)
do_thi_gian_1 = function()
	local tbA, tbB = {}, {}
	for i = 1, 600 do tbA[i] = i * 7 tbB[i] = 4200 - i * 7 end
	local dem = 0
	for i = 1, 600 do
		for j = 1, 600 do
			if (tbA[i] == tbB[j]) then dem = dem + 1 end
		end
	end
	return dem
end
do_thoi_gian("vong lap long 600x600", do_thi_gian_1)

-- 2. Goi ham nho 2.000.000 lan
local cong = function(a, b) return a + b end
do_thoi_gian("goi ham 2.000.000 lan", function()
	local s = 0
	for i = 1, 2000000 do s = %cong(s, i) end
	return s
end)

-- 3. Bang: chen 200.000 phan tu + doc lai
do_thoi_gian("tinsert 200.000 + doc", function()
	local tb = {}
	for i = 1, 200000 do tinsert(tb, i) end
	local s = 0
	for i = 1, getn(tb) do s = s + tb[i] end
	return s
end)

-- 4. Chuoi: format + ghep 100.000 lan
do_thoi_gian("format+ghep 100.000", function()
	local n = 0
	for i = 1, 100000 do
		local s = format("%d-%s", i, "abc") .. "x"
		n = n + strlen(s)
	end
	return n
end)

-- 5. floor/random 1.000.000 lan
do_thoi_gian("floor+random 1.000.000", function()
	local s = 0
	for i = 1, 1000000 do s = s + floor(random(1, 100) / 3) end
	return s > 0
end)
