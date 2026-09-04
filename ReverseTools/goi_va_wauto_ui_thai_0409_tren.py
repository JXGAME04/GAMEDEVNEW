# -*- coding: utf-8 -*-
"""
goi_va_wauto_ui_thai_0409_tren.py - DOT 4 + DOT 5 cua viec dung lai giao dien WAuto
theo auto Thai (vlhkmp): VUNG TREN 3 hang + DICH KHOI NOI DUNG XUONG 34 don vi,
va dua 5 o "Ac chinh" len vung tren (giu nguyen ID).

  Resource.h : 720..728 (9 ID moi, NGOAI dai [161,700) nen ShowTab khong bao gio an)
  WAuto.rc   : hop thoai 160x360 -> 160x430; them 9 control vung tren;
               rut gon chu 2 o Ac chinh; don lai tab 15
  WAuto.cpp  : WA_DICH_Y / WA_LuonHien / WA_DichKhoiNoiDung; goi truoc ShowTab;
               +34 cho 16 lenh MoveDlgItem chep cung; vong an bo qua 5 ID Ac chinh;
               WM_DRAWITEM ve 3 nut chrome; WM_CTLCOLORSTATIC nen chu chay;
               TIMER_APPLOOP xoay chu chay; WM_COMMAND cho 7 control moi;
               s_aTabDay[15] 244 -> 208; them 638 vao bang chu do.

Chay: python goi_va_wauto_ui_thai_0409_tren.py [--thu]
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="utf-16"):
        self.path = path; self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)

    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi, 1); self.log.append("  thay: %s" % dau[:60])

    def them_sau_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0] + 1] + self.N(moi).split(self.nl) + L[a[0] + 1:])
        self.log.append("  them sau dong: %s" % dau[:60])

    def them_truoc_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0]] + self.N(moi).split(self.nl) + L[a[0]:])
        self.log.append("  them truoc dong: %s" % dau[:60])

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# =====================================================================
# 1. Resource.h
# =====================================================================
RES = """// (04/09) VUNG TREN kieu auto Thai - dai 720..728 NAM NGOAI [161,700) nen
// vong an/hien cua ShowTab khong dung toi: cac o nay LUON HIEN o moi tab.
#define IDC_STRING_TICKER\t720
#define IDC_BTN_TOP_A\t\t721
#define IDC_BTN_TOP_H\t\t722
#define IDC_BTN_TOP_HELP\t723
#define IDC_COMBO_TOP_CD\t724
#define IDC_BTN_TOP_AC\t\t725
#define IDC_BTN_TOP_ACX\t\t726
#define IDC_BTN_TOP_Q\t\t727
#define IDC_SEP_TOP\t\t\t728"""

# =====================================================================
# 2. WAuto.rc - 9 control vung tren
# =====================================================================
RC_TREN = """
\t// ===== (04/09) VUNG TREN kieu auto Thai - 3 hang, LUON HIEN o moi tab =====
\t// Hang 1: chu chay + [A] quet lai cua so + [H] huong dan chung + [Tro giup] note cua tab.
\t// Hang 2: combo che do nhanh + [Ac chinh] + combo ac chinh + [X].
\t// Hang 3: [v] Tim ac chinh + so mps + [v] Trong thanh + [?].
\t// Toa do dat tay o WA_DichKhoiNoiDung (5 o Ac chinh) nen y o day chi la cho dat.
\tLTEXT "WAuto — đang dò cửa sổ game...", IDC_STRING_TICKER, 2, 1, 88, 14, SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | SS_NOPREFIX
\tCONTROL "A", IDC_BTN_TOP_A, "Button", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 92, 1, 14, 14
\tCONTROL "H", IDC_BTN_TOP_H, "Button", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 108, 1, 14, 14
\tCONTROL "Trợ giúp", IDC_BTN_TOP_HELP, "Button", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 124, 1, 34, 14
\tCOMBOBOX IDC_COMBO_TOP_CD, 2, 17, 42, 100, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
\tPUSHBUTTON "Ác chính", IDC_BTN_TOP_AC, 46, 17, 34, 12
\tPUSHBUTTON "X", IDC_BTN_TOP_ACX, 146, 17, 12, 12
\tPUSHBUTTON "?", IDC_BTN_TOP_Q, 146, 31, 12, 10
\tCONTROL "", IDC_SEP_TOP, "Static", SS_ETCHEDHORZ, 2, 43, 156, 1"""

