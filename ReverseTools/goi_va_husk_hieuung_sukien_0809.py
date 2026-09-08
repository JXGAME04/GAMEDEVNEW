# -*- coding: utf-8 -*-
"""goi_va_husk_hieuung_sukien_0809.py - [HUSK 08/09] HIEU UNG THEO SU KIEN MAY CHU (het 3 % chieu mat hinh khi dong).
May chu phat goi MOI s2c_skillfired = 224 (S2C_SKILL_FIRED 16 byte) dung luc KSkill::Cast thuc su chay o 6 diem ban theo hoat anh
(OnSkill, OnSpecial1, DoBlurMove, OnManyAttack, OnRunAttack, OnJumpAttack; 2 diem Cast(id,lv)/CastAutoSkillAt van dung 148 nhu cu).
Client: NPC KHAC -> ve hieu ung theo goi 224 (KProtocolProcess::s2cSkillFired), KHONG tu mo phong o khung 60 % nua; chinh minh van tu
mo phong. Hello phien ban 4; may chu chi phat khi KHONG con client nao < 4. Cong lui: [Client] HieuUngSuKien=0 (client bao 3 -> may chu
khong phat cho ai), [Server] HieuUngSuKien=0 (doc lai moi 10 s; PHAI tat ca client). Bo dem: [FX] 10s ... | HUSK(224): ...; [FX-SV].
Tep: Headers/KProtocolDef.h, Core/Src/KProtocol.h, KProtocol.cpp, KProtocolProcess.h/.cpp, KNpc.h, KNpc.cpp (latin-1, CRLF)."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta"
def load(rel):
    p = ROOT + "\\" + rel
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0, lf_ok=False):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if not lf_ok and re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# ---------------- Headers/KProtocolDef.h : ma 224
p, s = load(r"Headers\KProtocolDef.h"); h0 = hi(s)
EOL = "\r\n" if "\r\n" in s else "\n"      # tep nay co the la LF thuan
if "s2c_skillfired" not in s:
    s = rep(s, "\t\t\t\t\t\t\t// g_nProtocolSize (KProtocol.cpp) phai co o 158 cho ma nay. Chi phat khi client bao hello phien ban 3." + EOL,
        "\t\t\t\t\t\t\t// g_nProtocolSize (KProtocol.cpp) phai co o 158 cho ma nay. Chi phat khi client bao hello phien ban 3." + EOL +
        "\ts2c_skillfired,\t\t\t// [HUSK 08/09] = 224: may chu bao 'chieu DA BAN' dung luc KSkill::Cast (S2C_SKILL_FIRED 16 byte). Client ve" + EOL +
        "\t\t\t\t\t\t\t// hieu ung cua NPC KHAC theo goi nay (khong tu mo phong o khung 60 %). g_nProtocolSize (KProtocol.cpp) phai co o 159." + EOL +
        "\t\t\t\t\t\t\t// Chi phat khi moi client dang noi da bao hello phien ban 4." + EOL)
save(p, s, h0, lf_ok=True); print("OK KProtocolDef.h")   # tep goc da co 1 dong LF le (569 CRLF + 1 LF) -> khong kiem LF

# ---------------- KProtocol.h : struct
p, s = load(r"Sources\Core\Src\KProtocol.h"); h0 = hi(s)
if "S2C_SKILL_FIRED" not in s:
    s = rep(s, "} NPC_STATE_SYNC;\r\n",
        "} NPC_STATE_SYNC;\r\n\r\n"
        "// [HUSK 08/09] s2c_skillfired = 224: may chu bao chieu DA BAN (phat dung luc KSkill::Cast o may chu). nMpsX == -1 -> nMpsY = dwID\r\n"
        "// muc tieu (nhu s2c_castskilldirectly); khac -1 -> toa do MPS. Client: KProtocolProcess::s2cSkillFired.\r\n"
        "typedef struct\r\n{\r\n"
        "\tBYTE\tProtocolType;\r\n"
        "\tDWORD\tID;\t\t\t\t// dwID nguoi phat\r\n"
        "\tWORD\twSkillID;\r\n"
        "\tBYTE\tbySkillLevel;\r\n"
        "\tint\t\tnMpsX;\r\n"
        "\tint\t\tnMpsY;\r\n"
        "} S2C_SKILL_FIRED;\t\t\t// 16 byte (pack 1)\r\n")
save(p, s, h0); print("OK KProtocol.h")

# ---------------- KProtocol.cpp : bang kich thuoc o 159
p, s = load(r"Sources\Core\Src\KProtocol.cpp"); h0 = hi(s)
if "S2C_SKILL_FIRED" not in s:
    s = rep(s, "\tsizeof(NPC_STATE_SYNC),\t\t// s2c_syncnpcstate = 223 [DELTA 07/09 l] - o 158 = ma 223 (ngay sau 222), KHONG chen gi giua\r\n",
        "\tsizeof(NPC_STATE_SYNC),\t\t// s2c_syncnpcstate = 223 [DELTA 07/09 l] - o 158 = ma 223 (ngay sau 222), KHONG chen gi giua\r\n"
        "\tsizeof(S2C_SKILL_FIRED),\t// s2c_skillfired = 224 [HUSK 08/09] - o 159 = ma 224 (ngay sau 223), KHONG chen gi giua\r\n")
save(p, s, h0); print("OK KProtocol.cpp")

# ---------------- KProtocolProcess.h
p, s = load(r"Sources\Core\Src\KProtocolProcess.h"); h0 = hi(s)
if "s2cSkillFired" not in s:
    s = rep(s, "\tvoid\ts2cDirectlyCastSkill(BYTE * pMsg);\r\n", "\tvoid\ts2cDirectlyCastSkill(BYTE * pMsg);\r\n\tvoid\ts2cSkillFired(BYTE * pMsg);\t// [HUSK 08/09] goi 224: chieu DA BAN tu may chu\r\n")
save(p, s, h0); print("OK KProtocolProcess.h")

# ---------------- KProtocolProcess.cpp : dang ky, hello 4, handler
p, s = load(r"Sources\Core\Src\KProtocolProcess.cpp"); h0 = hi(s)
if "s2cSkillFired" not in s:
    s = rep(s, "\tProcessFunc[s2c_syncnpcstate] = &KProtocolProcess::SyncNpcState;\t// [DELTA 07/09 l]\r\n",
        "\tProcessFunc[s2c_syncnpcstate] = &KProtocolProcess::SyncNpcState;\t// [DELTA 07/09 l]\r\n"
        "\tProcessFunc[s2c_skillfired] = &KProtocolProcess::s2cSkillFired;\t// [HUSK 08/09]\r\n")
    s = rep(s, "\t\tsHello.byPhienBan = 3;\t// [DELTA 07/09 l] 3 = hieu them s2c_syncnpcstate (223); 2 = 222; may chu cu coi moi gia tri nhu nhau\r\n",
        "\t\t{ extern int HUSK_ClientBat(); sHello.byPhienBan = (BYTE)(HUSK_ClientBat() ? 4 : 3); }\t// [HUSK 08/09] 4 = hieu them s2c_skillfired (224) khi [Client] HieuUngSuKien=1; [DELTA 07/09 l] 3 = 223; 2 = 222\r\n")
    handler = (
        "// [HUSK 08/09] Goi 224: may chu bao chieu DA BAN (dung luc KSkill::Cast o may chu). Ve hieu ung cua NPC KHAC ngay, khong phu thuoc\r\n"
        "// client dang o khung nao / dang ban / hoi chieu / lech nhip. Chinh minh: bo qua (client tu mo phong nhu cu). Khong SetNextCastTime\r\n"
        "// (hoi chieu client van do OnSkill/148 lo). Bo dem in trong dong [FX] 10s (HUSK(224): ...).\r\n"
        "void\tKProtocolProcess::s2cSkillFired(BYTE * pMsg)\r\n{\r\n"
        "\textern int g_nFX_husk_rx, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_fire, g_nFX_husk_fail;\r\n"
        "\textern int HUSK_ClientBat();\r\n"
        "\tg_nFX_husk_rx++;\r\n"
        "\tif (!pMsg || !HUSK_ClientBat())\r\n\t\treturn;\r\n"
        "\tS2C_SKILL_FIRED* pGoi = (S2C_SKILL_FIRED*)pMsg;\r\n"
        "\tint nIdx = NpcSet.SearchID(pGoi->ID);\r\n"
        "\tif (nIdx <= 0 || nIdx >= MAX_NPC) { g_nFX_husk_noidx++; return; }\r\n"
        "\tif (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex) { g_nFX_husk_minh++; return; }\r\n"
        "\tKSkill* pSkill = (KSkill*)g_SkillManager.GetSkill((int)pGoi->wSkillID, (int)pGoi->bySkillLevel);\r\n"
        "\tif (!pSkill) { g_nFX_husk_noskill++; return; }\r\n"
        "\tBOOL bVe;\r\n"
        "\tif (pGoi->nMpsX == -1)\r\n\t{\r\n"
        "\t\tint nTgt = (pGoi->nMpsY > 0) ? NpcSet.SearchID((DWORD)pGoi->nMpsY) : 0;\r\n"
        "\t\tif (nTgt <= 0 || nTgt >= MAX_NPC || Npc[nTgt].m_RegionIndex < 0) { g_nFX_husk_notgt++; return; }\r\n"
        "\t\tbVe = pSkill->Cast(nIdx, -1, nTgt);\r\n"
        "\t}\r\n\telse\r\n"
        "\t\tbVe = pSkill->Cast(nIdx, pGoi->nMpsX, pGoi->nMpsY);\r\n"
        "\tif (bVe) g_nFX_husk_fire++; else g_nFX_husk_fail++;\r\n"
        "}\r\n\r\n")
    s = rep(s, "void\tKProtocolProcess::s2cShowMsg(BYTE *pMsg)\r\n{\r\n", handler + "void\tKProtocolProcess::s2cShowMsg(BYTE *pMsg)\r\n{\r\n")
save(p, s, h0); print("OK KProtocolProcess.cpp")

# ---------------- KNpc.h : khai bao ham may chu
p, s = load(r"Sources\Core\Src\KNpc.h"); h0 = hi(s)
if "HUSK_PhatDaBan" not in s:
    s = rep(s, "\tvoid\t\t\t\tCastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);\t// [HOASON 02/09] tu phong nham muc tieu: phat s2c_castskilldirectly (client ve dan) roi Cast(m_Index, -1, nTarget)\r\n",
        "\tvoid\t\t\t\tCastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);\t// [HOASON 02/09] tu phong nham muc tieu: phat s2c_castskilldirectly (client ve dan) roi Cast(m_Index, -1, nTarget)\r\n"
        "\tvoid\t\t\t\tHUSK_PhatDaBan(int nSkillId, int nLevel, int nParam1, int nParam2);\t// [HUSK 08/09] may chu: phat goi 224 'chieu da ban' cho client trong tam (chi dinh nghia khi _SERVER)\r\n")
save(p, s, h0); print("OK KNpc.h")

# ---------------- KNpc.cpp
p, s = load(r"Sources\Core\Src\KNpc.cpp"); h0 = hi(s)
if "HUSK_PhatDaBan" not in s:
    # 1. bien dem
    s = rep(s, "int g_nFX_sv_start = 0, g_nFX_sv_fire = 0, g_nFX_sv_huyhurt = 0, g_nFX_sv_start_all = 0, g_nFX_sv_fire_all = 0;\r\n",
        "int g_nFX_sv_start = 0, g_nFX_sv_fire = 0, g_nFX_sv_huyhurt = 0, g_nFX_sv_start_all = 0, g_nFX_sv_fire_all = 0;\r\n"
        "int g_nFX_husk_rx = 0, g_nFX_husk_noidx = 0, g_nFX_husk_minh = 0, g_nFX_husk_noskill = 0, g_nFX_husk_notgt = 0, g_nFX_husk_fire = 0, g_nFX_husk_fail = 0, g_nFX_husk_bocuc = 0;\t// [HUSK 08/09] client: goi 224\r\n"
        "int g_nFX_sv_husk = 0, g_nFX_sv_husk_cu = 0;\t// [HUSK 08/09] may chu: goi 224 da phat / lan bo qua vi con client cu (hello < 4)\r\n")
    # 2. ham tro giup sau S13_IsRealPlayer
    helpers = (
        "\treturn pNpc->m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;\r\n#endif\r\n}\r\n"
        "// [HUSK 08/09] HIEU UNG THEO SU KIEN MAY CHU: may chu phat goi 224 (s2c_skillfired) dung luc KSkill::Cast thuc su chay o cac diem ban\r\n"
        "// theo hoat anh (OnSkill, OnSpecial1, DoBlurMove, OnManyAttack, OnRunAttack, OnJumpAttack); client ve hieu ung cua NPC KHAC theo goi do\r\n"
        "// (KProtocolProcess::s2cSkillFired) va KHONG tu mo phong o khung 60 % nua. Truoc: client tu doan -> mat 3-4 % chieu vi bi tu choi im lang\r\n"
        "// (dang ban / trung don truoc 60 % / hoi chieu lech / hai goi 95 mot nhip). Chinh nhan vat: van tu mo phong (cam giac khong doi).\r\n"
        "// Cong lui: client [Client] HieuUngSuKien=0 (client bao hello 3 -> may chu khong phat 224 cho AI); may chu [Server] HieuUngSuKien=0\r\n"
        "// (doc lai moi 10 s; khi tat PHAI tat ca client, neu khong hieu ung NPC khac khong hien).\r\n"
        "#ifndef _SERVER\r\n"
        "int HUSK_ClientBat()\r\n{\r\n"
        "\tstatic int s_nBat = -1;\r\n"
        "\tif (s_nBat < 0)\r\n"
        "\t\ts_nBat = GetPrivateProfileIntA(\"Client\", \"HieuUngSuKien\", 1, \".\\\\config.ini\") ? 1 : 0;\r\n"
        "\treturn s_nBat;\r\n}\r\n"
        "static BOOL HUSK_BoCastCuc(KNpc* pNpc)\t// NPC khac + che do bat -> khong Cast cuc bo, cho goi 224\r\n{\r\n"
        "\treturn HUSK_ClientBat() && !S13_IsRealPlayer(pNpc);\r\n}\r\n"
        "#define HUSK_CAST(pSk, id, lv, p1, p2)\tdo { if (HUSK_BoCastCuc(this)) g_nFX_husk_bocuc++; else (pSk)->Cast(m_Index, (p1), (p2)); } while (0)\r\n"
        "#else\r\n"
        "static int HUSK_SvBat(DWORD dwLuc)\t// [Server] HieuUngSuKien (doc lai moi 10 s) va khong con client hello < 4\r\n{\r\n"
        "\tstatic DWORD s_dwMoc = 0;\r\n"
        "\tstatic int   s_nBat = 1, s_nCu = 0;\r\n"
        "\textern BYTE g_abyDeltaHello[MAX_PLAYER];\r\n"
        "\tif (s_dwMoc == 0 || (dwLuc - s_dwMoc) >= 10000)\r\n\t{\r\n"
        "\t\ts_dwMoc = dwLuc;\r\n"
        "\t\ts_nBat = GetPrivateProfileIntA(\"Server\", \"HieuUngSuKien\", 1, \".\\\\config.ini\") ? 1 : 0;\r\n"
        "\t\tint n = 0;\r\n"
        "\t\tfor (int i = 1; i < MAX_PLAYER; i++)\r\n"
        "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && g_abyDeltaHello[i] < 4)\r\n\t\t\t\tn++;\r\n"
        "\t\ts_nCu = n;\r\n\t}\r\n"
        "\tif (!s_nBat)\r\n\t\treturn 0;\r\n"
        "\tif (s_nCu > 0) { g_nFX_sv_husk_cu++; return 0; }\r\n"
        "\treturn 1;\r\n}\r\n"
        "void KNpc::HUSK_PhatDaBan(int nSkillId, int nLevel, int nParam1, int nParam2)\r\n{\r\n"
        "\tif (m_Index <= 0 || m_RegionIndex < 0 || m_SubWorldIndex < 0 || nSkillId <= 0 || nSkillId >= MAX_SKILL || nLevel <= 0 || nLevel >= MAX_SKILLLEVEL)\r\n\t\treturn;\r\n"
        "\tif (!HUSK_SvBat(SubWorld[m_SubWorldIndex].m_dwCurrentTime))\r\n\t\treturn;\r\n"
        "\tS2C_SKILL_FIRED sGoi;\r\n"
        "\tsGoi.ProtocolType = (BYTE)s2c_skillfired;\r\n"
        "\tsGoi.ID = m_dwID;\r\n"
        "\tsGoi.wSkillID = (WORD)nSkillId;\r\n"
        "\tsGoi.bySkillLevel = (BYTE)nLevel;\r\n"
        "\tif (nParam1 == -1)\r\n\t{\r\n"
        "\t\tif (nParam2 <= 0 || nParam2 >= MAX_NPC || Npc[nParam2].m_Index <= 0)\r\n\t\t\treturn;\r\n"
        "\t\tsGoi.nMpsX = -1;\r\n"
        "\t\tsGoi.nMpsY = (int)Npc[nParam2].m_dwID;\r\n\t}\r\n"
        "\telse\r\n\t{\r\n\t\tsGoi.nMpsX = nParam1;\r\n\t\tsGoi.nMpsY = nParam2;\r\n\t}\r\n"
        "\tstatic const POINT POff[8] = { {0, 32}, {-16, 32}, {-16, 0}, {-16, -32}, {0, -32}, {16, -32}, {16, 0}, {16, 32} };\r\n"
        "\tint nMaxCount = MAX_BROADCAST_COUNT;\r\n"
        "\tCURREGION.BroadCast(&sGoi, sizeof(sGoi), nMaxCount, m_MapX, m_MapY);\r\n"
        "\tfor (int i = 0; i < 8; i++)\r\n\t{\r\n"
        "\t\tif (CONREGIONIDX(i) == -1)\r\n\t\t\tcontinue;\r\n"
        "\t\tCONREGION(i).BroadCast(&sGoi, sizeof(sGoi), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);\r\n\t}\r\n"
        "\tg_nFX_sv_husk++;\r\n}\r\n"
        "#define HUSK_CAST(pSk, id, lv, p1, p2)\tdo { if ((pSk)->Cast(m_Index, (p1), (p2))) HUSK_PhatDaBan((id), (lv), (p1), (p2)); } while (0)\r\n"
        "#endif\r\n")
    s = rep(s, "\treturn pNpc->m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;\r\n#endif\r\n}\r\n", helpers)
    # 3. OnSkill
    s = rep(s, "\t\t\tconst BOOL bFXCast = pSkill->Cast(m_Index, m_DesX, m_DesY);\t// [FX 07/09] dem chieu that su BAN o khung 60%\r\n",
        "\t\t\tBOOL bFXCast;\r\n"
        "#ifndef _SERVER\r\n"
        "\t\t\tif (HUSK_BoCastCuc(this)) { g_nFX_husk_bocuc++; bFXCast = TRUE; }\t// [HUSK 08/09] NPC khac: hieu ung ve theo goi 224 cua may chu, khong tu mo phong\r\n"
        "\t\t\telse\r\n"
        "#endif\r\n"
        "\t\t\tbFXCast = pSkill->Cast(m_Index, m_DesX, m_DesY);\t// [FX 07/09] dem chieu that su BAN o khung 60%\r\n"
        "#ifdef _SERVER\r\n"
        "\t\t\tif (bFXCast) HUSK_PhatDaBan(m_ActiveSkillID, (int)pSkill->m_ulLevel, m_DesX, m_DesY);\t// [HUSK 08/09]\r\n"
        "#endif\r\n")
    # 4. OnSpecial1 (chieu con)
    s = rep(s, "\t\t\t\tif (pChildSkill)\r\n\t\t\t\t{\r\n\t\t\t\t\tpChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);\r\n\t\t\t\t}\r\n",
        "\t\t\t\tif (pChildSkill)\r\n\t\t\t\t{\r\n\t\t\t\t\tHUSK_CAST(pChildSkill, nChildSkill, nChildSkillLevel, m_SkillParam1, m_SkillParam2);\t// [HUSK 08/09]\r\n\t\t\t\t}\r\n")
    # 5. DoBlurMove (chieu con tai diem den)
    s = rep(s, "\t\t\tif (pChildBlur)\r\n\t\t\t\tpChildBlur->Cast(m_Index, -1, m_Index);\r\n",
        "\t\t\tif (pChildBlur)\r\n\t\t\t\tHUSK_CAST(pChildBlur, pSkill->GetChildSkillId(), (int)pSkill->GetSkillLevel(), -1, m_Index);\t// [HUSK 08/09]\r\n")
    # 6. OnManyAttack
    s = rep(s, "\t\t\tif (pOrdinSkill)\r\n            {\r\n\t\t\t\tpOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);\r\n            }\r\n",
        "\t\t\tif (pOrdinSkill)\r\n            {\r\n\t\t\t\tHUSK_CAST(pOrdinSkill, nPhySkillId, (int)pSkill->m_ulLevel, m_SkillParam1, m_SkillParam2);\t// [HUSK 08/09]\r\n            }\r\n")
    # 7. OnRunAttack
    s = rep(s, "\t\t\t\tif (pOrdinSkill)\r\n                {\r\n\t\t\t\t    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);\r\n                }\r\n",
        "\t\t\t\tif (pOrdinSkill)\r\n                {\r\n\t\t\t\t    HUSK_CAST(pOrdinSkill, nCurPhySkillId, (int)pSkill->m_ulLevel, m_SkillParam1, m_SkillParam2);\t// [HUSK 08/09]\r\n                }\r\n")
    # 8. OnJumpAttack
    s = rep(s, "\t\t\t\tif (pOrdinSkill)\r\n                {\r\n\t\t\t\t\tpOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);\r\n                }\r\n",
        "\t\t\t\tif (pOrdinSkill)\r\n                {\r\n\t\t\t\t\tHUSK_CAST(pOrdinSkill, nCurPhySkillId, (int)pSkill->m_ulLevel, m_SkillParam1, m_SkillParam2);\t// [HUSK 08/09]\r\n                }\r\n")
    # 9. [FX] 10s client
    s = rep(s, "huy_sync=%d msl_max=%d/%d\",\r\n", "huy_sync=%d msl_max=%d/%d | HUSK(224): rx=%d ve=%d hong=%d noidx=%d minh=%d noskill=%d notgt=%d bo_cuc=%d\",\r\n")
    s = rep(s, "g_nFX_huy_sync, g_nFX_msl_max, (int)MAX_MISSLE);\r\n",
        "g_nFX_huy_sync, g_nFX_msl_max, (int)MAX_MISSLE,\r\n\t\t\t\t\tg_nFX_husk_rx, g_nFX_husk_fire, g_nFX_husk_fail, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_bocuc);\r\n")
    s = rep(s, "\t\t\t\tg_nFX_msl_ol_trong = 0; g_nFX_msl_ol_khacid = 0; g_nFX_msl_ol_mocoi = 0; g_nFX_huy_sync = 0; g_nFX_msl_max = 0;\r\n",
        "\t\t\t\tg_nFX_msl_ol_trong = 0; g_nFX_msl_ol_khacid = 0; g_nFX_msl_ol_mocoi = 0; g_nFX_huy_sync = 0; g_nFX_msl_max = 0;\r\n"
        "\t\t\t\tg_nFX_husk_rx = 0; g_nFX_husk_fire = 0; g_nFX_husk_fail = 0; g_nFX_husk_noidx = 0; g_nFX_husk_minh = 0; g_nFX_husk_noskill = 0; g_nFX_husk_notgt = 0; g_nFX_husk_bocuc = 0;\t// [HUSK 08/09]\r\n")
    # 10. [FX-SV] may chu
    s = rep(s, "\t\t\t\tAUTOLOG(\"[FX-SV] 10s gan nguoi that (32 o): bat_dau=%d ban=%d ngat_vi_trung_don=%d | toan may chu: bat_dau=%d ban=%d\",\r\n\t\t\t\t\tg_nFX_sv_start, g_nFX_sv_fire, g_nFX_sv_huyhurt, g_nFX_sv_start_all, g_nFX_sv_fire_all);\r\n\t\t\t\tg_nFX_sv_start = 0; g_nFX_sv_fire = 0; g_nFX_sv_huyhurt = 0; g_nFX_sv_start_all = 0; g_nFX_sv_fire_all = 0;\r\n",
        "\t\t\t\tAUTOLOG(\"[FX-SV] 10s gan nguoi that (32 o): bat_dau=%d ban=%d ngat_vi_trung_don=%d | toan may chu: bat_dau=%d ban=%d | goi 224 da ban: phat=%d bo_vi_client_cu=%d\",\r\n\t\t\t\t\tg_nFX_sv_start, g_nFX_sv_fire, g_nFX_sv_huyhurt, g_nFX_sv_start_all, g_nFX_sv_fire_all, g_nFX_sv_husk, g_nFX_sv_husk_cu);\r\n\t\t\t\tg_nFX_sv_start = 0; g_nFX_sv_fire = 0; g_nFX_sv_huyhurt = 0; g_nFX_sv_start_all = 0; g_nFX_sv_fire_all = 0; g_nFX_sv_husk = 0; g_nFX_sv_husk_cu = 0;\r\n")
save(p, s, h0); print("OK KNpc.cpp")
print("XONG HUSK")
