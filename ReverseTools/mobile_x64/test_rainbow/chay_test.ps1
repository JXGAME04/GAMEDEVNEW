# [SDL 08/09 2b-2] Dich harness (cl x64) roi chay voi may chu gia cho tung thu muc Rainbow.dll.
# Dung: powershell -File chay_test.ps1 [-Dirs "D:\...\bin\client64","D:\...\bin\client64sdl"] [-Port 27015] [-Gop] [-Cham 0]
param(
  [string[]]$Dirs = @("D:\GAMEDEVNEW_wt_mobile\bin\client64", "D:\GAMEDEVNEW_wt_mobile\bin\client64sdl"),
  [int]$Port = 27015, [switch]$Gop, [int]$Cham = 0, [switch]$KhongDich
)
$here = $PSScriptRoot
$exe = Join-Path $here "test_rainbow.exe"
if (-not $KhongDich -or -not (Test-Path $exe)) {
  $vc = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  $cmd = "call `"$vc`" >nul && cd /d `"$here`" && cl /nologo /EHsc /MT /W3 /I `"D:\GAMEDEVNEW_wt_mobile\Sources\MultiServer\Rainbow`" test_rainbow.cpp /Fe:test_rainbow.exe /link ole32.lib uuid.lib"
  & cmd /c $cmd 2>&1 | ForEach-Object { "  cl: $_" }
  if (-not (Test-Path $exe)) { "KHONG dich duoc harness"; exit 2 }
}
$rc = 0
foreach ($d in $Dirs) {
  "===== $d ====="
  if (-not (Test-Path (Join-Path $d "Rainbow.dll"))) { "  THIEU Rainbow.dll"; $rc = 1; continue }
  $srvArgs = @("$here\may_chu_gia.py", "--port", "$Port", "--thoi-gian", "40", "--cham", "$Cham"); if ($Gop) { $srvArgs += "--gop" }
  $srvOut = Join-Path $here ("may_chu_" + (Split-Path $d -Leaf) + ".log")
  $srv = Start-Process -FilePath python -ArgumentList $srvArgs -PassThru -NoNewWindow -RedirectStandardOutput $srvOut
  Start-Sleep -Milliseconds 800
  & $exe $d $Port 5 2>&1 | ForEach-Object { "  $_" }
  $code = $LASTEXITCODE
  if ($code -ne 0) { $rc = 1 }
  "  ma thoat harness: $code"
  try { Stop-Process -Id $srv.Id -Force -ErrorAction SilentlyContinue } catch {}
  Start-Sleep -Milliseconds 300
  "  --- may chu gia:"; Get-Content $srvOut | ForEach-Object { "  $_" }
  $Port += 2
}
"KET QUA CHUNG: " + $(if ($rc -eq 0) { "PASS" } else { "FAIL" })
exit $rc
