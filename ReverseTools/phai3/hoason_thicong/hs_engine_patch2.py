# -*- coding: utf-8 -*-
"""hs_engine_patch2.py [HOASON 01/09 b]: port 4 nhom thuoc tinh ky nang Hoa Son CHUA CO ma xu ly trong JX1, theo dich nguoc jx_linux_y:
  - meleedamagereturnmana_p / rangedamagereturnmana_p : handler 0x080963D0/0x080963F0 (+= nValue[0] vao [0x137c]/[0x1380] cua NAN NHAN),
        dung o ham sat thuong 0x08089C90 (0x08089F19-0x08089F5F): mana(ke danh) += dmg * p / 100 (cat le), am -> 0 (0x0808A390)
  - addblockrate : handler 0x08096430 ([0x1388] += v0, [0x138c] += v2); 0x0808C078: neu v0>0 && v2>0 -> [0x1390] = min(25, Random(256)/v0*v2);
        0x0808B2C0: nBlock = [0x1390] + block_rate[0x1408] - anti_block_rate(ke danh)
  - reduceskillcd1/2 : handler 0x08097250 (this, pLauncher, pNpc, attr): KSkillList(launcher)::ReduceCD 0x080E4740 (NextCastTime > f -> -= f;
        f < WaitCastTime -> -= f) + gui goi 0xdd cho client
  - candetonate1/2/3 : handler 0x08097110 -> 0x08079870 (launcher, style=v0>>8, radius=v2, flag=v0&0xff, region + 8 vung ke):
        dan m_nMissleId == style, quan he chu dan (flag 0: self|ally, 1: enemy), khoang cach mps <= radius -> 0x08075210 = DoVanish
Sua latin-1, neo duy nhat, idempotent theo marker.  --kiem = chi kiem.
"""
import io, os, re, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
KIEM = "--kiem" in sys.argv
SRC = r"D:\GAMEDEVNEW\Sources"
MARK = "[HOASON 01/09b]"
LF = chr(10); CR = chr(13); CRLF = CR + LF

class Tep:
    def __init__(self, rel):
        self.p = os.path.join(SRC, rel)
        self.d = io.open(self.p, "r", encoding="latin-1", newline="").read()
        self.nl = CRLF if CRLF in self.d else LF
        self.n = 0; self.rel = rel
    def nx(self, s):
        return s.replace(CRLF, LF).replace(LF, CRLF) if self.nl == CRLF else s
    def da(self, m): return m in self.d
    def co(self, s): return self.nx(s) in self.d
    def thay(self, old, new, so=1):
        old = self.nx(old); new = self.nx(new)
        c = self.d.count(old)
        assert c == so, "%s: neo %d lan (can %d): %r" % (self.rel, c, so, old[:100])
        self.d = self.d.replace(old, new); self.n += 1
    def ghi(self):
        if KIEM: print("  (kiem) %s: %d cho" % (self.rel, self.n)); return
        io.open(self.p, "w", encoding="latin-1", newline="").write(self.d)
        print("  ghi %s: %d cho" % (self.rel, self.n))

# ---------------------------------------------------------------- giao thuc s2c_reduceskillcd
t = Tep(os.path.join("..", "Headers", "KProtocolDef.h"))
if not t.da(MARK):
    old = "\ts2c_syncfusion,\t\t\t// [DUNGLUYEN 01/09] dong bo 6 o Van Cuong + seed theo dwID\n"
    t.thay(old, old + "\ts2c_reduceskillcd,\t\t// %s Linux goi 0xdd: giam hoi chieu ky nang (reduceskillcd1/2 Hoa Son)\n" % MARK)
    t.ghi()
t = Tep(r"Core\Src\KProtocol.cpp")
if not t.da(MARK):
    old = "\tsizeof(ITEM_SYNC_FUSION),\t\t\t// s2c_syncfusion [DUNGLUYEN 01/09]\n"
    t.thay(old, old + "\tsizeof(S2C_REDUCE_SKILL_CD),\t\t// s2c_reduceskillcd %s\n" % MARK)
    t.ghi()
