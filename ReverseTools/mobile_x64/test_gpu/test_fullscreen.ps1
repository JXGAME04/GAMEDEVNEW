# [GPU 08/09 khung ao] Test toan man hinh: FullScreen=1 -> GameSDL (Rep3Api=100) -> cua so desktop 2560x1440, khung logic 1024x768
# letterbox scale 1.875, offset x 320 -> chup ca man hinh (thu nho 1/2), bam nut "Tuy chon he thong" tai (320+512*1.875, 275*1.875) = (1280,516), chup lai.
param([string]$Out = $PSScriptRoot)
Add-Type -AssemblyName System.Drawing
Add-Type @'
using System; using System.Runtime.InteropServices;
public class W4 {
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint x, uint y, uint d, UIntPtr e);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern int GetSystemMetrics(int i); }
'@
function ChupManHinh($f) {
  $w = [W4]::GetSystemMetrics(0); $h = [W4]::GetSystemMetrics(1)
  $bmp = New-Object System.Drawing.Bitmap $w, $h; $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen(0, 0, 0, 0, $bmp.Size); $g.Dispose()
  $small = New-Object System.Drawing.Bitmap $bmp, ([int]($w / 2)), ([int]($h / 2)); $small.Save($f, [System.Drawing.Imaging.ImageFormat]::Png); $small.Dispose(); $bmp.Dispose()
  return "$w x $h -> $f"
}
$cfg = 'D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\config.ini'
$txt = [IO.File]::ReadAllText($cfg); $txt2 = $txt -replace '(?m)^FullScreen=\d+', 'FullScreen=1'; [IO.File]::WriteAllText($cfg, $txt2)
try {
  $p = Start-Process -FilePath 'D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\GameSDL.exe' -WorkingDirectory 'D:\GAMEDEVNEW_wt_mobile\bin\client64sdl' -PassThru
  Start-Sleep -Seconds 15
  $q = Get-Process -Id $p.Id -ErrorAction SilentlyContinue
  if (-not $q) { "CHET truoc 15 s (exit " + $p.ExitCode + ")"; exit 1 }
  [W4]::SetForegroundWindow($q.MainWindowHandle) | Out-Null; Start-Sleep -Milliseconds 500
  "chup 1: " + (ChupManHinh "$Out\gpu_full_1.png")
  $sw = [W4]::GetSystemMetrics(0); $sh = [W4]::GetSystemMetrics(1)
  $sc = [Math]::Min($sw / 1024.0, $sh / 768.0); $ox = ($sw - 1024 * $sc) / 2; $oy = ($sh - 768 * $sc) / 2
  $cx = [int]($ox + 512 * $sc); $cy = [int]($oy + 275 * $sc)
  "bam tai ($cx,$cy) scale $sc"
  [W4]::SetCursorPos($cx, $cy) | Out-Null; Start-Sleep -Milliseconds 300
  [W4]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero); Start-Sleep -Milliseconds 80; [W4]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
  Start-Sleep -Milliseconds 1500
  "chup 2: " + (ChupManHinh "$Out\gpu_full_2.png")
  $q = Get-Process -Id $p.Id -ErrorAction SilentlyContinue
  if ($q) { "SONG, RAM " + [int]($q.WorkingSet64/1MB) + " MB"; Stop-Process -Id $p.Id -Force } else { "CHET (exit " + $p.ExitCode + ")" }
} finally {
  [IO.File]::WriteAllText($cfg, $txt)   # tra lai FullScreen cu
  "config da tra lai FullScreen cu"
}
