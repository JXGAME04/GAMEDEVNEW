# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_dich_server_0309.py - bo va idempotent (03-04/09/2026, DOT 5) cho WAuto Tong Kim:
VI TRI DICH TU MAY CHU. Chu game: "khong co cach nao xac dinh vi tri dich o trong map a? kieu nhu bot
xac dinh dich bang mau ten" -> "oke hay lam - va phan bien de co the tim thay dich va khong ton nhieu
tai nguyen".

Client chi thay NPC trong MAX_SYNC_RANGE 40 o (KNpc.cpp:763); bot phia may chu tim dich bang cach quet
Player[] theo camp (KPlayerBot.cpp pb_TkTimDichGanNhat). Lam theo TIEN LE danh ba sap [SapMap] (r5e):
client gui goi c2s_playerneedcount san co voi dwId dac biet, may chu tra loi bang tin He Thong rieng
nguoi hoi (mgs2player_from_c.lua -> Msg2Player) - KHONG goi moi, KHONG doi struct, hai ben lech ban
khong sao (client cu khong hoi; server cu khong tra -> client roi ve rao map nhu cu).

  KDaTauCap.h         : TK_DICH_ID / TK_DICH_MAP / TK_DICH_SO + extern g_szTKDich, g_uTKDichSeq
  KPlayer.cpp         : dinh nghia 2 bien (compile ca hai phia, vo hai)
  KNpcSet.cpp         : SetID bo qua TK_DICH_ID (khong cap cho npc that)
  KProtocolProcess.cpp: SERVER c2sNeedCount: dwId == TK_DICH_ID -> quet Player[] cung map 379, camp
                        doi phuong, con song, ngoai hau doanh (R 1440 mps) -> TK_DICH_SO dich gan nhat
                        "[TKDich] id:x:y ..." (o), chan 5 giay/nguoi theo chu khe.
                        CLIENT s2c chat: "[TKDich]" -> g_szTKDich + seq, an khoi khung chat.
  CoreShell.cpp       : TK_DichNhan (moi nhip TK_Process), TK_DichHoi (5,5 s), TK_DichXa - xen vao
                        TKP_FIGHT truoc TK_RaoDi: chon dich gan nhat co FindPath == 1, di toi; toi tam PK
                        ma khong thay -> bo muc, hoi lai; ket 3 s -> bo; tra loi cu > 15 s -> ve rao.
