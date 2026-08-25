# =====================================================================
#  KHOI DONG CUM MAY CHU DUNG THU TU
#
#  🔴 Bishop chi goi ConnectTo(RoleSvr 5011) DUNG MOT LAN va KHONG thu lai
#     (Intercessor.cpp:389). Bat Bishop truoc khi Goddess kip mo cong 5011
#     la mat gateway 5632 -> GameServer bao "Connect to gateway is failed!".
#     Vi vay script nay CHO cong 5011 nghe roi moi bat Bishop.
# =====================================================================

$ErrorActionPreference = 'Stop'
$MS = 'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver'
$SV = 'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'

function Buoc($s) { Write-Host "`n==> $s" -ForegroundColor Cyan }
function DangChay($t) { $null -ne (Get-Process $t -ErrorAction SilentlyContinue) }
function Bat($ten, $thumuc) {
    if (DangChay $ten) { Write-Host "    $ten da chay, bo qua"; return }
    $exe = Join-Path $thumuc "$ten.exe"
    if (-not (Test-Path $exe)) { throw "Khong thay $exe" }
    Start-Process -FilePath $exe -WorkingDirectory $thumuc
    Write-Host "    da bat $ten"
}

Buoc '0. Kiem tra'
if (-not (DangChay 'mysqld')) { throw 'mysqld KHONG chay -- phai bat MySQL truoc tien.' }
$md = (Get-FileHash (Join-Path $MS 'Goddess.exe') -Algorithm MD5).Hash
Write-Host "    Goddess.exe md5 = $md"
if ($md -eq 'DC98942939FB868B22B793D82C491269') {
    Write-Host '    -> dung BAN VA khoa 21/08' -ForegroundColor Green
} elseif ($md -eq 'B05B978D92C48A9D0B0FCE2A4BF0BCD2') {
    Write-Host '    -> dang la BAN CU (chua va). Se sap lai luc 3h sang.' -ForegroundColor Yellow
} else {
    Write-Host '    -> ban la, khong nhan ra.' -ForegroundColor Yellow
}

Buoc '1. Sword3PaySys'
Bat 'Sword3PaySys' $MS
Start-Sleep -Seconds 2

Buoc '2. Goddess'
Bat 'Goddess' $MS
Write-Host ''
Write-Host '    ###############################################################' -ForegroundColor Yellow
Write-Host '    #  BAY GIO BAM NUT "Start" TREN CUA SO Goddess                #' -ForegroundColor Yellow
Write-Host '    #                                                             #' -ForegroundColor Yellow
Write-Host '    #  Goddess KHONG tu mo cong 5011. Chi khi bam Start thi        #' -ForegroundColor Yellow
Write-Host '    #  CreateDatabaseEngine() moi chay (Goddess.cpp:629):          #' -ForegroundColor Yellow
Write-Host '    #    InitDBInterface -> StartBackupTimer -> OpenService(5011)  #' -ForegroundColor Yellow
Write-Host '    #  va tieu de doi thanh "Goddess - [Enable]".                  #' -ForegroundColor Yellow
Write-Host '    #  Nut "Manual Backup" cung chi sang sau khi Start.            #' -ForegroundColor Yellow
Write-Host '    ###############################################################' -ForegroundColor Yellow

Buoc '3. CHO cong 5011 nghe (bat buoc truoc khi bat Bishop)'
Write-Host '    dang cho... (toi da 5 phut)'
$ok = $false
for ($i = 0; $i -lt 1500; $i++) {
    if (Get-NetTCPConnection -State Listen -LocalPort 5011 -ErrorAction SilentlyContinue) { $ok = $true; break }
    Start-Sleep -Milliseconds 200
}
if (-not $ok) {
    throw 'Cong 5011 khong mo sau 5 phut -- co bam nut Start chua? KHONG duoc bat Bishop khi chua co 5011.'
}
Write-Host '    5011 da nghe.' -ForegroundColor Green

Buoc '4. Bishop'
Bat 'Bishop' $MS
Start-Sleep -Seconds 3
$bi = Get-Process Bishop -ErrorAction SilentlyContinue
if ($bi) {
    Write-Host "    tieu de cua so: '$($bi.MainWindowTitle)'"
    if ($bi.MainWindowTitle -notmatch '\[Enable\]') {
        Write-Host '    CANH BAO: chua thay [Enable]. Neu van la "Bishop" tron thi da hong:' -ForegroundColor Yellow
        Write-Host '    TAT HAN roi chay lai script -- TUYET DOI KHONG bam nut OK de thu lai.' -ForegroundColor Yellow
    }
}

Buoc '5. S3Relay'
Bat 'S3Relay' $MS
Start-Sleep -Seconds 2

Buoc '6. GameServer'
Bat 'GameServer' $SV

Buoc 'XONG -- kiem nhanh'
Start-Sleep -Seconds 3
Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue |
    Where-Object { $_.LocalPort -in 3306,5002,5011,5622,5632 } |
    Select-Object LocalPort, OwningProcess | Sort-Object LocalPort | Format-Table -AutoSize

Write-Host 'Chay nghiem_thu_goddess_khoa_2108.ps1 de kiem day du.' -ForegroundColor Green
