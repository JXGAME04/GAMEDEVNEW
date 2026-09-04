@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d "%~dp0"
cl /nologo /MD /O2 /EHsc /W1 /wd4996 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /I"D:\GAMEDEVNEW_wt_rep3\Sources\Engine\Src" /I"D:\GAMEDEVNEW_wt_rep3\Sources\Represent\iRepresent" rep_harness.cpp /Fe:rep_harness.exe /link /LIBPATH:"D:\GAMEDEVNEW_wt_rep3\Lib\release" engine.lib user32.lib gdi32.lib
echo exit=%ERRORLEVEL%
