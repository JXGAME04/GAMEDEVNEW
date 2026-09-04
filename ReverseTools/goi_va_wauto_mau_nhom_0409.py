# -*- coding: utf-8 -*-
r"""goi_va_wauto_mau_nhom_0409.py - TRA LAI MAU PHAN BIET NHOM, MA CHU VAN RO.

Chu game 04/09: "mat luon mau sac phan biet cac tab con va tab me - mau sac cai phan quan trong".

Dot 9 toi bo mau di de chua loi chu mo - bo qua tay. Dot 12 nay tra mau lai theo dung cach:
mau KHONG dat vao cho lam chu kho doc, ma dat vao NEN cua nut, con chu thi lay mau tuong
phan manh voi nen do.

  Nut NHOM (tab me)
      chua chon : nen NHAT cua chinh mau nhom + chu den 26,26,26   (13,5 - 14,5 : 1)
      dang chon : TO DAM mau nhom + chu TRANG                      ( 7,4 -  9,6 : 1)
                  -> nhin mot cai la biet dang o nhom nao
  Tab CON
      chua chon : nen RAT NHAT cua mau NHOM DANG MO + chu den      (15,2 - 15,8 : 1)
                  -> tab con luon mang mau cua nhom me
      dang chon : nen TRANG + vien day 2 px + vach day mau nhom + chu mau nhom dam (6,6 - 9,6:1)

Moi cap chu/nen deu >= 6,6:1, phan lon > 13:1 - tuc la co mau MA VAN ro han bang mau pastel
cu (cu la chu xam 70,70,85 tren nen 250,250,252, nut chi hon nen hop thoai 1,09:1).

Chay: python goi_va_wauto_mau_nhom_0409.py [--thu]
"""
import io
import os
import shutil
import sys

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []

BANG_CU = """static const COLORREF s_crNhomChu[WA_SO_NHOM] = {	// chu nhom khi DANG CHON (tren nen trang)
	RGB(  0,  45,  90),		// Dieu khien - xanh duong	13,79:1
	RGB(  0,  70,  30),		// Hau can    - xanh la		11,08:1
	RGB(105,  48,   0),		// Cai dat    - cam nau		10,38:1
	RGB( 75,  25, 110) };	// Hoat dong  - tim			12,62:1
#define WA_NEN_NHOM_MO	RGB(224, 224, 224)	// nut NHOM chua chon
#define WA_CHU_NHOM_MO	RGB( 26,  26,  26)	// 13,18:1
#define WA_NEN_TAB_MO	RGB(232, 232, 232)	// tab CON chua chon
#define WA_CHU_TAB_MO	RGB( 32,  32,  32)	// 13,30:1
#define WA_NEN_CHON		RGB(255, 255, 255)	// dang chon: SANG HON, nhu tab Windows
#define WA_CHU_TAB_CHON	RGB(  0,   0,   0)	// 21,00:1
#define WA_VIEN_CHON	RGB(  0,  84, 153)	// vien + vach day tab dang chon
#define WA_MAU_VIEN_MO	RGB(120, 120, 120)	// vien nut chua chon (3,6:1 - du thay)"""

BANG_MOI = """// (04/09 dot 12) MAU PHAN BIET NHOM - chu game: "mau sac cai phan quan trong".
// Nguyen tac: mau dat vao NEN, chu lay mau tuong phan MANH voi nen do. Nho vay vua co
// mau phan biet 4 nhom, vua doc ro han bang pastel cu (cu: chu xam tren nen gan trang,
// nut chi hon nen hop thoai 1,09:1 nen coi nhu khong thay nut).
// Ti so tuong phan ghi ben canh tung mau, do theo cong thuc WCAG.
static const COLORREF s_crNhomDam[WA_SO_NHOM] = {	// mau chinh cua nhom (nen khi DANG CHON)
	RGB(  0,  70, 130),		// Dieu khien - xanh duong	chu trang 9,55:1
	RGB(  0,  90,  45),		// Hau can    - xanh la		chu trang 8,40:1
	RGB(133,  60,   0),		// Cai dat    - cam nau		chu trang 7,94:1
	RGB( 95,  45, 140) };	// Hoat dong  - tim			chu trang 9,37:1
static const COLORREF s_crNhomNhat[WA_SO_NHOM] = {	// nut NHOM khi CHUA chon
	RGB(214, 228, 245), RGB(216, 238, 222), RGB(250, 232, 206), RGB(233, 224, 246) };
static const COLORREF s_crTabNhat[WA_SO_NHOM] = {	// tab CON chua chon (theo mau nhom dang mo)
	RGB(233, 240, 250), RGB(234, 246, 237), RGB(253, 243, 228), RGB(244, 239, 251) };
#define WA_CHU_DEN		RGB( 26,  26,  26)	// chu tren moi nen NHAT (13,5 - 15,8 : 1)
#define WA_CHU_TRANG	RGB(255, 255, 255)	// chu tren nen DAM cua nhom dang chon
#define WA_NEN_CHON		RGB(255, 255, 255)	// tab CON dang chon: nen trang cho noi bat
#define WA_MAU_VIEN_MO	RGB(150, 155, 165)	// vien nut chua chon"""

