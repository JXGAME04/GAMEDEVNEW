# -*- coding: ascii -*-
"""delta_patch.py - [DELTA 07/09] dong bo theo thay doi + goi 77 gon + bo tran + xa moi tick + TCP_NODELAY.
Byte-safe: doc/ghi latin-1, moi neo ASCII phai khop DUNG MOT lan, dem high-byte truoc/sau (chi duoc doi
o KProtocolProcess.cpp/KNpc.cpp neu ta CO Y giu nguyen dong Viet: o day moi dong Viet cu deu duoc giu)."""
import io, sys, re

ROOT = r"D:\GAMEDEVNEW_wt_delta"
edits = []   # (file, list of (old, new, count))
inserts = [] # (file, anchor_line_substring, text, where) where='before'|'after'

def E(f, old, new, count=1): edits.append((f, old, new, count))
def I(f, anchor, text, where): inserts.append((f, anchor, text, where))

S = ROOT + r"\Sources"
KREG_H = S + r"\Core\Src\KRegion.h"
KREG_C = S + r"\Core\Src\KRegion.cpp"
KNPC_H = S + r"\Core\Src\KNpc.h"
KNPC_C = S + r"\Core\Src\KNpc.cpp"
KPRO_H = S + r"\Core\Src\KProtocol.h"
KPRO_C = S + r"\Core\Src\KProtocol.cpp"
KDEF_H = ROOT + r"\Headers\KProtocolDef.h"
KPP_H  = S + r"\Core\Src\KProtocolProcess.h"
KPP_C  = S + r"\Core\Src\KProtocolProcess.cpp"
KSO_C  = S + r"\MultiServer\GameServer\KSOServer.cpp"
SOCK_C = S + r"\MultiServer\Common\SocketServer.cpp"

# ---------------------------------------------------------------- 1. giao thuc
I(KDEF_H, "\ts2c_scriptdata,\t\t\t// [MAIL 03/09] goi ScriptProtocol",
  "\ts2c_syncnpcpos = 221,\t// [DELTA 07/09] dong bo vi tri GON (NPC_POS_SYNC 25 byte). Ma 221 nam trong doan trong 221-249:\n"
  "\t\t\t\t\t\t\t// KHONG doi so hieu goi nao khac. g_nProtocolSize (KProtocol.cpp) phai co o 156 cho ma nay.\n", "after")

I(KPRO_H, "} NPC_NORMAL_SYNC;",
  "\n// [DELTA 07/09] Goi dong bo vi tri GON (s2c_syncnpcpos): chi phan DOI NHANH cua NPC_NORMAL_SYNC. May chu phat goi nay\n"
  "// khi nhom truong CHAM (phe, he, mau/noi luc toi da, toc do, StateInfo, MissionGroup, NpcEnchant, ProtectedTime)\n"
  "// khong doi so voi lan phat DAY DU truoc. Offset va ma vung KHONG gui: client tinh tu MPS bang Mps2Map (nhu client Linux).\n"
  "// Ban Linux tham chieu dung goi 27 byte cho viec nay; JX1 truoc day 99 byte.\n"
  "typedef struct\n"
  "{\n"
  "\tBYTE\tProtocolType;\t// s2c_syncnpcpos\n"
  "\tDWORD\tID;\n"
  "\tint\t\tMapX;\t\t\t// MPS\n"
  "\tint\t\tMapY;\n"
  "\tBYTE\tDoing;\n"
  "\tBYTE\tState;\t\t\t// STATE_FREEZE/POISON/STUN/HIDE/FROZEN/WALKRUN nhu NPC_NORMAL_SYNC.State\n"
  "\tBYTE\tCamp;\n"
  "\tBYTE\tm_bySeries;\n"
  "\tint\t\tm_CurrentLife;\n"
  "\tint\t\tm_CurrentMana;\n"
  "} NPC_POS_SYNC;\t\t\t\t\t\t// 25 byte (pack 1)\n", "after")

I(KPRO_C, "\t-1,\t\t\t\t\t\t\t// s2c_scriptdata [MAIL 03/09]",
  "\tsizeof(NPC_POS_SYNC),\t\t// s2c_syncnpcpos = 221 [DELTA 07/09] - o 156 = ma 221 (ngay sau 220), KHONG chen gi giua\n", "after")

I(KPP_H, "\tvoid\tSyncNpcMinPlayer(BYTE* pMsg);",
  "\tvoid\tSyncNpcPos(BYTE* pMsg);\t\t// [DELTA 07/09] goi vi tri gon s2c_syncnpcpos\n", "after")

I(KPP_C, "\tProcessFunc[s2c_syncnpcminplayer] = &KProtocolProcess::SyncNpcMinPlayer;",
  "\tProcessFunc[s2c_syncnpcpos] = &KProtocolProcess::SyncNpcPos;\t// [DELTA 07/09]\n", "after")

