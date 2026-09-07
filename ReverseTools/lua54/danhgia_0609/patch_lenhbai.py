import io, os
BS = chr(92)
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\vatpham\lenhbaiadmin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) > 127)
nl = "\r\n" if "\r\n" in s else "\n"
inc = 'Include("' + BS + BS + 'script' + BS + BS + 'kiemthu' + BS + BS + 'item' + BS + BS + 'test_lua54_admin.lua")'
if inc not in s:
    a = 'Include("' + BS + BS + 'script' + BS + BS + 'item' + BS + BS + 'test_kinhmach_admin.lua");'
    assert s.count(a) == 1, ("anchor include", s.count(a))
    s = s.replace(a, a + nl + inc + '\t-- [LUA54 06/09 toi] thu SayWait coroutine + lib_54', 1)
if "L54_MenuThu" not in s:
    k = s.find('/PB_Menu",')
    assert k >= 0, "khong thay /PB_Menu"
    e = s.find("\n", k)
    assert e >= 0
    if s[e - 1] == "\r": e -= 1
    s = s[:e] + nl + '\t\t"Thu nghiem Lua 5.4: SayWait coroutine, lib_54/L54_MenuThu",\t-- [LUA54 06/09 toi]' + s[e:]
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("lenhbaiadmin.lua: ok, byte cao", h0, "->", sum(1 for c in s if ord(c) > 127))
