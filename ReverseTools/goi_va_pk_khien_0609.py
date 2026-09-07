# -*- coding: utf-8 -*-
r"""
goi_va_pk_khien_0609.py - TAB PK: bo qua muc tieu dang co KHIEN BAO VE (AddSkillState 963).

Chu game 06/09: "them tinh nang o tab pk bo qua doi tuong dang co khien bao ve AddSkillState(963, 1, 0, 18*3)".

KHIEN = "vong tron bat tu": moi script dich chuyen goi SetProtectTime(18*3) + AddSkillState(963, 1, 0, 18*3)
(station.lua Xa Phu, shenxingfu.lua Than Hanh Phu, trap cong thanh, trap ra trai Tong Kim kimratrai /
tongratrai.lua, trinhsat.lua, playerlogin.lua; dichuyenmap.lua 18*6; pubg 18*180). Skill 963 = "Thanh Quang
Binh Thuan (chi hieu ung)", StateSpecialId = 159. Danh vao nguoi dang co khien = khong trung, ton luot.

Client biet khien cua NGUOI KHAC qua goi NPC_NORMAL_SYNC (KProtocolProcess.cpp:2453 m_nProtectedTime,
:2465 SetNpcState -> m_btStateInfo). NHUNG client KHONG tu dem lui m_nProtectedTime (KNpc.cpp:1626 nam
trong #ifdef _SERVER) -> gia tri co the "dinh" toi lan dong bo sau. Vi vay PK_CoKhien tu uoc HAN het
khien = luc thay + so khung con lai / 18, va chi keo dai han khi gia tri dong bo THAY DOI.

Vao viec:
  1. ipc_shared.h (CA HAI ban D:\GAMEDEVNEW va E:\Src_Auto_Ngoai): them `bPKBoQuaKhien` o CUOI autoData
     (+ khoi tao = 1). autoData dai them 4 byte -> WAuto.exe va CoreClient.dll phai doi CUNG LUC.
  2. CoreShell.cpp: PK_CoKhien(); loc trong ATYPE_PKFIGHT (muc tieu dang giu + muc tieu moi -> loai 3 s
     qua m_mAutoExcludeNpcID), TK_SanNguoi, TK_ChonDich, LD_ChonDich.
  3. WAuto: Resource.h ID 479; WAuto.rc o tick tab 7 y=336 (nhan ngan, do_wauto_bo_cuc.py 0 nhan cat); WAuto.cpp ShowTab / SaveRoleData / UpdateUI /
     LoadRoleData (mac dinh BAT o ca 2 nhanh) / tooltip / note. Luu ngay nho khoi luu chung 06/09.

Chay: python goi_va_pk_khien_0609.py [--thu]
CoreShell.cpp / ipc_shared.h / Resource.h la latin-1 (TCVN3 / ASCII); WAuto.cpp / WAuto.rc la UTF-16LE.
Tieng Viet trong ma C++ TCVN3 viet giua @@...@@. Idempotent. Chep mirror sang D:\GAMEDEVNEW\WAutoUI.
"""
import io, os, sys, re, shutil
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
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
        return vn(t) if self.enc == "latin-1" else t.replace("@@", "")

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

    def them_truoc_dong_sau(self, neo, chua, moi, dau):
        """Chen truoc dong DAU TIEN chua `chua` ma NAM SAU dong (duy nhat) chua `neo`."""
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60])
            return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if neo in l]
        if len(a) != 1:
            LOI.append("%s: neo %r xuat hien %d lan" % (os.path.basename(self.path), neo, len(a)))
            return
        for i in range(a[0] + 1, min(a[0] + 40, len(L))):
            if chua in L[i]:
                self.s = self.nl.join(L[:i] + self.N(moi).split(self.nl) + L[i:])
                self.log.append("  them truoc dong (sau neo): %s" % dau[:60])
                return
        LOI.append("%s: sau neo %r khong thay dong %r" % (os.path.basename(self.path), neo, chua))

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        if self.enc == "latin-1":
            cao_goc = sum(1 for c in self.goc if ord(c) > 127)
            cao_moi = sum(1 for c in self.s if ord(c) > 127)
            if cao_moi < cao_goc:
                LOI.append("%s: MAT %d byte cao - dung ghi" % (os.path.basename(self.path), cao_goc - cao_moi))
                return False
            print("%s: %d thay doi (byte cao %d -> %d)" % (os.path.basename(self.path), len(self.log), cao_goc, cao_moi))
        else:
            print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log:
            print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# ================================================================== ipc_shared.h
