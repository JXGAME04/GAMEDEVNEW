# =====================================================================
#  NGHIEM THU BAN VA KHOA Goddess.exe  --  21/08/2026
#  Chay NGAY SAU khi trien khai, va chay LAI sau 3 gio sang hom sau
#  (luc do Backup() moi thuc su chay va moi chung minh duoc ban va).
# =====================================================================

$MS = 'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver'
$dat = 0; $hong = 0
function Kiem($ten, $ok, $chitiet) {
    if ($ok) { Write-Host ("  [DAT ] {0}" -f $ten) -ForegroundColor Green; $script:dat++ }
    else     { Write-Host ("  [HONG] {0}" -f $ten) -ForegroundColor Red;   $script:hong++ }
    if ($chitiet) { Write-Host ("         {0}" -f $chitiet) -ForegroundColor DarkGray }
}

Write-Host "`n=== A. BINARY DANG CHAY DUNG BAN VA ===" -ForegroundColor Cyan
$g = Join-Path $MS 'Goddess.exe'
$md = (Get-FileHash $g -Algorithm MD5).Hash
Kiem 'Goddess.exe la ban va' ($md -eq 'DC98942939FB868B22B793D82C491269') "md5 = $md"
$b = [IO.File]::ReadAllBytes($g); $s = [Text.Encoding]::ASCII.GetString($b)
Kiem 'binary co chuoi canh bao backup cut' ($s -match 'CANH BAO: quet bang BI DUT giua chung')
Kiem 'binary co moc dung theo yeu cau'      ($s -match 'CANH BAO: dung theo yeu cau')

Write-Host "`n=== B. CUM MAY CHU SONG ===" -ForegroundColor Cyan
foreach ($t in 'Goddess','Bishop','S3Relay','GameServer','Sword3PaySys','mysqld') {
    $p = Get-Process $t -ErrorAction SilentlyContinue
    Kiem "$t dang chay" ($null -ne $p) $(if ($p) { "PID $($p.Id)" })
}
$cong = @{5011='Goddess';5622='Bishop';5632='Bishop gateway';5002='Sword3PaySys';3306='mysqld'}
foreach ($c in $cong.Keys | Sort-Object) {
    $n = Get-NetTCPConnection -State Listen -LocalPort $c -ErrorAction SilentlyContinue
    Kiem ("cong {0} ({1}) dang nghe" -f $c, $cong[$c]) ($null -ne $n)
}
$bi = Get-Process Bishop -ErrorAction SilentlyContinue
Kiem 'Bishop tieu de = [Enable]' ($bi -and $bi.MainWindowTitle -match '\[Enable\]') $(if ($bi) { $bi.MainWindowTitle })

Write-Host "`n=== C. KHONG CON DAU HIEU SU CO ===" -ForegroundColor Cyan
$khoiDong = (Get-Process Goddess -ErrorAction SilentlyContinue).StartTime
$log = Join-Path $MS 'mysql_roledb.log'
if (Test-Path $log) {
    $dong = Get-Content $log -Tail 200
    $moi = $dong | Where-Object { $_ -match '^\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\]' -and
                                  [datetime]$matches[1] -ge $khoiDong }
    Kiem 'khong co "LoadScanPage LOI" sau khi khoi dong' (-not ($moi -match 'LoadScanPage LOI')) `
         ("da soi {0} dong tu {1}" -f $moi.Count, $khoiDong)
    Kiem 'khong co "mat ket noi MySQL"' (-not ($moi -match 'mat ket noi MySQL'))
}
$wer = Get-WinEvent -FilterHashtable @{LogName='Application';Id=1000;StartTime=$khoiDong} -ErrorAction SilentlyContinue |
       Where-Object { $_.Message -match 'Goddess' }
Kiem 'khong co su kien sap Goddess moi' ($null -eq $wer)

Write-Host "`n=== D. SAU KHI Backup() DA CHAY (3h sang hom sau) ===" -ForegroundColor Cyan
$bl = Join-Path $MS 'Backup.log'
if (Test-Path $bl) {
    $t = Get-Item $bl
    Write-Host ("  Backup.log sua luc: {0}" -f $t.LastWriteTime) -ForegroundColor DarkGray
    if ($t.LastWriteTime -lt $khoiDong) {
        Write-Host '  (Backup() CHUA chay lai ke tu khi khoi dong -- muc D chua ket luan duoc)' -ForegroundColor Yellow
    } else {
        $nd = Get-Content $bl -Raw
        Kiem 'Backup.log KHONG co canh bao dut' (-not ($nd -match 'CANH BAO'))
        Kiem 'Backup.log bao "DB Dump Finished."' ($nd -match 'DB Dump Finished\.')
        $pl = Join-Path $MS 'playerlist.txt'
        if (Test-Path $pl) {
            $n = (Get-Content $pl | Measure-Object -Line).Lines
            # 1 dong tieu de + N nhan vat. Cut = boi so cua 64 cong 1 (192+1=193 hom 21/08).
            Kiem 'playerlist.txt KHONG phai boi so 64 (tuc khong bi cat trang)' `
                 ((($n - 1) % 64) -ne 0) "so dong = $n (ky vong ~1004)"
        }
    }
} else { Write-Host '  khong thay Backup.log' -ForegroundColor Yellow }

Write-Host "`n=== E. DU LIEU ===" -ForegroundColor Cyan
Write-Host '  Chay tay (can pymysql):' -ForegroundColor DarkGray
Write-Host '    SELECT COUNT(*) FROM jx1_role.role_save_fail;              -- ky vong 0'
Write-Host '    SELECT MAX(saved_at) FROM jx1_role.role_history;           -- phai gan hien tai'
Write-Host '    SELECT COUNT(*) FROM (SELECT world_stat FROM jx1_role.role'
Write-Host '      WHERE world_stat BETWEEN 1 AND 1000 GROUP BY world_stat'
Write-Host '      HAVING COUNT(*)>1) t;   -- 144 = di chung CHUA don, 0 = da sach'

Write-Host ("`n===== TONG: {0} DAT / {1} HONG =====" -f $dat, $hong) `
    -ForegroundColor $(if ($hong -eq 0) { 'Green' } else { 'Red' })
