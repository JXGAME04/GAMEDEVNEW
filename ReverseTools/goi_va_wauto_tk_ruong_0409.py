# -*- coding: utf-8 -*-
"""
goi_va_wauto_tk_ruong_0409.py - TONG KIM: HET TRAN VE THANH -> TOI RUONG CUA THANH DO.

Chu game 04/09: "them tinh nang tu chon het tran ve o tab tong kim ve thanh nao thi luu
ruong thanh do co them tuy chon luu ruong dong - tay - nam - bac - trung tam tuy thanh
co san o than hanh phu doi chieu lam theo".

Vao viec:
  1. ipc_shared.h (CA HAI ban D:\GAMEDEVNEW va E:\Src_Auto_Ngoai) - them 3 truong o CUOI
     struct autoData: bTKRuong / nTKRuongHuong / bTKRuongCat (+ khoi tao).
  2. CoreShell.cpp - pha moi TKP_RUONG giua TKP_VETHANH va TKP_DONE, 2 ham phu
     TK_TimRuongObj / TK_CatDo, va moc o TKP_VETHANH khi da ve toi thanh.
  3. WAuto Resource.h / WAuto.rc / WAuto.cpp - 4 o dieu khien moi trong tab 9 (TK).

Bang toa do ruong g_TKRuong da nam san trong KTongKimTables.h (sinh boi gen_tk_ruong.py).

Chay: python goi_va_wauto_tk_ruong_0409.py [--thu]
CoreShell.cpp / ipc_shared.h / KTongKimTables.h la TCVN3 -> latin-1.
WAuto.cpp / WAuto.rc la UTF-16LE; Resource.h ASCII. Idempotent.
"""
import io, os, sys, shutil, re

# LUAT (feedback-edit-tool-pha-tcvn3 / RULE 0): KHONG BAO GIO go tay byte TCVN3.
# Moi doan tieng Viet trong ma C++ ben duoi viet BINH THUONG giua cap @@...@@,
# ham vn() se ma hoa sang TCVN3 bang dung bang cua skill truoc khi ghi vao tep.
sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
IPC = [os.path.join(CORE, "ipc_shared.h"), os.path.join(WA_E, "ipc_shared.h")]
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="latin-1"):
        self.path = path
        self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        t = t.replace("\r\n", "\n").replace("\n", self.nl)
        # tep nguon JX1 (latin-1) -> ma hoa @@tieng Viet@@ sang TCVN3;
        # WAuto.cpp/.rc la UTF-16 nen giu nguyen Unicode.
        return vn(t) if self.enc == "latin-1" else t.replace("@@", "")

    def thay(self, cu, moi, dau):
        cu = self.N(cu)
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58])
            return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:90]))
            return
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau[:58])

    def them_sau_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58])
            return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a)))
            return
        i = a[0]
        self.s = self.nl.join(L[:i + 1] + self.N(moi).split(self.nl) + L[i + 1:])
        self.log.append("  them sau dong: %s" % dau[:58])

    def them_truoc_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58])
            return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a)))
            return
        i = a[0]
        self.s = self.nl.join(L[:i] + self.N(moi).split(self.nl) + L[i:])
        self.log.append("  them truoc dong: %s" % dau[:58])

    def them_truoc_dong_sau(self, neo, chua, moi, dau):
        """Chen truoc dong DAU TIEN chua `chua` ma NAM SAU dong chua `neo`."""
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58])
            return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if neo in l]
        if len(a) != 1:
            LOI.append("%s: neo %r xuat hien %d lan" % (os.path.basename(self.path), neo, len(a)))
            return
        for i in range(a[0] + 1, len(L)):
            if chua in L[i]:
                self.s = self.nl.join(L[:i] + self.N(moi).split(self.nl) + L[i:])
                self.log.append("  them truoc dong (sau neo): %s" % dau[:58])
                return
        LOI.append("%s: sau neo %r khong thay dong %r" % (os.path.basename(self.path), neo, chua))

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        cao_goc = sum(1 for c in self.goc if ord(c) > 127)
        cao_moi = sum(1 for c in self.s if ord(c) > 127)
        if self.enc == "latin-1" and cao_moi < cao_goc:
            LOI.append("%s: MAT %d byte cao - dung ghi" % (os.path.basename(self.path), cao_goc - cao_moi))
            return False
        print("%s: %d thay doi (byte cao %d -> %d)" % (os.path.basename(self.path), len(self.log), cao_goc, cao_moi))
        for l in self.log:
            print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# ---------------------------------------------------------------- ipc_shared.h
