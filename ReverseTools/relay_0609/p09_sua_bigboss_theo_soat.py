# -*- coding: utf-8 -*-
"""p09: sua bigboss.utf8.lua theo ket qua soat xet doi khang."""
import io
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
B = chr(92)  # backslash
p = r"D:\GAMEDEVNEW_wt_relay\serverscript_relay\script\mission\boss\bigboss.utf8.lua"
d = io.open(p, encoding="utf-8").read()
n = 0

# 1) khoi chu thich [5] day mot LUAT SAI -> viet lai cho dung
old = ('--   [5] format("%d", ...) giu nguyen o HAI ham dem boss vang (gold_boss_count\n'
       '--       luon la so nguyen nen an toan). Nhung o SetBigBossDead / IsBigBossDead\n'
       '--       thi log bang tostring(), KHONG dung %d: gia tri do di ra tu OB_PopDouble\n'
       '--       nen la so THUC, ma o 5.4 format("%d", so thuc le) la LOI se lam chet ca\n'
       '--       ham -> mat luon duong tra ve. Nho luat nay khi sua ve sau.')
new = ('--   [5] Log dung tostring() cho gon. DINH CHINH (soat 06/09): ban dau cho nay ghi\n'
       '--       mot luat SAI la "format(\'%d\', so thuc) la LOI o 5.4". DA DO THAT tren\n'
       '--       dung ngan xep dang chay (Lua54Dll + lua4_baselibopen): format(\'%d\',1.5)\n'
       '--       = "1", format(\'%d\',-2.7) = "-2" -- KHONG loi, vi bien toan cuc `format`\n'
       '--       la ham C l4_b_format cua shim, no tu ep nguyen. Chi string.format cua\n'
       '--       Lua 5.4 goc moi bao loi. Dung format("%d", ...) o day cung duoc.')
assert d.count(old) == 1, "khoi [5]"
d = d.replace(old, new, 1)
n += 1

# 2) chu thich sai ve "sai mot chu s"
old = ('--   script' + B + 'missions' + B + 'boss (MISSIONS, so nhieu). Sai mot chu "s" thi\n'
       '--   RemoteExecute quay lai chay TAI CHO trong GameServer, khong bao gio len relay.')
new = ('--   script' + B + 'missions' + B + 'boss (MISSIONS, so nhieu).\n'
       '--   [SOAT 06/09] Dat sai cho BEN RELAY thi GameServer VAN gui len relay nhu thuong\n'
       '--   (no ghi cung mot duong dan, khong nhin thu muc ben nay); dau hieu hong nam o\n'
       '--   NHAT KY RELAY: "khong nap duoc kich ban [...]", khong phai nhat ky GameServer.')
if d.count(old) == 1:
    d = d.replace(old, new, 1)
    n += 1
else:
    print("  ! bo qua muc 2 (khong khop neo)")

# 3) so dong callback lech 1
old = "--        GameServer doc o BigBossGlobalReward_CallBack (dong 179-199):"
if d.count(old) == 1:
    d = d.replace(old, "--        GameServer doc o BigBossGlobalReward_CallBack (dong 179-200):", 1)
    n += 1

# 4) bo hai dong lap lai luat sai trong than ham
old = ('\t\t\t-- dung tostring, KHONG dung %d: nDead di ra tu OB_PopDouble nen la so\n'
       '\t\t\t-- THUC; o 5.4 format("%d", so thuc le) la LOI se lam chet ca ham nay.\n')
if d.count(old) == 1:
    d = d.replace(old, "", 1)
    n += 1
else:
    print("  ! bo qua muc 4 (khong khop neo)")

# 5) IsBigBossDead chay THEO TUNG NGUOI CHOI -> bo log duong nong
old = ('\tObjBuffer:PushObject(ResultHandle, nDead);\n'
       '\tOutputMsg( "[BIGBOSS] IsBigBossDead -> " .. tostring(nDead) );')
new = ('\tObjBuffer:PushObject(ResultHandle, nDead);\n'
       '\t-- [SOAT 06/09] KHONG ghi log o day: GameServer goi ham nay THEO TUNG NGUOI CHOI\n'
       '\t-- (bigboss.lua:176 nam trong vong lap nguoi choi), ghi moi luot la do rTRACE\n'
       '\t-- tren luong chinh cua relay. Muon do duong ong thi xem log cua SetBigBossDead.')
if d.count(old) == 1:
    d = d.replace(old, new, 1)
    n += 1
else:
    print("  ! bo qua muc 5 (khong khop neo)")

io.open(p, "w", encoding="utf-8", newline="").write(d)
print("bigboss: da sua %d cho" % n)
