# [X64 08/09 pha 0 - chung minh Win32] Build chuoi client Win32 TU NHANH mobile-0809 trong worktree nay, uu tien thap.
# KHAC voi build binh thuong: TAT post-build (-p:PostBuildEventUseInBuild=false) vi post-build Win32 chep vao ..\..\..\bin = D:\bin\client
# (noi cay chinh D:\GAMEDEVNEW cung chep ra) va de len Lib\release\*.lib (theo doi trong git). Thay vao do: tu chep import lib moi vao
# Lib\release\ de S3Client link duoc, roi 'git checkout --' tra lai sau khi xong. Khong chep ket qua di dau: Game.exe/CoreClient.dll nam o
# Sources\S3Client\Release\ va Sources\Core\ClientRelease\ (chi de so sanh / thu).
# Dung: powershell -File build_chuoi_win32.ps1 [-From engine|core|rep3|s3client] [-Target Build|Rebuild]
param([string]$From = "engine", [string]$Target = "Build")
$sp = "C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad"
$WT = "D:\GAMEDEVNEW_wt_mobile"
$W = "$WT\Sources"
$thap = Join-Path $PSScriptRoot "build_thap.ps1"
if (-not (Test-Path $thap)) { $thap = "$sp\build_thap.ps1" }
$steps = @(
  @{ key="engine";   proj="$W\Engine\Engine.vcxproj";                   cfg="Release";        tag="engine_win32";   lib=@("$W\Engine\Release\Engine.lib") },
  @{ key="core";     proj="$W\Core\Core.vcxproj";                       cfg="Client Release"; tag="core_win32";     lib=@("$W\Core\ClientRelease\CoreClient.lib") },
  @{ key="rep3";     proj="$W\Represent\Represent3\Represent3.vcxproj"; cfg="Release";        tag="rep3_win32";     lib=@() },
  @{ key="s3client"; proj="$W\S3Client\S3Client.vcxproj";               cfg="Release";        tag="s3client_win32"; lib=@() }
)
$go = $false
foreach ($s in $steps) {
  if ($s.key -eq $From) { $go = $true }
  if (-not $go) { continue }
  "===== [$(Get-Date -Format HH:mm:ss)] " + $s.key + " (" + $s.cfg + "|Win32, khong post-build) ====="
  $out = & powershell -NoProfile -ExecutionPolicy Bypass -File $thap -Proj $s.proj -Cfg $s.cfg -Plat Win32 -Target $Target -Tag $s.tag -MaxErr 80 -Props "PostBuildEventUseInBuild=false"
  $out
  $first = ($out | Select-Object -First 1)
  if ($first -notmatch "exit=0") { "DUNG chuoi tai " + $s.key; break }
  foreach ($l in $s.lib) {
    if (Test-Path $l) { Copy-Item $l "$WT\Lib\release\" -Force; "  chep tam " + (Split-Path $l -Leaf) + " -> Lib\release (se tra lai bang git)" }
  }
}
# tra lai cac .lib theo doi trong git (Engine.lib / CoreClient.lib Win32)
Set-Location $WT
& git checkout -- Lib/release/CoreClient.lib Lib/release/Engine.lib 2>$null
"  git checkout -- Lib/release/CoreClient.lib Lib/release/Engine.lib : " + (& git status --short -- Lib/release | Out-String).Trim()
"===== [$(Get-Date -Format HH:mm:ss)] het chuoi Win32 ====="
