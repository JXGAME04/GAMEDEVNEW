# -*- coding: utf-8 -*-
#
# [WAUTO 13/09] Lam lai O DANH SACH cua bang phu theo yeu cau cua chu:
#   "co hien thi nut cuon thay vi nut chu - vuot len xuong de cuon - tham my hon co chieu sau hon"
#
#  1. VUOT DE CUON: KSdlApp da doi vuot doc tren giao dien thanh WM_MOUSEWHEEL tung nac 28 px
#     ([ANDROID 09/09 CUON]) - cung duong ma khung chat / thoai NPC / danh sach may chu dang dung.
#     KWndDsachWA nay nhan WM_MOUSEWHEEL, moi nac mot dong (28 px ngon tay ~ 22 px danh sach, gan 1:1).
#  2. THANH CUON THAT o mep phai o (ranh lom + con truot noi, cao theo ti le phan dang thay), thay cho
#     hai nut chu "Cuon len" / "Cuon xuong" - hai nut do bo han, o danh sach rong ra 560 -> 660 px.
#  3. VE CO CHIEU SAU: bong do chech phai-duoi, long o lom (canh tren-trai toi, duoi-phai sang), vien hai lop,
#     dong le sang rat nhe de dem dong, dong dang chon la dai vang ba lop + vien sang + vach danh dau ben trai,
#     mep tren/duoi co dai mo dan khi con noi dung chua thay.
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 13/09]"
KHOI = r'''// ---------------------------------------------------------------- o danh sach
// [WAUTO 13/09] Ve co CHIEU SAU (chu: "tham my hon, co chieu sau hon"):
//   * bong do chech phai-duoi (hai lop mo dan) -> o noi len khoi nen khung
//   * long o LOM: nen toi, canh TREN-TRAI toi hon (bong hat vao), canh DUOI-PHAI sang hon (anh sang tu tren)
//   * vien ngoai hai lop: vien toi om ngoai, vien vang am ben trong
//   * dong chan xen ke mot lop sang rat nhe -> de dem dong, do moi mat
//   * dong dang chon: dai mau vang am ba lop (gia chuyen sac), vien sang, va VACH DANH DAU ben trai
//   * con noi dung o tren / o duoi: ve dai mo dan o mep de biet con phai cuon
//   * THANH CUON that o mep phai (ranh lom + con truot noi), chi hien khi noi dung dai hon o
static void WAD_Vach(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	WAD_Bong(x0, y0, x1, y1, r, g, b, a);
}

KWndDsachWA::KWndDsachWA()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
	m_szDong[0][0] = 0;
}

int KWndDsachWA::SoDongHien() const
{
	int n = (m_Height - 2 * WA_DS_LE) / WA_DS_DONG;
	return n > 0 ? n : 1;
}

const char* KWndDsachWA::Dong(int i) const
{
	if (i < 0 || i >= m_nSo)
		return "";
	return m_szDong[i];
}

void KWndDsachWA::XoaHet()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
}

int KWndDsachWA::Them(const char* sz)
{
	if (m_nSo >= WA_DS_TOI_DA)
		return -1;
	strncpy(m_szDong[m_nSo], sz ? sz : "", WA_DS_CHU - 1);
	m_szDong[m_nSo][WA_DS_CHU - 1] = 0;
	return m_nSo++;
}

void KWndDsachWA::DatChon(int n)
{
	m_nChon = (n >= 0 && n < m_nSo) ? n : -1;
	if (m_nChon >= 0)
	{
		int nHien = SoDongHien();
		if (m_nChon < m_nDau)
			m_nDau = m_nChon;
		else if (m_nChon >= m_nDau + nHien)
			m_nDau = m_nChon - nHien + 1;
	}
}

// nBuoc > 0 = xuong. Dung cho ca lan chuot (vuot ngon: KSdlApp doi thanh WM_MOUSEWHEEL) lan trang.
void KWndDsachWA::Cuon(int nDong)
{
	int nHien = SoDongHien();
	m_nDau += nDong;
	if (m_nDau > m_nSo - nHien)
		m_nDau = m_nSo - nHien;
	if (m_nDau < 0)
		m_nDau = 0;
}

// hinh chu nhat dac co vien LOM (bong hat vao tu tren-trai) hoac NOI (anh sang tu tren-trai)
static void WAD_Khoi(int x0, int y0, int x1, int y1, int bNoi)
{
	if (bNoi)
	{
		WAD_Vach(x0, y0, x1, y0 + 1, 225, 205, 150, 10);			// mep tren sang
		WAD_Vach(x0, y0, x0 + 1, y1, 205, 185, 135, 12);			// mep trai sang
		WAD_Vach(x0, y1 - 1, x1, y1, 40, 28, 12, 6);				// mep duoi toi
		WAD_Vach(x1 - 1, y0, x1, y1, 55, 40, 18, 6);				// mep phai toi
	}
	else
	{
		WAD_Vach(x0, y0, x1, y0 + 2, 0, 0, 0, 8);					// bong hat vao tu tren
		WAD_Vach(x0, y0, x0 + 1, y1, 0, 0, 0, 12);					// va tu trai
		WAD_Vach(x0, y1 - 1, x1, y1, 130, 112, 76, 22);				// day hoi sang
		WAD_Vach(x1 - 1, y0, x1, y1, 120, 104, 72, 24);				// phai hoi sang
	}
}

void KWndDsachWA::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	int nHien = SoDongHien();
	int bCuon = (m_nSo > nHien);
	int xNoiDung = x1 - WA_DS_LE - (bCuon ? WA_DS_THANH : 0);	// mep phai cua phan chu

	// 1. bong do chech phai - duoi
	WAD_Bong(x0 + 4, y0 + 4, x1 + 4, y1 + 4, 0, 0, 0, 20);
	WAD_Bong(x0 + 2, y0 + 2, x1 + 2, y1 + 2, 0, 0, 0, 14);
	// 2. long o
	WAD_Bong(x0, y0, x1, y1, 14, 13, 11, 2);
	WAD_Khoi(x0, y0, x1, y1, 0);
	// 3. hai lop vien
	WAD_Vien(x0 - 1, y0 - 1, x1, y1, 18, 14, 8);
	WAD_Vien(x0, y0, x1 - 1, y1 - 1, 150, 122, 72);

	// 4. cac dong
	for (int i = 0; i < nHien; i++)
	{
		int k = m_nDau + i;
		if (k >= m_nSo)
			break;
		int yd = y0 + WA_DS_LE + i * WA_DS_DONG;
		int yh = yd + WA_DS_DONG;
		if (k == m_nChon)
		{	// dai vang am ba lop: dam o giua, nhat dan ra hai mep -> nhin nhu co do cong
			WAD_Bong(x0 + 2, yd, xNoiDung, yh - 1, 108, 78, 24, 10);
			WAD_Bong(x0 + 2, yd + 2, xNoiDung, yh - 3, 146, 106, 30, 8);
			WAD_Bong(x0 + 2, yd + WA_DS_DONG / 2 - 2, xNoiDung, yd + WA_DS_DONG / 2 + 2, 178, 132, 40, 6);
			WAD_Vien(x0 + 2, yd, xNoiDung - 1, yh - 2, 214, 176, 96);
			WAD_Vach(x0 + 2, yd, x0 + 5, yh - 1, 255, 214, 120, 2);		// vach danh dau ben trai
		}
		else if (k & 1)
			WAD_Bong(x0 + 2, yd, xNoiDung, yh - 1, 120, 124, 132, 28);	// dong le: sang rat nhe
		if (i && k != m_nChon && (k - 1) != m_nChon)
			WAD_Vach(x0 + 6, yd - 1, xNoiDung - 4, yd, 0, 0, 0, 20);	// vach chan dong

		int n = (int)strlen(m_szDong[k]);
		int nMax = (xNoiDung - (x0 + WA_DS_CHU_L) - 4) / 6;
		if (nMax < 1)
			nMax = 1;
		if (n > nMax)
			n = nMax;
		g_pRepresentShell->OutputText(12, m_szDong[k], n, x0 + WA_DS_CHU_L, yd + (WA_DS_DONG - 12) / 2,
			(k == m_nChon) ? 0x00FFF2C8 : 0x00DCD8D0, 0, TEXT_IN_SINGLE_PLANE_COORD, 0x00000000);
	}

	if (!bCuon)
		return;

	// 5. dai mo dan o mep tren / duoi: bao con noi dung phia do
	if (m_nDau > 0)
		for (int q = 0; q < 5; q++)
			WAD_Vach(x0 + 2, y0 + 1 + q, xNoiDung, y0 + 2 + q, 0, 0, 0, 10 + q * 4);
	if (m_nDau + nHien < m_nSo)
		for (int q = 0; q < 5; q++)
			WAD_Vach(x0 + 2, y1 - 2 - q, xNoiDung, y1 - 1 - q, 0, 0, 0, 10 + q * 4);

	// 6. thanh cuon: ranh LOM + con truot NOI, cao theo ti le phan dang thay
	{
		int xt0 = x1 - WA_DS_LE - WA_DS_THANH + 2, xt1 = x1 - WA_DS_LE;
		int yt0 = y0 + WA_DS_LE, yt1 = y1 - WA_DS_LE;
		int nCaoRanh = yt1 - yt0;
		int nCao = nCaoRanh * nHien / m_nSo;
		int nToiDa = m_nSo - nHien;
		int yc0, yc1;
		if (nCao < WA_DS_THANH * 2)
			nCao = WA_DS_THANH * 2;
		if (nCao > nCaoRanh)
			nCao = nCaoRanh;
		yc0 = yt0 + (nToiDa > 0 ? (nCaoRanh - nCao) * m_nDau / nToiDa : 0);
		yc1 = yc0 + nCao;
		WAD_Bong(xt0, yt0, xt1, yt1, 8, 7, 6, 6);			// ranh
		WAD_Khoi(xt0, yt0, xt1, yt1, 0);
		WAD_Bong(xt0 + 1, yc0, xt1 - 1, yc1, 150, 118, 62, 6);	// con truot
		WAD_Bong(xt0 + 1, yc0 + 1, xt1 - 1, yc0 + nCao / 2, 186, 150, 84, 8);
		WAD_Khoi(xt0 + 1, yc0, xt1 - 1, yc1, 1);
	}
}

int KWndDsachWA::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{	// [WAUTO 13/09] VUOT DOC de cuon: KSdlApp doi vuot doc tren giao dien thanh WM_MOUSEWHEEL
		// (moi 28 px mot nac, [ANDROID 09/09 CUON]) - dung duong ma khung chat / thoai NPC dang dung.
		int zDelta = (int)(short)HIWORD(uParam);
		if (zDelta)
			Cuon(-zDelta / WHEEL_DELTA);
		return 1;
	}
	if (uMsg == WM_LBUTTONDOWN)
	{
		int y = (int)(short)HIWORD(nParam);
		int i = (y - m_nAbsoluteTop - WA_DS_LE) / WA_DS_DONG;
		int k = m_nDau + i;
		if (i >= 0 && i < SoDongHien() && k < m_nSo)
		{
			m_nChon = k;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (KUPARAM)(KWndWindow*)this, k);
		}
		return 1;
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
'''


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def V(s):
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