I(KPP_C, "void KProtocolProcess::SyncNpcMin(BYTE* pMsg)",
  "#ifndef _SERVER\n"
  "// [DELTA 07/09] Goi vi tri GON s2c_syncnpcpos (25 byte): may chu chi gui phan doi nhanh (toa do MPS, Doing, State,\n"
  "// phe, he, mau/noi luc hien tai). Cach xu ly: DUNG LAI mot NPC_NORMAL_SYNC day du tu goi gon + cac truong cham dang\n"
  "// giu trong Npc[] (da nhan qua goi 77 day du hoac SendSyncData), roi giao cho SyncNpcMin - moi nhanh S6/S10, hoi NPC la,\n"
  "// gan lai NPC mo coi... giu nguyen mot dong ma. Offset va ma vung tinh tu MPS bang Mps2Map (dung hai dong tung bi\n"
  "// chu thich trong SyncNpcMin), giong client Linux. NPC chua biet: chi can ID/MapX/MapY/State -> hoi NPC nhu cu.\n"
  "void KProtocolProcess::SyncNpcPos(BYTE* pMsg)\n"
  "{\n"
  "\tNPC_POS_SYNC* pGon = (NPC_POS_SYNC*)pMsg;\n"
  "\tNPC_NORMAL_SYNC sDay;\n"
  "\tmemset(&sDay, 0, sizeof(sDay));\n"
  "\tsDay.ProtocolType = (BYTE)s2c_syncnpcmin;\n"
  "\tsDay.ID = pGon->ID;\n"
  "\tsDay.MapX = pGon->MapX;\n"
  "\tsDay.MapY = pGon->MapY;\n"
  "\tsDay.Doing = pGon->Doing;\n"
  "\tsDay.State = pGon->State;\n"
  "\tsDay.Camp = pGon->Camp;\n"
  "\tsDay.m_bySeries = pGon->m_bySeries;\n"
  "\tsDay.m_CurrentLife = pGon->m_CurrentLife;\n"
  "\tsDay.m_CurrentMana = pGon->m_CurrentMana;\n"
  "\tint nRegion = -1, nMapX = 0, nMapY = 0, nOffX = 0, nOffY = 0;\n"
  "\tSubWorld[0].Mps2Map(pGon->MapX, pGon->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);\n"
  "\tsDay.m_fkOffX = nOffX;\n"
  "\tsDay.m_fkOffY = nOffY;\n"
  "\tsDay.m_fkRegionID = (nRegion >= 0 && nRegion < MAX_REGION) ? SubWorld[0].m_Region[nRegion].m_RegionID : 0;\n"
  "\tconst int nIdx = NpcSet.SearchID(pGon->ID);\n"
  "\tif (nIdx > 0 && nIdx < MAX_NPC)\n"
  "\t{\n"
  "\t\tsDay.m_nProtectedTime = Npc[nIdx].m_nProtectedTime;\n"
  "\t\tsDay.m_CurrentLifeMax = Npc[nIdx].m_CurrentLifeMax;\n"
  "\t\tsDay.m_LifeMax = Npc[nIdx].m_LifeMax;\n"
  "\t\tsDay.m_WalkSpeed = Npc[nIdx].m_WalkSpeed;\n"
  "\t\tsDay.m_RunSpeed = Npc[nIdx].m_RunSpeed;\n"
  "\t\tsDay.m_ASpeed = Npc[nIdx].m_CurrentAttackSpeed;\n"
  "\t\tsDay.m_CSpeed = Npc[nIdx].m_CurrentCastSpeed;\n"
  "\t\tsDay.m_CurrentManaMax = Npc[nIdx].m_CurrentManaMax;\n"
  "\t\tsDay.m_ManaMax = Npc[nIdx].m_ManaMax;\n"
  "\t\tsDay.MissionGroup = Npc[nIdx].m_nMissionGroup;\n"
  "\t\tmemcpy(sDay.StateInfo, Npc[nIdx].m_btStateInfo, sizeof(BYTE) * MAX_SKILL_STATE);\n"
  "\t\tsDay.NpcEnchant = Npc[nIdx].m_Type;\n"
  "\t}\n"
  "\tSyncNpcMin((BYTE*)&sDay);\n"
  "}\n"
  "#endif\n\n", "before")

# ---------------------------------------------------------------- 2. KRegion.h: bo tran + moc nguoi vao vung
E(KREG_H, "#define\tMAX_BROADCAST_COUNT\t\t100",
  "// [DELTA 07/09] (chu game 07/09: ban test, khong gioi han trai nghiem) tran nguoi nhan doc tu config.ini [Server]\n"
  "// BroadCastMotLan (su kien mot lan) / BroadCastDongBo (goi dong bo 77/75), mac dinh 100000 = KHONG cat ai. Chi phi\n"
  "// that bi chan boi so NGUOI THAT trong tam 32 o (bot khong an suat tu F4 04/09). Ban Linux tham chieu: 100 / 1200.\n"
  "// Muon ve tran cu: BroadCastMotLan=100, BroadCastDongBo=500 (khong can build lai).\n"
  "int BC_TranMotLan();\n"
  "int BC_TranDongBo();\n"
  "#define\tMAX_BROADCAST_COUNT\t\tBC_TranMotLan()")
