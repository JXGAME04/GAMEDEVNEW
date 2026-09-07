# -*- coding: ascii -*-
"""delta_patch7.py - [DELTA 07/09 g]
 (1) ky lam moi goi 75 ngoai hinh: mac dinh 30 -> 300 s, tran 60 -> 3600 (config [Server] BroadCastLamMoi)
 (2) goi 207 so sat thuong: nguoi xem nhan ban GON 13 byte (s2c_showdamagegon = 222, bo dwLauncher),
     hai nguoi trong cuoc bi LOAI khoi phat vung (ho van nhan goi rieng day du nhu cu -> het nhan 2 lan)
 (3) hello phien ban 2; may chu luu phien ban, chi phat 222 khi moi client noi da bao >= 2
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
R = r"D:\GAMEDEVNEW_wt_delta"
F = {
 "def":  R + r"\Headers\KProtocolDef.h",
 "ph":   R + r"\Sources\Core\Src\KProtocol.h",
 "pc":   R + r"\Sources\Core\Src\KProtocol.cpp",
 "pph":  R + r"\Sources\Core\Src\KProtocolProcess.h",
 "ppc":  R + r"\Sources\Core\Src\KProtocolProcess.cpp",
 "npc":  R + r"\Sources\Core\Src\KNpc.cpp",
 "rh":   R + r"\Sources\Core\Src\KRegion.h",
 "rc":   R + r"\Sources\Core\Src\KRegion.cpp",
}
edits = {k: [] for k in F}
def E(k, old, new, count=1): edits[k].append((old, new, count))

# ---- 1. ma goi 222 ----
E("def", "//\ts2c_dynamic_structure,\t//dynamic structure\n",
  "\ts2c_showdamagegon,\t\t// [DELTA 07/09 g] = 222: so sat thuong GON cho nguoi xem (DAMAGESHOW_GON 13 byte = DAMAGESHOW bo dwLauncher).\n"
  "\t\t\t\t\t\t\t// g_nProtocolSize (KProtocol.cpp) phai co o 157 cho ma nay. Chi phat khi moi client noi da bao hello phien ban 2.\n"
  "//\ts2c_dynamic_structure,\t//dynamic structure\n")

# ---- 2. cau truc ----
E("ph", "} DAMAGESHOW, * PDAMAGESHOW;\n",
  "} DAMAGESHOW, * PDAMAGESHOW;\n"
  "\n"
  "// [DELTA 07/09 g] s2c_showdamagegon = 222: ban GON cho nguoi xem = DAMAGESHOW bo dwLauncher (client chi dung nguoi nhan,\n"
  "// so, loai, chi mang - KNpc::SetBlood2). 13 byte thay vi 17. Thu tu truong y het DAMAGESHOW de client chep thang.\n"
  "typedef struct tagDamageShowGon\n"
  "{\n"
  "\tBYTE\t\t\tProtocolType;\n"
  "\tBYTE\t\t\tenType;\n"
  "\tint\t\t\t\tnDamage;\n"
  "\tWORD\t\t\tSkillId;\n"
  "\tBYTE\t\t\tIsCrit;\n"
  "\tDWORD\t\t\tdwReceiver;\n"
  "} DAMAGESHOW_GON;\n")

# ---- 3. bang co goi client (o 157) ----
E("pc", "\tsizeof(NPC_POS_SYNC),\t\t// s2c_syncnpcpos = 221 [DELTA 07/09] - o 156 = ma 221 (ngay sau 220), KHONG chen gi giua\n",
  "\tsizeof(NPC_POS_SYNC),\t\t// s2c_syncnpcpos = 221 [DELTA 07/09] - o 156 = ma 221 (ngay sau 220), KHONG chen gi giua\n"
  "\tsizeof(DAMAGESHOW_GON),\t\t// s2c_showdamagegon = 222 [DELTA 07/09 g] - o 157 = ma 222 (ngay sau 221), KHONG chen gi giua\n")

# ---- 4. handler client ----
E("pph", "\tvoid\ts2cShowDamage(BYTE* pMsg);\n",
  "\tvoid\ts2cShowDamage(BYTE* pMsg);\n"
  "\tvoid\ts2cShowDamageGon(BYTE* pMsg);\t// [DELTA 07/09 g] so sat thuong GON 13 byte (khong dwLauncher)\n")

E("ppc", "\tProcessFunc[s2c_syncnpcpos] = &KProtocolProcess::SyncNpcPos;\t// [DELTA 07/09]\n",
  "\tProcessFunc[s2c_syncnpcpos] = &KProtocolProcess::SyncNpcPos;\t// [DELTA 07/09]\n"
  "\tProcessFunc[s2c_showdamagegon] = &KProtocolProcess::s2cShowDamageGon;\t// [DELTA 07/09 g]\n")

E("ppc", "void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)\n",
  "// [DELTA 07/09 g] so sat thuong GON cho nguoi xem (s2c_showdamagegon = 222): 13 byte = DAMAGESHOW bo dwLauncher\n"
  "// (client chi dung nguoi nhan, so, loai, chi mang - xem SetBlood2; dwLauncher chi de ghi log). Dung lai DAMAGESHOW\n"
  "// day du voi dwLauncher = 0 de di chung mot duong voi s2cShowDamage.\n"
  "void KProtocolProcess::s2cShowDamageGon(BYTE* pMsg)\n"
  "{\n"
  "\tDAMAGESHOW_GON* pGon = (DAMAGESHOW_GON*)pMsg;\n"
  "\tDAMAGESHOW sDay;\n"
  "\tmemset(&sDay, 0, sizeof(sDay));\n"
  "\tsDay.ProtocolType = (BYTE)s2c_show_damage;\n"
  "\tsDay.enType = pGon->enType;\n"
  "\tsDay.nDamage = pGon->nDamage;\n"
  "\tsDay.SkillId = pGon->SkillId;\n"
  "\tsDay.IsCrit = pGon->IsCrit;\n"
  "\tsDay.dwReceiver = pGon->dwReceiver;\n"
  "\tsDay.dwLauncher = 0;\n"
  "\ts2cShowDamage((BYTE*)&sDay);\n"
  "}\n"
  "void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)\n")

# ---- 5. hello phien ban 2 ----
E("ppc", "\t\tsHello.byPhienBan = 1;\n",
  "\t\tsHello.byPhienBan = 2;\t// [DELTA 07/09 g] 2 = hieu them s2c_showdamagegon (222); may chu cu coi moi gia tri nhu nhau\n")

E("ppc", "\tif (nIndex > 0 && nIndex < MAX_PLAYER)\n\t\tg_abyDeltaHello[nIndex] = 1;\n",
  "\t// [DELTA 07/09 g] luu PHIEN BAN client bao: 1 = hieu 221 (vi tri gon), 2 = hieu them 222 (so sat thuong gon); toi thieu 1\n"
  "\tC2S_DELTA_HELLO* pHello = (C2S_DELTA_HELLO*)pMsg;\n"
  "\tif (nIndex > 0 && nIndex < MAX_PLAYER)\n"
  "\t\tg_abyDeltaHello[nIndex] = (pHello != NULL && pHello->byPhienBan > 1) ? pHello->byPhienBan : (BYTE)1;\n")

# ---- 6. may chu: ky lam moi 75 + phat 207 gon ----
E("npc", "\t\t\ts_nPSLamMoi = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastLamMoi\", 30, \".\\\\config.ini\");\t// [DELTA 07/09] 5 -> 30 s\n"
         "\t\t\tif (s_nPSLamMoi < 1)  s_nPSLamMoi = 1;\n"
         "\t\t\tif (s_nPSLamMoi > 60) s_nPSLamMoi = 60;\n",
  "\t\t\ts_nPSLamMoi = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastLamMoi\", 300, \".\\\\config.ini\");\t// [DELTA 07/09 g] 30 -> 300 s: goi 75 da loc theo bam (ngua/toc do/co chien dau di theo 221), ky lam moi chi la luoi an toan\n"
  "\t\t\tif (s_nPSLamMoi < 1)  s_nPSLamMoi = 1;\n"
  "\t\t\tif (s_nPSLamMoi > 3600) s_nPSLamMoi = 3600;\n")

E("npc", "\treturn s_nCu;\n}\n",
  "\treturn s_nCu;\n}\n"
  "// [DELTA 07/09 g] so client that dang noi ma chua bao PHIEN BAN >= 2 (chua hieu 222 so sat thuong gon) - dem lai moi giay\n"
  "static int NS_SoClientCu2(DWORD dwLuc)\n"
  "{\n"
  "\tstatic DWORD s_dwMoc2 = 0;\n"
  "\tstatic int   s_nCu2 = 0;\n"
  "\tif (s_dwMoc2 == 0 || (dwLuc - s_dwMoc2) >= 1000)\n"
  "\t{\n"
  "\t\ts_dwMoc2 = dwLuc;\n"
  "\t\tint n = 0;\n"
  "\t\tfor (int i = 1; i < MAX_PLAYER; i++)\n"
  "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && g_abyDeltaHello[i] < 2)\n"
  "\t\t\t\tn++;\n"
  "\t\ts_nCu2 = n;\n"
  "\t}\n"
  "\treturn s_nCu2;\n"
  "}\n")

E("npc", "void KNpc::SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit, bool bBroadCast /* = false */)\n{\n",
  "static int NS_SoClientCu2(DWORD dwLuc);\t// [DELTA 07/09 g] dinh nghia ben duoi (canh NS_SoClientCu)\n"
  "void KNpc::SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit, bool bBroadCast /* = false */)\n{\n")

E("npc", "\tdamInfo.dwLauncher = Npc[nLauncher].GetId();\n\tbBroadCast = true;\n",
  "\tdamInfo.dwLauncher = Npc[nLauncher].GetId();\n"
  "\t// [DELTA 07/09 g] nguoi xem nhan ban GON 13 byte (bo dwLauncher, client khong dung); hai nguoi trong cuoc (danh / bi danh)\n"
  "\t// van nhan ban day du bang goi rieng ben duoi -> LOAI HO khoi phat vung de khong nhan hai lan (truoc day ho nhan 2 goi\n"
  "\t// trung nhau, client ve chong len nhau nen khong thay). Chi phat gon khi moi client noi da bao hello phien ban 2.\n"
  "\tDAMAGESHOW_GON gonInfo;\n"
  "\tgonInfo.ProtocolType = (BYTE)s2c_showdamagegon;\n"
  "\tgonInfo.enType = damInfo.enType;\n"
  "\tgonInfo.nDamage = damInfo.nDamage;\n"
  "\tgonInfo.SkillId = damInfo.SkillId;\n"
  "\tgonInfo.IsCrit = damInfo.IsCrit;\n"
  "\tgonInfo.dwReceiver = damInfo.dwReceiver;\n"
  "\tconst bool  bGon = (NS_SoClientCu2(GetTickCount()) == 0);\n"
  "\tconst void* pPhat = bGon ? (const void*)&gonInfo : (const void*)&damInfo;\n"
  "\tconst DWORD dwPhat = bGon ? (DWORD)sizeof(gonInfo) : (DWORD)sizeof(damInfo);\n"
  "\tint nBoNguoi1 = -1, nBoNguoi2 = -1;\n"
  "\tif (nLauncher != m_Index && Npc[nLauncher].m_nPlayerIdx >= 0)\n"
  "\t\tnBoNguoi1 = Npc[nLauncher].m_nPlayerIdx;\n"
  "\tif (GetKind() == kind_player)\n"
  "\t\tnBoNguoi2 = GetPlayerIdx();\n"
  "\t{\n"
  "\t\tstatic int s_nDmgGon = 0, s_nDmgDay = 0;\n"
  "\t\tif (bGon) s_nDmgGon++; else s_nDmgDay++;\n"
  "\t\tAUTOLOG_EVERY(20000, \"[DMG-GON] so sat thuong phat vung: gon=%d day=%d (client chua bao phien ban 2: %d)\", s_nDmgGon, s_nDmgDay, NS_SoClientCu2(GetTickCount()));\n"
  "\t}\n"
  "\tbBroadCast = true;\n")

E("npc", "\t\t\t\tCURREGION.BroadCast(&damInfo, sizeof(damInfo), nMaxCount, m_MapX, m_MapY);\n",
  "\t\t\t\tCURREGION.BroadCast(pPhat, dwPhat, nMaxCount, m_MapX, m_MapY, nBoNguoi1, nBoNguoi2);\t// [DELTA 07/09 g]\n")
E("npc", "\t\t\t\t\tCONREGION(i).BroadCast(&damInfo, sizeof(damInfo), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);\n",
  "\t\t\t\t\tCONREGION(i).BroadCast(pPhat, dwPhat, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y, nBoNguoi1, nBoNguoi2);\t// [DELTA 07/09 g]\n")

# ---- 7. KRegion::BroadCast bo nguoi trong cuoc ----
E("rh", "\tvoid\t\tBroadCast(const void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY);\n",
  "\tvoid\t\tBroadCast(const void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY, int nBoNguoi1 = -1, int nBoNguoi2 = -1);\t// [DELTA 07/09 g] nBoNguoi = chi so Player KHONG gui (ho nhan goi rieng)\n")
E("rc", "void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)\n",
  "void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY, int nBoNguoi1 /* = -1 */, int nBoNguoi2 /* = -1 */)\n")
E("rc", "\t\tif (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)\n"
        "\t\t{\n"
        "\t\t\tint nPlayerIndex = pNode->m_nIndex;\n"
        "\t\t\tint nNpcIndex = Player[nPlayerIndex].m_nIndex;\n"
        "\t\t\tint nDX = Npc[nNpcIndex].m_MapX - nOX;\n",
  "\t\tif (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER && pNode->m_nIndex != nBoNguoi1 && pNode->m_nIndex != nBoNguoi2)\t// [DELTA 07/09 g] bo nguoi trong cuoc (ho nhan goi rieng)\n"
  "\t\t{\n"
  "\t\t\tint nPlayerIndex = pNode->m_nIndex;\n"
  "\t\t\tint nNpcIndex = Player[nPlayerIndex].m_nIndex;\n"
  "\t\t\tint nDX = Npc[nNpcIndex].m_MapX - nOX;\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# kiem tat ca truoc, ghi sau (khong ghi nua chung)
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