IPC_TRUONG = """
	// (04/09) TONG KIM - het tran ve thanh xong thi TOI RUONG cua thanh do.
	// Cham vao ruong chay script obj "OpenBox(); SetRevPos(nn)" -> vua mo ruong vua
	// DAT LAI DIEM HOI SINH ve dung cua thanh nguoi choi chon (bang g_TKRuong trong
	// KTongKimTables.h, sinh tu shenxingfu.lua + RevivePos.ini).
	int		bTKRuong;			// 1 = bat (mac dinh 0)
	int		nTKRuongHuong;		// 0 Trung Tam / 1 Dong / 2 Tay / 3 Nam / 4 Bac / 5 gan nhat
	int		bTKRuongCat;		// 1 = cat TRANG BI trong hanh trang vao ruong (giu thuoc/phu)"""

IPC_KHOI = """		bTKRuong = 0;
		nTKRuongHuong = 5;		// mac dinh: ruong gan cho dung nhat
		bTKRuongCat = 1;"""

# ---------------------------------------------------------------- CoreShell.cpp
CS_ENUM = """	TKP_RUONG,		// (04/09) da ve thanh: toi RUONG cua huong da chon (dat diem hoi sinh + cat do)"""

CS_TEN = """		case TKP_RUONG:  sz = "@@Tống Kim: tới rương của thành@@"; break;"""

CS_HAM = r"""
// ============ TONG KIM: VE THANH XONG THI TOI RUONG (04/09/2026) ============
// Chu game 04/09: "ve thanh nao thi luu ruong thanh do, co them tuy chon luu ruong
// dong - tay - nam - bac - trung tam tuy thanh, co san o than hanh phu doi chieu lam theo".
// Bang g_TKRuong / g_TKRuongRev (KTongKimTables.h) sinh tu bin\server\script\item\ib\
// shenxingfu.lua (RUONG_ARRAY + THANH_ARRAY) va settings\RevivePos.ini: HUONG cua menu
// Than Hanh Phu ung voi RUONG nam gan diem hoi sinh cua chinh huong do nhat (thu tu
// RUONG_ARRAY KHONG trung thu tu ten tep obj nen phai ghep bang khoang cach).
// Cham vao ruong = server chay script obj "OpenBox(); SetRevPos(nn);" -> vua mo ruong
// vua DAT LAI DIEM HOI SINH ve dung cua thanh do. Server chi cho cham khi cach <= 200 mps
// (KProtocolProcess.cpp:6525, defMAX_EXEC_OBJ_SCRIPT_DISTANCE) va obj phai dang DONG.
// Rieng viec CAT DO thi KHONG can dung canh ruong: c2sdnmbr_exchangeitem chi doi
// m_CUnlocked (mat khau ruong o tab Hau can) va khong o fight-mode - dung lai
// DT_EnsureUnlock / DT_ChestRoomFor / DT_BagToBox cua may Da Tau.

// tim OBJ ruong (Obj_Kind_Box dang DONG) gan diem (nMpsX,nMpsY) nhat trong ban kinh nR mps.
static int TK_TimRuongObj(int nMpsX, int nMpsY, int nR)
{
	int nBest = 0, nBd = nR + 1;
	int nObj = ObjSet.GetNext(0);
	while (nObj)
	{
		if (Object[nObj].m_nKind == Obj_Kind_Box && Object[nObj].m_nState == OBJ_BOX_STATE_CLOSE)
		{
			int dX = 0, dY = 0;
			Object[nObj].GetMpsPos(&dX, &dY);
			int d = g_GetDistance(nMpsX, nMpsY, dX, dY);
			if (d < nBd)
			{
				nBd = d;
				nBest = nObj;
			}
		}
		nObj = ObjSet.GetNext(nObj);
	}
	return nBest;
}

// cat MOT mon TRANG BI trong hanh trang vao ruong (thuoc / phu / nguyen lieu giu nguyen
// trong tui de auto con chay tiep). Tra: 1 = vua gui mot lenh (goi lai nhip sau),
// 0 = khong con mon nao de cat, -1 = chiu (fight-mode / khong co mat khau / ruong day).
static int TK_CatDo(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
		return -1;		// ngoai thanh server tu choi moi thao tac ruong
	if (!Player[nPlayerIdx].m_CUnlocked && !pAp->szBoxPass[0])
		return -1;		// ruong khoa ma tab Hau can chua co mat khau
	if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
		return 1;		// dang go mat khau ruong
	KItemList& il = Player[nPlayerIdx].m_ItemList;
	for (int i = 0; i < EQUIPMENT_ROOM_HEIGHT; ++i)
		for (int j = 0; j < EQUIPMENT_ROOM_WIDTH; ++j)
		{
			int nIdx = il.m_Room[room_equipment].FindItem(j, i);
			if (nIdx <= 0)
				continue;
			if (Item[nIdx].GetGenre() != item_equip)
				continue;
			int nDst = DT_ChestRoomFor(nPlayerIdx, pAp, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
			if (!nDst)
				return -1;	// ruong day
			DT_BagToBox(nIdx, nDst);
			return 1;
		}
	return 0;
}
"""

