# [SDL 08/09 2b-2] Ap luc: may chu gia day N goi/giay, harness gui ping 20/s va tham GetPackFromServer 1 ms; so RTT/khoang cach giua 2 ban Rainbow.
# Dung: powershell -File chay_apluc.ps1 [-Dirs ...] [-Day 40] [-Giay 12] [-Port 27100]
param(
  [string[]]$Dirs = @("D:\GAMEDEVNEW_wt_mobile\bin\client64", "D:\GAMEDEVNEW_wt_mobile\bin\client64sdl"),
  [int]$Day = 40, [int]$Giay = 12, [int]$Port = 27100
)
$here = $PSScriptRoot
$exe = Join-Path $here "test_rainbow.exe"
if (-not (Test-Path $exe)) { "chua co test_rainbow.exe - chay chay_test.ps1 truoc"; exit 2 }
$rc = 0
foreach ($d in $Dirs) {
  "===== $d (day $Day goi/s, $Giay s) ====="
  $srvOut = Join-Path $here ("may_chu_apluc_" + (Split-Path $d -Leaf) + ".log")
  $srvArgs = @("$here\may_chu_gia.py", "--port", "$Port", "--thoi-gian", "$($Giay + 20)", "--day", "$Day")
  $srv = Start-Process -FilePath python -ArgumentList $srvArgs -PassThru -NoNewWindow -RedirectStandardOutput $srvOut
  Start-Sleep -Milliseconds 800
  & $exe $d $Port 5 $Giay 2>&1 | ForEach-Object { "  $_" }
  if ($LASTEXITCODE -ne 0) { $rc = 1 }
  try { Stop-Process -Id $srv.Id -Force -ErrorAction SilentlyContinue } catch {}
  Start-Sleep -Milliseconds 300
  "  --- may chu gia:"; Get-Content $srvOut | Select-Object -Last 4 | ForEach-Object { "  $_" }
  $Port += 2
}
"KET QUA CHUNG: " + $(if ($rc -eq 0) { "PASS" } else { "FAIL" })
exit $rc
