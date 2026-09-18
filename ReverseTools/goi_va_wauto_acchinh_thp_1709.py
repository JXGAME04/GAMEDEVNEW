# -*- coding: utf-8 -*-
r"""goi_va_wauto_acchinh_thp_1709.py - AC CHINH: ac phu sang map ac chinh CHI BANG THAN HANH PHU.

Chu game 17/09: "can fix lai cho nick phu di theo ac chinh phai dung than hanh phu de di chuyen
theo sau - khong can di chuyen theo toi xa phu - chi can dung than hanh phu".

Truoc (04/09): ac chinh o map khac -> LD_DiThanh (Xa Phu "Nhung thanh thi da di qua", co phu thi
thu phu 12 s roi van roi ve Xa Phu) hoac AC_DiWayPoint (Xa Phu "Nhung noi da di qua"); map khong
co Xa Phu thi con dung phu VE THANH truoc. Ac phu chay bo toi Xa Phu, ton tien xe, cham.

Nay: AC_DiThanHanh - ba duong, deu bang phu 6/1/1271 (script\item\ib\shenxingfu.lua):
  (a) bai luyen cong (31 map tab_lv20map..tab_lv90map = g_GoMapID[]): gui c2sdnmbr_movemapid ->
      may chu chay GotoMapId(map) - nhay ngay, khong thoai (duong "Di map luyen cong" cua Hau can);
  (b) 7 thanh thi: phu -> "Su dung thuat than hanh..." -> "Thanh thi" -> ten thanh -> cua dau tien;
  (c) 4 thon tran: phu -> "Su dung thuat than hanh..." -> "Thon trang" -> ten thon.
Khong con bat ky buoc Xa Phu nao. AC_DiWayPoint bi go (khong ai goi nua).
Them: o "Trong thanh" tat thi ac chinh dang o map co Xa Phu (thanh) cung KHONG sang - cung luat
voi truong hop cung map (tranh ca bay nhay vao thanh khi ac chinh ve ban do).

Tep: Sources\Core\Src\KPlayer.h (ExtAuto +6 truong, chi client), Sources\Core\Src\CoreShell.cpp
     (TCVN3 -> latin-1, CRLF), WAuto.rc + WAuto.cpp (UTF-16, E:\Src_Auto_Ngoai + mirror WAutoUI).
autoData KHONG doi -> CoreClient.dll.moi va WAuto.exe.moi khong bat buoc len cung luc.
Chay: python goi_va_wauto_acchinh_thp_1709.py [--thu] [--root <worktree>] [--khong-wauto]
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes

ROOT = r"D:\GAMEDEVNEW"
for i, a in enumerate(sys.argv):
    if a == "--root" and i + 1 < len(sys.argv):
        ROOT = sys.argv[i + 1]
THU = "--thu" in sys.argv
KHONG_WAUTO = "--khong-wauto" in sys.argv
CS = os.path.join(ROOT, r"Sources\Core\Src\CoreShell.cpp")
KP = os.path.join(ROOT, r"Sources\Core\Src\KPlayer.h")
WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = os.path.join(ROOT, "WAutoUI")
T = "\t"
LOI = []


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


class Tep(object):
    def __init__(self, path, enc="latin-1"):
        self.path = path
        self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)

    def thay(self, cu, moi, dau):
        cu = self.N(cu)
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:70])
            return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:90]))
            return
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau[:70])

    def xoa(self, cu, dau):
        cu = self.N(cu)
        n = self.s.count(cu)
        if n == 0:
            self.log.append("  da xoa: %s" % dau[:70])
            return
        if n != 1:
            LOI.append("%s: doan xoa xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:90]))
            return
        self.s = self.s.replace(cu, "", 1)
        self.log.append("  xoa: %s" % dau[:70])

    def thay_khoang(self, dong_dau, dong_cuoi_khong_gom, moi, dau):
        """thay CA KHOI tu dong chua dong_dau toi TRUOC dong chua dong_cuoi_khong_gom"""
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:70])
            return
        lines = self.s.split(self.nl)
        a = [i for i, l in enumerate(lines) if dong_dau in l]
        b = [i for i, l in enumerate(lines) if dong_cuoi_khong_gom in l]
        if len(a) != 1 or len(b) != 1 or b[0] <= a[0]:
            LOI.append("%s: khoang %r (%d) .. %r (%d)" % (os.path.basename(self.path), dong_dau, len(a),
                                                          dong_cuoi_khong_gom, len(b)))
            return
        lines[a[0]:b[0]] = moi.rstrip(self.nl).split(self.nl)
        self.s = self.nl.join(lines)
        self.log.append("  thay khoang [%d..%d): %s" % (a[0] + 1, b[0] + 1, dau[:60]))

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        if self.enc == "latin-1":
            hb0 = sum(1 for c in self.goc if ord(c) >= 0x80)
            hb1 = sum(1 for c in self.s if ord(c) >= 0x80)
            print("%s: byte cao %d -> %d (%d thay doi)" % (os.path.basename(self.path), hb0, hb1, len(self.log)))
            if hb1 < hb0 - 300:
                LOI.append("%s: MAT byte cao %d -> %d" % (os.path.basename(self.path), hb0, hb1))
                return False
        else:
            print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log:
            print(l.encode("ascii", "replace").decode("ascii"))
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# =====================================================================
# 1. KPlayer.h - ExtAuto: 6 truong cho luot Than Hanh Phu (chi client, #ifndef _SERVER)
# =====================================================================
KP_CU = "\tUINT uACNghi;        // ket -> nghi toi moc nay\n"
KP_MOI = KP_CU + """\t// (17/09) sang map ac chinh bang Than Hanh Phu (AC_DiThanHanh)
\tint  nACThp;         // buoc: 0 chua bat dau / 1 da gui movemapid / 10 da dung phu, dang lan thoai / <0 = -ly do hong, nghi toi uACThpHan
\tint  nACThpTry;      // so lan gui / dung phu trong luot
\tint  nACThpMap;      // map dich cua luot (ac chinh doi map = lam lai tu dau)
\tUINT uACThpT;        // moc gui / dung phu / tra loi thoai gan nhat
\tUINT uACThpHan;      // han luot (60 s) hoac moc het nghi
\tUINT uACDlgSeen;     // uDlgSeq da xu ly
"""
KP_CTOR_CU = "\t\tuACNghi = 0;\n"
KP_CTOR_MOI = KP_CTOR_CU + """\t\tnACThp = 0;
\t\tnACThpTry = 0;
\t\tnACThpMap = 0;
\t\tuACThpT = 0;
\t\tuACThpHan = 0;
\t\tuACDlgSeen = 0;
"""

# =====================================================================
# 2. CoreShell.cpp - go AC_DiWayPoint, them AC_DiThanHanh
# =====================================================================
CS_DAU_GO = '// (04/09) DI TOI MAP QUA MUC "NHUNG NOI DA DI QUA" CUA XA PHU.'
CS_CUOI_GO = "// (04/09) ac chinh con song va tin con moi, NHUNG dang o MAP KHAC?"

CS_HAM = r"""// ===== (17/09) SANG MAP AC CHINH BANG THAN HANH PHU - KHONG DI XA PHU =====
// Chu game 17/09: "nick phu di theo ac chinh phai dung than hanh phu de di chuyen theo sau -
// khong can di chuyen theo toi xa phu - chi can dung than hanh phu". Thay han hai duong Xa Phu
// cua 04/09 (LD_DiThanh "thanh thi da di qua" + AC_DiWayPoint "noi da di qua") bang phu 6/1/1271.
// Ba duong, tuy map ac chinh dang dung (theo menu cua script\item\ib\shenxingfu.lua):
//   (a) BAI LUYEN CONG - 31 map tab_lv20map..tab_lv90map (= g_GoMapID[] cua tab Hau can): gui
//       c2sdnmbr_movemapid, may chu chay GotoMapId(map) -> nhay ngay, KHONG can thoai (dung duong
//       "Di map luyen cong" cua Hau can, nHomeStep 10). May chu tu kiem phu trong tui, cap toi
//       thieu cua bai (gopos_step3lvXX) va map dang dung co cam phu hay khong.
//   (b) THANH THI (7, THANH_ARRAY): dung phu -> "Su dung thuat than hanh di den noi chi dinh"
//       -> "Thanh thi" -> ten thanh -> muc dau mang ten thanh (Trung Tam; Lam An = "Lam An Nam");
//       toi noi AC_DiTheo di not trong map. Chuoi menu chep tu DTP_CITYHOP (r5) da chay that.
//   (c) THON TRAN (4, THON_ARRAY): dung phu -> "Su dung thuat than hanh..." -> "Thon trang" -> ten thon.
// Map ngoai ba nhom (thon khong co trong menu phu, map su kien...) thi KHONG di, AC_Process bao
// mot lan / 60 s. Khong co buoc Xa Phu nao, khong dung phu ve thanh.
// Tra: 1 = dang di (cam may); 0 = khong di, *pnLyDo: 0 im lang (dang chet), 1 map khong co trong
//      menu phu, 2 khong co phu trong tui, 3 may chu khong cho (map dang dung cam phu / chua du
//      cap / thoai khong ra / het han luot) -> nghi AC_THP_NGHI roi tu thu lai.
#define AC_THP_GO_LAI	4000	// ms: chua doi map / chua ra thoai moi thi gui / dung phu lai
#define AC_THP_THU		3		// so lan gui / dung phu toi da trong mot luot
#define AC_THP_LUOT		60000	// ms: han mot luot
#define AC_THP_NGHI		60000	// ms: luot hong -> nghi roi thu lai