CS_MOC = """				// (04/09) da ve toi thanh: bat "toi ruong" thi di tiep sang pha RUONG
				// (toi ruong cua huong da chon -> dat lai diem hoi sinh + cat do),
				// khong bat thi tra may lai cho auto cu nhu truoc.
				if (pAp->bTKRuong)
				{
					TK_Msg(nPlayerIdx, "<color=Cyan>@@Đã về tới thành đã chọn - đi tiếp tới rương.@@");
					TK_Pha(nPlayerIdx, TKP_RUONG, uCurTime);
					return 1;
				}"""

CS_PHA = r"""
	case TKP_RUONG:
	{
		// (04/09) da dung trong thanh da chon: di toi RUONG cua huong nguoi choi chon
		// -> cham vao (OpenBox + SetRevPos: dat lai diem hoi sinh dung cua) -> cat
		// trang bi trong hanh trang vao ruong neu co tick. Xong thi tra may cho auto cu.
		ea.nTKHold = 1;
		int nVe = pAp->nTKVeThanh;
		if (nVe < 0 || nVe >= LD_VE_COUNT)
			nVe = 0;
		if (nMap != (int)g_LDVeMap[nVe])
		{
			// bi keo di noi khac (phu lac / Xa Phu) - quay lai mach di ve thanh
			TK_Pha(nPlayerIdx, TKP_VETHANH, uCurTime);
			return 1;
		}
		if (nVe >= TK_RUONG_THANH)
		{
			ea.nTKPhase = TKP_DONE;
			ea.nTKHold = 0;
			return 0;
		}
		// huong: nguoi choi chon; thanh KHONG co huong do (Dai Ly chi co Trung Tam +
		// Bac, Lam An khong co Trung Tam/Tay...) hoac chon "gan nhat" -> lay ruong
		// gan cho dang dung nhat trong cac huong thanh do that su co.
		int nH = pAp->nTKRuongHuong;
		if (nH < 0 || nH >= TK_RUONG_HUONG || g_TKRuong[nVe][nH].x <= 0)
		{
			int nBest = -1, nBd = 0;
			for (int h = 0; h < TK_RUONG_HUONG; ++h)
			{
				if (g_TKRuong[nVe][h].x <= 0)
					continue;
				int d = g_GetDistance(nX, nY, TK_O((int)g_TKRuong[nVe][h].x), TK_O((int)g_TKRuong[nVe][h].y));
				if (nBest < 0 || d < nBd)
				{
					nBd = d;
					nBest = h;
				}
			}
			if (nBest < 0)
			{
				TK_Msg(nPlayerIdx, "<color=Yellow>@@Thành này không có rương trong bảng - trả máy lại cho auto cũ.@@");
				ea.nTKPhase = TKP_DONE;
				ea.nTKHold = 0;
				return 0;
			}
			nH = nBest;
		}
		{
			const int nRx = TK_O((int)g_TKRuong[nVe][nH].x);
			const int nRy = TK_O((int)g_TKRuong[nVe][nH].y);
			if (ea.nTKStep == 0)
			{
				// buoc 0: di bo/cuoi ngua toi sat ruong (server doi <= 200 mps moi cho cham)
				if (!DT_WalkTo(nPlayerIdx, nRx, nRy, 150, uCurTime))
				{
					if (++ea.nTKTry > 900)		// ~6 phut van chua toi thi thoi
					{
						TK_Msg(nPlayerIdx, "<color=Yellow>@@Không đi tới được rương đã chọn - trả máy lại cho auto cũ.@@");
						ea.nTKPhase = TKP_DONE;
						ea.nTKHold = 0;
						return 0;
					}
					ea.uTKNext = uCurTime + 400;
					return 1;
				}
				ea.nTKStep = 1;
				ea.nTKTry = 0;
				ea.uTKNext = uCurTime + 300;
				return 1;
			}
			if (ea.nTKStep == 1)
			{
				// buoc 1: cham vao ruong -> server chay OpenBox(); SetRevPos(nn)
				int nObj = TK_TimRuongObj(nRx, nRy, 320);
				if (nObj)
				{
					Player[nPlayerIdx].CheckObject(nObj);
					TK_Msg(nPlayerIdx, "<color=Cyan>@@Đã tới rương - đặt lại điểm hồi sinh ở cửa này.@@");
				}
				else if (++ea.nTKTry < 12)
				{
					ea.uTKNext = uCurTime + 400;
					return 1;
				}
				else
					TK_Msg(nPlayerIdx, "<color=Yellow>@@Đứng đúng chỗ nhưng không thấy rương - vẫn cất đồ bình thường.@@");
				ea.nTKStep = 2;
				ea.nTKTry = 0;
				ea.uTKNext = uCurTime + 900;
				return 1;
			}
		}
		if (!pAp->bTKRuongCat)
		{
			TK_Msg(nPlayerIdx, "<color=Cyan>@@Xong việc ở rương - trả máy lại cho auto cũ.@@");
			ea.nTKPhase = TKP_DONE;
			ea.nTKHold = 0;
			return 0;
		}
		{
			// buoc 2: cat trang bi trong hanh trang vao ruong (moi nhip mot mon)
			int nR = TK_CatDo(nPlayerIdx, pAp, uCurTime);
			if (nR > 0)
			{
				if (++ea.nTKTry <= 80)
				{
					ea.uTKNext = uCurTime + 800;
					return 1;
				}
				TK_Msg(nPlayerIdx, "<color=Yellow>@@Cất đồ quá lâu - dừng lại, phần còn lại để trong túi.@@");
			}
			else if (nR < 0)
				TK_Msg(nPlayerIdx, "<color=Yellow>@@Không cất được vào rương (rương đầy / chưa có mật khẩu rương ở tab Hậu cần).@@");
			else if (ea.nTKTry > 0)
				TK_Msg(nPlayerIdx, "<color=Cyan>@@Đã cất trang bị vào rương xong - trả máy lại cho auto cũ.@@");
			else
				TK_Msg(nPlayerIdx, "<color=Cyan>@@Trong túi không có trang bị nào cần cất - trả máy lại cho auto cũ.@@");
		}
		ea.nTKPhase = TKP_DONE;
		ea.nTKHold = 0;
		return 0;
	}
"""

