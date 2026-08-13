@echo off
chcp 65001 >nul
REM ============================================================
REM  Day ma nguon JX1 len https://github.com/JXGAME04/GAMEDEVNEW.git
REM  Soan cho chu game ngay 12/08/2026.
REM  AN TOAN: KHONG dung "git add -A" (se nuot vcpkg_installed + x64 + Template).
REM ============================================================
setlocal
set REPO=E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608
cd /d "%REPO%" || (echo KHONG VAO DUOC %REPO% & pause & exit /b 1)

echo.
echo === B1. Chong doi byte file GBK/TCVN (BAT BUOC) ===
git config core.autocrlf false
git config core.autocrlf

echo.
echo === B2. Loai thu muc rac khoi git (them vao .gitignore neu chua co) ===
for %%D in ("vcpkg_installed/" "Template/" "x64/" "EngineServerDebug/" "EngineServerRelease/" "Lib/debug64/" "Lib/release64/" "Lib/x64/" "*.vcxproj.user") do (
  findstr /x /c:%%~D .gitignore >nul 2>&1 || echo %%~D>>.gitignore
)
echo    Da cap nhat .gitignore

echo.
echo === B3. Them remote GAMEDEVNEW (bo qua neu da co) ===
git remote add gamedevnew https://github.com/JXGAME04/GAMEDEVNEW.git 2>nul
git remote -v

echo.
echo === B4. Stage cac file DA THEO DOI bi sua/xoa ===
REM  "git add -u" CHI dong cac file git da theo doi -> khong the nuot rac moi
git add -u
git add .gitignore

echo.
echo === B5. Them cac file MA NGUON MOI (liet ke dich danh) ===
for %%F in (
  "Sources\Core\KMeridian.cpp"
  "Sources\Core\KMeridian.h"
  "Sources\Core\Src\BauCua.cpp"
  "Sources\Core\Src\BauCua.h"
  "Sources\Core\Src\GiftCodeManager.cpp"
  "Sources\Core\Src\GiftCodeManager.h"
  "Sources\Core\Src\MapHandler.cpp"
  "Sources\Core\Src\MapHandler.h"
  "Sources\Core\Src\base64.cpp"
  "Sources\Core\Src\base64.h"
  "Sources\Core\Src\ipc_shared.h"
  "Sources\MultiServer\GameServer\GameServerLog.h"
  "Sources\Core\vcpkg.json"
  "Sources\Core\vcpkg-configuration.json"
  "Sources\Engine\vcpkg.json"
  "Sources\Engine\vcpkg-configuration.json"
  "Sources\MultiServer\GameServer\vcpkg.json"
  "Sources\MultiServer\GameServer\vcpkg-configuration.json"
) do ( if exist %%F git add %%F )

echo.
echo === B6. Xem se commit nhung gi (chi dem, khong liet ke het) ===
git diff --cached --stat | tail -1

echo.
echo === B7. Commit ===
git commit -m "Snapshot nguon JX1 VS2022 truoc khi dung lai he bang hoi JX2"

echo.
echo === B8. Day len GAMEDEVNEW ===
git push -u gamedevnew master

echo.
echo ============================================================
echo  CON LAI CHUA DAY (xem qua roi tu quyet dinh co them khong):
echo ============================================================
git status --porcelain | findstr /b "??"

echo.
echo XONG. Kiem tra tren https://github.com/JXGAME04/GAMEDEVNEW
pause