IPC_CU_TRUONG = "\tint\t\tbTKRuongCat;\t\t// 1 = cat TRANG BI trong hanh trang vao ruong (giu thuoc/phu)\n"
IPC_MOI_TRUONG = IPC_CU_TRUONG + """
	// (06/09) TAB PK - bo qua muc tieu dang co KHIEN BAO VE (vong tron bat tu): moi script dich chuyen
	// goi SetProtectTime(18*3) + AddSkillState(963, 1, 0, 18*3) (Xa Phu, Than Hanh Phu, cong thanh,
	// trap ra trai Tong Kim...). Danh vao khong trung. Client nhan qua NPC_NORMAL_SYNC (m_nProtectedTime
	// + StateInfo 159) - CoreShell PK_CoKhien. Ap cho may PK va tang san Tong Kim / Lien dau.
	int		bPKBoQuaKhien;		// 1 = bo qua (mac dinh 1)
"""
IPC_CU_KHOI = "\t\tbTKRuongCat = 1;\n"
IPC_MOI_KHOI = IPC_CU_KHOI + "\t\tbPKBoQuaKhien = 1;\n"


def va_ipc():
    for p in (os.path.join(CORE, "ipc_shared.h"), os.path.join(WA_E, "ipc_shared.h")):
        t = Tep(p)
        t.thay(IPC_CU_TRUONG, IPC_MOI_TRUONG, "int\t\tbPKBoQuaKhien;")
        t.thay(IPC_CU_KHOI, IPC_MOI_KHOI, "\t\tbPKBoQuaKhien = 1;")
        t.ghi()


# ================================================================== CoreShell.cpp
CS_HAM_NEO = "static int TK_ChonDich(int nPlayerIdx, const autoData* pAp, int nLoai)\n{\n"
CS_HAM = r"""// ============ (06/09) TAB PK: BO QUA MUC TIEU DANG CO KHIEN BAO VE ============
// Chu game: "them tinh nang o tab pk bo qua doi tuong dang co khien bao ve AddSkillState(963, 1, 0, 18*3)".
// Khien = "vong tron bat tu": script dich chuyen goi SetProtectTime(18*3) + AddSkillState(963, 1, 0, 18*3)
// (station.lua Xa Phu, shenxingfu.lua, trap cong thanh, trap ra trai Tong Kim kimratrai/tongratrai.lua,
// trinhsat.lua, playerlogin.lua; dichuyenmap.lua 18*6; pubg 18*180). Skill 963 StateSpecialId = 159.
// Client biet khien cua nguoi khac qua NPC_NORMAL_SYNC: KProtocolProcess.cpp:2453 m_nProtectedTime va
// :2465 SetNpcState -> m_btStateInfo. Client KHONG tu dem lui m_nProtectedTime (KNpc.cpp:1626 nam trong
// #ifdef _SERVER) nen gia tri co the DINH toi lan dong bo sau -> tu uoc HAN het khien = luc thay + so khung
// con lai / 18 khung moi giay (chi co state 159 -> PK_KHIEN_MS), va chi tinh lai han khi gia tri dong bo
// THAY DOI (dong bo moi). Het han thi danh binh thuong du co con dinh - khong bao gio bo qua ai mai mai.
#define PK_KHIEN_STATE	159
#define PK_KHIEN_MS		3500u
struct PKKhienNho { UINT uHan; int nProt; };
static std::map<UINT, PKKhienNho> s_mPKKhien;	// dwID -> han het khien uoc tinh
static int PK_CoKhien(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC || !Npc[nIdx].m_dwID)
		return 0;
	const int nProt = Npc[nIdx].m_nProtectedTime;
	int nCoState = 0;
	for (int s = 0; s < MAX_NPC_RECORDER_STATE; ++s)
		if (Npc[nIdx].m_btStateInfo[s] == (BYTE)PK_KHIEN_STATE)
		{
			nCoState = 1;
			break;
		}
	const UINT uID = Npc[nIdx].m_dwID;
	std::map<UINT, PKKhienNho>::iterator it = s_mPKKhien.find(uID);
	if (nProt <= 0 && !nCoState)
	{	// dong bo moi nhat noi khong con khien
		if (it != s_mPKKhien.end())
			s_mPKKhien.erase(it);
		return 0;
	}
	const UINT uNow = timeGetTime();
	const UINT uHan = uNow + ((nProt > 0) ? ((UINT)nProt * 1000u / 18u) : PK_KHIEN_MS);
	if (it == s_mPKKhien.end())
	{
		if (s_mPKKhien.size() > 256)
			s_mPKKhien.clear();
		PKKhienNho nho;
		nho.uHan = uHan;
		nho.nProt = nProt;
		s_mPKKhien[uID] = nho;
		return 1;
	}
	if (nProt > 0 && nProt != it->second.nProt)
	{	// gia tri dong bo doi = may chu vua gui lai -> tinh lai han (khien moi hoac con lai it hon)
		it->second.nProt = nProt;
		it->second.uHan = uHan;
	}
	return ((int)(it->second.uHan - uNow) > 0) ? 1 : 0;
}

"""

