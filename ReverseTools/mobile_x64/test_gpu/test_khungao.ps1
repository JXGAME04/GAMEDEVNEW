# [GPU 08/09 khung ao] Chay GameSDL (Rep3Api=100), phong cua so len 1280x960 (client), chup; bam chuot vao nut "Tuy chon he thong"
# theo toa do da co gian (logic 512,275 -> cua so 640,344); chup lai. Hop tuy chon hien = letterbox + anh xa chuot dung.
param([string]$Out = $PSScriptRoot)
Add-Type -AssemblyName System.Drawing
Add-Type @'
using System; using System.Runtime.InteropServices;
public class W3 {
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out R r);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref P p);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint f);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint x, uint y, uint d, UIntPtr e);
  [StructLayout(LayoutKind.Sequential)] public struct R { public int L, T, Rt, B; }
  [StructLayout(LayoutKind.Sequential)] public struct P { public int X, Y; } }
'@
function Chup($h, $f) {
  $r = New-Object W3+R; [W3]::GetClientRect($h, [ref]$r) | Out-Null; $p = New-Object W3+P; [W3]::ClientToScreen($h, [ref]$p) | Out-Null
  $w = $r.Rt - $r.L; $hh = $r.B - $r.T; if ($w -le 0) { return "0x0" }
  $bmp = New-Object System.Drawing.Bitmap $w, $hh; $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($p.X, $p.Y, 0, 0, $bmp.Size)
  $bmp.Save($f, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose(); return "$w x $hh"
}
$p = Start-Process -FilePath 'D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\GameSDL.exe' -WorkingDirectory 'D:\GAMEDEVNEW_wt_mobile\bin\client64sdl' -PassThru
Start-Sleep -Seconds 14
$q = Get-Process -Id $p.Id -ErrorAction SilentlyContinue
if (-not $q) { "CHET truoc 14 s"; exit 1 }
$h = $q.MainWindowHandle
[W3]::SetForegroundWindow($h) | Out-Null; Start-Sleep -Milliseconds 400
$rc = New-Object W3+R; [W3]::GetClientRect($h, [ref]$rc) | Out-Null; $rw = New-Object W3+R; [W3]::GetWindowRect($h, [ref]$rw) | Out-Null
$dx = ($rw.Rt - $rw.L) - ($rc.Rt - $rc.L); $dy = ($rw.B - $rw.T) - ($rc.B - $rc.T)
"client truoc: " + ($rc.Rt - $rc.L) + "x" + ($rc.B - $rc.T)
[W3]::SetWindowPos($h, [IntPtr]::Zero, 40, 40, 1280 + $dx, 960 + $dy, 0x0004) | Out-Null   # SWP_NOZORDER
Start-Sleep -Seconds 2
"client sau: " + (Chup $h "$Out\gpu_scale.png")
# bam nut "Tuy chon he thong": logic (512,275) -> cua so 1280x960: scale 1.25 -> (640,344)
$pt = New-Object W3+P; $pt.X = 640; $pt.Y = 344; [W3]::ClientToScreen($h, [ref]$pt) | Out-Null
[W3]::SetCursorPos($pt.X, $pt.Y) | Out-Null; Start-Sleep -Milliseconds 300
[W3]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero); Start-Sleep -Milliseconds 80; [W3]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
Start-Sleep -Milliseconds 1500
"sau bam: " + (Chup $h "$Out\gpu_click.png")
$q = Get-Process -Id $p.Id -ErrorAction SilentlyContinue
if ($q) { "SONG, RAM " + [int]($q.WorkingSet64/1MB) + " MB"; Stop-Process -Id $p.Id -Force } else { "CHET sau khi phong/bam (exit " + $p.ExitCode + ")" }