def thay(p, cu, moi, ten):
    s = doc(p)
    cu, moi = V(cu), V(moi)
    if s.count(cu) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(cu), ten))
    ghi(p, s.replace(cu, moi))
    print("   da va:", ten)


# ---------------------------------------------------------------- 1. header
def va_h():
    P = "Sources/S3Client/Ui/UiCase/UiWAutoDsach.h"
    if "WA_DS_THANH" in doc(P):
        print("   bo qua (da va):", P)
        return
    thay(P,
         "#define WA_DS_DONG\t\t22\t\t// cao mot dong danh sach (px) - du cho ngon tay",
         "#define WA_DS_DONG\t\t22\t\t// cao mot dong danh sach (px) - du cho ngon tay\r\n"
         "#define WA_DS_LE\t\t4\t\t// %s le trong long o (de vien lom khong an vao chu)\r\n" % DAU +
         "#define WA_DS_THANH\t\t10\t\t// be ngang thanh cuon o mep phai (chi ve khi noi dung dai hon o)\r\n"
         "#define WA_DS_CHU_L\t\t10\t\t// chu bat dau cach mep trai bao nhieu px",
         "UiWAutoDsach.h: kich thuoc moi cua o danh sach")
    thay(P,
         "\tKWndLabeledButton\tm_Cuon[2];\r\n",
         "",
         "UiWAutoDsach.h: bo hai nut cuon")


