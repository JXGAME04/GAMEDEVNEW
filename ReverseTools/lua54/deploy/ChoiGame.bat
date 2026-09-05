@echo off
rem ============================================================
rem  CHOI GAME - tu cap nhat build moi roi mo game
rem  Quy uoc: ban build moi duoc dat canh voi ten <file>.moi
rem  (Game.exe.moi / CoreClient.dll.moi / Engine.dll.moi / WAuto.dll.moi)
rem  Chi can THOAT GAME roi chay lai file nay la an ban moi.
rem
rem  [LUA54 05/09] Nang Lua 4.0 -> 5.4: khi co dau hieu LUA54.moi, goi
rem  tools\doi_lua54.py client (kiem binary dung Lua54Dll, chuyen script, doi ten).
rem  Doi that bai thi KHONG mo game. Lui: LuiLua4.bat.
rem ============================================================
cd /d "%~dp0"
set LOI_SWAP=0

call :capnhat Game.exe
call :capnhat CoreClient.dll
call :capnhat Engine.dll
call :capnhat Represent3.dll
call :capnhat Represent2.dll
call :capnhat WAuto.dll
if "%LOI_SWAP%"=="1" (
    echo [Cap nhat] CO FILE .moi KHONG THAY DUOC - khong mo game. Thoat game han roi chay lai.
    exit /b 1
)

if exist "LUA54.moi" (
    python "tools\doi_lua54.py" client
    if errorlevel 1 (
        echo [LUA54] Doi cay script THAT BAI - KHONG mo game. Xem thong bao tren.
        exit /b 1
    )
)

start "" "%~dp0Game.exe"
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
