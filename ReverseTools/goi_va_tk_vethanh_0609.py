# -*- coding: utf-8 -*-
r"""
goi_va_tk_vethanh_0609.py - TONG KIM het tran VE THANH: vá 'về không đúng thành' + nhật ký pha.

Chu game 06/09: "khi tong kim xong ve thanh voi luu ruong chua hoat dong - ve khong dung
thanh - chua luu ruong".

GOC tim duoc khi doc ma (nhat ky phien truoc da bi ghi de nen khong con vet):
  * TKP_END o map bao danh 324 nho Xa Phu "Nhung thanh thi da di qua" dua ve thanh da chon.
    tong_kim_tcap/xaphu.lua dat CurStation = 1 (Phuong Tuong), GetCurStation() khong doi vi
    map 324 khong co trong STATION_ARRAY; station.lua chi liet ke tram co gia > 0 tinh tu
    CurStation (ScriptFuns.cpp LuaGetPlayerStation), ma StationPrice.txt Phuong Tuong ->
    Phuong Tuong = -1. => Chon "Phuong Tuong Phu" (muc MAC DINH cua o 'Het tran ve') thi Xa
    Phu 324 KHONG BAO GIO liet ke no. Ma cu roi map bang 'Tro lai cho luc nay' -> ve map
    luyen cong cu, map do thuong khong co Xa Phu; khong co phu ve thanh thi LD_DiThanh tra
    -1 ngay -> nguoi choi dung lai o map cu / thanh cua phu = "ve khong dung thanh".
  * TK_Msg / TK_Pha / LD_Msg chi hien chat, KHONG ghi jx_auto.log -> khong the truy vet.

SUA (CoreShell.cpp, client):
  1. TKP_END buoc 3: thanh da chon khong co trong danh sach -> di sang MOT THANH KHAC co
     trong danh sach (7 thanh noi nhau qua Xa Phu), TKP_VETHANH se nho Xa Phu / Than Hanh
     Phu o thanh do dua tiep ve dung thanh. Khong con thanh nao -> giu duong cu.
  2. AUTOLOG [TK-MSG] / [TK-PHA] / [LD-MSG] de lan sau doc log biet auto di dau.

Chay: python goi_va_tk_vethanh_0609.py [--thu]
CoreShell.cpp la TCVN3 -> latin-1; tieng Viet viet giua @@...@@ roi ma hoa bang bang skill.
Idempotent.
"""
import io, os, sys, re

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


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
        t = t.replace("\r\n", "\n").replace("\n", self.nl)
        return vn(t)

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


# ------------------------------------------------------------- 1. TK_Msg: ghi log
CU_MSG = """static void TK_Msg(int nPlayerIdx, const char* szMsg)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	UINT uNow = timeGetTime();
	if (ea.uTKMsgT > uNow)
		return;
	ea.uTKMsgT = uNow + 1200;
"""
MOI_MSG = """static void TK_Msg(int nPlayerIdx, const char* szMsg)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	UINT uNow = timeGetTime();
	if (ea.uTKMsgT > uNow)
		return;
	ea.uTKMsgT = uNow + 1200;
	AUTOLOG("[TK-MSG] %s", szMsg);	// (06/09) de truy vet 'het tran ve thanh nao / toi ruong' trong jx_auto.log
"""

# ------------------------------------------------------------- 2. TK_Pha: ghi log doi pha
CU_PHA = """	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	ea.nTKPhase = nPha;
	ea.nTKStep = 0;
	ea.nTKTry = 0;
"""
MOI_PHA = """	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	AUTOLOG("[TK-PHA] %d -> %d map=%d t=%u", ea.nTKPhase, nPha, SubWorld[0].m_SubWorldID, uCurTime);	// (06/09)
	ea.nTKPhase = nPha;
	ea.nTKStep = 0;
	ea.nTKTry = 0;
"""

