# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_rao_ket_0309.py - bo va idempotent (03/09/2026 toi) cho WAuto Tong Kim,
sua loi chu game bao: "vao tong kim WAuto di chuyen ra 1 goc roi dung yen - keo ra vi tri
khac thi chay ve lai goc do". Ba goc do duoc tu jx_auto.log 03/09 19:2x-19:3x:

  1. TK_RaoDi: KSubWorld::FindPath tra 2 khi dich KHONG toi duoc (di toi o gan nhat -
     KSubWorld.cpp FindPath_Block 'goal unreachable' + 1022-1032), ma buoc chon o chi loai
     '<= 0' => nhan vat toi o gan nhat roi dung cach dich 290-410 mps (> nguong 200 'toi noi'),
     cho het han 45 giay; DT_WalkTo cu 2,5 giay tinh lai duong nen keo di la bi keo ve.
     Sua: chi nhan o co FindPath == 1; them DO KET (dung yen >= 3 giay, < 32 mps) cho ca
     'o dang di' va 'diem cuoi thay dich' -> bo ngay, chon o khac; het luot thu thi van rao
     (nhip sau chon tiep), khong lui ve bang toa do co dinh.
  2. TK_Process: tat/bat tick auto trong WAuto -> PRT_TICKSTART -> ATYPE_CLEAR memset ExtAuto
     -> may TK ve TKP_OFF; loa bao danh da troi, khung gio khong cau hinh => nTK=0 den het
     tran, dung yen. Sua: dang dung tren map tran (379) va pha OFF thi vao lai ngay
     (cung luat TKP_GO 'vao lai game giua tran').
  3. WA_ChieuBiCam: ly do tu choi TAM THOI / THEO MUC TIEU (im lang, ngua, FightMode muc
     tieu vua them chua dong bo, quan he chua la dich) chi bo luot, KHONG cam 30 giay - log
     19:31 ca 3 chieu 1977/1985/1967 bi cam cung luc ngay khi ra khoi trai (1977 vu khi
     dung: particular 7 = yeu cau 7) -> dung canh dich 30 giay khong danh.
