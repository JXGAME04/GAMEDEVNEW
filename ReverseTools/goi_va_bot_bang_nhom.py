# -*- coding: utf-8 -*-
# [BOTBANG + BOTNHOM-NGUOI 01/09] Hai tinh nang chu game giao:
#  (1) "khi nguoi choi NHAN MAT voi bot ru vao bang hoi thi bot se ve tai NPC mon phai
#      va xin, chon dung bang hoi cua nguoi ru de xin vao bang hoi o ban bang hoi"
#  (2) "bot duoc nguoi choi moi vao party - neu chua co party thi se vao party nguoi
#      choi - khi nguoi choi doi map thi bot tu thoat party"
# Thiet ke (moi cho deu dung ham SAN CO, KHONG them goi tin):
#  - Bang hoi: bat trong PB_WhisperReply (ham nay CHI nhan whisper tu GameServer =
#    dung "nhan mat"), tu khoa ASCII "bang" + nguoi gui phai co bang. May trang thai
#    pb_XinVaoBang trong pb_DriveBot: khac map -> ChangeWorld canh NPC mon phai cua
#    minh (khuon Da Tau DTB_TOI_NPC), cung map -> PB_WalkTo toi NPC (khuon
#    PB_AI_GOTO_FACTION), toi noi -> g_TongJX2.DoClientOpBody(COP_APPLY_JOIN, id bang
#    nguoi ru) = CUNG THAN sJX2_DoApplyJoin voi cua so client (bang chu duoc bao, du
#    cap tu nhan thi vao thang) -> nhan mat ket qua lai cho nguoi ru.
#  - To doi: moc trong KPlayerTeam::InviteAdd(int, cmd*) (server) -> PB_MoiVaoNhom:
#    bot khong co client de bam 'dong y' nen ghi m_nInviteList + GetInviteReply(..,1)
#    (khuon pb_GhepNhom). Chi nhan tu NGUOI THAT; tu choi khi dang Tong Kim / ban sap /
#    chet / dang o nhom co nguoi that khac (nhom toan bot thi bo nhom do ma sang).
#    Moi tick pb_DriveBot canh nguoi moi: doi map / thoat game / het chung nhom -> roi.
#    pb_QuanLyNhom (quan ly nhom bot) KHONG dung vao nhom co nguoi that (truoc day no
#    thay doi truong khong phai bot la 'doi truong khong con' -> roi nhom ngay lap tuc).
# Tep: KPlayerBot.h (1 hunk), KPlayerTeam.cpp (2 hunk, CRLF, co TCVN3 -> latin-1),
#      KPlayerBot.cpp (8 hunk). AP SAU goi_va_tkket3_moxe.py. Idempotent.
import io, sys

def ap_file(P, hunks):
    s = io.open(P, "r", encoding="latin-1", newline="").read()
    truoc = sum(1 for c in s if ord(c) > 127)
    crlf = "\r\n" in s
    n = 0
    for ten, cu, moi in hunks:
        if crlf:
            cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
        if moi in s:
            print("  [=] %s da ap tu truoc" % ten); continue
        if s.count(cu) != 1:
            print("LOI: neo %s (%s) khop %d cho (can 1)" % (ten, P, s.count(cu))); sys.exit(1)
        s = s.replace(cu, moi); n += 1
        print("  [+] %s" % ten)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    sau = sum(1 for c in s if ord(c) > 127)
    print("%s: %d hunk moi, high-byte %d -> %d (%s)" % (P.split("\\")[-1], n, truoc, sau,
          "OK" if truoc == sau else "LECH!"))

# =========================================================== KPlayerBot.h
ap_file(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.h", [
 ("H1 khai PB_MoiVaoNhom",
  'int  PB_WhisperReply(const PB_WHISPER* p);\n',
  'int  PB_WhisperReply(const PB_WHISPER* p);\n'
  '// [BOTNHOM-NGUOI 01/09] nguoi choi THAT moi bot vao to doi - goi tu KPlayerTeam::InviteAdd\n'
  '// (server). Tra 1 = bot da tu "dong y" va vao nhom (caller return luon); 0 = tu choi ->\n'
  '// roi xuong duong cu (bot khong bat m_bCanTeamFlag nen nguoi moi thay "khong the moi").\n'
  'int  PB_MoiVaoNhom(int nMoi, int nBot);\n'),
])