E(KREG_H, "#define\tNPC_SYNC_BROADCAST_LIMIT\t500",
  "#define\tNPC_SYNC_BROADCAST_LIMIT\tBC_TranDongBo()")
E(KREG_H, "\tint\t\t\tm_nBroadCastCursor;\n",
  "\tint\t\t\tm_nBroadCastCursor;\n"
  "\tDWORD\t\tm_dwLucCoNguoiVao;\t// [DELTA 07/09] GetTickCount() lan cuoi co nguoi vao vung (AddPlayer); NormalSync xem 9 vung\n")

# ---------------------------------------------------------------- 3. KRegion.cpp
E(KREG_C, "\treturn (nLoai == 75 || nLoai == 77 || nLoai == 85 || nLoai == 86);\n}\n",
  "\t// [DELTA 07/09] bo 75 (da bam, hiem, KHONG duoc bo vi lam moi 30 s), them 221 goi vi tri gon\n"
  "\treturn (nLoai == 77 || nLoai == 85 || nLoai == 86 || nLoai == 221);\n}\n"
  "#ifdef _SERVER\n"
  "// [DELTA 07/09] tran nguoi nhan (xem chu thich KRegion.h). Doc mot lan, kep 100..100000.\n"
  "int BC_TranMotLan()\n"
  "{\n"
  "\tstatic int s_n = -1;\n"
  "\tif (s_n < 0)\n"
  "\t{\n"
  "\t\ts_n = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastMotLan\", 100000, \".\\\\config.ini\");\n"
  "\t\tif (s_n < 100) s_n = 100;\n"
  "\t\tif (s_n > 100000) s_n = 100000;\n"
  "\t}\n"
  "\treturn s_n;\n"
  "}\n"
  "int BC_TranDongBo()\n"
  "{\n"
  "\tstatic int s_n = -1;\n"
  "\tif (s_n < 0)\n"
  "\t{\n"
  "\t\ts_n = (int)GetPrivateProfileIntA(\"Server\", \"BroadCastDongBo\", 100000, \".\\\\config.ini\");\n"
  "\t\tif (s_n < 100) s_n = 100;\n"
  "\t\tif (s_n > 100000) s_n = 100000;\n"
  "\t}\n"
  "\treturn s_n;\n"
  "}\n"
  "#endif\n")
E(KREG_C, "GetPrivateProfileIntA(\"Server\", \"BroadCastGoiToiDa\", 1500, \".\\\\config.ini\");",
  "GetPrivateProfileIntA(\"Server\", \"BroadCastGoiToiDa\", 5000, \".\\\\config.ini\");\t// [DELTA 07/09] 1500 -> 5000: chi con la van an toan")
E(KREG_C, "\tm_nBroadCastCursor = 0;\n",
  "\tm_nBroadCastCursor = 0;\n\tm_dwLucCoNguoiVao = 0;\t// [DELTA 07/09]\n")
E(KREG_C, "\t\t\tm_PlayerList.AddTail(&Player[nIdx].m_Node);\n\t\t\tPlayer[nIdx].m_Node.AddRef();\n",
  "\t\t\tm_PlayerList.AddTail(&Player[nIdx].m_Node);\n\t\t\tPlayer[nIdx].m_Node.AddRef();\n"
  "\t\t\tm_dwLucCoNguoiVao = GetTickCount();\t// [DELTA 07/09] NormalSync phat bat ke bam trong ky lam moi de nguoi moi biet NPC\n")
# Activate: chunk theo config + dem theo lan phat that
E(KREG_C, "\tconst int kNpcSyncChunkSize = 5;  // Number of NPCs to sync per frame\n",
  "\t// [DELTA 07/09] so NPC dong bo moi khung doc tu config.ini [Server] DongBoMoiTick (mac dinh 10, kep 1..40; truoc 5).\n"
  "\t// Dem theo so lan PHAT THAT (NormalSync tra ve TRUE): NPC dung yen khong doi bi bo qua khong ton suat, nen NPC dang\n"
  "\t// di chuyen duoc nan day hon voi cung so goi. Moi khung xet toi da 4 x DongBoMoiTick NPC ke tu con tro (chan chi phi bam).\n"
  "\tstatic int s_nNSChunk = -1;\n"
  "\tif (s_nNSChunk < 0)\n"
  "\t{\n"
  "\t\ts_nNSChunk = (int)GetPrivateProfileIntA(\"Server\", \"DongBoMoiTick\", 10, \".\\\\config.ini\");\n"
  "\t\tif (s_nNSChunk < 1) s_nNSChunk = 1;\n"
  "\t\tif (s_nNSChunk > 40) s_nNSChunk = 40;\n"
  "\t}\n"
  "\tconst int kNpcSyncChunkSize = s_nNSChunk;  // Number of NPCs to sync per frame\n")
