# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_ngua_xq_0309.py - bo va idempotent (03/09/2026 dem, DOT 4) cho WAuto Tong Kim.
Chu game (21:0x) giao 5 yeu cau:
  * ra khoi doanh trai thi Auto xac dinh vi tri dich de di chuyen toi danh
  * tren duong di gap dich thi dung lai danh
  * doi tuong xac dinh luc dau da chet thi doi qua doi tuong khac
  * di chuyen duong xa - luc chet ve (hoi sinh) thi phai tu len ngua - xuong ngua chi khi gap dich
  * dang duoi ngua ma xung quanh co dich thi khong duoc len ngua - chi len ngua khi di chuyen xa

Sua (chi CoreShell.cpp -> chi CoreClient.dll):
  1. TK_RaoDi (b2): chua thay dich va khong con 'diem cuoi thay dich' -> moi luot ra tran di thang mot lan
     toi KHU XUAT QUAN cua dich (8 diem trap ra trai ben kia - KTongKimTables.h g_TKXuatQuanA/B, noi dich
     hien ra sau moi lan hoi sinh; client khong thay dich ngoai bong bong dong bo nen day la vi tri dich
     chac nhat). Toi noi / qua 90 s / ket 3 s -> rao quanh do. Gap dich tren duong: tang san / may PK
     cuop quyen (thu tu TKP_FIGHT san co); dich chet: tang san chon con khac moi nhip (san co).
  2. NGUA: DT_DuocLenNgua doi luat - o 'Xuong ngua' (tab PK) / o ngua tab Chien dau = xuong chi noi ve
     luc DANH; di duong: LEN khi duong con >= NGUA_XA, khong co muc tieu, khong co dich (quan he enemy)
     trong Tam nhin PK + NGUA_DICH_THEM, va da >= NGUA_NGHI ke tu lan xuong. May TK XUONG NGUA khi gap
     dich: giao muc tieu cho may PK, dich san vao tam PK, hay may PK dang om muc tieu.
Chay:  python goi_va_wauto_tk_ngua_xq_0309.py [--thu] [--root <Sources\\Core\\Src>]
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
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, neo + moi, 1)
        self.log.append("  them sau: %s" % dau[:60])
    def them_truoc(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, moi + neo, 1)
        self.log.append("  them truoc: %s" % dau[:60])
    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi); dau = self.N(dau)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        n = self.s.count(cu)
        if n != 1:
            raise SystemExit("doan can thay khong duy nhat (%d): %r" % (n, cu[:90]))
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

f = Tep(os.path.join(ROOT, "CoreShell.cpp"))

