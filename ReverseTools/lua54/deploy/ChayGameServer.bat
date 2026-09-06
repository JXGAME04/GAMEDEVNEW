@echo off
setlocal EnableDelayedExpansion
rem ============================================================
rem  CHAY GAMESERVER - tu cap nhat build moi roi mo server
rem  Quy uoc: ban build moi duoc dat canh voi ten <file>.moi
rem  (CoreServer.dll.moi, heaven.dll.moi, engine.dll.moi). Chi can TAT SERVER
rem  roi chay lai file nay la an ban moi.
rem
rem  [LUA54 05/09 v3] Nang Lua 4.0 -> 5.4:
rem   - Khi co LUA54.moi va Claude da chuan bi san cay script54 (co dau
rem     script54\LUA54_DA_CHUYEN.txt) thi CHI DOI TEN: script -> script.lua4,
rem     script54 -> script (lenh cmd thuan, khong can python).
rem   - Chua co script54 thi goi python tools\doi_lua54.py server (tu chuyen).
rem   - Moi buoc doi ten thu lai 5 lan (Defender co the giu tep vai giay);
rem     buoc nao hong thi TRA LAI ban cu, KHONG mo server, va DUNG LAI (pause)
rem     de doc loi. Thanh cong thi mo GameServer.exe.
rem ============================================================
cd /d "%~dp0"
set LOI=0

call :capnhat CoreServer.dll
call :capnhat heaven.dll
call :capnhat engine.dll
call :capnhat Lua54Dll.dll
if "%LOI%"=="1" goto :loi

if exist "LUA54.moi" call :doiscript
if "%LOI%"=="1" goto :loi

echo [OK] Mo GameServer.exe ...
start "" "%~dp0GameServer.exe"
timeout /t 3 /nobreak >nul
exit /b 0

:loi
echo.
echo ===== KHONG MO SERVER - doc loi o tren, chup man hinh gui Claude =====
pause
exit /b 1

:capnhat
if not exist "%~1.moi" exit /b 0
echo [Cap nhat] %~1 : co ban moi (.moi)
if exist "%~1" (
    if exist "%~1.truoc" del /f /q "%~1.truoc" >nul 2>&1
    call :ren_thu "%~1" "%~1.truoc"
    if exist "%~1" (
        echo [Cap nhat] LOI: khong doi ten duoc %~1 - dang bi mo? Tat GameServer roi chay lai.
        set LOI=1
        exit /b 1
    )
)
call :ren_thu "%~1.moi" "%~1"
if exist "%~1.moi" (
    echo [Cap nhat] LOI: khong dua duoc %~1.moi vao - tra lai ban cu.
    if exist "%~1.truoc" ren "%~1.truoc" "%~1"
    set LOI=1
    exit /b 1
)
echo [Cap nhat] %~1 da len ban moi
exit /b 0

:ren_thu
rem doi ten "%~1" -> "%~2", thu lai toi da 5 lan cach 2 giay
set /a N=0
:ren_lap
ren "%~1" "%~2" >nul 2>&1
if not exist "%~1" exit /b 0
set /a N+=1
if !N! geq 5 exit /b 1
timeout /t 2 /nobreak >nul
goto :ren_lap

:doiscript
if exist "script54\LUA54_DA_CHUYEN.txt" if exist "script54\timerserver.lua" goto :doiten
echo [LUA54] chua co script54 chuan bi san - goi python tools\doi_lua54.py server
set PY=python
where python >nul 2>&1 || set "PY=%LocalAppData%\Programs\Python\Python312\python.exe"
"%PY%" "tools\doi_lua54.py" server
if errorlevel 1 (
    set LOI=1
    exit /b 1
)
exit /b 0

:doiten
if exist "script.lua4" (
    echo [LUA54] LOI: da co script.lua4 tu truoc - trang thai la, bao Claude
    set LOI=1
    exit /b 1
)
echo [LUA54] doi ten script -^> script.lua4 ...
call :ren_thu "script" "script.lua4"
if exist "script" (
    echo [LUA54] LOI: khong doi ten duoc thu muc script - co tep dang bi mo?
    set LOI=1
    exit /b 1
)
echo [LUA54] doi ten script54 -^> script ...
call :ren_thu "script54" "script"
if exist "script54" (
    echo [LUA54] LOI: khong doi ten duoc script54 - tra lai script cu.
    ren "script.lua4" "script"
    set LOI=1
    exit /b 1
)
ren "LUA54.moi" "LUA54.da_doi"
echo [LUA54] XONG: script = ban Lua 5.4, ban cu giu o script.lua4
exit /b 0
