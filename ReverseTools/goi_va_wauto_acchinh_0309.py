# -*- coding: utf-8 -*-
"""
goi_va_wauto_acchinh_0309.py - bo va idempotent (03/09/2026): WAuto "Tim ac chinh" +
"Danh cung muc tieu ac chinh" (thiet ke: D:\\GAMEDEVNEW\\THIETKE_WAUTO_ACCHINH_0309.md, chu duyet 1-6).
  Core (worktree, goc rep3-0309): ipc_shared.h (PRG_VITRI, IPCViTri, autoData AC_* SAU 3 truong REP3),
      KPlayer.h (ExtAuto AC), CoreShell.h (ATYPE_ACCHINH sau ATYPE_HIENTHI), CoreShell.cpp (AC_Process,
      case, 2 moc trong TKP_FIGHT), S3Client.cpp (cong nAC).
  WAuto (E:\\Src_Auto_Ngoai, mirror WAutoUI trong worktree): Resource.h (632..641, IDC_INDEX_END 700,
      IDM_ACCHINH_ALL 716), WAuto.rc (tab 15), WAuto.cpp (tab, luu/nap, PRG_VITRI, ghep AC_*, menu chuot phai).
Chay:  python goi_va_wauto_acchinh_0309.py [--thu] [--root D:\\GAMEDEVNEW_wt_acchinh]
Tep TCVN3/GBK -> latin-1; WAuto.cpp/.rc UTF-16. Ba tep .moi phai len cung luc (autoData doi).
"""
import io, sys, os, shutil

ROOT = r"D:\GAMEDEVNEW_wt_acchinh"
for i, a in enumerate(sys.argv):
    if a == "--root" and i + 1 < len(sys.argv):
        ROOT = sys.argv[i + 1]
THU = "--thu" in sys.argv
CORE = os.path.join(ROOT, r"Sources\Core\Src")
S3C = os.path.join(ROOT, r"Sources\S3Client\S3Client.cpp")
WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = os.path.join(ROOT, "WAutoUI")
LOI = []