# ------------------------------------------------------------- 3. LD_Msg: ghi log
CU_LDMSG = """static void LD_Msg(int nPlayerIdx, const char* szMsg)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	UINT uNow = timeGetTime();
	if (ea.uLDMsgT > uNow)
		return;
	ea.uLDMsgT = uNow + 1200;
"""
MOI_LDMSG = """static void LD_Msg(int nPlayerIdx, const char* szMsg)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	UINT uNow = timeGetTime();
	if (ea.uLDMsgT > uNow)
		return;
	ea.uLDMsgT = uNow + 1200;
	AUTOLOG("[LD-MSG] %s", szMsg);	// (06/09) LD_DiThanh dung chung cho TK / CT ve thanh
"""

# ------------------------------------------------------------- 4. TKP_END: thanh khong co trong danh sach Xa Phu
CU_END = """			if (ea.nTKStep == 3)
			{
				ea.nTKStep = 4;
"""
MOI_END = """			// (06/09) Xa Phu map bao danh KHONG liet ke thanh da chon. Voi Phuong Tuong (muc
			// mac dinh cua o 'Het tran ve') thi LUON the: tong_kim_tcap/xaphu.lua dat
			// CurStation = 1 (Phuong Tuong), GetCurStation() khong doi vi map 324 khong co
			// trong STATION_ARRAY, ma station.lua chi liet ke tram co gia > 0 tu tram hien tai
			// (StationPrice.txt: Phuong Tuong -> Phuong Tuong = -1). Duong cu 'Tro lai cho
			// luc nay' dua ve map luyen cong cu - thuong khong Xa Phu, khong phu -> LD_DiThanh
			// bo tay, nguoi choi dung o map cu ("ve khong dung thanh"). Nay: di sang MOT THANH
			// KHAC co trong danh sach (7 thanh noi nhau qua Xa Phu); TKP_VETHANH se nho Xa Phu
			// / Than Hanh Phu o thanh do dua tiep ve dung thanh da chon. Khong con thanh nao
			// trong danh sach thi moi roi map theo duong cu.
			if (ea.nTKStep == 3)
			{
				for (int i = 0; i < LD_VE_COUNT; ++i)
				{
					if (i == nVe)
						continue;
					const char* szKhac = DT_SapTownMenu((int)g_LDVeMap[i]);
					if (!szKhac || (nOpt = DT_FindAns(apAns, nAns, szKhac)) < 0)
						continue;
					DT_Answer(nPlayerIdx, nOpt);
					ea.nTKStep = 4;
					TK_Msg(nPlayerIdx, "<color=Yellow>@@Xa Phu điểm báo danh không liệt kê thành đã chọn - đi qua thành khác rồi nhờ Xa Phu ở đó về tiếp.@@");
					AUTOLOG("[TK-VE] Xa Phu 324 khong co thanh %d (map %d) - di qua thanh %d (map %d) truoc", nVe, (int)g_LDVeMap[nVe], i, (int)g_LDVeMap[i]);
					ea.uTKNext = uCurTime + 2000;
					return 1;
				}
			}
			if (ea.nTKStep == 3)
			{
				ea.nTKStep = 4;
"""


def va_coreshell():
    t = Tep(os.path.join(CORE, "CoreShell.cpp"))
    t.thay(CU_MSG, MOI_MSG, '[TK-MSG] %s')
    t.thay(CU_PHA, MOI_PHA, '[TK-PHA] %d -> %d')
    t.thay(CU_LDMSG, MOI_LDMSG, '[LD-MSG] %s')
    t.thay(CU_END, MOI_END, '[TK-VE] Xa Phu 324 khong co thanh')
    t.ghi()


def main():
    print("== goi_va_tk_vethanh_0609 %s ==" % ("(THU - khong ghi)" if THU else ""))
    va_coreshell()
    if LOI:
        print("\nLOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    print("XONG")


if __name__ == "__main__":
    main()
