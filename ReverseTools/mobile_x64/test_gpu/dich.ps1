$here = $PSScriptRoot
$vc = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
$cmd = "call `"$vc`" >nul && cd /d `"$here`" && cl /nologo /EHsc /MD /W3 /I `"D:\GAMEDEVNEW_wt_mobile\ThirdParty\SDL3\include`" gpu_test.cpp /Fe:gpu_test.exe /link `"D:\GAMEDEVNEW_wt_mobile\ThirdParty\SDL3\lib\x64\SDL3.lib`" /SUBSYSTEM:CONSOLE"
& cmd /c $cmd 2>&1 | ForEach-Object { "  cl: $_" }
if (Test-Path (Join-Path $here "gpu_test.exe")) { & (Join-Path $here "gpu_test.exe") 2>&1 | ForEach-Object { "  $_" } } else { "KHONG dich duoc" }