E(KREG_C, "\tint aSyncIdx[kNpcSyncChunkSize];\n\tfor (int k = 0; k < kNpcSyncChunkSize; k++)\n\t\taSyncIdx[k] = (npcCount > 0) ? ((m_nNpcSyncCursor + k) % npcCount) : -1;\n",
  "\t// [DELTA 07/09] cua so lien tuc theo thu tu danh sach tu con tro: khong con quan vong (khong bo sot NPC quan).\n"
  "\tconst int nNSBatDau = (npcCount > 0) ? (m_nNpcSyncCursor % npcCount) : 0;\n"
  "\tconst int nNSCuaSo = kNpcSyncChunkSize * 4;\n"
  "\tint nNSXetToi = -1;\t// chi so lon nhat da xet trong khung nay (de day con tro)\n")
E(KREG_C, "\t\t\tif (synced < kNpcSyncChunkSize && currentIndex == aSyncIdx[synced])\n\t\t\t{\n\t\t\t\tNpc[nNpcIdx].NormalSync();\n\t\t\t\tsynced++;\n\t\t\t}\n",
  "\t\t\tif (synced < kNpcSyncChunkSize && currentIndex >= nNSBatDau && (currentIndex - nNSBatDau) < nNSCuaSo)\n"
  "\t\t\t{\n"
  "\t\t\t\tif (Npc[nNpcIdx].NormalSync())\t// [DELTA 07/09] TRUE = co phat (goi gon hoac day du)\n"
  "\t\t\t\t\tsynced++;\n"
  "\t\t\t\tnNSXetToi = currentIndex;\n"
  "\t\t\t}\n")
E(KREG_C, "\tm_nNpcSyncCursor = (m_nNpcSyncCursor + kNpcSyncChunkSize) % (npcCount > 0 ? npcCount : 1);\n",
  "#ifdef _SERVER\n"
  "\t// [DELTA 07/09] day con tro qua cac NPC da xet; het danh sach thi quay ve dau (lap sau).\n"
  "\tm_nNpcSyncCursor = (nNSXetToi >= 0 && nNSXetToi + 1 < npcCount) ? (nNSXetToi + 1) : 0;\n"
  "#else\n"
  "\tm_nNpcSyncCursor = 0;\n"
  "#endif\n")
# [BC-LOAI]: in 8 loai NHIEU NHAT (truoc: 8 ma nho nhat -> giau chien dau)
E(KREG_C, "\t\t\tchar szBC[512]; int nBCLen = 0; int nBCIn = 0;\n"
  "\t\t\tfor (int nBCK = 0; nBCK < 256 && nBCLen < 440; nBCK++)\n"
  "\t\t\t{\n"
  "\t\t\t\tif (s_anBCGoi[nBCK] <= 0)\n"
  "\t\t\t\t\tcontinue;\n"
  "\t\t\t\tif (nBCIn >= 8 && s_anBCBo[nBCK] <= 0)\n"
  "\t\t\t\t\tcontinue;\n"
  "\t\t\t\tnBCLen += _snprintf(szBC + nBCLen, sizeof(szBC) - 1 - nBCLen, \" %d:%d/%d/%d\", nBCK, s_anBCGoi[nBCK], s_anBCGui[nBCK], s_anBCBo[nBCK]);\n"
  "\t\t\t\tnBCIn++;\n"
  "\t\t\t}\n",
  "\t\t\t// [DELTA 07/09] in 8 loai NHIEU NHAT theo so luot phat (ban cu in 8 ma NHO NHAT -> giau 91/95/148/207) + loai bi cat\n"
  "\t\t\tchar szBC[512]; int nBCLen = 0;\n"
  "\t\t\tint anBCDaIn[256]; memset(anBCDaIn, 0, sizeof(anBCDaIn));\n"
  "\t\t\tfor (int nBCLan = 0; nBCLan < 8 && nBCLen < 440; nBCLan++)\n"
  "\t\t\t{\n"
  "\t\t\t\tint nBCK = -1, nBCMax = 0;\n"
  "\t\t\t\tfor (int k = 0; k < 256; k++)\n"
  "\t\t\t\t\tif (!anBCDaIn[k] && s_anBCGoi[k] > nBCMax) { nBCMax = s_anBCGoi[k]; nBCK = k; }\n"
  "\t\t\t\tif (nBCK < 0)\n"
  "\t\t\t\t\tbreak;\n"
  "\t\t\t\tanBCDaIn[nBCK] = 1;\n"
  "\t\t\t\tnBCLen += _snprintf(szBC + nBCLen, sizeof(szBC) - 1 - nBCLen, \" %d:%d/%d/%d\", nBCK, s_anBCGoi[nBCK], s_anBCGui[nBCK], s_anBCBo[nBCK]);\n"
  "\t\t\t}\n"
  "\t\t\tfor (int nBCK = 0; nBCK < 256 && nBCLen < 440; nBCK++)\n"
  "\t\t\t\tif (!anBCDaIn[nBCK] && s_anBCBo[nBCK] > 0)\n"
  "\t\t\t\t\tnBCLen += _snprintf(szBC + nBCLen, sizeof(szBC) - 1 - nBCLen, \" %d:%d/%d/%d\", nBCK, s_anBCGoi[nBCK], s_anBCGui[nBCK], s_anBCBo[nBCK]);\n")