# =====================================================================
# 1. DT_DuocLenNgua: luat ngua moi (+ DT_CoDichGan, hang so, moc lan xuong)
# =====================================================================
f.thay(
    "static bool DT_DuocLenNgua(const ExtAuto& ea)\n"
    "{\n"
    "	if (s_pApDiDuong && (s_pApDiDuong->bPKDownHorse || s_pApDiDuong->nSelFHorse == 2))\n"
    "		return false;\n"
    "	if (ea.nTKHold == 2 || ea.nCTHold == 2 || ea.nLDHold == 2 || ea.nHDHold == 2 || ea.nSTHold == 2)\n"
    "		return false;\n"
    "	if (ea.nDTPhase == DTP_FARM)\n"
    "		return false;\n"
    "	return true;\n"
    "}\n",
    "// (03/09 dem, dot 4) LUAT NGUA MOI - chu game: \"di chuyen duong xa (ke ca luc chet ve lai) thi phai tu\n"
    "// len ngua; xuong ngua chi khi gap dich; dang duoi ngua ma xung quanh co dich thi KHONG len; chi len\n"
    "// khi di chuyen xa\". => o 'Xuong ngua' (tab PK) / o ngua tab Chien dau = xuong CHI noi ve luc DANH,\n"
    "// khong cam len ngua khi di duong nua (doi lai luat 03/09 chieu), va may dang trong tran cung LEN\n"
    "// khi duong xa ma quanh minh khong co dich. Dich = NPC/nguoi choi quan he enemy con song, trong Tam\n"
    "// nhin PK + NGUA_DICH_THEM (tre hysteresis de khong len/xuong lien tuc o bien tam nhin); NPC chi\n"
    "// tinh khi dang trong tran Tong Kim va o 'Danh quai' bat. Vua xuong ngua de danh thi NGUA_NGHI sau\n"
    "// moi len lai. Luat cu con giu: pha farm Da Tau khong len.\n"
    "#define NGUA_XA			480		// mps: duong con lai ngan hon = khong dang len (15 o)\n"
    "#define NGUA_DICH_THEM	400		// mps: cong them vao Tam nhin PK khi xet 'xung quanh co dich'\n"
    "#define NGUA_NGHI		6000	// ms: sau lan xuong ngua (gap dich) it nhat bay nhieu moi len lai\n"
    "static UINT s_uNguaXuongT = 0;	// moc lan cuoi may TK xuong ngua vi gap dich\n"
    "static int DT_CoDichGan(int nPlayerIdx, int nTam, int nKeNpc)\n"
    "{\n"
    "	const int nSelf = Player[nPlayerIdx].m_nIndex;\n"
    "	if (nSelf <= 0)\n"
    "		return 0;\n"
    "	int nX, nY, x, y;\n"
    "	Npc[nSelf].GetMpsPos(&nX, &nY);\n"
    "	int nIdx = 0;\n"
    "	while (nIdx = NpcSet.GetNextIdx(nIdx))\n"
    "	{\n"
    "		if (nIdx == nSelf || !Npc[nIdx].m_dwID || Npc[nIdx].m_RegionIndex < 0)\n"
    "			continue;\n"
    "		if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive || Npc[nIdx].m_CurrentLife <= 0)\n"
    "			continue;\n"
    "		if (Npc[nIdx].m_Kind != kind_player && !nKeNpc)\n"
    "			continue;\n"
    "		if (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)\n"
    "			continue;\n"
    "		Npc[nIdx].GetMpsPos(&x, &y);\n"
    "		if (g_GetDistance(nX, nY, x, y) <= nTam)\n"
    "			return nIdx;\n"
    "	}\n"
    "	return 0;\n"
    "}\n"
    "static bool DT_DuocLenNgua(int nPlayerIdx, const ExtAuto& ea, int nX, int nY, int nDX, int nDY, UINT uCurTime)\n"
    "{\n"
    "	if (ea.nDTPhase == DTP_FARM)\n"
    "		return false;\n"
    "	if (g_GetDistance(nX, nY, nDX, nDY) < NGUA_XA)\n"
    "		return false;		// duong ngan - khong dang len\n"
    "	if (ea.uNpcID)\n"
    "		return false;		// dang co muc tieu danh\n"
    "	if (s_uNguaXuongT && (int)(uCurTime - s_uNguaXuongT) < NGUA_NGHI)\n"
    "		return false;\n"
    "	int nTam = 800;\n"
    "	if (s_pApDiDuong)\n"
    "	{\n"
    "		nTam = s_pApDiDuong->nPKVision;\n"
    "		if (nTam < 100)\n"
    "			nTam = 100;\n"
    "		else if (nTam > 1200)\n"
    "			nTam = 1200;\n"
    "	}\n"
    "	const int nKeNpc = (ea.nTKHold == 2 && s_pApDiDuong && s_pApDiDuong->bPKNpc) ? 1 : 0;\n"
    "	if (DT_CoDichGan(nPlayerIdx, nTam + NGUA_DICH_THEM, nKeNpc))\n"
    "		return false;		// xung quanh co dich - khong len\n"
    "	return true;\n"
    "}\n",
    "static bool DT_DuocLenNgua(int nPlayerIdx, const ExtAuto& ea, int nX, int nY, int nDX, int nDY, UINT uCurTime)")

f.thay(
    "	if (DT_DuocLenNgua(ea))\n"
    "		DT_Ride(nPlayerIdx);\n",
    "	if (DT_DuocLenNgua(nPlayerIdx, ea, px, py, nX, nY, uCurTime))\n"
    "		DT_Ride(nPlayerIdx);\n",
    "DT_DuocLenNgua(nPlayerIdx, ea, px, py, nX, nY, uCurTime)")

# =====================================================================
# 2. bien khu xuat quan dich (sau bien dot 3)
# =====================================================================
f.them_sau(
    "static UINT s_uTKPosT = 0;\n",
    "// (03/09 dem, dot 4) mot luot ra tran di mot lan toi KHU XUAT QUAN DICH - xem TK_RaoDi (b2)\n"
    "static int  s_nTKXQDa = 0;		// 1 = luot nay da toi / da bo\n"
    "static int  s_nTKXQChon = -1;	// diem xuat quan dich dang toi (chi so g_TKXuatQuanA/B)\n"
    "static UINT s_uTKXQHan = 0;\n",
    "static int  s_nTKXQChon = -1;")

