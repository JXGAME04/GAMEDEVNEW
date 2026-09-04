# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_raomap_0309.py - bo va idempotent (03/09/2026) cho WAuto Tong Kim:
  1. DT_WalkTo chi len ngua khi that su di duong va cau hinh cho phep
     (o 'Xuong ngua' tab PK / o ngua tab Chien dau = xuong) - het len/xuong lien tuc.
  2. Tong Kim: thu tu muc tieu moi (tuong dich -> NGUOI khac mau -> san nguoi -> linh NPC)
     va RAO MAP theo luoi A* client khi khong thay ai (thay cho bang toa do co dinh).
  3. Chan gui chieu ma client se tu choi (vi du 1967 sai vu khi) - cam 30 giay, sang khe khac.
  4. KSubWorld.h: 2 ham inline doc luoi A* client (khong doi bo cuc lop).
Chay:  python goi_va_wauto_tk_raomap_0309.py [--thu]
Tep nguon la TCVN3/GBK -> doc/ghi latin-1, newline=''. Chi CoreClient.dll can build lai.
"""
import io, sys, os

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
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
        # khoi viet bang '\n' -> doi theo newline cua tep
        return text.replace("\r\n", "\n").replace("\n", self.nl)
    def them_truoc(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, moi + neo, 1)
        self.log.append("  them truoc: %s" % dau)
    def them_sau(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        if self.s.count(neo) != 1:
            raise SystemExit("neo khong duy nhat (%d): %r" % (self.s.count(neo), neo[:70]))
        self.s = self.s.replace(neo, neo + moi, 1)
        self.log.append("  them sau: %s" % dau)
    def thay(self, cu, moi, dau=None, tatca=False):
        cu = self.N(cu); moi = self.N(moi)
        if moi in self.s and (dau is None or dau in self.s):
            self.log.append("  da co: %s" % (dau or moi[:50])); return
        n = self.s.count(cu)
        if n == 0:
            raise SystemExit("khong thay doan can thay: %r" % cu[:90])
        if n > 1 and not tatca:
            raise SystemExit("doan can thay khong duy nhat (%d): %r" % (n, cu[:90]))
        self.s = self.s.replace(cu, moi) if tatca else self.s.replace(cu, moi, 1)
        self.log.append("  thay (%d): %s" % (n, (dau or moi[:50])))
    def thay_doan(self, tu, den, moi, dau):
        """thay ca doan tu 'tu' (bao gom) den 'den' (bao gom)"""
        tu = self.N(tu); den = self.N(den); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        i = self.s.find(tu)
        if i < 0 or self.s.count(tu) != 1:
            raise SystemExit("dau doan khong duy nhat: %r" % tu[:80])
        j = self.s.find(den, i)
        if j < 0:
            raise SystemExit("khong thay cuoi doan: %r" % den[:80])
        j += len(den)
        self.s = self.s[:i] + moi + self.s[j:]
        self.log.append("  thay doan: %s (%d -> %d ky tu)" % (dau, j - i, len(moi)))
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
# 1. KSubWorld.h - hai ham doc luoi A* client
# =====================================================================
h = Tep(os.path.join(ROOT, "KSubWorld.h"))
NEO_HT = ("\tbool HaveTarget(int& x, int& y)\n"
          "\t{\n"
          "\t\tx = m_nTargetX;\n"
          "\t\ty = m_nTargetY;\n"
          "\t\tif(m_vRetPath.size() > 0 && m_nTargetX > 0 && m_nTargetY > 0)\n"
          "\t\t\treturn true;\n"
          "\t\treturn false;\n"
          "\t}\n")
KHOI_H = ("\t// (03/09 WAuto TK) DOC luoi A* cua ban do hien tai cho may auto rao map.\n"
          "\t// Chi doc, khong them truong -> bo cuc lop khong doi (Game.exe khong can build lai).\n"
          "\t// Tra: 1 = o di duoc, 0 = vat can, -1 = chua co luoi / dang nap / ngoai luoi.\n"
          "\tint LuoiOCoDiDuoc(int nMpsX, int nMpsY) const\n"
          "\t{\n"
          "\t\tif (!m_bHavePath || !m_nGridTotal)\n"
          "\t\t\treturn -1;\n"
          "\t\tif (m_hLoadPathGrid && WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_TIMEOUT)\n"
          "\t\t\treturn -1;\n"
          "\t\tconst int nW = m_nGridW * REGION_GRID_WIDTH;\n"
          "\t\tconst int nH = m_nGridH * REGION_GRID_HEIGHT;\n"
          "\t\tconst int cx = nMpsX / 32 - m_nRegionBeginX * REGION_GRID_WIDTH;\n"
          "\t\tconst int cy = nMpsY / 32 - m_nRegionBeginY * REGION_GRID_HEIGHT;\n"
          "\t\tif (cx < 0 || cy < 0 || cx >= nW || cy >= nH)\n"
          "\t\t\treturn -1;\n"
          "\t\treturn m_GridNode[cy * nW + cx].obs ? 0 : 1;\n"
          "\t}\n"
          "\t// bao cua luoi (mps): [x0, x1) x [y0, y1). Tra false khi chua co luoi.\n"
          "\tbool LuoiPhamViMps(int& x0, int& y0, int& x1, int& y1) const\n"
          "\t{\n"
          "\t\tif (!m_bHavePath || !m_nGridTotal)\n"
          "\t\t\treturn false;\n"
          "\t\tif (m_hLoadPathGrid && WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_TIMEOUT)\n"
          "\t\t\treturn false;\n"
          "\t\tx0 = m_nRegionBeginX * REGION_GRID_WIDTH * 32;\n"
          "\t\ty0 = m_nRegionBeginY * REGION_GRID_HEIGHT * 32;\n"
          "\t\tx1 = x0 + m_nGridW * REGION_GRID_WIDTH * 32;\n"
          "\t\ty1 = y0 + m_nGridH * REGION_GRID_HEIGHT * 32;\n"
          "\t\treturn true;\n"
          "\t}\n")
h.them_sau(NEO_HT, KHOI_H, "LuoiOCoDiDuoc")
h.ghi()

# =====================================================================
# 2. CoreShell.cpp
# =====================================================================
c = Tep(os.path.join(ROOT, "CoreShell.cpp"))

# --- 2a. len ngua ---
KHOI_NGUA = """// (03/09) Con tro cau hinh cua may auto dang goi DT_WalkTo (moi *_Process ghi
// vao dau nhip) - de ham di duong DUNG CHUNG biet nguoi choi co cho len ngua khong.
static const autoData* s_pApDiDuong = NULL;