# ---------------------------------------------------------------- 4. KNpc.h
E(KNPC_H, "\tvoid\t\t\t\tNormalSync();",
  "\tBOOL\t\t\t\tNormalSync();\t// [DELTA 07/09] TRUE = co phat goi dong bo (gon hoac day du)\n"
  "\tvoid\t\t\t\tNS_DungGoi(void* pOut, int nMpsX, int nMpsY);\t// [DELTA 07/09] dung NPC_NORMAL_SYNC (NormalSync + SendSyncData)")

# ---------------------------------------------------------------- 5. KNpc.cpp
NS_BLOCK = (
  "// [DELTA 07/09] (chu game 07/09: ban test, khong gioi han trai nghiem) DONG BO THEO THAY DOI cho goi 77:\n"
  "//  - bam toan goi NPC_NORMAL_SYNC; giong lan phat truoc, chua toi ky lam moi va khong co nguoi vua vao 9 vung quanh\n"
  "//    -> KHONG phat (NPC dung yen khong ton bang thong; ban Linux tham chieu cung chi phat 1 NPC/vung/2 tick).\n"
  "//  - bam nhom CHAM (bo toa do, Doing, State, mau/noi luc hien tai) giong lan phat DAY DU truoc va chua toi ky lam moi\n"
  "//    day du -> phat goi GON s2c_syncnpcpos 25 byte (Linux: 27 byte) thay cho 99 byte.\n"
  "//  - Client vua biet NPC (RequestNpc -> SendSyncData) duoc day RIENG mot goi 77 day du, nen goi gon khong lam thieu\n"
  "//    trang thai cham. Khoa config.ini [Server]: DongBoLamMoi (ms, mac dinh 2000; 0 = luon phat nhu cu),\n"
  "//    DongBoLamMoiDay (ms, mac dinh 10000), DongBoGoiGon (1 = dung goi gon, 0 = luon goi day du).\n"
  "static DWORD s_adwNSBamTat[MAX_NPC];\t// bam toan goi lan phat truoc\n"
  "static DWORD s_adwNSBamCham[MAX_NPC];\t// bam nhom cham lan phat DAY DU truoc\n"
  "static DWORD s_adwNSLucGui[MAX_NPC];\t// GetTickCount lan phat truoc (gon hoac day)\n"
  "static DWORD s_adwNSLucDay[MAX_NPC];\t// GetTickCount lan phat DAY DU truoc\n"
  "static int   s_nNSLamMoi = -1, s_nNSLamMoiDay = 10000, s_nNSGon = 1;\n"
  "static int   s_nNSBo = 0, s_nNSGonDem = 0, s_nNSDay = 0;\n"
  "static void NS_DocCauHinh()\n"
  "{\n"
  "\tif (s_nNSLamMoi >= 0)\n"
  "\t\treturn;\n"
  "\ts_nNSLamMoi = (int)GetPrivateProfileIntA(\"Server\", \"DongBoLamMoi\", 2000, \".\\\\config.ini\");\n"
  "\tif (s_nNSLamMoi < 0) s_nNSLamMoi = 0;\n"
  "\tif (s_nNSLamMoi > 10000) s_nNSLamMoi = 10000;\n"
  "\ts_nNSLamMoiDay = (int)GetPrivateProfileIntA(\"Server\", \"DongBoLamMoiDay\", 10000, \".\\\\config.ini\");\n"
  "\tif (s_nNSLamMoiDay < 1000) s_nNSLamMoiDay = 1000;\n"
  "\tif (s_nNSLamMoiDay > 60000) s_nNSLamMoiDay = 60000;\n"
  "\ts_nNSGon = (int)GetPrivateProfileIntA(\"Server\", \"DongBoGoiGon\", 1, \".\\\\config.ini\");\n"
  "}\n"
  "// co nguoi vua vao vung nay hoac 8 vung ke trong ky lam moi -> phai phat de ho biet NPC (client chi biet NPC la qua goi 77)\n"
  "static BOOL NS_CoNguoiVuaVao(const KNpc* pNpc, DWORD dwLuc)\n"
  "{\n"
  "\tif (pNpc->m_SubWorldIndex < 0 || pNpc->m_SubWorldIndex >= MAX_SUBWORLD || pNpc->m_RegionIndex < 0)\n"
  "\t\treturn FALSE;\n"
  "\tconst KRegion& rV = SubWorld[pNpc->m_SubWorldIndex].m_Region[pNpc->m_RegionIndex];\n"
  "\tif (rV.m_dwLucCoNguoiVao && (dwLuc - rV.m_dwLucCoNguoiVao) < (DWORD)s_nNSLamMoi)\n"
  "\t\treturn TRUE;\n"
  "\tfor (int i = 0; i < 8; i++)\n"
  "\t{\n"
  "\t\tconst int nKe = rV.m_nConnectRegion[i];\n"
  "\t\tif (nKe < 0)\n"
  "\t\t\tcontinue;\n"
  "\t\tconst KRegion& rK = SubWorld[pNpc->m_SubWorldIndex].m_Region[nKe];\n"
  "\t\tif (rK.m_dwLucCoNguoiVao && (dwLuc - rK.m_dwLucCoNguoiVao) < (DWORD)s_nNSLamMoi)\n"
  "\t\t\treturn TRUE;\n"
  "\t}\n"
  "\treturn FALSE;\n"
  "}\n")
