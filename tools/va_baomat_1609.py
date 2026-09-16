# -*- coding: utf-8 -*-
r"""[BAOMAT 16/09] Va 5 lo bao mat may chu trong BAOMAT_TRUOC_PHAT_HANH_1409.md (nhom A) - chay lai duoc (idempotent).

  A1  KGMCommand.cpp        lenh "?gm ..." qua chat chay Lua tuy y: them kiem quyen theo \GmTaiKhoan.ini [GM] TaiKhoan=
                            (dong cua an toan: khong co tep = khong ai chay duoc) + ghi log moi lan thu; chan tran bo dem
                            ten lenh 20 byte va bo dem tham so RSF/RLS.
  A2  S3PAccount.cpp        chen SQL o dang nhap / dang nhap may chu / doi mat khau: tu choi chuoi co ' hoac \ hoac ky tu
                            dieu khien hoac khong ket thuc trong bo dem goi tin (API DB chi co QuerySql(chuoi)).
  A3  KProtocolProcess.cpp  mua sap: kiem bien chi so, cam tu mua, mon phai o hanh trang nguoi ban gia > 0, o dich chi la
                            hanh trang, dat do xong moi tra tien.
  A4  KProtocolProcess.cpp + KItemList.cpp  gia am: chan o dat gia (goi tin + ham) va o duong mua (gia <= 0).
  A5  KProtocolProcess.cpp  gui tien ruong: chi nhan 1..INT_MAX (DWORD >= 2^31 hoa am -> doi chieu chuyen).

Moi sua trong Core deu nam trong vung _SERVER (hoac macro theo _SERVER) -> CoreClient.dll khong doi byte.
Tep TCVN3/CRLF: doc-ghi latin-1, chi them ASCII, kiem so byte cao truoc/sau.
Dung:  python tools\va_baomat_1609.py [--thu]      (chay o goc worktree)
"""
import io
import os
import re
import sys

TAG = "[BAOMAT"
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
THU = "--thu" in sys.argv