// (03/09) CHI len ngua khi dang DI DUONG that su. Truoc day cu DT_WalkTo (tru pha
// farm Da Tau) la len ngua, trong khi may PK (o 'Xuong ngua') va may danh (o ngua
// tab Chien dau = xuong) lai xuong moi 2 giay => trong tran Tong Kim len/xuong
// lien tuc ~2,5 giay/lan, cat chieu, doi toc do (chu game bao 03/09). Luat:
//   - cau hinh noi XUONG (bPKDownHorse hoac nSelFHorse == 2): khong bao gio tu len;
//   - may nao dang TRONG TRAN (hold == 2: TK / CT / LD / HD / ST): khong len;
//   - pha farm Da Tau: khong len (luat cu 19/08);
//   - con lai (di duong toi NPC / Xa Phu / ve thanh): len nhu cu.
static bool DT_DuocLenNgua(const ExtAuto& ea)
{
	if (s_pApDiDuong && (s_pApDiDuong->bPKDownHorse || s_pApDiDuong->nSelFHorse == 2))
		return false;
	if (ea.nTKHold == 2 || ea.nCTHold == 2 || ea.nLDHold == 2 || ea.nHDHold == 2 || ea.nSTHold == 2)
		return false;
	if (ea.nDTPhase == DTP_FARM)
		return false;
	return true;
}

