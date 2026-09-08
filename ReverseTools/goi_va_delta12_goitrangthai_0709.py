# -*- coding: ascii -*-
"""delta_patch12.py - [DELTA 07/09 l] goi TRANG THAI gon s2c_syncnpcstate = 223.
Do 16:21-16:58 (va k): trong 100 giay, bam cham doi 40.490 lan thi 30.400 (75 %) la nhom TRANG THAI KY NANG
(StateInfo) va 9.457 (23 %) la CHI SO TOI DA (LifeMax/ManaMax); toc do = 0. Moi lan nhu vay phai phat goi
NPC_NORMAL_SYNC 98 byte du toa do khong doi.
Va l: bo hai nhom nay khoi bam cham, gui rieng NPC_STATE_SYNC 39 byte (1 + 4 + 18 + 16). Khi van phai gui goi
day du thi goi do da mang san trang thai -> khong gui them. Chi bat khi MOI client noi da bao hello phien ban 3.
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
F = {
 "def": r"D:\GAMEDEVNEW_wt_delta\Headers\KProtocolDef.h",
 "ph":  r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocol.h",
 "pc":  r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocol.cpp",
 "pph": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocolProcess.h",
 "ppc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KProtocolProcess.cpp",
 "npc": r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp",
}
edits = {k: [] for k in F}
def E(k, old, new, count=1): edits[k].append((old, new, count))

# ---- 1. ma goi 223 ----
E("def", "//\ts2c_dynamic_structure,\t//dynamic structure\n",
  "\ts2c_syncnpcstate,\t\t// [DELTA 07/09 l] = 223: trang thai ky nang + chi so toi da (NPC_STATE_SYNC 39 byte).\n"
  "\t\t\t\t\t\t\t// Thay goi day du 98 byte khi CHI hai nhom nay doi (do live: 98 % so lan bam cham doi).\n"
  "\t\t\t\t\t\t\t// g_nProtocolSize (KProtocol.cpp) phai co o 158 cho ma nay. Chi phat khi client bao hello phien ban 3.\n"
  "//\ts2c_dynamic_structure,\t//dynamic structure\n")

# ---- 2. cau truc ----
E("ph", "typedef struct\n{\n\tBYTE\tProtocolType;\n\tBYTE\tWeatherID;\n} SYNC_WEATHER;\n",
  "// [DELTA 07/09 l] s2c_syncnpcstate = 223: chi trang thai ky nang + chi so toi da (39 byte thay vi 98 byte cua\n"
  "// NPC_NORMAL_SYNC). Client ap y het phan tuong ung trong SyncNpcMin.\n"
  "typedef struct\n"
  "{\n"
  "\tBYTE\t\t\tProtocolType;\n"
  "\tDWORD\t\t\tID;\n"
  "\tBYTE\t\t\tStateInfo[MAX_SKILL_STATE];\n"
  "\tint\t\t\t\tm_CurrentLifeMax;\n"
  "\tint\t\t\t\tm_LifeMax;\n"
  "\tint\t\t\t\tm_CurrentManaMax;\n"
  "\tint\t\t\t\tm_ManaMax;\n"
  "} NPC_STATE_SYNC;\n"
  "\n"
  "typedef struct\n{\n\tBYTE\tProtocolType;\n\tBYTE\tWeatherID;\n} SYNC_WEATHER;\n")

# ---- 3. bang co goi client (o 158) ----
E("pc", "\tsizeof(DAMAGESHOW_GON),\t\t// s2c_showdamagegon = 222 [DELTA 07/09 g] - o 157 = ma 222 (ngay sau 221), KHONG chen gi giua\n",
  "\tsizeof(DAMAGESHOW_GON),\t\t// s2c_showdamagegon = 222 [DELTA 07/09 g] - o 157 = ma 222 (ngay sau 221), KHONG chen gi giua\n"
  "\tsizeof(NPC_STATE_SYNC),\t\t// s2c_syncnpcstate = 223 [DELTA 07/09 l] - o 158 = ma 223 (ngay sau 222), KHONG chen gi giua\n")

# ---- 4. client: handler ----
E("pph", "\tvoid\ts2cShowDamageGon(BYTE* pMsg);\t// [DELTA 07/09 g] so sat thuong GON 13 byte (khong dwLauncher)\n",
  "\tvoid\ts2cShowDamageGon(BYTE* pMsg);\t// [DELTA 07/09 g] so sat thuong GON 13 byte (khong dwLauncher)\n"
  "\tvoid\tSyncNpcState(BYTE* pMsg);\t\t// [DELTA 07/09 l] goi trang thai ky nang + chi so toi da (223)\n")

E("ppc", "\tProcessFunc[s2c_showdamagegon] = &KProtocolProcess::s2cShowDamageGon;\t// [DELTA 07/09 g]\n",
  "\tProcessFunc[s2c_showdamagegon] = &KProtocolProcess::s2cShowDamageGon;\t// [DELTA 07/09 g]\n"
  "\tProcessFunc[s2c_syncnpcstate] = &KProtocolProcess::SyncNpcState;\t// [DELTA 07/09 l]\n")

E("ppc", "void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg) //Sync li",
  "// [DELTA 07/09 l] Goi trang thai gon (223): dat y het phan tuong ung cua SyncNpcMin. May chu chi phat goi nay\n"
  "// khi bam trang thai doi MA khong phai gui goi day du (goi day du da mang san trang thai).\n"
  "void KProtocolProcess::SyncNpcState(BYTE* pMsg)\n"
  "{\n"
  "\tNPC_STATE_SYNC* pSync = (NPC_STATE_SYNC*)pMsg;\n"
  "\tint nIdx = NpcSet.SearchID(pSync->ID);\n"
  "\tif (nIdx <= 0 || nIdx >= MAX_NPC)\n"
  "\t\treturn;\n"
  "\tNpc[nIdx].SetNpcState(pSync->StateInfo);\n"
  "\tNpc[nIdx].m_CurrentLifeMax\t= pSync->m_CurrentLifeMax;\n"
  "\tNpc[nIdx].m_LifeMax\t\t\t= pSync->m_LifeMax;\n"
  "\tNpc[nIdx].m_CurrentManaMax\t= pSync->m_CurrentManaMax;\n"
  "\tNpc[nIdx].m_ManaMax\t\t\t= pSync->m_ManaMax;\n"
  "\tNpc[nIdx].m_SyncSignal\t\t= SubWorld[0].m_dwCurrentTime;\n"
  "}\n"
  "\n"
  "void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg) //Sync li")

# ---- 5. hello phien ban 3 ----
E("ppc", "\t\tsHello.byPhienBan = 2;\t// [DELTA 07/09 g] 2 = hieu them s2c_showdamagegon (222); may chu cu coi moi gia tri nhu nhau\n",
  "\t\tsHello.byPhienBan = 3;\t// [DELTA 07/09 l] 3 = hieu them s2c_syncnpcstate (223); 2 = 222; may chu cu coi moi gia tri nhu nhau\n")

# ---- 6. may chu ----
E("npc", "static int   s_anNSNhom[NS_SO_NHOM] = { 0, 0, 0, 0, 0 };\n",
  "static int   s_anNSNhom[NS_SO_NHOM] = { 0, 0, 0, 0, 0 };\n"
  "// [DELTA 07/09 l] bam nhom TRANG THAI KY NANG + CHI SO TOI DA lan phat truoc (goi 223 thay goi day du 98 byte)\n"
  "static DWORD s_adwNSBamTT[MAX_NPC];\n"
  "static int   s_nNSTT = 0;\n")

E("npc", "// so client that dang noi ma CHUA bao hieu goi gon (client cu) - dem lai moi giay; > 0 thi khong phat goi gon cho ai\n",
  "// [DELTA 07/09 l] so client that dang noi ma chua bao phien ban >= 3 (chua hieu goi trang thai 223)\n"
  "static int NS_SoClientCu3(DWORD dwLuc)\n"
  "{\n"
  "\tstatic DWORD s_dwMoc3 = 0;\n"
  "\tstatic int   s_nCu3 = 0;\n"
  "\textern BYTE g_abyDeltaHello[MAX_PLAYER];\n"
  "\tif (s_dwMoc3 == 0 || (dwLuc - s_dwMoc3) >= 1000)\n"
  "\t{\n"
  "\t\ts_dwMoc3 = dwLuc;\n"
  "\t\tint n = 0;\n"
  "\t\tfor (int i = 1; i < MAX_PLAYER; i++)\n"
  "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && g_abyDeltaHello[i] < 3)\n"
  "\t\t\t\tn++;\n"
  "\t\ts_nCu3 = n;\n"
  "\t}\n"
  "\treturn s_nCu3;\n"
  "}\n"
  "// so client that dang noi ma CHUA bao hieu goi gon (client cu) - dem lai moi giay; > 0 thi khong phat goi gon cho ai\n")

E("npc", "\t\t\tconst DWORD dwBamCham = PS_Bam(&sCham, (int)sizeof(sCham));\n",
  "\t\t\t// [DELTA 07/09 l] TRANG THAI KY NANG + CHI SO TOI DA la 98 % so lan bam cham doi (do live 16:21-16:58:\n"
  "\t\t\t// trang_thai 30.400 + chi_so_max 9.457 tren 40.490 lan / 100 giay). Bo ca hai khoi bam cham va gui rieng\n"
  "\t\t\t// goi 223 (39 byte) thay vi goi day du 98 byte. Chi lam khi moi client deu hieu goi 223.\n"
  "\t\t\tconst BOOL bTTMoi = (NS_SoClientCu3(dwLuc) == 0);\n"
  "\t\t\tDWORD dwBamTT = 0;\n"
  "\t\t\tif (bTTMoi)\n"
  "\t\t\t{\n"
  "\t\t\t\tNPC_STATE_SYNC sTTBam;\n"
  "\t\t\t\tmemset(&sTTBam, 0, sizeof(sTTBam));\n"
  "\t\t\t\tmemcpy(sTTBam.StateInfo, NpcSync.StateInfo, sizeof(sTTBam.StateInfo));\n"
  "\t\t\t\tsTTBam.m_CurrentLifeMax = NpcSync.m_CurrentLifeMax;\n"
  "\t\t\t\tsTTBam.m_LifeMax = NpcSync.m_LifeMax;\n"
  "\t\t\t\tsTTBam.m_CurrentManaMax = NpcSync.m_CurrentManaMax;\n"
  "\t\t\t\tsTTBam.m_ManaMax = NpcSync.m_ManaMax;\n"
  "\t\t\t\tdwBamTT = PS_Bam(&sTTBam, (int)sizeof(sTTBam));\n"
  "\t\t\t\tmemset(sCham.StateInfo, 0, sizeof(sCham.StateInfo));\n"
  "\t\t\t\tsCham.m_CurrentLifeMax = 0; sCham.m_LifeMax = 0; sCham.m_CurrentManaMax = 0; sCham.m_ManaMax = 0;\n"
  "\t\t\t}\n"
  "\t\t\tconst DWORD dwBamCham = PS_Bam(&sCham, (int)sizeof(sCham));\n")

E("npc", "\t\t\t\tif (!bGon)\n"
         "\t\t\t\t{\n"
         "\t\t\t\t\ts_adwNSBamCham[m_Index] = dwBamCham;\n"
         "\t\t\t\t\ts_adwNSLucDay[m_Index] = dwLuc;\n"
         "\t\t\t\t\tfor (int k = 0; k < NS_SO_NHOM; k++)\t// [DELTA 07/09 k] moc nhom luu CUNG LUC voi bam cham\n"
         "\t\t\t\t\t\ts_adwNSNhom[k][m_Index] = adwNSNhom[k];\n"
         "\t\t\t\t}\n",
  "\t\t\t\tif (!bGon)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_adwNSBamCham[m_Index] = dwBamCham;\n"
  "\t\t\t\t\ts_adwNSLucDay[m_Index] = dwLuc;\n"
  "\t\t\t\t\tfor (int k = 0; k < NS_SO_NHOM; k++)\t// [DELTA 07/09 k] moc nhom luu CUNG LUC voi bam cham\n"
  "\t\t\t\t\t\ts_adwNSNhom[k][m_Index] = adwNSNhom[k];\n"
  "\t\t\t\t}\n"
  "\t\t\t\t// [DELTA 07/09 l] trang thai/chi so toi da doi: goi DAY DU da mang san -> chi cap nhat moc;\n"
  "\t\t\t\t// goi GON khong mang -> phat them goi 223 (39 byte).\n"
  "\t\t\t\tif (bTTMoi && dwBamTT != s_adwNSBamTT[m_Index])\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tif (bGon)\n"
  "\t\t\t\t\t\tbTT = TRUE;\n"
  "\t\t\t\t\ts_adwNSBamTT[m_Index] = dwBamTT;\n"
  "\t\t\t\t}\n")

E("npc", "\t\tif (bGonThem) s_nNSGonThem++;\n",
  "\t\tif (bTT)\n"
  "\t\t{\t// [DELTA 07/09 l] goi trang thai gon\n"
  "\t\t\ts_nNSTT++;\n"
  "\t\t\tsTT.ProtocolType = (BYTE)s2c_syncnpcstate;\n"
  "\t\t\tsTT.ID = m_dwID;\n"
  "\t\t\tmemcpy(sTT.StateInfo, NpcSync.StateInfo, sizeof(sTT.StateInfo));\n"
  "\t\t\tsTT.m_CurrentLifeMax = NpcSync.m_CurrentLifeMax;\n"
  "\t\t\tsTT.m_LifeMax = NpcSync.m_LifeMax;\n"
  "\t\t\tsTT.m_CurrentManaMax = NpcSync.m_CurrentManaMax;\n"
  "\t\t\tsTT.m_ManaMax = NpcSync.m_ManaMax;\n"
  "\t\t}\n"
  "\t\tif (bGonThem) s_nNSGonThem++;\n")

E("npc", "\t\t\t\t\ts_anNSNhom[0], s_anNSNhom[1], s_anNSNhom[2], s_anNSNhom[3], s_anNSNhom[4], s_nNSNhomNhieu);\n",
  "\t\t\t\t\ts_anNSNhom[0], s_anNSNhom[1], s_anNSNhom[2], s_anNSNhom[3], s_anNSNhom[4], s_nNSNhomNhieu);\n"
  "\t\t\t\tAUTOLOG(\"[NS-TT] 10s goi trang thai gon (223): %d lan (client chua bao phien ban 3: %d)\", s_nNSTT, NS_SoClientCu3(dwLuc));\n"
  "\t\t\t\ts_nNSTT = 0;\n")

# bien cuc bo trong NormalSync
E("npc", "\tBOOL bPhat = TRUE, bGon = FALSE, bGonThem = FALSE;\n\tNPC_POS_SYNC sGon;\n",
  "\tBOOL bPhat = TRUE, bGon = FALSE, bGonThem = FALSE;\n"
  "\tBOOL bTT = FALSE;\t// [DELTA 07/09 l] co phat goi trang thai gon 223\n"
  "\tNPC_POS_SYNC sGon;\n"
  "\tNPC_STATE_SYNC sTT;\n")

E("npc", "\t\t\tif (nNSLan == 1 && !bGonThem)\n"
         "\t\t\t\tbreak;\n"
         "\t\t\tconst void* pB = (nNSLan == 0) ? pNSBuf : (const void*)&sGon;\n"
         "\t\t\tconst DWORD dwB = (nNSLan == 0) ? dwNSSize : (DWORD)sizeof(NPC_POS_SYNC);\n",
  "\t\t\tif (nNSLan == 1 && !bGonThem)\n"
  "\t\t\t\tcontinue;\t// [DELTA 07/09 l] van con luot 2 (goi trang thai)\n"
  "\t\t\tif (nNSLan == 2 && !bTT)\n"
  "\t\t\t\tbreak;\n"
  "\t\t\tconst void* pB = (nNSLan == 0) ? pNSBuf : ((nNSLan == 1) ? (const void*)&sGon : (const void*)&sTT);\n"
  "\t\t\tconst DWORD dwB = (nNSLan == 0) ? dwNSSize : ((nNSLan == 1) ? (DWORD)sizeof(NPC_POS_SYNC) : (DWORD)sizeof(NPC_STATE_SYNC));\n")

E("npc", "\t\tfor (int nNSLan = 0; nNSLan < 2; nNSLan++)\n",
  "\t\tfor (int nNSLan = 0; nNSLan < 3; nNSLan++)\t// [DELTA 07/09 l] them luot 2 = goi trang thai 223\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
out = {}
for k, p in F.items():
    if not edits[k]: continue
    s = rd(p); before = hb(s)
    for old, new, cnt in edits[k]:
        old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
        if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII", k); sys.exit(1)
        n = s.count(old)
        if n != cnt:
            print("FAIL %s: found %d (expect %d): %s" % (k, n, cnt, old[:100].encode("ascii", "replace").decode())); sys.exit(1)
        s = s.replace(old, new)
    if hb(s) != before: print("FAIL high-byte changed", k); sys.exit(1)
    out[k] = (p, s, before)
for k, (p, s, before) in out.items():
    wr(p, s); print("OK %-4s high-bytes %5d  %s" % (k, before, p))
print("ALL APPLIED")