t = Tep(r"Core\Src\KProtocol.h")
if not t.da(MARK):
    m = re.search(r"\} PLAYER_SKILL_LEVEL_SYNC;[^\r\n]*\r?\n", t.d)
    assert m, "KProtocol.h: khong thay PLAYER_SKILL_LEVEL_SYNC"
    add = ("\n// %s Linux handler reduceskillcd (0x08097250) gui goi 0xdd (type, v0, v1, v2) toi client cua NGUOI PHAT\n"
           "// de client giam NextCastTime/WaitCastTime cua ky nang v0 di v2 khung (KSkillList::ReduceCoolDown).\n"
           "typedef struct\n{\n\tBYTE\tProtocolType;\t\t// s2c_reduceskillcd\n\tWORD\tm_wSkillId;\n\tWORD\tm_wFrames;\n} S2C_REDUCE_SKILL_CD;\n" % MARK)
    t.d = t.d[:m.end()] + t.nx(add) + t.d[m.end():]; t.n += 1
    t.ghi()
t = Tep(r"Core\Src\KProtocolProcess.h")
if not t.da(MARK):
    old = "\tvoid\ts2cSyncItemFusion(BYTE* pMsg);\t// [DUNGLUYEN 01/09]\n"
    t.thay(old, old + "\tvoid\ts2cReduceSkillCD(BYTE* pMsg);\t// %s\n" % MARK)
    t.ghi()
t = Tep(r"Core\Src\KProtocolProcess.cpp")
if not t.da(MARK):
    old = "\tProcessFunc[s2c_syncfusion] = &KProtocolProcess::s2cSyncItemFusion;\t// [DUNGLUYEN 01/09]\n"
    t.thay(old, old + "\tProcessFunc[s2c_reduceskillcd] = &KProtocolProcess::s2cReduceSkillCD;\t// %s\n" % MARK)
    old = "\t}\n}\n#endif\n\nvoid KProtocolProcess::s2cOpenQuestFinishDlg(BYTE* pMsg)\n"
    new = ("\t}\n}\n\n// %s Linux 0xdd (tu handler reduceskillcd1/2): giam hoi chieu ky nang cua chinh minh tren client\n"
           "void KProtocolProcess::s2cReduceSkillCD(BYTE* pMsg)\n{\n"
           "\tS2C_REDUCE_SKILL_CD* pSync = (S2C_REDUCE_SKILL_CD*)pMsg;\n"
           "\tif (pSync->ProtocolType != s2c_reduceskillcd)\n\t\treturn;\n"
           "\tint nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;\n"
           "\tif (nNpc > 0 && nNpc < MAX_NPC)\n\t\tNpc[nNpc].m_SkillList.ReduceCoolDown((int)pSync->m_wSkillId, (int)pSync->m_wFrames);\n}\n"
           "#endif\n\nvoid KProtocolProcess::s2cOpenQuestFinishDlg(BYTE* pMsg)\n" % MARK)
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KSkillList::ReduceCoolDown
t = Tep(r"Core\Src\KSkillList.h")
if not t.da(MARK):
    old = "\tvoid\t\tSetNextCastTime(int nSkillID, DWORD dwCurrentTime, DWORD dwNextCastTime);\n"
    t.thay(old, old + "\tvoid\t\tReduceCoolDown(int nSkillID, int nFrames);\t// %s Linux 0x080E4740\n" % MARK)
    t.ghi()
t = Tep(r"Core\Src\KSkillList.cpp")
if not t.da(MARK):
    old = "#endif\n}\t\n\n#ifndef _SERVER\n#define MAX_FIGHTSKILL_SORTLIST 50\n"
    new = ("#endif\n}\t\n\n"
           "// %s Linux KSkillList 0x080E4740 (goi tu handler reduceskillcd1/2 0x08097250 voi (skillId=nValue[0], frames=nValue[2])):\n"
           "// tim o co SkillId; NextCastTime > frames -> tru frames; frames < WaitCastTime -> tru frames (so sanh KHONG dau).\n"
           "// Khong kiem 'dang hoi chieu hay khong' - dung nhu Linux (tru mot moc da qua la vo hai).\n"
           "void KSkillList::ReduceCoolDown(int nSkillID, int nFrames)\n{\n"
           "\tif (nSkillID <= 0 || nFrames <= 0)\n\t\treturn;\n"
           "\tint i = FindSame(nSkillID);\n\tif (!i)\n\t\treturn;\n"
           "\tif (m_Skills[i].NextCastTime > (DWORD)nFrames)\n\t\tm_Skills[i].NextCastTime -= (DWORD)nFrames;\n"
           "\tif ((DWORD)nFrames < (DWORD)m_Skills[i].WaitCastTime)\n\t\tm_Skills[i].WaitCastTime -= nFrames;\n"
           "}\n\n#ifndef _SERVER\n#define MAX_FIGHTSKILL_SORTLIST 50\n" % MARK)
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KMissle::Detonate
t = Tep(r"Core\Src\KMissle.h")
if not t.da(MARK):
    old = "\tvoid\t\t\t\tGetMpsPos(int *pPosX, int *pPosY);\n"
    t.thay(old, old + "\tvoid\t\t\t\tDetonate() { DoVanish(); }\t// %s kich no (Linux 0x08075210: su kien tan + GWM_MISSLE_DEL + trang thai tan)\n" % MARK)
    t.ghi()