# ---------------------------------------------------------------- Resource.h
RES_ID = """#define IDC_CHECKBOX_9_RUONG	475
#define IDC_STRING_9_RH			476
#define IDC_COMBO_9_RH			477
#define IDC_CHECKBOX_9_RCAT		478"""

# ---------------------------------------------------------------- WAuto.rc
RC_CTRL = """	CONTROL "Về thành xong thì tới rương", IDC_CHECKBOX_9_RUONG, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 271, 152, 10
	LTEXT "Rương cửa:", IDC_STRING_9_RH, 4, 283, 46, 10, SS_CENTERIMAGE
	COMBOBOX IDC_COMBO_9_RH, 52, 282, 104, 120, WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS
	CONTROL "Cất trang bị trong túi vào rương", IDC_CHECKBOX_9_RCAT, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 295, 152, 10"""

# ---------------------------------------------------------------- WAuto.cpp
CPP_BANG = """// (04/09) TONG KIM - RUONG: thanh nao CO nhung huong nao (theo menu Than Hanh Phu,
// doi chieu bang g_TKRuong trong Sources\\Core\\Src\\KTongKimTables.h - sinh tu
// shenxingfu.lua + RevivePos.ini). Hang = thu tu o "Hết trận về"; cot = 0 Trung Tam /
// 1 Dong / 2 Tay / 3 Nam / 4 Bac. Combo "Rương cửa" chi liet ke huong thanh do CO THAT.
#define WA_TKR_THANH	7
#define WA_TKR_HUONG	5
static const wchar_t* s_aTKRHuong[WA_TKR_HUONG] = {
	L"Trung tâm", L"Đông", L"Tây", L"Nam", L"Bắc" };
static const BYTE s_aTKRCo[WA_TKR_THANH][WA_TKR_HUONG] = {
	{ 1, 1, 1, 1, 1 },	// Phượng Tường Phủ
	{ 1, 1, 1, 1, 1 },	// Thành Đô Phủ
	{ 1, 0, 0, 0, 1 },	// Đại Lý
	{ 1, 1, 1, 0, 1 },	// Biện Kinh Phủ
	{ 1, 1, 1, 1, 1 },	// Tương Dương Phủ
	{ 1, 1, 1, 1, 1 },	// Dương Châu Phủ
	{ 0, 1, 0, 1, 1 },	// Lâm An Phủ
};

// nap lai combo "Rương cửa" theo thanh dang chon o combo "Hết trận về".
// Moi muc mang MA HUONG that (CB_SETITEMDATA) nen thu tu dong khong anh huong gia tri
// luu xuong autoData; muc cuoi luon la "Gần nhất" (ma 5 - de may tu chon).
static void WA_NapRuongHuong(HWND hDlg, int nThanh, int nChon)
{
	HWND hRH = ::GetDlgItem(hDlg, IDC_COMBO_9_RH);
	if (!hRH)
		return;
	SendMessage(hRH, CB_RESETCONTENT, 0, 0);
	if (nThanh < 0 || nThanh >= WA_TKR_THANH)
		nThanh = 0;
	int nSel = -1;
	for (int h = 0; h < WA_TKR_HUONG; ++h)
	{
		if (!s_aTKRCo[nThanh][h])
			continue;
		int nRow = (int)SendMessage(hRH, CB_ADDSTRING, 0, (LPARAM)s_aTKRHuong[h]);
		SendMessage(hRH, CB_SETITEMDATA, nRow, (LPARAM)h);
		if (h == nChon)
			nSel = nRow;
	}
	int nRow = (int)SendMessage(hRH, CB_ADDSTRING, 0, (LPARAM)L"Gần nhất (tự chọn)");
	SendMessage(hRH, CB_SETITEMDATA, nRow, (LPARAM)5);
	if (nSel < 0)
		nSel = nRow;
	SendMessage(hRH, CB_SETCURSEL, nSel, 0);
}
"""