# =====================================================================
# 3. WAuto.cpp - khoi dich + vung tren
# =====================================================================
KHOI_DICH = """// ===================== (04/09) VUNG TREN KIEU AUTO THAI =====================
// Auto Thai co 3 hang chrome tren dinh (chu chay + nut, combo che do + ac chinh,
// [v] tim ac chinh + [v] trong thanh) roi moi den danh sach nhan vat va dai tab.
// WAuto dung lai dung trat tu do MA KHONG NAN LAI 467 toa do trong WAuto.rc:
// dich CA KHOI NOI DUNG xuong WA_DICH_Y don vi MOT LAN luc WM_INITDIALOG.
//   - SWP_NOSIZE: chi doi CHO, khong dung chieu cao -> khong pha chieu cao tha
//     xuong cua 63 COMBOBOX (bay da ghi san trong ShowTab).
//   - 12 nut dai tab + ListView + 5 o Ac chinh nam NGOAI dai [161,700) hoac phai
//     len vung tren -> dat TAY o cuoi ham.
#define WA_DICH_Y	34

// 5 o "Ac chinh" (634..638) DOI CHO len vung tren nhung GIU NGUYEN ID, nen vong an
// cua ShowTab va vong dich phai bo qua chung.
static BOOL WA_LuonHien(int nId)
{
	switch (nId)
	{
	case IDC_COMBO_16_AC:
	case IDC_CHECKBOX_16_TIM:
	case IDC_EDITOR_16_KC:
	case IDC_STRING_16_KC:
	case IDC_CHECKBOX_16_THANH:
		return TRUE;
	}
	return FALSE;
}

static void WA_DichKhoiNoiDung(HWND hDlg)	// GOI DUNG MOT LAN trong WM_INITDIALOG
{
	RECT rc = { 0, 0, 0, WA_DICH_Y };
	MapDialogRect(hDlg, &rc);
	const int dy = rc.bottom;
	for (int i = IDC_STRING_0_L; i < IDC_INDEX_END; ++i)
	{
		if (WA_LuonHien(i))
			continue;
		HWND h = ::GetDlgItem(hDlg, i);
		if (!h)
			continue;
		RECT r;
		GetWindowRect(h, &r);
		MapWindowPoints(NULL, hDlg, (POINT*)&r, 2);
		SetWindowPos(h, NULL, r.left, r.top + dy, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	// ---- dat tay: ListView + 12 nut dai tab (ID ngoai dai tren) ----
	MoveDlgItem(hDlg, IDC_PLAYERLIST,   2,  45, 156, 58);
	MoveDlgItem(hDlg, IDC_TABBTN_1,     2, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_2,    41, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_3,    80, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_4,   119, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_5,     6, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_6,    44, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_7,    82, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_8,   120, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_9,     6, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_10,   44, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_11,   82, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_12,  120, 133,  36, 10);
	// ---- dat tay: 5 o Ac chinh len hang 2 va hang 3 cua vung tren ----
	// combo phai truyen chieu cao THA XUONG (120) chu khong phai chieu cao o.
	MoveDlgItem(hDlg, IDC_COMBO_16_AC,       82, 17, 62, 120);
	MoveDlgItem(hDlg, IDC_CHECKBOX_16_TIM,    2, 31, 60, 10);
	MoveDlgItem(hDlg, IDC_EDITOR_16_KC,      63, 31, 18, 10);
	MoveDlgItem(hDlg, IDC_STRING_16_KC,      82, 31, 13, 10);
	MoveDlgItem(hDlg, IDC_CHECKBOX_16_THANH, 97, 31, 48, 10);
}

// Chu chay hang 1 - doi cau moi 6 giay (Thai de mot khung chu tinh o day).
static const wchar_t* const s_aChuChay[] = {
	L"WAuto — %d cửa sổ, %d đang bật auto.",
	L"Bấm [Trợ giúp] để xem hướng dẫn của tab đang mở.",
	L"Rê chuột vào từng ô để đọc ghi chú nhanh.",
	L"Ô tích đầu dòng danh sách = BẬT auto cho nhân vật đó.",
	L"Võ Lâm Ngạo Thế",
};
static UINT s_uChuChayT = 0;
static int  s_nChuChay = 0;

static void WA_XoayChuChay(HWND hDlg)
{
	UINT now = timeGetTime();
	if (s_uChuChayT && now - s_uChuChayT < 6000)
		return;
	s_uChuChayT = now ? now : 1;
	const int nSo = (int)(sizeof(s_aChuChay) / sizeof(s_aChuChay[0]));
	s_nChuChay = (s_nChuChay + 1) % nSo;
	wchar_t wb[160];
	if (s_nChuChay == 0)
	{
		int nBat = 0;
		HWND hL = ::GetDlgItem(hDlg, IDC_PLAYERLIST);
		for (int i = 0; i < (int)m_vGameNode.size(); ++i)
			if (ListView_GetCheckState(hL, i))
				++nBat;
		GVWPRINT(wb, s_aChuChay[0], (int)m_vGameNode.size(), nBat);
	}
	else
		lstrcpynW(wb, s_aChuChay[s_nChuChay], 160);
	SetDlgItemTextW(hDlg, IDC_STRING_TICKER, wb);
}
// ===================== HET VUNG TREN =====================

"""

