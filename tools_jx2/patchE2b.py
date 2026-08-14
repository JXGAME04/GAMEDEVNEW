# -*- coding: utf-8 -*-
# E2b: dang ky KJx2League (28 LG_/LGM_ + DoScript + GlbMission/Timer) + hook Breathe
# (utf-8 vi anchor sua DIEUTRA_CONGTHANH_BINARY.md co tieng Viet; moi thao tac file van la BYTE)
SRC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608"

def rd(p): return open(p, 'rb').read()
def wr(p, d): open(p, 'wb').write(d)
def hb(d): return sum(1 for b in d if b >= 0x80)

FUNCS = [
    'LG_CreateLeagueObj', 'LG_FreeLeagueObj', 'LG_SetLeagueInfo', 'LG_AddMemberToObj',
    'LG_ApplyAddLeague', 'LG_ApplyRemoveLeague',
    'LG_GetLeagueObj', 'LG_GetLeagueObjByRole', 'LG_GetFirstLeague', 'LG_GetNextLeague',
    'LG_GetLeagueInfo', 'LG_GetLeagueCreateTime', 'LG_GetMemberCount', 'LG_GetMemberInfo',
    'LG_GetMemberObj', 'LG_GetMemberJoinTime',
    'LG_GetLeagueTask', 'LG_ApplySetLeagueTask', 'LG_ApplyAppendLeagueTask',
    'LG_GetMemberTask', 'LG_ApplySetMemberTask', 'LG_ApplyAppendMemberTask',
    'LGM_CreateMemberObj', 'LGM_SetMemberInfo', 'LGM_ApplyAddMember',
    'LGM_ApplyRemoveMember', 'LGM_FreeMemberObj',
    'LG_ApplyDoScript',
    'OpenGlbMission', 'StartGlbMSTimer', 'StopGlbMSTimer',
]

# ---------- 1) ScriptFuns.cpp ----------
p = SRC + r"\Sources\Core\Src\ScriptFuns.cpp"
d = rd(p); h0 = hb(d)
assert b'LuaLG_CreateLeagueObj' not in d
a = b'extern int LuaGetGlbValue(Lua_State* L);\r\n#endif'
assert d.count(a) == 1
ext = b'extern int LuaGetGlbValue(Lua_State* L);\r\n'
ext += b'// ==== DOT E cong thanh JX2: League/GlbMission engine-side (KJx2League.cpp) ====\r\n'
for f in FUNCS:
    ext += b'extern int Lua' + f.encode() + b'(Lua_State* L);\r\n'
ext += b'#endif'
d = d.replace(a, ext)

a = b'\t\t{ "GetGlbValue",\tLuaGetGlbValue },\r\n#endif'
assert d.count(a) == 1
tbl = b'\t\t{ "GetGlbValue",\tLuaGetGlbValue },\r\n'
tbl += b'\t\t// ==== DOT E cong thanh JX2: League + GLOBAL mission timer (KJx2League) ====\r\n'
for f in FUNCS:
    tbl += b'\t\t{ "' + f.encode() + b'",\tLua' + f.encode() + b' },\r\n'
tbl += b'#endif'
d = d.replace(a, tbl)
assert hb(d) == h0
wr(p, d); print('OK ScriptFuns.cpp (+%d extern + entry)' % len(FUNCS))

# ---------- 2) CoreServerShell.cpp: include + hook Breathe ----------
p = SRC + r"\Sources\Core\Src\CoreServerShell.cpp"
d = rd(p); h0 = hb(d)
assert b'KJx2League.h' not in d
a = b'#include "KTongJX2.h"\t// JX2 port\r\n'
assert d.count(a) == 1
d = d.replace(a, a + b'#include "KJx2League.h"\t// DOT E cong thanh: GLOBAL mission timer\r\n')
a = (b'\t}\r\n'
     b'\r\n'
     b'\tg_SubWorldSet.MessageLoop();\r\n'
     b'\tg_SubWorldSet.MainLoop();\r\n')
assert d.count(a) == 1
new = (b'\t}\r\n'
       b'\r\n'
       b'\tKJx2GlbMission_Breathe();\t// DOT E: nhip timer GLOBAL mission (cong thanh 5\')\r\n'
       b'\r\n'
       b'\tg_SubWorldSet.MessageLoop();\r\n'
       b'\tg_SubWorldSet.MainLoop();\r\n')
d = d.replace(a, new)
assert hb(d) == h0
wr(p, d); print('OK CoreServerShell.cpp (include + Breathe hook)')

# ---------- 3) Core.vcxproj ----------
p = SRC + r"\Sources\Core\Core.vcxproj"
d = rd(p)
assert b'KJx2League' not in d
a = b'    <ClCompile Include="Src\\KJx2SharedStore.cpp">'
assert d.count(a) == 1
excl = (b'    <ClCompile Include="Src\\KJx2League.cpp">\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Debug|Win32\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Debug|x64\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Release|Win32\'">true</ExcludedFromBuild>\r\n'
        b'      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Client Release|x64\'">true</ExcludedFromBuild>\r\n'
        b'    </ClCompile>\r\n')
d = d.replace(a, excl + a)
a = b'    <ClInclude Include="Src\\KJx2SharedStore.h">'
assert d.count(a) == 1
d = d.replace(a, b'    <ClInclude Include="Src\\KJx2League.h">\r\n    </ClInclude>\r\n' + a)
wr(p, d); print('OK Core.vcxproj')

# ---------- 4) DIEUTRA doc: sua chu ky StartGlbMSTimer ----------
p = SRC + r"\DIEUTRA_CONGTHANH_BINARY.md"
d = rd(p)
a = '`StartGlbMSTimer(nMissionId, nHour?)` / `StopGlbMSTimer` — timer cho GLOBAL mission\n  (g_GlobalMissionArray ta chỉ có Set/Get value) — citywar_global/mission.lua:6,14 cần.'.encode('utf-8')
assert d.count(a) == 1, 'anchor DIEUTRA khong khop'
new = ('`StartGlbMSTimer(nMissionID, nTimerID, nIntervalFrames)` — 3 THAM SỐ, timer LẶP VÔ HẠN\n'
       '  (18 frame = 1 giây; citywar dùng (8, 18, 5*60*18) = 5 phút; đáo hạn chạy OnTimer() của\n'
       '  script TimerTask.txt[nTimerID]) / `StopGlbMSTimer(nMissionID, nTimerID)` — 2 tham số;\n'
       '  kèm `OpenGlbMission(id)` (autoexec gốc :142 — gọi InitMission() của missions.txt[id]).').encode('utf-8')
d = d.replace(a, new)
wr(p, d); print('OK DIEUTRA_CONGTHANH_BINARY.md (StartGlbMSTimer 3 tham so)')
print('=== E2b patch DONE ===')