"""
c.them_truoc("// di bo trong map; tra 1 khi da toi gan (nNear mps)\nstatic int DT_WalkTo(", KHOI_NGUA, "static bool DT_DuocLenNgua(")
c.thay("\tif (ea.nDTPhase != DTP_FARM)\n\t\tDT_Ride(nPlayerIdx);",
       "\tif (DT_DuocLenNgua(ea))\n\t\tDT_Ride(nPlayerIdx);", "DT_DuocLenNgua(ea)")
for may in ("DT", "TK", "CT", "LD", "HD", "ST"):
    neo = ("static int %s_Process(int nPlayerIdx, const autoData* pAp, UINT uCurTime)\n{\n"
           "\tExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;\n" % may)
    dau = neo + "\ts_pApDiDuong = pAp;"
    c.them_sau(neo, "\ts_pApDiDuong = pAp;\t// (03/09) cho DT_WalkTo biet cau hinh ngua\n", c.N(dau))

# --- 2b. chan chieu client se tu choi ---
KHOI_CAM = """// (03/09) [CHIEU-CAM] KHONG gui chieu ma chinh client se tu choi (sai vu khi /
// dang cuoi ngua / bi im lang / quan he muc tieu): moi lan gui bi tu choi la mot
// lan Exit -> DoStand cat dut duong chay (wauto-6a do: chieu 1967 Tran Bien Thuy
// - Vu Hon he THUAN, EqtLimit 8 - bi tu choi 282 lan / 12,5 phut vi nhan vat cam
// DAO, moi lan dung 560 ms roi bi nan). Hoi truoc bang dung ham kiem cua client
// (KSkill::CanCastSkill), truot thi cam chieu do 30 giay va coi nhu 'da ban' de
// bang ket hop sang khe ke tiep; het han thu lai (doi vu khi / xuong ngua la dung duoc).
#define WA_CHIEUCAM_SO	16
static UINT s_uChieuCamId[WA_CHIEUCAM_SO];
static UINT s_uChieuCamHan[WA_CHIEUCAM_SO];
static bool WA_ChieuBiCam(KSkill* pSkill, int nSkill, int nSelf, int nTG, UINT uCurTime)
{
	int nTrong = -1;
	for (int i = 0; i < WA_CHIEUCAM_SO; ++i)
	{
		if (s_uChieuCamId[i] == (UINT)nSkill)
		{
			if ((int)(s_uChieuCamHan[i] - uCurTime) > 0)
				return true;
			nTrong = i;
			break;
		}
		if (nTrong < 0 && (!s_uChieuCamId[i] || (int)(s_uChieuCamHan[i] - uCurTime) <= 0))
			nTrong = i;
	}
	int p1 = -1, p2 = nTG;
	if (!pSkill || pSkill->CanCastSkill(nSelf, p1, p2))
		return false;
	if (nTrong < 0)
		nTrong = 0;
	s_uChieuCamId[nTrong] = (UINT)nSkill;
	s_uChieuCamHan[nTrong] = uCurTime + 30000u;
	AUTOLOG("[CHIEU-CAM] t=%u skill=%d bi client tu choi (vu khi / ngua / quan he / im lang) - cam 30 giay, sang khe khac", uCurTime, nSkill);
	return true;
}

