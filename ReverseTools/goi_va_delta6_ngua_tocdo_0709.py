# -*- coding: ascii -*-
"""delta_patch6.py - [DELTA 07/09 f] ngua + toc do di/chay HIEN TAI di theo goi vi tri gon (NPC_POS_SYNC 25 -> 28 byte),
bo HorseType/WalkSpeed/RunSpeed/AttackSpeed/CastSpeed khoi bam goi 75 (bot len/xuong ngua lien tuc: [PS-BO] nhom nhieu=90 %).
Khi phai gui goi 77 day du ma ngua/toc do cung doi -> gui them mot goi gon. Byte-safe nhu delta_patch.py."""
import io, sys
S = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
KPRO_H = S + r"\KProtocol.h"; KNPC_C = S + r"\KNpc.cpp"; KPP_C = S + r"\KProtocolProcess.cpp"
edits = []
def E(f, old, new, count=1): edits.append((f, old, new, count))

# 1. goi gon 28 byte
E(KPRO_H, "\tint\t\tm_CurrentMana;\n} NPC_POS_SYNC;\t\t\t\t\t\t// 25 byte (pack 1)\n",
  "\tint\t\tm_CurrentMana;\n"
  "\tBYTE\tHorseType;\t\t// [DELTA 07/09 f] ngua dang cuoi (255 = khong), truoc chi co trong goi 75\n"
  "\tBYTE\tWalkSpeed;\t\t// toc do di/chay HIEN TAI (nhu goi 75) - doi theo buff, len/xuong ngua\n"
  "\tBYTE\tRunSpeed;\n"
  "} NPC_POS_SYNC;\t\t\t\t\t\t// 28 byte (pack 1); ban Linux 27 byte\n")

# 2. KNpc.cpp: static + quyet dinh + fill + phat them
E(KNPC_C, "static DWORD s_adwNSLucDay[MAX_NPC];\t// GetTickCount lan phat DAY DU truoc\n",
  "static DWORD s_adwNSLucDay[MAX_NPC];\t// GetTickCount lan phat DAY DU truoc\n"
  "static DWORD s_adwNSNgua[MAX_NPC];\t// [DELTA 07/09 f] bam ngua+toc do lan phat truoc (de gui them goi gon sau goi day du)\n"
  "static int   s_nNSGonThem = 0;\n")

E(KNPC_C, "\tBOOL bPhat = TRUE, bGon = FALSE;\n\tNPC_POS_SYNC sGon;\n",
  "\tBOOL bPhat = TRUE, bGon = FALSE, bGonThem = FALSE;\n\tNPC_POS_SYNC sGon;\n"
  "\t// [DELTA 07/09 f] ngua + toc do hien tai: doi lien tuc (bot len/xuong ngua, buff) -> di theo goi gon, khong qua goi 75\n"
  "\tBYTE abyNgua[3];\n"
  "\tabyNgua[0] = (BYTE)(m_bRideHorse ? m_HorseType : -1);\n"
  "\tabyNgua[1] = (BYTE)m_CurrentWalkSpeed;\n"
  "\tabyNgua[2] = (BYTE)m_CurrentRunSpeed;\n"
  "\tconst DWORD dwBamNgua = PS_Bam(abyNgua, 3);\n")

E(KNPC_C, "\t\t\tconst DWORD dwBamTat = PS_Bam(&NpcSync, (int)sizeof(NpcSync));\n",
  "\t\t\tconst DWORD dwBamTat = PS_Bam(&NpcSync, (int)sizeof(NpcSync)) ^ (dwBamNgua * 0x9E3779B1u);\t// [DELTA 07/09 f] gom ca ngua/toc do\n")

E(KNPC_C, "\t\t\tif (bPhat)\n\t\t\t{\n\t\t\t\ts_adwNSBamTat[m_Index] = dwBamTat;\n\t\t\t\ts_adwNSLucGui[m_Index] = dwLuc;\n",
  "\t\t\tif (bPhat)\n\t\t\t{\n"
  "\t\t\t\tif (!bGon && dwBamNgua != s_adwNSNgua[m_Index])\t// [DELTA 07/09 f] goi day du khong mang ngua/toc do -> gui them goi gon\n"
  "\t\t\t\t\tbGonThem = TRUE;\n"
  "\t\t\t\ts_adwNSNgua[m_Index] = dwBamNgua;\n"
  "\t\t\t\ts_adwNSBamTat[m_Index] = dwBamTat;\n\t\t\t\ts_adwNSLucGui[m_Index] = dwLuc;\n")

E(KNPC_C, "\t\tif (bGon)\n\t\t{\n\t\t\tsGon.ProtocolType = (BYTE)s2c_syncnpcpos;\n",
  "\t\tif (bGon || bGonThem)\n\t\t{\n\t\t\tsGon.ProtocolType = (BYTE)s2c_syncnpcpos;\n")
E(KNPC_C, "\t\t\tsGon.m_CurrentMana = NpcSync.m_CurrentMana;\n\t\t}\n",
  "\t\t\tsGon.m_CurrentMana = NpcSync.m_CurrentMana;\n"
  "\t\t\tsGon.HorseType = abyNgua[0];\t// [DELTA 07/09 f]\n"
  "\t\t\tsGon.WalkSpeed = abyNgua[1];\n"
  "\t\t\tsGon.RunSpeed = abyNgua[2];\n"
  "\t\t}\n"
  "\t\tif (bGonThem) s_nNSGonThem++;\n")