# ---------------------------------------------------------------- KNpc.h
t = Tep(r"Core\Src\KNpc.h")
if not t.da(MARK):
    old = "\tint\t\t\t\t\tm_nKMHitPercent;\t\t\t\t\t\t\t\t\t//#he so don danh luot nay\n"
    new = old + ("\t// %s Linux KNpc [0x137c] meleedamagereturnmana_p, [0x1380] rangedamagereturnmana_p (NAN NHAN giu, ke danh +mana),\n"
                 "\t// [0x1388]/[0x138c] addblockrate v0/v2 -> phan tram do ngau nhien min(25, Random(256)/v0*v2) cong vao hoa giai\n"
                 "\tint\t\t\t\t\tm_CurrentMeleeDamageReturnManaP;\n\tint\t\t\t\t\tm_CurrentRangeDamageReturnManaP;\n"
                 "\tint\t\t\t\t\tm_CurrentAddBlockRateV0;\n\tint\t\t\t\t\tm_CurrentAddBlockRateV2;\n" % MARK)
    t.thay(old, new)
    old = "\tvoid\t\t\t\tGetMpsPos(int * pPosX, int *pPosY);\n"
    t.thay(old, old + "\tint\t\t\t\t\tDetonateMissles(int nStyle, int nRadius, int nFlag);\t// %s Linux 0x08079870 (candetonate1-3)\n" % MARK)
    t.ghi()

# ---------------------------------------------------------------- KNpcAttribModify.h / .cpp
t = Tep(r"Core\Src\KNpcAttribModify.h")
if not t.da(MARK):
    old = "\tvoid\tAntiBlockRate(KNpc* pNpc, void* pData);\n"
    t.thay(old, old + ("\tvoid\tMeleeDamageReturnManaP(KNpc* pNpc, void* pData);\t// %s\n"
                       "\tvoid\tRangeDamageReturnManaP(KNpc* pNpc, void* pData);\n"
                       "\tvoid\tAddBlockRate(KNpc* pNpc, void* pData);\n" % MARK))
    t.ghi()
t = Tep(r"Core\Src\KNpcAttribModify.cpp")
if not t.da(MARK):
    old = "\tProcessFunc[magic_anti_block_rate] = &KNpcAttribModify::AntiBlockRate;\n"
    t.thay(old, old + ("\tProcessFunc[magic_meleedamagereturnmana_p] = &KNpcAttribModify::MeleeDamageReturnManaP;\t// %s\n"
                       "\tProcessFunc[magic_rangedamagereturnmana_p] = &KNpcAttribModify::RangeDamageReturnManaP;\n"
                       "\tProcessFunc[magic_addblockrate] = &KNpcAttribModify::AddBlockRate;\n" % MARK))
    old = "void KNpcAttribModify::EnhanceHitRate( KNpc* pNpc, void* pData )\t\t//#trong kich\n"
    new = ("// %s Linux 0x080963D0 / 0x080963F0: [0x137c] / [0x1380] += nValue[0] (tren NAN NHAN); ap dung o KNpc::CalcDamage\n"
           "void KNpcAttribModify::MeleeDamageReturnManaP( KNpc* pNpc, void* pData )\n{\n"
           "\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\n\tpNpc->m_CurrentMeleeDamageReturnManaP += pMagic->nValue[0];\n}\n\n"
           "void KNpcAttribModify::RangeDamageReturnManaP( KNpc* pNpc, void* pData )\n{\n"
           "\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\n\tpNpc->m_CurrentRangeDamageReturnManaP += pMagic->nValue[0];\n}\n\n"
           "// Linux 0x08096430: [0x1388] += nValue[0], [0x138c] += nValue[2]; phan tram do ngau nhien tinh o KNpc::ReceiveDamage (Linux 0x0808C078)\n"
           "void KNpcAttribModify::AddBlockRate( KNpc* pNpc, void* pData )\n{\n"
           "\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\n\tpNpc->m_CurrentAddBlockRateV0 += pMagic->nValue[0];\n\tpNpc->m_CurrentAddBlockRateV2 += pMagic->nValue[2];\n}\n\n" % MARK) + old
    t.thay(old, new)
    t.ghi()