"""
c.them_truoc("static int TK_Process(int nPlayerIdx, const autoData* pAp, UINT uCurTime)\n{", KHOI_CAM, "static bool WA_ChieuBiCam(")

CU_PK = ("\t\t\t\t\t\t\tif(!bCamBan)\n"
         "\t\t\t\t\t\t\t{\n"
         "\t\t\t\t\t\t\t\tNpc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);\n"
         "\t\t\t\t\t\t\t\tSendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);\n"
         "\t\t\t\t\t\t\t\tbBanRoi = true;\n"
         "\t\t\t\t\t\t\t}\n")
MOI_PK = ("\t\t\t\t\t\t\tif(!bCamBan)\n"
          "\t\t\t\t\t\t\t{\n"
          "\t\t\t\t\t\t\t\tif(WA_ChieuBiCam(pSkill, nMainSkill, nNpcIdx, nTGNpcIdx, uCurTime))\n"
          "\t\t\t\t\t\t\t\t\tbBanRoi = true;\t// (03/09) coi nhu da ban de sang khe ke\n"
          "\t\t\t\t\t\t\t\telse\n"
          "\t\t\t\t\t\t\t\t{\n"
          "\t\t\t\t\t\t\t\t\tNpc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);\n"
          "\t\t\t\t\t\t\t\t\tSendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);\n"
          "\t\t\t\t\t\t\t\t\tbBanRoi = true;\n"
          "\t\t\t\t\t\t\t\t}\n"
          "\t\t\t\t\t\t\t}\n")
c.thay(CU_PK, MOI_PK, "PK: WA_ChieuBiCam", tatca=True)
CU_FT = ("\t\t\t\t\telse if(nDist < nSkillRadius)\n"
         "\t\t\t\t\t{\n"
         "\t\t\t\t\t\tNpc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);\n"
         "\t\t\t\t\t\tSendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);\n"
         "\t\t\t\t\t\tbBanRoi = true;\n"
         "\t\t\t\t\t}\n")
MOI_FT = ("\t\t\t\t\telse if(nDist < nSkillRadius)\n"
          "\t\t\t\t\t{\n"
          "\t\t\t\t\t\tif(WA_ChieuBiCam(pSkill, nMainSkill, nNpcIdx, nTGNpcIdx, uCurTime))\n"
          "\t\t\t\t\t\t\tbBanRoi = true;\t// (03/09) coi nhu da ban de sang khe ke\n"
          "\t\t\t\t\t\telse\n"
          "\t\t\t\t\t\t{\n"
          "\t\t\t\t\t\t\tNpc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);\n"
          "\t\t\t\t\t\t\tSendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);\n"
          "\t\t\t\t\t\t\tbBanRoi = true;\n"
          "\t\t\t\t\t\t}\n"
          "\t\t\t\t\t}\n")
c.thay(CU_FT, MOI_FT, "FIGHT: WA_ChieuBiCam")

# --- 2c. TK_ChonDich: tham so nLoai (0 nhu cu, 1 chi tuong NPC, 2 chi nguoi) ---
c.thay("static int TK_ChonDich(int nPlayerIdx, const autoData* pAp)\n{",
       "// (03/09) nLoai: 0 = nhu cu (gan nhat, tuong truoc neu 'Uu tien'), 1 = CHI tuong\n"
       "// NPC trong bang g_TKQuanRes, 2 = CHI nguoi choi khac mau.\n"
       "static int TK_ChonDich(int nPlayerIdx, const autoData* pAp, int nLoai)\n{", "TK_ChonDich(int nPlayerIdx, const autoData* pAp, int nLoai)")
c.thay("\t\tif (Npc[nIdx].m_Kind == kind_player)\n\t\t{\n\t\t\tif (!pAp->bPKPlayer)\n\t\t\t\tcontinue;\n\t\t}\n\t\telse if (!pAp->bPKNpc)\n\t\t\tcontinue;\n",
       "\t\tif (Npc[nIdx].m_Kind == kind_player)\n\t\t{\n\t\t\tif (!pAp->bPKPlayer || nLoai == 1)\n\t\t\t\tcontinue;\n\t\t}\n\t\telse if (!pAp->bPKNpc || nLoai == 2)\n\t\t\tcontinue;\n"
       "\t\tif (nLoai == 1)\n\t\t{\t// chi nhan TUONG dich\n\t\t\tint q = 0;\n\t\t\tfor (; q < TK_QUAN_COUNT; ++q)\n\t\t\t\tif (Npc[nIdx].m_NpcSettingIdx == (int)g_TKQuanRes[q])\n\t\t\t\t\tbreak;\n\t\t\tif (q >= TK_QUAN_COUNT)\n\t\t\t\tcontinue;\n\t\t}\n",
       "TK_ChonDich loc nLoai")

# --- 2d. RAO MAP ---
MSG_RAO = tcvn3("<color=Cyan>Kh\u00f4ng th\u1ea5y \u0111\u1ecbch quanh \u0111\u00e2y - r\u1ea3o map t\u00ecm \u0111\u1ecbch.")
KHOI_RAO = """
// ===================== (03/09) RAO MAP TIM DICH =====================
// Chu game: "WAuto tu di chuyen tim doi thu trong Tong Kim HET ban do, thay vi chay
// toa do co dinh". Khong the hoi may chu vi tri nguoi choi (khong co goi nao - da mo
// auto Thai: no cung chi doc mang NPC client + toa do nguoi dung ghi), nen phai TU DI:
// chia luoi A* client cua map tran (SubWorld[0].LuoiOCoDiDuoc, nap tu maps/<id>.fp
// khi vao map) thanh o vuong TK_RAO_O x TK_RAO_O o luoi, moi o giu MOT diem di duoc
// gan tam o nhat. Moi khi khong thay dich: chon o CHUA THAM (hoac tham da lau) gan
// nhat, nghieng ve nua ban do DICH; toi noi hoac qua han thi danh dau da tham; dang
// di ma tang san / tang danh thay ai thi chung chiem quyen ngay (thu tu TKP_FIGHT).
// Diem cuoi cung THAY nguoi khac mau duoc di lai truoc (3 phut). Tranh hai hau doanh
// (vao hau doanh dich = chet ngay). Het o thi mo lai tat ca, rao vong moi.
#define TK_RAO_O		20			// canh o rao (o luoi 32 mps) = 640 mps ~ tam PK
#define TK_RAO_MAX		2048		// tran so o rao
#define TK_RAO_THAM		480000		// ms: da tham thi 8 phut sau moi quay lai
#define TK_RAO_LAST		180000		// ms: diem cuoi thay dich con gia tri
#define TK_RAO_HAN		45000		// ms: han di toi mot o
static int   s_nTKRaoMap = 0;
static int   s_nTKRaoSo = 0;
static short s_aTKRaoX[TK_RAO_MAX];	// diem dai dien (o luoi tuyet doi)
static short s_aTKRaoY[TK_RAO_MAX];
static UINT  s_aTKRaoT[TK_RAO_MAX];	// moc lan cuoi tham (0 = chua)
static int   s_nTKRaoDangDi = -1;
static UINT  s_uTKRaoHan = 0;
static int   s_nTKRaoLastX = 0, s_nTKRaoLastY = 0;	// diem cuoi thay nguoi khac mau (mps)
static UINT  s_uTKRaoLastT = 0;
static int   s_nTKRaoLastDi = 0;		// 1 = da quay lai diem do roi
static UINT  s_uTKRaoMsgT = 0;

