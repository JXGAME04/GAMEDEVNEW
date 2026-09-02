# lua4.exe - Lua 4.0 CUA ENGINE (build tu Sources\Library\LuaLib\src + baselib) - dung kiem script offline

Build (VS2022 x86): copy src\{lapi,lcode,ldebug,ldo,lfunc,lgc,llex,lmem,lobject,lparser,lstate,lstring,ltable,ltm,lundump,lvm,lzio,lua}.c
+ src\*.h + src\baselib\*.c vao 1 thu muc, roi trong "vcvars32.bat":
    cl /nologo /O1 /D_CRT_SECURE_NO_WARNINGS /I . /Fe:lua4.exe *.c
(bo luac.c, ltests.c, opt.c, print.c, stubs.c, dump.c, LuaExtend.c)

Dung:
    lua4.exe -s100 file.lua          # -sN = kich co stack; 100 = KLuaScript() mac dinh (client nap Lua ky nang: KSkills.cpp #ifndef _SERVER)
    lua4.exe file.lua                # 1024 = KLuaScript::Init() (g_ScriptSet server)
Kiem "stack Overflow" cua bang SKILLS={...} lon (02/09: wuhuntang.lua client VLTK). Ham game (AddItem...) KHONG co -> chi kiem chunk/thuan Lua.
lua4.exe khong commit git (binary); build lai theo huong dan tren khi can.
