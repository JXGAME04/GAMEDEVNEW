r"""patch_pa1b.py - ap phan con lai cua PA-1 (khop anchor bo qua khoang trang cuoi dong) + noi menu thu Lua 5.4 vao lenhbaiadmin.
Chay SAU patch_pa1.py bi dung o lib_ham (tep chua bi ghi). Doc/ghi latin-1, giu CRLF.
"""
import io, os, sys, re
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for c in s if ord(c) > 127)
def rx_of(old, nl):
    parts = [re.escape(ln.rstrip()) + r"[ \t]*" for ln in old.split("\n")]
    return re.compile(re.escape(nl).join(parts))
def patch(rel, pairs):
    p = os.path.join(ROOT, rel)
    s = rd(p); h0 = hb(s); nl = "\r\n" if "\r\n" in s else "\n"
    for old, new in pairs:
        rx = rx_of(old, nl)
        ms = rx.findall(s)
        if len(ms) != 1:
            print("ANCHOR %s: %d lan: %r" % (rel, len(ms), old[:80])); sys.exit(1)
        s = rx.sub(lambda m: new.replace("\n", nl), s, count=1)
    wr(p, s)
    print("%-45s %d cho, byte cao %d -> %d" % (rel, len(pairs), h0, hb(s)))

BS = chr(92)
patch(r"lib\lib_ham.lua", [
 ("""	if strNum > 2 then
		for i=2,strNum - 1 do
			strSel = strSel..format("%q", strSay[i])..",";
		end;
		strSel = strSel..format("%q", strSay[strNum]);
		strMsg = "Say("..format("%q", strSay[1])..","..(strNum - 1)..","..strSel..");";
	elseif strNum == 2 then
		strSel = format("%q", strSay[strNum]);
		strMsg = "Say("..format("%q", strSay[1])..",1"..","..strSel..");";
	end;
	dostring(strMsg);
end;""",
  """	-- [PA1 06/09 toi] truoc: ghep chuoi "Say(%q,...)" roi dostring (bien dich mot chunk moi lan mo menu); nay goi thang
	Say(strSay[1], strNum - 1, table.unpack(strSay, 2, strNum));
end;"""),
 ("""function TaoBang(tTable,sTableName,sTab)
    sTab = sTab or "";
    sTmp = ""
    sTmp = sTmp..sTab..sTableName.."={"

    local tStart = 0
    for key, value in pairs(tTable) do

        if tStart == 1 then
            sTmp = sTmp..",""" + BS + "r" + BS + """n"

        else
            sTmp = sTmp..\"""" + BS + "r" + BS + """n"
            tStart = 1
        end
        local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);
        if(type(value) == "table") then
            sTmp = sTmp..TaoBang(value, sKey, sTab..\"""" + BS + """t");
        else
            local sValue = (type(value) == "string") and format("%q",value) or tostring(value);
            sTmp = sTmp..sTab..\"""" + BS + """t"..sKey.." = "..sValue
        end

    end
    sTmp = sTmp..\"""" + BS + "r" + BS + """n"..sTab.."}"
    return sTmp

end""",
  """function TaoBang(tTable,sTableName,sTab)
    -- [PA1 06/09 toi] ghep chuoi trong vong lap (O(n^2) voi bang lon khi luu du lieu) -> bo dem table.concat
    sTab = sTab or "";
    local tb = { sTab..sTableName.."={" }
    local tStart = 0
    for key, value in pairs(tTable) do
        if tStart == 1 then
            tb[#tb + 1] = ",""" + BS + "r" + BS + """n"
        else
            tb[#tb + 1] = \"""" + BS + "r" + BS + """n"
            tStart = 1
        end
        local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);
        if(type(value) == "table") then
            tb[#tb + 1] = TaoBang(value, sKey, sTab..\"""" + BS + """t")
        else
            local sValue = (type(value) == "string") and format("%q",value) or tostring(value);
            tb[#tb + 1] = sTab..\"""" + BS + """t"..sKey.." = "..sValue
        end
    end
    tb[#tb + 1] = \"""" + BS + "r" + BS + """n"..sTab.."}"
    sTmp = table.concat(tb)
    return sTmp

end"""),
 ("""function TaoBangNew(tTable, sTableName, sTab)
    sTab = sTab or ""
    local sTmp = ""

    if sTableName and sTableName ~= "" then
        sTmp = sTmp .. sTab .. sTableName .. " = {""" + BS + """n"
    else
        sTmp = sTmp .. sTab .. "{""" + BS + """n"
    end

    for key, value in pairs(tTable) do
        local sKey = (type(key) == "string") and format("[%q]", key) or format("[%d]", key)
        if type(value) == "table" then

            sTmp = sTmp .. sTab .. \"""" + BS + """t" .. sKey .. " = " .. TaoBangNew(value, "", sTab .. \"""" + BS + """t") .. ",""" + BS + """n"
        else
            local sValue = (type(value) == "string") and format("%q", value) or tostring(value)
            sTmp = sTmp .. sTab .. \"""" + BS + """t" .. sKey .. " = " .. sValue .. ",""" + BS + """n"
        end
    end

    sTmp = sTmp .. sTab .. "}\"""",
  """function TaoBangNew(tTable, sTableName, sTab)
    -- [PA1 06/09 toi] bo dem table.concat thay ghep chuoi trong vong lap
    sTab = sTab or ""
    local tb = {}

    if sTableName and sTableName ~= "" then
        tb[#tb + 1] = sTab .. sTableName .. " = {""" + BS + """n"
    else
        tb[#tb + 1] = sTab .. "{""" + BS + """n"
    end

    for key, value in pairs(tTable) do
        local sKey = (type(key) == "string") and format("[%q]", key) or format("[%d]", key)
        if type(value) == "table" then

            tb[#tb + 1] = sTab .. \"""" + BS + """t" .. sKey .. " = " .. TaoBangNew(value, "", sTab .. \"""" + BS + """t") .. ",""" + BS + """n"
        else
            local sValue = (type(value) == "string") and format("%q", value) or tostring(value)
            tb[#tb + 1] = sTab .. \"""" + BS + """t" .. sKey .. " = " .. sValue .. ",""" + BS + """n"
        end
    end

    tb[#tb + 1] = sTab .. "}"
    local sTmp = table.concat(tb)\""""),
])