CMD_TREN = """			case IDC_BTN_TOP_A:
			{	// (04/09) quet lai danh sach cua so game ngay (nut [A] cua Thai)
				EnumWindows(EnumWindowsProc, (LPARAM)hDlg);
				break;
			}
			case IDC_BTN_TOP_H:
			{	// (04/09) huong dan CHUNG
				WA_MoNote(hDlg, L"Chung", s_szNoteChung, 600);
				break;
			}
			case IDC_BTN_TOP_HELP:
			{	// (04/09) huong dan cua TAB DANG MO
				WA_MoNoteTab(hDlg);
				break;
			}
			case IDC_BTN_TOP_Q:
			{	// (04/09) huong dan cua VUNG TREN
				WA_MoNote(hDlg, L"Vùng trên", s_szNoteVungTren, 600);
				break;
			}
			case IDC_BTN_TOP_AC:
			{	// (04/09) dat nhan vat DANG CHON lam ac chinh cho moi cua so khac.
				// Dung lai nhanh IDM_ACCHINH_ALL: nhanh do tra theo s_uACMenuPID nen
				// phai gan truoc, khong thi nMain = -1 va ca nhanh thanh vo nghia.
				if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				{
					s_uACMenuPID = m_vGameNode[m_nCurSel].procPID;
					SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDM_ACCHINH_ALL, 0), 0);
				}
				break;
			}
			case IDC_BTN_TOP_ACX:
			{	// (04/09) xoa ac chinh cua nhan vat dang chon
				if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				{
					gameNode& gn = m_vGameNode[m_nCurSel];
					gn.apdata.szAcChinhTen[0] = 0;
					SaveRoleDataFast(gn);
					AC_NapCombo(hDlg, gn);
				}
				break;
			}
			case IDC_COMBO_TOP_CD:
			{	// (04/09) combo CHE DO NHANH (Thai: "WAR (PK -Boss)" / "TK" / "CTC"...):
				// chon mot muc = vua nhay sang tab do vua bat cong tac chinh cua no.
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					static const int s_anCDTab[] = { 2, 7, 9, 14, 10, 8 };
					static const int s_anCDBat[] = { IDC_CHECKBOX_2_F, IDC_CHECKBOX_7_F,
						IDC_CHECKBOX_9_ON, IDC_CHECKBOX_15_ON, IDC_CHECKBOX_10_ON, IDC_CHECKBOX_8_ON };
					int idx = (int)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
					if (idx >= 0 && idx < (int)(sizeof(s_anCDTab) / sizeof(s_anCDTab[0])))
					{
						ShowTab(hDlg, s_anCDTab[idx]);
						if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
						{
							CheckDlgButton(hDlg, s_anCDBat[idx], BST_CHECKED);
							SaveRoleData(hDlg, m_vGameNode[m_nCurSel]);
						}
					}
				}
				break;
			}
"""