class Tep(object):
    def __init__(self, path, enc="latin-1"):
        self.path = path; self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []
    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)
    def _dem(self, neo):
        return self.s.count(neo)
    def them_truoc(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        n = self._dem(neo)
        if n != 1:
            LOI.append("%s: neo (%d lan): %r" % (os.path.basename(self.path), n, neo[:70])); return
        self.s = self.s.replace(neo, moi + neo, 1); self.log.append("  them truoc: %s" % dau)
    def them_sau(self, neo, moi, dau):
        neo = self.N(neo); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        n = self._dem(neo)
        if n != 1:
            LOI.append("%s: neo (%d lan): %r" % (os.path.basename(self.path), n, neo[:70])); return
        self.s = self.s.replace(neo, neo + moi, 1); self.log.append("  them sau: %s" % dau)
    def them_sau_dong(self, chua, moi, dau):
        """chen SAU dong (duy nhat) co chua 'chua'"""
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        lines = self.s.split(self.nl)
        idx = [i for i, l in enumerate(lines) if chua in l]
        if len(idx) != 1:
            LOI.append("%s: dong chua %r: %d lan" % (os.path.basename(self.path), chua, len(idx))); return
        lines.insert(idx[0] + 1, moi.rstrip(self.nl))
        self.s = self.nl.join(lines); self.log.append("  them sau dong: %s" % dau)
    def them_truoc_dong(self, chua, moi, dau):
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        lines = self.s.split(self.nl)
        idx = [i for i, l in enumerate(lines) if chua in l]
        if len(idx) != 1:
            LOI.append("%s: dong chua %r: %d lan" % (os.path.basename(self.path), chua, len(idx))); return
        lines.insert(idx[0], moi.rstrip(self.nl))
        self.s = self.nl.join(lines); self.log.append("  them truoc dong: %s" % dau)
    def thay(self, cu, moi, dau=None):
        cu = self.N(cu); moi = self.N(moi)
        if moi in self.s and (dau is None or dau in self.s):
            self.log.append("  da co: %s" % (dau or moi[:50])); return
        n = self._dem(cu)
        if n != 1:
            LOI.append("%s: doan thay (%d lan): %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi, 1); self.log.append("  thay: %s" % (dau or moi[:50]))
    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return
        hb0 = sum(1 for c in self.goc if ord(c) >= 0x80); hb1 = sum(1 for c in self.s if ord(c) >= 0x80)
        print("%s: byte cao %d -> %d" % (os.path.basename(self.path), hb0, hb1))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)

# =====================================================================
# 1. ipc_shared.h (Core worktree + E:) - PRG_VITRI, IPCViTri, autoData AC_*
# =====================================================================
IPC_FIELDS = """	// == AC CHINH (03/09/2026) - PHAI o cuoi struct, SAU khoi HIEN THI cua REP3 ==
	// cau hinh (WAuto luu .dat):
	int		bTimAcChinh;		// o "Tim ac chinh": ac phu di theo ac chinh khi cach hon nAcChinhKC
	int		nAcChinhKC;			// mps, mac dinh 200 (auto Thai)
	int		bAcChinhThanh;		// o "Tim trong thanh": 0 = ac chinh dung trong thanh thi khong theo
	int		bCungMucTieu;		// o "Danh cung muc tieu ac chinh"
	int		bAcChinhVaoMap;		// (dot 2, chua co UI) ac phu tu sang map ac chinh
	char	szAcChinhTen[32];	// ten nhan vat ac chinh ma cua so nay theo ("" = khong)
	// WAuto dien moi vong TRUOC khi gui IPCGameLoop (khong phai cau hinh):
	int		nACLaChinh;			// 1 = cua so nay CHINH LA ac chinh
	int		nACMap;
	int		nACX;
	int		nACY;
	unsigned int uACMucTieu;	// dwID NPC ac chinh dang danh (0 = khong)
	int		nACSong;
	int		nACCamp;
	unsigned int uACTuoi;		// ms ke tu lan WAuto nhan PRG_VITRI cuoi cua ac chinh (0 = chua co)
"""
IPC_CTOR = """		bTimAcChinh = 0;
		nAcChinhKC = 200;
		bAcChinhThanh = 0;
		bCungMucTieu = 0;
		bAcChinhVaoMap = 0;
		szAcChinhTen[0] = 0;
		nACLaChinh = 0;
		nACMap = 0;
		nACX = 0;
		nACY = 0;
		uACMucTieu = 0;
		nACSong = 0;
		nACCamp = 0;
		uACTuoi = 0;
"""
IPC_STRUCT = """// (03/09) AC CHINH: moi cua so gui vi tri + muc tieu len WAuto 300 ms/lan (CoreShell AC_GuiViTri)
struct IPCViTri : public SharedState
{
	unsigned int dwPID;		// Player.m_dwID (khop gameNode.player.dwPID)
	int  nMapId;
	int  nX;				// mps
	int  nY;
	unsigned int uMucTieu;	// dwID NPC dang danh (uu tien ea.uNpcID cua auto, khong co thi m_nPeapleIdx); 0 = khong
	int  bSong;				// con song (mau > 0, khong do_death/do_revive)
	int  nCamp;				// m_CurrentCamp (Tong Kim: 1 Tong / 2 Kim)
};

"""
def va_ipc(path):
    t = Tep(path)
    t.them_sau_dong("\tPRG_TEAMNAMELIST,", "\tPRG_VITRI,\t\t// (03/09) vi tri + muc tieu cua cua so (tinh nang Ac chinh), 300 ms/lan", "PRG_VITRI,")
    t.them_sau_dong("int\t\tnWAMissleIndex;", IPC_FIELDS, "int\t\tbTimAcChinh;")
    t.them_sau_dong("\t\tnWAMissleIndex = 1;", IPC_CTOR, "\t\tnAcChinhKC = 200;")
    t.them_truoc("struct IPCHideGame : public SharedState\n", IPC_STRUCT, "struct IPCViTri : public SharedState")
    t.ghi()

# =====================================================================
# 2. KPlayer.h - ExtAuto
# =====================================================================
def va_kplayer():
    t = Tep(os.path.join(CORE, "KPlayer.h"))
    t.them_sau_dong("UINT uCTMuaT;", """	// == AC CHINH (03/09) ==
	int  nACHold;        // 0 tha may / 1 dang di theo ac chinh / 2 dang giao muc tieu ac chinh cho may PK
	UINT uACNext;        // nhip 300 ms
	int  nACMyX;         // do ket
	int  nACMyY;
	UINT uACMoveT;
	int  nACRepath;
	UINT uACNghi;        // ket -> nghi toi moc nay""", "int  nACHold;")
    t.them_sau_dong("\t\tuCTMuaT = 0;", """		nACHold = 0;
		uACNext = 0;
		nACMyX = 0;
		nACMyY = 0;
		uACMoveT = 0;
		nACRepath = 0;
		uACNghi = 0;""", "\t\tnACHold = 0;")
    t.ghi()

# =====================================================================
# 3. CoreShell.h - ATYPE_ACCHINH
# =====================================================================
def va_coreshell_h():
    t = Tep(os.path.join(CORE, "CoreShell.h"))
    t.them_sau_dong("\tATYPE_HIENTHI,", "\tATYPE_ACCHINH,\t\t// (03/09) may Ac chinh: gui vi tri len WAuto + ac phu di theo / danh cung muc tieu (AC_Process)", "ATYPE_ACCHINH,")
    t.ghi()

# =====================================================================
# 4. CoreShell.cpp - AC_Process + case + 2 moc TKP_FIGHT
# =====================================================================
AC_BLOCK = """// ===================== (03/09) AC CHINH - ac phu di theo + danh cung muc tieu =====================
// Thiet ke: D:\\GAMEDEVNEW\\THIETKE_WAUTO_ACCHINH_0309.md (hoc auto Thai: doc vi tri ac chinh -> RUNTO;
// WAuto khong doc bo nho ma dung IPC san). Moi cua so gui IPCViTri len WAuto 300 ms/lan; WAuto ghep
// vi tri + muc tieu cua ac chinh vao autoData (AC_*) cua tung ac phu; may nay:
//   - cung muc tieu: NpcSet.SearchID(uACMucTieu) thay duoc -> ea.uNpcID, tra 2 (may PK danh);
//   - di theo: cung map va cach > nAcChinhKC -> DT_WalkTo diem cach ac chinh AC_DUNG_CACH, tra 1;
//   - con lai tra 0 (auto thuong). Ac chinh (nACLaChinh) chi gui vi tri.
// Trong Tong Kim: TKP_FIGHT goi AC_CungMucTieu truoc buoc 1 va AC_DiTheo truoc "hoi may chu / rao map".
#define AC_NHIP			300
#define AC_DUNG_CACH	150		// dung cach ac chinh (Thai: -150)
#define AC_TUOI_MAX		5000	// ms: qua 5 s khong co tin = mat ac chinh
static UINT s_uACViTriT = 0;

static void AC_GuiViTri(int nPlayerIdx, UINT uCurTime)
{
	if (s_uACViTriT && (int)(uCurTime - s_uACViTriT) < AC_NHIP)
		return;
	s_uACViTriT = uCurTime ? uCurTime : 1;
	const int nSelf = Player[nPlayerIdx].m_nIndex;
	if (nSelf <= 0)
		return;
	IPCViTri v;
	v.CmdID = PRG_VITRI;
	v.Size = sizeof(IPCViTri);
	v.dwPID = Player[nPlayerIdx].m_dwID;
	v.nMapId = SubWorld[0].m_SubWorldID;
	Npc[nSelf].GetMpsPos(&v.nX, &v.nY);
	UINT uMT = Player[nPlayerIdx].m_sExtAuto.uNpcID;
	if (!uMT)
	{
		const int nT = Player[nPlayerIdx].GetTargetNpc();
		if (nT > 0 && nT < MAX_NPC && nT != nSelf)
			uMT = Npc[nT].m_dwID;
	}
	v.uMucTieu = uMT;
	v.bSong = (Npc[nSelf].m_CurrentLife > 0 && Npc[nSelf].m_Doing != do_death && Npc[nSelf].m_Doing != do_revive) ? 1 : 0;
	v.nCamp = Npc[nSelf].m_CurrentCamp;
	SendDataToTool(&v, sizeof(IPCViTri));
}

// ac chinh hop le de theo (cung map, con song, tin con moi)?
static int AC_CoAcChinh(const autoData* pAp, int nMap)
{
	if (!pAp->szAcChinhTen[0] || pAp->nACLaChinh)
		return 0;
	if (!pAp->uACTuoi || (int)pAp->uACTuoi > AC_TUOI_MAX)
		return 0;
	if (!pAp->nACSong)
		return 0;
	if (pAp->nACMap != nMap)
		return 0;
	if (pAp->nACX <= 0 || pAp->nACY <= 0)
		return 0;
	return 1;
}

// giao muc tieu cua ac chinh cho may PK neu THAY DUOC; tra 1 = da giao (ea.uNpcID)
static int AC_CungMucTieu(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	if (!pAp->bCungMucTieu || !pAp->uACMucTieu)
		return 0;
	const int nSelf = Player[nPlayerIdx].m_nIndex;
	const int nIdx = NpcSet.SearchID(pAp->uACMucTieu);
	if (nIdx <= 0 || nIdx == nSelf || Npc[nIdx].m_RegionIndex < 0)
		return 0;
	if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive || Npc[nIdx].m_CurrentLife <= 0)
		return 0;
	if (NpcSet.GetRelation(nSelf, nIdx) != relation_enemy)
		return 0;
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	if (ea.uNpcID != pAp->uACMucTieu)
		AUTOLOG("[AC] cung muc tieu ac chinh id=%u idx=%d", pAp->uACMucTieu, nIdx);
	ea.uNpcID = pAp->uACMucTieu;
	return 1;
}

// di toi ac chinh khi cach hon nguong; tra 1 = dang di (da phat lenh)
static int AC_DiTheo(int nPlayerIdx, const autoData* pAp, int nX, int nY, UINT uCurTime)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	const int d = g_GetDistance(nX, nY, pAp->nACX, pAp->nACY);
	int nKC = pAp->nAcChinhKC;
	if (nKC < 100)
		nKC = 100;
	if (d <= nKC)
		return 0;
	if (ea.uACNghi && (int)(uCurTime - ea.uACNghi) < 0)
		return 0;
	// diem dich: tren doan noi, cach ac chinh AC_DUNG_CACH; lech nho theo dwID de cac ac phu khong chong o
	int tx = pAp->nACX, ty = pAp->nACY;
	if (d > AC_DUNG_CACH)
	{
		tx = pAp->nACX + (nX - pAp->nACX) * AC_DUNG_CACH / d;
		ty = pAp->nACY + (nY - pAp->nACY) * AC_DUNG_CACH / d;
	}
	const int k = (int)(Player[nPlayerIdx].m_dwID % 5u);
	tx += (k - 2) * 40;
	ty += ((k * 3) % 5 - 2) * 40;
	// do ket: 3 giay khong nhuc nhich -> tinh lai duong (2 lan) -> nghi 10 giay
	if (g_GetDistance(nX, nY, ea.nACMyX, ea.nACMyY) >= 32)
	{
		ea.nACMyX = nX;
		ea.nACMyY = nY;
		ea.uACMoveT = uCurTime;
		ea.nACRepath = 0;
	}
	else if (ea.uACMoveT && (int)(uCurTime - ea.uACMoveT) > 3000)
	{
		if (ea.nACRepath < 2)
		{
			g_ScenePlace.RemoveFlag();
			ea.uDTPath = 0;
			++ea.nACRepath;
			ea.uACMoveT = uCurTime;
		}
		else
		{
			ea.uACNghi = uCurTime + 10000u;
			ea.nACRepath = 0;
			ea.uACMoveT = uCurTime;
			AUTOLOG("[AC] ket khi di theo ac chinh (%d,%d) me=(%d,%d) - nghi 10 giay", pAp->nACX, pAp->nACY, nX, nY);
			return 0;
		}
	}
	DT_WalkTo(nPlayerIdx, tx, ty, 100, uCurTime);
	AUTOLOG_EVERY(2000, "[AC] theo ac chinh (%d,%d) d=%d -> dich (%d,%d) me=(%d,%d)", pAp->nACX, pAp->nACY, d, tx, ty, nX, nY);
	return 1;
}

// tra 0 tha may / 1 dang di theo (cam lai) / 2 da giao muc tieu (may PK danh)
static int AC_Process(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	s_pApDiDuong = pAp;
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return 0;
	AC_GuiViTri(nPlayerIdx, uCurTime);		// moi cua so (ke ca ac chinh) bao vi tri len WAuto
	if (pAp->nACLaChinh || (!pAp->bTimAcChinh && !pAp->bCungMucTieu))
	{
		ea.nACHold = 0;
		return 0;
	}
	// may su kien dang cam lai (Tong Kim tu goi AC_* trong TKP_FIGHT) -> khong tranh quyen
	if (ea.nTKHold || ea.nCTHold || ea.nLDHold || ea.nHDHold || ea.nSTHold)
	{
		ea.nACHold = 0;
		return 0;
	}
	if (ea.uACNext && (int)(uCurTime - ea.uACNext) < 0)
		return ea.nACHold;
	ea.uACNext = uCurTime + AC_NHIP;
	const int nMap = SubWorld[0].m_SubWorldID;
	if (!AC_CoAcChinh(pAp, nMap))
	{
		if (ea.nACHold)
			AUTOLOG("[AC] mat ac chinh (tuoi=%u song=%d map=%d/%d) - ve auto thuong", pAp->uACTuoi, pAp->nACSong, pAp->nACMap, nMap);
		ea.nACHold = 0;
		return 0;
	}
	if (!pAp->bAcChinhThanh && g_MoveStation.find(nMap) != g_MoveStation.end())
	{	// map co Xa Phu = thanh: o "Tim trong thanh" tat thi khong theo
		ea.nACHold = 0;
		return 0;
	}
	int nX = 0, nY = 0;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
	if (AC_CungMucTieu(nPlayerIdx, pAp, uCurTime))
	{
		ea.nACHold = 2;
		return 2;
	}
	if (pAp->bTimAcChinh && AC_DiTheo(nPlayerIdx, pAp, nX, nY, uCurTime))
	{
		ea.nACHold = 1;
		return 1;
	}
	ea.nACHold = 0;
	return 0;
}
// ===================== HET AC CHINH =====================

"""
def va_coreshell_cpp():
    t = Tep(os.path.join(CORE, "CoreShell.cpp"))
    t.them_truoc("static int TK_Process(int nPlayerIdx, const autoData* pAp, UINT uCurTime)\n{", AC_BLOCK, "static int AC_Process(")
    t.them_truoc_dong("case ATYPE_HIENTHI:", """				case ATYPE_ACCHINH:	// (03/09) Ac chinh (AC_Process)
				{
					return AC_Process(nPlayerIdx, (const autoData*)nParam, uCurTime);
				}""", "case ATYPE_ACCHINH:")
    t.them_truoc_dong("// (03/09 chieu) THU TU MOI - chu game:", """		// (03/09 dem) AC CHINH: ac phu danh CUNG MUC TIEU ac chinh neu thay duoc (truoc moi thu tu)
		if (AC_CoAcChinh(pAp, nMap) && AC_CungMucTieu(nPlayerIdx, pAp, uCurTime))
		{
			TK_XuongNgua(nPlayerIdx, uCurTime);
			ea.uTKDestT = 0;
			s_uTKSanQuyen = 0;
			TK_SanBo();
			g_ScenePlace.RemoveFlag();
			ea.uTKNext = uCurTime + 300;
			return 2;
		}""", "AC CHINH: ac phu danh CUNG MUC TIEU")
    t.them_truoc_dong("// (04/09, dot 5) vi tri dich do MAY CHU bao (ngoai tam nhin client) - truoc rao map", """		// (03/09 dem) AC CHINH: ac phu DI THEO ac chinh thay cho hoi may chu / rao map
		if (pAp->bTimAcChinh && AC_CoAcChinh(pAp, nMap) && AC_DiTheo(nPlayerIdx, pAp, nX, nY, uCurTime))
			return 2;""", "AC CHINH: ac phu DI THEO ac chinh")
    t.ghi()

# =====================================================================
# 5. S3Client.cpp - cong nAC
# =====================================================================
def va_s3client():
    t = Tep(S3C)
    t.thay("\tconst int nBS = nCT ? nCT : (nTK ? nTK : (nLD ? nLD : (nHD ? nHD : nST)));",
           """	// (03/09) AC CHINH: goi khi cua so co ten ac chinh (ac phu) hoac chinh la ac chinh - may tu nhuong
	// khi may su kien dang cam lai (chi gui vi tri). Tra 1 = dang di theo, 2 = da giao muc tieu cho may PK.
	int nAC = 0;
	if(pApData->szAcChinhTen[0] || pApData->nACLaChinh == 1)
		nAC = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ACCHINH, (int)pApData);
	if(nTK || nLD || nHD || nCT || nST)
		nAC = 0;
	const int nBS = nCT ? nCT : (nTK ? nTK : (nLD ? nLD : (nHD ? nHD : (nST ? nST : nAC))));""", "ATYPE_ACCHINH, (int)pApData")
    t.ghi()

# =====================================================================
# 6. Resource.h (E: + mirror)
# =====================================================================
RES_IDS = """// == tab 15 "Ac chinh" (03/09/2026): dai 632..641 lien mach, ShowTab hien ca dai ==
#define IDC_GRP_TAB15		632
#define IDC_STRING_16_AC	633
#define IDC_COMBO_16_AC		634
#define IDC_CHECKBOX_16_TIM	635
#define IDC_STRING_16_KC	636
#define IDC_EDITOR_16_KC	637
#define IDC_CHECKBOX_16_THANH	638
#define IDC_CHECKBOX_16_MT	639
#define IDC_SEP_15A			640
#define IDC_STRING_16_TT	641
"""
def va_resource():
    p = os.path.join(WA_E, "Resource.h")
    t = Tep(p)
    t.them_truoc_dong("#define IDC_INDEX_END", RES_IDS, "IDC_GRP_TAB15")
    t.thay("#define IDC_INDEX_END\t\t\t640", "#define IDC_INDEX_END\t\t\t700", "IDC_INDEX_END\t\t\t700")
    t.them_sau_dong("#define IDC_TABBTN_13", "#define IDM_ACCHINH_ALL		716	// (03/09) menu chuot phai danh sach nhan vat", "IDM_ACCHINH_ALL")
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "Resource.h"))

