# Build chuoi client x64 theo thu tu phu thuoc, dung ngay khi mot du an loi. Uu tien thap (ke thua tu build_thap.ps1).
# Dung: powershell -File build_chuoi_x64.ps1 [-From engine|core|rep3|s3client] [-MPCount 3]
param([string]$From = "engine", [int]$MPCount = 3, [string]$Target = "Build")
$sp = "C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad"
$W = "D:\GAMEDEVNEW_wt_mobile\Sources"
$steps = @(
  @{ key="engine";   proj="$W\Engine\Engine.vcxproj";                       cfg="Release";        tag="engine_x64" },
  @{ key="core";     proj="$W\Core\Core.vcxproj";                           cfg="Client Release"; tag="core_x64" },
  @{ key="rep3";     proj="$W\Represent\Represent3\Represent3.vcxproj";     cfg="Release";        tag="rep3_x64" },
  @{ key="s3client"; proj="$W\S3Client\S3Client.vcxproj";                   cfg="Release";        tag="s3client_x64" }
)
$go = $false
foreach ($s in $steps) {
  if ($s.key -eq $From) { $go = $true }
  if (-not $go) { continue }
  "===== [$(Get-Date -Format HH:mm:ss)] " + $s.key + " (" + $s.cfg + "|x64) ====="
  $out = & powershell -NoProfile -ExecutionPolicy Bypass -File "$sp\build_thap.ps1" -Proj $s.proj -Cfg $s.cfg -Plat x64 -Target $Target -Tag $s.tag -MaxErr 80
  $out
  $first = ($out | Select-Object -First 1)
  if ($first -notmatch "exit=0") { "DUNG chuoi tai " + $s.key; break }
}
"===== [$(Get-Date -Format HH:mm:ss)] het chuoi ====="