DRAW_TREN = """		// --- (04/09) 3 nut chrome vung tren kieu auto Thai ---
		if (dis->CtlID == IDC_BTN_TOP_A || dis->CtlID == IDC_BTN_TOP_H || dis->CtlID == IDC_BTN_TOP_HELP)
		{
			HDC hdc = dis->hDC;
			RECT rc = dis->rcItem;
			const BOOL bHelp = (dis->CtlID == IDC_BTN_TOP_HELP);
			COLORREF crNen = bHelp ? RGB(143, 188, 143) : RGB(176, 196, 222);	// DarkSeaGreen / LightSteelBlue
			COLORREF crChu = (dis->CtlID == IDC_BTN_TOP_A) ? RGB(160, 0, 0) : RGB(20, 20, 20);
			FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
			{
				HBRUSH hNen = CreateSolidBrush(crNen);
				HPEN hVien = CreatePen(PS_SOLID, 1, RGB(120, 130, 140));
				HBRUSH hNenCu = (HBRUSH)SelectObject(hdc, hNen);
				HPEN hVienCu = (HPEN)SelectObject(hdc, hVien);
				RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 5, 5);
				SelectObject(hdc, hNenCu);
				SelectObject(hdc, hVienCu);
				DeleteObject(hNen);
				DeleteObject(hVien);
			}
			if (dis->itemState & ODS_SELECTED)
				OffsetRect(&rc, 1, 1);
			wchar_t wszN[64];
			wszN[0] = 0;
			GetWindowTextW(dis->hwndItem, wszN, 64);
			HFONT hFCu = (HFONT)SelectObject(hdc, WA_FontTabCon(hDlg));
			SetTextColor(hdc, crChu);
			SetBkMode(hdc, TRANSPARENT);
			DrawTextW(hdc, wszN, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
			SelectObject(hdc, hFCu);
			return TRUE;
		}
"""


