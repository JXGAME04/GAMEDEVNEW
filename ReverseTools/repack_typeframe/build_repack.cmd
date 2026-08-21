@echo off
rem Build repack_typeframe.exe (x86) using the ucl-1.01 sources shipped with Sources\Pack.
rem MUST be 32-bit: ucl-1.01's ucl_init() self-check truncates pointers on x64 and fails.
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d "%~dp0"
set UCL=..\..\Sources\Pack\ucl-1.01
cl /nologo /O2 /MT /W3 /I "%UCL%\include" /I "%UCL%\src" ^
    repack_typeframe.cpp ^
    "%UCL%\src\n2b_99.c" "%UCL%\src\n2b_d.c" "%UCL%\src\n2b_ds.c" ^
    "%UCL%\src\ucl_init.c" "%UCL%\src\alloc.c" "%UCL%\src\ucl_util.c" ^
    "%UCL%\src\ucl_ptr.c" "%UCL%\src\ucl_str.c" "%UCL%\src\ucl_crc.c" ^
    /Fe:repack_typeframe.exe
echo exit=%ERRORLEVEL%
