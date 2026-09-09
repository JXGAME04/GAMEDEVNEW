# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 16: VUOT DOC TREN GIAO DIEN = CUON DANH SACH.
#
# Chu yeu cau "kich vao npc phai hien cac dong chat voi npc de de kich vao - vuot len xuong duoc".
# Ban PC cuon danh sach bang LAN CHUOT. Cac lop danh sach cua bo giao dien nay deu nhan WM_MOUSEWHEEL:
# WndList.cpp, WndList2.cpp, WndMessageListBox.cpp (da kiem). Nen chi can dich cu chi VUOT DOC thanh
# lan chuot la moi danh sach trong game deu vuot duoc: thoai NPC, chat, danh sach may chu, nhiem vu...
#
# Quy tac:
#   vuot DOC   tren giao dien  -> lan chuot (cuon)
#   vuot NGANG tren giao dien  -> giu chuot trai roi re nhu cu (keo cua so di cho khac)
#   vuot tren ban do           -> khong doi (can dieu khien / di lien tuc)
#
# Khong so mat duong chuyen vat pham: ban PC chuyen do bang BAM de nhac len roi BAM cho de dat xuong,
# khong bat buoc phai keo - cham hai lan van lam duoc het.
#
# Ban Windows khong doi hanh vi: moi khoi deu trong #ifdef JX_ANDROID.
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


KSDL_C = "Sources/S3Client/Platform/KSdlApp.cpp"
KSDL_H = "Sources/S3Client/Platform/KSdlApp.h"

print("1. KSdlApp.h: trang thai cuon")
va(KSDL_H, "CHAM_CUON",
"""	enum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN };""",
"""	enum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN, CHAM_CUON };""", "KSdlApp.h them CHAM_CUON")

va(KSDL_H, "m_nCuonDon",
"""	int				m_nNgonToiDa;		// [ANDROID 09/09 HAINGON] nhieu nhat may ngon trong lan cham nay""",
"""	int				m_nNgonToiDa;		// [ANDROID 09/09 HAINGON] nhieu nhat may ngon trong lan cham nay
	int				m_nCuonDon;			// [ANDROID 09/09 CUON] so diem anh da vuot, con du chua thanh mot nac lan""", "KSdlApp.h them m_nCuonDon")

print("2. Khoi tao")
va(KSDL_C, "m_nCuonDon = 0;",
"""	m_nNgonDangDat = 0;
	m_nNgonToiDa = 0;""",
"""	m_nNgonDangDat = 0;
	m_nNgonToiDa = 0;
	m_nCuonDon = 0;""", "ham dung khoi tao m_nCuonDon")

print("3. Vuot doc tren giao dien -> cuon")
va(KSDL_C, "CHAM_CUON;",
"""			// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;
			// keo o cho khac = giu chuot trai roi re nhu ban PC (di lien tuc, keo tha vat pham).
			if (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
			{
				m_nCham = CHAM_CAN;
				JxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);
				return true;
			}""",
"""			// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;
			// keo o cho khac = giu chuot trai roi re nhu ban PC (di lien tuc, keo tha vat pham).
			if (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
			{
				m_nCham = CHAM_CAN;
				JxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);
				return true;
			}
			// [ANDROID 09/09 CUON] Vuot DOC tren giao dien = cuon danh sach (dich thanh lan chuot).
			// Moi lop danh sach cua bo giao dien nay deu nhan WM_MOUSEWHEEL (WndList, WndList2,
			// WndMessageListBox) nen thoai NPC / chat / danh sach may chu deu vuot duoc.
			// Vuot NGANG thi van la giu chuot trai roi re (keo cua so di cho khac).
			if (JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0) &&
				abs(m_nChamY - m_nChamY0) > abs(m_nChamX - m_nChamX0))
			{
				m_nCham = CHAM_CUON;
				m_nCuonDon = 0;
				return true;
			}""", "vuot doc = cuon")

print("4. Trong luc cuon: moi mot doan thi ban mot nac lan chuot")
va(KSDL_C, "CHAM_CUON_BUOC",
"""		if (m_nCham == CHAM_CAN)
		{
			JxCan_Keo(m_nChamX, m_nChamY);
			return true;
		}""",
"""		if (m_nCham == CHAM_CAN)
		{
			JxCan_Keo(m_nChamX, m_nChamY);
			return true;
		}
		if (m_nCham == CHAM_CUON)
		{
			// Ngon di XUONG = doc nguoc len = lan chuot VE PHIA TRUOC (delta duong), giong moi may.
			const int CHAM_CUON_BUOC = 28;		// bao nhieu diem anh vuot thi thanh mot nac lan
			m_nCuonDon += (m_nChamY - nYTruoc);
			while (m_nCuonDon >= CHAM_CUON_BUOC || m_nCuonDon <= -CHAM_CUON_BUOC)
			{
				int nDau = (m_nCuonDon > 0) ? 1 : -1;
				m_nCuonDon -= nDau * CHAM_CUON_BUOC;
				WPARAM w = MAKEWPARAM(0, (WORD)(short)(nDau * WHEEL_DELTA));
				MsgProc(hWnd, WM_MOUSEWHEEL, w, MAKELPARAM(m_nChamX0, m_nChamY0));
			}
			return true;
		}""", "cuon thanh nac lan chuot")

print("5. Nho vi tri y truoc do de tinh doan vuot")
va(KSDL_C, "int nYTruoc = m_nChamY;",
"""		float fx = ev.motion.x, fy = ev.motion.y; SdlToLogical(m_pWindow, fx, fy);
		m_nChamX = (int)fx; m_nChamY = (int)fy;""",
"""		float fx = ev.motion.x, fy = ev.motion.y; SdlToLogical(m_pWindow, fx, fy);
		int nYTruoc = m_nChamY;		// [ANDROID 09/09 CUON] de tinh doan vua vuot duoc
		m_nChamX = (int)fx; m_nChamY = (int)fy;""", "nho y truoc do")

print("")
print("XONG dot va 16.")
