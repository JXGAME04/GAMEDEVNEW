r"""patch_pa1.py - PA-1: sua dich danh cac cho viet kieu Lua 4 chay cham/khong can thiet (doc/ghi latin-1, giu CRLF).
  - lib/lib_ham.lua      : SayEx dostring("Say(...)") -> goi Say truc tiep; TaoBang/TaoBangNew ghep chuoi -> bo dem table.concat
  - lib/common.lua       : join -> table.concat
  - giaodich/thu/mailmanager.lua 664-669: ghep -> table.concat
  - timerserver.lua      : TS_GhiProf ghep -> bo dem
  - tinhnang/activitysys/functionlib.lua: dostring -> so sanh truc tiep / load cache theo chuoi
  - nhiemvu/system/task_string.lua: TalkEx dostring("Talk(...)") -> goi Talk truc tiep
  - global/onkillnpc.lua : call(f,{x},"x") -> xpcall
  - nhiemvu/hoithoai/dailogsay.lua: call(pFun, tbParam) -> goi truc tiep
Moi cho: assert anchor duy nhat; in bao cao.
"""
import io, os, sys, re
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for c in s if ord(c) > 127)
def patch(rel, pairs):
    p = os.path.join(ROOT, rel)
    s = rd(p); h0 = hb(s); nl = "\r\n" if "\r\n" in s else "\n"
    for old, new in pairs:
        old = old.replace("\n", nl); new = new.replace("\n", nl)
        if s.count(old) != 1:
            print("ANCHOR %s: %d lan: %r" % (rel, s.count(old), old[:80])); sys.exit(1)
        s = s.replace(old, new)
    wr(p, s)
    print("%-45s %d cho, byte cao %d -> %d" % (rel, len(pairs), h0, hb(s)))

# ---- lib_ham.lua
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
            sTmp = sTmp..",\\r\\n"

        else
            sTmp = sTmp.."\\r\\n"
            tStart = 1
        end
        local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);
        if(type(value) == "table") then
            sTmp = sTmp..TaoBang(value, sKey, sTab.."\\t");
        else
            local sValue = (type(value) == "string") and format("%q",value) or tostring(value);
            sTmp = sTmp..sTab.."\\t"..sKey.." = "..sValue
        end

    end
    sTmp = sTmp.."\\r\\n"..sTab.."}"
    return sTmp

end  """,
  """function TaoBang(tTable,sTableName,sTab)
    -- [PA1 06/09 toi] ghep chuoi trong vong lap (O(n^2) voi bang lon khi luu du lieu) -> bo dem table.concat
    sTab = sTab or "";
    local tb = { sTab..sTableName.."={" }
    local tStart = 0
    for key, value in pairs(tTable) do
        if tStart == 1 then
            tb[#tb + 1] = ",\\r\\n"
        else
            tb[#tb + 1] = "\\r\\n"
            tStart = 1
        end
        local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);
        if(type(value) == "table") then
            tb[#tb + 1] = TaoBang(value, sKey, sTab.."\\t")
        else
            local sValue = (type(value) == "string") and format("%q",value) or tostring(value);
            tb[#tb + 1] = sTab.."\\t"..sKey.." = "..sValue
        end
    end
    tb[#tb + 1] = "\\r\\n"..sTab.."}"
    sTmp = table.concat(tb)
    return sTmp
end  """),
 ("""function TaoBangNew(tTable, sTableName, sTab)
    sTab = sTab or ""
    local sTmp = ""

    if sTableName and sTableName ~= "" then
        sTmp = sTmp .. sTab .. sTableName .. " = {\\n"
    else
        sTmp = sTmp .. sTab .. "{\\n"
    end

    for key, value in pairs(tTable) do
        local sKey = (type(key) == "string") and format("[%q]", key) or format("[%d]", key)
        if type(value) == "table" then

            sTmp = sTmp .. sTab .. "\\t" .. sKey .. " = " .. TaoBangNew(value, "", sTab .. "\\t") .. ",\\n"
        else
            local sValue = (type(value) == "string") and format("%q", value) or tostring(value)
            sTmp = sTmp .. sTab .. "\\t" .. sKey .. " = " .. sValue .. ",\\n"
        end
    end

    sTmp = sTmp .. sTab .. "}\"""",
  """function TaoBangNew(tTable, sTableName, sTab)
    -- [PA1 06/09 toi] bo dem table.concat thay ghep chuoi trong vong lap
    sTab = sTab or ""
    local tb = {}

    if sTableName and sTableName ~= "" then
        tb[#tb + 1] = sTab .. sTableName .. " = {\\n"
    else
        tb[#tb + 1] = sTab .. "{\\n"
    end

    for key, value in pairs(tTable) do
        local sKey = (type(key) == "string") and format("[%q]", key) or format("[%d]", key)
        if type(value) == "table" then

            tb[#tb + 1] = sTab .. "\\t" .. sKey .. " = " .. TaoBangNew(value, "", sTab .. "\\t") .. ",\\n"
        else
            local sValue = (type(value) == "string") and format("%q", value) or tostring(value)
            tb[#tb + 1] = sTab .. "\\t" .. sKey .. " = " .. sValue .. ",\\n"
        end
    end

    tb[#tb + 1] = sTab .. "}"
    local sTmp = table.concat(tb)\""""),
])

# ---- common.lua join
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

# ---- mailmanager
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

# ---- timerserver TS_GhiProf
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

# ---- functionlib (activitysys)
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
 ("""	szAwardMsg = gsub(szAwardMsg, "<enter>", "\\n")
	szAwardMsg = gsub(szAwardMsg, "<tab>", "\\t")

	return dostring(szAwardMsg)""",
  """	szAwardMsg = gsub(szAwardMsg, "<enter>", "\\n")
	szAwardMsg = gsub(szAwardMsg, "<tab>", "\\t")

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

# ---- task_string TalkEx
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

# ---- onkillnpc
patch(r"global\onkillnpc.lua", [
 ("""	call(BDH_OnKillNpc, {nNpcIndex}, "x")""",
  """	-- [PA1 06/09 toi] xpcall thay call(...,"x") (moi lan giet quai): loi van vao _ERRORMESSAGE (ScriptError.log)
	xpcall(BDH_OnKillNpc, function(m) if type(_ERRORMESSAGE) == "function" then _ERRORMESSAGE(tostring(m)) end return m end, nNpcIndex)"""),
])

# ---- dailogsay
patch(r"nhiemvu\hoithoai\dailogsay.lua", [
 ("""			local pFun = tbOpt[nSelectId][2]
			local tbParam = tbOpt[nSelectId][3]

			call(pFun, tbParam)	""",
  """			local pFun = tbOpt[nSelectId][2]
			local tbParam = tbOpt[nSelectId][3]

			pFun(table.unpack(tbParam, 1, getn(tbParam)))	-- [PA1 06/09 toi] goi thang thay call(): loi van lan ra nhu cu"""),
])
print("XONG PA-1")