# ---------------------------------------------------------------- 2. thay ca khoi KWndDsachWA
def va_cpp_khoi():
    P = "Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp"
    s = doc(P)
    if "WA_DS_THANH" in s:
        print("   bo qua (da va): khoi KWndDsachWA")
        return
    d1 = "// ---------------------------------------------------------------- o danh sach"
    d2 = "// ---------------------------------------------------------------- cau hinh tung danh sach"
    a, b = s.index(d1), s.index(d2)
    moi = KHOI.replace(chr(10), chr(13) + chr(10))
    ghi(P, s[:a] + moi + s[b:])
    print("   da va: UiWAutoDsach.cpp: khoi KWndDsachWA moi")


# ---------------------------------------------------------------- 3. bo hai nut cuon khoi bang phu
def va_cpp_bangphu():
    P = "Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp"
    if "m_Cuon" not in doc(P):
        print("   bo qua (da va): bo nut cuon")
        return
    thay(P,
         "\tAddChild(&m_Dsach);\r\n"
         "\tfor (i = 0; i < 2; i++)\r\n"
         "\t\tAddChild(&m_Cuon[i]);\r\n",
         "\tAddChild(&m_Dsach);\r\n",
         "UiWAutoDsach.cpp: bo AddChild nut cuon")
    thay(P,
         "\tm_Dsach.Init(&Ini, \"Dsach\");\r\n"
         "\tfor (i = 0; i < 2; i++)\r\n"
         "\t{\r\n"
         "\t\tsprintf(szMuc, \"Cuon%d\", i);\r\n"
         "\t\tm_Cuon[i].Init(&Ini, szMuc);\r\n"
         "\t}\r\n",
         "\tm_Dsach.Init(&Ini, \"Dsach\");\r\n",
         "UiWAutoDsach.cpp: bo Init nut cuon")
    thay(P,
         "\t\tfor (i = 0; i < 2; i++)\r\n"
         "\t\t\tif (uParam == (KUPARAM)(KWndWindow*)&m_Cuon[i])\r\n"
         "\t\t\t{\r\n"
         "\t\t\t\tm_Dsach.Cuon(i ? 1 : -1);\r\n"
         "\t\t\t\treturn 1;\r\n"
         "\t\t\t}\r\n",
         "",
         "UiWAutoDsach.cpp: bo xu ly nut cuon")
    # chu huong dan: khong con nut cuon nua
    thay(P,
         "\t\tm_Huong.SetText(\"@@Cuộn bằng hai nút bên phải. Bấm Đóng để quay lại.@@\");",
         "\t\tm_Huong.SetText(\"@@Vuốt lên xuống trong ô để cuộn. Bấm Đóng để quay lại.@@\");",
         "UiWAutoDsach.cpp: doi chu huong dan")


# ---------------------------------------------------------------- 4. bo cuc
def va_ini():
    P = "android/du_lieu_ghi_de/ui/ui3/uiwauto_dsach.ini"
    s = doc(P)
    if "[Cuon0]" not in s:
        print("   bo qua (da va):", P)
        return
    # o danh sach rong ra het be ngang bang
    s = re.sub(r"(?m)^(\[Dsach\]\r?\nLeft=12\r?\nTop=32\r?\nWidth=)560", r"\g<1>660", s)
    # bo hai muc nut cuon
    out = []
    for blk in re.split(r"(?m)(?=^\[)", s):
        m = re.match(r"^\[(\w+)\]", blk)
        if m and m.group(1) in ("Cuon0", "Cuon1"):
            continue
        out.append(blk)
    s = "".join(out)
    ghi(P, s)
    print("   da va:", P)


def main():
    va_h()
    va_cpp_khoi()
    va_cpp_bangphu()
    va_ini()
    print("xong wauto30")


if __name__ == "__main__":
    main()