static void TK_Msg(int nPlayerIdx, const char* szMsg);	// dinh nghia o duoi

static void TK_RaoThayDich(int nIdx, UINT uCurTime)
{
	if (nIdx <= 0)
		return;
	Npc[nIdx].GetMpsPos(&s_nTKRaoLastX, &s_nTKRaoLastY);
	s_uTKRaoLastT = uCurTime ? uCurTime : 1;
	s_nTKRaoLastDi = 0;
}

// dung bang o rao cho map hien tai (mot lan moi map); tra so o dung duoc
static int TK_RaoDung(int nMap)
{
	if (s_nTKRaoMap == nMap && s_nTKRaoSo > 0)
		return s_nTKRaoSo;
	int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	if (!SubWorld[0].LuoiPhamViMps(x0, y0, x1, y1))
		return 0;
	const int nCamAX = TK_O((int)g_TKHauDoanhA.x), nCamAY = TK_O((int)g_TKHauDoanhA.y);
	const int nCamBX = TK_O((int)g_TKHauDoanhB.x), nCamBY = TK_O((int)g_TKHauDoanhB.y);
	const int nOx0 = x0 / 32, nOy0 = y0 / 32, nOx1 = x1 / 32, nOy1 = y1 / 32;
	int nSo = 0;
	for (int oy = nOy0; oy < nOy1 && nSo < TK_RAO_MAX; oy += TK_RAO_O)
	for (int ox = nOx0; ox < nOx1 && nSo < TK_RAO_MAX; ox += TK_RAO_O)
	{
		const int cx = ox + TK_RAO_O / 2, cy = oy + TK_RAO_O / 2;
		int bx = 0, by = 0, bd = 0x7fffffff;
		for (int dy = -TK_RAO_O / 2; dy < TK_RAO_O / 2; ++dy)
		for (int dx = -TK_RAO_O / 2; dx < TK_RAO_O / 2; ++dx)
		{
			const int d = dx * dx + dy * dy;
			if (d >= bd)
				continue;
			if (SubWorld[0].LuoiOCoDiDuoc((cx + dx) * 32 + 16, (cy + dy) * 32 + 16) == 1)
			{
				bd = d;
				bx = cx + dx;
				by = cy + dy;
			}
		}
		if (bd == 0x7fffffff)
			continue;
		const int mx = bx * 32 + 16, my = by * 32 + 16;
		if (g_GetDistance(mx, my, nCamAX, nCamAY) < TK_SAN_CAMTRAI
		 || g_GetDistance(mx, my, nCamBX, nCamBY) < TK_SAN_CAMTRAI)
			continue;
		s_aTKRaoX[nSo] = (short)bx;
		s_aTKRaoY[nSo] = (short)by;
		s_aTKRaoT[nSo] = 0;
		++nSo;
	}
	s_nTKRaoMap = nMap;
	s_nTKRaoSo = nSo;
	s_nTKRaoDangDi = -1;
	s_uTKRaoHan = 0;
	AUTOLOG("[TK-RAO] dung bang o rao map=%d: %d o (luoi o %d..%d x %d..%d, canh %d)", nMap, nSo, nOx0, nOx1, nOy0, nOy1, TK_RAO_O);
	return nSo;
}

