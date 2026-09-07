# -*- coding: utf-8 -*-
r"""
goi_va_hc_ban_0609.py - HAU CAN buoc 1 (ban rac): mon ban KHONG MAT -> lap vo han -> "ve thanh la dung yen".

Chu game 06/09 toi: "Wauto gio bi loi ve thanh la dung yen".

Do jx_auto.log 19:22-19:25 (map 78 Tuong Duong): [HC-STATE] buoc=1 dung yen hang phut o cung mot toa do,
tui con 28-29 o trong. Ma buoc 1 (CoreShell.cpp ATYPE_RETURN nHomeStep == 1): quet tui, gap mon trang bi
<= xanh, khong khoa, khong phai do nhiem vu -> SendClientCmdSell(id); return 1 -> 300 ms sau quet lai; mon
van con (may chu tu choi im lang: KBuySell::Sell tra FALSE khi khoa / -2 / FightMode, hoac lenh khong toi)
-> gui lai mai, KHONG co gioi han thu, khong sang buoc 2. Ca ngay 18:43 cung mon do khong co nen buoc 1
qua trong 3 giay.

Sua: dem so lan gui ban theo dwID (s_mHCBanThu); qua HC_BAN_THU lan ma mon van trong tui thi bo qua mon do
trong luot nay (quet mon khac / sang buoc 2), ghi log [HC-BAN] ten mon de biet vi sao. Buoc 0 (dau chu ky)
xoa bo dem de luot sau thu lai.

Chay: python goi_va_hc_ban_0609.py [--thu]   (CoreShell.cpp la TCVN3 -> latin-1; ma chen thuan ASCII)
"""
import io, os, sys
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
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
        cao_goc = sum(1 for c in self.goc if ord(c) > 127)
        cao_moi = sum(1 for c in self.s if ord(c) > 127)
        if cao_moi < cao_goc:
            LOI.append("%s: MAT %d byte cao - dung ghi" % (os.path.basename(self.path), cao_goc - cao_moi))
            return False
        print("%s: %d thay doi (byte cao %d -> %d)" % (os.path.basename(self.path), len(self.log), cao_goc, cao_moi))
        for l in self.log:
            print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# 1. bien dem + define, dat ngay truoc case ATYPE_RETURN (trong ham OperationRequest -> khai bao static o day
#    khong duoc; dung static toan cuc dat truoc `case` cung khong hop le) -> chen TRUOC dinh nghia ham chua
#    ATYPE_RETURN thi kho neo; don gian: dat static o pham vi tep ngay sau khoi PK_CoKhien (06/09).
NEO_HAM = "static int TK_ChonDich(int nPlayerIdx, const autoData* pAp, int nLoai)\n{\n"
HAM = """// (06/09) [HC-BAN] HAU CAN buoc 1 (ban rac): mon gui lenh ban ma KHONG MAT (may chu tu choi im lang)
// truoc day bi gui lai 300 ms/lan MAI MAI, nhan vat dung yen trong thanh ("ve thanh la dung yen" - log
// 19:22 map 78: [HC-STATE] buoc=1 hang phut, tui 28 o trong). Nay dem so lan gui theo dwID, qua
// HC_BAN_THU lan van con trong tui thi bo qua mon do trong luot nay; buoc 0 xoa bo dem de luot sau thu lai.
#define HC_BAN_THU	6
static std::map<DWORD, int> s_mHCBanThu;	// dwID -> so lan da gui lenh ban trong luot Hau can nay

"""

# 2. buoc 0: xoa bo dem
CU_B0 = """						g_ScenePlace.RemoveFlag();
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						int nIdx = Player[nPlayerIdx].m_ItemList.Hand();
"""
MOI_B0 = """						g_ScenePlace.RemoveFlag();
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						s_mHCBanThu.clear();	// (06/09) [HC-BAN] luot moi - thu ban lai moi mon
						int nIdx = Player[nPlayerIdx].m_ItemList.Hand();
"""

# 3. buoc 1: bo qua mon da gui qua nhieu lan
CU_SKIP = """										// (20/08) item dat yeu cau nhiem vu Da Tau dang lam: CAM ban
										if(DT_IsQuestItem(nPlayerIdx, pApData, nIdx))
											continue;
"""
MOI_SKIP = """										// (20/08) item dat yeu cau nhiem vu Da Tau dang lam: CAM ban
										if(DT_IsQuestItem(nPlayerIdx, pApData, nIdx))
											continue;
										{	// (06/09) [HC-BAN] mon da gui lenh ban HC_BAN_THU lan ma van con -> bo qua
											std::map<DWORD, int>::iterator itBan = s_mHCBanThu.find((DWORD)Item[nIdx].GetID());
											if(itBan != s_mHCBanThu.end() && itBan->second >= HC_BAN_THU)
												continue;
										}
"""

# 4. buoc 1: dem + log khi gui lenh ban
CU_SEND = """							if(nSelIdx)
							{
								SendClientCmdSell(Item[nSelIdx].GetID());
								return 1;
							}
"""
MOI_SEND = """							if(nSelIdx)
							{
								// (06/09) [HC-BAN] dem so lan gui; toi HC_BAN_THU ma mon van con la bo qua (khoi lap vo han)
								const int nBanThu = ++s_mHCBanThu[(DWORD)Item[nSelIdx].GetID()];
								if(nBanThu == 1 || nBanThu == HC_BAN_THU)
									AUTOLOG("[HC-BAN] gui ban id=%u '%s' genre=%d detail=%d mau=%d khoa=%d/%d lan %d/%d%s",
										Item[nSelIdx].GetID(), Item[nSelIdx].GetName(), (int)Item[nSelIdx].GetGenre(),
										(int)Item[nSelIdx].GetDetailType(), (int)Item[nSelIdx].GetColorItem(),
										(int)Item[nSelIdx].GetPlayerItemLock(), (int)Item[nSelIdx].GetPlayerItemHLock(),
										nBanThu, HC_BAN_THU, (nBanThu >= HC_BAN_THU) ? " - VAN CON TRONG TUI, bo qua mon nay" : "");
								SendClientCmdSell(Item[nSelIdx].GetID());
								return 1;
							}
"""


def main():
    print("== goi_va_hc_ban_0609 %s ==" % ("(THU - khong ghi)" if THU else ""))
    t = Tep(os.path.join(CORE, "CoreShell.cpp"))
    t.thay(NEO_HAM, HAM + NEO_HAM, "static std::map<DWORD, int> s_mHCBanThu;")
    t.thay(CU_B0, MOI_B0, "s_mHCBanThu.clear();")
    t.thay(CU_SKIP, MOI_SKIP, "itBan != s_mHCBanThu.end()")
    t.thay(CU_SEND, MOI_SEND, '"[HC-BAN] gui ban id=%u')
    t.ghi()
    if LOI:
        print("\nLOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    print("XONG")


if __name__ == "__main__":
    main()
