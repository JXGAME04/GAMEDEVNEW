# Chay MSBuild o uu tien THAP: ha uu tien chinh tien trinh PowerShell nay xuong BelowNormal truoc,
# moi tien trinh con (msbuild, cl, link) ke thua. -m:1, ghi log, in tom tat loi.
# Dung: powershell -File build_thap.ps1 -Proj <path.vcxproj> -Cfg "<Configuration>" [-Plat x64] [-Target Build|Rebuild] [-Tag ten]
param(
  [Parameter(Mandatory=$true)][string]$Proj,
  [Parameter(Mandatory=$true)][string]$Cfg,
  [string]$Plat = "x64",
  [string]$Target = "Build",
  [string]$Tag = "build",
  [string]$SolutionDir = "D:\GAMEDEVNEW_wt_mobile\Sources\",
  [int]$MaxErr = 60,
  [string[]]$Props = @()      # [08/09] them -p:... (vd -p:PostBuildEventUseInBuild=false cho build Win32 thu nghiem, khong chep vao D:in / Libelease)
)
try { (Get-Process -Id $PID).PriorityClass = 'BelowNormal' } catch { Write-Host "khong ha duoc uu tien: $_" }
$msb = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"
if (-not (Test-Path $msb)) { $msb = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" }
$logDir = "C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad\logs"
New-Item -ItemType Directory -Force -Path $logDir | Out-Null
$stamp = Get-Date -Format "HHmmss"
$log = Join-Path $logDir ("{0}_{1}.log" -f $Tag, $stamp)
$t0 = Get-Date
& $msb "$Proj" "-t:$Target" "-m:1" "-nologo" "-v:m" "-p:Configuration=$Cfg" "-p:Platform=$Plat" "-p:SolutionDir=$SolutionDir" "-p:UseMultiToolTask=false" "-p:CL_MPCount=1" @Props "-clp:NoSummary;ErrorsOnly;NoItemAndPropertyList" "-fl" "-flp:logfile=$log;verbosity=normal" | Out-Null
$code = $LASTEXITCODE
$dt = [int]((Get-Date) - $t0).TotalSeconds
$txt = Get-Content $log -ErrorAction SilentlyContinue
$errs = @($txt | Select-String -Pattern "(error [A-Z]+\d+|fatal error)" | ForEach-Object { $_.Line.Trim() } | Sort-Object -Unique)
$warns = @($txt | Select-String -Pattern "warning [A-Z]+\d+" | ForEach-Object { $_.Line.Trim() } | Sort-Object -Unique)
"exit=$code time=${dt}s errors=$($errs.Count) warnings=$($warns.Count) log=$log"
$errs | Select-Object -First $MaxErr