// muc trong menu "Su dung thuat than hanh" cua phu: nNhom 0 = "Thanh thi", 1 = "Thon trang".
// Ten rut ngan de strstr khop CA danh sach ten LAN danh sach cua ("Thanh Do Phu" / "Thanh Do Trung Tam").
struct ACPhuMuc { int nMapId; int nNhom; const char* szMuc; };
static const ACPhuMuc g_aACPhuMuc[11] =
{
	{ 1,   0, "@@Phượng Tường@@" },
	{ 11,  0, "@@Thành Đô@@" },
	{ 162, 0, "@@Đại Lý@@" },
	{ 37,  0, "@@Biện Kinh@@" },
	{ 78,  0, "@@Tương Dương@@" },
	{ 80,  0, "@@Dương Châu@@" },
	{ 176, 0, "@@Lâm An@@" },
	{ 53,  1, "@@Ba Lăng@@" },		// "Ba Lang Huyen"
	{ 20,  1, "@@Giang Tân@@" },	// "Giang Tan Thon"
	{ 121, 1, "@@Long Môn@@" },	// "Long Mon Tran"
	{ 54,  1, "@@Nam Nhạc@@" },	// "Nam Nhac Tran" - khong co Xa Phu, chi phu toi duoc
};
static const char* const g_aACPhuNhom[2] = { "@@Thành thị@@", "@@Thôn trang@@" };

