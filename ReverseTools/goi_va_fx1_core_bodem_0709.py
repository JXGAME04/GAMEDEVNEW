# -*- coding: ascii -*-
"""fx_patch1_core.py - [FX 07/09] Core (client + may chu): bo dem HIEU UNG KY NANG chinh xac + 3 sua chac chan.
Chu: "mat het hinh anh ky nang khi danh dong, chi con dong tac danh"; "tim so chinh xac, khong ket luan voi".
Moi nhan cu tren duong chieu/dan la AUTOLOG_EVERY (rate-limit) nen KHONG dem duoc. Nay dem thang:
 CLIENT ([FX] moi 10 s, in tu KNpc::Activate cua chinh nhan vat):
   rx95 / noidx / start           : goi 95 nhan / nguoi phong khong co trong bang / da ra lenh do_skill
   fire / fire_fail / bo_tgt      : OnSkill toi khung 60% goi Cast (OK / FALSE) / bo vi muc tieu <=0 hoac mo coi  (KHAC va MINH)
   huy_hurt (truoc60)             : DoHurt khi dang do_magic (va so lan chua toi 60%)
   huy_lenh di/dung/skill/khac    : ProcCommand de lenh moi len do_magic chua toi 60%
   huy_chet                       : DoDeath khi dang do_magic
   148: rx / noskill / cast / fail: goi cast truc tiep
   dan: add_full / add_vung       : MissleSet.Add tra -1 vi het be / vi vi tri ngoai vung da nap
   kieu(line ext wall circle spread zone): Add -1 theo kieu cast (5 cho truoc day IM LANG)
   chet_som(nolauncher ownerlost tgtlost): dan chet som trong KMissle::Activate
   buff_het_o                     : KNpcRes::SetState het 6 o hieu ung/loai (truoc day im lang)
 MAY CHU ([FX-SV] moi 10 s canh [NS-BO]): bat dau (DoSkill phat 95) / ban (OnSkill Cast) / ngat vi trung don,
   loc theo NPC trong 32 o quanh nguoi choi THAT, va toan may chu.
 SUA: (1) tuy chon 'Giam skill' ghi LowEstMissle nhung 8 cho doc LowMissle -> ghi LowMissle;
      (2) [E3_MISSLES_BADLAUNCHER] dat sau dieu kien; (3) [MISSLE-POOL-FULL] doi ten [MISSLE-ADD-FAIL] (khong phai het be).
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao va so lan khop."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
F = {
 "npc": ROOT + r"\KNpc.cpp",
 "ppc": ROOT + r"\KProtocolProcess.cpp",
 "sk":  ROOT + r"\KSkills.cpp",
 "mset": ROOT + r"\KMissleSet.cpp",
 "msl": ROOT + r"\KMissle.cpp",
 "res": ROOT + r"\KNpcRes.cpp",
 "shell": ROOT + r"\CoreShell.cpp",
}
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
def crlf(s): return s.replace("\r\n", "\n").replace("\n", "\r\n")
def chk(*parts):
    for t in parts:
        if any(ord(c) >= 0x80 for c in t): print("FAIL non-ASCII in patch"); sys.exit(1)
def rep(s, old, new, cnt=1, tag=""):
    old = crlf(old); new = crlf(new); chk(old, new)
    n = s.count(old)
    if n != cnt: print("FAIL [%s]: found %d expect %d: %r" % (tag, n, cnt, old[:90])); sys.exit(1)
    return s.replace(old, new)
def rep_re(s, pat, new, cnt=1, tag=""):
    ms = list(re.finditer(pat, s))
    if len(ms) != cnt: print("FAIL re[%s]: found %d expect %d" % (tag, len(ms), cnt)); sys.exit(1)
    return re.sub(pat, new, s)

out = {}
# ============================================================================ KNpc.cpp
s = rd(F["npc"]); h0 = hb(s)

# (1) dinh nghia bo dem + helper may chu, truoc S13_IsRealPlayer (moi include da xong)
s = rep(s,
"static NPC_COMMAND	s_S13Move[MAX_NPC];\nstatic BOOL S13_IsRealPlayer(KNpc* pNpc)\n",
"// [FX 07/09] Bo dem HIEU UNG KY NANG - dem CHINH XAC (khong rate-limit), in moi 10 s:\n"
"//   client: [FX] trong KNpc::Activate cua chinh nhan vat; may chu: [FX-SV] canh [NS-BO] trong NormalSync.\n"
"// Dinh nghia ngoai moi #ifdef de ca hai ban lien ket duoc; tep khac dung 'extern int g_nFX_...;' tai cho dung.\n"
"int g_nFX_rx95 = 0, g_nFX_rx95_noidx = 0, g_nFX_rx95_start = 0;\n"
"int g_nFX_rx148 = 0, g_nFX_rx148_noskill = 0, g_nFX_rx148_cast = 0, g_nFX_rx148_fail = 0;\n"
"int g_nFX_fire_khac = 0, g_nFX_fire_minh = 0, g_nFX_firefail_khac = 0, g_nFX_firefail_minh = 0;\n"
"int g_nFX_botgt_khac = 0, g_nFX_botgt_minh = 0;\n"
"int g_nFX_huyhurt_khac = 0, g_nFX_huyhurt_minh = 0, g_nFX_huyhurt_truoc = 0;\n"
"int g_nFX_huylenh_di = 0, g_nFX_huylenh_dung = 0, g_nFX_huylenh_skill = 0, g_nFX_huylenh_khac = 0;\n"
"int g_nFX_huychet = 0;\n"
"int g_nFX_add_full = 0, g_nFX_add_vung = 0;\n"
"int g_nFX_style_line = 0, g_nFX_style_ext = 0, g_nFX_style_wall = 0, g_nFX_style_circle = 0, g_nFX_style_spread = 0, g_nFX_style_zone = 0;\n"
"int g_nFX_msl_nolauncher = 0, g_nFX_msl_ownerlost = 0, g_nFX_msl_tgtlost = 0;\n"
"int g_nFX_buff_heto = 0;\n"
"int g_nFX_sv_start = 0, g_nFX_sv_fire = 0, g_nFX_sv_huyhurt = 0, g_nFX_sv_start_all = 0, g_nFX_sv_fire_all = 0;\n"
"DWORD g_uFXMoc = 0;\n"
"#ifdef _SERVER\n"
"// NPC co nam trong 32 o (tam phat tan) quanh mot nguoi choi THAT khong - de so thang voi [FX] cua client\n"
"static int   s_anFXNguoiThat[8];\n"
"static int   s_nFXNguoiThat = 0;\n"
"static DWORD s_uFXNguoiThatMoc = 0;\n"
"static BOOL FX_GanNguoiThat(KNpc* pNpc)\n"
"{\n"
"	const DWORD uNay = timeGetTime();\n"
"	if (s_uFXNguoiThatMoc == 0 || (DWORD)(uNay - s_uFXNguoiThatMoc) >= 2000)\n"
"	{\n"
"		s_uFXNguoiThatMoc = uNay;\n"
"		s_nFXNguoiThat = 0;\n"
"		for (int i = 1; i < MAX_PLAYER && s_nFXNguoiThat < 8; i++)\n"
"		{\n"
"			if (Player[i].m_nIndex > 0 && Player[i].m_nNetConnectIdx >= 0)\n"
"				s_anFXNguoiThat[s_nFXNguoiThat++] = Player[i].m_nIndex;\n"
"		}\n"
"	}\n"
"	if (!pNpc || pNpc->m_Index <= 0 || pNpc->m_RegionIndex < 0 || pNpc->m_SubWorldIndex < 0)\n"
"		return FALSE;\n"
"	for (int k = 0; k < s_nFXNguoiThat; k++)\n"
"	{\n"
"		const int nIdx = s_anFXNguoiThat[k];\n"
"		if (nIdx <= 0 || nIdx >= MAX_NPC)\n"
"			continue;\n"
"		KNpc* pP = &Npc[nIdx];\n"
"		if (pP->m_Index <= 0 || pP->m_RegionIndex < 0 || pP->m_SubWorldIndex != pNpc->m_SubWorldIndex)\n"
"			continue;\n"
"		int nX1 = 0, nY1 = 0, nX2 = 0, nY2 = 0;\n"
"		SubWorld[pNpc->m_SubWorldIndex].Map2Mps(pNpc->m_RegionIndex, pNpc->m_MapX, pNpc->m_MapY, 0, 0, &nX1, &nY1);\n"
"		SubWorld[pP->m_SubWorldIndex].Map2Mps(pP->m_RegionIndex, pP->m_MapX, pP->m_MapY, 0, 0, &nX2, &nY2);\n"
"		const int nTamX = 32 * SubWorld[pNpc->m_SubWorldIndex].m_nCellWidth;\n"
"		const int nTamY = 32 * SubWorld[pNpc->m_SubWorldIndex].m_nCellHeight;\n"
"		const int nDX = (nX1 > nX2) ? (nX1 - nX2) : (nX2 - nX1);\n"
"		const int nDY = (nY1 > nY2) ? (nY1 - nY2) : (nY2 - nY1);\n"
"		if (nDX <= nTamX && nDY <= nTamY)\n"
"			return TRUE;\n"
"	}\n"
"	return FALSE;\n"
"}\n"
"#endif\n"
"\n"
"static NPC_COMMAND	s_S13Move[MAX_NPC];\nstatic BOOL S13_IsRealPlayer(KNpc* pNpc)\n", 1, "npc-def")

# (2) bao cao client moi 10 s trong Activate cua chinh nhan vat
s = rep(s,
"	if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)\n	{\n		// [NAMBEP 07/09 m] Ghi trang thai SAU HOI SINH",
"	if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)\n	{\n"
"		{	// [FX 07/09] bao cao bo dem hieu ung moi 10 s (dem chinh xac; doc BANGIAO_HIEUUNG_KHAOSAT_0709.md)\n"
"			const DWORD uFXNay = timeGetTime();\n"
"			if (g_uFXMoc == 0)\n"
"				g_uFXMoc = uFXNay;\n"
"			else if ((DWORD)(uFXNay - g_uFXMoc) >= 10000)\n"
"			{\n"
"				g_uFXMoc = uFXNay;\n"
"				AUTOLOG(\"[FX] 10s KHAC: rx95=%d noidx=%d start=%d fire=%d fire_fail=%d bo_tgt=%d huy_hurt=%d (truoc60=%d ca hai) huy_lenh(di=%d dung=%d skill=%d khac=%d) huy_chet=%d | MINH: fire=%d fire_fail=%d bo_tgt=%d huy_hurt=%d | 148: rx=%d noskill=%d cast=%d fail=%d | dan: add_full=%d add_vung=%d kieu(line=%d ext=%d wall=%d circle=%d spread=%d zone=%d) chet_som(nolauncher=%d ownerlost=%d tgtlost=%d) | buff_het_o=%d\",\n"
"					g_nFX_rx95, g_nFX_rx95_noidx, g_nFX_rx95_start, g_nFX_fire_khac, g_nFX_firefail_khac, g_nFX_botgt_khac, g_nFX_huyhurt_khac, g_nFX_huyhurt_truoc,\n"
"					g_nFX_huylenh_di, g_nFX_huylenh_dung, g_nFX_huylenh_skill, g_nFX_huylenh_khac, g_nFX_huychet,\n"
"					g_nFX_fire_minh, g_nFX_firefail_minh, g_nFX_botgt_minh, g_nFX_huyhurt_minh,\n"
"					g_nFX_rx148, g_nFX_rx148_noskill, g_nFX_rx148_cast, g_nFX_rx148_fail,\n"
"					g_nFX_add_full, g_nFX_add_vung, g_nFX_style_line, g_nFX_style_ext, g_nFX_style_wall, g_nFX_style_circle, g_nFX_style_spread, g_nFX_style_zone,\n"
"					g_nFX_msl_nolauncher, g_nFX_msl_ownerlost, g_nFX_msl_tgtlost, g_nFX_buff_heto);\n"
"				g_nFX_rx95 = 0; g_nFX_rx95_noidx = 0; g_nFX_rx95_start = 0;\n"
"				g_nFX_rx148 = 0; g_nFX_rx148_noskill = 0; g_nFX_rx148_cast = 0; g_nFX_rx148_fail = 0;\n"
"				g_nFX_fire_khac = 0; g_nFX_fire_minh = 0; g_nFX_firefail_khac = 0; g_nFX_firefail_minh = 0;\n"
"				g_nFX_botgt_khac = 0; g_nFX_botgt_minh = 0;\n"
"				g_nFX_huyhurt_khac = 0; g_nFX_huyhurt_minh = 0; g_nFX_huyhurt_truoc = 0;\n"
"				g_nFX_huylenh_di = 0; g_nFX_huylenh_dung = 0; g_nFX_huylenh_skill = 0; g_nFX_huylenh_khac = 0;\n"
"				g_nFX_huychet = 0; g_nFX_add_full = 0; g_nFX_add_vung = 0;\n"
"				g_nFX_style_line = 0; g_nFX_style_ext = 0; g_nFX_style_wall = 0; g_nFX_style_circle = 0; g_nFX_style_spread = 0; g_nFX_style_zone = 0;\n"
"				g_nFX_msl_nolauncher = 0; g_nFX_msl_ownerlost = 0; g_nFX_msl_tgtlost = 0; g_nFX_buff_heto = 0;\n"
"			}\n"
"		}\n"
"		// [NAMBEP 07/09 m] Ghi trang thai SAU HOI SINH", 1, "npc-baocao")

# (3) OnSkill: bo vi muc tieu
s = rep(s,
"			if (m_DesY <= 0) \n				goto Label_ProcessAI;\n			\n			if (Npc[m_DesY].m_RegionIndex < 0) \n				goto Label_ProcessAI;\n",
"			if (m_DesY <= 0) \n"
"			{\n"
"#ifndef _SERVER\n"
"				if (S13_IsRealPlayer(this)) g_nFX_botgt_minh++; else g_nFX_botgt_khac++;	// [FX 07/09]\n"
"#endif\n"
"				goto Label_ProcessAI;\n"
"			}\n"
"			\n"
"			if (Npc[m_DesY].m_RegionIndex < 0) \n"
"			{\n"
"#ifndef _SERVER\n"
"				if (S13_IsRealPlayer(this)) g_nFX_botgt_minh++; else g_nFX_botgt_khac++;	// [FX 07/09] muc tieu mo coi (cuon vung) - truoc day bo im lang\n"
"#endif\n"
"				goto Label_ProcessAI;\n"
"			}\n", 1, "npc-onskill-tgt")

# (4) OnSkill: Cast
s = rep(s,
"			pSkill->Cast(m_Index, m_DesX, m_DesY);\n",
"			const BOOL bFXCast = pSkill->Cast(m_Index, m_DesX, m_DesY);	// [FX 07/09] dem chieu that su BAN o khung 60%\n"
"#ifndef _SERVER\n"
"			if (S13_IsRealPlayer(this)) { if (bFXCast) g_nFX_fire_minh++; else g_nFX_firefail_minh++; }\n"
"			else { if (bFXCast) g_nFX_fire_khac++; else g_nFX_firefail_khac++; }\n"
"#else\n"
"			g_nFX_sv_fire_all++;\n"
"			if (FX_GanNguoiThat(this)) g_nFX_sv_fire++;\n"
"#endif\n", 1, "npc-onskill-cast")

# (5) DoHurt: dang do_magic
s = rep(s,
"	m_Doing = do_hurt;\n	m_ProcessAI	= 0;\n",
"	if (m_Doing == do_magic)	// [FX 07/09] trung don NGAT thi trien (luat JX1, ca hai ben) - dem de biet mat hieu ung do dau\n"
"	{\n"
"#ifndef _SERVER\n"
"		if (S13_IsRealPlayer(this)) g_nFX_huyhurt_minh++; else g_nFX_huyhurt_khac++;\n"
"		if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame * ATTACKACTION_EFFECT_PERCENT / 100) g_nFX_huyhurt_truoc++;\n"
"#else\n"
"		if (FX_GanNguoiThat(this)) g_nFX_sv_huyhurt++;\n"
"#endif\n"
"	}\n"
"	m_Doing = do_hurt;\n	m_ProcessAI	= 0;\n", 1, "npc-dohurt")

# (6) ProcCommand: lenh moi de len do_magic chua toi 60% (lenh duoc xoa o cuoi ham, dong 1344, nen moi lenh dem mot lan)
s = rep(s,
"		if (m_RegionIndex < 0)\n			return;\n		switch (m_Command.CmdKind)\n		{\n		case do_stand:\n",
"		if (m_RegionIndex < 0)\n			return;\n"
"#ifndef _SERVER\n"
"		if (m_Doing == do_magic && m_Frames.nCurrentFrame < m_Frames.nTotalFrame * ATTACKACTION_EFFECT_PERCENT / 100)\n"
"		{	// [FX 07/09] lenh moi de len thi trien chua toi khung 60% -> hieu ung khong bao gio ra\n"
"			switch (m_Command.CmdKind)\n"
"			{\n"
"			case do_walk: case do_run: case do_jump: g_nFX_huylenh_di++; break;\n"
"			case do_stand: g_nFX_huylenh_dung++; break;\n"
"			case do_skill: g_nFX_huylenh_skill++; break;\n"
"			case do_hurt: case do_death: break;	// dem rieng o DoHurt / DoDeath\n"
"			default: g_nFX_huylenh_khac++; break;\n"
"			}\n"
"		}\n"
"#endif\n"
"		switch (m_Command.CmdKind)\n		{\n		case do_stand:\n", 1, "npc-proccmd")

# (7) DoDeath
s = rep(s,
"	m_Doing = do_death;\n",
"#ifndef _SERVER\n	if (m_Doing == do_magic) g_nFX_huychet++;	// [FX 07/09]\n#endif\n	m_Doing = do_death;\n", 1, "npc-dodeath")

# (8) DoSkill may chu: phat 95
s = rep(s,
"NetCommand.ProtocolType = (BYTE)s2c_skillcast;\n",
"NetCommand.ProtocolType = (BYTE)s2c_skillcast;\n"
"#ifdef _SERVER\n"
"					g_nFX_sv_start_all++;	// [FX 07/09] chieu bat dau (phat 95) - so voi [FX] rx95 cua client\n"
"					if (FX_GanNguoiThat(this)) g_nFX_sv_start++;\n"
"#endif\n", 1, "npc-doskill")

# (9) bao cao may chu canh [NS-TT]
s = rep(s,
"				AUTOLOG(\"[NS-TT] 10s goi trang thai gon (223): %d lan (client chua bao phien ban 3: %d)\", s_nNSTT, NS_SoClientCu3(dwLuc));\n",
"				AUTOLOG(\"[NS-TT] 10s goi trang thai gon (223): %d lan (client chua bao phien ban 3: %d)\", s_nNSTT, NS_SoClientCu3(dwLuc));\n"
"				AUTOLOG(\"[FX-SV] 10s gan nguoi that (32 o): bat_dau=%d ban=%d ngat_vi_trung_don=%d | toan may chu: bat_dau=%d ban=%d\",\n"
"					g_nFX_sv_start, g_nFX_sv_fire, g_nFX_sv_huyhurt, g_nFX_sv_start_all, g_nFX_sv_fire_all);\n"
"				g_nFX_sv_start = 0; g_nFX_sv_fire = 0; g_nFX_sv_huyhurt = 0; g_nFX_sv_start_all = 0; g_nFX_sv_fire_all = 0;\n", 1, "npc-baocao-sv")
if hb(s) != h0: print("FAIL hb npc"); sys.exit(1)
out["npc"] = (s, h0)

# ============================================================================ KProtocolProcess.cpp
s = rd(F["ppc"]); h0 = hb(s)
s = rep(s,
"	nSkillEnChance = *(int *)&pMsg[21];\n",
"	nSkillEnChance = *(int *)&pMsg[21];\n	{ extern int g_nFX_rx95; g_nFX_rx95++; }	// [FX 07/09]\n", 1, "ppc-rx95")
s = rep(s,
"	if (nIdx <= 0) \n		return;\n\n	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))\n	{\n		AUTOLOG_EVERY(1000, \"NETSKILL-DO",
"	if (nIdx <= 0) \n	{\n		extern int g_nFX_rx95_noidx; g_nFX_rx95_noidx++;	// [FX 07/09]\n		return;\n	}\n\n	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))\n	{\n		AUTOLOG_EVERY(1000, \"NETSKILL-DO", 1, "ppc-noidx")
s = rep(s,
"		Npc[nIdx].SendCommand(do_skill, nSkillID, MapX, MapY);\n",
"		{ extern int g_nFX_rx95_start; g_nFX_rx95_start++; }	// [FX 07/09]\n		Npc[nIdx].SendCommand(do_skill, nSkillID, MapX, MapY);\n", 1, "ppc-start")
s = rep(s,
"	int nIdx = NpcSet.SearchID(dwNpcID);\n	if ((nSkillID >= 1363 && nSkillID <= 1384) || (nSkillID >= 1965",
"	int nIdx = NpcSet.SearchID(dwNpcID);\n	{ extern int g_nFX_rx148; g_nFX_rx148++; }	// [FX 07/09]\n	if ((nSkillID >= 1363 && nSkillID <= 1384) || (nSkillID >= 1965", 1, "ppc-rx148")
s = rep(s,
"	if (!pOrdinSkill) \n        return ;\n",
"	if (!pOrdinSkill) \n	{\n		extern int g_nFX_rx148_noskill; g_nFX_rx148_noskill++;	// [FX 07/09]\n		return ;\n	}\n", 1, "ppc-noskill")
s = rep_re(s,
 r"    if \(nVhtdTargetIdx > 0\)\r\n        pOrdinSkill->Cast\(nIdx, -1, nVhtdTargetIdx\);([^\r\n]*)\r\n    else\r\n        pOrdinSkill->Cast\(nIdx, MapX, MapY\);\r\n",
 lambda m: ("    {\r\n"
            "        extern int g_nFX_rx148_cast, g_nFX_rx148_fail;	// [FX 07/09]\r\n"
            "        BOOL bFXCast;\r\n"
            "        if (nVhtdTargetIdx > 0)\r\n"
            "            bFXCast = pOrdinSkill->Cast(nIdx, -1, nVhtdTargetIdx);" + m.group(1) + "\r\n"
            "        else\r\n"
            "            bFXCast = pOrdinSkill->Cast(nIdx, MapX, MapY);\r\n"
            "        if (bFXCast) g_nFX_rx148_cast++; else g_nFX_rx148_fail++;\r\n"
            "    }\r\n"), 1, "ppc-148cast")
if hb(s) != h0: print("FAIL hb ppc"); sys.exit(1)
out["ppc"] = (s, h0)

# ============================================================================ KSkills.cpp
s = rd(F["sk"]); h0 = hb(s)
s = rep(s,
"	AUTOLOG_EVERY(1000, \"[E3_MISSLES_BADLAUNCHER] skill=%d launcher=%d ltype=%d p1=%d p2=%d\", (int)m_nId, nLauncher, (int)eLauncherType, nParam1, nParam2);\n	if (nLauncher <= 0) return FALSE;\n",
"	if (nLauncher <= 0)\n	{	// [FX 07/09] nhan nay truoc day dat TRUOC dieu kien nen in ca khi binh thuong\n		AUTOLOG_EVERY(1000, \"[E3_MISSLES_BADLAUNCHER] skill=%d launcher=%d ltype=%d p1=%d p2=%d\", (int)m_nId, nLauncher, (int)eLauncherType, nParam1, nParam2);\n		return FALSE;\n	}\n", 1, "sk-badlauncher")
# Line
s = rep(s,
"			if (nSubWorldId < 0)	goto exit;\n			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			\n			if (nMissleIndex < 0)	continue;\n",
"			if (nSubWorldId < 0)	goto exit;\n			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			\n			if (nMissleIndex < 0)	{ extern int g_nFX_style_line; g_nFX_style_line++; continue; }	// [FX 07/09] truoc day im lang\n", 1, "sk-line")
# ExtractiveLine
s = rep(s,
"			nMissleIndex = MissleSet.Add(nSubWorldId, nSrcX, nSrcY);\n			\n			if (nMissleIndex < 0)	goto exit;\n",
"			nMissleIndex = MissleSet.Add(nSubWorldId, nSrcX, nSrcY);\n			\n			if (nMissleIndex < 0)	{ extern int g_nFX_style_ext; g_nFX_style_ext++; goto exit; }	// [FX 07/09]\n", 1, "sk-ext")
# Wall
s = rep(s,
"			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			if (nMissleIndex < 0)	\n			{\n				continue;\n			}\n",
"			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			if (nMissleIndex < 0)	\n			{\n				extern int g_nFX_style_wall; g_nFX_style_wall++;	// [FX 07/09]\n				continue;\n			}\n", 1, "sk-wall")
# Circle
s = rep(s,
"			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubPX, nDesSubPY);\n			\n			if (nMissleIndex < 0)	\n			{\n				continue;\n			}\n",
"			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubPX, nDesSubPY);\n			\n			if (nMissleIndex < 0)	\n			{\n				extern int g_nFX_style_circle; g_nFX_style_circle++;	// [FX 07/09]\n				continue;\n			}\n", 1, "sk-circle")
# Spread
s = rep(s,
"			if (nSubWorldId < 0)	goto exit;\n			\n			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			\n			if (nMissleIndex < 0)	continue;\n",
"			if (nSubWorldId < 0)	goto exit;\n			\n			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);\n			\n			if (nMissleIndex < 0)	{ extern int g_nFX_style_spread; g_nFX_style_spread++; continue; }	// [FX 07/09]\n", 1, "sk-spread")
# Zone: nhan doi ten + dem
s = rep(s,
"						AUTOLOG_EVERY(1000, \"[MISSLE-POOL-FULL] sk=%d launcher=%d subworld=%d des(%d,%d) i=%d j=%d Add tra ve %d -> BO QUA vien dan THAT\", (int)m_nId, nLauncher, nSubWorldId, nDesSubX, nDesSubY, i, j, nMissleIndex);\n",
"						{ extern int g_nFX_style_zone; g_nFX_style_zone++; }	// [FX 07/09]\n"
"						AUTOLOG_EVERY(1000, \"[MISSLE-ADD-FAIL] (het be HOAC vi tri ngoai vung da nap - xem [FX] add_full/add_vung) sk=%d launcher=%d subworld=%d des(%d,%d) i=%d j=%d Add tra ve %d -> BO QUA vien dan THAT\", (int)m_nId, nLauncher, nSubWorldId, nDesSubX, nDesSubY, i, j, nMissleIndex);\n", 1, "sk-zone")
if hb(s) != h0: print("FAIL hb sk"); sys.exit(1)
out["sk"] = (s, h0)

# ============================================================================ KMissleSet.cpp
s = rd(F["mset"]); h0 = hb(s)
s = rep(s,
"		printf(\"MissleSet Have Full!!!, It Maybe A Error!\");\n",
"		printf(\"MissleSet Have Full!!!, It Maybe A Error!\");\n		{ extern int g_nFX_add_full; g_nFX_add_full++; }	// [FX 07/09]\n", 1, "mset-full")
s = rep(s,
"	if (Missle[nFreeIndex].m_nRegionId < 0) return -1;\n",
"	if (Missle[nFreeIndex].m_nRegionId < 0)\n	{\n		extern int g_nFX_add_vung; g_nFX_add_vung++;	// [FX 07/09] vi tri ngoai vung da nap - [MISSLE-POOL-FULL] cu bao nham la het be\n		return -1;\n	}\n", 1, "mset-vung")
if hb(s) != h0: print("FAIL hb mset"); sys.exit(1)
out["mset"] = (s, h0)

# ============================================================================ KMissle.cpp
s = rd(F["msl"]); h0 = hb(s)
s = rep_re(s,
 r"(\[MIS-ACT-NOLAUNCHER\][^\r\n]*\r\n\tif \(m_nLauncher <= 0\)\r\n)\t\treturn 0;\r\n",
 lambda m: m.group(1) + "\t{ extern int g_nFX_msl_nolauncher; g_nFX_msl_nolauncher++; return 0; }	// [FX 07/09]\r\n", 1, "msl-nolauncher")
s = rep(s,
"		(Npc[m_nLauncher].m_RegionIndex < 0))\n	{\n		DoVanish();\n",
"		(Npc[m_nLauncher].m_RegionIndex < 0))\n	{\n		{ extern int g_nFX_msl_ownerlost; g_nFX_msl_ownerlost++; }	// [FX 07/09]\n		DoVanish();\n", 1, "msl-ownerlost")
s = rep(s,
"			|| Npc[m_nFollowNpcIdx].m_HideState.nTime > 0)\n		{\n			m_nFollowNpcIdx = 0;\n",
"			|| Npc[m_nFollowNpcIdx].m_HideState.nTime > 0)\n		{\n			{ extern int g_nFX_msl_tgtlost; g_nFX_msl_tgtlost++; }	// [FX 07/09]\n			m_nFollowNpcIdx = 0;\n", 1, "msl-tgtlost")
if hb(s) != h0: print("FAIL hb msl"); sys.exit(1)
out["msl"] = (s, h0)

# ============================================================================ KNpcRes.cpp
s = rd(F["res"]); h0 = hb(s)
s = rep(s,
"				m_cStateSpr[j].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);\n				break;\n			}\n		}\n",
"				m_cStateSpr[j].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);\n				break;\n			}\n		}\n"
"		if (j >= nType * (MAX_SKILL_STATE/3) + (MAX_SKILL_STATE/3))\n"
"		{\n"
"			extern int g_nFX_buff_heto; g_nFX_buff_heto++;	// [FX 07/09] het 6 o hieu ung/loai -> buff nay khong ve (truoc day im lang)\n"
"		}\n", 1, "res-buff")
if hb(s) != h0: print("FAIL hb res"); sys.exit(1)
out["res"] = (s, h0)

# ============================================================================ CoreShell.cpp
s = rd(F["shell"]); h0 = hb(s)
s = rep(s,
"			Option.SetLow(LowEstMissle, nParam);\n",
"			Option.SetLow(LowMissle, nParam);	// [FX 07/09] truoc ghi LowEstMissle nhung 8 cho doc LowMissle -> tuy chon 'Giam skill' chua bao gio co tac dung\n", 1, "shell-giamskill")
if hb(s) != h0: print("FAIL hb shell"); sys.exit(1)
out["shell"] = (s, h0)

for k, (s, h0) in out.items():
    if re.search(r"[^\r]\n", s): print("FAIL bare LF", k); sys.exit(1)
for k, (s, h0) in out.items():
    wr(F[k], s); print("OK %-6s high-bytes %5d  %s" % (k, h0, F[k]))
print("ALL APPLIED (core)")
