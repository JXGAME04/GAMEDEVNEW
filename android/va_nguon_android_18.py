# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 18: DONG MENU RONG VA CAO HON CHO DE CHAM.
#
# Chu: "vi ban mobile nen man hinh nho can phai lam cac dong thong tin rong ra o ban mobile cho de
# cham vao". Cac danh sach tuy chon (KPopupMenu) duoc tinh theo CO CHU: moi dong cao
#     nItemHeight = byFontSize * so_dong + 2 * byItemTitleUpSpace
# va rong = byFontSize * so_ky_tu / 2 + 2 * nItemTitleIndent + ...
#
# Nen KHONG nhan thang nItemHeight/nItemWidth (chu se dinh len tren, lech chinh giua), ma NOI RONG
# HAI CAI DEM: byItemTitleUpSpace (dem tren/duoi) va nItemTitleIndent (dem trai/phai). Nhu vay dong
# to ra deu ca bon phia va chu van nam dung cho.
#
# Muc tieu: vung cham cao khoang 40-44 diem anh khung ve (huong dan cua Android la >= 48dp).
#
# Ban Windows khong doi hanh vi: khoi nay trong #ifdef JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)

def nl(s, crlf):
    return s.replace("\n", "\r\n") if crlf else s

def va(duong, dau_da_co, cu, moi, ten):
    s = doc(duong)
    crlf = ("\r\n" in s)
    if nl(dau_da_co, crlf) in s:
        print("  bo qua (da co): %s" % ten)
        return
    cu2, moi2 = nl(cu, crlf), nl(moi, crlf)
    if s.count(cu2) != 1:
        print("  !! KHONG VA DUOC %s: tim thay %d cho (can dung 1)" % (ten, s.count(cu2)))
        sys.exit(1)
    ghi(duong, s.replace(cu2, moi2))
    print("  va xong: %s" % ten)


P = "Sources/S3Client/Ui/Elem/PopupMenu.cpp"

print("1. Noi rong dem cua moi dong menu")
va(P, "CHAM] man hinh dien thoai nho",
"""	if (m_pMenu->nItemTitleIndent == MENU_ITEM_DEFAULT_INDENT)
		m_pMenu->nItemTitleIndent = m_nIndent;""",
"""	if (m_pMenu->nItemTitleIndent == MENU_ITEM_DEFAULT_INDENT)
		m_pMenu->nItemTitleIndent = m_nIndent;

#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] man hinh dien thoai nho, ngon tay to: noi rong dem tren/duoi va trai/phai
	// cua moi dong cho de cham. KHONG nhan thang nItemHeight/nItemWidth vi chu se dinh len canh tren
	// va lech sang trai - hai cai dem nay chinh la thu de dua chu vao giua.
	//   cao mot dong = byFontSize * so_dong + 2 * byItemTitleUpSpace
	//   rong mot dong = byFontSize * so_ky_tu / 2 + 2 * nItemTitleIndent + ...
	// Voi chu co 12 thi dem 7 cho ra dong cao khoang 26 diem anh khung ve (dem 14 chu bao QUA TO).
	if (m_pMenu->byItemTitleUpSpace < 7)
		m_pMenu->byItemTitleUpSpace = 7;
	if (m_pMenu->nItemTitleIndent < 9)
		m_pMenu->nItemTitleIndent = 9;
#endif""", "noi rong dem dong menu")

print("")
print("XONG dot va 18.")
