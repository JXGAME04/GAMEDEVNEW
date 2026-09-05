@echo off
rem ============================================================
rem  CHAY GAMESERVER - tu cap nhat build moi roi mo server
rem  Quy uoc: ban build moi duoc dat canh voi ten <file>.moi
rem  (CoreServer.dll.moi, heaven.dll.moi, engine.dll.moi). Chi can TAT SERVER
rem  roi chay lai file nay la an ban moi.
rem
rem  [LUA54 05/09] Nang Lua 4.0 -> 5.4: khi co dau hieu LUA54.moi thi doi cay
rem  script: script (Lua 4) -> script.lua4, script54 (sinh lai ngay luc nay bang
rem  tools\chuyen_lua4_54.py tu cay script hien tai) -> script. Lui: LuiLua4.bat.
rem ============================================================
cd /d "%~dp0"

call :capnhat CoreServer.dll
call :capnhat heaven.dll
call :capnhat engine.dll

if exist "LUA54.moi" call :doiscript54

start "" "%~dp0GameServer.exe"
exit /b

rem CoreServer.dll phai la ban dung Lua54Dll (tim chuoi "Lua54Dll.dll" trong file) moi doi cay script.
:kiem_lua54_dll
findstr /m /i /c:"Lua54Dll.dll" "CoreServer.dll" >nul 2>&1
exit /b

:capnhat
if exist "%~1.moi" (
    if exist "%~1" (
        del /f /q "%~1.truoc" >nul 2>&1
        ren "%~1" "%~1.truoc" >nul 2>&1
    )
    ren "%~1.moi" "%~1"
    echo [Cap nhat] %~1 da len ban moi
)
exit /b

:doiscript54
call :kiem_lua54_dll
if errorlevel 1 (
    echo [LUA54] CoreServer.dll hien tai KHONG dung Lua54Dll - chua doi cay script (chay lai khi da co CoreServer.dll ban 5.4)
    exit /b
)
if exist "script.lua4\nul" (
    echo [LUA54] script.lua4 da co - cay script hien tai da la ban 5.4
    ren "LUA54.moi" "LUA54.da_doi"
    exit /b
)
if not exist "tools\chuyen_lua4_54.py" (
    echo [LUA54] THIEU tools\chuyen_lua4_54.py - KHONG doi cay script. Bao Claude.
    exit /b
)
echo [LUA54] Dang chuyen cay script (Lua 4 -> 5.4) vao script54 ...
python "tools\chuyen_lua4_54.py" "script" "script54" --baocao "tools\chuyen_baocao.txt"
if errorlevel 1 (
    echo [LUA54] CHUYEN THAT BAI - giu nguyen cay script cu. Bao Claude.
    exit /b
)
if not exist "script54\LUA54_DA_CHUYEN.txt" (
    echo [LUA54] script54 thieu dau hieu LUA54_DA_CHUYEN.txt - giu nguyen cay cu. Bao Claude.
    exit /b
)
ren "script" "script.lua4"
ren "script54" "script"
ren "LUA54.moi" "LUA54.da_doi"
echo [LUA54] Da doi: script = ban 5.4 (ban cu giu o script.lua4)
exit /b