# =========================================================== KPlayerTeam.cpp (CRLF)
ap_file(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerTeam.cpp", [
 ("H2 include KPlayerBot.h (server)",
  '#include\t"KPlayerTeam.h"\n',
  '#include\t"KPlayerTeam.h"\n'
  '#ifdef _SERVER\n'
  '#include\t"LuaLib.h"       // KPlayerBot.h khai Lua_State\n'
  '#include\t"KPlayerBot.h"   // [BOTNHOM-NGUOI 01/09] PB_IsBot / PB_MoiVaoNhom\n'
  '#endif\n'),
 ("H3 moc InviteAdd -> bot tu dong y",
  '\tif (nTargetIdx == -1)\n'
  '\t\treturn;\n'
  '\tif (!Player[nTargetIdx].m_cTeam.m_bCanTeamFlag)\n',
  '\tif (nTargetIdx == -1)\n'
  '\t\treturn;\n'
  '\t// [BOTNHOM-NGUOI 01/09] (chu game) "bot duoc nguoi choi moi vao party - neu chua co\n'
  '\t// party thi vao party nguoi choi": bot khong co client de bam dong y, nen bot module\n'
  '\t// tu ghi m_nInviteList + GetInviteReply(...,1) ngay tai day (khuon pb_GhepNhom).\n'
  '\tif (PB_IsBot(nTargetIdx) && PB_MoiVaoNhom(nIdx, nTargetIdx))\n'
  '\t\treturn;\n'
  '\tif (!Player[nTargetIdx].m_cTeam.m_bCanTeamFlag)\n'),
])