static int AC_LaBaiLuyen(int nMap)
{
	for (int i = 0; i < (int)(sizeof(g_GoMapID) / sizeof(g_GoMapID[0])); ++i)
		if (g_GoMapID[i] == nMap)
			return 1;
	return 0;
}

static const ACPhuMuc* AC_TimMucPhu(int nMap)
{
	for (int i = 0; i < (int)(sizeof(g_aACPhuMuc) / sizeof(g_aACPhuMuc[0])); ++i)
		if (g_aACPhuMuc[i].nMapId == nMap)
			return &g_aACPhuMuc[i];
	return NULL;
}

// gui "nhay toi map bang Than Hanh Phu" - may chu chay GotoMapId (khuon Hau can nHomeStep 10)
static void AC_GuiMoveMapId(int nMap)
{
	char szPack[16];
	DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
	pCmd->ProtocolType = c2s_dynamic_structure;
	pCmd->nBranch = c2sdnmbr_movemapid;
	pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + sizeof(int);
	*(int*)(pCmd + 1) = nMap;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)pCmd, pCmd->m_wLength + 1);
}

static void AC_ThpXoa(ExtAuto& ea)
{
	ea.nACThp = 0;
	ea.nACThpTry = 0;
	ea.nACThpMap = 0;
	ea.uACThpT = 0;
	ea.uACThpHan = 0;
}

static int AC_ThpHong(int nPlayerIdx, int nLyDo, UINT uCurTime, int* pnLyDo)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	AUTOLOG("[AC-PHU] hong (ly do %d, buoc %d, lan %d, map dich %d) - nghi %d giay roi thu lai",
		nLyDo, ea.nACThp, ea.nACThpTry, ea.nACThpMap, AC_THP_NGHI / 1000);
	ea.nACThp = -nLyDo;
	ea.uACThpHan = uCurTime + AC_THP_NGHI;
	*pnLyDo = nLyDo;
	return 0;
}