# =====================================================================
# 3. TK_XuongNgua (truoc TK_RaoKet)
# =====================================================================
f.them_truoc(
    "// (03/09 toi) DO KET khi dang rao. Goc loi chu game bao \"di chuyen ra 1 goc roi dung yen -\n",
    "// (03/09 dem, dot 4) XUONG NGUA khi gap dich (chu game: \"xuong ngua chi khi gap dich\") - cung khuon\n"
    "// PA_RIDE (co ngua o o trang bi, khong ngoi, het gian TIME_RIDE); ghi moc de DT_DuocLenNgua khong\n"
    "// len lai ngay. May PK (o 'Xuong ngua') van xuong theo cach cua no - cung chieu, khong xung dot.\n"
    "static void TK_XuongNgua(int nPlayerIdx, UINT uCurTime)\n"
    "{\n"
    "	const int nSelf = Player[nPlayerIdx].m_nIndex;\n"
    "	if (nSelf <= 0 || !Npc[nSelf].m_bRideHorse)\n"
    "		return;\n"
    "	s_uNguaXuongT = uCurTime ? uCurTime : 1;\n"
    "	if (Player[nPlayerIdx].m_ItemList.GetEquipment(itempart_horse) <= 0)\n"
    "		return;\n"
    "	if (Npc[nSelf].m_Doing == do_sit)\n"
    "		return;\n"
    "	if (GetTickCount() - Npc[nSelf].m_TimeHorse < TIME_RIDE)\n"
    "		return;\n"
    "	Npc[nSelf].m_TimeHorse = GetTickCount();\n"
    "	SendClientCmdRide(TRUE);\n"
    "	AUTOLOG(\"[TK-NGUA] gap dich - xuong ngua t=%u\", uCurTime);\n"
    "}\n"
    "\n",
    "static void TK_XuongNgua(int nPlayerIdx, UINT uCurTime)")

# =====================================================================
# 4. TK_RaoDi (b2): tien ve khu xuat quan dich
# =====================================================================
MSG_XQ = tcvn3("<color=Cyan>Không thấy địch - tiến về khu xuất quân của địch.")
f.them_truoc(
    "	// (c) o dang di: toi noi / qua han -> danh dau da tham\n",
    "	// (b2) (03/09 dem, dot 4) chua co diem cuoi thay dich: TIEN THANG VE KHU XUAT QUAN DICH (8 diem trap\n"
    "	// ra trai ben kia - g_TKXuatQuanA/B) MOT lan moi luot ra tran. Chu game: \"ra khoi doanh trai thi xac\n"
    "	// dinh vi tri dich de di toi danh\" - client chi thay dich trong bong bong dong bo (~40 o) nen khu\n"
    "	// xuat quan (noi dich hien ra sau moi lan hoi sinh) la vi tri dich chac nhat. Toi noi (< 480 mps),\n"
    "	// qua 90 giay, hay ket 3 giay -> thoi, rao quanh do. Tren duong thay dich la tang san / may PK cuop\n"
    "	// quyen ngay (thu tu TKP_FIGHT); dich chet thi tang san tu chon con khac moi nhip.\n"
    "	if (!s_nTKXQDa && (ea.nTKThe == 1 || ea.nTKThe == 2))\n"
    "	{\n"
    "		const TKPoint* pXQ = (ea.nTKThe == 1) ? g_TKXuatQuanB : g_TKXuatQuanA;\n"
    "		if (s_nTKXQChon < 0)\n"
    "		{\n"
    "			int nBest = -1;\n"
    "			long nBestD = 0x7fffffff;\n"
    "			for (int i = 0; i < TK_XQ_COUNT; ++i)\n"
    "			{\n"
    "				const int qx = TK_O((int)pXQ[i].x), qy = TK_O((int)pXQ[i].y);\n"
    "				const long d = g_GetDistance(nX, nY, qx, qy);\n"
    "				if (d >= nBestD)\n"
    "					continue;\n"
    "				if (SubWorld[0].FindPath(qx, qy) != 1)\n"
    "					continue;		// khong co duong tron ven (2 = chi toi o gan nhat) - bo diem nay\n"
    "				nBestD = d;\n"
    "				nBest = i;\n"
    "			}\n"
    "			if (nBest < 0)\n"
    "			{\n"
    "				s_nTKXQDa = 1;\n"
    "				AUTOLOG(\"[TK-XQ] khong co duong toi khu xuat quan dich (the=%d) me=(%d,%d) - rao\", ea.nTKThe, nX, nY);\n"
    "			}\n"
    "			else\n"
    "			{\n"
    "				s_nTKXQChon = nBest;\n"
    "				s_uTKXQHan = uCurTime + 90000u;\n"
    "				s_nTKRaoKetMuc = 0;\n"
    "				TK_Msg(nPlayerIdx, \"" + MSG_XQ + "\");\n"
    "				AUTOLOG(\"[TK-XQ] tien ve khu xuat quan dich diem %d (%d,%d) xa=%ld me=(%d,%d) the=%d\", nBest, TK_O((int)pXQ[nBest].x), TK_O((int)pXQ[nBest].y), nBestD, nX, nY, ea.nTKThe);\n"
    "			}\n"
    "		}\n"
    "		if (s_nTKXQChon >= 0)\n"
    "		{\n"
    "			const int tx = TK_O((int)pXQ[s_nTKXQChon].x), ty = TK_O((int)pXQ[s_nTKXQChon].y);\n"
    "			const int nKet = TK_RaoKet(3, nX, nY, uCurTime);\n"
    "			if (g_GetDistance(nX, nY, tx, ty) < 480 || (int)(uCurTime - s_uTKXQHan) > 0 || nKet)\n"
    "			{\n"
    "				s_nTKXQDa = 1;\n"
    "				s_nTKRaoKetMuc = 0;\n"
    "				AUTOLOG(\"[TK-XQ] toi khu xuat quan dich (%d,%d) me=(%d,%d) ket=%d - rao quanh day\", tx, ty, nX, nY, nKet);\n"
    "			}\n"
    "			else\n"
    "			{\n"
    "				DT_WalkTo(nPlayerIdx, tx, ty, 320, uCurTime);\n"
    "				return 1;\n"
    "			}\n"
    "		}\n"
    "	}\n",
    "[TK-XQ] tien ve khu xuat quan dich")

