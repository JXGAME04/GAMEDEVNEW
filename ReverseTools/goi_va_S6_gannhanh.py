# -*- coding: utf-8 -*-
# [S6-GANNHANH 28/08 toi] Chu game: "BOT - NPC - nguoi choi hay bi nhay toa do".
# Do (client log 20:15-21:02, tran TK 500 bot): 90 cu BIEN MAT roi hien lai 2-60s
# (p50 = 8,4 giay!) = ban sao bi MO COI do recenter (KRegion::Close) roi phai cho
# vong quay sync cua server (5 npc/tick/region, region dong ~150 con) toi 8s+ moi
# duoc gan lai; 131 cu S10-SNAP 3-5 o la nan troi binh thuong. Ma nhin thay = 0.
# Va: trong KNpcSet::CheckBalance (chay MOI TICK client) - con mo coi ma:
#   - SyncSignal con TUOI (<= 36 khung ~2s: server VUA sync no = con song, vi tri
#     chinh xac; gac nay tu loai ma cu),
#   - khong phai chet/hoi sinh (ke toan AddRef/DecRef co gac death - khong dung),
#   - khong phai partner (he rieng tu quan),
#   - con trong 38 o quanh minh (ngoai vanh de bi VANH 42 go lai = flap),
#   - region cu DA NAP LAI (khop m_RegionID VA nam trong cua so 3x3 quanh region
#     cua minh - chan slot cu ky)
# -> GAN LAI NGAY (AddNpc + AddRef, dung khuon [S6-ORPHAN-BACK]) thay vi doi 8s.
# Nhan dem duoc: [S6-GANNHANH]. CLIENT-only (CheckBalance trong #ifndef _SERVER).
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpcSet.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

ap("H1 gan nhanh mo coi tuoi",
 '\t\tif (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)\n'
 '\t\t{\n'
 '\t\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)\n'
 '\t\t\t{\n',
 '\t\t// [S6-GANNHANH 28/08] (chu game: "BOT/NPC/nguoi choi hay nhay toa do, bien mat\n'
 '\t\t// roi hien lai") Do 20:15-21:02: 90 cu bien-mat 2-60s (p50 8,4s) = mo coi do\n'
 '\t\t// RECENTER roi cho vong quay sync server qua lau. Con mo coi ma SyncSignal con\n'
 '\t\t// TUOI (<=36 khung ~2s: server VUA sync = con song, vi tri chuan - tu loai ma cu)\n'
 '\t\t// va region cu DA NAP LAI trong cua so 3x3 -> gan lai NGAY, khoi cho 8s.\n'
 '\t\t// Gac: bo chet/hoi sinh (ke toan ref co gac death), partner (he rieng), chi gan\n'
 '\t\t// khi con trong 38 o (ngoai hon la de VANH 42 go lai = flap).\n'
 '\t\tif (Npc[nIdx].m_RegionIndex < 0\n'
 '\t\t && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex\n'
 '\t\t && Npc[nIdx].m_Kind != kind_partner\n'
 '\t\t && Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive\n'
 '\t\t && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal <= 36)\n'
 '\t\t{\n'
 '\t\t\tconst int nSelf6 = Player[CLIENT_PLAYER_INDEX].m_nIndex;\n'
 '\t\t\tif (nSelf6 > 0 && nSelf6 < MAX_NPC && Npc[nSelf6].m_RegionIndex >= 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int nGx6 = (int)LOWORD(Npc[nIdx].m_dwRegionID) - (int)LOWORD(Npc[nSelf6].m_dwRegionID);\n'
 '\t\t\t\tconst int nGy6 = (int)HIWORD(Npc[nIdx].m_dwRegionID) - (int)HIWORD(Npc[nSelf6].m_dwRegionID);\n'
 '\t\t\t\tconst int nRw6 = SubWorld[0].m_nRegionWidth;\n'
 '\t\t\t\tconst int nRh6 = SubWorld[0].m_nRegionHeight;\n'
 '\t\t\t\tconst int nDx6 = nGx6 * nRw6 + Npc[nIdx].m_MapX - Npc[nSelf6].m_MapX;\n'
 '\t\t\t\tconst int nDy6 = nGy6 * nRh6 + Npc[nIdx].m_MapY - Npc[nSelf6].m_MapY;\n'
 '\t\t\t\tconst int nAx6 = (nDx6 < 0) ? -nDx6 : nDx6;\n'
 '\t\t\t\tconst int nAy6 = (nDy6 < 0) ? -nDy6 : nDy6;\n'
 '\t\t\t\tif (nGx6 >= -1 && nGx6 <= 1 && nGy6 >= -1 && nGy6 <= 1\n'
 '\t\t\t\t && nAx6 <= 38 && nAy6 <= 38)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tfor (int r6 = 0; r6 < MAX_REGION; r6++)\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tif (SubWorld[0].m_Region[r6].m_RegionID != Npc[nIdx].m_dwRegionID)\n'
 '\t\t\t\t\t\t\tcontinue;\n'
 '\t\t\t\t\t\tNpc[nIdx].m_RegionIndex = r6;\n'
 '\t\t\t\t\t\tSubWorld[0].m_Region[r6].AddNpc(nIdx);\n'
 '\t\t\t\t\t\tSubWorld[0].m_Region[r6].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);\n'
 '\t\t\t\t\t\tAUTOLOG("[S6-GANNHANH] npc=%u idx=%d kind=%u cell=(%d,%d) reg=%d t=%u", Npc[nIdx].m_dwID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, r6, SubWorld[0].m_dwCurrentTime);\n'
 '\t\t\t\t\t\tbreak;\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\tif (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)\n'
 '\t\t{\n'
 '\t\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)\n'
 '\t\t\t{\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
