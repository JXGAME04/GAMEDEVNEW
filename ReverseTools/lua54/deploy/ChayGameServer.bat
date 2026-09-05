@echo off
rem ============================================================
rem  CHAY GAMESERVER - tu cap nhat build moi roi mo server
rem  Quy uoc: ban build moi duoc dat canh voi ten <file>.moi
rem  (CoreServer.dll.moi, heaven.dll.moi, engine.dll.moi). Chi can TAT SERVER
rem  roi chay lai file nay la an ban moi.
rem
rem  [LUA54 05/09] Nang Lua 4.0 -> 5.4: khi co dau hieu LUA54.moi, goi
rem  tools\doi_lua54.py (kiem binary dung Lua54Dll, chuyen script -> script54,
rem  doi ten script/script.lua4). Doi that bai thi KHONG mo server. Lui: LuiLua4.bat.
rem ============================================================
cd /d "%~dp0"
set LOI_SWAP=0

call :capnhat CoreServer.dll
call :capnhat heaven.dll
call :capnhat engine.dll
if "%LOI_SWAP%"=="1" (
    echo [Cap nhat] CO FILE .moi KHONG THAY DUOC - khong mo server. Kiem tra thu muc roi chay lai.
    exit /b 1
)

if exist "LUA54.moi" (
    python "tools\doi_lua54.py" server
    if errorlevel 1 (
        echo [LUA54] Doi cay script THAT BAI - KHONG mo server. Xem thong bao tren.
        exit /b 1
    )
)

start "" "%~dp0GameServer.exe"
exit /b 0

:capnhat
if not exist "%~1.moi" exit /b 0
if exist "%~1" (
    del /f /q "%~1.truoc" >nul 2>&1
    ren "%~1" "%~1.truoc"
    if exist "%~1" (
        echo [Cap nhat] KHONG doi ten duoc %~1 (dang bi mo?) - giu nguyen
        set LOI_SWAP=1
        exit /b 1
    )
)
ren "%~1.moi" "%~1"
if exist "%~1.moi" (
    echo [Cap nhat] KHONG dua duoc %~1.moi vao - giu nguyen
    set LOI_SWAP=1
    exit /b 1
)
echo [Cap nhat] %~1 da len ban moi
exit /b 0
