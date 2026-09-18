# -*- coding: utf-8 -*-
r"""goi_va_wauto_acchinh_thp_1709b.py - AC CHINH dot b (18/09): bai luyen cong cung phai MO PHU va lan menu.

Chu game 18/09 test: "toi noi dung than hanh phu de di chuyen theo acc chinh nhung toi test thay no
di chuyen thang len map khong bam mo than hanh phu?" -> dot a (17/09) di bai luyen cong bang lenh
c2sdnmbr_movemapid (may chu GotoMapId, khong mo phu). Chu muon THAY ac phu bam mo phu nhu nguoi choi.

Dot b: bo han movemapid. Bai luyen cong di bang thoai cua chinh phu:
  "Su dung thuat than hanh..." -> "Ban do luyen cong tu 20 den 90" -> "Ban do luyen cong 90 tro len." |
  "Ban do Luyen cong 20 den 80." -> (20..80) "Di chuyen den ban do luyen cong cap NN" -> ten bai.
Ten bai + 4 chuoi muc menu lay THANG tu script (KThanHanhTables.h, sinh boi gen_thanhanh_tables.py) de
dung byte TCVN3 (vi du "Khoa Lang" script viet 'oa' + dau hoi, go tay de sai).
Chay: python goi_va_wauto_acchinh_thp_1709b.py [--thu] [--root <worktree>]  (sau goi_va_wauto_acchinh_thp_1709.py)
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes

ROOT = r"D:\GAMEDEVNEW"
for i, a in enumerate(sys.argv):
    if a == "--root" and i + 1 < len(sys.argv):
        ROOT = sys.argv[i + 1]
THU = "--thu" in sys.argv
CS = os.path.join(ROOT, r"Sources\Core\Src\CoreShell.cpp")
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

    def thay_khoang(self, dong_dau, dong_cuoi_khong_gom, moi, dau):
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
        lines[a[0]:b[0]] = (moi[:-len(self.nl)] if moi.endswith(self.nl) else moi).split(self.nl)
        self.s = self.nl.join(lines)
        self.log.append("  thay khoang [%d..%d): %s" % (a[0] + 1, b[0] + 1, dau[:60]))

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        hb0 = sum(1 for c in self.goc if ord(c) >= 0x80)
        hb1 = sum(1 for c in self.s if ord(c) >= 0x80)
        print("%s: byte cao %d -> %d (%d thay doi)" % (os.path.basename(self.path), hb0, hb1, len(self.log)))
        if hb1 < hb0 - 300:
            LOI.append("%s: MAT byte cao %d -> %d" % (os.path.basename(self.path), hb0, hb1))
            return False
        for l in self.log:
            print(l.encode("ascii", "replace").decode("ascii"))
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# 1. include bang menu phu
INC_CU = '#include "KWayPointTables.h"\n'
INC_MOI = INC_CU + '#include "KThanHanhTables.h"\n'

# 2. chu thich dau khoi: duong (a)
CMT_CU = """//   (a) BAI LUYEN CONG - 31 map tab_lv20map..tab_lv90map (= g_GoMapID[] cua tab Hau can): gui
//       c2sdnmbr_movemapid, may chu chay GotoMapId(map) -> nhay ngay, KHONG can thoai (dung duong
//       "Di map luyen cong" cua Hau can, nHomeStep 10). May chu tu kiem phu trong tui, cap toi
//       thieu cua bai (gopos_step3lvXX) va map dang dung co cam phu hay khong.
"""
CMT_MOI = """//   (a) BAI LUYEN CONG - 31 map tab_lv20map..tab_lv90map (bang KThanHanhTables.h sinh tu script):
//       dung phu -> "Su dung thuat than hanh..." -> "Ban do luyen cong tu 20 den 90" -> "90 tro len." |
//       "20 den 80." -> (20..80) "... luyen cong cap NN" -> ten bai -> gopos_step3lvNN (may chu kiem cap).
//       (18/09) Chu game test dot 17/09 thay ac phu "di thang len map khong bam mo than hanh phu" vi
//       dot do di tat bang lenh c2sdnmbr_movemapid (GotoMapId) -> BO, phai mo phu nhu nguoi choi.
"""

# 3. AC_LaBaiLuyen + AC_GuiMoveMapId -> AC_DuongPhu (giu AC_TimMucPhu)
GO_DAU = "static int AC_LaBaiLuyen(int nMap)"
GO_CUOI = "static void AC_ThpXoa(ExtAuto& ea)"
HAM_MOI = r"""static const ACPhuMuc* AC_TimMucPhu(int nMap)
{
	for (int i = 0; i < (int)(sizeof(g_aACPhuMuc) / sizeof(g_aACPhuMuc[0])); ++i)
		if (g_aACPhuMuc[i].nMapId == nMap)
			return &g_aACPhuMuc[i];
	return NULL;
}

// duong bam trong menu phu toi map dich: szTen = muc cuoi (ten thanh / thon / bai), szNhom = muc o
// "chondiadiem1" ("Thanh thi" / "Thon trang" / "Ban do luyen cong tu 20 den 90"); bai luyen cong con
// szCap1 ("90 tro len." | "20 den 80.") va szCap2 ("... luyen cong cap NN", chi 20..80). Tra 0 = phu khong toi.
struct ACPhuDuong { const char* szTen; const char* szNhom; const char* szCap1; char szCap2[64]; };
static int AC_DuongPhu(int nMap, ACPhuDuong* p)
{
	p->szTen = NULL;
	p->szNhom = NULL;
	p->szCap1 = NULL;
	p->szCap2[0] = 0;
	const ACPhuMuc* pMuc = AC_TimMucPhu(nMap);
	if (pMuc)
	{
		p->szTen = pMuc->szMuc;
		p->szNhom = g_aACPhuNhom[pMuc->nNhom];
		return 1;
	}
	for (int i = 0; i < THP_BAI_COUNT; ++i)
	{
		if (g_aTHPBai[i].nMapId != nMap)
			continue;
		p->szTen = g_aTHPBai[i].szTen;
		p->szNhom = THPM_LUYEN;
		p->szCap1 = (g_aTHPBai[i].nCap >= 90) ? THPM_LV90 : THPM_LV2080;
		if (g_aTHPBai[i].nCap < 90)
			sprintf(p->szCap2, "%s%d", THPM_CAP, g_aTHPBai[i].nCap);
		return 1;
	}
	return 0;
}