def main():
    print("== goi_va_wauto_ui_thai_0409_tren %s ==" % ("(THU - khong ghi)" if THU else ""))

    # ---------- Resource.h ----------
    r = Tep(os.path.join(WA_E, "Resource.h"), "latin-1")
    r.them_sau_dong("#define IDC_HELP_TEXT", RES, "IDC_STRING_TICKER")
    r.ghi()

    # ---------- WAuto.rc ----------
    rc = Tep(os.path.join(WA_E, "WAuto.rc"))
    rc.thay("IDD_DLG_WAUTOMAIN DIALOGEX 0, 0, 160, 360",
            "IDD_DLG_WAUTOMAIN DIALOGEX 0, 0, 160, 430",
            "IDD_DLG_WAUTOMAIN DIALOGEX 0, 0, 160, 430")
    rc.them_sau_dong("        4,10,152,60", RC_TREN, "IDC_STRING_TICKER, 2, 1, 88, 14")
    # rut gon chu 2 o Ac chinh (cau day du chuyen vao tooltip + note)
    rc.thay('CONTROL "Tìm ac chính khi cách hơn", IDC_CHECKBOX_16_TIM',
            'CONTROL "Tìm ác chính", IDC_CHECKBOX_16_TIM',
            'CONTROL "Tìm ác chính", IDC_CHECKBOX_16_TIM')
    rc.thay('CONTROL "Tìm trong thành (ac chính đứng trong thành vẫn theo)", IDC_CHECKBOX_16_THANH',
            'CONTROL "Trong thành", IDC_CHECKBOX_16_THANH',
            'CONTROL "Trong thành", IDC_CHECKBOX_16_THANH')
    # tab 15 don lai: bo nhan roi, keo 3 o con lai len
    rc.thay('LTEXT "Ac chính:", IDC_STRING_16_AC, 4, 125, 36, 10, SS_CENTERIMAGE',
            'LTEXT "", IDC_STRING_16_AC, 4, 125, 36, 10, SS_CENTERIMAGE',
            'LTEXT "", IDC_STRING_16_AC')
    rc.thay('CONTROL "Đánh cùng mục tiêu ac chính", IDC_CHECKBOX_16_MT, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 162, 152, 10',
            'CONTROL "Đánh cùng mục tiêu ác chính", IDC_CHECKBOX_16_MT, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 126, 152, 10',
            'IDC_CHECKBOX_16_MT, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 126')
    rc.thay('CONTROL "", IDC_SEP_15A, "Static", SS_ETCHEDHORZ, 6, 176, 148, 1',
            'CONTROL "", IDC_SEP_15A, "Static", SS_ETCHEDHORZ, 6, 140, 148, 1',
            'IDC_SEP_15A, "Static", SS_ETCHEDHORZ, 6, 140')
    rc.thay('LTEXT "Chưa chọn ac chính.", IDC_STRING_16_TT, 4, 180, 152, 48',
            'LTEXT "Chưa chọn ác chính.", IDC_STRING_16_TT, 4, 144, 152, 48',
            'IDC_STRING_16_TT, 4, 144')
    rc.thay('GROUPBOX "Ac chính · ac phụ đi theo và đánh cùng mục tiêu", IDC_GRP_TAB15, 2, 112, 156, 122',
            'GROUPBOX "Ác chính · ac phụ đi theo và đánh cùng mục tiêu", IDC_GRP_TAB15, 2, 112, 156, 86',
            'IDC_GRP_TAB15, 2, 112, 156, 86')
    rc.ghi()

    # ---------- WAuto.cpp ----------
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    t.them_truoc_dong("static void ShowTab(HWND hDlg, int nTabBtn)", KHOI_DICH, "static void WA_DichKhoiNoiDung(")
    # vong an cua ShowTab bo qua 5 o Ac chinh
    t.thay("""	for(i=IDC_STRING_0_L;i<IDC_INDEX_END;++i)
	{
		ShowWindow(::GetDlgItem( hDlg, i ), SW_HIDE);
	}""",
           """	for(i=IDC_STRING_0_L;i<IDC_INDEX_END;++i)
	{
		if (WA_LuonHien(i))	// (04/09) 5 o Ac chinh da len vung tren - luon hien
			continue;
		ShowWindow(::GetDlgItem( hDlg, i ), SW_HIDE);
	}""", "if (WA_LuonHien(i))\t// (04/09) 5 o Ac chinh")
    # 16 lenh MoveDlgItem chep cung: +WA_DICH_Y
    for cu, moi in (
        ("MoveDlgItem(hDlg,IDC_STRING_2_SA1, 4,252,65,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SA1, 4,286,65,10);"),
        ("MoveDlgItem(hDlg,IDC_COMBO_2_SA1, 70, 252, 86, 22);", "MoveDlgItem(hDlg,IDC_COMBO_2_SA1, 70, 286, 86, 22);"),
        ("MoveDlgItem(hDlg,IDC_STRING_2_SA2, 4,265,65,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SA2, 4,299,65,10);"),
        ("MoveDlgItem(hDlg,IDC_COMBO_2_SA2, 70, 265, 86, 22);", "MoveDlgItem(hDlg,IDC_COMBO_2_SA2, 70, 299, 86, 22);"),
        ("MoveDlgItem(hDlg,IDC_STRING_2_SA1, 4,265,65,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SA1, 4,299,65,10);"),
        ("MoveDlgItem(hDlg,IDC_COMBO_2_SA1, 70, 265, 86, 22);", "MoveDlgItem(hDlg,IDC_COMBO_2_SA1, 70, 299, 86, 22);"),
        ("MoveDlgItem(hDlg,IDC_STRING_2_SA2, 4,278,65,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SA2, 4,312,65,10);"),
        ("MoveDlgItem(hDlg,IDC_COMBO_2_SA2, 70, 278, 86, 22);", "MoveDlgItem(hDlg,IDC_COMBO_2_SA2, 70, 312, 86, 22);"),
        ("MoveDlgItem(hDlg,IDC_STRING_2_SC, 4,291,45,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SC, 4,325,45,10);"),
        ("MoveDlgItem(hDlg,IDC_EDITOR_2_SC, 50, 291, 12, 10);", "MoveDlgItem(hDlg,IDC_EDITOR_2_SC, 50, 325, 12, 10);"),
        ("MoveDlgItem(hDlg,IDC_STRING_2_SC2, 64,291,4,10);", "MoveDlgItem(hDlg,IDC_STRING_2_SC2, 64,325,4,10);"),
        ("MoveDlgItem(hDlg,IDC_COMBO_2_SC, 70, 291, 86, 22);", "MoveDlgItem(hDlg,IDC_COMBO_2_SC, 70, 325, 86, 22);"),
        ("MoveDlgItem(hDlg, nGrpId[nTabBtn], 2, 112, 156, nDay - 111);", "MoveDlgItem(hDlg, nGrpId[nTabBtn], 2, 112 + WA_DICH_Y, 156, nDay - 111);"),
        ("MoveDlgItem(hDlg, IDC_STRING_HD_L, 4, nDay + 3, 14, 10);", "MoveDlgItem(hDlg, IDC_STRING_HD_L, 4, nDay + 3 + WA_DICH_Y, 14, 10);"),
        ("MoveDlgItem(hDlg, IDC_STRING_HD_V, 20, nDay + 3, 136, 10);", "MoveDlgItem(hDlg, IDC_STRING_HD_V, 20, nDay + 3 + WA_DICH_Y, 136, 10);"),
        ("MoveDlgItem(hDlg, IDC_STRING_HOMEPAGE, 5, nDay + 16, 150, 10);", "MoveDlgItem(hDlg, IDC_STRING_HOMEPAGE, 5, nDay + 16 + WA_DICH_Y, 150, 10);"),
        ("RECT rcU = { 0, 0, 160, nDay + 30 };", "RECT rcU = { 0, 0, 160, nDay + 30 + WA_DICH_Y };"),
    ):
        t.thay(cu, moi, moi)
    # goi ham dich TRUOC ShowTab trong WM_INITDIALOG
    t.them_truoc_dong("\t\tShowTab(hDlg, 2);",
                      "\t\tWA_DichKhoiNoiDung(hDlg);\t// (04/09) dich ca khoi noi dung xuong duoi vung tren",
                      "WA_DichKhoiNoiDung(hDlg);")
    # nap combo che do nhanh (dat ngay sau khi dich)
    t.them_sau_dong("\t\tWA_DichKhoiNoiDung(hDlg);",
                    """\t\t{	// (04/09) combo CHE DO NHANH cua vung tren
\t\t\tHWND hCD = ::GetDlgItem(hDlg, IDC_COMBO_TOP_CD);
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"Thường");
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"PK");
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"TK");
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"CTC");
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"Liên đấu");
\t\t\tSendMessage(hCD, CB_ADDSTRING, 0, (LPARAM)L"Dã Tẩu");
\t\t\tSendMessage(hCD, CB_SETCURSEL, 0, 0);
\t\t}""",
                    'CB_ADDSTRING, 0, (LPARAM)L"Liên đấu");')
    # WM_DRAWITEM: 3 nut chrome
    t.them_truoc_dong("\t\tif (dis->CtlID == IDC_BTN_7_PYS)", DRAW_TREN, "3 nut chrome vung tren kieu auto Thai")
    # WM_COMMAND: 7 control moi
    t.them_truoc_dong("\t\t\tcase IDC_STRING_HOMEPAGE:", CMD_TREN, "case IDC_BTN_TOP_A:")
    # xoay chu chay trong nhip san co
    t.them_sau_dong("\t\t\tAppLoop(hDlg);", "\t\t\tWA_XoayChuChay(hDlg);\t// (04/09) chu chay hang 1", "WA_XoayChuChay(hDlg);")
    # o "Trong thanh" cung mau do nhu Thai
    t.thay("\t\t\t\tIDC_CHECKBOX_16_TIM,\n", "\t\t\t\tIDC_CHECKBOX_16_TIM, IDC_CHECKBOX_16_THANH,\n",
           "IDC_CHECKBOX_16_TIM, IDC_CHECKBOX_16_THANH,")
    # tab 15 ngan lai sau khi rut 5 o len tren
    t.thay("326, 328, 225, 335, 270, 244", "326, 328, 225, 335, 270, 208", "335, 270, 208")
    doi = t.ghi()

    if LOI:
        print("LOI:")
        for l in LOI: print("  " + l)
        sys.exit(1)
    if not THU:
        for f in ("Resource.h", "WAuto.rc", "WAuto.cpp"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
