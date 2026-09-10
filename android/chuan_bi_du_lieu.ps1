# [ANDROID 08/09] Chuan bi thu muc du lieu game cho Android: chep tu cay client (Windows) sang mot thu muc tam, BO binary Windows,
# roi HA CHU THUONG toan bo ten tep/thu muc (he tep Android phan biet hoa/thuong; lop tuong thich ha chu thuong moi duong dan tuong doi).
# Dung:  powershell -File android\chuan_bi_du_lieu.ps1 -Nguon E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client -Dich D:\jx1_android_data
# Sau do chep len may:  adb push D:\jx1_android_data\. /storage/emulated/0/Android/data/vn.jx1.mobile/files/
# (hoac chep bang cap/USB vao dung thu muc do; mo app 1 lan truoc de Android tao thu muc).
param(
  [string]$Nguon = "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client",
  [string]$Dich  = "D:\jx1_android_data",
  [string]$ConfigTu = "D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\config.ini"   # config cua bo SDL (Rep3Api=100); doi IP may chu trong do
)
$ErrorActionPreference = "Stop"
if (-not (Test-Path $Nguon)) { throw "khong thay thu muc nguon $Nguon" }
New-Item -ItemType Directory -Force $Dich | Out-Null
$thuMuc = @("spr", "data", "maps", "maps2", "script", "settings", "ui", "userdata", "sound", "music")
foreach ($t in $thuMuc) {
  $src = Join-Path $Nguon $t
  if (Test-Path $src) {
    "chep $t ..."
    robocopy $src (Join-Path $Dich $t) /E /R:1 /W:1 /NFL /NDL /NJH /NJS /XF *.exe *.dll *.pdb *.lib *.exp *.bat *.moi *.log *.dmp | Out-Null
  } else { "  (khong co $t)" }
}
foreach ($f in @("package.ini")) { if (Test-Path (Join-Path $Nguon $f)) { Copy-Item (Join-Path $Nguon $f) $Dich -Force } }
if (Test-Path $ConfigTu) { Copy-Item $ConfigTu (Join-Path $Dich "config.ini") -Force; "config.ini lay tu $ConfigTu" }
elseif (Test-Path (Join-Path $Nguon "config.ini")) { Copy-Item (Join-Path $Nguon "config.ini") $Dich -Force }

# [ANDROID 10/09 GHIDE] Lop ghi de rieng cho Android (anh VNKU cho nut ky nang, khung thanh duoi, bo cuc mac dinh
# UiToaDo, config.ini co [Resolution]/[Login]): chep DE LEN sau cung. Them tep chi-Android thi bo vao day.
$GhiDe = Join-Path $PSScriptRoot "du_lieu_ghi_de"
if (Test-Path $GhiDe) {
  "chep lop ghi de Android tu $GhiDe ..."
  robocopy $GhiDe $Dich /E /R:1 /W:1 /NFL /NDL /NJH /NJS | Out-Null
} else { "  (khong co $GhiDe - bo qua lop ghi de)" }

"ha chu thuong ten tep/thu muc ..."
# doi ten tu sau ra truoc (tep truoc, thu muc sau) - qua ten tam vi NTFS khong phan biet hoa/thuong
# [ANDROID 11/09 TENGBK] CHI ha A-Z (ASCII). Ten GBK doc bang cp1252 la chu Latin-1 hoa ('Ö' 'Ï' 'É'...): ToLowerInvariant ha luon
# -> byte tren dia doi (0xD6 -> 0xF6) -> game (JxPathPosix doi cp1252 -> UTF-8 tu byte GBK goc) khong tim thay tep. Da mat 1.639/3.518
# ten GBK trong spr/settings/maps (anh vat pham, hieu ung chieu, ban do). Sua lai du lieu da sinh: android\sua_ten_gbk_android.py
function HaAscii([string]$s) { return [regex]::Replace($s, '[A-Z]', { param($m) $m.Value.ToLowerInvariant() }) }
Get-ChildItem -LiteralPath $Dich -Recurse -File | Where-Object { $_.Name -cne (HaAscii $_.Name) } | ForEach-Object {
  $tmp = $_.FullName + ".__tmp__"; Rename-Item -LiteralPath $_.FullName -NewName ($_.Name + ".__tmp__"); Rename-Item -LiteralPath $tmp -NewName (HaAscii $_.Name)
}
Get-ChildItem -LiteralPath $Dich -Recurse -Directory | Sort-Object { $_.FullName.Length } -Descending | Where-Object { $_.Name -cne (HaAscii $_.Name) } | ForEach-Object {
  $tmp = $_.FullName + "__tmp__"; Rename-Item -LiteralPath $_.FullName -NewName ($_.Name + "__tmp__"); Rename-Item -LiteralPath $tmp -NewName (HaAscii $_.Name)
}
$size = (Get-ChildItem -LiteralPath $Dich -Recurse -File | Measure-Object -Property Length -Sum).Sum
"xong: $Dich  ({0:N1} GB)" -f ($size / 1GB)
"chep len may:  adb push `"$Dich\.`" /storage/emulated/0/Android/data/vn.jx1.mobile/files/"