# =====================================================================
# 7. WAuto.rc (UTF-16) - tab 15
# =====================================================================
RC_BLOCK = """		GROUPBOX "Ac chính · ac phụ đi theo và đánh cùng mục tiêu", IDC_GRP_TAB15, 2, 112, 156, 122
		LTEXT "Ac chính:", IDC_STRING_16_AC, 4, 125, 36, 10, SS_CENTERIMAGE
		COMBOBOX IDC_COMBO_16_AC, 42, 124, 114, 120, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
		CONTROL "Tìm ac chính khi cách hơn", IDC_CHECKBOX_16_TIM, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 138, 96, 10
		EDITTEXT IDC_EDITOR_16_KC, 102, 138, 30, 10, ES_NUMBER | WS_TABSTOP
		LTEXT "mps", IDC_STRING_16_KC, 135, 138, 20, 10, SS_CENTERIMAGE
		CONTROL "Tìm trong thành (ac chính đứng trong thành vẫn theo)", IDC_CHECKBOX_16_THANH, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 150, 152, 10
		CONTROL "Đánh cùng mục tiêu ac chính", IDC_CHECKBOX_16_MT, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 162, 152, 10
		CONTROL "", IDC_SEP_15A, "Static", SS_ETCHEDHORZ, 6, 176, 148, 1
		LTEXT "Chưa chọn ac chính.", IDC_STRING_16_TT, 4, 180, 152, 48
"""
def va_rc():
    p = os.path.join(WA_E, "WAuto.rc")
    t = Tep(p, "utf-16")
    t.them_sau_dong("COMBOBOX IDC_COMBO_15_CAP,", RC_BLOCK, "IDC_GRP_TAB15,")
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.rc"))

