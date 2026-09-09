# [SDL 08/09 2b] Build chuoi "ban SDL" (JX_PLATFORM_SDL) theo thu tu phu thuoc, uu tien thap: Engine ReleaseSDL -> Core Client ReleaseSDL -> S3Client ReleaseSDL.
# Post-build tu chep vao Lib\release64sdl va bin\client64sdl. Represent3/Rainbow/Lua54 dung lai ban x64 thuong (chep san o bin\client64sdl).
# Dung: powershell -File build_chuoi_sdl.ps1 [-From engine|core|s3client] [-Target Build|Rebuild]
param([string]$From = "common", [string]$Target = "Build")
$W = "D:\GAMEDEVNEW_wt_mobile\Sources"
$thap = Join-Path $PSScriptRoot "build_thap.ps1"
$steps = @(
  @{ key="common";   proj="$W\MultiServer\Common\Common.vcxproj";   cfg="ReleaseSDL";  tag="common_sdl" },
  @{ key="rainbow";  proj="$W\MultiServer\Rainbow\Rainbow.vcxproj"; cfg="ReleaseSDL";  tag="rainbow_sdl" },
  @{ key="engine";   proj="$W\Engine\Engine.vcxproj";     cfg="ReleaseSDL";        tag="engine_sdl" },
  @{ key="rep3";     proj="$W\Represent\Represent3\Represent3.vcxproj"; cfg="ReleaseSDL"; tag="rep3_sdl" },
  @{ key="core";     proj="$W\Core\Core.vcxproj";         cfg="Client ReleaseSDL"; tag="core_sdl" },
  @{ key="s3client"; proj="$W\S3Client\S3Client.vcxproj"; cfg="ReleaseSDL";        tag="s3client_sdl" }
)
$go = $false
foreach ($s in $steps) {
  if ($s.key -eq $From) { $go = $true }
  if (-not $go) { continue }
  "===== [$(Get-Date -Format HH:mm:ss)] " + $s.key + " (" + $s.cfg + "|x64) ====="
  $out = & powershell -NoProfile -ExecutionPolicy Bypass -File $thap -Proj $s.proj -Cfg $s.cfg -Plat x64 -Target $Target -Tag $s.tag -MaxErr 80
  $out
  $first = ($out | Select-Object -First 1)
  if ($first -notmatch "exit=0") { "DUNG chuoi tai " + $s.key; break }
}
"===== [$(Get-Date -Format HH:mm:ss)] het chuoi SDL ====="
foreach ($f in @("Rainbow.dll", "Represent3.dll", "Engine.dll", "CoreClient.dll", "GameSDL.exe", "SDL3.dll")) {
  $p = "D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\$f"
  if (Test-Path $p) { "  co $f (" + (Get-Item $p).LastWriteTime.ToString('HH:mm:ss') + ", " + (Get-Item $p).Length + " B)" } else { "  THIEU $f" }
}