# =====================================================================
# 5. TK_Pha: luot moi -> di lai khu xuat quan
# =====================================================================
f.them_sau(
    "	s_uTKPosT = 0;		// (03/09 toi, dot 3) doi pha (hoi sinh / ra trai) la nhay hop le - khong tinh la bi nem ve\n",
    "	s_nTKXQDa = 0;		// (03/09 dem, dot 4) luot ra tran moi -> di lai khu xuat quan dich\n"
    "	s_nTKXQChon = -1;\n",
    "s_nTKXQDa = 0;		// (03/09 dem, dot 4)")

# =====================================================================
# 6. TKP_FIGHT: xuong ngua khi gap dich (3 cho)
# =====================================================================
f.them_sau(
    "		s_nTKPosX = nX;\n"
    "		s_nTKPosY = nY;\n"
    "		s_uTKPosT = uCurTime;\n",
    "		if (ea.uNpcID)\n"
    "			TK_XuongNgua(nPlayerIdx, uCurTime);	// (03/09 dem, dot 4) dang om muc tieu = dang gap dich\n",
    "TK_XuongNgua(nPlayerIdx, uCurTime);	// (03/09 dem, dot 4) dang om muc tieu")

f.thay(
    "				const int nSan = TK_SanNguoi(nPlayerIdx, pAp, uCurTime, &nAimX, &nAimY);\n"
    "				if (nSan)\n"
    "				{\n"
    "					ea.uNpcID = 0;			// chua danh duoc ai - de may PK khong om id cu\n",
    "				const int nSan = TK_SanNguoi(nPlayerIdx, pAp, uCurTime, &nAimX, &nAimY);\n"
    "				if (nSan)\n"
    "				{\n"
    "					{	// (03/09 dem, dot 4) dich san da vao Tam nhin PK (+ tre) -> xuong ngua truoc khi ap sat\n"
    "						int sx = 0, sy = 0;\n"
    "						Npc[nSan].GetMpsPos(&sx, &sy);\n"
    "						int nTamNg = pAp->nPKVision;\n"
    "						if (nTamNg < 100)\n"
    "							nTamNg = 100;\n"
    "						else if (nTamNg > 1200)\n"
    "							nTamNg = 1200;\n"
    "						if (g_GetDistance(nX, nY, sx, sy) <= nTamNg + NGUA_DICH_THEM)\n"
    "							TK_XuongNgua(nPlayerIdx, uCurTime);\n"
    "					}\n"
    "					ea.uNpcID = 0;			// chua danh duoc ai - de may PK khong om id cu\n",
    "dich san da vao Tam nhin PK (+ tre) -> xuong ngua")

f.thay(
    "			if (nTG)\n"
    "			{\n"
    "				if (Npc[nTG].m_Kind == kind_player)\n"
    "					TK_RaoThayDich(nTG, uCurTime);\n",
    "			if (nTG)\n"
    "			{\n"
    "				if (Npc[nTG].m_Kind == kind_player)\n"
    "					TK_RaoThayDich(nTG, uCurTime);\n"
    "				TK_XuongNgua(nPlayerIdx, uCurTime);	// (03/09 dem, dot 4) gap dich = xuong ngua roi giao may PK\n",
    "gap dich = xuong ngua roi giao may PK")

f.ghi()
