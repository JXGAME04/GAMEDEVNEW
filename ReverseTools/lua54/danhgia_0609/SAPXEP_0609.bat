@echo off
setlocal EnableDelayedExpansion
rem ============================================================
rem  SAPXEP_0609.bat - SAP XEP LAI CAY SCRIPT (mot nut) - chay khi GameServer DA TAT
rem   1. thay CoreServer.dll / Lua54Dll.dll bang ban .moi (co bi danh duong dan)
rem   2. doi cho 1.419 tep theo bo cuc tieng Viet (tools\sapxep\r33_sapxep.py) + sinh script\_duongdan_cu.txt
rem   3. kiem: tep xuat hien lai o duong dan cu; mo phong boot (boot_gia.py) so voi truoc khi doi
rem   4. dong bo guong git D:\GAMEDEVNEW\serverscript_live (robocopy /MIR)
rem   5. mo GameServer.exe
rem  Lui: script\_duongdan_cu.txt + tools\sapxep\r33_kehoach.txt cho biet cu->moi; python tools\sapxep\r33_lui.py
rem ============================================================
cd /d "%~dp0"
set PY=python
where python >nul 2>&1 || set "PY=%LocalAppData%\Programs\Python\Python312\python.exe"
tasklist /FI "IMAGENAME eq GameServer.exe" 2>nul | find /I "GameServer.exe" >nul
if not errorlevel 1 (
    echo [LOI] GameServer.exe dang chay - tat server truoc roi chay lai.
    pause
    exit /b 1
)
if not exist "script\LUA54_DA_CHUYEN.txt" (
    echo [LOI] khong o thu muc bin\server?
    pause
    exit /b 1
)
set LOI=0
call :capnhat CoreServer.dll
call :capnhat Lua54Dll.dll
if "%LOI%"=="1" goto :loi

echo [SAPXEP] doi cho tep ...
"%PY%" tools\sapxep\r33_sapxep.py tools\sapxep thuc_hien > tools\sapxep\r33_ketqua.txt 2>&1
if errorlevel 1 (
    type tools\sapxep\r33_ketqua.txt
    echo [LOI] r33_sapxep.py that bai - KHONG mo server, bao Claude.
    set LOI=1
    goto :loi
)
findstr /C:"DA DOI CHO" tools\sapxep\r33_ketqua.txt

echo [SAPXEP] kiem tep o duong dan cu ...
"%PY%" tools\sapxep\kiem_duongdan_cu.py sua

echo [SAPXEP] mo phong boot voi cay moi (30-60 giay) ...
"%PY%" tools\sapxep\boot_gia.py "%~dp0Lua54Dll.dll" cache 0 tools\sapxep\boot_loi_sau.txt > tools\sapxep\boot_sau.log 2>&1
findstr /C:"TONG" tools\sapxep\boot_sau.log
"%PY%" tools\sapxep\so_boot.py tools\sapxep\boot_loi_truoc.txt tools\sapxep\boot_loi_sau.txt script\_duongdan_cu.txt

echo [SAPXEP] dong bo guong git serverscript_live ...
robocopy "script" "D:\GAMEDEVNEW\serverscript_live\script" /MIR /NJH /NJS /NFL /NDL /XF ScriptError.log >nul
robocopy "scriptjx2" "D:\GAMEDEVNEW\serverscript_live\scriptjx2" /MIR /NJH /NJS /NFL /NDL /XF ScriptError.log >nul

echo.
echo [OK] Sap xep xong. Mo GameServer.exe ... (nhin dong "[script] LoadAllScript" va "[script] Bi danh duong dan" tren console)
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
        echo [Cap nhat] LOI: khong doi ten duoc %~1 - dang bi mo?
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
set /a N=0
:ren_lap
ren "%~1" "%~2" >nul 2>&1
if not exist "%~1" exit /b 0
set /a N+=1
if !N! geq 5 exit /b 1
timeout /t 2 /nobreak >nul
goto :ren_lap