static int AC_DiThanHanh(int nPlayerIdx, const autoData* pAp, int nDestMap, UINT uCurTime, int* pnLyDo)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	KDaTauCapture& cap = g_sDTCap;
	const int nSelf = Player[nPlayerIdx].m_nIndex;
	*pnLyDo = 0;
	// dang nghi sau luot hong: giu ly do cho AC_Process bao, het nghi thi lam lai tu dau
	if (ea.nACThp < 0)
	{
		if ((int)(uCurTime - ea.uACThpHan) < 0)
		{
			*pnLyDo = -ea.nACThp;
			return 0;
		}
		AC_ThpXoa(ea);
	}
	// ac chinh doi map giua chung -> luot moi
	if (ea.nACThp > 0 && ea.nACThpMap != nDestMap)
		AC_ThpXoa(ea);
	const int nBai = AC_LaBaiLuyen(nDestMap);
	const ACPhuMuc* pMuc = nBai ? NULL : AC_TimMucPhu(nDestMap);
	if (!nBai && !pMuc)
	{
		*pnLyDo = 1;
		return 0;
	}
	if (Npc[nSelf].m_Doing == do_death || Npc[nSelf].m_Doing == do_revive || Npc[nSelf].m_CurrentLife <= 0)
		return 0;		// dang chet: im lang, nhip sau xem lai (hoi sinh do tab Co ban / S3Client lo)
	if (ea.nACThp == 0)
	{
		if (TK_DemThanHanhPhu(nPlayerIdx) <= 0)
		{
			*pnLyDo = 2;
			return 0;
		}
		ea.nACThpMap = nDestMap;
		ea.nACThpTry = 0;
		ea.uACThpHan = uCurTime + AC_THP_LUOT;
	}
	if ((int)(uCurTime - ea.uACThpHan) > 0)
		return AC_ThpHong(nPlayerIdx, 3, uCurTime, pnLyDo);
	if (nBai)
	{	// (a) bai luyen cong: gui movemapid roi doi may chu doi map (AC_KhacMap tu het khi cung map)
		if (ea.nACThp == 1 && (int)(uCurTime - ea.uACThpT) < AC_THP_GO_LAI)
			return 1;
		if (ea.nACThpTry >= AC_THP_THU)
			return AC_ThpHong(nPlayerIdx, 3, uCurTime, pnLyDo);
		AC_GuiMoveMapId(nDestMap);
		ea.nACThp = 1;
		++ea.nACThpTry;
		ea.uACThpT = uCurTime;
		AUTOLOG("[AC-PHU] gui movemapid map=%d lan %d/%d (dang o map %d)", nDestMap, ea.nACThpTry, AC_THP_THU, SubWorld[0].m_SubWorldID);
		return 1;
	}
	// (b)/(c) thanh thi / thon tran: dung phu roi lan theo thoai
	if (ea.nACThp == 10 && cap.uDlgSeq != ea.uACDlgSeen)
	{
		ea.uACDlgSeen = cap.uDlgSeq;
		char szBuf[2048];
		char* apAns[24];
		g_StrCpyLen(szBuf, cap.szDlg, sizeof(szBuf));
		int nAns = DT_Split(szBuf, apAns, 24);
		int nOpt;
		// thu tu: ten thanh/thon (o danh sach ten LAN danh sach cua) -> nhom -> muc "thuat than hanh"
		if ((nOpt = DT_FindAns(apAns, nAns, pMuc->szMuc)) >= 0)
		{
			DT_Answer(nPlayerIdx, nOpt);
			ea.uACThpT = uCurTime;
			ea.uACNext = uCurTime + 1500;
			AUTOLOG("[AC-PHU] thoai: chon '%s' (muc %d/%d)", apAns[nOpt], nOpt, nAns);
			return 1;
		}
		if ((nOpt = DT_FindAns(apAns, nAns, g_aACPhuNhom[pMuc->nNhom])) >= 0
		 || (nOpt = DT_FindAns(apAns, nAns, "@@thuật thần hành@@")) >= 0)
		{
			DT_Answer(nPlayerIdx, nOpt);
			ea.uACThpT = uCurTime;
			ea.uACNext = uCurTime + 900;
			AUTOLOG("[AC-PHU] thoai: chon '%s' (muc %d/%d)", apAns[nOpt], nOpt, nAns);
			return 1;
		}
		AUTOLOG("[AC-PHU] thoai la (%d muc): %.80s - dong", nAns, szBuf);
		CoreDataChanged(GDCNI_UI_ACT, 1, 0);	// thoai khong phai menu phu - dong de khoi ket
	}
	if (ea.nACThp == 10 && (int)(uCurTime - ea.uACThpT) < AC_THP_GO_LAI)
		return 1;
	if (ea.nACThpTry >= AC_THP_THU)
		return AC_ThpHong(nPlayerIdx, 3, uCurTime, pnLyDo);
	ea.uACDlgSeen = cap.uDlgSeq;	// chi xet thoai MOI sau khi dung phu
	if (!Player[nPlayerIdx].m_ItemList.AutoUseItem(TK_ITEM_THP_G, TK_ITEM_THP_D, TK_ITEM_THP_P, nPlayerIdx))
	{
		AC_ThpXoa(ea);
		*pnLyDo = 2;
		return 0;
	}
	ea.nACThp = 10;
	++ea.nACThpTry;
	ea.uACThpT = uCurTime;
	ea.uACNext = uCurTime + 900;
	AUTOLOG("[AC-PHU] dung phu lan %d/%d - dich '%s' (nhom %d) map=%d (dang o map %d)",
		ea.nACThpTry, AC_THP_THU, pMuc->szMuc, pMuc->nNhom, nDestMap, SubWorld[0].m_SubWorldID);
	return 1;
}
"""

# ---- khoi trong AC_Process ----
CS_DAU_KHOI = "// (04/09) AC CHINH O MAP KHAC: truoc day may tra 0 ngay -> ac phu dung yen chay auto"
CS_CUOI_KHOI = "if (!AC_CoAcChinh(pAp, nMap))"

CS_KHOI = r"""	// (17/09) AC CHINH O MAP KHAC -> sang CHI BANG THAN HANH PHU (AC_DiThanHanh), khong Xa Phu.
	// (04/09 cu: LD_DiThanh / AC_DiWayPoint = chay bo toi Xa Phu, chu game bo 17/09.)
	if (pAp->bAcChinhVaoMap && pAp->bTimAcChinh && AC_KhacMap(pAp, nMap))
	{
		// o "Trong thanh" tat: ac chinh dang o map co Xa Phu (thanh) thi khong sang - cung luat voi
		// cung map ben duoi (tranh ca bay nhay vao thanh moi lan ac chinh ve ban do / sua do)
		if (!pAp->bAcChinhThanh && g_MoveStation.find(pAp->nACMap) != g_MoveStation.end())
		{
			ea.nACHold = 0;
			return 0;
		}
		int nLyDo = 0;
		const int nDi = AC_DiThanHanh(nPlayerIdx, pAp, pAp->nACMap, uCurTime, &nLyDo);
		if (nDi <= 0)
		{
			if (nLyDo && (!s_uACBaoLoi || (int)(uCurTime - s_uACBaoLoi) > AC_BAO_LAI))
			{
				s_uACBaoLoi = uCurTime ? uCurTime : 1;
				char szB[200];
				if (nLyDo == 1)
					sprintf(szB, "<color=Yellow>@@Ac chính đang ở map@@ %d @@- Thần Hành Phù không có mục tới map này nên không tự sang được.@@", pAp->nACMap);
				else if (nLyDo == 2)
					strcpy(szB, "<color=Yellow>@@Không có Thần Hành Phù trong túi - không sang được map của ác chính.@@");
				else
					strcpy(szB, "<color=Yellow>@@Dùng Thần Hành Phù sang map ác chính không được (map đang đứng cấm phù / chưa đủ cấp / thoại không ra) - nghỉ 1 phút rồi thử lại.@@");
				DT_Msg(nPlayerIdx, szB);
			}
			ea.nACHold = 0;
			return 0;
		}
		if (ea.nACHold != 1)
		{
			s_uACBaoLoi = 0;		// di duoc roi - lan sau hong thi duoc bao ngay
			DT_Msg(nPlayerIdx, "<color=Cyan>@@Ac chính ở map khác - dùng Thần Hành Phù sang.@@");
		}
		ea.nACHold = 1;
		return 1;
	}
	AC_ThpXoa(ea);		// (17/09) cung map / mat ac chinh / khong sang: luot phu (neu co) ket thuc