Chay:  python goi_va_wauto_tk_rao_ket_0309.py [--thu] [--root <Sources\\Core\\Src>]
Tep nguon la TCVN3/GBK -> doc/ghi latin-1, newline=''. Chi CoreClient.dll can build lai
(CoreShell.cpp ExcludedFromBuild o moi cau hinh Server; khong doi struct / goi mang).
"""
import io, sys, os

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
if "--root" in sys.argv:
    ROOT = sys.argv[sys.argv.index("--root") + 1]
THU = "--thu" in sys.argv

# ---- TCVN3 (chi chu thuong + 7 chu hoa goc) de ghi thong bao trong game ----
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

class Tep(object):
    def __init__(self, path):
        self.path = path
        self.s = io.open(path, encoding="latin-1", newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []
    def N(self, text):
        return text.replace("\r\n", "\n").replace("\n", self.nl)
    def them_sau(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, neo + moi, 1)
        self.log.append("  them sau: %s" % dau)
    def them_truoc(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, moi + neo, 1)
        self.log.append("  them truoc: %s" % dau)
    def thay(self, cu, moi, dau):
        """thay 'cu' -> 'moi'; da ap khi 'dau' (chuoi chi co trong ban moi) da co trong tep"""
        cu = self.N(cu); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        n = self.s.count(cu)
        if n != 1:
            raise SystemExit("doan can thay khong duy nhat (%d): %r" % (n, cu[:90]))
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau)
    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return
        hb_goc = sum(1 for c in self.goc if ord(c) >= 0x80)
        hb_moi = sum(1 for c in self.s if ord(c) >= 0x80)
        print("%s: %d dong log, byte cao %d -> %d" % (os.path.basename(self.path), len(self.log), hb_goc, hb_moi))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)

f = Tep(os.path.join(ROOT, "CoreShell.cpp"))

# =====================================================================
# 1a. bien do ket + hang so (sau bang thong ke rao)
# =====================================================================
f.them_sau(
    "static UINT  s_uTKRaoMsgT = 0;\n",
    "// (03/09 toi) DO KET khi dang rao - xem TK_RaoKet\n"
    "#define TK_RAO_KET_MS	3000		// ms: dung yen (< 32 mps) lau hon the = khong toi duoc\n"
    "static int   s_nTKRaoKetX = 0, s_nTKRaoKetY = 0;	// vi tri lan cuoi con nhuc nhich\n"
    "static UINT  s_uTKRaoKetT = 0;\n"
    "static int   s_nTKRaoKetMuc = 0;		// dang do cho muc nao: 0 chua / 1 diem cuoi thay dich / 2 o rao\n"
    "static UINT  s_uTKRaoTickT = 0;		// nhip TK_RaoDi lan cuoi (vua danh / san xong thi do lai tu dau)\n",
    "s_nTKRaoKetMuc")

# =====================================================================
# 1b. ham TK_RaoKet (truoc TK_RaoDi)
# =====================================================================
f.them_truoc(
    "// khong thay dich: di rao. Tra 1 = dang rao (da phat lenh di), 0 = khong co luoi.\n"
    "static int TK_RaoDi(int nPlayerIdx, int nX, int nY, UINT uCurTime)\n",
    "// (03/09 toi) DO KET khi dang rao. Goc loi chu game bao \"di chuyen ra 1 goc roi dung yen -\n"
    "// keo ra vi tri khac thi chay ve lai goc do\": KSubWorld::FindPath tra 2 khi dich KHONG toi\n"
    "// duoc - no di toi o GAN NHAT roi ghi m_nTargetX/Y = o trung gian (KSubWorld.cpp\n"
    "// FindPath_Block 'goal unreachable', 1022-1032). DT_WalkTo khong biet, cu 2,5 giay tinh lai\n"
    "// duong ve dung o trung gian do => nhan vat dung yen cach dich 290-410 mps (> nguong 200\n"
    "// 'toi noi') cho het han 45 giay; keo di thi bi keo ve. Do trong log 03/09 19:3x: o 678 /\n"
    "// 619 / 589 deu dung yen 20-35 giay, o moi chi duoc chon dung moi 45 giay.\n"
    "// Tra 1 = da dung yen (dich < 32 mps) lau hon TK_RAO_KET_MS khi dang di toi muc nMuc.\n"
    "static int TK_RaoKet(int nMuc, int nX, int nY, UINT uCurTime)\n"
    "{\n"
    "	if (s_nTKRaoKetMuc != nMuc || g_GetDistance(nX, nY, s_nTKRaoKetX, s_nTKRaoKetY) >= 32)\n"
    "	{\n"
    "		s_nTKRaoKetMuc = nMuc;\n"
    "		s_nTKRaoKetX = nX;\n"
    "		s_nTKRaoKetY = nY;\n"
    "		s_uTKRaoKetT = uCurTime;\n"
    "		return 0;\n"
    "	}\n"
    "	return (int)(uCurTime - s_uTKRaoKetT) > TK_RAO_KET_MS;\n"
    "}\n"
    "\n",
    "static int TK_RaoKet(")

# =====================================================================
# 1c. TK_RaoDi: do lai tu dau sau khoang danh / san
# =====================================================================
f.them_sau(
    "	if (!TK_RaoDung(nMap))\n"
    "		return 0;\n",
    "	if ((int)(uCurTime - s_uTKRaoTickT) > 1500)\n"
    "		s_nTKRaoKetMuc = 0;		// (03/09 toi) vua danh / san xong quay lai rao -> do ket lai tu dau\n"
    "	s_uTKRaoTickT = uCurTime;\n",
    "s_uTKRaoTickT = uCurTime;")

# =====================================================================
# 1d. (b) diem cuoi thay dich: ket thi bo
# =====================================================================
f.thay(
    "		if (g_GetDistance(nX, nY, s_nTKRaoLastX, s_nTKRaoLastY) < 300)\n"
    "			s_nTKRaoLastDi = 1;\n"
    "		else\n"
    "		{\n"
    "			AUTOLOG_EVERY(3000, \"[TK-RAO] ve diem cuoi thay dich (%d,%d) me=(%d,%d)\", s_nTKRaoLastX, s_nTKRaoLastY, nX, nY);\n",
    "		if (g_GetDistance(nX, nY, s_nTKRaoLastX, s_nTKRaoLastY) < 300)\n"
    "			s_nTKRaoLastDi = 1;\n"
    "		else if (TK_RaoKet(1, nX, nY, uCurTime))\n"
    "		{	// (03/09 toi) khong toi duoc (FindPath 2) - bo, rao o khac\n"
    "			s_nTKRaoLastDi = 1;\n"
    "			s_nTKRaoKetMuc = 0;\n"
    "			AUTOLOG(\"[TK-RAO] ket khi ve diem cuoi thay dich (%d,%d) me=(%d,%d) - bo\", s_nTKRaoLastX, s_nTKRaoLastY, nX, nY);\n"
    "		}\n"
    "		else\n"
    "		{\n"
    "			AUTOLOG_EVERY(3000, \"[TK-RAO] ve diem cuoi thay dich (%d,%d) me=(%d,%d)\", s_nTKRaoLastX, s_nTKRaoLastY, nX, nY);\n",
    "ket khi ve diem cuoi thay dich")

# =====================================================================
# 1e. (c) o dang di: ket thi danh dau da tham, chon o khac ngay
# =====================================================================
f.thay(
    "		const int tx = s_aTKRaoX[s_nTKRaoDangDi] * 32 + 16, ty = s_aTKRaoY[s_nTKRaoDangDi] * 32 + 16;\n"
    "		if (g_GetDistance(nX, nY, tx, ty) < 200 || (int)(uCurTime - s_uTKRaoHan) > 0)\n"
    "		{\n"
    "			s_aTKRaoT[s_nTKRaoDangDi] = uCurTime ? uCurTime : 1;\n"
    "			s_nTKRaoDangDi = -1;\n"
    "		}\n",
    "		const int tx = s_aTKRaoX[s_nTKRaoDangDi] * 32 + 16, ty = s_aTKRaoY[s_nTKRaoDangDi] * 32 + 16;\n"
    "		const int nKet = TK_RaoKet(2, nX, nY, uCurTime);	// (03/09 toi) dung yen 3 giay = khong toi duoc\n"
    "		if (g_GetDistance(nX, nY, tx, ty) < 200 || (int)(uCurTime - s_uTKRaoHan) > 0 || nKet)\n"
    "		{\n"
    "			if (nKet)\n"
    "				AUTOLOG(\"[TK-RAO] ket o %d (%d,%d) me=(%d,%d) - bo o nay, chon o khac\", s_nTKRaoDangDi, tx, ty, nX, nY);\n"
    "			s_aTKRaoT[s_nTKRaoDangDi] = uCurTime ? uCurTime : 1;\n"
    "			s_nTKRaoDangDi = -1;\n"
    "			s_nTKRaoKetMuc = 0;\n"
    "		}\n",
    "[TK-RAO] ket o %d")

# =====================================================================
# 1f. (d) chon o: thu 6 o; chi nhan FindPath == 1; het luot van rao
# =====================================================================
f.thay(
    "	for (int nThu = 0; nThu < 4; ++nThu)\n"
    "	{\n"
    "		int nBest = -1;\n"
    "		long nBestScore = 0x7fffffff;\n",
    "	for (int nThu = 0; nThu < 6; ++nThu)\n"
    "	{\n"
    "		int nBest = -1;\n"
    "		long nBestScore = 0x7fffffff;\n",
    "for (int nThu = 0; nThu < 6; ++nThu)\n	{\n		int nBest = -1;\n		long nBestScore")

f.thay(
    "		if (SubWorld[0].FindPath(tx, ty) <= 0)\n"
    "		{	// khong co duong (o kin / dao nho) - bo o nay mot vong\n"
    "			s_aTKRaoT[nBest] = uCurTime ? uCurTime : 1;\n"
    "			continue;\n"
    "		}\n"
    "		s_nTKRaoDangDi = nBest;\n"
    "		s_uTKRaoHan = uCurTime + TK_RAO_HAN;\n",
    "		if (SubWorld[0].FindPath(tx, ty) != 1)\n"
    "		{	// (03/09 toi) 1 = toi dung dich; 2 = dich KHONG toi duoc, chi di toi o gan nhat\n"
    "			// (KSubWorld.cpp:1022-1032) - truoc day coi 2 la 'co duong' nen nhan vat toi o gan\n"
    "			// nhat roi dung yen 45 giay; <= 0 = khong duong / luoi chua nap. Bo o nay mot vong.\n"
    "			s_aTKRaoT[nBest] = uCurTime ? uCurTime : 1;\n"
    "			continue;\n"
    "		}\n"
    "		s_nTKRaoDangDi = nBest;\n"
    "		s_uTKRaoHan = uCurTime + TK_RAO_HAN;\n"
    "		s_nTKRaoKetMuc = 0;		// do ket lai tu dau cho o moi\n",
    "SubWorld[0].FindPath(tx, ty) != 1")

f.thay(
    "		DT_WalkTo(nPlayerIdx, tx, ty, 160, uCurTime);\n"
    "		return 1;\n"
    "	}\n"
    "	return 0;\n"
    "}\n"
    "// ===================== HET RAO MAP =====================\n",
    "		DT_WalkTo(nPlayerIdx, tx, ty, 160, uCurTime);\n"
    "		return 1;\n"
    "	}\n"
    "	// (03/09 toi) 6 o gan nhat deu khong toi duoc nhip nay (da danh dau) - van dang rao,\n"
    "	// nhip sau (400 ms) chon tiep; KHONG lui ve bang toa do co dinh (chu game bo cach do).\n"
    "	return 1;\n"
    "}\n"
    "// ===================== HET RAO MAP =====================\n",
    "6 o gan nhat deu khong toi duoc nhip nay")

# =====================================================================
# 2. TK_Process: dang dung trong map tran (379) + pha OFF -> vao lai ngay
# =====================================================================
f.thay(
    "	if (ea.nTKPhase == TKP_OFF || ea.nTKPhase == TKP_DONE)\n"
    "	{\n"
    "		ea.nTKHold = 0;\n"
    "		if (!nTrongGio && !nLoaMo)\n"
    "		{\n"
    "			ea.nTKPhase = TKP_OFF;\n"
    "			return 0;\n"
    "		}\n",
    "	if (ea.nTKPhase == TKP_OFF || ea.nTKPhase == TKP_DONE)\n"
    "	{\n"
    "		ea.nTKHold = 0;\n"
    "		// (03/09 toi) DANG DUNG TRONG MAP TRAN (379) = chac chan giua tran, khong can gio / loa.\n"
    "		// Tat/bat tick auto o dong nhan vat trong WAuto -> PRT_TICKSTART -> ATYPE_CLEAR memset\n"
    "		// ExtAuto -> may ve TKP_OFF; loa bao danh da troi, khung gio khong cau hinh (Tong Kim\n"
    "		// dang mo test) => truoc day nTK = 0 den het tran, nhan vat dung yen (log 03/09 19:34:\n"
    "		// [HD-GATE] nTK 2 -> 0 sau 13 giay auto nghi, giu 0 tro di). Cung luat voi TKP_GO\n"
    "		// 'dang o san trong tran (vao lai game giua tran)'. Chi ap cho pha OFF, khong ap cho\n"
    "		// DONE (bo cuoc sau 3 phut ket - giu quyet dinh cu).\n"
    "		const int nDungTrongTran = (nMap == TK_MAP_TRAN && ea.nTKPhase == TKP_OFF);\n"
    "		if (!nTrongGio && !nLoaMo && !nDungTrongTran)\n"
    "		{\n"
    "			ea.nTKPhase = TKP_OFF;\n"
    "			return 0;\n"
    "		}\n",
    "nDungTrongTran = (nMap == TK_MAP_TRAN")

# thong bao rieng khi vao lai giua tran (dong TK_Msg goc giu nguyen byte, chi doi thanh nhanh else)
DAU_MSG = "<color=Cyan>" + tcvn3("Đang đứng trong trận Tống Kim - vào lại đánh tiếp.")
if DAU_MSG in f.s:
    f.log.append("  da co: thong bao vao lai giua tran")
else:
    neo = f.N("		TK_Pha(nPlayerIdx, TKP_GO, uCurTime);\n		TK_Msg(nPlayerIdx, \"<color=Cyan>T")
    if f.s.count(neo) != 1:
        raise SystemExit("neo TK_Pha(GO)+TK_Msg khong duy nhat: %d" % f.s.count(neo))
    i = f.s.find(neo)
    j = f.s.find(f.nl, i + len(neo))          # het dong TK_Msg goc
    dong_goc = f.s[i + len(f.N("		TK_Pha(nPlayerIdx, TKP_GO, uCurTime);\n")):j]
    if not dong_goc.strip().startswith("TK_Msg(nPlayerIdx, \"<color=Cyan>T") or not dong_goc.rstrip().endswith("\");"):
        raise SystemExit("dong TK_Msg goc khong nhu mong doi: %r" % dong_goc[:60])
    moi = f.N("		TK_Pha(nPlayerIdx, TKP_GO, uCurTime);\n"
              "		if (nDungTrongTran)\n"
              "			TK_Msg(nPlayerIdx, \"" + DAU_MSG + "\");\n"
              "		else\n"
              "	") + dong_goc
    f.s = f.s[:i] + moi + f.s[j:]
    f.log.append("  thay: thong bao vao lai giua tran (dong goc giu nguyen byte)")

# =====================================================================
# 3. WA_ChieuBiCam: ly do tam thoi / theo muc tieu -> bo luot, khong cam 30 giay
# =====================================================================
f.thay(
    "	int p1 = -1, p2 = nTG;\n"
    "	if (!pSkill || pSkill->CanCastSkill(nSelf, p1, p2))\n"
    "		return false;\n"
    "	if (nTrong < 0)\n"
    "		nTrong = 0;\n",
    "	int p1 = -1, p2 = nTG;\n"
    "	if (!pSkill || pSkill->CanCastSkill(nSelf, p1, p2))\n"
    "		return false;\n"
    "	// (03/09 toi) chi CAM 30 giay khi ly do thuoc VE MINH va LAU DAI (sai vu khi). Ly do TAM\n"
    "	// THOI hay THEO MUC TIEU thi chi bo luot nay, KHONG cam: bi im lang; dang / khong cuoi\n"
    "	// ngua theo gioi han chieu; muc tieu la nguoi choi vua duoc them, FightMode chua dong bo\n"
    "	// (KSkills.cpp SKILL-REFUSE-FIGHTMODE); quan he chua la dich. Log 03/09 19:31: ca ba chieu\n"
    "	// 1977 / 1985 / 1967 bi cam cung luc ngay khi ra khoi trai (1977 vu khi DUNG: particular 7\n"
    "	// = yeu cau 7) => nhan vat dung canh dich 30 giay khong danh.\n"
    "	int nTam = 0;\n"
    "	if (Npc[nSelf].m_SilentState.nTime > 0)\n"
    "		nTam = 1;\n"
    "	else if (pSkill->GetHorseLimit() == 1 && Npc[nSelf].m_bRideHorse)\n"
    "		nTam = 2;\n"
    "	else if (pSkill->GetHorseLimit() == 2 && !Npc[nSelf].m_bRideHorse)\n"
    "		nTam = 2;\n"
    "	else if (nTG > 0 && nTG < MAX_NPC && Npc[nTG].IsPlayer()\n"
    "		&& Npc[nSelf].m_FightMode != Npc[nTG].m_FightMode)\n"
    "		nTam = 3;\n"
    "	else if (nTG > 0 && nTG < MAX_NPC && pSkill->IsTargetEnemy()\n"
    "		&& !(NpcSet.GetRelation(nSelf, nTG) & relation_enemy))\n"
    "		nTam = 4;\n"
    "	if (nTam)\n"
    "	{\n"
    "		AUTOLOG_EVERY(2000, \"[CHIEU-CAM] t=%u skill=%d bi tu choi TAM THOI (ly do %d: 1 im lang / 2 ngua / 3 FightMode muc tieu / 4 quan he) - bo luot nay, khong cam\", uCurTime, nSkill, nTam);\n"
    "		return true;\n"
    "	}\n"
    "	if (nTrong < 0)\n"
    "		nTrong = 0;\n",
    "bi tu choi TAM THOI")

f.ghi()