CS_TKCHON_CU = """		if (Npc[nIdx].m_Kind == kind_player)
		{
			if (!pAp->bPKPlayer || nLoai == 1)
				continue;
		}
"""
CS_TKCHON_MOI = CS_TKCHON_CU + """		if (pAp->bPKBoQuaKhien && PK_CoKhien(nIdx))
			continue;	// (06/09) dang co khien bao ve - danh khong trung
"""

CS_TKSAN_CU = """		if ((int)Npc[nIdx].m_CurrentCamp != nCampDich)
		{
			++nLoaiPhe;
			continue;	// cung phe (hoac chua co phe) - dung phep thu cua bot
		}
"""
CS_TKSAN_MOI = CS_TKSAN_CU + """		if (pAp->bPKBoQuaKhien && PK_CoKhien(nIdx))
			continue;	// (06/09) vua ra trai / vua dich chuyen, con khien bao ve 3 giay - danh khong trung
"""

CS_LD_CU = """		if (Npc[nIdx].m_Kind != kind_player)
			continue;		// trong san chi co nguoi choi
"""
CS_LD_MOI = CS_LD_CU + """		if (pAp->bPKBoQuaKhien && PK_CoKhien(nIdx))
			continue;	// (06/09) dang co khien bao ve - danh khong trung
"""

CS_PK_GIU_CU = """						if(!nTGNpcIdx || Npc[nTGNpcIdx].m_RegionIndex < 0
						|| Npc[nTGNpcIdx].m_Doing == do_death || Npc[nTGNpcIdx].m_Doing == do_revive
							|| !(NpcSet.GetRelation(nNpcIdx, nTGNpcIdx) == relation_enemy))
							Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
					}
					if(!Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						// (03/09) tang SAN cua Tong Kim dang hanh quan thi NHUONG khe lenh."""
CS_PK_GIU_MOI = """						if(!nTGNpcIdx || Npc[nTGNpcIdx].m_RegionIndex < 0
						|| Npc[nTGNpcIdx].m_Doing == do_death || Npc[nTGNpcIdx].m_Doing == do_revive
							|| !(NpcSet.GetRelation(nNpcIdx, nTGNpcIdx) == relation_enemy)
							|| (pApData->bPKBoQuaKhien && PK_CoKhien(nTGNpcIdx)))	// (06/09) muc tieu vua co khien bao ve -> bo, tim nguoi khac
							Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
					}
					if(!Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						// (03/09) tang SAN cua Tong Kim dang hanh quan thi NHUONG khe lenh."""