"""

# 4. trong AC_DiThanHanh: tim duong
TIM_CU = """	const int nBai = AC_LaBaiLuyen(nDestMap);
	const ACPhuMuc* pMuc = nBai ? NULL : AC_TimMucPhu(nDestMap);
	if (!nBai && !pMuc)
	{
		*pnLyDo = 1;
		return 0;
	}
"""
TIM_MOI = """	ACPhuDuong duong;
	if (!AC_DuongPhu(nDestMap, &duong))
	{
		*pnLyDo = 1;
		return 0;
	}
"""

# 5. bo nhanh movemapid + viet lai bo lan thoai
LAN_DAU = "\tif (nBai)"
LAN_CUOI = 'AUTOLOG("[AC-PHU] thoai la (%d muc): %.80s - dong", nAns, szBuf);'
LAN_MOI = r"""	// dung phu roi lan theo thoai - chung cho ca ba duong (a)/(b)/(c)
	if (ea.nACThp == 10 && cap.uDlgSeq != ea.uACDlgSeen)
	{
		ea.uACDlgSeen = cap.uDlgSeq;
		char szBuf[2048];
		char* apAns[24];
		g_StrCpyLen(szBuf, cap.szDlg, sizeof(szBuf));
		int nAns = DT_Split(szBuf, apAns, 24);
		int nOpt;
		// thu tu: muc cuoi (ten thanh/thon/bai - o danh sach ten LAN danh sach cua cua thanh) -> "cap NN"
		// -> "90 tro len." / "20 den 80." -> nhom o chondiadiem1 -> muc "thuat than hanh" o menu chinh.
		// Moi thoai chi chua muc cua tang no va cac chuoi khong lan nhau nen strstr khong bam nham tang.
		if ((nOpt = DT_FindAns(apAns, nAns, duong.szTen)) >= 0)
		{
			DT_Answer(nPlayerIdx, nOpt);
			ea.uACThpT = uCurTime;
			ea.uACNext = uCurTime + 1500;
			AUTOLOG("[AC-PHU] thoai: chon '%s' (muc %d/%d)", apAns[nOpt], nOpt, nAns);
			return 1;
		}
		if ((duong.szCap2[0] && (nOpt = DT_FindAns(apAns, nAns, duong.szCap2)) >= 0)
		 || (duong.szCap1 && (nOpt = DT_FindAns(apAns, nAns, duong.szCap1)) >= 0)
		 || (nOpt = DT_FindAns(apAns, nAns, duong.szNhom)) >= 0
		 || (nOpt = DT_FindAns(apAns, nAns, "@@thuật thần hành@@")) >= 0)
		{
			DT_Answer(nPlayerIdx, nOpt);
			ea.uACThpT = uCurTime;
			ea.uACNext = uCurTime + 900;
			AUTOLOG("[AC-PHU] thoai: chon '%s' (muc %d/%d)", apAns[nOpt], nOpt, nAns);
			return 1;
		}
"""

# 6. log cuoi
LOG_CU = """	AUTOLOG("[AC-PHU] dung phu lan %d/%d - dich '%s' (nhom %d) map=%d (dang o map %d)",
		ea.nACThpTry, AC_THP_THU, pMuc->szMuc, pMuc->nNhom, nDestMap, SubWorld[0].m_SubWorldID);
"""
LOG_MOI = """	AUTOLOG("[AC-PHU] dung phu lan %d/%d - dich '%s' (nhom '%s') map=%d (dang o map %d)",
		ea.nACThpTry, AC_THP_THU, duong.szTen, duong.szNhom, nDestMap, SubWorld[0].m_SubWorldID);
"""


def main():
    print("== goi_va_wauto_acchinh_thp_1709b %s root=%s ==" % ("(THU)" if THU else "", ROOT))
    if not os.path.exists(os.path.join(ROOT, r"Sources\Core\Src\KThanHanhTables.h")):
        print("!! chua co KThanHanhTables.h - chay gen_thanhanh_tables.py --ghi truoc")
        sys.exit(1)
    cs = Tep(CS)
    cs.thay(INC_CU, INC_MOI, '#include "KThanHanhTables.h"')
    cs.thay(CMT_CU, CMT_MOI, "phai mo phu nhu nguoi choi")
    cs.thay_khoang(GO_DAU, GO_CUOI, HAM_MOI, "static int AC_DuongPhu(")
    cs.thay(TIM_CU, TIM_MOI, "if (!AC_DuongPhu(nDestMap, &duong))")
    cs.thay_khoang(LAN_DAU, LAN_CUOI, vn(LAN_MOI), "duong.szCap2[0] && (nOpt = DT_FindAns")
    cs.thay(LOG_CU, LOG_MOI, "duong.szTen, duong.szNhom, nDestMap")
    if LOI:
        for l in LOI:
            print("!! " + l)
        sys.exit(1)
    cs.ghi()
    if LOI:
        for l in LOI:
            print("!! " + l)
        sys.exit(1)


if __name__ == "__main__":
    main()
