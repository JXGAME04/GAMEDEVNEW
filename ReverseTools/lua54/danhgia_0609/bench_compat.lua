-- bench_compat.lua : ten ham Lua 4 (lop tuong thich) vs ten ham 5.4 thuan, chay tren Lua54Dll x64 (dung DLL may chu)
local clock = os.clock
local OUT = {}
local function P(s) OUT[#OUT+1] = s end
local function do3(f)
  local best = 1e9
  for r = 1, 3 do
    local t0 = clock(); f(); local dt = (clock() - t0) * 1000
    if dt < best then best = dt end
  end
  return best
end
local function cmp(name, n, fa, fb, fc, namec)
  local a, b = do3(fa), do3(fb)
  local c = fc and do3(fc) or nil
  local s = string.format("%-32s n=%-8d cu=%8.1f ms  moi=%8.1f ms  cu/moi=%5.2f", name, n, a, b, a / b)
  if c then s = s .. string.format("  moi2=%8.1f ms (%s)", c, namec or "") end
  P(s)
end
local N
N = 2000000
cmp("floor(x) vs math.floor(x)", N,
  function() local s = 0 for i = 1, N do s = s + floor(i / 3) end return s end,
  function() local s = 0 for i = 1, N do s = s + math.floor(i / 3) end return s end,
  function() local s = 0 for i = 1, N do s = s + (i / 3) // 1 end return s end, "(i/3)//1")
for _, sz in ipairs({10, 100, 1000}) do
  local t = {} for i = 1, sz do t[i] = i end
  N = 2000000 // sz
  if N < 2000 then N = 2000 end
  cmp(string.format("getn(t) vs #t, %d pt, khong t.n", sz), N,
    function() local s = 0 for i = 1, N do s = s + getn(t) end return s end,
    function() local s = 0 for i = 1, N do s = s + #t end return s end)
end
do
  local t = {n = 100} for i = 1, 100 do t[i] = i end
  N = 1000000
  cmp("getn(t) co t.n vs #t", N,
    function() local s = 0 for i = 1, N do s = s + getn(t) end return s end,
    function() local s = 0 for i = 1, N do s = s + #t end return s end)
end
N = 200000
cmp("tinsert(t,v) vs table.insert", N,
  function() local t = {} for i = 1, N do tinsert(t, i) end return t end,
  function() local t = {} for i = 1, N do table.insert(t, i) end return t end,
  function() local t = {} for i = 1, N do t[#t + 1] = i end return t end, "t[#t+1]=v")
N = 300000
cmp("format %d-%s (so nguyen)", N,
  function() local n = 0 for i = 1, N do n = n + #format("%d-%s", i, "abc") end return n end,
  function() local n = 0 for i = 1, N do n = n + #string.format("%d-%s", i, "abc") end return n end)
cmp("format %d (so thuc, ep nguyen)", N,
  function() local n = 0 for i = 1, N do n = n + #format("%d", i * 0.5) end return n end,
  function() local n = 0 for i = 1, N do n = n + #string.format("%d", math.floor(i * 0.5)) end return n end)
N = 1000000
cmp("random(1,100) vs math.random", N,
  function() local s = 0 for i = 1, N do s = s + random(1, 100) end return s end,
  function() local s = 0 for i = 1, N do s = s + math.random(1, 100) end return s end)
cmp("mod(a,b) vs math.fmod vs %", N,
  function() local s = 0 for i = 1, N do s = s + mod(i, 7) end return s end,
  function() local s = 0 for i = 1, N do s = s + math.fmod(i, 7) end return s end,
  function() local s = 0 for i = 1, N do s = s + i % 7 end return s end, "i % 7")
N = 500000
local S = "abcdefghij_klmnop"
cmp("strfind vs string.find", N,
  function() local s = 0 for i = 1, N do s = s + strfind(S, "_", 1, true) end return s end,
  function() local s = 0 for i = 1, N do s = s + string.find(S, "_", 1, true) end return s end)
cmp("strsub vs string.sub", N,
  function() local s = 0 for i = 1, N do s = s + #strsub(S, 3, 8) end return s end,
  function() local s = 0 for i = 1, N do s = s + #string.sub(S, 3, 8) end return s end)
cmp("strlen vs #s", N,
  function() local s = 0 for i = 1, N do s = s + strlen(S) end return s end,
  function() local s = 0 for i = 1, N do s = s + #S end return s end)
N = 100000
cmp("date vs os.date", N,
  function() local s = 0 for i = 1, N do s = s + #date("%H:%M") end return s end,
  function() local s = 0 for i = 1, N do s = s + #os.date("%H:%M") end return s end)
N = 200000
local function f2(a, b) return a + b end
cmp("call(f,{a,b},'x') vs pcall", N,
  function() local s = 0 for i = 1, N do s = s + call(f2, {i, 1}, "x") end return s end,
  function() local s = 0 for i = 1, N do local ok, r = pcall(f2, i, 1) s = s + r end return s end)
N = 50000
cmp("dostring vs load()()", N,
  function() local s = 0 for i = 1, N do s = s + dostring("return 1+1") end return s end,
  function() local s = 0 for i = 1, N do s = s + load("return 1+1")() end return s end)
N = 200
cmp("sort vs table.sort (1000 pt)", N,
  function() for r = 1, N do local t = {} for i = 1, 1000 do t[i] = (i * 7919) % 1000 end sort(t) end end,
  function() for r = 1, N do local t = {} for i = 1, 1000 do t[i] = (i * 7919) % 1000 end table.sort(t) end end)
N = 2000
do
  local t = {} for i = 1, 1000 do t[i] = i end
  cmp("foreachi vs ipairs (1000 pt)", N,
    function() local s = 0 for r = 1, N do foreachi(t, function(i, v) s = s + v end) end return s end,
    function() local s = 0 for r = 1, N do for i, v in ipairs(t) do s = s + v end end return s end)
  cmp("foreach vs pairs (1000 pt)", N,
    function() local s = 0 for r = 1, N do foreach(t, function(k, v) s = s + v end) end return s end,
    function() local s = 0 for r = 1, N do for k, v in pairs(t) do s = s + v end end return s end)
end
N = 500000
cmp("i..'' vs tostring(i)", N,
  function() local s = 0 for i = 1, N do s = s + #(i .. "") end return s end,
  function() local s = 0 for i = 1, N do s = s + #tostring(i) end return s end)
-- goi ham toan cuc vs local (kieu viet script cu vs moi, khong lien quan lop tuong thich)
N = 3000000
GHam = function(a) return a + 1 end
local LHam = GHam
cmp("goi ham toan cuc vs local", N,
  function() local s = 0 for i = 1, N do s = GHam(s) end return s end,
  function() local s = 0 for i = 1, N do s = LHam(s) end return s end)
io.output(RA_TEP); io.write(table.concat(OUT, "\n"), "\n"); io.output():close()
