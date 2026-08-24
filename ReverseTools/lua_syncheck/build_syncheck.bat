@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul
set S=D:\GAMEDEVNEW\Sources\Library\LuaLib\src
set OUT=C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\667a822b-6945-44b9-8b84-5bf2eac05ce5\scratchpad\luac
set SC=C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\667a822b-6945-44b9-8b84-5bf2eac05ce5\scratchpad\syncheck.c
if not exist "%OUT%" mkdir "%OUT%"
cd /d "%OUT%"
cl /nologo /O2 /D_CRT_SECURE_NO_WARNINGS /DNDEBUG /I"%S%" /I"%S%\baselib" ^
 "%SC%" ^
 "%S%\lapi.c" "%S%\lcode.c" "%S%\ldebug.c" "%S%\ldo.c" "%S%\lfunc.c" "%S%\lgc.c" ^
 "%S%\llex.c" "%S%\lmem.c" "%S%\lobject.c" "%S%\lparser.c" "%S%\lstate.c" ^
 "%S%\lstring.c" "%S%\ltable.c" "%S%\ltm.c" "%S%\lundump.c" "%S%\lvm.c" "%S%\lzio.c" ^
 "%S%\baselib\lauxlib.c" "%S%\baselib\lbaselib.c" "%S%\baselib\lstrlib.c" ^
 "%S%\baselib\lmathlib.c" "%S%\baselib\liolib.c" "%S%\baselib\ldblib.c" ^
 /Fe:syncheck.exe > sc.log 2>&1
echo exit=%ERRORLEVEL%
if exist syncheck.exe (echo SYNCHECK OK) else (findstr /C:"error" sc.log)
