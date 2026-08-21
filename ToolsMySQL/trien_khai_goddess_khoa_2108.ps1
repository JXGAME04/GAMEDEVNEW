# =====================================================================
#  TRIEN KHAI BAN VA KHOA CHO Goddess.exe  --  21/08/2026
#
#  Va su co 03:25:45 ngay 21/08: hai luong dung chung MOT MYSQL* khong
#  khoa -> heap corruption 0xC0000374 dung vao gio backup (3h sang).
#
#  QUAN TRONG: Bishop chi ConnectTo cong 5011 DUNG MOT LAN va khong thu
#  lai (Intercessor.cpp:389). Vi vay restart Goddess BAT BUOC phai
#  restart ca Bishop / S3Relay / GameServer theo dung thu tu duoi day,
#  neu khong se mat gateway 5632 va GameServer bao
#  "Connect to gateway is failed!".
#
#  Chay bang PowerShell, quyen Administrator khong bat buoc.
# =====================================================================

$ErrorActionPreference = 'Stop'
$MS  = 'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver'
$SV  = 'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
$MOI = Join-Path $MS 'Goddess.exe.moi_khoa_2108'
$CU  = Join-Path $MS 'Goddess.exe'
$BAK = Join-Path $MS 'Goddess.exe.bak_truoc_khoa_2108'

function Buoc($s) { Write-Host "`n==> $s" -ForegroundColor Cyan }

# --- 0. Kiem tra dieu kien ---------------------------------------------
Buoc '0. Kiem tra ban va co san'
if (-not (Test-Path $MOI)) { throw "KHONG THAY $MOI -- chua dat ban va vao day." }
$mdMoi = (Get-FileHash $MOI -Algorithm MD5).Hash
Write-Host "    ban moi : $mdMoi  ($((Get-Item $MOI).Length) byte)"
if ($mdMoi -ne 'DC98942939FB868B22B793D82C491269') {
    Write-Host "    CANH BAO: md5 khac voi ban da nghiem thu (DC989429...)." -ForegroundColor Yellow
    Write-Host "    Dung lai neu ban khong chu dong build lai." -ForegroundColor Yellow
}
if (Test-Path $CU) {
    Write-Host "    ban cu  : $((Get-FileHash $CU -Algorithm MD5).Hash)  ($((Get-Item $CU).Length) byte)"
}

# --- 1. Dung theo thu tu NGUOC ------------------------------------------
Buoc '1. Dung cum may chu (thu tu nguoc)'
foreach ($ten in 'GameServer','S3Relay','Bishop','Goddess') {
    $p = Get-Process $ten -ErrorAction SilentlyContinue
    if ($p) {
        Write-Host "    dung $ten (PID $($p.Id)) ..."
        $p | Stop-Process -Force
    } else {
        Write-Host "    $ten khong chay"
    }
}
$het = $false
for ($i = 0; $i -lt 60; $i++) {
    if (-not (Get-Process Goddess -ErrorAction SilentlyContinue)) { $het = $true; break }
    Start-Sleep -Milliseconds 500
}
if (-not $het) { throw 'Goddess khong chiu thoat -- dung lai, kiem tra tay.' }

# --- 2. Thay binary bang RENAME (khong ghi de) --------------------------
Buoc '2. Thay binary'
if (Test-Path $BAK) { Remove-Item $BAK -Force }
Rename-Item $CU  (Split-Path $BAK -Leaf)
Rename-Item $MOI (Split-Path $CU  -Leaf)
Write-Host "    ban cu -> $(Split-Path $BAK -Leaf)"
Write-Host "    ban moi -> Goddess.exe  ($((Get-FileHash $CU -Algorithm MD5).Hash))"

# --- 3. Khoi dong lai dung thu tu ---------------------------------------
Buoc '3. Khoi dong Goddess'
Start-Process -FilePath $CU -WorkingDirectory $MS

Buoc '   Cho cong 5011 nghe (BAT BUOC truoc khi bat Bishop)'
$ok = $false
for ($i = 0; $i -lt 300; $i++) {
    if (Get-NetTCPConnection -State Listen -LocalPort 5011 -ErrorAction SilentlyContinue) { $ok = $true; break }
    Start-Sleep -Milliseconds 200
}
if (-not $ok) { throw 'Cong 5011 khong mo sau 60 giay -- KHONG duoc bat Bishop. Xem mysql_roledb.log.' }
Write-Host '    5011 da nghe.' -ForegroundColor Green

Buoc '4. Khoi dong Bishop'
Start-Process -FilePath (Join-Path $MS 'Bishop.exe') -WorkingDirectory $MS
Start-Sleep -Seconds 3

Buoc '5. Khoi dong S3Relay'
Start-Process -FilePath (Join-Path $MS 'S3Relay.exe') -WorkingDirectory $MS
Start-Sleep -Seconds 2

Buoc '6. Khoi dong GameServer'
Start-Process -FilePath (Join-Path $SV 'GameServer.exe') -WorkingDirectory $SV

Write-Host "`nXONG. Chay nghiem_thu_goddess_khoa_2108.ps1 de kiem." -ForegroundColor Green
Write-Host "Neu hong: doi ten $BAK tro lai thanh Goddess.exe roi lam lai buoc 3-6."