// khong thay dich: di rao. Tra 1 = dang rao (da phat lenh di), 0 = khong co luoi.
static int TK_RaoDi(int nPlayerIdx, int nX, int nY, UINT uCurTime)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	const int nMap = SubWorld[0].m_SubWorldID;
	if (!TK_RaoDung(nMap))
		return 0;
	if (!s_uTKRaoMsgT || (int)(uCurTime - s_uTKRaoMsgT) > 120000)
	{
		s_uTKRaoMsgT = uCurTime ? uCurTime : 1;
		TK_Msg(nPlayerIdx, "%s");
	}
	// (a) o quanh minh = DA THAM
	for (int i = 0; i < s_nTKRaoSo; ++i)
		if (g_GetDistance(nX, nY, s_aTKRaoX[i] * 32 + 16, s_aTKRaoY[i] * 32 + 16) < TK_O(TK_RAO_O) / 2 + 100)
			s_aTKRaoT[i] = uCurTime ? uCurTime : 1;
	// (b) diem cuoi thay nguoi khac mau (3 phut) - quay lai truoc
	if (s_uTKRaoLastT && !s_nTKRaoLastDi && (int)(uCurTime - s_uTKRaoLastT) < TK_RAO_LAST)
	{
		if (g_GetDistance(nX, nY, s_nTKRaoLastX, s_nTKRaoLastY) < 300)
			s_nTKRaoLastDi = 1;
		else
		{
			AUTOLOG_EVERY(3000, "[TK-RAO] ve diem cuoi thay dich (%d,%d) me=(%d,%d)", s_nTKRaoLastX, s_nTKRaoLastY, nX, nY);
			DT_WalkTo(nPlayerIdx, s_nTKRaoLastX, s_nTKRaoLastY, 250, uCurTime);
			return 1;
		}
	}
	// (c) o dang di: toi noi / qua han -> danh dau da tham
	if (s_nTKRaoDangDi >= 0 && s_nTKRaoDangDi < s_nTKRaoSo)
	{
		const int tx = s_aTKRaoX[s_nTKRaoDangDi] * 32 + 16, ty = s_aTKRaoY[s_nTKRaoDangDi] * 32 + 16;
		if (g_GetDistance(nX, nY, tx, ty) < 200 || (int)(uCurTime - s_uTKRaoHan) > 0)
		{
			s_aTKRaoT[s_nTKRaoDangDi] = uCurTime ? uCurTime : 1;
			s_nTKRaoDangDi = -1;
		}
		else
		{
			DT_WalkTo(nPlayerIdx, tx, ty, 160, uCurTime);
			return 1;
		}
	}
	// (d) chon o moi: chua tham (hoac tham da lau) gan nhat, nghieng ve nua ban do dich
	int nCamX = 0, nCamY = 0;
	if (ea.nTKThe == 1)
	{
		nCamX = TK_O((int)g_TKHauDoanhB.x);
		nCamY = TK_O((int)g_TKHauDoanhB.y);
	}
	else if (ea.nTKThe == 2)
	{
		nCamX = TK_O((int)g_TKHauDoanhA.x);
		nCamY = TK_O((int)g_TKHauDoanhA.y);
	}
	for (int nThu = 0; nThu < 4; ++nThu)
	{
		int nBest = -1;
		long nBestScore = 0x7fffffff;
		for (int i = 0; i < s_nTKRaoSo; ++i)
		{
			if (s_aTKRaoT[i] && (int)(uCurTime - s_aTKRaoT[i]) < TK_RAO_THAM)
				continue;
			const int tx = s_aTKRaoX[i] * 32 + 16, ty = s_aTKRaoY[i] * 32 + 16;
			long nScore = g_GetDistance(nX, nY, tx, ty);
			if (nCamX)
				nScore += g_GetDistance(tx, ty, nCamX, nCamY) / 3;
			if (nScore < nBestScore)
			{
				nBestScore = nScore;
				nBest = i;
			}
		}
		if (nBest < 0)
		{
			for (int i = 0; i < s_nTKRaoSo; ++i)
				s_aTKRaoT[i] = 0;
			AUTOLOG("[TK-RAO] da rao het %d o - rao lai tu dau", s_nTKRaoSo);
			continue;
		}
		const int tx = s_aTKRaoX[nBest] * 32 + 16, ty = s_aTKRaoY[nBest] * 32 + 16;
		if (SubWorld[0].FindPath(tx, ty) <= 0)
		{	// khong co duong (o kin / dao nho) - bo o nay mot vong
			s_aTKRaoT[nBest] = uCurTime ? uCurTime : 1;
			continue;
		}
		s_nTKRaoDangDi = nBest;
		s_uTKRaoHan = uCurTime + TK_RAO_HAN;
		ea.uTKDestT = 0;
		AUTOLOG("[TK-RAO] o %d/%d -> (%d,%d) me=(%d,%d) the=%d", nBest, s_nTKRaoSo, tx, ty, nX, nY, ea.nTKThe);
		DT_WalkTo(nPlayerIdx, tx, ty, 160, uCurTime);
		return 1;
	}
	return 0;
}
// ===================== HET RAO MAP =====================