Chay:  python goi_va_wauto_tk_dich_server_0309.py [--thu] [--root <Sources\\Core\\Src>]
Can build CA HAI: CoreServer.dll (Server Release x64) + CoreClient.dll (Client Release Win32).
"""
import io, sys, os

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
if "--root" in sys.argv:
    ROOT = sys.argv[sys.argv.index("--root") + 1]
THU = "--thu" in sys.argv

_TV = ("b5:à b6:ả b7:ã b8:á b9:ạ a8:ă bb:ằ bc:ẳ bd:ẵ be:ắ c6:ặ a9:â c7:ầ c8:ẩ c9:ẫ ca:ấ cb:ậ a7:Đ ae:đ "
       "cc:è ce:ẻ cf:ẽ d0:é d1:ẹ aa:ê d2:ề d3:ể d4:ễ d5:ế d6:ệ d7:ì d8:ỉ dc:ĩ dd:í de:ị df:ò e1:ỏ e2:õ e3:ó e4:ọ "
       "ab:ô e5:ồ e6:ổ e7:ỗ e8:ố e9:ộ ac:ơ ea:ờ eb:ở ec:ỡ ed:ớ ee:ợ ef:ù f1:ủ f2:ũ f3:ú f4:ụ ad:ư f5:ừ f6:ử f7:ữ "
       "f8:ứ f9:ự fa:ỳ fb:ỷ fc:ỹ fd:ý fe:ỵ a1:Ă a2:Â a3:Ê a4:Ô a5:Ơ a6:Ư")
_U2T = {v: chr(int(k, 16)) for k, v in (x.split(":") for x in _TV.split())}

def tcvn3(s):
    out = []
    for ch in s:
        if ord(ch) < 0x80:
            out.append(ch)
        elif ch in _U2T:
            out.append(_U2T[ch])
        else:
            raise SystemExit("khong ma hoa duoc TCVN3: %r trong %r" % (ch, s))
    return "".join(out)

def rd(path):
    return io.open(path, "r", encoding="latin-1", newline="").read()

class Tep(object):
    def __init__(self, path):
        self.path = path
        self.s = rd(path)
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []
    def N(self, text):
        return text.replace("\r\n", "\n").replace("\n", self.nl)
    def them_sau(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        if self.s.count(neo) != 1:
            raise SystemExit("%s: neo khong duy nhat (%d): %r" % (os.path.basename(self.path), self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, neo + moi, 1)
        self.log.append("  them sau: %s" % dau[:60])
    def them_truoc(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        if self.s.count(neo) != 1:
            raise SystemExit("%s: neo khong duy nhat (%d): %r" % (os.path.basename(self.path), self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, moi + neo, 1)
        self.log.append("  them truoc: %s" % dau[:60])
    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        n = self.s.count(cu)
        if n != 1:
            raise SystemExit("%s: doan can thay khong duy nhat (%d): %r" % (os.path.basename(self.path), n, cu[:90]))
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau[:60])
    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return
        hb_goc = sum(1 for c in self.goc if ord(c) >= 0x80)
        hb_moi = sum(1 for c in self.s if ord(c) >= 0x80)
        print("%s: %d dong log, byte cao %d -> %d" % (os.path.basename(self.path), len(self.log), hb_goc, hb_moi))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)

# =====================================================================
# 1. KDaTauCap.h
# =====================================================================
h = Tep(os.path.join(ROOT, "KDaTauCap.h"))
h.them_sau(
    "#define DATAU_SAPMAP_MAXLEN 200\n",
    "// (04/09 WAuto Tong Kim) id DAT CHO goi xin VI TRI DICH ca map tran (cung goi c2s_playerneedcount).\n"
    "// Dung o: KNpcSet::SetID (bo qua), KProtocolProcess::c2sNeedCount (server tra \"[TKDich] id:x:y ...\"\n"
    "// toa do O, TK_DICH_SO dich khac camp gan nhat con song ngoai hau doanh), KProtocolProcess s2c chat\n"
    "// (client chup vao g_szTKDich), CoreShell TK_DichXa (client hoi 5,5 giay/lan khi khong thay dich).\n"
    "#define TK_DICH_ID		0x7D1C0A11u\n"
    "#define TK_DICH_MAP		379		// map tran Tong Kim (lib_tktc.lua MAP_TK_TC)\n"
    "#define TK_DICH_SO		6		// so dich gan nhat tra ve moi lan\n",
    "#define TK_DICH_ID")
h.them_truoc(
    "#endif // KDATAUCAP_H\n",
    "// (04/09) vi tri dich Tong Kim server tra ve - ngoai struct de bo cuc KDaTauCapture khong doi\n"
    "extern char g_szTKDich[256];\n"
    "extern unsigned int g_uTKDichSeq;\n"
    "\n",
    "extern char g_szTKDich[256];")
h.ghi()

# =====================================================================
# 2. KPlayer.cpp - dinh nghia bien
# =====================================================================
kp = Tep(os.path.join(ROOT, "KPlayer.cpp"))
kp.them_sau(
    "KDaTauCapture g_sDTCap; // bo dem chup hoi thoai cho auto Da Tau (khai bao KDaTauCap.h)\n",
    "char g_szTKDich[256] = { 0 };	// (04/09) \"[TKDich] ...\" vi tri dich Tong Kim server tra ve (KDaTauCap.h)\n"
    "unsigned int g_uTKDichSeq = 0;\n",
    "char g_szTKDich[256] = { 0 };")
kp.ghi()

# =====================================================================
# 3. KNpcSet.cpp - dat cho id
# =====================================================================
ns = Tep(os.path.join(ROOT, "KNpcSet.cpp"))
ns.thay(
    "	if (m_dwIDCreator == DATAU_SAPMAP_ID)\n"
    "		m_dwIDCreator++;\n",
    "	if (m_dwIDCreator == DATAU_SAPMAP_ID || m_dwIDCreator == TK_DICH_ID)	// (04/09) + id xin vi tri dich Tong Kim\n"
    "		m_dwIDCreator++;\n",
    "m_dwIDCreator == TK_DICH_ID")
ns.ghi()

# =====================================================================
# 4. KProtocolProcess.cpp - server tra loi + client chup
# =====================================================================
pp = Tep(os.path.join(ROOT, "KProtocolProcess.cpp"))
pp.them_sau(
    "	PLAYER_NEED_COUNT *pView = (PLAYER_NEED_COUNT *)pProtocol;\n",
    "	// (04/09 WAuto Tong Kim) dwId dac biet = xin VI TRI DICH ca map tran. Client chi thay dich trong\n"
    "	// MAX_SYNC_RANGE 40 o; bot phia may chu tim dich bang cach quet Player[] theo camp (KPlayerBot.cpp\n"
    "	// pb_TkTimDichGanNhat) - lam y vay cho nguoi choi that dung WAuto. Tra \"[TKDich] id:x:y ...\" (o)\n"
    "	// TK_DICH_SO dich khac camp gan nhat con song, BO dich con trong hau doanh (R 1440 mps - trap vao\n"
    "	// trai chan, khong toi duoc), qua tin He Thong rieng nguoi hoi (mgs2player_from_c.lua - cung duong\n"
    "	// [SapMap]). Chi phi: mot vong MAX_PLAYER so sanh, toi da 5 giay/nguoi (chan theo chu khe nhu SapMap),\n"
    "	// tin ~110 byte. Chi tra khi nguoi hoi dang o map tran va da co camp 1/2.\n"
    "	if (pView->dwId == TK_DICH_ID)\n"
    "	{\n"
    "		static DWORD s_uTKDNext[MAX_PLAYER] = { 0 };\n"
    "		static DWORD s_uTKDChu[MAX_PLAYER] = { 0 };\n"
    "		const DWORD dwNayTD = SubWorld[0].m_dwCurrentTime;\n"
    "		if (s_uTKDChu[nIndex] != Player[nIndex].m_dwID)\n"
    "		{\n"
    "			s_uTKDNext[nIndex] = 0;\n"
    "			s_uTKDChu[nIndex] = Player[nIndex].m_dwID;\n"
    "		}\n"
    "		if (dwNayTD < s_uTKDNext[nIndex])\n"
    "			return;\n"
    "		s_uTKDNext[nIndex] = dwNayTD + GAME_FPS * 5;\n"
    "		const int nMeTD = Player[nIndex].m_nIndex;\n"
    "		const int nSubTD = Npc[nMeTD].m_SubWorldIndex;\n"
    "		if (nSubTD < 0 || nSubTD >= MAX_SUBWORLD || SubWorld[nSubTD].m_SubWorldID != TK_DICH_MAP)\n"
    "			return;\n"
    "		const int nCampToiTD = (int)Npc[nMeTD].m_CurrentCamp;\n"
    "		if (nCampToiTD != 1 && nCampToiTD != 2)\n"
    "			return;\n"
    "		const int nCampDichTD = 3 - nCampToiTD;\n"
    "		int mxTD = 0, myTD = 0;\n"
    "		Npc[nMeTD].GetMpsPos(&mxTD, &myTD);\n"
    "		// hau doanh hai phe (o) - khop KPlayerBot.cpp aZHx/aZHy va KTongKimTables.h g_TKHauDoanhA/B\n"
    "		static const int aTDHx[2] = { 1229, 1689 };\n"
    "		static const int aTDHy[2] = { 3561, 3074 };\n"
    "		DWORD aIdTD[TK_DICH_SO];\n"
    "		int aXTD[TK_DICH_SO], aYTD[TK_DICH_SO];\n"
    "		__int64 aDTD[TK_DICH_SO];\n"
    "		int nSoTD = 0;\n"
    "		for (int iTD = 1; iTD < MAX_PLAYER; ++iTD)\n"
    "		{\n"
    "			if (iTD == nIndex)\n"
    "				continue;\n"
    "			const int nn = Player[iTD].m_nIndex;\n"
    "			if (nn <= 0 || nn >= MAX_NPC || Npc[nn].m_dwID == 0)\n"
    "				continue;\n"
    "			if (Npc[nn].m_SubWorldIndex != nSubTD)\n"
    "				continue;\n"
    "			if ((int)Npc[nn].m_CurrentCamp != nCampDichTD)\n"
    "				continue;\n"
    "			if (Npc[nn].m_Doing == do_death || Npc[nn].m_Doing == do_revive || Npc[nn].m_CurrentLife <= 0)\n"
    "				continue;\n"
    "			int xTD = 0, yTD = 0;\n"
    "			Npc[nn].GetMpsPos(&xTD, &yTD);\n"
    "			int bTraiTD = 0;\n"
    "			for (int hTD = 0; hTD < 2; ++hTD)\n"
    "			{\n"
    "				const __int64 hx = xTD - aTDHx[hTD] * 32, hy = yTD - aTDHy[hTD] * 32;\n"
    "				if (hx * hx + hy * hy < (__int64)1440 * 1440)\n"
    "				{\n"
    "					bTraiTD = 1;\n"
    "					break;\n"
    "				}\n"
    "			}\n"
    "			if (bTraiTD)\n"
    "				continue;\n"
    "			const __int64 ddx = xTD - mxTD, ddy = yTD - myTD;\n"
    "			const __int64 dTD = ddx * ddx + ddy * ddy;\n"
    "			// chen vao mang nho da sap xep tang dan (TK_DICH_SO phan tu)\n"
    "			int pTD = nSoTD;\n"
    "			if (nSoTD < TK_DICH_SO)\n"
    "				++nSoTD;\n"
    "			else if (dTD >= aDTD[TK_DICH_SO - 1])\n"
    "				continue;\n"
    "			else\n"
    "				pTD = TK_DICH_SO - 1;\n"
    "			while (pTD > 0 && aDTD[pTD - 1] > dTD)\n"
    "			{\n"
    "				aDTD[pTD] = aDTD[pTD - 1];\n"
    "				aIdTD[pTD] = aIdTD[pTD - 1];\n"
    "				aXTD[pTD] = aXTD[pTD - 1];\n"
    "				aYTD[pTD] = aYTD[pTD - 1];\n"
    "				--pTD;\n"
    "			}\n"
    "			aDTD[pTD] = dTD;\n"
    "			aIdTD[pTD] = Npc[nn].m_dwID;\n"
    "			aXTD[pTD] = xTD;\n"
    "			aYTD[pTD] = yTD;\n"
    "		}\n"
    "		char szTD[DATAU_SAPMAP_MAXLEN + 48];\n"
    "		int nLenTD = sprintf(szTD, \"[TKDich]\");\n"
    "		for (int kTD = 0; kTD < nSoTD; ++kTD)\n"
    "		{\n"
    "			if (nLenTD > DATAU_SAPMAP_MAXLEN)\n"
    "				break;\n"
    "			nLenTD += sprintf(szTD + nLenTD, \" %u:%d:%d\", (unsigned int)aIdTD[kTD], aXTD[kTD] / 32, aYTD[kTD] / 32);\n"
    "		}\n"
    "		Player[nIndex].ExecuteScript(\"\\\\script\\\\player\\\\mgs2player_from_c.lua\", \"main\", szTD, false);\n"
    "		return;\n"
    "	}\n",
    "if (pView->dwId == TK_DICH_ID)")

pp.thay(
    "				++g_sDTCap.uSapMapSeq;\n"
    "			}\n"
    "			else if (nDTLen > 0)\n",
    "				++g_sDTCap.uSapMapSeq;\n"
    "			}\n"
    "			else if (nDTLen >= 8 && !memcmp((const char*)(pCccSync + 1), \"[TKDich]\", 8))\n"
    "			{	// (04/09) vi tri dich Tong Kim server tra ve - kenh du lieu rieng cho auto, an khoi khung chat\n"
    "				bDTSapMap = true;\n"
    "				int nLTD = nDTLen;\n"
    "				if (nLTD > (int)sizeof(g_szTKDich) - 1)\n"
    "					nLTD = (int)sizeof(g_szTKDich) - 1;\n"
    "				memcpy(g_szTKDich, (const char*)(pCccSync + 1), nLTD);\n"
    "				g_szTKDich[nLTD] = 0;\n"
    "				++g_uTKDichSeq;\n"
    "			}\n"
    "			else if (nDTLen > 0)\n",
    "\"[TKDich]\", 8))")
pp.ghi()

# =====================================================================
# 5. CoreShell.cpp - client hoi / nhan / di toi
# =====================================================================
f = Tep(os.path.join(ROOT, "CoreShell.cpp"))
MSG_DICH = tcvn3("<color=Cyan>Máy chủ báo địch cách %d ô - tiến tới.")
f.them_truoc(
    "// khong thay dich: di rao. Tra 1 = dang rao (da phat lenh di), 0 = khong co luoi.\n"
    "static int TK_RaoDi(int nPlayerIdx, int nX, int nY, UINT uCurTime)\n",
    "// ===== (04/09, dot 5) VI TRI DICH TU MAY CHU =====\n"
    "// Chu game: \"khong co cach nao xac dinh vi tri dich o trong map a? kieu nhu bot xac dinh dich bang\n"
    "// mau ten\". Client chi thay dich trong MAX_SYNC_RANGE 40 o nen HOI MAY CHU theo tien le danh ba sap\n"
    "// [SapMap]: gui c2s_playerneedcount voi TK_DICH_ID, may chu tra \"[TKDich] id:x:y ...\" (o) TK_DICH_SO\n"
    "// dich khac camp gan nhat con song ngoai hau doanh (KProtocolProcess.cpp c2sNeedCount), chan 5 giay\n"
    "// moi nguoi. Client: hoi moi TK_DICH_HOI_MS khi KHONG thay dich (TKP_FIGHT truoc TK_RaoDi), chon dich\n"
    "// gan nhat co FindPath == 1 roi di toi; toi tam PK ma van khong thay (no da doi cho) -> bo muc, hoi\n"
    "// lai; ket 3 giay -> bo; tra loi cu hon TK_DICH_CU_MS (server cu khong tra) -> ve cach cu (rao).\n"
    "// Tren duong gap dich: tang san / may PK cuop quyen (thu tu TKP_FIGHT); dich chet: server khong tra.\n"
    "#define TK_DICH_HOI_MS	5500		// ms: nhip hoi (server chan 5 giay/nguoi - GAME_FPS*5)\n"
    "#define TK_DICH_CU_MS	15000		// ms: tra loi cu hon the = khong dung\n"
    "static UINT  s_uTKDichSeen = 0;		// g_uTKDichSeq da xem\n"
    "static UINT  s_uTKDichHoiT = 0;		// moc duoc hoi lan tiep\n"
    "static UINT  s_uTKDichTraT = 0;		// moc nhan tra loi cuoi (0 = chua)\n"
    "static int   s_nTKDichSo = 0;\n"
    "static DWORD s_aTKDichID[TK_DICH_SO];\n"
    "static int   s_aTKDichX[TK_DICH_SO], s_aTKDichY[TK_DICH_SO];	// mps\n"
    "static int   s_aTKDichBo[TK_DICH_SO];	// 1 = da toi ma khong thay / ket / khong duong - bo\n"
    "static int   s_nTKDichChon = -1;\n"
    "static UINT  s_uTKDichTickT = 0;\n"
    "static UINT  s_uTKDichMsgT = 0;\n"
    "\n"
    "static void TK_DichHoi(UINT uCurTime)\n"
    "{\n"
    "	if (s_uTKDichHoiT && (int)(uCurTime - s_uTKDichHoiT) < 0)\n"
    "		return;\n"
    "	s_uTKDichHoiT = uCurTime + TK_DICH_HOI_MS;\n"
    "	SendClientCmdGetCount(TK_DICH_ID);\n"
    "}\n"
    "\n"
    "// goi MOI NHIP TK_Process de tra loi duoc chup dung luc den (vi tri co tuoi)\n"
    "static void TK_DichNhan(UINT uCurTime)\n"
    "{\n"
    "	if (g_uTKDichSeq == s_uTKDichSeen)\n"
    "		return;\n"
    "	s_uTKDichSeen = g_uTKDichSeq;\n"
    "	s_uTKDichTraT = uCurTime ? uCurTime : 1;\n"
    "	s_nTKDichSo = 0;\n"
    "	s_nTKDichChon = -1;\n"
    "	const char* p = g_szTKDich + 8;\n"
    "	while (s_nTKDichSo < TK_DICH_SO)\n"
    "	{\n"
    "		while (*p == ' ')\n"
    "			++p;\n"
    "		if (!*p)\n"
    "			break;\n"
    "		unsigned int id = 0;\n"
    "		int cx = 0, cy = 0;\n"
    "		if (sscanf(p, \"%u:%d:%d\", &id, &cx, &cy) != 3)\n"
    "			break;\n"
    "		s_aTKDichID[s_nTKDichSo] = (DWORD)id;\n"
    "		s_aTKDichX[s_nTKDichSo] = cx * 32 + 16;\n"
    "		s_aTKDichY[s_nTKDichSo] = cy * 32 + 16;\n"
    "		s_aTKDichBo[s_nTKDichSo] = 0;\n"
    "		++s_nTKDichSo;\n"
    "		while (*p && *p != ' ')\n"
    "			++p;\n"
    "	}\n"
    "	AUTOLOG(\"[TK-DICH] server tra %d dich: %s\", s_nTKDichSo, g_szTKDich);\n"
    "	if (s_nTKDichSo == 0)\n"
    "	{	// khong con dich ngoai hau doanh: quen diem cuoi thay dich, khong di khu xuat quan nua - rao\n"
    "		s_uTKRaoLastT = 0;\n"
    "		s_nTKXQDa = 1;\n"
    "	}\n"
    "}\n"
    "\n"
    "static int TK_DichChonMuc(int nPlayerIdx, int nX, int nY, long* pnD)\n"
    "{\n"
    "	int nBest = -1;\n"
    "	long nBestD = 0x7fffffff;\n"
    "	for (int i = 0; i < s_nTKDichSo; ++i)\n"
    "	{\n"
    "		if (s_aTKDichBo[i])\n"
    "			continue;\n"
    "		if (Player[nPlayerIdx].m_mAutoExcludeNpcID.find(s_aTKDichID[i]) != Player[nPlayerIdx].m_mAutoExcludeNpcID.end())\n"
    "			continue;\n"
    "		const long d = g_GetDistance(nX, nY, s_aTKDichX[i], s_aTKDichY[i]);\n"
    "		if (d < nBestD)\n"
    "		{\n"
    "			nBestD = d;\n"
    "			nBest = i;\n"
    "		}\n"
    "	}\n"
    "	if (pnD)\n"
    "		*pnD = nBestD;\n"
    "	return nBest;\n"
    "}\n"
    "\n"
    "// tra 1 = dang di toi dich do may chu bao; 0 = khong co du lieu / het muc -> ben goi rao nhu cu\n"
    "static int TK_DichXa(int nPlayerIdx, const autoData* pAp, int nX, int nY, UINT uCurTime)\n"
    "{\n"
    "	if ((int)(uCurTime - s_uTKDichTickT) > 1500)\n"
    "		s_nTKRaoKetMuc = 0;		// vua danh / san xong quay lai -> do ket lai tu dau\n"
    "	s_uTKDichTickT = uCurTime;\n"
    "	TK_DichHoi(uCurTime);\n"
    "	if (!s_uTKDichTraT || (int)(uCurTime - s_uTKDichTraT) > TK_DICH_CU_MS)\n"
    "		return 0;\n"
    "	int nTam = pAp->nPKVision;\n"
    "	if (nTam < 100)\n"
    "		nTam = 100;\n"
    "	else if (nTam > 1200)\n"
    "		nTam = 1200;\n"
    "	if (s_nTKDichChon >= 0 && s_nTKDichChon < s_nTKDichSo)\n"
    "	{\n"
    "		const int i = s_nTKDichChon;\n"
    "		const int tx = s_aTKDichX[i], ty = s_aTKDichY[i];\n"
    "		const int nKet = TK_RaoKet(4, nX, nY, uCurTime);\n"
    "		if (g_GetDistance(nX, nY, tx, ty) <= nTam || nKet)\n"
    "		{	// da vao tam PK ma tang danh / tang san khong thay (no da doi cho), hoac ket -> bo muc nay\n"
    "			s_aTKDichBo[i] = 1;\n"
    "			s_nTKDichChon = -1;\n"
    "			s_nTKRaoKetMuc = 0;\n"
    "			AUTOLOG(\"[TK-DICH] toi (%d,%d) khong thay id=%u (ket=%d) - doi muc / hoi lai. me=(%d,%d)\", tx, ty, (unsigned int)s_aTKDichID[i], nKet, nX, nY);\n"
    "			if (!nKet)\n"
    "				s_uTKDichHoiT = 0;		// hoi lai ngay (server van chan 5 giay - lan sau tu hoi tiep)\n"
    "		}\n"
    "		else\n"
    "		{\n"
    "			DT_WalkTo(nPlayerIdx, tx, ty, TK_O(4), uCurTime);\n"
    "			return 1;\n"
    "		}\n"
    "	}\n"
    "	long nD = 0;\n"
    "	int nBest = TK_DichChonMuc(nPlayerIdx, nX, nY, &nD);\n"
    "	while (nBest >= 0 && SubWorld[0].FindPath(s_aTKDichX[nBest], s_aTKDichY[nBest]) != 1)\n"
    "	{	// 2 = dich khong toi duoc (vung kin) - bo muc nay, chon muc khac (toi da TK_DICH_SO lan)\n"
    "		AUTOLOG(\"[TK-DICH] khong co duong toi id=%u (%d,%d) - bo\", (unsigned int)s_aTKDichID[nBest], s_aTKDichX[nBest], s_aTKDichY[nBest]);\n"
    "		s_aTKDichBo[nBest] = 1;\n"
    "		nBest = TK_DichChonMuc(nPlayerIdx, nX, nY, &nD);\n"
    "	}\n"
    "	if (nBest < 0)\n"
    "		return 0;\n"
    "	s_nTKDichChon = nBest;\n"
    "	s_nTKRaoKetMuc = 0;\n"
    "	if (!s_uTKDichMsgT || (int)(uCurTime - s_uTKDichMsgT) > 30000)\n"
    "	{\n"
    "		s_uTKDichMsgT = uCurTime ? uCurTime : 1;\n"
    "		char szDich[160];\n"
    "		sprintf(szDich, \"" + MSG_DICH + "\", (int)(nD / 32));\n"
    "		TK_Msg(nPlayerIdx, szDich);\n"
    "	}\n"
    "	AUTOLOG(\"[TK-DICH] toi dich id=%u (%d,%d) xa=%ld me=(%d,%d) muc %d/%d\", (unsigned int)s_aTKDichID[nBest], s_aTKDichX[nBest], s_aTKDichY[nBest], nD, nX, nY, nBest, s_nTKDichSo);\n"
    "	DT_WalkTo(nPlayerIdx, s_aTKDichX[nBest], s_aTKDichY[nBest], TK_O(4), uCurTime);\n"
    "	return 1;\n"
    "}\n"
    "// ===== HET VI TRI DICH TU MAY CHU =====\n"
    "\n",
    "static int TK_DichXa(int nPlayerIdx, const autoData* pAp, int nX, int nY, UINT uCurTime)")

# nhan tra loi moi nhip TK_Process
f.them_truoc(
    "	const int nLoaMo = (nTinMo || uCurTime < ea.uTKMoT);\n",
    "	TK_DichNhan(uCurTime);	// (04/09, dot 5) chup tra loi vi tri dich dung luc den\n",
    "TK_DichNhan(uCurTime);	// (04/09, dot 5)")

# TK_Pha: doi pha -> chon lai muc
f.them_sau(
    "	s_nTKXQDa = 0;		// (03/09 dem, dot 4) luot ra tran moi -> di lai khu xuat quan dich\n"
    "	s_nTKXQChon = -1;\n",
    "	s_nTKDichChon = -1;	// (04/09, dot 5) chon lai muc do may chu bao\n",
    "s_nTKDichChon = -1;	// (04/09, dot 5)")

# TKP_FIGHT: truoc rao
f.thay(
    "		ea.uNpcID = 0;\n"
    "		s_uTKSanQuyen = 0;\n"
    "		if (TK_RaoDi(nPlayerIdx, nX, nY, uCurTime))\n"
    "			return 2;\n",
    "		ea.uNpcID = 0;\n"
    "		s_uTKSanQuyen = 0;\n"
    "		// (04/09, dot 5) vi tri dich do MAY CHU bao (ngoai tam nhin client) - truoc rao map\n"
    "		if (TK_DichXa(nPlayerIdx, pAp, nX, nY, uCurTime))\n"
    "			return 2;\n"
    "		if (TK_RaoDi(nPlayerIdx, nX, nY, uCurTime))\n"
    "			return 2;\n",
    "if (TK_DichXa(nPlayerIdx, pAp, nX, nY, uCurTime))")
f.ghi()