E(KNPC_C, "void PS_XoaDauVet(int nNpcIdx)\n{\n\tif (nNpcIdx > 0 && nNpcIdx < MAX_NPC)\n\t\ts_adwPSBam[nNpcIdx] = 0;\n}\n#endif\n",
  "void PS_XoaDauVet(int nNpcIdx)\n{\n\tif (nNpcIdx > 0 && nNpcIdx < MAX_NPC)\n\t\ts_adwPSBam[nNpcIdx] = 0;\n}\n#endif\n" + NS_BLOCK)

# SendSyncData: day rieng goi 77 day du ngay sau NPC_SYNC
E(KNPC_C, "\t\tprintf(\"Packing sync data failed...\\n\");\n\t\treturn FALSE;\n\t}\n",
  "\t\tprintf(\"Packing sync data failed...\\n\");\n\t\treturn FALSE;\n\t}\n"
  "\t// [DELTA 07/09] client vua biet NPC nay -> day RIENG goi 77 DAY DU (goi 77 phat tan tu nay thuong la ban GON).\n"
  "\tif (m_RegionIndex >= 0 && m_SubWorldIndex >= 0)\n"
  "\t{\n"
  "\t\tNPC_NORMAL_SYNC sNSDay;\n"
  "\t\tint nNSX = 0, nNSY = 0;\n"
  "\t\tGetMpsPos(&nNSX, &nNSY);\n"
  "\t\tNS_DungGoi(&sNSDay, nNSX, nNSY);\n"
  "\t\tg_pServer->PackDataToClient(nClient, (BYTE*)&sNSDay, sizeof(sNSDay));\n"
  "\t}\n")

# NormalSync -> NS_DungGoi (giu nguyen dong header co chu Viet: chi doi ten ham) + NormalSync moi
E(KNPC_C, "void KNpc::NormalSync() //Sync npc min li",
  "// [DELTA 07/09] Phan DUNG goi tach ra de SendSyncData cung dung duoc (day goi 77 day du khi client vua biet NPC).\n"
  "void KNpc::NS_DungGoi(void* pOut, int nMpsX, int nMpsY) //Sync npc min li")
E(KNPC_C, "{\n\textern int g_nBCNormalSync;\t// [BC 04/09 do] dem so lan phat dong bo (KRegion.cpp)\n\tg_nBCNormalSync++;\n\n"
  "\tif (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)\n\t\treturn;\n\n"
  "\tNPC_NORMAL_SYNC NpcSync;\n\tint\tnMpsX, nMpsY;\n\n\tGetMpsPos(&nMpsX, &nMpsY);\n\n",
  "{\n\tNPC_NORMAL_SYNC& NpcSync = *(NPC_NORMAL_SYNC*)pOut;\n\n")
