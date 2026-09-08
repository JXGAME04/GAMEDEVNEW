# Kiem tra bo client64: moi .exe/.dll phai la x64 (machine 8664), liet ke DLL nhap va bao thieu.
$dir = "D:\GAMEDEVNEW_wt_mobile\bin\client64"
$dumpbin = (Get-ChildItem "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" | Select-Object -Last 1).FullName
$sys = @("kernel32","user32","gdi32","advapi32","shell32","ole32","oleaut32","ws2_32","winmm","imm32","comdlg32","version","dbghelp","d3d9","dinput8","ddraw","dsound","wininet","psapi","iphlpapi","crypt32","shlwapi","comctl32","rpcrt4","secur32","bcrypt","gdiplus","windowscodecs","msvcp140","vcruntime140","vcruntime140_1","ucrtbase","d3dx9_43","d3dcompiler_43","dxgi","d3d11","odbc32","odbccp32","wsock32","mpr","netapi32","wintrust","setupapi","uxtheme","dwmapi","msimg32","opengl32","dbgcore","powrprof","ntdll")
$files = Get-ChildItem $dir -Include *.exe,*.dll -File -Recurse -Depth 0
foreach ($f in $files) {
  $hdr = & $dumpbin -nologo -headers $f.FullName 2>$null | Select-String "machine \((\w+)\)" | Select-Object -First 1
  $mach = if ($hdr) { $hdr.Matches[0].Groups[1].Value } else { "?" }
  $deps = & $dumpbin -nologo -dependents $f.FullName 2>$null | Where-Object { $_ -match "^\s+\S+\.dll$" } | ForEach-Object { $_.Trim() }
  $missing = @()
  foreach ($d in $deps) {
    $base = [IO.Path]::GetFileNameWithoutExtension($d).ToLower()
    if ($base -like "api-ms-*" -or $sys -contains $base) { continue }
    if (-not (Test-Path (Join-Path $dir $d))) { $missing += $d }
  }
  "{0,-18} {1,-6} {2,9} B  thieu: {3}" -f $f.Name, $mach, $f.Length, ($(if ($missing.Count) { $missing -join "," } else { "-" }))
}