# =========================================================== KPlayerBot.cpp (LF)
HELPERS = r'''
// ===========================================================================
// [BOTNHOM-NGUOI 01/09] (chu game) "bot duoc nguoi choi moi vao party - neu chua co
// party thi se vao party nguoi choi - khi nguoi choi doi map thi bot tu thoat party".
// [BOTBANG 01/09] (chu game) "nguoi choi NHAN MAT voi bot ru vao bang hoi thi bot ve
// NPC mon phai, chon dung bang cua nguoi ru de xin vao".
// ===========================================================================
static PB_Bot* pb_BotCua(int nPlayerIdx)
{
	for (int i = 0; i < s_botCount; i++)
		if (s_bots[i].nPlayerIdx == nPlayerIdx)
			return &s_bots[i];
	return NULL;
}

// Nhom cua nIdx co it nhat mot NGUOI THAT (co client) khong? Nhom toan bot -> 0.
static int pb_NhomCoNguoiThat(int nIdx)
{
	if (!pb_TrongNhom(nIdx))
		return 0;
	const KTeam& t = g_Team[Player[nIdx].m_cTeam.m_nID];
	const int c = t.m_nCaptain;
	if (c > 0 && c < MAX_PLAYER && Player[c].m_nNetConnectIdx >= 0)
		return 1;
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		const int m = t.m_nMember[i];
		if (m > 0 && m < MAX_PLAYER && Player[m].m_nNetConnectIdx >= 0)
			return 1;
	}
	return 0;
}

int PB_MoiVaoNhom(int nMoi, int nBot)
{
	if (nMoi <= 0 || nMoi >= MAX_PLAYER || nBot <= 0 || nBot >= MAX_PLAYER || nMoi == nBot)
		return 0;
	if (!PB_IsBot(nBot) || PB_IsBot(nMoi))
		return 0;                          // bot moi bot da co duong rieng (pb_GhepNhom)
	if (Player[nMoi].m_nNetConnectIdx < 0)
		return 0;                          // chi nhan loi moi cua NGUOI THAT
	PB_Bot* pB = pb_BotCua(nBot);
	const int nNpcB = Player[nBot].m_nIndex;
	if (!pB || nNpcB <= 0 || nNpcB >= MAX_NPC)
		return 0;
	if (pB->nTk || pB->nBanSap
	 || Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive)
	{
		pb_Log("[BotNhomNguoi] %s tu choi loi moi cua %s (dang Tong Kim / ban sap / chet)\n",
		       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName);
		return 0;
	}
	// "neu chua co party": dang o nhom co NGUOI THAT khac -> tu choi (khong bo nguoi ta);
	// nhom toan bot -> roi nhom do ma sang (nguoi that uu tien hon bot).
	if (pb_NhomCoNguoiThat(nBot) && pb_DoiTruongCua(nBot) != nMoi)
	{
		pb_Log("[BotNhomNguoi] %s tu choi %s: dang trong nhom cua nguoi choi khac\n",
		       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName);
		return 0;
	}
	KPlayerTeam& ta = Player[nMoi].m_cTeam;
	if (!ta.m_nFlag || ta.m_nFigure != TEAM_CAPTAIN || ta.m_nID < 0 || ta.m_nID >= MAX_TEAM)
		return 0;
	if (g_Team[ta.m_nID].m_nMemNum >= MAX_TEAM_MEMBER
	 || g_Team[ta.m_nID].m_nMemNum >= g_Team[ta.m_nID].CalcCaptainPower())
		return 0;                          // nhom day -> de duong cu bao cho nguoi moi
	if (pb_TrongNhom(nBot) && Player[nBot].m_cTeam.m_nID != ta.m_nID)
		pb_RoiNhom(nBot, "nguoi choi that moi vao nhom");
	// "bam dong y" ho: ghi vao danh sach moi roi GetInviteReply(.., 1) (KPlayerTeam.cpp:414)
	Player[nBot].m_cTeam.SetCanTeamFlag(nBot, TRUE);
	ta.m_nInviteList[ta.m_nListPos] = nBot;
	ta.m_nListPos = (ta.m_nListPos + 1) % MAX_TEAM_MEMBER;
	ta.GetInviteReply(nMoi, nBot, 1);
	const int bOk = (Player[nBot].m_cTeam.m_nFlag != 0 && Player[nBot].m_cTeam.m_nID == ta.m_nID);
	if (!bOk)
	{
		pb_Log("[BotNhomNguoi] %s VAO NHOM %s THAT BAI (GetInviteReply tu choi: mem=%d/tran=%d, mo=%d)\n",
		       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName,
		       g_Team[ta.m_nID].m_nMemNum, g_Team[ta.m_nID].CalcCaptainPower(),
		       (int)g_Team[ta.m_nID].IsOpen());
		return 0;
	}
	pB->nNhomNguoiIdx = nMoi;
	pB->dwNhomNguoiID = Player[nMoi].m_dwID;
	pb_Log("[BotNhomNguoi] %s vao nhom cua nguoi choi %s (%d thanh vien)\n",
	       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, g_Team[ta.m_nID].m_nMemNum);
	{
		const char* szOk = "Ok, minh vao nhom nhe!";
		pb_GuiChatMat(nMoi, nBot, szOk, (int)strlen(szOk));
	}
	return 1;
}

// [BOTBANG] may trang thai xin vao bang. Tra 1 = dang ban (caller return), 0 = xong/huy.
static int pb_XinVaoBang(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, unsigned int now, int nLech)
{
	if (b.nBangTick == 0)
		b.nBangTick = now;
	const int nAi = b.nBangNguoiIdx;
	const int bNguoiCon = (nAi > 0 && nAi < MAX_PLAYER && Player[nAi].m_dwID == b.dwBangNguoiID
	                    && Player[nAi].m_nIndex > 0);
	const int bDaCoBang = (Player[nIdx].m_cTong.m_nFlag != 0);
	const int bBangMat  = (g_TongJX2.FindTong(b.dwBangID) == NULL);
	if (now - b.nBangTick > (unsigned int)(GAME_FPS * 360) || bDaCoBang || bBangMat)
	{
		pb_Log("[BotBang] %s HUY xin vao bang %u (het han %d / da co bang %d / bang mat %d)\n",
		       Player[nIdx].m_PlayerName, (unsigned int)b.dwBangID,
		       (int)(now - b.nBangTick > (unsigned int)(GAME_FPS * 360)), bDaCoBang, bBangMat);
		if (bNguoiCon && !bDaCoBang)
		{
			const char* szHuy = "Minh khong toi duoc NPC mon phai, de khi khac nhe.";
			pb_GuiChatMat(nAi, nIdx, szHuy, (int)strlen(szHuy));
		}
		b.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;
		b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;
		b.walk.Reset();
		return 0;
	}
	// NPC mon phai CUA MINH; chua ro phai (blob mau) thi lay Thieu Lam - ca 10 NPC deu o
	// map 53 canh nhau nen "ve NPC mon phai" van dung nghia.
	int nFac = b.nFaction;
	if (nFac < 0 || nFac >= MAX_FACTION)
		nFac = (int)Player[nIdx].m_cFaction.m_nCurFaction;
	if (nFac < 0 || nFac >= MAX_FACTION)
		nFac = 0;
	const int nFacNpc = pb_FindFacNpc(nFac);
	if (nFacNpc <= 0)
		return 1;                          // NPC chua co trong the gioi - cho (het han thi huy)
	const int nSubNpc = Npc[nFacNpc].m_SubWorldIndex;
	if (nSubNpc < 0 || nSubNpc >= MAX_SUBWORLD)
		return 1;
	int nx = 0, ny = 0;
	Npc[nFacNpc].GetMpsPos(&nx, &ny);
	if (nSubNpc != nSub)
	{
		// khac map -> dich chuyen canh NPC (so le 3 giay, roi nhom truoc - khuon Da Tau)
		if (b.nBangDoiMapTick && now - b.nBangDoiMapTick < (unsigned int)(GAME_FPS * 3))
			return 1;
		b.nBangDoiMapTick = now;
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "ve NPC mon phai xin vao bang");
		b.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;
		int vx = 0, vy = 0;
		if (pb_ODat(nSubNpc, nx / 32, ny / 32, nLech, 12, &vx, &vy))
		{
			const int nRet = Npc[nNpcIdx].ChangeWorld(SubWorld[nSubNpc].m_SubWorldID, vx, vy);
			pb_Log("[BotBang] %s dich chuyen ve map %d canh NPC %s de xin vao bang (ret=%d)\n",
			       Player[nIdx].m_PlayerName, SubWorld[nSubNpc].m_SubWorldID,
			       s_facNpc[nFac].szTen, nRet);
		}
		b.walk.Reset();
		return 1;
	}
	// cung map -> di bo toi NPC (khuon PB_AI_GOTO_FACTION)
	if (b.nBangNghiToi && now < b.nBangNghiToi)
		return 1;
	const int nW = PB_WalkTo(nNpcIdx, nx, ny, nSub, b.walk, PB_FAC_ARRIVE_MPS);
	if (nW == 0)
		return 1;
	if (nW < 0)
	{
		b.walk.Reset();
		b.nBangNghiToi = now + (unsigned int)PB_FAC_RETRY_TICK;
		if (++b.nBangThu >= PB_FAC_MAX_RETRY)
		{
			// khong co duong -> dat canh NPC (khuon Da Tau KPlayerBot.cpp DTB_TOI_NPC)
			int dx3 = 0, dy3 = 0;
			if (pb_ODat(nSub, nx / 32, ny / 32, nLech, 6, &dx3, &dy3))
				Npc[nNpcIdx].SetPos(dx3, dy3);
			b.nBangThu = 0;
		}
		return 1;
	}
	// ---- TOI NOI: nop don xin vao DUNG bang cua nguoi ru ----
	Npc[nNpcIdx].SendCommand(do_stand);
	TONG_JX2OP_COMMAND sCmd;
	memset(&sCmd, 0, sizeof(sCmd));
	sCmd.m_btOp = defTONG_JX2_COP_APPLY_JOIN;
	sCmd.m_dwTarget = b.dwBangID;
	const int nKq = g_TongJX2.DoClientOpBody(nIdx, &sCmd);
	const KTongJX2Tong* pT = g_TongJX2.FindTong(b.dwBangID);
	const char* szTen = (pT && pT->szName[0]) ? pT->szName : "cua ban";
	char szMsg[200];
	if (nKq == 7)
		sprintf(szMsg, "Minh da gia nhap bang %s roi, cam on ban!", szTen);
	else if (nKq == 0)
		sprintf(szMsg, "Minh da nop don xin vao bang %s, ban nho nhac bang chu duyet nhe.", szTen);
	else if (nKq == 12)
		sprintf(szMsg, "Minh da nop don vao bang %s tu truoc roi, dang cho duyet.", szTen);
	else if (nKq == 11)
		sprintf(szMsg, "Bang %s dat nguong cap cao hon cap cua minh, chua xin duoc.", szTen);
	else
		sprintf(szMsg, "Minh xin vao bang %s khong duoc (ma %d).", szTen, nKq);
	pb_Log("[BotBang] %s nop don vao bang %s (id %u) tai NPC %s: ket qua=%d\n",
	       Player[nIdx].m_PlayerName, szTen, (unsigned int)b.dwBangID, s_facNpc[nFac].szTen, nKq);
	if (bNguoiCon)
		pb_GuiChatMat(nAi, nIdx, szMsg, (int)strlen(szMsg));
	b.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;
	b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;
	b.walk.Reset();
	return 0;
}

int PB_WhisperReply(const PB_WHISPER* p)
{
'''