patch(r"lib\common.lua", [
 ("""	local str = ""
	if (getn(tb) > 0) then
		str = tostring(tb[1])
		for i = 2, getn(tb) do
			str = str .. str_sep .. tostring(tb[i])
		end
	end
	return str""",
  """	-- [PA1 06/09 toi] table.concat thay ghep chuoi trong vong lap (O(n))
	local n = getn(tb)
	if n <= 0 then return "" end
	local t = {}
	for i = 1, n do
		t[i] = tostring(tb[i])
	end
	return table.concat(t, str_sep)"""),
])

patch(r"giaodich\thu\mailmanager.lua", [
 ("""    local sz = ""
    for i = 1, getn(tb) do
        if i > 1 then
            sz = sz..";"
        end
        sz = sz..tb[i]
    end
    return sz, getn(tb)""",
  """    local sz = table.concat(tb, ";", 1, getn(tb))    -- [PA1 06/09 toi] thay ghep chuoi trong vong lap
    return sz, getn(tb)"""),
])

patch(r"timerserver.lua", [
 ("""	local sz = ""
	for k, v in pairs(TS_tProf) do
		if (v >= 1) then sz = sz .. format(" %s=%d", k, v) end
	end
	GhiLog("PROF", format("RunTime %d ms:%s", nTongMs, sz))""",
  """	local tb = {}	-- [PA1 06/09 toi] bo dem thay ghep chuoi
	for k, v in pairs(TS_tProf) do
		if (v >= 1) then tb[#tb + 1] = format(" %s=%d", k, v) end
	end
	GhiLog("PROF", format("RunTime %d ms:%s", nTongMs, table.concat(tb)))"""),
])

patch(r"tinhnang\activitysys\functionlib.lua", [
 ("""	return dostring("return "..nValue1..szOption..nValue2)
end""",
  """	-- [PA1 06/09 toi] chuoi/khac: so sanh truc tiep (cung ngu nghia toan tu Lua), khong bien dich chunk
	if (szOption == "==") then return nValue1 == nValue2
	elseif (szOption == "~=") then return nValue1 ~= nValue2
	elseif (type(nValue1) == type(nValue2) and (type(nValue1) == "number" or type(nValue1) == "string")) then
		if (szOption == ">") then return nValue1 > nValue2
		elseif (szOption == ">=") then return nValue1 >= nValue2
		elseif (szOption == "<") then return nValue1 < nValue2
		elseif (szOption == "<=") then return nValue1 <= nValue2
		end
	end
	return dostring("return "..tostring(nValue1)..szOption..tostring(nValue2))
end

-- [PA1 06/09 toi] cache chunk da bien dich theo chuoi (cau hinh hoat dong lap lai cung chuoi hang nghin lan)
lib.tbChunkCache = lib.tbChunkCache or {}
function lib:ChayChuoi(szCode)
	local f = self.tbChunkCache[szCode]
	if f == nil then
		local e
		f, e = load(szCode, szCode, "t")
		if f == nil then
			if type(_ERRORMESSAGE) == "function" then _ERRORMESSAGE("ChayChuoi: "..tostring(e)) end
			return nil
		end
		self.tbChunkCache[szCode] = f
	end
	local ok, r = xpcall(f, function(m) if type(_ERRORMESSAGE) == "function" then _ERRORMESSAGE(tostring(m)) end return m end)
	if ok then return r end
	return nil
end"""),
 ("""	szAwardMsg = gsub(szAwardMsg, "<enter>", \"""" + BS + """n")
	szAwardMsg = gsub(szAwardMsg, "<tab>", \"""" + BS + """t")

	return dostring(szAwardMsg)""",
  """	szAwardMsg = gsub(szAwardMsg, "<enter>", \"""" + BS + """n")
	szAwardMsg = gsub(szAwardMsg, "<tab>", \"""" + BS + """t")

	return self:ChayChuoi(szAwardMsg)	-- [PA1 06/09 toi] cache chunk"""),
 ("""	elseif type(pParam) == "string" then
		return dostring("return "..pParam)
	end""",
  """	elseif type(pParam) == "string" then
		return tonumber(pParam) or self:ChayChuoi("return "..pParam)	-- [PA1 06/09 toi] so thuan -> tonumber; con lai cache chunk
	end"""),
 ("""		szValue = dostring("return ".. szParam)""",
  """		szValue = self:ChayChuoi("return ".. szParam)	-- [PA1 06/09 toi] cache chunk"""),
])