CS_PK_MOI_NEO = '"[PK-FIND] t=%u ret=%d prio=%d'
CS_PK_MOI_CHUA = "if(!nTGNpcIdx)"
CS_PK_MOI = """						if(nTGNpcIdx && pApData->bPKBoQuaKhien && PK_CoKhien(nTGNpcIdx))
						{	// (06/09) nguoi tim duoc dang co KHIEN BAO VE (vua dich chuyen / ra trai): loai khoi
							// danh sach tim vai giay (FindTargetNpc bo qua m_mAutoExcludeNpcID) roi nhip sau tim nguoi khac
							Player[nPlayerIdx].m_mAutoExcludeNpcID[Npc[nTGNpcIdx].m_dwID] = uCurTime + PK_KHIEN_MS;
							AUTOLOG_EVERY(1000, "[PK-KHIEN] bo qua id=%u dang co khien bao ve (protect=%d)", Npc[nTGNpcIdx].m_dwID, Npc[nTGNpcIdx].m_nProtectedTime);
							return 0;
						}
"""


def va_coreshell():
    t = Tep(os.path.join(CORE, "CoreShell.cpp"))
    t.thay(CS_HAM_NEO, CS_HAM + CS_HAM_NEO, "static int PK_CoKhien(int nIdx)")
    t.thay(CS_TKCHON_CU, CS_TKCHON_MOI, "(06/09) dang co khien bao ve - danh khong trung\n")
    t.thay(CS_TKSAN_CU, CS_TKSAN_MOI, "vua ra trai / vua dich chuyen, con khien bao ve 3 giay")
    # LD_ChonDich: dau nhan dien phai khac TK_ChonDich (cung cau chu thich) -> kiem theo doan
    if t.N(CS_LD_MOI) in t.s:
        t.log.append("  da co: LD_ChonDich bo qua khien")
    else:
        t.thay(CS_LD_CU, CS_LD_MOI, "<<khong dung dau>>")
    t.thay(CS_PK_GIU_CU, CS_PK_GIU_MOI, "muc tieu vua co khien bao ve -> bo, tim nguoi khac")
    t.them_truoc_dong_sau(CS_PK_MOI_NEO, CS_PK_MOI_CHUA, CS_PK_MOI, "[PK-KHIEN] bo qua id=%u")
    t.ghi()


# ================================================================== WAuto: Resource.h
RES_CU = "#define IDC_CHECKBOX_9_RCAT\t\t478\n"
RES_MOI = RES_CU + "#define IDC_CHECKBOX_7_KHIEN\t479\t// (06/09) tab PK: bo qua muc tieu dang co khien bao ve (hien rieng trong ShowTab tab 7)\n"


def va_resource():
    t = Tep(os.path.join(WA_E, "Resource.h"))
    t.thay(RES_CU, RES_MOI, "IDC_CHECKBOX_7_KHIEN")
    t.ghi()


# ================================================================== WAuto: WAuto.rc (UTF-16)
RC_CU = '\tCONTROL "Vẽ vòng", IDC_CHECKBOX_7_DV, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 141, 171, 42, 12\n'
RC_MOI = RC_CU + '\tCONTROL "Bỏ qua mục tiêu đang có khiên bảo vệ", IDC_CHECKBOX_7_KHIEN, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 5, 336, 177, 12\n'


def va_rc():
    t = Tep(os.path.join(WA_E, "WAuto.rc"), "utf-16")
    t.thay(RC_CU, RC_MOI, "IDC_CHECKBOX_7_KHIEN")
    t.ghi()


# ================================================================== WAuto: WAuto.cpp (UTF-16)
CPP_SHOW_CU = """		for(i=IDC_CHECKBOX_7_F;i<=IDC_STRING_7_VS;++i)
		{
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}
		HWND hCtrl = GetDlgItem(hDlg, IDC_STRING_2_SC);
"""
CPP_SHOW_MOI = """		for(i=IDC_CHECKBOX_7_F;i<=IDC_STRING_7_VS;++i)
		{
			ShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);
		}
		ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_7_KHIEN ), SW_SHOW);	// (06/09) ID 479 nam ngoai dai 7_F..7_VS
		HWND hCtrl = GetDlgItem(hDlg, IDC_STRING_2_SC);
"""

CPP_SAVE_CU = "\tgnode.apdata.nPriority = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_7_PYS) == BST_CHECKED);\n"
CPP_SAVE_MOI = CPP_SAVE_CU + "\tgnode.apdata.bPKBoQuaKhien = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_7_KHIEN) == BST_CHECKED);\t// (06/09)\n"