# ---------------------------------------------------------------- KNpc.cpp
t = Tep(r"Core\Src\KNpc.cpp")
if not t.da(MARK):
    old = "\tm_CurrentBlockRate = 0; m_CurrentAntiBlockRate = 0;\t\t\t\t\t// [KM 27/08]\n"
    t.thay(old, old + "\tm_CurrentMeleeDamageReturnManaP = 0; m_CurrentRangeDamageReturnManaP = 0; m_CurrentAddBlockRateV0 = 0; m_CurrentAddBlockRateV2 = 0;\t// %s\n" % MARK, 2)
    old = "\t\t\tint nKMBlock = m_CurrentBlockRate - Npc[nLauncher].m_CurrentAntiBlockRate;\n"
    new = ("\t\t\t// %s addblockrate (Hao Nhien Chi Khi 1370) - Linux 0x0808C078: neu v0>0 && v2>0 thi [0x1390] = min(25, Random(256)/v0*v2)\n"
           "\t\t\t// (Linux tinh moi lan tinh lai thuoc tinh; JX1 tung moi lan bi danh - cung phan bo), 0x0808B2C0 cong [0x1390] vao block_rate.\n"
           "\t\t\tint nKMAddBlock = 0;\n"
           "\t\t\tif (m_CurrentAddBlockRateV0 > 0 && m_CurrentAddBlockRateV2 > 0)\n\t\t\t{\n"
           "\t\t\t\tnKMAddBlock = ((int)g_Random(256) / m_CurrentAddBlockRateV0) * m_CurrentAddBlockRateV2;\n"
           "\t\t\t\tif (nKMAddBlock > 25)\n\t\t\t\t\tnKMAddBlock = 25;\n\t\t\t}\n"
           "\t\t\tint nKMBlock = m_CurrentBlockRate + nKMAddBlock - Npc[nLauncher].m_CurrentAntiBlockRate;\n" % MARK)
    t.thay(old, new)
    old = "#ifdef _SERVER\n\t// [WLLS 21/08] bo dem sat thuong HUNG CHIU (ST_*DamageCounter): dat NGAY\n"
    new = ("#ifdef _SERVER\n"
           "\t// %s melee/rangedamagereturnmana_p (Khi Chan Son Ha 1378) - Linux ham sat thuong 0x08089C90 (0x08089F19-0x08089F5F, 0x0808A240):\n"
           "\t// truong nam tren NAN NHAN; sau khi tru mau: mana(KE DANH) += sat_thuong * p / 100 (cat phan le), am -> 0 (0x0808A390).\n"
           "\t// Linux khong kep tran; JX1 kep tran mana toi da de khong vuot UI. bIsMelee chon melee/range nhu [ebp+0x1c] Linux.\n"
           "\tif (nAttacker > 0 && nAttacker < MAX_NPC && nAttacker != m_Index && Npc[nAttacker].m_Index > 0 && nDamage > 0)\n\t{\n"
           "\t\tint nRetManaP = bIsMelee ? m_CurrentMeleeDamageReturnManaP : m_CurrentRangeDamageReturnManaP;\n"
           "\t\tif (nRetManaP != 0)\n\t\t{\n"
           "\t\t\tint nNewMana = Npc[nAttacker].m_CurrentMana + (int)((double)nDamage * nRetManaP / 100.0);\n"
           "\t\t\tif (nNewMana < 0)\n\t\t\t\tnNewMana = 0;\n"
           "\t\t\tif (nNewMana > Npc[nAttacker].m_CurrentManaMax)\n\t\t\t\tnNewMana = Npc[nAttacker].m_CurrentManaMax;\n"
           "\t\t\tAUTOLOG_EVERY(1000, \"[HS-RETMANA] tgt=%%d atk=%%d melee=%%d dmg=%%d p=%%d mana %%d->%%d\", m_Index, nAttacker, (int)bIsMelee, nDamage, nRetManaP, Npc[nAttacker].m_CurrentMana, nNewMana);\n"
           "\t\t\tNpc[nAttacker].m_CurrentMana = nNewMana;\n\t\t}\n\t}\n"
           "\t// [WLLS 21/08] bo dem sat thuong HUNG CHIU (ST_*DamageCounter): dat NGAY\n" % MARK)
    t.thay(old, new)
    old = "void KNpc::ModifyAttrib(int nAttacker, void* pData)\n{\n\tif (pData != NULL)\n\t\tg_NpcAttribModify.ModifyAttrib(this, pData);\n}\n"
    new = ("#ifdef _SERVER\n"
           "// %s Linux 0x08079870 (goi tu handler candetonate1-3 0x08097110 voi launcher=[ebp+0xc]): duyet dan trong vung cua NGUOI PHAT\n"
           "// va 8 vung ke ([region+0x78+i*4]); dan co m_nMissleId == nStyle; quan he chu dan <-> nguoi phat: flag 0 -> self|ally (test al,6),\n"
           "// flag 1 -> enemy (test al,8), khac -> khong kiem; khoang cach mps sqrt(dx^2+dy^2) <= nRadius -> 0x08075210 = tan ngay (DoVanish:\n"
           "// su kien tan cua ky nang me (VD 1380 Ma Van Kiem Khi -> 1411) + GWM_MISSLE_DEL). Tra so dan da kich no.\n"
           "int KNpc::DetonateMissles(int nStyle, int nRadius, int nFlag)\n{\n"
           "\tif (m_Index <= 0 || m_SubWorldIndex < 0 || m_RegionIndex < 0 || nStyle <= 0 || nRadius < 0)\n\t\treturn 0;\n"
           "\tint nMyX = 0, nMyY = 0;\n\tGetMpsPos(&nMyX, &nMyY);\n"
           "\tKSubWorld* pSW = &SubWorld[m_SubWorldIndex];\n"
           "\tint nRegions[9];\n\tint nR = 0;\n\tnRegions[nR++] = m_RegionIndex;\n"
           "\tfor (int k = 0; k < 8; k++)\n\t{\n\t\tint r = pSW->m_Region[m_RegionIndex].m_nConnectRegion[k];\n"
           "\t\tif (r >= 0 && r < MAX_REGION)\n\t\t\tnRegions[nR++] = r;\n\t}\n"
           "\tint nDem = 0;\n"
           "\tfor (int ri = 0; ri < nR; ri++)\n\t{\n"
           "\t\tKRegion* pRegion = &pSW->m_Region[nRegions[ri]];\n"
           "\t\tKIndexNode* pNode = (KIndexNode*)pRegion->m_MissleList.GetHead();\n"
           "\t\twhile (pNode)\n\t\t{\n"
           "\t\t\tKIndexNode* pNext = (KIndexNode*)pNode->GetNext();\t// lay next TRUOC: DoVanish co the go dan khoi danh sach\n"
           "\t\t\tint nIdx = pNode->m_nIndex;\n"
           "\t\t\tif (nIdx > 0 && nIdx < MAX_MISSLE && Missle[nIdx].m_nMissleId == nStyle\n"
           "\t\t\t\t&& Missle[nIdx].m_nLauncher > 0 && Missle[nIdx].m_nLauncher < MAX_NPC && Npc[Missle[nIdx].m_nLauncher].m_Index > 0)\n"
           "\t\t\t{\n"
           "\t\t\t\tint nRel = (int)NpcSet.GetRelation(m_Index, Missle[nIdx].m_nLauncher);\n"
           "\t\t\t\tBOOL bOk = TRUE;\n"
           "\t\t\t\tif (nFlag == 0)\n\t\t\t\t\tbOk = (nRel & (relation_self | relation_ally)) != 0;\n"
           "\t\t\t\telse if (nFlag == 1)\n\t\t\t\t\tbOk = (nRel & relation_enemy) != 0;\n"
           "\t\t\t\tif (bOk)\n\t\t\t\t{\n"
           "\t\t\t\t\tint nMX = 0, nMY = 0;\n\t\t\t\t\tMissle[nIdx].GetMpsPos(&nMX, &nMY);\n"
           "\t\t\t\t\tint nDx = nMX - nMyX, nDy = nMY - nMyY;\n"
           "\t\t\t\t\tint nDist = (int)sqrt((double)(nDx * nDx + nDy * nDy));\n"
           "\t\t\t\t\tif (nDist >= 0 && nRadius >= nDist)\n\t\t\t\t\t{\n"
           "\t\t\t\t\t\tAUTOLOG_IDX(m_Index, \"[HS-DETONATE] lch=%%d style=%%d msl=%%d(sk=%%d owner=%%d rel=%%d) dist=%%d/%%d flag=%%d\", m_Index, nStyle, nIdx, Missle[nIdx].m_nSkillId, Missle[nIdx].m_nLauncher, nRel, nDist, nRadius, nFlag);\n"
           "\t\t\t\t\t\tMissle[nIdx].Detonate();\n\t\t\t\t\t\tnDem++;\n\t\t\t\t\t}\n"
           "\t\t\t\t}\n\t\t\t}\n\t\t\tpNode = pNext;\n\t\t}\n\t}\n\treturn nDem;\n}\n#endif\n\n"
           "void KNpc::ModifyAttrib(int nAttacker, void* pData)\n{\n"
           "\tif (pData == NULL)\n\t\treturn;\n"
           "#ifdef _SERVER\n"
           "\t// %s 2 nhom thuoc tinh Linux can NGUOI PHAT (handler Linux nhan (this, pLauncher, pNpc, pAttrib), JX1 chi co (pNpc, pData)):\n"
           "\tKMagicAttrib* pMA = (KMagicAttrib*)pData;\n"
           "\tif (pMA->nAttribType == magic_reduceskillcd1 || pMA->nAttribType == magic_reduceskillcd2)\n\t{\n"
           "\t\t// Linux 0x08097250: giam hoi chieu ky nang nValue[0] cua NGUOI PHAT di nValue[2] khung, roi gui 0xdd cho client cua nguoi phat\n"
           "\t\tif (nAttacker > 0 && nAttacker < MAX_NPC && Npc[nAttacker].m_Index > 0 && pMA->nValue[0] > 0)\n\t\t{\n"
           "\t\t\tNpc[nAttacker].m_SkillList.ReduceCoolDown(pMA->nValue[0], pMA->nValue[2]);\n"
           "\t\t\tif (Npc[nAttacker].m_Kind == kind_player && Npc[nAttacker].m_nPlayerIdx > 0 && Npc[nAttacker].m_nPlayerIdx < MAX_PLAYER\n"
           "\t\t\t\t&& g_pServer && Player[Npc[nAttacker].m_nPlayerIdx].m_nNetConnectIdx != -1)\n\t\t\t{\n"
           "\t\t\t\tS2C_REDUCE_SKILL_CD sSync;\n\t\t\t\tsSync.ProtocolType = s2c_reduceskillcd;\n"
           "\t\t\t\tsSync.m_wSkillId = (WORD)pMA->nValue[0];\n\t\t\t\tsSync.m_wFrames = (WORD)(pMA->nValue[2] > 0 ? pMA->nValue[2] : 0);\n"
           "\t\t\t\tg_pServer->PackDataToClient(Player[Npc[nAttacker].m_nPlayerIdx].m_nNetConnectIdx, &sSync, sizeof(sSync));\n"
           "\t\t\t}\n\t\t}\n\t\treturn;\n\t}\n"
           "\tif (pMA->nAttribType >= magic_candetonate1 && pMA->nAttribType <= magic_candetonate3)\n\t{\n"
           "\t\t// Linux 0x08097110: nValue[0] = id_dan*256 + co (0 = dan phe ta, 1 = dan dich), nValue[2] = ban kinh (mps), quanh NGUOI PHAT\n"
           "\t\tif (nAttacker > 0 && nAttacker < MAX_NPC && Npc[nAttacker].m_Index > 0 && pMA->nValue[0] > 0)\n"
           "\t\t\tNpc[nAttacker].DetonateMissles(pMA->nValue[0] >> 8, pMA->nValue[2], pMA->nValue[0] & 0xff);\n"
           "\t\treturn;\n\t}\n"
           "#endif\n"
           "\tg_NpcAttribModify.ModifyAttrib(this, pData);\n}\n" % (MARK, MARK))
    t.thay(old, new)
    if not t.co("#include <math.h>") and not t.co("#include \"math.h\""):
        t.thay("#include \"KRegion.h\"\n", "#include \"KRegion.h\"\n#include <math.h>\t// %s sqrt cho DetonateMissles\n" % MARK)
    t.ghi()
print("XONG" + (" (chi kiem)" if KIEM else ""))