patch(r"nhiemvu\system\task_string.lua", [
 ("""	local num = getn(szMsg);
	local szmsg = "";
	PushString(szmsg)
	for i=1,num-1 do
		-- szmsg = szmsg..format("%q",szMsg[i])..",";
		AppendString(format("%q",szMsg[i]))
		AppendString(",")
	end;
	szmsg = PopString()
	szmsg = szmsg .. format("%q",SetTaskSayColor(szMsg[num]));
	szmsg = "Talk("..num..","..format("%q",fun)..","..szmsg..")";
	dostring(szmsg);""",
  """	local num = getn(szMsg);
	-- [PA1 06/09 toi] truoc: ghep chuoi "Talk(n,%q,...)" roi dostring; nay goi Talk truc tiep (cau cuoi qua SetTaskSayColor nhu cu)
	local tb = {}
	for i=1,num-1 do
		tb[i] = szMsg[i]
	end;
	tb[num] = SetTaskSayColor(szMsg[num])
	Talk(num, fun, table.unpack(tb, 1, num));"""),
])

patch(r"global\onkillnpc.lua", [
 ("""	call(BDH_OnKillNpc, {nNpcIndex}, "x")""",
  """	-- [PA1 06/09 toi] xpcall thay call(...,"x") (moi lan giet quai): loi van vao _ERRORMESSAGE (ScriptError.log)
	xpcall(BDH_OnKillNpc, function(m) if type(_ERRORMESSAGE) == "function" then _ERRORMESSAGE(tostring(m)) end return m end, nNpcIndex)"""),
])

patch(r"nhiemvu\hoithoai\dailogsay.lua", [
 ("""			local pFun = tbOpt[nSelectId][2]
			local tbParam = tbOpt[nSelectId][3]

			call(pFun, tbParam)""",
  """			local pFun = tbOpt[nSelectId][2]
			local tbParam = tbOpt[nSelectId][3]

			pFun(table.unpack(tbParam, 1, getn(tbParam)))	-- [PA1 06/09 toi] goi thang thay call(): loi van lan ra nhu cu"""),
])

# ---- lenhbaiadmin: Include module thu + muc menu
p = os.path.join(ROOT, r"vatpham\lenhbaiadmin.lua")
s = rd(p); h0 = hb(s); nl = "\r\n" if "\r\n" in s else "\n"
a = 'Include("' + BS + BS + 'script' + BS + BS + 'item' + BS + BS + 'test_kinhmach_admin.lua");'
assert s.count(a) == 1, ("anchor include", s.count(a))
s = s.replace(a, a + nl + 'Include("' + BS + BS + 'script' + BS + BS + 'kiemthu' + BS + BS + 'item' + BS + BS + 'test_lua54_admin.lua")\t-- [LUA54 06/09 toi] thu SayWait coroutine + lib_54', 1)
m = re.search(r'^[^\r\n]*/PB_Menu",[^\r\n]*$', s, re.M)
assert m, "khong thay dong PB_Menu"
s = s[:m.end()] + nl + '\t\t"Thu nghiem Lua 5.4: SayWait coroutine, lib_54/L54_MenuThu",\t-- [LUA54 06/09 toi]' + s[m.end():]
wr(p, s)
print("lenhbaiadmin.lua: ok, byte cao", h0, "->", hb(s))
print("XONG PA-1")
