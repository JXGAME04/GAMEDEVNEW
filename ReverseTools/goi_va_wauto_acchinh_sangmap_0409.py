# -*- coding: utf-8 -*-
r"""goi_va_wauto_acchinh_sangmap_0409.py - AC CHINH: ac phu TU QUA XA PHU SANG MAP ac chinh.

Chu game 04/09: "tinh nang tim acc chinh chua hoat dong" + "no chua tu di chuyen den xa phu
len map cua acc chinh va tim acc chinh de theo train".

HAI LOI, day la loi thu hai (loi thu nhat nam ben WAuto.cpp - xem AC_GhepChoNode):

  LOI 2: AC_CoAcChinh tra 0 ngay khi "pAp->nACMap != nMap", tuc la KHAC MAP thi coi nhu khong
  co ac chinh -> may tra 0, ac phu chay auto thuong tai cho. Truong bAcChinhVaoMap da khai bao
  san trong autoData tu dot 2 nhung CHUA DUOC DUNG O DAU (grep CoreShell.cpp = 0 lan).

  Nay: khi ac chinh con song, tin con moi (<= AC_TUOI_MAX) ma o MAP KHAC va nguoi choi bat
  "Ac chinh o map khac thi tu qua Xa Phu sang" -> goi LD_DiThanh de di. LD_DiThanh la may di
  duong san co cua khoi Lien Dau: tu tim Xa Phu, mo thoai, chon "nhung thanh thi da di qua"
  roi chon ten thanh; map khong co Xa Phu thi dung phu ve thanh truoc.
  LD_DiThanh chi toi duoc map CO TEN trong bang g_aDTSapTown[10] (cac thanh thi). Ac chinh dung
  o bai quai ngoai thanh thi KHONG co tuyen Xa Phu -> bao mot lan roi tha may, khong quay vong.

Chay: python goi_va_wauto_acchinh_sangmap_0409.py [--thu]
CoreShell.cpp la TCVN3 -> latin-1; tieng Viet viet giua @@...@@ roi ma hoa bang bang cua skill.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes

CS = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp"
THU = "--thu" in sys.argv


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


# ---- 1. ham phu: ac chinh co hop le NHUNG dang o map khac ----
HAM = r"""
// (04/09) ac chinh con song va tin con moi, NHUNG dang o MAP KHAC?
// Tach rieng voi AC_CoAcChinh (von doi cung map) de con duong sang map ac chinh.
static int AC_KhacMap(const autoData* pAp, int nMap)
{
	if (!pAp->szAcChinhTen[0] || pAp->nACLaChinh)
		return 0;
	if (!pAp->uACTuoi || (int)pAp->uACTuoi > AC_TUOI_MAX)
		return 0;
	if (!pAp->nACSong)
		return 0;
	if (pAp->nACMap <= 0 || pAp->nACMap == nMap)
		return 0;
	return 1;
}
"""

NEO_HAM = "// ac chinh hop le de theo (cung map, con song, tin con moi)?"

# ---- 2. nhanh sang map trong AC_Process ----
CU = """	const int nMap = SubWorld[0].m_SubWorldID;
	if (!AC_CoAcChinh(pAp, nMap))
	{"""

MOI = """	const int nMap = SubWorld[0].m_SubWorldID;
	// (04/09) AC CHINH O MAP KHAC: truoc day may tra 0 ngay -> ac phu dung yen chay auto
	// thuong, dung nhu chu game bao "khong tu di den Xa Phu len map cua ac chinh".
	// Nay bat o "Ac chinh o map khac thi tu qua Xa Phu sang" thi di bang may di duong
	// san co LD_DiThanh (tu tim Xa Phu -> "nhung thanh thi da di qua" -> ten thanh).
	if (pAp->bAcChinhVaoMap && pAp->bTimAcChinh && AC_KhacMap(pAp, nMap))
	{
		if (!DT_SapTownMenu(pAp->nACMap))
		{	// map ac chinh khong co tuyen Xa Phu (bai quai ngoai thanh) - bao MOT LAN
			if (ea.nACHold != 3)
			{
				char szB[160];
				sprintf(szB, "<color=Yellow>@@Ac chính đang ở map@@ %d @@- map này không có tuyến Xa Phu nên không tự sang được.@@", pAp->nACMap);
				DT_Msg(nPlayerIdx, szB);
				ea.nACHold = 3;
			}
			return 0;
		}
		int nDi = LD_DiThanh(nPlayerIdx, pAp, pAp->nACMap, uCurTime);
		if (ea.uLDNext > uCurTime)
			ea.uACNext = ea.uLDNext;		// ton trong nhip noi bo cua LD_DiThanh
		if (nDi < 0)
		{
			ea.uLDHopT = 0;
			if (ea.nACHold != 3)
			{
				DT_Msg(nPlayerIdx, "<color=Yellow>@@Không sang được map của ác chính (hết Thần Hành Phù / không thấy Xa Phu).@@");
				ea.nACHold = 3;
			}
			return 0;
		}
		if (ea.nACHold != 1)
			DT_Msg(nPlayerIdx, "<color=Cyan>@@Ac chính ở map khác - đang qua Xa Phu để sang.@@");
		ea.nACHold = 1;
		return 1;
	}
	if (!AC_CoAcChinh(pAp, nMap))
	{"""

# ---- 3. vao map roi thi xoa dong ho di duong ----
CU2 = """	int nX = 0, nY = 0;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
	if (AC_CungMucTieu(nPlayerIdx, pAp, uCurTime))"""
MOI2 = """	ea.uLDHopT = 0;		// (04/09) da o cung map voi ac chinh - xoa dong ho di duong
	int nX = 0, nY = 0;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
	if (AC_CungMucTieu(nPlayerIdx, pAp, uCurTime))"""


def main():
    print("== goi_va_wauto_acchinh_sangmap_0409 %s ==" % ("(THU)" if THU else ""))
    s = io.open(CS, encoding="latin-1", newline="").read()
    goc = s
    nl = "\r\n"

    if "AC_KhacMap" in s:
        print("  da co AC_KhacMap - bo qua")
    else:
        if s.count(NEO_HAM) != 1:
            print("!! khong thay neo dat ham"); sys.exit(1)
        s = s.replace(NEO_HAM, vn(HAM).replace("\n", nl).lstrip(nl) + nl + NEO_HAM, 1)
        print("  them ham AC_KhacMap")

    for cu, moi, dau in ((CU, MOI, "pAp->bAcChinhVaoMap && pAp->bTimAcChinh"),
                         (CU2, MOI2, "da o cung map voi ac chinh - xoa dong ho")):
        cu = cu.replace("\n", nl)
        moi = vn(moi).replace("\n", nl)
        if dau in s:
            print("  da co: %s" % dau[:45]); continue
        if s.count(cu) != 1:
            print("  !! xuat hien %d lan: %r" % (s.count(cu), cu[:70])); sys.exit(1)
        s = s.replace(cu, moi, 1)
        print("  thay: %s" % dau[:45])

    if s == goc:
        print("khong doi"); return
    cao_g = sum(1 for c in goc if ord(c) > 127)
    cao_m = sum(1 for c in s if ord(c) > 127)
    print("byte cao: %d -> %d" % (cao_g, cao_m))
    if cao_m < cao_g:
        print("!! MAT byte cao - dung ghi"); sys.exit(1)
    if THU:
        print("(--thu: khong ghi)"); return
    io.open(CS, "w", encoding="latin-1", newline="").write(s)
    print("da ghi CoreShell.cpp")


if __name__ == "__main__":
    main()
