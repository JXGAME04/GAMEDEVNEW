@echo off
rem ============================================================
rem  LUI LAI LUA 4 (client) - chay khi ban Lua 5.4 co van de. THOAT GAME truoc.
rem  Tra lai Game.exe / CoreClient.dll / Engine.dll ban .truoc va cay script Lua 4 (script.lua4).
rem  Khong xoa gi: ban 5.4 duoc doi ten giu lai.
rem ============================================================
cd /d "%~dp0"
for %%F in (Game.exe CoreClient.dll Engine.dll) do (
    if exist "%%F.truoc" (
        if exist "%%F" ren "%%F" "%%F.lua54"
        ren "%%F.truoc" "%%F"
        echo [Lui] %%F ve ban truoc
    )
)
if exist "script.lua4\nul" (
    if exist "script\nul" ren "script" "script54_%RANDOM%"
    ren "script.lua4" "script"
    echo [Lui] cay script client ve ban Lua 4
)
if exist "LUA54.da_doi" ren "LUA54.da_doi" "LUA54.da_lui"
echo Xong. Chay ChoiGame.bat de mo lai.
