# -*- coding: ascii -*-
# E2a: dang ky KJx2SharedStore vao ScriptFuns.cpp + Core.vcxproj (+.filters)
SRC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608"

def rd(p): return open(p, 'rb').read()
def wr(p, d): open(p, 'wb').write(d)
def hb(d): return sum(1 for b in d if b >= 0x80)

# ---------- 1) ScriptFuns.cpp: extern (trong #ifdef _SERVER, sau TONG_DailyMaintainAll) ----------
p = SRC + r"\Sources\Core\Src\ScriptFuns.cpp"
d = rd(p); h0 = hb(d)
assert b'LuaOB_Create' not in d
a = b'extern int LuaTONG_DailyMaintainAll(Lua_State* L);\r\n#endif'
assert d.count(a) == 1
blk = (b'extern int LuaTONG_DailyMaintainAll(Lua_State* L);\r\n'
       b'// ==== DOT E cong thanh JX2: kho chia se giua cac Lua state (KJx2SharedStore.cpp) ====\r\n'
       b'extern int LuaOB_Create(Lua_State* L);\r\n'
       b'extern int LuaOB_Release(Lua_State* L);\r\n'
       b'extern int LuaOB_IsEmpty(Lua_State* L);\r\n'
       b'extern int LuaOB_Clear(Lua_State* L);\r\n'
       b'extern int LuaOB_Append(Lua_State* L);\r\n'
       b'extern int LuaOB_Copy(Lua_State* L);\r\n'
       b'extern int LuaOB_PushByte(Lua_State* L);\r\n'
       b'extern int LuaOB_PopByte(Lua_State* L);\r\n'
       b'extern int LuaOB_PushInt(Lua_State* L);\r\n'
       b'extern int LuaOB_PopInt(Lua_State* L);\r\n'
       b'extern int LuaOB_PushDouble(Lua_State* L);\r\n'
       b'extern int LuaOB_PopDouble(Lua_State* L);\r\n'
       b'extern int LuaOB_PushString(Lua_State* L);\r\n'
       b'extern int LuaOB_PopString(Lua_State* L);\r\n'
       b'extern int LuaLadder_NewLadder(Lua_State* L);\r\n'
       b'extern int LuaLadder_ClearLadder(Lua_State* L);\r\n'
       b'extern int LuaLadder_GetLadderInfo(Lua_State* L);\r\n'
       b'extern int LuaSetGlbValue(Lua_State* L);\r\n'
       b'extern int LuaGetGlbValue(Lua_State* L);\r\n'
       b'#endif')
d = d.replace(a, blk)

# ---------- 2) ScriptFuns.cpp: bang dang ky (truoc dong TONG_DailyMaintainAll + #endif) ----------
a = b'\t\t{ "TONG_DailyMaintainAll",\tLuaTONG_DailyMaintainAll },\r\n#endif'
assert d.count(a) == 1
blk = (b'\t\t{ "TONG_DailyMaintainAll",\tLuaTONG_DailyMaintainAll },\r\n'
       b'\t\t// ==== DOT E cong thanh JX2: ObjBuffer + Ladder + GlbValue (KJx2SharedStore) ====\r\n'
       b'\t\t{ "OB_Create",\tLuaOB_Create },\r\n'
       b'\t\t{ "OB_Release",\tLuaOB_Release },\r\n'
       b'\t\t{ "OB_IsEmpty",\tLuaOB_IsEmpty },\r\n'
       b'\t\t{ "OB_Clear",\tLuaOB_Clear },\r\n'
       b'\t\t{ "OB_Append",\tLuaOB_Append },\r\n'
       b'\t\t{ "OB_Copy",\tLuaOB_Copy },\r\n'
       b'\t\t{ "OB_PushByte",\tLuaOB_PushByte },\r\n'
       b'\t\t{ "OB_PopByte",\tLuaOB_PopByte },\r\n'
       b'\t\t{ "OB_PushInt",\tLuaOB_PushInt },\r\n'
       b'\t\t{ "OB_PopInt",\tLuaOB_PopInt },\r\n'
       b'\t\t{ "OB_PushDouble",\tLuaOB_PushDouble },\r\n'
       b'\t\t{ "OB_PopDouble",\tLuaOB_PopDouble },\r\n'
       b'\t\t{ "OB_PushString",\tLuaOB_PushString },\r\n'
       b'\t\t{ "OB_PopString",\tLuaOB_PopString },\r\n'
       b'\t\t{ "Ladder_NewLadder",\tLuaLadder_NewLadder },\r\n'
       b'\t\t{ "Ladder_ClearLadder",\tLuaLadder_ClearLadder },\r\n'
       b'\t\t{ "Ladder_GetLadderInfo",\tLuaLadder_GetLadderInfo },\r\n'
       b'\t\t{ "SetGlbValue",\tLuaSetGlbValue },\r\n'
       b'\t\t{ "GetGlbValue",\tLuaGetGlbValue },\r\n'
       b'#endif')
d = d.replace(a, blk)
assert hb(d) == h0
wr(p, d); print('OK ScriptFuns.cpp (extern + 19 entry)')

# ---------- 3) Core.vcxproj ----------
p = SRC + r"\Sources\Core\Core.vcxproj"
d = rd(p)
assert b'KJx2SharedStore' not in d
a = b'    <ClCompile Include="Src\\KTongJX2.cpp">'
assert d.count(a) == 1
excl = (b'    <ClCompile Include="Src\\KJx2SharedStore.cpp">\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Debug|Win32\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Debug|x64\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Release|Win32\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Release|x64\'">true</ExcludedFromBuild>\r\n'
        b'    </ClCompile>\r\n')
d = d.replace(a, excl + a)
a = b'    <ClInclude Include="Src\\KTongJX2.h">'
assert d.count(a) == 1
d = d.replace(a, b'    <ClInclude Include="Src\\KJx2SharedStore.h">\r\n    </ClInclude>\r\n' + a)
wr(p, d); print('OK Core.vcxproj')

# ---------- 4) Core.vcxproj.filters (neu co) ----------
import os
p = SRC + r"\Sources\Core\Core.vcxproj.filters"
if os.path.exists(p):
    d = rd(p)
    if b'KTongJX2.cpp' in d and b'KJx2SharedStore' not in d:
        i = d.find(b'<ClCompile Include="Src\\KTongJX2.cpp"')
        j = d.find(b'</ClCompile>', i) + len(b'</ClCompile>')
        seg = d[i - 4:j]  # gom indent 4 space
        newseg = seg.replace(b'KTongJX2.cpp', b'KJx2SharedStore.cpp')
        d = d[:j] + b'\r\n' + newseg + d[j:]
        i = d.find(b'<ClInclude Include="Src\\KTongJX2.h"')
        if i > 0:
            j = d.find(b'</ClInclude>', i)
            if j > 0:
                j += len(b'</ClInclude>')
                seg = d[i - 4:j]
                newseg = seg.replace(b'KTongJX2.h', b'KJx2SharedStore.h')
                d = d[:j] + b'\r\n' + newseg + d[j:]
        wr(p, d); print('OK Core.vcxproj.filters')
    else:
        print('filters: bo qua (khong tim thay khuon hoac da co)')
else:
    print('filters: khong ton tai')
print('=== E2a patch DONE ===')