# =====================================================================
# 8. WAuto.cpp (UTF-16)
# =====================================================================
CPP_HELPERS = """// ===================== (03/09) AC CHINH - phia WAuto =====================
static UINT s_uACMenuPID = 0;		// procPID cua dong vua chuot phai
static UINT s_uACTTT = 0;

// nap combo "Ac chinh": muc 0 = (khong), con lai = ten cac cua so DANG dang nhap (tru chinh no);
// ten da luu ma dang offline van duoc them de khong mat lua chon
static void AC_NapCombo(HWND hDlg, gameNode& gnode)
{
	HWND hAC = GetDlgItem(hDlg, IDC_COMBO_16_AC);
	if (!hAC)
		return;
	SendMessage(hAC, CB_RESETCONTENT, 0, 0);
	SendMessage(hAC, CB_ADDSTRING, 0, (LPARAM)L"(không)");
	int nSel = 0, nCo = 0;
	for (size_t j = 0; j < m_vGameNode.size(); ++j)
	{
		gameNode& m = m_vGameNode[j];
		if (!m.player.dwPID || !m.player.szName[0] || &m == &gnode)
			continue;
		wchar_t* pN = g_Convert2UCEChar(m.player.szName, strlen(m.player.szName));
		int k = (int)SendMessage(hAC, CB_ADDSTRING, 0, (LPARAM)pN);
		delete [] pN;
		if (!strcmp(m.player.szName, gnode.apdata.szAcChinhTen))
		{
			nSel = k;
			nCo = 1;
		}
	}
	if (gnode.apdata.szAcChinhTen[0] && !nCo)
	{
		wchar_t* pN = g_Convert2UCEChar(gnode.apdata.szAcChinhTen, strlen(gnode.apdata.szAcChinhTen));
		nSel = (int)SendMessage(hAC, CB_ADDSTRING, 0, (LPARAM)pN);
		delete [] pN;
	}
	SendMessage(hAC, CB_SETCURSEL, nSel, 0);
}

// ghep vi tri + muc tieu cua ac chinh vao autoData cua ac phu (goi ngay truoc khi gui IPCGameLoop)
static void AC_GhepChoNode(gameNode& gnode)
{
	autoData& a = gnode.apdata;
	a.nACLaChinh = 0;
	a.uACTuoi = 0;
	if (!a.szAcChinhTen[0])
		return;
	if (gnode.player.dwPID && !strcmp(gnode.player.szName, a.szAcChinhTen))
	{
		a.nACLaChinh = 1;
		return;
	}
	for (size_t j = 0; j < m_vGameNode.size(); ++j)
	{
		gameNode& m = m_vGameNode[j];
		if (!m.player.dwPID || strcmp(m.player.szName, a.szAcChinhTen))
			continue;
		if (!m.player.uViTriT)
			return;
		a.nACMap = m.player.nMapId;
		a.nACX = m.player.nX;
		a.nACY = m.player.nY;
		a.uACMucTieu = m.player.uMucTieu;
		a.nACSong = m.player.bSong;
		a.nACCamp = m.player.nCamp;
		UINT tuoi = timeGetTime() - m.player.uViTriT;
		a.uACTuoi = tuoi ? tuoi : 1;
		return;
	}
}

// dong trang thai tab Ac chinh (cua so dang chon), 500 ms/lan
static void AC_TrangThai(HWND hDlg, gameNode& gnode)
{
	UINT now = timeGetTime();
	if (s_uACTTT && now - s_uACTTT < 500)
		return;
	s_uACTTT = now;
	wchar_t wbuf[200];
	autoData& a = gnode.apdata;
	if (!a.szAcChinhTen[0])
		GVWPRINT(wbuf, L"%s", L"Chưa chọn ac chính (combo trên, hoặc chuột phải tên nhân vật).");
	else if (a.nACLaChinh)
		GVWPRINT(wbuf, L"%s", L"Cửa sổ này LÀ ac chính - các cửa sổ khác đi theo nó.");
	else
	{
		wchar_t* pN = g_Convert2UCEChar(a.szAcChinhTen, strlen(a.szAcChinhTen));
		if (!a.uACTuoi)
			GVWPRINT(wbuf, L"Ac chính %s: mất kết nối / chưa có vị trí.", pN);
		else
		{
			int dx = gnode.player.nX - a.nACX, dy = gnode.player.nY - a.nACY;
			if (dx < 0) dx = -dx;
			if (dy < 0) dy = -dy;
			int d = (dx > dy) ? (dx + dy / 2) : (dy + dx / 2);
			GVWPRINT(wbuf, L"Ac chính %s: map %d (%s), cách ~%d mps, mục tiêu %u, %s.", pN, a.nACMap,
				(a.nACMap == gnode.player.nMapId) ? L"cùng map" : L"KHÁC map", d, a.uACMucTieu,
				a.nACSong ? L"đang sống" : L"đã chết");
		}
		delete [] pN;
	}
	SetDlgItemText(hDlg, IDC_STRING_16_TT, wbuf);
}

"""
CPP_SAVE = """	// tab 15 Ac chinh (03/09) - ten ac chinh luu rieng o CBN_SELCHANGE cua combo
	gnode.apdata.bTimAcChinh = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_16_TIM) == BST_CHECKED);
	GetDlgItemTextA(hDlg, IDC_EDITOR_16_KC, szRootPath, MAX_PATH);
	gnode.apdata.nAcChinhKC = atoi(szRootPath);
	if(gnode.apdata.nAcChinhKC < 100) gnode.apdata.nAcChinhKC = 100;
	if(gnode.apdata.nAcChinhKC > 3000) gnode.apdata.nAcChinhKC = 3000;
	gnode.apdata.bAcChinhThanh = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_16_THANH) == BST_CHECKED);
	gnode.apdata.bCungMucTieu = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_16_MT) == BST_CHECKED);
"""
CPP_UI = """	// tab 15 Ac chinh (03/09)
	CheckDlgButton(hDlg, IDC_CHECKBOX_16_TIM, gnode.apdata.bTimAcChinh?BST_CHECKED:BST_UNCHECKED);
	{ wchar_t wct[16]; _itow_s(gnode.apdata.nAcChinhKC, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_16_KC, wct); }
	CheckDlgButton(hDlg, IDC_CHECKBOX_16_THANH, gnode.apdata.bAcChinhThanh?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(hDlg, IDC_CHECKBOX_16_MT, gnode.apdata.bCungMucTieu?BST_CHECKED:BST_UNCHECKED);
	AC_NapCombo(hDlg, gnode);
"""
CPP_DEF_BODY = """		gnode.apdata.bTimAcChinh = 0;
		gnode.apdata.nAcChinhKC = 200;
		gnode.apdata.bAcChinhThanh = 0;
		gnode.apdata.bCungMucTieu = 0;
		gnode.apdata.bAcChinhVaoMap = 0;
		gnode.apdata.szAcChinhTen[0] = 0;
"""
CPP_DEF = "\t\t// tab 15 Ac chinh (03/09): mac dinh TAT, khoang cach 200 (auto Thai), khong theo trong thanh\n" + CPP_DEF_BODY
CPP_MIGR = ("\t\tif(uOldSize <= offsetof(autoData, bTimAcChinh))\n"
            "\t\t{\t// .dat truoc dot Ac chinh 03/09: dat mac dinh khoi AC (tinh nang van TAT)\n"
            + CPP_DEF_BODY.replace("\t\tgnode", "\t\t\tgnode") + "\t\t}\n")