VE_CU = """				// (04/09 dot 9) chu LUON gan den cho de doc (mau Thai: chu den he
				// thong). Mau chi danh dau tab DANG CHON, va tab dang chon phai
				// SANG HON tab thuong (nen trang) - dung chieu cua tab Windows.
				int nNhomVe = bNhom ? nBtn : m_nNhomIndex;
				if (nNhomVe < 0 || nNhomVe >= WA_SO_NHOM)
					nNhomVe = 0;
				COLORREF crNen = bNhom ? WA_NEN_NHOM_MO : WA_NEN_TAB_MO;
				COLORREF crChu = bNhom ? WA_CHU_NHOM_MO : WA_CHU_TAB_MO;
				if (bChon)
				{
					crNen = WA_NEN_CHON;
					crChu = bNhom ? s_crNhomChu[nNhomVe] : WA_CHU_TAB_CHON;
				}"""

VE_MOI = """				// (04/09 dot 12) MAU theo NHOM, chu van ro:
				//   nut NHOM  chua chon = nen NHAT mau nhom + chu den
				//             dang chon = TO DAM mau nhom  + chu TRANG
				//   tab CON   chua chon = nen RAT NHAT mau nhom DANG MO + chu den
				//             dang chon = nen TRANG + vien/vach day mau nhom + chu mau nhom
				int nNhomVe = bNhom ? nBtn : m_nNhomIndex;
				if (nNhomVe < 0 || nNhomVe >= WA_SO_NHOM)
					nNhomVe = 0;
				COLORREF crNen, crChu;
				if (bNhom)
				{
					crNen = bChon ? s_crNhomDam[nNhomVe] : s_crNhomNhat[nNhomVe];
					crChu = bChon ? WA_CHU_TRANG : WA_CHU_DEN;
				}
				else
				{
					crNen = bChon ? WA_NEN_CHON : s_crTabNhat[nNhomVe];
					crChu = bChon ? s_crNhomDam[nNhomVe] : WA_CHU_DEN;
				}"""

VIEN_CU = """					HPEN hVien = CreatePen(PS_SOLID, 1,
						bChon ? WA_VIEN_CHON : WA_MAU_VIEN_MO);"""
VIEN_MOI = """					// vien DAY 2 px khi dang chon - dau hieu thu hai canh mau nen
					HPEN hVien = CreatePen(PS_SOLID, bChon ? 2 : 1,
						bChon ? s_crNhomDam[nNhomVe] : WA_MAU_VIEN_MO);"""

VACH_CU = """				if (bChon)
				{	// vach day 2 px - dau hieu "tab dang mo" quen thuoc cua Windows
					RECT rcV = { rc.left + 1, rc.bottom - 2, rc.right - 1, rc.bottom };
					HBRUSH hV = CreateSolidBrush(WA_VIEN_CHON);
					FillRect(hdc, &rcV, hV);
					DeleteObject(hV);
				}"""
VACH_MOI = """				if (bChon && !bNhom)
				{	// tab CON dang mo: vach day mau nhom (nut NHOM dang mo da to dam
					// ca nen roi nen khong can them vach)
					RECT rcV = { rc.left + 1, rc.bottom - 3, rc.right - 1, rc.bottom };
					HBRUSH hV = CreateSolidBrush(s_crNhomDam[nNhomVe]);
					FillRect(hdc, &rcV, hV);
					DeleteObject(hV);
				}"""

KHUNG_CU = """					SetTextColor(hdcStatic,
						(m_nNhomIndex >= 0 && m_nNhomIndex < WA_SO_NHOM)
						? s_crNhomChu[m_nNhomIndex] : WA_MAU_NHOM_CHU);"""
KHUNG_MOI = """					SetTextColor(hdcStatic,
						(m_nNhomIndex >= 0 && m_nNhomIndex < WA_SO_NHOM)
						? s_crNhomDam[m_nNhomIndex] : WA_MAU_NHOM_CHU);"""


def main():
    print("== goi_va_wauto_mau_nhom_0409 %s ==" % ("(THU)" if THU else ""))
    p = os.path.join(WA_E, "WAuto.cpp")
    s = io.open(p, encoding="utf-16", newline="").read()
    goc = s
    for cu, moi, dau in ((BANG_CU, BANG_MOI, "s_crNhomDam[WA_SO_NHOM]"),
                         (VE_CU, VE_MOI, "crNen = bChon ? s_crNhomDam[nNhomVe]"),
                         (VIEN_CU, VIEN_MOI, "CreatePen(PS_SOLID, bChon ? 2 : 1"),
                         (VACH_CU, VACH_MOI, "if (bChon && !bNhom)"),
                         (KHUNG_CU, KHUNG_MOI, "? s_crNhomDam[m_nNhomIndex]")):
        cu = cu.replace("\n", "\r\n")
        moi = moi.replace("\n", "\r\n")
        if dau in s:
            print("  da co: %s" % dau[:50])
            continue
        if s.count(cu) != 1:
            LOI.append("xuat hien %d lan: %r" % (s.count(cu), cu[:80]))
            continue
        s = s.replace(cu, moi, 1)
        print("  thay: %s" % dau[:50])
    if LOI:
        print("LOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    if s == goc:
        print("khong doi")
        return
    if not THU:
        io.open(p, "w", encoding="utf-16", newline="").write(s)
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.cpp"))
        print("da ghi WAuto.cpp (+ mirror)")
    else:
        print("(--thu: khong ghi)")


if __name__ == "__main__":
    main()