E(KNPC_C, "\tstatic const POINT\tPOff[8] = \t//MAX_PLAYER\n",
  "}\n\n"
  "BOOL KNpc::NormalSync()\n"
  "{\n"
  "\textern int g_nBCNormalSync;\t// [BC 04/09 do] dem so lan phat dong bo (KRegion.cpp)\n"
  "\tg_nBCNormalSync++;\n\n"
  "\tif (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)\n"
  "\t\treturn FALSE;\n\n"
  "\tNPC_NORMAL_SYNC NpcSync;\n"
  "\tint\tnMpsX, nMpsY;\n\n"
  "\tGetMpsPos(&nMpsX, &nMpsY);\n"
  "\tNS_DungGoi(&NpcSync, nMpsX, nMpsY);\n\n"
  "\t// [DELTA 07/09] quyet dinh: bo qua / goi GON / goi DAY DU (xem chu thich tren cac bien s_adwNS*).\n"
  "\tBOOL bPhat = TRUE, bGon = FALSE;\n"
  "\tNPC_POS_SYNC sGon;\n"
  "\t{\n"
  "\t\tNS_DocCauHinh();\n"
  "\t\tconst DWORD dwLuc = GetTickCount();\n"
  "\t\tif (m_Index > 0 && m_Index < MAX_NPC && s_nNSLamMoi > 0)\n"
  "\t\t{\n"
  "\t\t\tconst DWORD dwBamTat = PS_Bam(&NpcSync, (int)sizeof(NpcSync));\n"
  "\t\t\tNPC_NORMAL_SYNC sCham = NpcSync;\n"
  "\t\t\tsCham.MapX = 0; sCham.MapY = 0; sCham.m_fkRegionID = 0; sCham.m_fkOffX = 0; sCham.m_fkOffY = 0;\n"
  "\t\t\tsCham.Doing = 0; sCham.State = 0; sCham.m_CurrentLife = 0; sCham.m_CurrentMana = 0;\n"
  "\t\t\tconst DWORD dwBamCham = PS_Bam(&sCham, (int)sizeof(sCham));\n"
  "\t\t\tconst BOOL bNguoiMoi = NS_CoNguoiVuaVao(this, dwLuc);\n"
  "\t\t\tif (dwBamTat == s_adwNSBamTat[m_Index] && !bNguoiMoi && (dwLuc - s_adwNSLucGui[m_Index]) < (DWORD)s_nNSLamMoi)\n"
  "\t\t\t\tbPhat = FALSE;\n"
  "\t\t\telse if (s_nNSGon && dwBamCham == s_adwNSBamCham[m_Index] && (dwLuc - s_adwNSLucDay[m_Index]) < (DWORD)s_nNSLamMoiDay)\n"
  "\t\t\t\tbGon = TRUE;\n"
  "\t\t\tif (bPhat)\n"
  "\t\t\t{\n"
  "\t\t\t\ts_adwNSBamTat[m_Index] = dwBamTat;\n"
  "\t\t\t\ts_adwNSLucGui[m_Index] = dwLuc;\n"
  "\t\t\t\tif (!bGon)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_adwNSBamCham[m_Index] = dwBamCham;\n"
  "\t\t\t\t\ts_adwNSLucDay[m_Index] = dwLuc;\n"
  "\t\t\t\t}\n"
  "\t\t\t}\n"
  "\t\t}\n"
  "\t\tif (bGon)\n"
  "\t\t{\n"
  "\t\t\tsGon.ProtocolType = (BYTE)s2c_syncnpcpos;\n"
  "\t\t\tsGon.ID = m_dwID;\n"
  "\t\t\tsGon.MapX = nMpsX;\n"
  "\t\t\tsGon.MapY = nMpsY;\n"
  "\t\t\tsGon.Doing = NpcSync.Doing;\n"
  "\t\t\tsGon.State = NpcSync.State;\n"
  "\t\t\tsGon.Camp = NpcSync.Camp;\n"
  "\t\t\tsGon.m_bySeries = NpcSync.m_bySeries;\n"
  "\t\t\tsGon.m_CurrentLife = NpcSync.m_CurrentLife;\n"
  "\t\t\tsGon.m_CurrentMana = NpcSync.m_CurrentMana;\n"
  "\t\t}\n"
  "\t\tif (!bPhat) s_nNSBo++; else if (bGon) s_nNSGonDem++; else s_nNSDay++;\n"
  "\t\tAUTOLOG_EVERY(10000, \"[NS-BO] dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d)\",\n"
  "\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon);\n"
  "\t}\n"
  "\tBOOL bNSKq = bPhat;\n"
  "\tstatic const POINT\tPOff[8] = \t//MAX_PLAYER\n")
E(KNPC_C, "\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;\n"
  "\tCURREGION.BroadCast(&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);\n"
  "\tint j;\n"
  "\tfor (j = 0; j < 8; j++)\n"
  "\t{\n"
  "\t\tint nConRegion = CURREGION.m_nConnectRegion[j];\n"
  "\t\tif (nConRegion == -1)\n"
  "\t\t\tcontinue;\n"
  "\t\t_ASSERT(m_SubWorldIndex >= 0 && nConRegion >= 0);\n"
  "\t\tSubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);\n"
  "\t}\n",
  "\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;\n"
  "\t// [DELTA 07/09] phat goi GON hoac DAY DU, hoac khong phat (NPC khong doi)\n"
  "\tconst void* pNSBuf = bGon ? (const void*)&sGon : (const void*)&NpcSync;\n"
  "\tconst DWORD dwNSSize = bGon ? (DWORD)sizeof(NPC_POS_SYNC) : (DWORD)sizeof(NPC_NORMAL_SYNC);\n"
  "\tint j;\n"
  "\tif (bPhat)\n"
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
  "\t}\n")