CPP_LUU = """	gnode.apdata.bTKRuong = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_9_RUONG) == BST_CHECKED);
	gnode.apdata.bTKRuongCat = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_9_RCAT) == BST_CHECKED);
	{	// (04/09) lay MA HUONG tu item data - khong lay chi so dong (moi thanh mot danh sach khac)
		HWND hRH = ::GetDlgItem(hDlg, IDC_COMBO_9_RH);
		int nRow = (int)SendMessage(hRH, CB_GETCURSEL, 0, 0);
		int nMa = (nRow >= 0) ? (int)SendMessage(hRH, CB_GETITEMDATA, nRow, 0) : 5;
		gnode.apdata.nTKRuongHuong = (nMa >= 0 && nMa <= 5) ? nMa : 5;
	}"""

CPP_NAP = """	CheckDlgButton(hDlg, IDC_CHECKBOX_9_RUONG, gnode.apdata.bTKRuong?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(hDlg, IDC_CHECKBOX_9_RCAT, gnode.apdata.bTKRuongCat?BST_CHECKED:BST_UNCHECKED);
	WA_NapRuongHuong(hDlg, gnode.apdata.nTKVeThanh, gnode.apdata.nTKRuongHuong);"""

CPP_SHOW = """		ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_9_RUONG ), SW_SHOW);
		ShowWindow(::GetDlgItem( hDlg, IDC_STRING_9_RH ), SW_SHOW);
		ShowWindow(::GetDlgItem( hDlg, IDC_COMBO_9_RH ), SW_SHOW);
		ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_9_RCAT ), SW_SHOW);"""