E(KNPC_C, "\t\t\t\tAUTOLOG(\"[NS-BO] 10s dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n"
  "\t\t\t\ts_nNSBo = 0; s_nNSGonDem = 0; s_nNSDay = 0;\n",
  "\t\t\t\tAUTOLOG(\"[NS-BO] 10s dong bo theo thay doi: bo=%d gon=%d day=%d gon_them=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSGonThem, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n"
  "\t\t\t\ts_nNSBo = 0; s_nNSGonDem = 0; s_nNSDay = 0; s_nNSGonThem = 0;\n")

# phat: them goi gon sau goi day du khi ngua/toc do doi
E(KNPC_C, "\tif (bPhat)\n"
  "\t{\n"
  "\t\tCURREGION.BroadCast(pNSBuf, dwNSSize, nMaxCount, m_MapX, m_MapY);\n"
  "\t\tfor (j = 0; j < 8; j++)\n"
  "\t\t{\n"
  "\t\t\tint nConRegion = CURREGION.m_nConnectRegion[j];\n"
  "\t\t\tif (nConRegion == -1)\n"
  "\t\t\t\tcontinue;\n"
  "\t\t\t_ASSERT(m_SubWorldIndex >= 0 && nConRegion >= 0);\n"
  "\t\t\tSubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)pNSBuf, dwNSSize, nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);\n"
  "\t\t}\n"
  "\t}\n",
  "\tif (bPhat)\n"
  "\t{\n"
  "\t\t// [DELTA 07/09 f] lan 0 = goi da chon (gon/day du); lan 1 = goi gon them khi goi day du khong mang ngua/toc do vua doi\n"
  "\t\tfor (int nNSLan = 0; nNSLan < 2; nNSLan++)\n"
  "\t\t{\n"
  "\t\t\tif (nNSLan == 1 && !bGonThem)\n"
  "\t\t\t\tbreak;\n"
  "\t\t\tconst void* pB = (nNSLan == 0) ? pNSBuf : (const void*)&sGon;\n"
  "\t\t\tconst DWORD dwB = (nNSLan == 0) ? dwNSSize : (DWORD)sizeof(NPC_POS_SYNC);\n"
  "\t\t\tint nMaxLan = NPC_SYNC_BROADCAST_LIMIT;\n"
  "\t\t\tCURREGION.BroadCast(pB, dwB, nMaxLan, m_MapX, m_MapY);\n"
  "\t\t\tfor (j = 0; j < 8; j++)\n"
  "\t\t\t{\n"
  "\t\t\t\tint nConRegion = CURREGION.m_nConnectRegion[j];\n"
  "\t\t\t\tif (nConRegion == -1)\n"
  "\t\t\t\t\tcontinue;\n"
  "\t\t\t\t_ASSERT(m_SubWorldIndex >= 0 && nConRegion >= 0);\n"
  "\t\t\t\tSubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)pB, dwB, nMaxLan, m_MapX - POff[j].x, m_MapY - POff[j].y);\n"
  "\t\t\t}\n"
  "\t\t}\n"
  "\t}\n")

# 75: bo ngua + 4 toc do khoi bam
E(KNPC_C, "\t\t\tsPSBam.m_btSomeFlag &= ~0x02;\n",
  "\t\t\tsPSBam.m_btSomeFlag &= ~0x02;\n"
  "\t\t\t// [DELTA 07/09 f] ngua + toc do di/chay di theo goi gon 221; toc do danh/ra chieu client da nhan qua goi 77 (m_ASpeed/m_CSpeed)\n"
  "\t\t\tsPSBam.HorseType = 0; sPSBam.WalkSpeed = 0; sPSBam.RunSpeed = 0; sPSBam.AttackSpeed = 0; sPSBam.CastSpeed = 0;\n")

# 3. client: nhan ngua + toc do tu goi gon (nguoi choi khac)
E(KPP_C, "\tconst int nIdx = NpcSet.SearchID(pGon->ID);\n\tif (nIdx > 0 && nIdx < MAX_NPC)\n\t{\n",
  "\tconst int nIdx = NpcSet.SearchID(pGon->ID);\n\tif (nIdx > 0 && nIdx < MAX_NPC)\n\t{\n"
  "\t\tif (Npc[nIdx].m_Kind == kind_player && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)\n"
  "\t\t{\t// [DELTA 07/09 f] ngua + toc do hien tai tu goi gon (truoc chi qua goi 75; y het SyncPlayerMin)\n"
  "\t\t\tNpc[nIdx].m_CurrentWalkSpeed = pGon->WalkSpeed;\n"
  "\t\t\tNpc[nIdx].m_CurrentRunSpeed = pGon->RunSpeed;\n"
  "\t\t\tNpc[nIdx].m_HorseType = (char)pGon->HorseType;\n"
  "\t\t\tNpc[nIdx].m_bRideHorse = (Npc[nIdx].m_HorseType >= 0) ? TRUE : FALSE;\n"
  "\t\t}\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
files = {}
for f, old, new, cnt in edits: files.setdefault(f, rd(f))
before = {f: hb(s) for f, s in files.items()}
for f, old, new, cnt in edits:
    old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
    s = files[f]; n = s.count(old)
    if n != cnt: print("FAIL %s: found %d (expect %d): %s" % (f, n, cnt, old[:90].encode("ascii", "replace").decode())); sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII", f); sys.exit(1)
    files[f] = s.replace(old, new)
for f, s in files.items():
    if hb(s) != before[f]: print("FAIL high-byte changed", f); sys.exit(1)
for f, s in files.items(): wr(f, s); print("OK", f.split("\\")[-1], "high-bytes", before[f])
print("ALL APPLIED")