""".replace("%s", MSG_RAO, 1)
c.them_truoc("static int TK_Abs(int v)\n", KHOI_RAO, "static int TK_RaoDi(")
c.them_truoc("\tconst UINT uID = Npc[nChon].m_dwID;\n", "\tTK_RaoThayDich(nChon, uCurTime);\t// (03/09) nho diem cuoi thay nguoi khac mau\n", "TK_RaoThayDich(nChon, uCurTime);")

# --- 2e. TKP_FIGHT: thu tu muc tieu moi + rao map ---
TU = ("\t\tif (TK_AnThuoc(nPlayerIdx, pAp, uCurTime))\n"
      "\t\t\treturn 2;\n"
      "\t\t{\n"
      "\t\t\tint nTG = TK_ChonDich(nPlayerIdx, pAp);\n")
DEN = ("\t\tDT_WalkTo(nPlayerIdx, TK_O(ea.nTKDestX), TK_O(ea.nTKDestY), 160, uCurTime);\n"
       "\t\treturn 2;\n"
       "\t}\n"
       "\n"
       "\tcase TKP_END:")
MOI_FIGHT = """		if (TK_AnThuoc(nPlayerIdx, pAp, uCurTime))
			return 2;
		// (03/09 chieu) THU TU MOI - chu game: "tim doi thu KHAC MAU, khong chay toa do":
		//   1. tuong dich (chi khi o 'Uu tien' = Hieu Uy / Pho Tuong / Dai Tuong)
		//   2. NGUOI khac mau trong tam PK co duong nhin -> giao may PK
		//   3. NGUOI khac mau o xa / khuat trong bong bong dong bo -> tang san di toi
		//   4. linh NPC dich trong tam (o 'Danh quai') -> giao may PK
		//   5. khong thay ai: RAO MAP theo luoi A* (TK_RaoDi); chua co luoi thi bang toa do
		{
			int nTG = 0;
			if (pAp->nTKUuTien == 1)
				nTG = TK_ChonDich(nPlayerIdx, pAp, 1);
			if (!nTG)
				nTG = TK_ChonDich(nPlayerIdx, pAp, 2);
			if (!nTG)
			{
				int nAimX = 0, nAimY = 0;
				const int nSan = TK_SanNguoi(nPlayerIdx, pAp, uCurTime, &nAimX, &nAimY);
				if (nSan)
				{
					ea.uNpcID = 0;			// chua danh duoc ai - de may PK khong om id cu
					s_uTKSanQuyen = uCurTime + 700;
					ea.uTKDestT = 0;
					DT_WalkTo(nPlayerIdx, nAimX, nAimY, TK_O(4), uCurTime);
					return 2;
				}
				nTG = TK_ChonDich(nPlayerIdx, pAp, 0);
			}
			if (nTG)
			{
				if (Npc[nTG].m_Kind == kind_player)
					TK_RaoThayDich(nTG, uCurTime);
				// giao muc tieu cho may PK (tab PK) danh - no nhan luon uNpcID nay
				ea.uNpcID = Npc[nTG].m_dwID;
				ea.uTKDestT = 0;
				s_uTKSanQuyen = 0;	// da co dich danh duoc - tra toan quyen cho may PK
				TK_SanBo();
				g_ScenePlace.RemoveFlag();
				ea.uTKNext = uCurTime + 300;
				return 2;
			}
		}
		// (25/08) khong co dich hop le: PHAI xoa muc tieu cu, khong thi may PK con om
		// id cu ma vung vao khong khi. Van tra 2 de con danh tra khi bi danh.
		ea.uNpcID = 0;
		s_uTKSanQuyen = 0;
		if (TK_RaoDi(nPlayerIdx, nX, nY, uCurTime))
			return 2;
		// chua co luoi A* (map chua nap xong) -> cach cu: bang toa do binh doan ben dich
		if (!ea.uTKDestT || uCurTime > ea.uTKDestT
		 || g_GetDistance(nX, nY, TK_O(ea.nTKDestX), TK_O(ea.nTKDestY)) < 200)
			TK_ChonDiem(nPlayerIdx, uCurTime);
		DT_WalkTo(nPlayerIdx, TK_O(ea.nTKDestX), TK_O(ea.nTKDestY), 160, uCurTime);
		return 2;
	}

	case TKP_END:"""
c.thay_doan(TU, DEN, MOI_FIGHT, "TKP_FIGHT: thu tu moi + TK_RaoDi")
c.ghi()
print("XONG" + (" (thu, chua ghi)" if THU else ""))