CPP_TIP = """	{ IDC_CHECKBOX_9_RUONG, L"Hết trận Tống Kim, sau khi đã về tới thành đã chọn ở ô trên thì auto đi tiếp tới RƯƠNG của thành đó. Chạm vào rương vừa mở rương vừa ĐẶT LẠI ĐIỂM HỒI SINH về đúng cửa đó (giống khi bạn tự bấm vào rương)." },
	{ IDC_STRING_9_RH, L"Chọn rương ở cửa nào của thành. Danh sách chỉ hiện những cửa mà thành đó THẬT SỰ có, lấy đúng theo menu Thần Hành Phù." },
	{ IDC_COMBO_9_RH, L"Trung tâm / Đông / Tây / Nam / Bắc tuỳ thành, hoặc Gần nhất để auto tự chọn rương gần chỗ đứng nhất. Chọn cửa nào thì điểm hồi sinh cũng về cửa đó." },
	{ IDC_CHECKBOX_9_RCAT, L"Tới rương xong thì cất TRANG BỊ trong hành trang vào rương. Thuốc, phù, nguyên liệu được giữ nguyên trong túi để auto còn chạy tiếp. Cần mật khẩu rương ở tab Hậu cần (hoặc rương đang mở)." },"""

# combo "Rương cửa" tu luu nhu moi combo khac; rieng combo "Hết trận về" phai co khoi
# RIENG vi doi thanh thi danh sach huong cung phai nap lai.
CPP_COMBO_CU = """			case IDC_COMBO_9_VE:"""
CPP_COMBO_MOI = """			case IDC_COMBO_9_RH:"""

CPP_VE_RIENG = """			case IDC_COMBO_9_VE:
			{	// (04/09) doi THANH se ve -> nap lai danh sach "Rương cửa" cho dung thanh do
				if (HIWORD(wParam) == CBN_SELCHANGE && m_nCurSel < (int)m_vGameNode.size()
				 && m_vGameNode[m_nCurSel].player.dwPID)
				{
					int nThanh = (int)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
					WA_NapRuongHuong(hDlg, nThanh, m_vGameNode[m_nCurSel].apdata.nTKRuongHuong);
					SaveRoleData(hDlg, m_vGameNode[m_nCurSel]);
				}
				break;
			}"""


def va_ipc():
    for p in IPC:
        t = Tep(p)
        t.them_sau_dong("\tunsigned int uACTuoi;", IPC_TRUONG, "int\t\tbTKRuong;")
        t.them_sau_dong("\t\tuACTuoi = 0;", IPC_KHOI, "bTKRuong = 0;")
        t.ghi()


def va_coreshell():
    p = os.path.join(CORE, "CoreShell.cpp")
    t = Tep(p)
    # 1. enum: TKP_RUONG dat TRUOC TKP_DONE (khong co cho nao so sanh thu tu pha)
    t.them_truoc_dong("\tTKP_DONE\t\t// xong khung gio nay - tha may", CS_ENUM, "TKP_RUONG,")
    # 2. hai ham phu - dat ngay truoc may chinh Tong Kim
    t.them_truoc_dong("// ================== MAY CHINH TONG KIM ==================", CS_HAM,
                      "static int TK_TimRuongObj(")
    # 3. moc o TKP_VETHANH: chen NGAY TRUOC dong dat TKP_DONE dau tien sau 'if (nMap == nDest)'
    t.them_truoc_dong_sau("\t\t\tif (nMap == nDest)", "ea.nTKPhase = TKP_DONE;", CS_MOC,
                          "TK_Pha(nPlayerIdx, TKP_RUONG, uCurTime);")
    # 4. pha moi - dat truoc 'default:' DAU TIEN sau 'case TKP_VETHANH:'
    # ("case TKP_VETHANH:" co 2 cho: may TK va bang ten pha - neo bang binh luan duy nhat)
    t.them_truoc_dong_sau("// (26/08) chu game doi: het tran dua nguoi choi VE THANH da chon",
                          "\tdefault:", CS_PHA, "\tcase TKP_RUONG:")
    # ten pha hien o cot "Viec" cua danh sach WAuto
    t.them_truoc_dong("\t\tcase TKP_VETHANH: sz =", CS_TEN, "case TKP_RUONG: sz =")
    t.ghi()


def va_resource():
    p = os.path.join(WA_E, "Resource.h")
    t = Tep(p, "latin-1")
    t.them_sau_dong("#define IDC_SEP_9B\t\t\t\t474", RES_ID, "IDC_CHECKBOX_9_RUONG\t475")
    t.ghi()


def va_rc():
    p = os.path.join(WA_E, "WAuto.rc")
    t = Tep(p, "utf-16")
    t.them_sau_dong("COMBOBOX IDC_COMBO_9_VE, 58, 258, 98, 140", RC_CTRL, "IDC_CHECKBOX_9_RUONG")
    t.ghi()


