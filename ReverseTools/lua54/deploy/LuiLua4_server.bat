@echo off
rem ============================================================
rem  LUI LAI LUA 4 (may chu) - chay khi ban Lua 5.4 co van de. TAT GameServer truoc.
rem  Tra lai CoreServer.dll / engine.dll ban .truoc va cay script Lua 4 (script.lua4).
rem  Khong xoa gi: ban 5.4 duoc doi ten giu lai (.lua54 / script54_xxx).
rem ============================================================
cd /d "%~dp0"
for %%F in (CoreServer.dll engine.dll) do (
    if exist "%%F.truoc" (
        if exist "%%F" ren "%%F" "%%F.lua54"
        ren "%%F.truoc" "%%F"
        echo [Lui] %%F ve ban truoc
    )
)
if exist "script.lua4\nul" (
    if exist "script\nul" ren "script" "script54_%RANDOM%"
    ren "script.lua4" "script"
    echo [Lui] cay script ve ban Lua 4
)
if exist "LUA54.da_doi" ren "LUA54.da_doi" "LUA54.da_lui"
echo Xong. Chay ChayGameServer.bat de mo lai.
