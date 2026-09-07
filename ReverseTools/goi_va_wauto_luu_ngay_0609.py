# -*- coding: utf-8 -*-
r"""
goi_va_wauto_luu_ngay_0609.py - WAuto: MOI O CAU HINH DOI LA LUU NGAY + mac dinh 3 truong ruong TK.

Chu game 06/09: "khi tong kim xong ve thanh voi luu ruong chua hoat dong ... chua luu ruong ...
can ban doc het lai WAuto phan luu - toi muon khi nao config moi la auto tu luu lien".

GOC (doc WAuto.cpp): SaveRoleData chi duoc goi tu cac khoi `case IDC_...:` liet ke TAY trong
WM_COMMAND. Hai o tick moi cua tab TK 04/09 - IDC_CHECKBOX_9_RUONG (475) 'Ve thanh xong thi
toi ruong' va IDC_CHECKBOX_9_RCAT (478) 'Cat trang bi' - KHONG co trong danh sach do (combo
IDC_COMBO_9_RH thi co). Bam vao chung: khong luu .dat, khong cap nhat gnode.apdata (chinh la
khoi gui sang game moi 54 ms) -> CoreClient luon nhan bTKRuong = 0 -> het tran ve toi thanh
la tra may, khong bao gio sang pha TKP_RUONG. Chon lai nhan vat / game ket noi lai ->
UpdateUI tra o tick ve 0. Tep APdata\2513089250.dat (18:05 06/09) xac nhan: bTKRuong = 0,
nTKRuongHuong = 0, bTKRuongCat = 0 du nguoi choi da tick.

SUA:
  1. WM_COMMAND: khoi LUU CHUNG truoc `switch` - bat ky checkbox/radio (BN_CLICKED) hay
     combobox (CBN_SELCHANGE) co ID trong dai cau hinh [IDC_STRING_0_L, IDC_INDEX_END) la
     SaveRoleData ngay. Tu nay them o moi khong con phai nho liet ke case.
  2. LoadRoleData: mac dinh 3 truong ruong TK (tat / cua gan nhat / cat trang bi bat) cho ca
     nhanh 'chua co tep' lan nhanh 'tep cu ngan hon' (truoc day memset 0 -> cua Trung Tam,
     khong cat - trai voi ban giao 04/09).

Chay: python goi_va_wauto_luu_ngay_0609.py [--thu]
WAuto.cpp la UTF-16LE (BOM FF FE). Idempotent. Chep mirror sang D:\GAMEDEVNEW\WAutoUI.
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="utf-16"):
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
            self.log.append("  da co: %s" % dau[:60])
            return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:90]))
            return
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau[:60])

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log:
            print(l)
        if not THU:
            # utf-16 cua Python ghi BOM FF FE + little-endian: dung dinh dang goc cua tep
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# ------------------------------------------------------------------ 1. luu chung
CU_SWITCH = """		if (HIWORD(wParam) == EN_KILLFOCUS
		 && LOWORD(wParam) >= IDC_STRING_0_L && LOWORD(wParam) < IDC_INDEX_END)
		{
			KillTimer(hDlg, TIMER_SAVEEDIT);
			if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				SaveRoleData(hDlg, m_vGameNode[m_nCurSel]);
		}
        switch (LOWORD(wParam))
		{"""

MOI_SWITCH = """		if (HIWORD(wParam) == EN_KILLFOCUS
		 && LOWORD(wParam) >= IDC_STRING_0_L && LOWORD(wParam) < IDC_INDEX_END)
		{
			KillTimer(hDlg, TIMER_SAVEEDIT);
			if (m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
				SaveRoleData(hDlg, m_vGameNode[m_nCurSel]);
		}
		// (06/09) chu game: "khi nao config moi la auto tu luu lien". Truoc day chi cac o
		// duoc liet ke TAY trong cac khoi case ben duoi moi luu; o nao quen liet ke (hai o
		// tick 'Ve thanh xong thi toi ruong' / 'Cat trang bi' cua tab TK 04/09) thi bam
		// xong KHONG luu .dat va KHONG cap nhat apdata (khoi gui sang game moi 54 ms) ->
		// game luon nhan bTKRuong = 0, tinh nang khong bao gio chay; chon lai nhan vat thi
		// UpdateUI tra o tick ve gia tri cu. Nay: BAT KY o tick / o chon (Button kieu
		// checkbox-radio, ComboBox) trong dai ID cau hinh doi la SaveRoleData ngay - doc ca
		// giao dien mot the + ghi .dat; apdata sang game o nhip TIMER_APPLOOP ke tiep.
		// Nut bam thuong (BS_PUSHBUTTON) khong phai cau hinh nen bo qua. CheckDlgButton /
		// ComboBox_SetCurSel tu ma KHONG phat BN_CLICKED / CBN_SELCHANGE -> khong lap.
		// Cac khoi case ben duoi van goi SaveRoleData lan nua: vo hai (ghi lai cung noi dung).
		if (lParam && LOWORD(wParam) >= IDC_STRING_0_L && LOWORD(wParam) < IDC_INDEX_END
		 && m_nCurSel < (int)m_vGameNode.size() && m_vGameNode[m_nCurSel].player.dwPID)
		{
			wchar_t szLop[32] = { 0 };
			GetClassNameW((HWND)lParam, szLop, 31);
			int bLuu = 0;
			if (HIWORD(wParam) == CBN_SELCHANGE && !_wcsicmp(szLop, L"ComboBox"))
				bLuu = 1;
			else if (HIWORD(wParam) == BN_CLICKED && !_wcsicmp(szLop, L"Button"))
			{
				LONG_PTR nKieu = GetWindowLongPtr((HWND)lParam, GWL_STYLE) & BS_TYPEMASK;
				if (nKieu == BS_AUTOCHECKBOX || nKieu == BS_CHECKBOX || nKieu == BS_AUTO3STATE
				 || nKieu == BS_3STATE || nKieu == BS_AUTORADIOBUTTON || nKieu == BS_RADIOBUTTON)
					bLuu = 1;
			}
			if (bLuu)
				SaveRoleData(hDlg, m_vGameNode[m_nCurSel]);
		}
        switch (LOWORD(wParam))
		{"""

# ------------------------------------------------------------------ 2a. mac dinh (chua co tep)
CU_MACDINH = """		gnode.apdata.bTKVeCho = 1;
		gnode.apdata.nTKVeThanh = 0;
		// tab 15 Ac chinh (03/09): mac dinh TAT, khoang cach 200 (auto Thai), khong theo trong thanh"""

MOI_MACDINH = """		gnode.apdata.bTKVeCho = 1;
		gnode.apdata.nTKVeThanh = 0;
		// (06/09) 3 o ruong TK 04/09: tinh nang TAT, cua 'gan nhat', cat trang bi BAT (memset o
		// tren da xoa gia tri cua constructor autoData nen phai dat lai o day)
		gnode.apdata.bTKRuong = 0;
		gnode.apdata.nTKRuongHuong = 5;
		gnode.apdata.bTKRuongCat = 1;
		// tab 15 Ac chinh (03/09): mac dinh TAT, khoang cach 200 (auto Thai), khong theo trong thanh"""

# ------------------------------------------------------------------ 2b. mac dinh (tep cu ngan hon)
CU_CU = """			gnode.apdata.bWANpcTheSame = 0;
			gnode.apdata.bWAMissle = 0;
			gnode.apdata.nWAMissleIndex = 1;
		}
	}
	if(bUpUI)"""

MOI_CU = """			gnode.apdata.bWANpcTheSame = 0;
			gnode.apdata.bWAMissle = 0;
			gnode.apdata.nWAMissleIndex = 1;
		}
		if(uOldSize <= offsetof(autoData, bTKRuong))
		{	// (06/09) .dat truoc dot ruong Tong Kim 04/09: tinh nang TAT, cua gan nhat, cat trang bi bat
			gnode.apdata.bTKRuong = 0;
			gnode.apdata.nTKRuongHuong = 5;
			gnode.apdata.bTKRuongCat = 1;
		}
	}
	if(bUpUI)"""


def va_cpp():
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    t.thay(CU_SWITCH, MOI_SWITCH, '(06/09) chu game: "khi nao config moi la auto tu luu lien"')
    t.thay(CU_MACDINH, MOI_MACDINH, "(06/09) 3 o ruong TK 04/09: tinh nang TAT, cua 'gan nhat'")
    t.thay(CU_CU, MOI_CU, "(06/09) .dat truoc dot ruong Tong Kim 04/09")
    t.ghi()


def main():
    print("== goi_va_wauto_luu_ngay_0609 %s ==" % ("(THU - khong ghi)" if THU else ""))
    va_cpp()
    if LOI:
        print("\nLOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    if not THU:
        shutil.copyfile(os.path.join(WA_E, "WAuto.cpp"), os.path.join(WA_D, "WAuto.cpp"))
        print("da chep mirror WAutoUI\\WAuto.cpp")
    print("XONG")


if __name__ == "__main__":
    main()
