@echo off
rem build_b0.cmd - dich b0_d3d11_ram.cpp 32 bit (nhu Game.exe). Ket qua: b0_d3d11_ram.exe canh tep nguon.
setlocal
cd /d "%~dp0"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul
cl /nologo /O2 /EHsc /W3 b0_d3d11_ram.cpp /Fe:b0_d3d11_ram.exe /link /SUBSYSTEM:CONSOLE
if errorlevel 1 (echo BUILD LOI & exit /b 1)
echo BUILD OK
del /q b0_d3d11_ram.obj 2>nul
