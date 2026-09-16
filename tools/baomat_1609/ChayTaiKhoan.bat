@echo off
setlocal
rem ============================================================
rem  CHAY SWORD3PAYSYS (may chu tai khoan) - tu cap nhat ban moi roi mo  [BAOMAT 16/09]
rem  Quy uoc giong ChayS3Relay.bat: ban build moi dat canh voi ten
rem  Sword3PaySys.exe.moi. TAT Sword3PaySys roi chay lai file nay la an ban moi
rem  (ban cu duoc giu thanh Sword3PaySys.exe.truoc de lui).
rem
rem  LUU Y: GameServer dang nhap vao may chu tai khoan luc mo. Sau khi mo
rem  Sword3PaySys moi, neu GameServer bao mat ket noi tai khoan thi mo lai
rem  GameServer (bin\server\ChayGameServer.bat).
rem ============================================================
cd /d "%~dp0"

if not exist "Sword3PaySys.exe.moi" goto :mo

echo [Cap nhat] Sword3PaySys.exe : co ban moi (.moi)
tasklist /FI "IMAGENAME eq Sword3PaySys.exe" 2>nul | find /I "Sword3PaySys.exe" >nul
if not errorlevel 1 (
    echo [Cap nhat] LOI: Sword3PaySys.exe dang chay - tat no roi chay lai file nay.
    pause
    exit /b 1
)
if exist "Sword3PaySys.exe.truoc" del /f /q "Sword3PaySys.exe.truoc" >nul 2>&1
if exist "Sword3PaySys.exe" ren "Sword3PaySys.exe" "Sword3PaySys.exe.truoc"
if exist "Sword3PaySys.exe" (
    echo [Cap nhat] LOI: khong doi ten duoc Sword3PaySys.exe - dang bi mo?
    pause
    exit /b 1
)
ren "Sword3PaySys.exe.moi" "Sword3PaySys.exe"
if not exist "Sword3PaySys.exe" (
    echo [Cap nhat] LOI: khong dua duoc Sword3PaySys.exe.moi vao - tra lai ban cu.
    if exist "Sword3PaySys.exe.truoc" ren "Sword3PaySys.exe.truoc" "Sword3PaySys.exe"
    pause
    exit /b 1
)
echo [Cap nhat] Xong: Sword3PaySys.exe la ban moi, ban cu = Sword3PaySys.exe.truoc

:mo
echo [OK] Mo Sword3PaySys.exe ...
start "" "%~dp0Sword3PaySys.exe"
timeout /t 2 /nobreak >nul
exit /b 0
