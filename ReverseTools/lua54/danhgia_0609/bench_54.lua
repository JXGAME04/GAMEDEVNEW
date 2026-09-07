-- bench_54.lua : tinh nang Lua 5.4 co the ap cho script JX1 - do tren Lua54Dll x64 (lop tuong thich dang bat)
local clock = os.clock
local OUT = {}
local function P(s) OUT[#OUT+1] = s end
local function do3(f)
  local best = 1e9
  for r = 1, 3 do local t0 = clock(); f(); local dt = (clock() - t0) * 1000; if dt < best then best = dt end end
  return best
end
local function cmp(name, n, fa, fb, ghichu)
  local a, b = do3(fa), do3(fb)
  P(string.format("%-46s n=%-8d cach cu=%8.1f ms   5.4=%8.1f ms   nhanh %5.1fx  %s", name, n, a, b, a / b, ghichu or ""))
end
local N
-- 1. co bit: doc bit k cua x (kieu cu floor/mod) vs toan tu bit 5.4
N = 1000000
cmp("doc bit: floor(x/2^k) mod 2  vs  (x>>k)&1", N,
  function() local s = 0 for i = 1, N do s = s + mod(floor(i / 2^7), 2) end return s end,
  function() local s = 0 for i = 1, N do s = s + ((i >> 7) & 1) end return s end)
cmp("dat bit: x + 2^k (kiem truoc)  vs  x | (1<<k)", N,
  function() local s = 0 for i = 1, N do local x = i if mod(floor(x / 2^5), 2) == 0 then x = x + 2^5 end s = s + x end return s end,
  function() local s = 0 for i = 1, N do s = s + (i | (1 << 5)) end return s end)
-- 2. ghep chuoi trong vong lap vs table.concat
N = 20000
cmp("ghep chuoi s = s .. x (20.000 lan) vs table.concat", N,
  function() local s = "" for i = 1, N do s = s .. i .. "," end return #s end,
  function() local t = {} for i = 1, N do t[#t + 1] = i end local s = table.concat(t, ",") return #s end, "(O(n^2) vs O(n))")
-- 3. so nguyen 64 bit vs so thuc
N = 3000000
cmp("cong so nguyen (5.4 integer) vs so thuc", N,
  function() local s = 0.0 for i = 1, N do s = s + i * 1.0 end return s end,
  function() local s = 0 for i = 1, N do s = s + i end return s end)
cmp("chia lay phan nguyen floor(a/b) vs a//b", N,
  function() local s = 0 for i = 1, N do s = s + floor(i / 7) end return s end,
  function() local s = 0 for i = 1, N do s = s + i // 7 end return s end)
-- 4. sao chep bang: vong for vs table.move
N = 2000
local SRC = {} for i = 1, 1000 do SRC[i] = i end
cmp("sao chep bang 1000 pt: for vs table.move", N,
  function() for r = 1, N do local d = {} for i = 1, #SRC do d[i] = SRC[i] end end end,
  function() for r = 1, N do local d = table.move(SRC, 1, #SRC, 1, {}) end end)
-- 5. dem so pt: getn(t) (quet) vs #t  (da do truoc: 25x voi 10 pt)
-- 6. bat loi: call(f, {..}, 'x') vs pcall (da do: 22x)
-- 7. goto continue vs co flag
N = 3000000
cmp("bo qua phan tu: co 'flag' vs goto continue", N,
  function() local s = 0 for i = 1, N do local bo = false if i % 3 == 0 then bo = true end if not bo then s = s + i end end return s end,
  function() local s = 0 for i = 1, N do if i % 3 == 0 then goto tiep end s = s + i ::tiep:: end return s end)
-- 8. coroutine: chi phi resume/yield (cho hoi thoai nhieu buoc)
N = 300000
cmp("state machine bang bien vs coroutine resume/yield", N,
  function() local st, s = 0, 0 for i = 1, N do st = (st + 1) % 3 s = s + st end return s end,
  function() local co = coroutine.wrap(function() local s = 0 while true do s = s + 1 coroutine.yield(s) end end) local s = 0 for i = 1, N do s = co() end return s end, "(coroutine cham hon nhung don gian hoa code)")
-- 9. tra bang cau hinh: bang toan cuc vs local upvalue (cache)
N = 3000000
CFG = {ExpRate = 5}
cmp("doc CFG.ExpRate toan cuc vs local cache", N,
  function() local s = 0 for i = 1, N do s = s + CFG.ExpRate end return s end,
  function() local rate = CFG.ExpRate local s = 0 for i = 1, N do s = s + rate end return s end)
-- 10. string.format vs ghep .. cho thong bao
N = 300000
cmp("format('%s co %d vp') vs a..' co '..b..' vp'", N,
  function() local n = 0 for i = 1, N do n = n + #string.format("%s co %d vat pham", "Ten", i) end return n end,
  function() local n = 0 for i = 1, N do n = n + #("Ten" .. " co " .. i .. " vat pham") end return n end)
io.output(RA_TEP); io.write(table.concat(OUT, "\n"), "\n"); io.output():close()
