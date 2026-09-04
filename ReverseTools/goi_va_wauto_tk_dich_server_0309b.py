# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_dich_server_0309b.py - bo va bo sung (04/09, DOT 5b) SAU PHAN BIEN cua tac tu doc lap
cho bo va goi_va_wauto_tk_dich_server_0309.py (phai chay tool goc truoc). Sua 5 diem:
  2. TK_DichXa: tat o 'Danh nguoi' (bPKPlayer) -> khong dat di, khong hoi (tang danh khong nhan nguoi).
  3. FindPath tham do goi StopPath -> khi moi ung vien deu khong duong thi duong dang di bi huy va
     DT_WalkTo tang rao doi toi 2,5 s moi tinh lai -> RemoveFlag + ea.uDTPath = 0 ngay sau tham do
     that bai; FindPath < 0 (luoi dang nap) thi thu nhip sau, KHONG bo muc.
  4. Bien chan sat: server 90 khung (GAME_FPS*5) vs client 5,5 s -> server 4 giay, client 6 giay.
  5. Quan tinh chon muc: tin moi giu muc dang di neu con trong tin (tru khi muc khac gan hon >= 25%,
     luat TK_SanNguoi); nho id da bo 30 giay qua cac tin (khong chon lai ngay muc 'toi ma khong thay').
  6. TK_DICH_SO 6 -> 9 (moi muc <= 21 byte: 8 + 9*21 = 197 <= DATAU_SAPMAP_MAXLEN 200).