CPP_TIPS = """\t{ IDC_COMBO_16_AC, L"Nhân vật làm ac chính cho cửa sổ này (chỉ liệt kê các cửa sổ đang đăng nhập trong WAuto). Chuột phải tên nhân vật ở danh sách trên = đặt làm ac chính cho tất cả cửa sổ khác." },
\t{ IDC_CHECKBOX_16_TIM, L"Ac phụ tự đi theo ac chính (cùng map) khi cách xa hơn số mps bên cạnh; dừng cách ~150. Ac chính chết hoặc mất tin quá 5 giây thì thôi theo." },
\t{ IDC_EDITOR_16_KC, L"Khoảng cách (mps) bắt đầu đi tìm ac chính. 100-3000, mặc định 200 (auto Thái: 200)." },
\t{ IDC_CHECKBOX_16_THANH, L"Ac chính đứng trong thành (map có Xa Phu) thì ac phụ vẫn theo. Tắt = trong thành không theo (tránh cả bầy chạy theo khi ac chính về thành)." },
\t{ IDC_CHECKBOX_16_MT, L"Ac phụ đánh đúng con/người mà ac chính đang đánh (khi thấy được trong tầm đồng bộ), giao cho máy PK. Không thấy thì đi theo ac chính trước." },
"""
CPP_SHOWTAB = """\telse if(nTabBtn == 15)
\t{\t// Ac chinh (03/09) - dai ID 632..641 lien mach
\t\tfor(i=IDC_GRP_TAB15;i<=IDC_STRING_16_TT;++i)
\t\t{
\t\t\tShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
\t\t}
\t\tif(m_nCurSel < (int)m_vGameNode.size())
\t\t\tAC_NapCombo(hDlg, m_vGameNode[m_nCurSel]);
\t}
"""
CPP_GRP_OLD = """\tstatic const int nGrpId[15] = { IDC_GRP_TAB0, IDC_GRP_TAB1, IDC_GRP_TAB2,
\t\tIDC_GRP_TAB3, IDC_GRP_TAB4, IDC_GRP_TAB5, IDC_GRP_TAB6, IDC_GRP_TAB7, IDC_GRP_TAB8,
\t\tIDC_GRP_TAB9, IDC_GRP_TAB10, IDC_GRP_TAB11, IDC_GRP_TAB12, IDC_GRP_TAB13, IDC_GRP_TAB14 };
\tstatic const int nSepFrom[15] = { IDC_SEP_0A, IDC_SEP_1A, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4A, IDC_SEP_5A, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8A, IDC_SEP_9A, IDC_SEP_10A, IDC_SEP_11A,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14A };
\tstatic const int nSepTo[15] = { IDC_SEP_0B, IDC_SEP_1B, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4B, IDC_SEP_5B, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8B, IDC_SEP_9B, IDC_SEP_10B, IDC_SEP_11B,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14B };
\tif (nTabBtn >= 0 && nTabBtn <= 14)"""
CPP_GRP_NEW = """\tstatic const int nGrpId[16] = { IDC_GRP_TAB0, IDC_GRP_TAB1, IDC_GRP_TAB2,
\t\tIDC_GRP_TAB3, IDC_GRP_TAB4, IDC_GRP_TAB5, IDC_GRP_TAB6, IDC_GRP_TAB7, IDC_GRP_TAB8,
\t\tIDC_GRP_TAB9, IDC_GRP_TAB10, IDC_GRP_TAB11, IDC_GRP_TAB12, IDC_GRP_TAB13, IDC_GRP_TAB14, IDC_GRP_TAB15 };
\tstatic const int nSepFrom[16] = { IDC_SEP_0A, IDC_SEP_1A, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4A, IDC_SEP_5A, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8A, IDC_SEP_9A, IDC_SEP_10A, IDC_SEP_11A,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14A, IDC_SEP_15A };
\tstatic const int nSepTo[16] = { IDC_SEP_0B, IDC_SEP_1B, IDC_SEP_2A, IDC_SEP_3A,
\t\tIDC_SEP_4B, IDC_SEP_5B, IDC_SEP_6A, IDC_SEP_7A, IDC_SEP_8B, IDC_SEP_9B, IDC_SEP_10B, IDC_SEP_11B,
\t\tIDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14B, IDC_SEP_15A };
\tif (nTabBtn >= 0 && nTabBtn <= 15)"""
CPP_VITRI = """		case PRG_VITRI:
		{	// (03/09) vi tri + muc tieu 300 ms/lan (tinh nang Ac chinh)
			if(p->Size >= sizeof(IPCViTri))
			{
				IPCViTri* pV = (IPCViTri*)p;
				gnode.player.nMapId = pV->nMapId;
				gnode.player.nX = pV->nX;
				gnode.player.nY = pV->nY;
				gnode.player.uMucTieu = pV->uMucTieu;
				gnode.player.bSong = pV->bSong;
				gnode.player.nCamp = pV->nCamp;
				gnode.player.uViTriT = timeGetTime();
			}
		}
		break;
"""
CPP_COMBO_CASE = """			case IDC_COMBO_16_AC:
			{	// (03/09) Ac chinh: muc 0 = (khong), con lai = ten nhan vat (luu ngay)
				if (HIWORD(wParam) == CBN_SELCHANGE && m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				{
					HWND hCombo = (HWND)lParam;
					int idx = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
					gameNode& gn = m_vGameNode[m_nCurSel];
					if (idx <= 0)
						gn.apdata.szAcChinhTen[0] = 0;
					else
					{
						wchar_t wbuf[80];
						char szBuf[80];
						SendMessage(hCombo, CB_GETLBTEXT, idx, (LPARAM)wbuf);
						g_ConvertW2A(wbuf, szBuf, lstrlenW(wbuf));
						lstrcpynA(gn.apdata.szAcChinhTen, szBuf, (int)sizeof(gn.apdata.szAcChinhTen));
					}
					SaveRoleDataFast(gn);
				}
				break;
			}
"""
CPP_MENU_CMD = """			case IDM_ACCHINH_ALL:
			{	// (03/09) cua so vua chuot phai = ac chinh; moi cua so khac theo no
				int nMain = -1;
				for (size_t j = 0; j < m_vGameNode.size(); ++j)
					if (m_vGameNode[j].procPID == s_uACMenuPID && m_vGameNode[j].player.dwPID) { nMain = (int)j; break; }
				if (nMain >= 0)
				{
					for (size_t j = 0; j < m_vGameNode.size(); ++j)
					{
						gameNode& gn = m_vGameNode[j];
						if ((int)j == nMain)
							gn.apdata.szAcChinhTen[0] = 0;
						else
							lstrcpynA(gn.apdata.szAcChinhTen, m_vGameNode[nMain].player.szName, (int)sizeof(gn.apdata.szAcChinhTen));
						if (gn.player.dwPID)
							SaveRoleDataFast(gn);
					}
					if (m_nCurSel < (int)m_vGameNode.size())
						AC_NapCombo(hDlg, m_vGameNode[m_nCurSel]);
				}
				break;
			}
"""
CPP_RCLICK = """				if (pnmh->code == NM_RCLICK && row >= 0)
				{	// (03/09) menu chuot phai: dat lam ac chinh cho tat ca cua so khac
					LVITEMW lvi{};
					lvi.mask = LVIF_PARAM;
					lvi.iItem = row;
					if (ListView_GetItem(hCtrl, &lvi))
					{
						s_uACMenuPID = (UINT)lvi.lParam;
						HMENU hMenu = CreatePopupMenu();
						AppendMenuW(hMenu, MF_STRING, IDM_ACCHINH_ALL, L"Đặt làm ac chính cho tất cả cửa sổ khác");
						TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos), 0, hDlg, NULL);
						DestroyMenu(hMenu);
					}
				}
"""
def va_wauto_cpp():
    p = os.path.join(WA_E, "WAuto.cpp")
    t = Tep(p, "utf-16")
    # playerInfo
    t.them_sau_dong("char szHoatDong[48] = {0};", """	// (03/09) Ac chinh: tu PRG_VITRI 300 ms/lan
	UINT uMucTieu = 0;
	int bSong = 1;
	int nCamp = 0;
	UINT uViTriT = 0;""", "UINT uViTriT = 0;")
    # bang tab
    t.thay("#define WA_SO_TAB\t\t15", "#define WA_SO_TAB\t\t16", "WA_SO_TAB\t\t16")
    t.thay('{ L"Điều khiển", { 2, 13, 3, 4, 5, 7 }, 6 },', '{ L"Điều khiển", { 2, 13, 3, 4, 5, 7, 15 }, 7 },', "{ 2, 13, 3, 4, 5, 7, 15 }, 7")
    t.thay('L"H.động", L"Sát thủ", L"Chiêu KH", L"Công Thành",', 'L"H.động", L"Sát thủ", L"Chiêu KH", L"Công Thành", L"Ac chính",', 'L"Ac chính",')
    t.thay("362, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270\t// [REP3 03/09]",
           "362, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 244\t// [REP3 03/09]", "335, 270, 244")
    # helpers truoc UpdateUI
    t.them_truoc("static void UpdateUI(HWND hDlg, gameNode& gnode)\n{", CPP_HELPERS, "static void AC_NapCombo(")
    # luu / nap
    t.them_truoc("\t// tab 14 Cong Thanh Chien (03/09)\n\tgnode.apdata.bCongThanh = (IsDlgButtonChecked", CPP_SAVE, "gnode.apdata.bTimAcChinh = (IsDlgButtonChecked")
    t.them_truoc("\t// tab 14 Cong Thanh Chien (03/09)\n\tCheckDlgButton(hDlg, IDC_CHECKBOX_15_ON,", CPP_UI, "CheckDlgButton(hDlg, IDC_CHECKBOX_16_TIM,")
    t.them_truoc("\t\t// tab 14 Cong Thanh Chien (03/09): mac dinh TAT;", CPP_DEF, "// tab 15 Ac chinh (03/09): mac dinh TAT")
    t.them_truoc("\t\tif(uOldSize <= offsetof(autoData, bCongThanh))", CPP_MIGR, "offsetof(autoData, bTimAcChinh)")
    t.them_truoc('\t{ IDC_CHECKBOX_15_ON, L"', CPP_TIPS, 'IDC_COMBO_16_AC, L"')
    t.them_truoc("\telse if(nTabBtn == 14)\n", CPP_SHOWTAB, "else if(nTabBtn == 15)")
    t.thay(CPP_GRP_OLD, CPP_GRP_NEW, "nGrpId[16]")
    t.them_sau("\t\t\t\tIDC_CHECKBOX_15_ON,\n", "\t\t\t\tIDC_CHECKBOX_16_TIM,\n", "IDC_CHECKBOX_16_TIM,\n")
    t.thay("IDC_GRP_TAB12, IDC_GRP_TAB14,\n", "IDC_GRP_TAB12, IDC_GRP_TAB14, IDC_GRP_TAB15,\n", "IDC_GRP_TAB15,")
    # WM_COMMAND
    t.them_truoc("\t\t\tcase IDC_CHECKBOX_15_ON:\n", "\t\t\tcase IDC_CHECKBOX_16_TIM:\n\t\t\tcase IDC_CHECKBOX_16_THANH:\n\t\t\tcase IDC_CHECKBOX_16_MT:\n", "case IDC_CHECKBOX_16_TIM:")
    t.them_truoc("\t\t\tcase IDC_EDITOR_15_GIO:\n", "\t\t\tcase IDC_EDITOR_16_KC:\n", "case IDC_EDITOR_16_KC:")
    t.them_truoc("\t\t\tcase IDC_COMBO_15_CAP:\n", CPP_COMBO_CASE, "case IDC_COMBO_16_AC:")
    t.them_truoc("\t\t\tcase IDC_BTN_4_NOP:\n", CPP_MENU_CMD, "case IDM_ACCHINH_ALL:")
    # IPC nhan
    t.them_truoc("\t\tcase PRG_TEAMNAMELIST:\n", CPP_VITRI, "case PRG_VITRI:")
    # ghep + trang thai truoc khi gui gameloop
    t.them_truoc("\t\t\t\t\tIPCGameLoop gl;\n\t\t\t\t\tgl.CmdID = PRT_GAMELOOP;",
                 "\t\t\t\t\tAC_GhepChoNode(gnode);\t// (03/09) ghep vi tri + muc tieu ac chinh vao AC_*\n"
                 "\t\t\t\t\tif(foundIndex == m_nCurSel)\n\t\t\t\t\t\tAC_TrangThai(hDlg, gnode);\n", "AC_GhepChoNode(gnode);")
    # chuot phai
    t.them_sau("\t\t\t\tif (row < 0 && m_nCurSel < (int)m_vGameNode.size())\n\t\t\t\t{\n\t\t\t\t\tSetCurSel(hCtrl, m_nCurSel);\n\t\t\t\t}\n",
               CPP_RCLICK, "AppendMenuW(hMenu, MF_STRING, IDM_ACCHINH_ALL")
    t.ghi()
    if not THU:
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.cpp"))

def main():
    print("== goi_va_wauto_acchinh %s root=%s ==" % ("(THU - khong ghi)" if THU else "", ROOT))
    va_ipc(os.path.join(CORE, "ipc_shared.h"))
    va_ipc(os.path.join(WA_E, "ipc_shared.h"))
    va_kplayer()
    va_coreshell_h()
    va_coreshell_cpp()
    va_s3client()
    va_resource()
    va_rc()
    va_wauto_cpp()
    if not THU:
        shutil.copyfile(os.path.join(WA_E, "ipc_shared.h"), os.path.join(WA_D, "ipc_shared.h"))
    if LOI:
        print("LOI:")
        for l in LOI: print("  " + l)
        sys.exit(1)
    print("XONG")

if __name__ == "__main__":
    main()