WHISPER_CU = (
 '\t// ---- bat y dinh ----\n'
 '\t// Thu tu co chu y: "bot" xet TRUOC (nguoi choi hay thu "ban la bot ha"), roi cac cau\n'
 '\t// hoi cu the, cuoi cung moi toi chao hoi chung chung.\n'
 '\tif (strstr(szLow, "bot ") || strstr(szLow, " bot") || strstr(szLow, "auto"))\n')
WHISPER_MOI = (
 '\t// [BOTBANG 01/09] (chu game) "nguoi choi NHAN MAT voi bot ru vao bang hoi thi bot ve\n'
 '\t// NPC mon phai xin, chon dung bang cua nguoi ru". Ham nay CHI nhan whisper (PB_WHISPER\n'
 '\t// tu GameServer) nen dung nghia "nhan mat" - chat kenh/gan khong toi day. Tu khoa\n'
 '\t// ASCII "bang" (vao bang / gia nhap bang / bang hoi); "bang" dau (bang gia, bang nhau)\n'
 '\t// mang byte TCVN3 khac nen khong dinh. Nguoi ru PHAI dang co bang.\n'
 '\tint bBangHoi = 0;\n'
 '\tif (pB && strstr(szLow, "bang"))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n'
 '\t\tconst DWORD dwNg = Player[p->nSenderIdx].m_cTong.m_nFlag\n'
 '\t\t                 ? Player[p->nSenderIdx].m_cTong.GetTongNameID() : 0;\n'
 '\t\tconst KTongJX2Tong* pTg = dwNg ? g_TongJX2.FindTong(dwNg) : NULL;\n'
 '\t\tif (Player[nBot].m_cTong.m_nFlag)\n'
 '\t\t\tsprintf(szTraLoi, "Minh co bang hoi roi, cam on ban nhe.");\n'
 '\t\telse if (!pTg)\n'
 '\t\t\tsprintf(szTraLoi, "Ban chua co bang hoi ma, ru minh vao dau?");\n'
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tpB->nBangPha = 1;  pB->dwBangID = dwNg;\n'
 '\t\t\tpB->nBangNguoiIdx = p->nSenderIdx;  pB->dwBangNguoiID = Player[p->nSenderIdx].m_dwID;\n'
 '\t\t\tpB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;\n'
 '\t\t\tsprintf(szTraLoi, "Ok! Minh ve NPC mon phai xin vao bang %s ngay day.", pTg->szName);\n'
 '\t\t\tpb_Log("[BotBang] %s nhan PM ru vao bang %s (id %u) tu %s%s\\n",\n'
 '\t\t\t       Player[nBot].m_PlayerName, pTg->szName, (unsigned int)dwNg,\n'
 '\t\t\t       Player[p->nSenderIdx].m_PlayerName,\n'
 '\t\t\t       pB->nTk ? " - dang Tong Kim, xong tran moi di" : "");\n'
 '\t\t}\n'
 '\t\tpB->nPmCamToi = 0;                 // cau nay quan trong - khong "lam ngo" theo han cam\n'
 '\t}\n'
 '\t// ---- bat y dinh ----\n'
 '\t// Thu tu co chu y: "bot" xet TRUOC (nguoi choi hay thu "ban la bot ha"), roi cac cau\n'
 '\t// hoi cu the, cuoi cung moi toi chao hoi chung chung.\n'
 '\tif (bBangHoi)\n'
 '\t{\n'
 '\t\t// cau tra loi da soan o khoi [BOTBANG] ben tren\n'
 '\t}\n'
 '\telse if (strstr(szLow, "bot ") || strstr(szLow, " bot") || strstr(szLow, "auto"))\n')