Diem 1 (loai ca hai vong hau doanh) GIU: dich khong the vao trong trai minh (trap vao trai chan),
vong 1440 mps chi phu loi trai, cong trai cach tam 132 o - khong mat dich nao. Diem 7/8 = thong tin.
Chay:  python goi_va_wauto_tk_dich_server_0309b.py [--thu] [--root <Sources\\Core\\Src>]
"""
import io, sys, os

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
if "--root" in sys.argv:
    ROOT = sys.argv[sys.argv.index("--root") + 1]
THU = "--thu" in sys.argv

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

# ---- 6. KDaTauCap.h: 6 -> 9 muc ----
h = Tep(os.path.join(ROOT, "KDaTauCap.h"))
h.thay(
    "#define TK_DICH_SO		6		// so dich gan nhat tra ve moi lan\n",
    "#define TK_DICH_SO		9		// so dich gan nhat tra ve moi lan (phan bien 04/09: 6 -> 9; moi muc <= 21 byte, 8 + 9*21 = 197 <= 200)\n",
    "#define TK_DICH_SO		9")
h.ghi()

# ---- 4. server: chan 4 giay ----
pp = Tep(os.path.join(ROOT, "KProtocolProcess.cpp"))
pp.thay(
    "		s_uTKDNext[nIndex] = dwNayTD + GAME_FPS * 5;\n",
    "		s_uTKDNext[nIndex] = dwNayTD + GAME_FPS * 4;	// (phan bien 04/09) 4 giay: client hoi 6 giay/lan, may chu cham nhip van khong vut goi\n",
    "dwNayTD + GAME_FPS * 4;")
pp.ghi()

# ---- CoreShell.cpp ----
f = Tep(os.path.join(ROOT, "CoreShell.cpp"))
# 4. client 6 giay
f.thay(
    "#define TK_DICH_HOI_MS	5500		// ms: nhip hoi (server chan 5 giay/nguoi - GAME_FPS*5)\n",
    "#define TK_DICH_HOI_MS	6000		// ms: nhip hoi (server chan 4 giay/nguoi - GAME_FPS*4; phan bien: 5,5 s sat 90 khung khi may chu cham nhip)\n",
    "#define TK_DICH_HOI_MS	6000")

# 5. quan tinh + nho muc da bo
f.them_sau(
    "static UINT  s_uTKDichMsgT = 0;\n",
    "// (04/09 phan bien) quan tinh chon muc + nho muc da bo qua cac tin: khong dao chieu moi 6 giay giua hai\n"
    "// dich cach deu, khong chon lai ngay muc 'toi ma khong thay' o tin sau.\n"
    "#define TK_DICH_BO_SO	8\n"
    "#define TK_DICH_BO_MS	30000\n"
    "static DWORD s_uTKDichChonID = 0;	// id muc dang di (0 = chua)\n"
    "static DWORD s_aTKDichBoID[TK_DICH_BO_SO];\n"
    "static UINT  s_uTKDichBoHan[TK_DICH_BO_SO];\n"
    "static void TK_DichBoNho(DWORD id, UINT uCurTime)\n"
    "{\n"
    "	int nKhe = 0;\n"
    "	for (int i = 0; i < TK_DICH_BO_SO; ++i)\n"
    "	{\n"
    "		if (s_aTKDichBoID[i] == id || !s_aTKDichBoID[i] || (int)(uCurTime - s_uTKDichBoHan[i]) >= 0)\n"
    "		{\n"
    "			nKhe = i;\n"
    "			break;\n"
    "		}\n"
    "		if ((int)(s_uTKDichBoHan[i] - s_uTKDichBoHan[nKhe]) < 0)\n"
    "			nKhe = i;		// khong con khe trong -> de len khe het han som nhat\n"
    "	}\n"
    "	s_aTKDichBoID[nKhe] = id;\n"
    "	s_uTKDichBoHan[nKhe] = uCurTime + TK_DICH_BO_MS;\n"
    "}\n"
    "static int TK_DichDaBo(DWORD id, UINT uCurTime)\n"
    "{\n"
    "	for (int i = 0; i < TK_DICH_BO_SO; ++i)\n"
    "		if (s_aTKDichBoID[i] == id && (int)(uCurTime - s_uTKDichBoHan[i]) < 0)\n"
    "			return 1;\n"
    "	return 0;\n"
    "}\n",
    "static DWORD s_uTKDichChonID = 0;")

f.thay(
    "static void TK_DichNhan(UINT uCurTime)\n"
    "{\n",
    "static void TK_DichNhan(int nX, int nY, UINT uCurTime)\n"
    "{\n",
    "static void TK_DichNhan(int nX, int nY, UINT uCurTime)")

f.thay(
    "	AUTOLOG(\"[TK-DICH] server tra %d dich: %s\", s_nTKDichSo, g_szTKDich);\n"
    "	if (s_nTKDichSo == 0)\n",
    "	// (04/09 phan bien) muc da bo con han -> danh dau; GIU muc dang di neu con trong tin moi, tru khi co muc\n"
    "	// khac gan hon >= 25% (luat TK_SanNguoi) - khong dao chieu moi tin giua hai dich cach deu.\n"
    "	{\n"
    "		int nCu = -1, nGan = -1;\n"
    "		long nCuD = 0x7fffffff, nGanD = 0x7fffffff;\n"
    "		for (int i = 0; i < s_nTKDichSo; ++i)\n"
    "		{\n"
    "			if (TK_DichDaBo(s_aTKDichID[i], uCurTime))\n"
    "			{\n"
    "				s_aTKDichBo[i] = 1;\n"
    "				continue;\n"
    "			}\n"
    "			const long d = g_GetDistance(nX, nY, s_aTKDichX[i], s_aTKDichY[i]);\n"
    "			if (s_uTKDichChonID && s_aTKDichID[i] == s_uTKDichChonID)\n"
    "			{\n"
    "				nCu = i;\n"
    "				nCuD = d;\n"
    "			}\n"
    "			if (d < nGanD)\n"
    "			{\n"
    "				nGanD = d;\n"
    "				nGan = i;\n"
    "			}\n"
    "		}\n"
    "		if (nCu >= 0 && !(nGan >= 0 && nGan != nCu && nGanD * 4 < nCuD * 3))\n"
    "			s_nTKDichChon = nCu;		// giu muc cu - TK_DichXa di tiep, kiem tam / ket nhu thuong\n"
    "		else\n"
    "			s_uTKDichChonID = 0;\n"
    "	}\n"
    "	AUTOLOG(\"[TK-DICH] server tra %d dich (giu muc %d): %s\", s_nTKDichSo, s_nTKDichChon, g_szTKDich);\n"
    "	if (s_nTKDichSo == 0)\n",
    "server tra %d dich (giu muc %d)")

f.thay(
    "	TK_DichNhan(uCurTime);	// (04/09, dot 5) chup tra loi vi tri dich dung luc den\n",
    "	TK_DichNhan(nX, nY, uCurTime);	// (04/09, dot 5) chup tra loi vi tri dich dung luc den\n",
    "TK_DichNhan(nX, nY, uCurTime);")

# 2 + 3. TK_DichXa
f.thay(
    "static int TK_DichXa(int nPlayerIdx, const autoData* pAp, int nX, int nY, UINT uCurTime)\n"
    "{\n"
    "	if ((int)(uCurTime - s_uTKDichTickT) > 1500)\n",
    "static int TK_DichXa(int nPlayerIdx, const autoData* pAp, int nX, int nY, UINT uCurTime)\n"
    "{\n"
    "	if (!pAp->bPKPlayer)\n"
    "		return 0;		// (04/09 phan bien) tat 'Danh nguoi': tang danh khong nhan nguoi -> khong dat di, khong hoi\n"
    "	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;\n"
    "	if ((int)(uCurTime - s_uTKDichTickT) > 1500)\n",
    "tat 'Danh nguoi': tang danh khong nhan nguoi")

f.thay(
    "			s_aTKDichBo[i] = 1;\n"
    "			s_nTKDichChon = -1;\n"
    "			s_nTKRaoKetMuc = 0;\n"
    "			AUTOLOG(\"[TK-DICH] toi (%d,%d) khong thay id=%u (ket=%d) - doi muc / hoi lai. me=(%d,%d)\", tx, ty, (unsigned int)s_aTKDichID[i], nKet, nX, nY);\n",
    "			s_aTKDichBo[i] = 1;\n"
    "			TK_DichBoNho(s_aTKDichID[i], uCurTime);	// (04/09 phan bien) nho 30 giay - tin sau khong chon lai ngay\n"
    "			s_uTKDichChonID = 0;\n"
    "			s_nTKDichChon = -1;\n"
    "			s_nTKRaoKetMuc = 0;\n"
    "			AUTOLOG(\"[TK-DICH] toi (%d,%d) khong thay id=%u (ket=%d) - doi muc / hoi lai. me=(%d,%d)\", tx, ty, (unsigned int)s_aTKDichID[i], nKet, nX, nY);\n",
    "TK_DichBoNho(s_aTKDichID[i], uCurTime);	// (04/09 phan bien)")

f.thay(
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
    "	s_nTKDichChon = nBest;\n",
    "	long nD = 0;\n"
    "	int nBest = TK_DichChonMuc(nPlayerIdx, nX, nY, &nD);\n"
    "	int nDuongTD = 1, nThamDo = 0;\n"
    "	while (nBest >= 0 && (nDuongTD = SubWorld[0].FindPath(s_aTKDichX[nBest], s_aTKDichY[nBest])) != 1)\n"
    "	{\n"
    "		nThamDo = 1;\n"
    "		if (nDuongTD < 0)\n"
    "			break;		// (04/09 phan bien) luoi dang nap (-1/-2): thu nhip sau, KHONG bo muc\n"
    "		// 2/0 = dich khong toi duoc (vung kin) - bo muc nay 30 giay, chon muc khac (toi da TK_DICH_SO lan)\n"
    "		AUTOLOG(\"[TK-DICH] khong co duong toi id=%u (%d,%d) - bo\", (unsigned int)s_aTKDichID[nBest], s_aTKDichX[nBest], s_aTKDichY[nBest]);\n"
    "		s_aTKDichBo[nBest] = 1;\n"
    "		TK_DichBoNho(s_aTKDichID[nBest], uCurTime);\n"
    "		nBest = TK_DichChonMuc(nPlayerIdx, nX, nY, &nD);\n"
    "	}\n"
    "	if (nBest < 0 || nDuongTD != 1)\n"
    "	{\n"
    "		if (nThamDo)\n"
    "		{	// (04/09 phan bien) FindPath tham do da StopPath -> cho DT_WalkTo cua tang rao tinh lai NGAY, khong cho 2,5 s\n"
    "			g_ScenePlace.RemoveFlag();\n"
    "			ea.uDTPath = 0;\n"
    "		}\n"
    "		return 0;\n"
    "	}\n"
    "	s_nTKDichChon = nBest;\n"
    "	s_uTKDichChonID = s_aTKDichID[nBest];\n",
    "int nDuongTD = 1, nThamDo = 0;")

# TK_Pha: bat dau luot moi thi quen muc cu
f.thay(
    "	s_nTKDichChon = -1;	// (04/09, dot 5) chon lai muc do may chu bao\n",
    "	s_nTKDichChon = -1;	// (04/09, dot 5) chon lai muc do may chu bao\n"
    "	s_uTKDichChonID = 0;\n",
    "	s_uTKDichChonID = 0;\n	ea.uTKPhaseT")
f.ghi()
