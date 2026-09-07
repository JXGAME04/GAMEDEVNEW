# -*- coding: ascii -*-
"""goi_va_delta8_fightmode_chinhminh_0709.py - [DELTA 07/09 h] hoi quy cua va d/g: co chien dau cua CHINH MINH.
Va d bo bit 0x02 khoi bam goi 75 va dua co theo STATE_FIGHTMODE cua goi 77/221, nhung client SyncNpcMin loai tru
chinh minh -> co cua minh chi con ve theo ky lam moi 75 (va g: 300 s). Trieu chung: phu ve thanh van 'danh skill duoc'.
Sua: ap co cho ca chinh minh (client khong bao gio tu ghi co nay cho minh - moi cho ghi deu trong #ifdef _SERVER)."""
import io, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocolProcess.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
hb = sum(1 for ch in s if ord(ch) >= 0x80)
old = ("\t\t// [DELTA 07/09 d] co chien dau cua nguoi choi khac di theo goi 77/221 (truoc chi qua goi 75 -> 75 phat lai lien tuc)\r\n"
       "\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nIdx].m_Kind == kind_player)\r\n"
       "\t\t\tNpc[nIdx].m_FightMode = (NpcSync->State & STATE_FIGHTMODE) ? enumPKWar : enumPKNormal;\r\n")
new = ("\t\t// [DELTA 07/09 d] co chien dau cua nguoi choi di theo goi 77/221 (truoc chi qua goi 75 -> 75 phat lai lien tuc).\r\n"
       "\t\t// [DELTA 07/09 h] AP CA CHO CHINH MINH: goi 75 khong phat lai khi chi co nay doi (bit 0x02 da bo khoi bam) nen day la\r\n"
       "\t\t// duong DUY NHAT mang co cua minh ve; loai tru chinh minh (ban d) lam client giu co cu toi ky lam moi (300 s):\r\n"
       "\t\t// phu ve thanh van 'danh skill duoc' (client cho mua chieu, may chu tu choi). Client khong bao gio tu ghi co nay\r\n"
       "\t\t// cho chinh minh (moi cho ghi deu nam trong #ifdef _SERVER) nen khong dung nhau.\r\n"
       "\t\tif (Npc[nIdx].m_Kind == kind_player)\r\n"
       "\t\t\tNpc[nIdx].m_FightMode = (NpcSync->State & STATE_FIGHTMODE) ? enumPKWar : enumPKNormal;\r\n")
n = s.count(old)
if n != 1: print("FAIL count", n); sys.exit(1)
s2 = s.replace(old, new)
if sum(1 for ch in s2 if ord(ch) >= 0x80) != hb: print("FAIL high-byte"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s2)
print("OK patched, high-bytes", hb)