CANH_NHOM = (
 '\t// PM da hen gio thi gui - chay truoc cac nhanh return nhu chat\n'
 '\tpb_XuLyPmCho(nIdx, b, nowAll);\n'
 '\t// [BOTNHOM-NGUOI 01/09] dang trong nhom NGUOI CHOI THAT: ho doi map -> tu roi nhom;\n'
 '\t// ho thoat game / kick / giai tan -> thoi canh. Chay truoc moi nhanh return.\n'
 '\tif (b.nNhomNguoiIdx > 0)\n'
 '\t{\n'
 '\t\tconst int nM = b.nNhomNguoiIdx;\n'
 '\t\tint bXoa = 0;\n'
 '\t\tif (nM >= MAX_PLAYER || Player[nM].m_dwID != b.dwNhomNguoiID || Player[nM].m_nIndex <= 0)\n'
 '\t\t\tbXoa = 1;                      // nguoi moi da thoat game\n'
 '\t\telse if (!pb_TrongNhom(nIdx) || !Player[nM].m_cTeam.m_nFlag\n'
 '\t\t      || Player[nM].m_cTeam.m_nID != Player[nIdx].m_cTeam.m_nID)\n'
 '\t\t\tbXoa = 1;                      // khong con chung nhom (kick / giai tan / ho roi)\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tconst int nNpcM = Player[nM].m_nIndex;\n'
 '\t\t\tif (nNpcM > 0 && nNpcM < MAX_NPC && Npc[nNpcM].m_SubWorldIndex != nSub)\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "nguoi choi da doi map");\n'
 '\t\t\t\tconst char* szBye = "Ban doi map roi, minh roi nhom nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nM, nIdx, szBye, (int)strlen(szBye));\n'
 '\t\t\t\tbXoa = 1;\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\tif (bXoa)\n'
 '\t\t{\n'
 '\t\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;\n'
 '\t\t}\n'
 '\t}\n')