def va_cpp():
    p = os.path.join(WA_E, "WAuto.cpp")
    t = Tep(p, "utf-16")
    # bang huong + ham nap combo. PHAI dat TRUOC moi noi goi: SaveRoleData (~1130) va
    # UpdateRoleData (~2305) deu o tren ShowTab (~4180) -> neo vao ham dau tien cua tep.
    t.them_truoc_dong("static HFONT WA_FontNhom(HWND hDlg)", CPP_BANG, "WA_NapRuongHuong(HWND hDlg")
    # luu / nap
    t.them_sau_dong("gnode.apdata.nTKVeThanh = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_9_VE), CB_GETCURSEL, 0, 0);",
                    CPP_LUU, "gnode.apdata.bTKRuong = (IsDlgButtonChecked")
    t.them_sau_dong("ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_9_VE), gnode.apdata.nTKVeThanh);",
                    CPP_NAP, "CheckDlgButton(hDlg, IDC_CHECKBOX_9_RUONG")
    # hien trong tab 9
    t.them_sau_dong("\t\tShowWindow(::GetDlgItem( hDlg, IDC_COMBO_9_VE ), SW_SHOW);", CPP_SHOW,
                    "ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_9_RUONG )")
    # ghi chu ro chuot
    t.them_sau_dong("\t{ IDC_COMBO_9_VE, L\"Chọn thành sẽ về sau khi hết trận Tống Kim", CPP_TIP,
                    "{ IDC_CHECKBOX_9_RUONG, L\"Hết trận Tống Kim")
    # combo "Rương cửa" vao dai tu luu; combo "Hết trận về" tach ra khoi dai (co khoi rieng)
    t.thay(CPP_COMBO_CU, CPP_COMBO_MOI, "case IDC_COMBO_9_RH:")
    t.them_truoc_dong("\t\t\tcase IDC_COMBO_0_CH:", CPP_VE_RIENG, "WA_NapRuongHuong(hDlg, nThanh,")
    # tab 9 cao them cho 4 o moi (268 -> 306; van duoi tran 335 cua tab Chieu KH)
    t.thay("286, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 208, 224",
           "286, 279, 279, 304, 282, 260, 266, 303, 294, 306, 326, 328, 225, 335, 270, 208, 224",
           "294, 306, 326, 328")
    # ---- SUA LOI CO SAN (do dot Ac chinh 03/09 chen case sai cho) ----
    # Ca dai case IDC_COMBO_0_CH..IDC_COMBO_15_VE dang ROI THANG vao khoi cua
    # IDC_COMBO_16_AC, nen doi BAT KY combo nao cung ghi ten muc dang chon vao
    # szAcChinhTen. Chuyen khoi 16_AC xuong SAU khoi luu chung thi ca dai roi dung cho.
    if "case IDC_COMBO_16_AC:" in t.s and t.s.index("case IDC_COMBO_16_AC:") < t.s.index("case IDC_COMBO_0_STF:"):
        L = t.s.split(t.nl)
        # ("case IDC_COMBO_16_AC:" con mot lan nua trong WA_LuonHien - loc theo do thut dong)
        a = [i for i, l in enumerate(L) if l.startswith("\t\t\tcase IDC_COMBO_16_AC:")]
        b = [i for i, l in enumerate(L) if "case IDC_COMBO_15_CAP:" in l]
        c = [i for i, l in enumerate(L) if "case IDC_COMBO_0_STF:" in l]
        if len(a) == 1 and len(b) == 1 and len(c) == 1 and a[0] < b[0] < c[0]:
            khoi = L[a[0]:b[0]]
            con = L[:a[0]] + L[b[0]:]
            d = [i for i, l in enumerate(con) if "case IDC_COMBO_0_STF:" in l][0]
            t.s = t.nl.join(con[:d] + khoi + con[d:])
            t.log.append("  chuyen khoi IDC_COMBO_16_AC xuong sau khoi luu chung (sua loi roi case)")
        else:
            LOI.append("WAuto.cpp: khong dinh vi duoc khoi IDC_COMBO_16_AC de chuyen")
    t.ghi()


def main():
    print("== goi_va_wauto_tk_ruong_0409 %s ==" % ("(THU - khong ghi)" if THU else ""))
    va_ipc()
    va_coreshell()
    va_resource()
    va_rc()
    va_cpp()
    if LOI:
        print("\nLOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    if not THU:
        for f in ("Resource.h", "WAuto.rc", "WAuto.cpp", "ipc_shared.h"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
