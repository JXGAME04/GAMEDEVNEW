@echo off
set MSB="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set WT=D:\GAMEDEVNEW_wt_lua54b
set ST=%WT%\build_status.txt
echo [%time%] START > %ST%
%MSB% %WT%\Sources\Library\Lua54\Lua54Dll.vcxproj -p:Configuration=Release -p:Platform=x64 -m:1 -nologo -v:m > %WT%\build_dll64.log 2>&1
echo [%time%] DLL64 EXIT %ERRORLEVEL% >> %ST%
%MSB% %WT%\Sources\Library\Lua54\Lua54Dll.vcxproj -p:Configuration=Release -p:Platform=Win32 -m:1 -nologo -v:m > %WT%\build_dll32.log 2>&1
echo [%time%] DLL32 EXIT %ERRORLEVEL% >> %ST%
cd /d %WT%\Sources\Core
%MSB% Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64 -p:SolutionDir=%WT%\Sources\ -m:1 -p:PostBuildEventUseInBuild=false -nologo -v:m > %WT%\build_server.log 2>&1
echo [%time%] SERVER EXIT %ERRORLEVEL% >> %ST%
echo DONE >> %ST%
