# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 19: NUT NHO THI NOI RONG VUNG CHAM (khong doi cach ve).
#
# Chu: "vi ban mobile nen man hinh nho can phai lam cac dong thong tin rong ra o ban mobile cho de
# cham vao". Ngoai danh sach tuy chon (da lam o dot 18), con ca ru nut nho trong giao dien: nut dong
# cua so, mui ten cuon, o ky nang, nut tab... Nhieu cai chi 12-16 diem anh, ngon tay bam rat truot.
#
# Cach lam AN TOAN NHAT: KHONG dong vao bo cuc, KHONG doi anh - chi noi rong VUNG BAT CHAM cua rieng
# lop nut (KWndButton::PtInWindow). Nut van ve y nguyen, chi la bam hut hon mot chut.
#
# Nut to san (>= NUT_CHAM_TOI_THIEU) thi khong dong vao. Nut nho thi noi deu ra bon phia cho du co,
# nhung khong bao gio noi qua NUT_CHAM_NOI_TOI_DA moi ben de hai nut ke nhau khong cuop cham cua nhau.
#
# Ban Windows khong doi hanh vi: toan bo trong #ifdef JX_ANDROID.
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


H = "Sources/S3Client/Ui/Elem/WndButton.h"
C = "Sources/S3Client/Ui/Elem/WndButton.cpp"

print("1. WndButton.h: khai bao do trung rieng")
va(H, "PtInWindow",
"""class KWndButton : public KWndImage""",
"""class KWndButton : public KWndImage""", "(khong doi dong lop)") if False else None

s = doc(H)
crlf = ("\r\n" in s)
if "ANDROID 09/09 CHAM] noi rong vung cham" in s:
    print("  bo qua (da co): khai bao PtInWindow")
else:
    neo = nl("class KWndButton : public KWndImage\r\n{\r\n" if crlf else "class KWndButton : public KWndImage\n{\n", False)
    neo = "class KWndButton : public KWndImage" + ("\r\n{\r\n" if crlf else "\n{\n")
    if s.count(neo) != 1:
        print("  !! khong tim thay dau lop KWndButton (%d cho)" % s.count(neo))
        sys.exit(1)
    them = neo + nl("""#ifdef JX_ANDROID
public:
	// [ANDROID 09/09 CHAM] noi rong vung cham cho nut nho - xem WndButton.cpp
	virtual int	PtInWindow(int x, int y);
#endif
""", crlf)
    ghi(H, s.replace(neo, them))
    print("  va xong: khai bao PtInWindow")

print("2. WndButton.cpp: than ham")
THAN = r'''
#ifdef JX_ANDROID
//--------------------------------------------------------------------------
// [ANDROID 09/09 CHAM] Nut nho thi noi rong VUNG BAT CHAM (khong doi cach ve).
//
// Man hinh dien thoai nho ma nhieu nut trong giao dien nay chi 12-16 diem anh (nut dong cua so, mui
// ten cuon, o ky nang, nut tab...), ngon tay bam rat truot. Chi noi vung bat cham cua rieng lop nut,
// KHONG dong vao bo cuc va KHONG doi anh - nen nhin y nguyen, chi la bam hut hon.
//
// Nut da to san thi khong dong vao. Nut nho thi noi deu bon phia cho du NUT_CHAM_TOI_THIEU, nhung
// khong bao gio noi qua NUT_CHAM_NOI_TOI_DA moi ben, de hai nut ke nhau khong cuop cham cua nhau.
//--------------------------------------------------------------------------
#define	NUT_CHAM_TOI_THIEU		26		// canh nho nhat mong muon cua vung cham (diem anh khung ve)
#define	NUT_CHAM_NOI_TOI_DA		6		// noi nhieu nhat bao nhieu moi ben

int KWndButton::PtInWindow(int x, int y)
{
	if (KWndImage::PtInWindow(x, y))
		return 1;
	if ((m_Style & WND_S_VISIBLE) == 0 || IsDisable())
		return 0;
	if (m_Width <= 0 || m_Height <= 0)
		return 0;
	int nNoiX = (NUT_CHAM_TOI_THIEU - m_Width) / 2;
	int nNoiY = (NUT_CHAM_TOI_THIEU - m_Height) / 2;
	if (nNoiX < 0) nNoiX = 0;
	if (nNoiY < 0) nNoiY = 0;
	if (nNoiX > NUT_CHAM_NOI_TOI_DA) nNoiX = NUT_CHAM_NOI_TOI_DA;
	if (nNoiY > NUT_CHAM_NOI_TOI_DA) nNoiY = NUT_CHAM_NOI_TOI_DA;
	if (nNoiX == 0 && nNoiY == 0)
		return 0;
	return (x >= m_nAbsoluteLeft - nNoiX && y >= m_nAbsoluteTop - nNoiY &&
			x <  m_nAbsoluteLeft + m_Width + nNoiX &&
			y <  m_nAbsoluteTop + m_Height + nNoiY) ? 1 : 0;
}
#endif

'''
s = doc(C)
crlf = ("\r\n" in s)
if "KWndButton::PtInWindow" in s:
    print("  bo qua (da co): than PtInWindow")
else:
    neo = nl("int KWndButton::WndProc(", crlf)
    if s.count(neo) < 1:
        print("  !! khong tim thay KWndButton::WndProc de dat truoc")
        sys.exit(1)
    i = s.find(neo)
    ghi(C, s[:i] + nl(THAN, crlf) + s[i:])
    print("  va xong: than PtInWindow")

print("")
print("XONG dot va 19.")