"""

CS_LDHOP_CU = "\tea.uLDHopT = 0;\t\t// (04/09) da o cung map voi ac chinh - xoa dong ho di duong\n"

# =====================================================================
# 3. WAuto (UTF-16): nhan o + tooltip + note tab Ac chinh
# =====================================================================
RC_CU = u'CONTROL "Ác chính ở map khác thì tự qua Xa Phu sang", IDC_CHECKBOX_16_VAOMAP,'
RC_MOI = u'CONTROL "Ác chính ở map khác thì dùng Thần Hành Phù sang", IDC_CHECKBOX_16_VAOMAP,'

TIP_CU = (u'{ IDC_CHECKBOX_16_VAOMAP, L"Ac chính đang ở MAP KHÁC thì ac phụ tự tới Xa Phu để sang map đó rồi mới đi tìm. '
          u'Chỉ sang được những map có tuyến Xa Phu (các thành thị); ac chính đứng ở bãi quái ngoài thành thì ac phụ '
          u'đi tới thành gần nhất rồi dừng, dòng trạng thái sẽ báo." },')
TIP_MOI = (u'{ IDC_CHECKBOX_16_VAOMAP, L"Ac chính đang ở MAP KHÁC thì ac phụ dùng THẦN HÀNH PHÙ (phải có trong túi) nhảy sang map đó '
           u'rồi mới đi tìm - KHÔNG chạy tới Xa Phu. Sang được 7 thành thị, 4 thôn trấn (Ba Lăng, Giang Tân, Long Môn, Nam Nhạc) '
           u'và 31 bãi luyện công có trong menu phù; map khác hoặc hết phù thì dòng trạng thái báo, ac phụ đứng auto tại chỗ. '
           u'Ô Trong thành tắt thì ác chính về thành cũng không sang." },')

NOTE_CU = u'còn sống hay đã chết.||LƯU Ý: cửa sổ ĐANG LÀ ác chính'
NOTE_MOI = (u'còn sống hay đã chết.||6. [v] Ác chính ở map khác thì dùng Thần Hành Phù sang (ô trong tab này): '
            u'ác chính đứng ở map khác thì ac phụ dùng Thần Hành Phù (phải có sẵn trong túi) nhảy sang map đó rồi mới đi tìm, '
            u'không chạy tới Xa Phu. Sang được 7 thành thị, 4 thôn trấn và các bãi luyện công có trong menu phù; '
            u'map khác hoặc hết phù thì dòng trạng thái báo và ac phụ đứng auto tại chỗ. Ô Trong thành tắt thì ác chính về thành cũng không sang.'
            u'||LƯU Ý: cửa sổ ĐANG LÀ ác chính')


def main():
    print("== goi_va_wauto_acchinh_thp_1709 %s root=%s ==" % ("(THU)" if THU else "", ROOT))

    kp = Tep(KP)
    kp.thay(KP_CU, KP_MOI, "nACThp;")
    kp.thay(KP_CTOR_CU, KP_CTOR_MOI, "nACThp = 0;")

    cs = Tep(CS)
    cs.thay_khoang(CS_DAU_GO, CS_CUOI_GO, vn(CS_HAM), "static int AC_DiThanHanh(")
    cs.thay_khoang(CS_DAU_KHOI, CS_CUOI_KHOI, vn(CS_KHOI), "AC_DiThanHanh(nPlayerIdx, pAp, pAp->nACMap")
    cs.xoa(CS_LDHOP_CU, "ea.uLDHopT = 0 (04/09) trong AC_Process")

    if LOI:
        for l in LOI:
            print("!! " + l)
        sys.exit(1)
    kp.ghi()
    cs.ghi()
    if LOI:
        for l in LOI:
            print("!! " + l)
        sys.exit(1)

    if KHONG_WAUTO:
        return
    rc = Tep(os.path.join(WA_E, "WAuto.rc"), "utf-16")
    rc.thay(RC_CU, RC_MOI, RC_MOI)
    cp = Tep(os.path.join(WA_E, "WAuto.cpp"), "utf-16")
    cp.thay(TIP_CU, TIP_MOI, u"KHÔNG chạy tới Xa Phu")
    cp.thay(NOTE_CU, NOTE_MOI, u"6. [v] Ác chính ở map khác thì dùng Thần Hành Phù sang")
    if LOI:
        for l in LOI:
            print("!! " + l)
        sys.exit(1)
    d1 = rc.ghi()
    d2 = cp.ghi()
    if not THU:
        for f in ("WAuto.rc", "WAuto.cpp"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI (WAuto.rc, WAuto.cpp)")


if __name__ == "__main__":
    main()
