-- do thoi gian dofile cay timerserver.lua (nhu NHIPNAP) ngoai engine: ham engine gia lap qua tag method getglobal (Lua 4)
local stub = function() return 0 end
settagmethod(tag(nil), "getglobal", function(name) return %stub end)
function Include(p)
  local q = gsub(p, "\\", "/")
  if strsub(q, 1, 1) == "/" then q = strsub(q, 2) end
  call(dofile, {q}, "x")
end
local t0 = clock()
dofile("script/timerserver.lua")
local t1 = clock()
print(format("NAPLAI %.1f ms", (t1 - t0) * 1000))
