# -*- coding: ascii -*-
# FIX-2 r2: khong bao gio giao dich phia SAU (chu game: "quay dau lui roi di tiep")
import io, sys
CRLF="\r\n"
def L(*a): return CRLF.join(a)
p=r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
c=io.open(p,'r',encoding='latin-1',newline='').read(); hb=sum(1 for ch in c if ord(ch)>127)

old=L(
"\t\t// Chi kich hoat khi da lech >= 1 o (NPC chay dung thi lech p50 chi 12 mps) va < 12 o",
"\t\t// (xa hon la dich chuyen that - de nguyen cho nhanh nan ben duoi lo).",
"\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&",
"\t\t\t(NpcSync->Doing == do_run || NpcSync->Doing == do_walk) &&",
"\t\t\tNpc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion)",
"\t\t{",
"\t\t\tint nS9X = 0, nS9Y = 0;",
"\t\t\tNpc[nIdx].GetMpsPos(&nS9X, &nS9Y);",
"\t\t\tint nS9D = g_GetDistance(nS9X, nS9Y, NpcSync->MapX, NpcSync->MapY);",
"\t\t\tif (nS9D >= 32 && nS9D < 32 * 12)",
"\t\t\t{",
"\t\t\t\tAUTOLOG_EVERY(1000, \"[S9-DICH] npc=%u idx=%d doing=%d lech=%d -> giao dich (%d,%d) t=%u\", NpcSync->ID, nIdx, (int)NpcSync->Doing, nS9D, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY);",
"\t\t\t}",
"\t\t}")

new=L(
"\t\t// Chi kich hoat khi da lech >= 1 o (NPC chay dung thi lech p50 chi 12 mps) va < 12 o",
"\t\t// (xa hon la dich chuyen that - de nguyen cho nhanh nan ben duoi lo).",
"\t\t//",
"\t\t// [r2 26/08 - chu game: \"nguoi xung quanh dang di chuyen thi QUAY DAU LUI roi di tiep\"]",
"\t\t// Ban r1 giao dich = vi tri may chu TAI THOI DIEM GOI TIN, ma vi tri do da cu (tre",
"\t\t// mang + nhip tick). Neu ban sao client da chay VUOT QUA diem do thi lenh moi bat no",
"\t\t// quay dau chay nguoc lai. Do that tren log: 2.413/22.857 lan = 10,6% dich giao ra",
"\t\t// nam PHIA SAU huong dang chay.",
"\t\t// Sua: suy ra HUONG DI CUA MAY CHU tu hai goi lien tiep (V), giao dich o PHIA TRUOC",
"\t\t// (P + V, tuc mot nhip nua), va neu dich do VAN nam phia sau thi BO QUA han - tha",
"\t\t// cho no chay tiep, goi sau se dung. Khong bao gio ep NPC quay dau.",
"\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&",
"\t\t\t(NpcSync->Doing == do_run || NpcSync->Doing == do_walk) &&",
"\t\t\tNpc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion)",
"\t\t{",
"\t\t\t// vi tri may chu cua LAN SYNC TRUOC (theo khe NPC; kem dwID de biet khe doi chu)",
"\t\t\tstatic DWORD s_uS9CuID[MAX_NPC] = {0};",
"\t\t\tstatic int   s_nS9CuX[MAX_NPC]  = {0};",
"\t\t\tstatic int   s_nS9CuY[MAX_NPC]  = {0};",
"\t\t\tint nS9Vx = 0, nS9Vy = 0;",
"\t\t\tif (nIdx > 0 && nIdx < MAX_NPC)",
"\t\t\t{",
"\t\t\t\tif (s_uS9CuID[nIdx] == NpcSync->ID)",
"\t\t\t\t{",
"\t\t\t\t\tnS9Vx = NpcSync->MapX - s_nS9CuX[nIdx];",
"\t\t\t\t\tnS9Vy = NpcSync->MapY - s_nS9CuY[nIdx];",
"\t\t\t\t\t// chan buoc nhay bat thuong (dich chuyen) - khong ngoai suy theo no",
"\t\t\t\t\tif (nS9Vx > 200 || nS9Vx < -200 || nS9Vy > 200 || nS9Vy < -200)",
"\t\t\t\t\t{",
"\t\t\t\t\t\tnS9Vx = 0; nS9Vy = 0;",
"\t\t\t\t\t}",
"\t\t\t\t}",
"\t\t\t\ts_uS9CuID[nIdx] = NpcSync->ID;",
"\t\t\t\ts_nS9CuX[nIdx]  = NpcSync->MapX;",
"\t\t\t\ts_nS9CuY[nIdx]  = NpcSync->MapY;",
"\t\t\t}",
"\t\t\tint nS9X = 0, nS9Y = 0;",
"\t\t\tNpc[nIdx].GetMpsPos(&nS9X, &nS9Y);",
"\t\t\tint nS9D = g_GetDistance(nS9X, nS9Y, NpcSync->MapX, NpcSync->MapY);",
"\t\t\tif (nS9D >= 32 && nS9D < 32 * 12)",
"\t\t\t{",
"\t\t\t\tint nS9DesX = NpcSync->MapX + nS9Vx;\t// ngoai suy mot nhip ve PHIA TRUOC",
"\t\t\t\tint nS9DesY = NpcSync->MapY + nS9Vy;",
"\t\t\t\t// con nam phia sau huong may chu dang di? (tich vo huong am) => bo qua",
"\t\t\t\tint nS9Dot = (nS9DesX - nS9X) * nS9Vx + (nS9DesY - nS9Y) * nS9Vy;",
"\t\t\t\tif ((nS9Vx || nS9Vy) && nS9Dot < 0)",
"\t\t\t\t{",
"\t\t\t\t\tAUTOLOG_EVERY(2000, \"[S9-LUI] npc=%u idx=%d lech=%d dich sau lung -> BO QUA (khong ep quay dau) t=%u\", NpcSync->ID, nIdx, nS9D, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\t}",
"\t\t\t\telse",
"\t\t\t\t{",
"\t\t\t\t\tAUTOLOG_EVERY(1000, \"[S9-DICH] npc=%u idx=%d doing=%d lech=%d v=(%d,%d) -> giao dich (%d,%d) t=%u\", NpcSync->ID, nIdx, (int)NpcSync->Doing, nS9D, nS9Vx, nS9Vy, nS9DesX, nS9DesY, SubWorld[0].m_dwCurrentTime);",
"\t\t\t\t\tNpc[nIdx].SendCommand((NPCCMD)NpcSync->Doing, nS9DesX, nS9DesY);",
"\t\t\t\t}",
"\t\t\t}",
"\t\t}")

if c.count(old)!=1:
    print("LOI: neo khop %d lan"%c.count(old)); sys.exit(1)
c=c.replace(old,new)
if sum(1 for ch in c if ord(ch)>127)!=hb:
    print("LOI high-byte doi"); sys.exit(1)
io.open(p,'w',encoding='latin-1',newline='').write(c)
print("OK FIX-2 r2 (high-byte %d giu nguyen)"%hb)
