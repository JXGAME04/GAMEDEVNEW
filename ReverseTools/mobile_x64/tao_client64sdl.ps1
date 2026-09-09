# [SDL 08/09 2b] Tao bo thu nghiem bin\client64sdl giong bin\client64 (junction du lieu toi cay live, ban sao cau hinh),
# nhung Engine.dll/CoreClient.dll/GameSDL.exe la ban SDL (JX_PLATFORM_SDL). Chay lai an toan (khong ghi de tep da co tru DLL dung chung).
param([string]$WT = "D:\GAMEDEVNEW_wt_mobile")
$src = Join-Path $WT "bin\client64"
$dst = Join-Path $WT "bin\client64sdl"
if (-not (Test-Path $src)) { throw "khong co $src" }
New-Item -ItemType Directory -Force -Path $dst | Out-Null
# junction: sao dung dich cua client64
foreach ($j in @("data", "Spr", "maps", "maps2")) {
  $s = Join-Path $src $j; $d = Join-Path $dst $j
  if (Test-Path $d) { continue }
  $it = Get-Item $s -ErrorAction SilentlyContinue
  if ($it -and $it.LinkType -eq "Junction") { New-Item -ItemType Junction -Path $d -Target ($it.Target | Select-Object -First 1) | Out-Null; "junction $j -> $($it.Target | Select-Object -First 1)" }
  elseif ($it) { New-Item -ItemType Junction -Path $d -Target $it.FullName | Out-Null; "junction $j -> $($it.FullName) (thu muc that o client64)" }
}
# ban sao thu muc cau hinh/script (chi khi chua co)
foreach ($c in @("settings", "script", "Ui", "UserData", "ini", "GameGuard", "sound", "music")) {
  $s = Join-Path $src $c; $d = Join-Path $dst $c
  if ((Test-Path $s) -and -not (Test-Path $d)) { $it = Get-Item $s; if ($it.LinkType -eq "Junction") { New-Item -ItemType Junction -Path $d -Target ($it.Target | Select-Object -First 1) | Out-Null; "junction $c" } else { Copy-Item $s $d -Recurse; "chep $c" } }
}
# tep roi: config.ini + DLL dung chung (Represent3, Rainbow, Lua54Dll, FilterText, D3DX9_43) - luon lam moi DLL, config chi khi chua co
foreach ($f in Get-ChildItem $src -File | Where-Object { $_.Extension -in ".dll", ".ini", ".txt", ".dat" -and $_.Name -notin @("Engine.dll", "CoreClient.dll", "SDL3.dll", "Rainbow.dll", "Represent3.dll") }) {   # [2b-2] Rainbow.dll cung la ban SDL rieng
  $d = Join-Path $dst $f.Name
  if ($f.Extension -eq ".dll" -or -not (Test-Path $d)) { Copy-Item $f.FullName $d -Force }
}
# bat chay
$bat = @"
@echo off
rem [SDL 08/09 2b] Ban SDL rieng: Engine.dll/CoreClient.dll/GameSDL.exe o day deu build voi JX_PLATFORM_SDL (thoi gian/luong/tep qua SDL3).
rem Du lieu: junction toi cay live nhu bin\client64. Bao loi: jx_auto.log, jx_crash.log cung thu muc.
cd /d "%~dp0"
if not exist GameSDL.exe (echo Chua co GameSDL.exe - build ReleaseSDL^|x64 chua xong & pause & exit /b 1)
if not exist SDL3.dll (echo Thieu SDL3.dll & pause & exit /b 1)
start "" GameSDL.exe
"@
Set-Content -Path (Join-Path $dst "ChoiGameSDL64.bat") -Value $bat -Encoding ASCII
"xong: " + $dst
Get-ChildItem $dst | Select-Object Name, LinkType, Length | Format-Table -AutoSize | Out-String -Width 120