ap_file(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp", [
 ("H4 include KTongJX2.h",
  '#include "KPlayerBot.h"\n',
  '#include "KPlayerBot.h"\n'
  '#include "KTongJX2.h"      // [BOTBANG 01/09] g_TongJX2 - bot xin vao bang hoi theo PM\n'),
 ("H5 truong PB_Bot",
  '\tunsigned int nPmLapHash;                  // hash cau tra loi truoc (chong lap y het)\n',
  '\tunsigned int nPmLapHash;                  // hash cau tra loi truoc (chong lap y het)\n'
  '\t// ---- [BOTNHOM-NGUOI 01/09] vao nhom cua nguoi choi that ----\n'
  '\tint          nNhomNguoiIdx;               // Player idx nguoi that da moi (0 = khong)\n'
  '\tDWORD        dwNhomNguoiID;               // chot danh tinh nguoi moi\n'
  '\t// ---- [BOTBANG 01/09] xin vao bang hoi theo nhan mat ----\n'
  '\tint          nBangPha;                    // 0 khong; 1 dang ve NPC mon phai de nop don\n'
  '\tDWORD        dwBangID;                    // NameID bang cua nguoi ru\n'
  '\tint          nBangNguoiIdx;               // nguoi ru (bao ket qua)\n'
  '\tDWORD        dwBangNguoiID;\n'
  '\tunsigned int nBangTick;                   // moc bat dau (het han 6 phut)\n'
  '\tunsigned int nBangNghiToi;                // A* thua -> nghi toi moc nay\n'
  '\tunsigned int nBangDoiMapTick;             // gion nhip ChangeWorld 3 giay\n'
  '\tint          nBangThu;                    // so lan A* thua lien tiep\n'),
 ("H6 khoi tao truong",
  '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n',
  '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n'
  '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;\n'
  '\t\tb.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;\n'
  '\t\tb.nBangTick = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;  b.nBangThu = 0;\n'),
 ("H7 ham PB_MoiVaoNhom + pb_XinVaoBang",
  '\nint PB_WhisperReply(const PB_WHISPER* p)\n{\n',
  HELPERS),
 ("H8 bat y dinh ru vao bang trong PM",
  WHISPER_CU, WHISPER_MOI),
 ("H8b cau tra loi 'nhom' cu",
  '\t\tsprintf(szTraLoi, "Minh dang di mot minh, de khi khac nhe.");\n',
  '\t\tsprintf(szTraLoi, "Ban cu moi minh vao nhom di, minh dong y lien.");\n'),
 ("H9 canh nguoi choi doi map",
  '\t// PM da hen gio thi gui - chay truoc cac nhanh return nhu chat\n'
  '\tpb_XuLyPmCho(nIdx, b, nowAll);\n',
  CANH_NHOM),
 ("H10 nhanh xin vao bang trong pb_DriveBot",
  '\t// ---------------------------------------------------------------- NGOI BAN SAP\n',
  '\t// ---------------------------------------------------------------- XIN VAO BANG HOI\n'
  '\t// [BOTBANG 01/09] sau Tong Kim (xong tran moi di), truoc ban sap / ve thanh / danh.\n'
  '\tif (b.nBangPha)\n'
  '\t{\n'
  '\t\tif (pb_XinVaoBang(nIdx, nNpcIdx, nSub, b, nowAll, (int)(&b - s_bots)))\n'
  '\t\t\treturn;\n'
  '\t}\n'
  '\n'
  '\t// ---------------------------------------------------------------- NGOI BAN SAP\n'),
 ("H11 pb_QuanLyNhom khong dung nhom co nguoi that",
  '\t\tif (pb_TrongNhom(p))\n'
  '\t\t{\n'
  '\t\t\tnDangNhom++;\n'
  '\t\t\tif (bXao)\n',
  '\t\tif (pb_TrongNhom(p))\n'
  '\t\t{\n'
  '\t\t\t// [BOTNHOM-NGUOI 01/09] nhom co NGUOI THAT (nguoi choi moi bot): quan ly nhom bot\n'
  '\t\t\t// KHONG dung vao (khong xao, khong roi vi "doi truong khong phai bot") - viec roi\n'
  '\t\t\t// khi nguoi choi doi map do pb_DriveBot lo.\n'
  '\t\t\tif (pb_NhomCoNguoiThat(p))\n'
  '\t\t\t{\n'
  '\t\t\t\tnDangNhom++;\n'
  '\t\t\t\tcontinue;\n'
  '\t\t\t}\n'
  '\t\t\tnDangNhom++;\n'
  '\t\t\tif (bXao)\n'),
])
print("XONG.")
