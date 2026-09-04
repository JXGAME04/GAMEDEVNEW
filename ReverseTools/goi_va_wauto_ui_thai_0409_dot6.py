# -*- coding: utf-8 -*-
"""
goi_va_wauto_ui_thai_0409_dot6.py - DOT 6 (cuoi) cua viec dung lai giao dien WAuto theo auto Thai:

  1. TACH tab 0 "Co ban" (cao 362) thanh "Co ban" (280) + tab MOI so 16 "Dang nhap" (224)
     -> HA TRAN chieu cao cua so: tab cao nhat con 335 (Chieu KH) thay vi 362.
     Khong tao ID control moi: 10 o dang nhap (174, 177..185) chi DOI TOA DO trong .rc,
     nen SaveRoleData / UpdateRoleData / tu-luu EN_CHANGE khong phai sua mot dong nao.
  2. HANG NUT DAY kieu Thai: [Bat het] [Tat het] [Dong bo] [Toa do] (ID 730..733)
     chiem dung cho cua IDC_STRING_HOMEPAGE -> KHONG ton them mot px chieu cao nao.
  3. Sua loi co san: IDC_GRP_TAB13 thieu trong bang to mau khung (s_anKhung).

Chay: python goi_va_wauto_ui_thai_0409_dot6.py [--thu]
WAuto.cpp / WAuto.rc UTF-16LE; Resource.h ASCII. Idempotent.
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
            self.log.append("  da co: %s" % dau[:58]); return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi, 1); self.log.append("  thay: %s" % dau[:58])

    def them_sau_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0] + 1] + self.N(moi).split(self.nl) + L[a[0] + 1:])
        self.log.append("  them sau dong: %s" % dau[:58])

    def them_truoc_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0]] + self.N(moi).split(self.nl) + L[a[0]:])
        self.log.append("  them truoc dong: %s" % dau[:58])

    def xoa_dong(self, chua, dau):
        if dau not in self.s:
            self.log.append("  da xoa: %s" % chua[:58]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong xoa %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        del L[a[0]]
        self.s = self.nl.join(L)
        self.log.append("  xoa dong: %s" % chua[:58])

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


NOTE16 = ("ĐĂNG NHẬP — đồng bộ thiết lập và tự vào game||"
 "1. Cài đặt theo + [Đồng bộ]: chép TOÀN BỘ thiết lập của nhân vật khác sang nhân vật đang chọn. "
 "Dùng khi mở thêm cửa sổ mới cho khỏi chỉnh lại từ đầu. Muốn chép NGƯỢC (từ nhân vật đang chọn "
 "sang mọi cửa sổ khác) thì bấm nút [Đồng bộ] ở HÀNG NÚT DƯỚI CÙNG.||"
 "2. [v] Tự động đăng nhập: WAuto tự gõ tài khoản và chọn nhân vật khi game mở lên.||"
 "3. Nhân vật + [Thêm vào]: thêm nhân vật đang chọn vào danh sách tự đăng nhập (tối đa 8).||"
 "4. Danh sách bên dưới + [Xóa] / [Xóa hết]: quản lý danh sách đó.||"
 "LƯU Ý: tài khoản và mật khẩu lấy từ chính cửa sổ game đang đăng nhập, WAuto không hỏi lại.||"
 "LIÊN QUAN: số liệu nhân vật và các ô thoát game nằm ở tab Cơ bản cùng nhóm.")


def c_str(s):
    return s.replace("\\", "\\\\").replace('"', '\\"')


CMD_DAY = """			case IDC_BTN_BOT_ON:
			case IDC_BTN_BOT_OFF:
			{	// (04/09) hang nut day kieu Thai: bat/tat auto cho MOI cua so mot luot
				HWND hL = ::GetDlgItem(hDlg, IDC_PLAYERLIST);
				BOOL bBat = (LOWORD(wParam) == IDC_BTN_BOT_ON);
				int nSo = ListView_GetItemCount(hL);
				for (int k = 0; k < nSo; ++k)
					ListView_SetCheckState(hL, k, bBat);
				break;
			}
			case IDC_BTN_BOT_SYNC:
			{	// (04/09) chep thiet lap cua nhan vat DANG CHON sang MOI cua so khac.
				// Giu rieng ten ac chinh cua tung cua so (khong thi ac phu tro vao chinh no).
				if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				{
					if (MessageBoxW(hDlg,
						L"Chép toàn bộ thiết lập của nhân vật đang chọn sang MỌI cửa sổ khác?",
						L"Đồng bộ", MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						int nSo = 0;
						for (size_t j = 0; j < m_vGameNode.size(); ++j)
						{
							if ((int)j == m_nCurSel || !m_vGameNode[j].player.dwPID)
								continue;
							char szAcCu[sizeof(m_vGameNode[j].apdata.szAcChinhTen)];
							lstrcpynA(szAcCu, m_vGameNode[j].apdata.szAcChinhTen, (int)sizeof(szAcCu));
							memcpy(&m_vGameNode[j].apdata, &m_vGameNode[m_nCurSel].apdata, sizeof(autoData));
							lstrcpynA(m_vGameNode[j].apdata.szAcChinhTen, szAcCu,
								(int)sizeof(m_vGameNode[j].apdata.szAcChinhTen));
							SaveRoleDataFast(m_vGameNode[j]);
							++nSo;
						}
						wchar_t wb[128];
						GVWPRINT(wb, L"Đã chép sang %d cửa sổ.", nSo);
						MessageBoxW(hDlg, wb, L"Đồng bộ", MB_OK | MB_ICONINFORMATION);
					}
				}
				break;
			}
			case IDC_BTN_BOT_POS:
			{	// (04/09) bao toa do nhan vat dang chon ra dong HD (Thai: nut "Bao toa do")
				if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				{
					gameNode& gn = m_vGameNode[m_nCurSel];
					wchar_t* pM = g_Convert2UCEChar(gn.player.szMap, strlen(gn.player.szMap));
					wchar_t wb[192];
					GVWPRINT(wb, L"%s (%d,%d)", pM ? pM : L"—", gn.player.nX / 256, gn.player.nY / 512);
					SetDlgItemText(hDlg, IDC_STRING_HD_V, wb);
					delete [] pM;
				}
				break;
			}
"""

RC_TAB16 = """\t\t// ===== (04/09) tab 16 "Dang nhap" - tach ra tu tab 0 de ha tran chieu cao cua so.
\t\t// 10 o duoi day GIU NGUYEN ID (174, 177..185), chi doi toa do.
\t\tGROUPBOX "Đăng nhập · đồng bộ thiết lập, tự vào game", IDC_GRP_TAB16, 2, 112, 156, 108
\t\tCONTROL "", IDC_SEP_16A, "Static", SS_ETCHEDHORZ, 6, 137, 148, 1
\t\t// ===== (04/09) hang nut day kieu Thai - ShowTab dat lai cho moi lan doi tab =====
\t\tPUSHBUTTON "Bật hết", IDC_BTN_BOT_ON, 4, 400, 36, 12
\t\tPUSHBUTTON "Tắt hết", IDC_BTN_BOT_OFF, 42, 400, 36, 12
\t\tPUSHBUTTON "Đồng bộ", IDC_BTN_BOT_SYNC, 80, 400, 36, 12
\t\tPUSHBUTTON "Toạ độ", IDC_BTN_BOT_POS, 118, 400, 38, 12"""


def main():
    print("== goi_va_wauto_ui_thai_0409_dot6 %s ==" % ("(THU - khong ghi)" if THU else ""))

    # ---------------- Resource.h ----------------
    r = Tep(os.path.join(WA_E, "Resource.h"), "latin-1")
    r.them_sau_dong("#define IDC_EDITOR_0_MIS",
                    "// (04/09) tab 16 \"Dang nhap\" - PHAI nam trong [161,700) de ShowTab an/hien duoc\n"
                    "#define IDC_GRP_TAB16\t\t642\n"
                    "#define IDC_SEP_16A\t\t\t643",
                    "IDC_GRP_TAB16")
    r.them_sau_dong("#define IDC_SEP_TOP",
                    "// (04/09) hang nut day kieu auto Thai (ngoai dai an/hien)\n"
                    "#define IDC_BTN_BOT_ON\t\t730\n"
                    "#define IDC_BTN_BOT_OFF\t\t731\n"
                    "#define IDC_BTN_BOT_SYNC\t732\n"
                    "#define IDC_BTN_BOT_POS\t\t733",
                    "IDC_BTN_BOT_ON")
    r.ghi()

    # ---------------- WAuto.rc ----------------
    rc = Tep(os.path.join(WA_E, "WAuto.rc"))
    # 1. khoi REP3 cua tab 0 don len cho trong (sau khi rut khoi dang nhap ra)
    rc.thay('CONTROL "", IDC_SEP_0C, "Static", SS_ETCHEDHORZ, 6, 334, 148, 1',
            'CONTROL "", IDC_SEP_0C, "Static", SS_ETCHEDHORZ, 6, 248, 148, 1',
            'IDC_SEP_0C, "Static", SS_ETCHEDHORZ, 6, 248')
    rc.thay("IDC_CHECKBOX_0_NTS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 338, 152, 10",
            "IDC_CHECKBOX_0_NTS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 252, 152, 10",
            "IDC_CHECKBOX_0_NTS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 252")
    rc.thay("IDC_CHECKBOX_0_MIS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 350, 122, 10",
            "IDC_CHECKBOX_0_MIS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 264, 122, 10",
            "IDC_CHECKBOX_0_MIS, \"Button\", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 264")
    rc.thay("EDITTEXT IDC_EDITOR_0_MIS, 128, 350, 28, 10", "EDITTEXT IDC_EDITOR_0_MIS, 128, 264, 28, 10",
            "IDC_EDITOR_0_MIS, 128, 264")
    # 2. 10 o dang nhap doi toa do sang tab 16 (GIU NGUYEN ID)
    rc.thay('IDC_CHECKBOX_0_ALG, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 260, 74, 10',
            'IDC_CHECKBOX_0_ALG, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 124, 74, 10',
            'IDC_CHECKBOX_0_ALG, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 124')
    rc.thay('LTEXT "Cài đặt theo",IDC_STRING_0_STF,4,248,40,10', 'LTEXT "Cài đặt theo",IDC_STRING_0_STF,4,142,40,10',
            "IDC_STRING_0_STF,4,142")
    rc.thay("COMBOBOX IDC_COMBO_0_STF, 44, 248, 70, 52,", "COMBOBOX IDC_COMBO_0_STF, 44, 142, 70, 52,",
            "IDC_COMBO_0_STF, 44, 142")
    rc.thay('PUSHBUTTON "Đồng bộ", IDC_BTN_0_STF, 118, 248, 38, 10', 'PUSHBUTTON "Đồng bộ", IDC_BTN_0_STF, 118, 142, 38, 10',
            "IDC_BTN_0_STF, 118, 142")
    rc.thay('LTEXT "Nhân vật",IDC_STRING_0_ALG,4,271,40,10', 'LTEXT "Nhân vật",IDC_STRING_0_ALG,4,156,40,10',
            "IDC_STRING_0_ALG,4,156")
    rc.thay("COMBOBOX IDC_COMBO_0_ALG, 44, 271, 70, 52,", "COMBOBOX IDC_COMBO_0_ALG, 44, 156, 70, 52,",
            "IDC_COMBO_0_ALG, 44, 156")
    rc.thay('PUSHBUTTON "Thêm vào", IDC_BTN_0_ALG, 118, 271, 38, 10', 'PUSHBUTTON "Thêm vào", IDC_BTN_0_ALG, 118, 156, 38, 10',
            "IDC_BTN_0_ALG, 118, 156")
    rc.thay("IDC_LIST_0_ALG, 4, 284, 110, 46", "IDC_LIST_0_ALG, 4, 170, 110, 46", "IDC_LIST_0_ALG, 4, 170")
    rc.thay('PUSHBUTTON "Xóa", IDC_BTN_0_ALD, 118, 285, 38, 12', 'PUSHBUTTON "Xóa", IDC_BTN_0_ALD, 118, 171, 38, 12',
            "IDC_BTN_0_ALD, 118, 171")
    rc.thay('PUSHBUTTON "Xóa hết", IDC_BTN_0_ALDA, 118, 298, 38, 12', 'PUSHBUTTON "Xóa hết", IDC_BTN_0_ALDA, 118, 184, 38, 12',
            "IDC_BTN_0_ALDA, 118, 184")
    # 3. khung tab 16 + hang nut day; go dong chu duoi cung (chu do da vao vong chu chay)
    rc.them_truoc_dong('CTEXT "Võ Lâm Ngạo Thế"', RC_TAB16, "IDC_GRP_TAB16, 2, 112, 156, 108")
    rc.xoa_dong('CTEXT "Võ Lâm Ngạo Thế"', 'CTEXT "Võ Lâm Ngạo Thế"')
    # 4. ten khung tab 0 cho dung noi dung con lai
    rc.thay('GROUPBOX "Cơ bản · phiên chơi, đăng nhập", IDC_GRP_TAB0, 2, 112, 156, 219',
            'GROUPBOX "Cơ bản · số liệu nhân vật, phiên chơi", IDC_GRP_TAB0, 2, 112, 156, 168',
            "IDC_GRP_TAB0, 2, 112, 156, 168")
    rc.ghi()

    # ---------------- WAuto.cpp ----------------
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    t.thay("#define WA_SO_TAB\t\t16", "#define WA_SO_TAB\t\t17", "#define WA_SO_TAB\t\t17")
    t.thay('{ L"Cài đặt",   { 0 },                  1 },', '{ L"Cài đặt",   { 0, 16 },              2 },',
           '{ L"Cài đặt",   { 0, 16 },')
    t.thay('L"Chiêu KH", L"CTC", L"Ác chính",', 'L"Chiêu KH", L"CTC", L"Ác chính", L"Đăng nhập",',
           'L"Ác chính", L"Đăng nhập",')
    t.thay("362, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 208",
           "280, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 208, 224",
           "280, 279, 279, 304")
    # note cho tab 16
    t.thay('\tL"%s",\n};\nstatic const wchar_t* const s_szNoteChung' % c_str(__import__("re").search(r"", "").string) if False else
           "};\nstatic const wchar_t* const s_szNoteChung",
           '\tL"%s",\n};\nstatic const wchar_t* const s_szNoteChung' % c_str(NOTE16),
           "ĐĂNG NHẬP — đồng bộ thiết lập")
    # tach nhanh ShowTab tab 0
    t.thay("""	if(nTabBtn == 0)
	{
		for(i=IDC_STRING_0_L;i<=IDC_STRING_0_NE;++i)
		{
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}""",
           """	if(nTabBtn == 0)
	{	// (04/09) tab 0 GIU: so lieu + tuy chon phien choi + khoi hien thi.
		// Khoi DANG NHAP (174, 177..185) da tach sang tab 16 de ha tran chieu cao cua so.
		for(i=IDC_STRING_0_L;i<=IDC_COMBO_0_CH;++i)		// 161..176
		{
			if(i == IDC_CHECKBOX_0_ALG)					// 174 -> tab 16
				continue;
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}
		for(i=IDC_STRING_0_NL;i<=IDC_STRING_0_NE;++i)	// 186..195
		{
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}""", "tab 0 GIU: so lieu + tuy chon phien choi")
    # nhanh tab 16 (dat ngay truoc nhanh tab 14 nhu tab 15)
    t.them_truoc_dong("\telse if(nTabBtn == 14)",
                      """	else if(nTabBtn == 16)
	{	// (04/09) tab Dang nhap - 10 o tach tu tab 0, GIU NGUYEN ID
		ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_0_ALG ), SW_SHOW);
		for(i=IDC_STRING_0_STF;i<=IDC_BTN_0_ALDA;++i)	// 177..185
		{
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}
	}""", "(04/09) tab Dang nhap - 10 o tach tu tab 0")
    # 3 mang khung/ke + dieu kien
    t.thay("static const int nGrpId[16] = { IDC_GRP_TAB0", "static const int nGrpId[17] = { IDC_GRP_TAB0", "nGrpId[17]")
    t.thay("IDC_GRP_TAB14, IDC_GRP_TAB15 };", "IDC_GRP_TAB14, IDC_GRP_TAB15, IDC_GRP_TAB16 };", "IDC_GRP_TAB15, IDC_GRP_TAB16 };")
    t.thay("static const int nSepFrom[16] = {", "static const int nSepFrom[17] = {", "nSepFrom[17]")
    t.thay("IDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14A, IDC_SEP_15A };",
           "IDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14A, IDC_SEP_15A, IDC_SEP_16A };", "IDC_SEP_15A, IDC_SEP_16A };")
    t.thay("static const int nSepTo[16] = {", "static const int nSepTo[17] = {", "nSepTo[17]")
    t.thay("IDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14B, IDC_SEP_15A };",
           "IDC_SEP_12A, IDC_SEP_13A, IDC_SEP_14B, IDC_SEP_15A, IDC_SEP_16A };", "IDC_SEP_14B, IDC_SEP_15A, IDC_SEP_16A };")
    t.thay("if (nTabBtn >= 0 && nTabBtn <= 15)", "if (nTabBtn >= 0 && nTabBtn <= 16)", "nTabBtn <= 16)")
    # hang nut day thay cho dong chu duoi cung
    t.thay("\t\tMoveDlgItem(hDlg, IDC_STRING_HOMEPAGE, 5, nDay + 16 + WA_DICH_Y, 150, 10);",
           "\t\t// (04/09) hang nut day kieu Thai - dung dung cho cua dong chu cu, khong ton them chieu cao\n"
           "\t\tMoveDlgItem(hDlg, IDC_BTN_BOT_ON,     4, nDay + 16 + WA_DICH_Y, 36, 12);\n"
           "\t\tMoveDlgItem(hDlg, IDC_BTN_BOT_OFF,   42, nDay + 16 + WA_DICH_Y, 36, 12);\n"
           "\t\tMoveDlgItem(hDlg, IDC_BTN_BOT_SYNC,  80, nDay + 16 + WA_DICH_Y, 36, 12);\n"
           "\t\tMoveDlgItem(hDlg, IDC_BTN_BOT_POS,  118, nDay + 16 + WA_DICH_Y, 38, 12);",
           "MoveDlgItem(hDlg, IDC_BTN_BOT_ON,")
    # to mau khung: bo sung TAB13 (thieu san) + TAB16
    t.thay("\t\t\t\tIDC_GRP_TAB12, IDC_GRP_TAB14, IDC_GRP_TAB15,\n",
           "\t\t\t\tIDC_GRP_TAB12, IDC_GRP_TAB13, IDC_GRP_TAB14, IDC_GRP_TAB15,\n"
           "\t\t\t\tIDC_GRP_TAB16,\n", "IDC_GRP_TAB16,\n")
    # WM_COMMAND 4 nut day
    t.them_truoc_dong("\t\t\tcase IDC_BTN_TOP_A:", CMD_DAY, "case IDC_BTN_BOT_ON:")
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