E(KNPC_C, "\t\tif (bPSGui)\n\t\t{\n\t\t\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;\n",
  "\t\tif (bPSGui)\n\t\t{\n\t\t\tbNSKq = TRUE;\t// [DELTA 07/09]\n\t\t\tint nMaxCount = NPC_SYNC_BROADCAST_LIMIT;\n")
E(KNPC_C, "\t\tif (!m_btSimCityBot && m_nPlayerIdx > 0)\n\t\t\tg_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));\n"
  "\t\t//------------------------------------------------------End SYNC 3-------------------------------\n\t}\n}\n",
  "\t\tif (bPhat && !m_btSimCityBot && m_nPlayerIdx > 0)\t// [DELTA 07/09] chi khi co phat dong bo\n"
  "\t\t\tg_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));\n"
  "\t\t//------------------------------------------------------End SYNC 3-------------------------------\n\t}\n"
  "\treturn bNSKq;\n}\n")
# 75: lam moi 5 -> 30 giay (SendSyncData xoa dau vet cho nguoi moi thay nen an toan)
E(KNPC_C, "GetPrivateProfileIntA(\"Server\", \"BroadCastLamMoi\", 5, \".\\\\config.ini\");",
  "GetPrivateProfileIntA(\"Server\", \"BroadCastLamMoi\", 30, \".\\\\config.ini\");\t// [DELTA 07/09] 5 -> 30 s")

# ---------------------------------------------------------------- 6. GameServer: xa moi tick
E(KSO_C, "\tif (m_nGameLoop & 0x01)\n\t{\n\t\tm_pServer->SendPackToClient(-1);\n\t}\n",
  "\t// [DELTA 07/09] xa socket MOI tick (truoc: moi 2 tick '& 0x01' -> moi goi may chu->client cho 0-111 ms trong dem).\n"
  "\t// 18 lan WSASend/giay/client thay vi 9; do [PERF] MainLoop truoc sau.\n"
  "\tm_pServer->SendPackToClient(-1);\n")

# ---------------------------------------------------------------- 7. heaven.dll: TCP_NODELAY khi accept
E(SOCK_C, "\t\t\t\t\t\tSocket *pSocket = AllocateSocket( acceptedSocket );\n",
  "\t\t\t\t\t\t// [DELTA 07/09] tat Nagle tren socket vua nhan: goi nho (13-21 byte) di ngay, khong doi ACK/200 ms.\n"
  "\t\t\t\t\t\t{\n"
  "\t\t\t\t\t\t\tBOOL bNoDelay = TRUE;\n"
  "\t\t\t\t\t\t\t::setsockopt( acceptedSocket, IPPROTO_TCP, TCP_NODELAY, ( const char * )&bNoDelay, sizeof( bNoDelay ) );\n"
  "\t\t\t\t\t\t}\n"
  "\t\t\t\t\t\tSocket *pSocket = AllocateSocket( acceptedSocket );\n")

# ================================================================= apply
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)

files = {}
for f, old, new, cnt in edits:
    files.setdefault(f, rd(f))
for f, anc, txt, where in inserts:
    files.setdefault(f, rd(f))
before = {f: hb(s) for f, s in files.items()}

# moi tep deu CRLF (da kiem): doi "\n" trong neo/van ban moi thanh "\r\n"
edits = [(f, old.replace("\n", "\r\n"), new.replace("\n", "\r\n"), cnt) for f, old, new, cnt in edits]
inserts = [(f, anc, txt.replace("\n", "\r\n"), where) for f, anc, txt, where in inserts]

for f, old, new, cnt in edits:
    s = files[f]
    n = s.count(old)
    if n != cnt:
        print("FAIL edit %s: anchor found %d times (expect %d):\n%s" % (f, n, cnt, old[:120].encode("ascii", "replace").decode()))
        sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new):
        print("FAIL non-ASCII in edit for", f); sys.exit(1)
    files[f] = s.replace(old, new)
for f, anc, txt, where in inserts:
    s = files[f]
    lines = s.split("\n")
    hits = [i for i, l in enumerate(lines) if anc in l]
    if len(hits) != 1:
        print("FAIL insert %s: anchor found %d times: %s" % (f, len(hits), anc)); sys.exit(1)
    if any(ord(c) >= 0x80 for c in txt): print("FAIL non-ASCII insert", f); sys.exit(1)
    i = hits[0]
    if where == "after":
        lines[i] = lines[i] + "\n" + txt.rstrip("\n")      # lines[i] ket thuc bang "\r"; txt con "\r" cuoi
    else:
        lines[i] = txt.rstrip("\n") + "\n" + lines[i]
    files[f] = "\n".join(lines)

for f, s in files.items():
    if hb(s) != before[f]:
        print("FAIL high-byte count changed in", f, before[f], "->", hb(s)); sys.exit(1)
for f, s in files.items():
    wr(f, s)
    print("OK", f.split("\\")[-1], "high-bytes", before[f])
print("ALL APPLIED")