CPP_UI_CU = "\tCheckDlgButton(hDlg, IDC_CHECKBOX_7_DV, gnode.apdata.bDrawVision?BST_CHECKED:BST_UNCHECKED);\n"
CPP_UI_MOI = CPP_UI_CU + "\tCheckDlgButton(hDlg, IDC_CHECKBOX_7_KHIEN, gnode.apdata.bPKBoQuaKhien?BST_CHECKED:BST_UNCHECKED);\n"

CPP_TIP_CU = '\t{ IDC_CHECKBOX_7_DV, L"Vẽ vòng tầm quét lên màn hình game." },\n'
CPP_TIP_MOI = CPP_TIP_CU + '\t{ IDC_CHECKBOX_7_KHIEN, L"Không đánh mục tiêu đang có khiên bảo vệ 3 giây (vòng tròn bất tử sau khi dịch chuyển bằng Xa Phu, Thần Hành Phù, cổng thành, ra trại Tống Kim). Đánh vào không trúng, chỉ tốn lượt. Áp cho cả tầng săn Tống Kim và Liên đấu." },\n'

CPP_NOTE_CU = "||LƯU Ý: tab PK mượn lại các ô kỹ năng của tab Chiến đấu"
CPP_NOTE_MOI = "||7. [v] Bỏ qua mục tiêu đang có khiên bảo vệ: người vừa dịch chuyển tới (Xa Phu, phù, cổng thành, ra trại Tống Kim) có vòng tròn bất tử 3 giây, đánh không trúng - auto chọn người khác hoặc chờ hết khiên." + CPP_NOTE_CU

CPP_LOAD_CU = "\t\tgnode.apdata.bTKRuongCat = 1;\n\t\t// tab 15 Ac chinh"
CPP_LOAD_MOI = "\t\tgnode.apdata.bTKRuongCat = 1;\n\t\tgnode.apdata.bPKBoQuaKhien = 1;\t// (06/09) tab PK: bo qua muc tieu co khien bao ve - mac dinh BAT\n\t\t// tab 15 Ac chinh"

CPP_CU_CU = "\t\t\tgnode.apdata.bTKRuongCat = 1;\n\t\t}\n\t}\n\tif(bUpUI)"
CPP_CU_MOI = """			gnode.apdata.bTKRuongCat = 1;
		}
		if(uOldSize <= offsetof(autoData, bPKBoQuaKhien))
		{	// (06/09) .dat truoc o 'Bo qua muc tieu dang co khien bao ve' cua tab PK: mac dinh BAT
			gnode.apdata.bPKBoQuaKhien = 1;
		}
	}
	if(bUpUI)"""


def va_cpp():
    t = Tep(os.path.join(WA_E, "WAuto.cpp"), "utf-16")
    t.thay(CPP_SHOW_CU, CPP_SHOW_MOI, "ShowWindow(::GetDlgItem( hDlg, IDC_CHECKBOX_7_KHIEN ), SW_SHOW);")
    t.thay(CPP_SAVE_CU, CPP_SAVE_MOI, "gnode.apdata.bPKBoQuaKhien = (IsDlgButtonChecked")
    t.thay(CPP_UI_CU, CPP_UI_MOI, "CheckDlgButton(hDlg, IDC_CHECKBOX_7_KHIEN,")
    t.thay(CPP_TIP_CU, CPP_TIP_MOI, "{ IDC_CHECKBOX_7_KHIEN, L\"")
    t.thay(CPP_NOTE_CU, CPP_NOTE_MOI, "||7. [v] Bỏ qua mục tiêu đang có khiên bảo vệ")
    t.thay(CPP_LOAD_CU, CPP_LOAD_MOI, "gnode.apdata.bPKBoQuaKhien = 1;\t// (06/09) tab PK")
    t.thay(CPP_CU_CU, CPP_CU_MOI, "offsetof(autoData, bPKBoQuaKhien)")
    t.ghi()


def main():
    print("== goi_va_pk_khien_0609 %s ==" % ("(THU - khong ghi)" if THU else ""))
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