def doc(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def ghi(p, s):
    if THU:
        print("  (thu) khong ghi %s" % p)
        return
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def cao(s):
    return sum(1 for ch in s if ord(ch) >= 0x80)


def crlf(s, co_crlf):
    # @T@ = dong trong chi co mot dau tab trong nguon (cong cu soan thao hay cat khoang trang cuoi dong)
    s = s.replace("@T@", "\t")
    return s.replace("\n", "\r\n") if co_crlf else s


def thay(s, cu, moi, ten, so=1):
    # tep co the tron CRLF/LF (checkout theo autocrlf): thu neo dang CRLF truoc, roi LF; thay bang cung kieu xuong dong
    for cu2, moi2 in ((crlf(cu, True), crlf(moi, True)), (cu, moi)):
        n = s.count(cu2)
        if n == so:
            return s.replace(cu2, moi2)
    raise SystemExit("!! %s: neo gap %d lan (can %d):\n%s" % (ten, s.count(cu), so, cu[:200]))


def va_tep(rel, cac_sua):
    p = os.path.join(ROOT, rel)
    s0 = doc(p)
    if TAG in s0:
        print("  da va, bo qua: %s" % rel)
        return
    s = s0
    for ten, cu, moi in cac_sua:
        s = thay(s, cu, moi, ten)
    if cao(s) != cao(s0):
        raise SystemExit("!! %s: so byte cao doi (%d -> %d)" % (rel, cao(s0), cao(s)))
    for ch in set(c for c in s if ord(c) >= 0x80):
        if s0.count(ch) != s.count(ch):
            raise SystemExit("!! %s: byte cao %02X doi so lan" % (rel, ord(ch)))
    ghi(p, s)
    print("  da va: %s (%d cho)" % (rel, len(cac_sua)))


# ---------------------------------------------------------------- A1: KGMCommand.cpp
GM_HELPER = '''#define GM_CTRL_CMD_PREFIX '?'

#ifdef _SERVER
#include "KNpc.h"
// [BAOMAT A1 16/09] Lenh "?gm ..." qua kenh chat truoc day KHONG kiem quyen: nguoi choi nao cung chay duoc Lua tuy y
// tren may chu (ds/dw/RSF/RLS/RLAS -> mat may chu chu khong chi mat do). Nay chi tai khoan co ten trong tep
// \\GmTaiKhoan.ini (canh GameServer.exe) moi duoc chay:
//   [GM]
//   TaiKhoan=gm1,gm2      (ten TAI KHOAN dang nhap, khong phai ten nhan vat; cach nhau dau phay; khong phan biet hoa/thuong)
// Khong co tep / danh sach rong = KHONG AI chay duoc (dong cua an toan). Moi lan thu (cho hay chan) deu ghi log may chu.
// Tep doc lai moi lan co lenh ?gm (hiem) nen sua danh sach khong can mo lai may chu.
static BOOL JxGmDuocPhep(int nPlayerIdx, const char* pText, int nLen)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_dwID <= 0)
		return FALSE;
	const char* szTk = Player[nPlayerIdx].GetPlayerAccount();
	const char* szNv = "";
	if (Player[nPlayerIdx].m_nIndex > 0 && Player[nPlayerIdx].m_nIndex < MAX_NPC)
		szNv = Npc[Player[nPlayerIdx].m_nIndex].Name;
	char szDs[1024];
	szDs[0] = 0;
	KIniFile ini;
	if (ini.Load("\\\\GmTaiKhoan.ini"))
		ini.GetString("GM", "TaiKhoan", "", szDs, sizeof(szDs));
	BOOL bOk = FALSE;
	if (szTk && szTk[0] && szDs[0])
	{
		int nTk = (int)strlen(szTk);
		const char* p = szDs;
		while (*p && !bOk)
		{
			while (*p == ',' || *p == ';' || *p == ' ' || *p == '\\t' || *p == '\\r' || *p == '\\n')
				p++;
			const char* q = p;
			while (*q && *q != ',' && *q != ';' && *q != ' ' && *q != '\\t' && *q != '\\r' && *q != '\\n')
				q++;
			if (q > p && (int)(q - p) == nTk && _strnicmp(p, szTk, nTk) == 0)
				bOk = TRUE;
			p = q;
		}
	}
	if (nLen < 0) nLen = 0;
	if (nLen > 200) nLen = 200;
	// printf = cua so console GameServer (luon co); g_DebugLog chi toi DebugWin.exe khi dang mo.
	printf("[BAOMAT-GM] %s: tai khoan '%s' nhan vat '%s' lenh '%.*s'\\n",
		bOk ? "CHO CHAY" : "CHAN", szTk ? szTk : "", szNv, nLen, pText);
	g_DebugLog((LPSTR)"[BAOMAT-GM] %s: tai khoan '%s' nhan vat '%s' lenh '%.*s'",
		bOk ? "CHO CHAY" : "CHAN", szTk ? szTk : "", szNv, nLen, pText);
	return bOk;
}
#define JX_GM_DEM_TEP	300	// [BAOMAT A1 16/09] moi token co the dai toi nLen (< 300): bo dem cu 200/100/100 tran
#define JX_GM_DEM_HAM	300
#define JX_GM_DEM_THAM	300
#else
#define JX_GM_DEM_TEP	200
#define JX_GM_DEM_HAM	100
#define JX_GM_DEM_THAM	100
#endif
'''

A1 = [
    ("A1 helper", "#define GM_CTRL_CMD_PREFIX '?'\n", GM_HELPER),
    ("A1 kiem quyen",
     '''			{
				bHandled = (bool)(0 != TextMsgProcessGMCmd(nPlayerIdx, pText + GMCMD_GENRE_PREFIX_LEN + 1,
''',
     '''			{
#ifdef _SERVER
				if (!JxGmDuocPhep(nPlayerIdx, pText, nLen))	// [BAOMAT A1 16/09] khong phai GM: nuot cau chat, khong chay
					return TRUE;
#endif
				bHandled = (bool)(0 != TextMsgProcessGMCmd(nPlayerIdx, pText + GMCMD_GENRE_PREFIX_LEN + 1,
'''),
    ("A1 ten lenh 20 byte",
     '''	if (NULL == pStart)
	{
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nLen] = 0;
	}
	else
	{	nTempLen = pStart - pGMCmd;
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nTempLen] = 0;
	}
''',
     '''#ifdef _SERVER
	// [BAOMAT A1 16/09] ten lenh dai hon bo dem 20 byte -> truoc day tran ngan xep; strstr cung co the chay qua nLen
	// (cau chat khong ket thuc bang NUL) nen chi nhan dau cach nam trong nLen.
	if (NULL != pStart && (int)(pStart - pGMCmd) < nLen)
		nTempLen = (int)(pStart - pGMCmd);
	if (nTempLen <= 0 || nTempLen >= (int)sizeof(szCmd))
		return FALSE;
	memcpy(szCmd, pGMCmd, nTempLen);
	szCmd[nTempLen] = 0;
#else
	if (NULL == pStart)
	{
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nLen] = 0;
	}
	else
	{	nTempLen = pStart - pGMCmd;
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nTempLen] = 0;
	}
#endif
'''),
    ("A1 RSF bo dem",
     '''			char szScriptFile[200];
			char szScriptFun[100];
			char szScriptParam[100];
			int nBufLen = GetNextUnit(pParam, ' ', nLen, szScriptFile);
''',
     '''#ifdef _SERVER
			if (nLen <= 0 || nLen >= 300)	// [BAOMAT A1 16/09] nhu ds/dw: tham so phai < 300 de vua bo dem
				return FALSE;
#endif
			char szScriptFile[JX_GM_DEM_TEP];
			char szScriptFun[JX_GM_DEM_HAM];
			char szScriptParam[JX_GM_DEM_THAM];
			int nBufLen = GetNextUnit(pParam, ' ', nLen, szScriptFile);
'''),
    ("A1 RLS bo dem",
     '''			char szScriptFile[200];
			GetNextUnit(pParam, ' ', nLen, szScriptFile);
''',
     '''#ifdef _SERVER
			if (nLen <= 0 || nLen >= 300)	// [BAOMAT A1 16/09]
				return FALSE;
#endif
			char szScriptFile[JX_GM_DEM_TEP];
			GetNextUnit(pParam, ' ', nLen, szScriptFile);
'''),
]

# ---------------------------------------------------------------- A3/A4/A5: KProtocolProcess.cpp
A345 = [
    ("A3/A4 mua sap",
     '''	PLAYER_TRADE_BUY_ITEM_COMMAND* pPlayer = (PLAYER_TRADE_BUY_ITEM_COMMAND *)pProtocol;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_PlayerId);
@T@
	if (nPlayerIdx <= 0 || !Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)
		return;
	int nPrice = Player[nPlayerIdx].m_ItemList.GetPrice(pPlayer->m_Idx);
	if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice || nPrice == 0)
		return;
	int nIdx = ItemSet.AddI(&Item[pPlayer->m_Idx]);
	if (nIdx > 0 && nIdx < MAX_ITEM)
		Item[nIdx].m_CommonAttrib.uPrice = 0;	// [DUNGLUYEN-PB 01/09] ban sao nguoi mua khong mang gia sap nguoi ban (AddKIL chep uPrice -> nPrice: tu len sap nguoi mua)
@T@
	Player[nIndex].m_ItemList.AddKIL(nIdx,pPlayer->m_Place,pPlayer->m_X,pPlayer->m_Y);
@T@
	Player[nIndex].Pay(nPrice); //nguoi mua
''',
     '''	PLAYER_TRADE_BUY_ITEM_COMMAND* pPlayer = (PLAYER_TRADE_BUY_ITEM_COMMAND *)pProtocol;
	// [BAOMAT A3/A4 16/09] Truoc day: chi so mon do may nguoi choi gui len dung thang cho Item[] (khong kiem bien),
	// khong chan tu mua sap minh (nhan doi do, chi mat thue), khong kiem mon co dang bay ban, o dich do nguoi mua chon
	// (ke ca o trang bi), gia am/0 lot, va tru tien ngay ca khi dat do that bai. Nay: kiem bien, cam tu mua, mon phai
	// dang o hanh trang nguoi ban voi gia > 0, o dich chi la hanh trang, dat do xong moi tra tien.
	if (pPlayer->m_Idx <= 0 || pPlayer->m_Idx >= MAX_ITEM)
		return;
	if (pPlayer->m_PlayerId == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_PlayerId);
@T@
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || nPlayerIdx == nIndex || !Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)
		return;
	int nJxMuc = Player[nPlayerIdx].m_ItemList.FindSame(pPlayer->m_Idx);
	if (nJxMuc <= 0 || Player[nPlayerIdx].m_ItemList.m_Items[nJxMuc].nPlace != pos_equiproom)
		return;
	int nPrice = Player[nPlayerIdx].m_ItemList.m_Items[nJxMuc].nPrice;
	if (nPrice <= 0 || Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice)
		return;
	if (pPlayer->m_Place != pos_equiproom)
		return;
	int nIdx = ItemSet.AddI(&Item[pPlayer->m_Idx]);
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return;
	Item[nIdx].m_CommonAttrib.uPrice = 0;	// [DUNGLUYEN-PB 01/09] ban sao nguoi mua khong mang gia sap nguoi ban (AddKIL chep uPrice -> nPrice: tu len sap nguoi mua)
@T@
	if (!Player[nIndex].m_ItemList.AddKIL(nIdx,pPlayer->m_Place,pPlayer->m_X,pPlayer->m_Y))
	{
		ItemSet.Remove(nIdx);	// [BAOMAT A3 16/09] o dich bi chiem / ngoai luoi: huy ban sao, khong tru tien, sap giu nguyen
		return;
	}
@T@
	Player[nIndex].Pay(nPrice); //nguoi mua
'''),
    ("A4 dat gia",
     '''	PLAYER_SET_PRICE *pSP=(PLAYER_SET_PRICE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
''',
     '''	PLAYER_SET_PRICE *pSP=(PLAYER_SET_PRICE *)pProtocol;
	if (pSP->m_Price < 0)	// [BAOMAT A4 16/09] gia am tu may nguoi choi: truoc day luu thang -> mua gan nhu mien phi
		return;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
'''),
]
A5_RE = re.compile(r"(STORE_MONEY_COMMAND\*\s+pCommand = \(STORE_MONEY_COMMAND \*\)pProtocol;\r?\n\r?\n)")
A5_MOI = '''	// [BAOMAT A5 16/09] So tien tu may nguoi choi la DWORD, ExchangeMoney nhan int: >= 2^31 hoa am -> doi chieu chuyen
	// (rut ruong khi chua mo khoa, cong tien phong nguon). Chi nhan 1..INT_MAX; so du nguon do AddMoney(-n) kiem.
	if (pCommand->m_dwMoney == 0 || pCommand->m_dwMoney > (DWORD)0x7FFFFFFF)
		return;
'''

# ---------------------------------------------------------------- A4: KItemList.cpp
A4_ITEMLIST = [
    ("A4 SetPrice",
     '''void KItemList::SetPrice( int nIdx, int nPrice)
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (nPrice)
''',
     '''void KItemList::SetPrice( int nIdx, int nPrice)
{
	if (nPrice < 0)	// [BAOMAT A4 16/09] khong nhan gia am (goi tin dat gia da chan, day la lop thu hai)
		return;
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (nGameId <= 0 || nId <= 0)	// [BAOMAT A4 16/09] mon khong co trong tui: truoc day ghi vao m_Items[0] / Item[0]
		return;
	if (nPrice)
'''),
]

# ---------------------------------------------------------------- A2: S3PAccount.cpp
A2_HELPER = '''DWORD GetGMID();

// [BAOMAT A2 16/09] Moi cau SQL trong tep nay ghep thang ten tai khoan / mat khau / ten may chu bang sprintf
// (chen SQL: go  ' or '1'='1  vao o tai khoan la dang nhap thanh nguoi khac, nang hon la doc / xoa ca bang).
// API DB chi co QuerySql(chuoi), khong co tham so hoa, nen chan o dau vao: tu choi chuoi khong ket thuc trong bo dem
// goi tin, chua dau nhay don, gach cheo nguoc hay ky tu dieu khien. Khong co ' va \\ thi khong cach nao dong chuoi SQL.
static bool S3P_ChuoiSqlAnToan(const char* s, int nMax)
{
	if (!s)
		return false;
	int n = 0;
	for (; n < nMax && s[n]; ++n)
	{
		unsigned char c = (unsigned char)s[n];
		if (c == '\\'' || c == '\\\\' || c < 0x20)
			return false;
	}
	return n < nMax;	// phai gap NUL trong bo dem
}
#define S3P_KIEM_TK(tk, loi)	if (!S3P_ChuoiSqlAnToan((tk), LOGIN_USER_ACCOUNT_MAX_LEN)) return (loi)
#define S3P_KIEM_MK(mk, loi)	if (!S3P_ChuoiSqlAnToan((mk), LOGIN_USER_PASSWORD_MAX_LEN)) return (loi)
'''
PCONN_BLOCK = '''	if (NULL == pConn)
	{
		return iRet;
	}
'''
# (ham, guard) - guard chen ngay sau khoi kiem pConn cua ham do
A2_HAM = [
    ("Login", "\tS3P_KIEM_TK(strAccName, E_ACCOUNT_OR_PASSWORD);\t// [BAOMAT A2 16/09]\n\tS3P_KIEM_MK(strPassword, E_ACCOUNT_OR_PASSWORD);\n"),
    ("LoginGame", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("Logout", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("ElapseTime", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("QueryTime", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("ServerLogin", "\tS3P_KIEM_TK(strAccName, E_ACCOUNT_OR_PASSWORD);\t// [BAOMAT A2 16/09]\n\tS3P_KIEM_MK(strPassword, E_ACCOUNT_OR_PASSWORD);\n"),
    ("GetServerID", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("GetAccountGameID", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("GetLockAccount", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("GetLeftSecondsOfDeposit", "\tS3P_KIEM_TK(strAccName, ACTION_FAILED);\t// [BAOMAT A2 16/09]\n"),
    ("VerifyUserModifyPassword", "\tS3P_KIEM_TK(strAccName, E_ACCOUNT_OR_PASSWORD);\t// [BAOMAT A2 16/09]\n\tS3P_KIEM_MK(strPassword, E_ACCOUNT_OR_PASSWORD);\n"),
]


def va_s3paccount():
    rel = "Sources/Sword3PaySys/S3AccServer/S3PAccount.cpp"
    p = os.path.join(ROOT, rel)
    s0 = doc(p)
    if TAG in s0:
        print("  da va, bo qua: %s" % rel)
        return
    co_crlf = "\r\n" in s0
    s = thay(s0, crlf("DWORD GetGMID();\n", co_crlf), crlf(A2_HELPER, co_crlf), "A2 helper")
    khoi = crlf(PCONN_BLOCK, co_crlf)
    for ham, guard in A2_HAM:
        sig = "int S3PAccount::%s(" % ham
        i = s.find(sig)
        if i < 0:
            raise SystemExit("!! A2: khong thay ham " + ham)
        j = s.find(khoi, i)
        k = s.find("int S3PAccount::", i + len(sig))
        if j < 0 or (k >= 0 and j > k):
            raise SystemExit("!! A2: khoi kiem pConn khong nam trong ham " + ham)
        s = s[:j + len(khoi)] + crlf(guard, co_crlf) + s[j + len(khoi):]
    if cao(s) != cao(s0):
        raise SystemExit("!! %s: so byte cao doi" % rel)
    ghi(p, s)
    print("  da va: %s (helper + %d ham)" % (rel, len(A2_HAM)))


def va_kprotocol():
    rel = "Sources/Core/Src/KProtocolProcess.cpp"
    p = os.path.join(ROOT, rel)
    s0 = doc(p)
    if TAG in s0:
        print("  da va, bo qua: %s" % rel)
        return
    co_crlf = "\r\n" in s0
    s = s0
    for ten, cu, moi in A345:
        s = thay(s, crlf(cu, co_crlf), crlf(moi, co_crlf), ten)
    m = list(A5_RE.finditer(s))
    if len(m) != 1:
        raise SystemExit("!! A5: neo STORE_MONEY_COMMAND gap %d lan" % len(m))
    e = m[0].end()
    s = s[:e] + crlf(A5_MOI, co_crlf) + s[e:]
    if cao(s) != cao(s0):
        raise SystemExit("!! %s: so byte cao doi" % rel)
    ghi(p, s)
    print("  da va: %s (A3/A4 mua sap, A4 dat gia, A5 gui tien)" % rel)


def main():
    print("[BAOMAT 16/09] goc: %s%s" % (ROOT, " (chi thu)" if THU else ""))
    va_tep("Sources/Core/Src/KGMCommand.cpp", A1)
    va_kprotocol()
    va_tep("Sources/Core/Src/KItemList.cpp", A4_ITEMLIST)
    va_s3paccount()
    print("xong.")


if __name__ == "__main__":
    main()
