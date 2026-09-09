# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 10: nho TEN NHAN VAT o TEP CHUNG de tu dang nhap duoc NGAY TU LUC MOI MO APP.
#
# Vi sao can: KLogin::IsAutoLoginEnable() doi du ca 4 thu - tai khoan, mat ma, TEN NHAN VAT, ten may chu.
# Ten nhan vat von duoc luu o "tep rieng" UserData\<ma>\UiConfig.ini, ma <ma> = bam(tai khoan)+bam(TEN NHAN VAT)
# (KUiBase::SetUserAccount). Tuc la muon MO tep rieng thi phai biet ten nhan vat truoc - ma ten nhan vat lai
# nam trong chinh tep do. Tren PC khong sao: loi tat Alt+A chi dung SAU khi da dang nhap mot lan trong cung
# lan chay (luc do ten nhan vat da nam san trong bo nho). Tren dien thoai ta can tu dang nhap ngay tu luc
# moi mo app -> phai chep them ten nhan vat vao tep CHUNG (UserData\UiCommon.ini), doc duoc ma khong can <ma>.
#
# Ban Windows khong doi mot byte: ca hai khoi deu trong #ifdef JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DUONG = "Sources/S3Client/Login/Login.cpp"


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


# --- doc: neu tep rieng khong cho ten nhan vat thi lay o tep chung -----------
va(DUONG, '"LastCharacter"',
"""			g_UiBase.ClosePrivateSettingFile(false);
		}

		g_UiBase.CloseCommSettingFile(false);""",
"""			g_UiBase.ClosePrivateSettingFile(false);
#ifdef JX_ANDROID
			// [ANDROID 09/09 LOGIN] Tep rieng (UserData\\<ma>\\UiConfig.ini) chi mo duoc khi DA biet ten nhan vat
			// (vi <ma> = bam(tai khoan) + bam(ten nhan vat)) nen luc moi mo app no luon rong. Lay ban sao ten
			// nhan vat da chep o tep chung -> du ca 4 thu cho IsAutoLoginEnable() ngay tu man hinh chinh.
			if (m_Choices.szProcessingRoleName[0] == 0 &&
				pSetting->GetString($LOGIN, "LastCharacter", "",
					m_Choices.szProcessingRoleName, sizeof(m_Choices.szProcessingRoleName)) &&
				m_Choices.szProcessingRoleName[0])
			{
				EDOneTimePad_Decipher(m_Choices.szProcessingRoleName, strlen(m_Choices.szProcessingRoleName));
			}
#endif
		}

		g_UiBase.CloseCommSettingFile(false);""", "LoadLoginChoice doc ten nhan vat o tep chung")

# --- ghi: chep ten nhan vat sang tep chung ----------------------------------
va(DUONG, "chep them ten nhan vat vao tep chung",
"""			KIniFile*	pPrivate = g_UiBase.GetPrivateSettingFile();
			if (pPrivate)
			{
				if (m_Choices.szProcessingRoleName[0])""",
"""#ifdef JX_ANDROID
			// [ANDROID 09/09 LOGIN] chep them ten nhan vat vao tep chung (xem giai thich o LoadLoginChoice).
			// Luu y: luc SelectRole goi SaveLoginChoice thi KUiBase::m_UserAccountId con RONG (no chi duoc dat
			// sau khi may chu tra loi vao game), nen doan ghi "tep rieng" ben duoi khong chay - ban sao nay
			// moi la cai thuc su giu duoc ten nhan vat qua cac lan mo app.
			if (m_Choices.szProcessingRoleName[0])
			{
				i = strlen(m_Choices.szProcessingRoleName);
				memcpy(szBuffer, m_Choices.szProcessingRoleName, i);
				szBuffer[i] = 0;
				EDOneTimePad_Encipher(szBuffer, i);
				pSetting->WriteString($LOGIN, "LastCharacter", szBuffer);
			}
#endif
			KIniFile*	pPrivate = g_UiBase.GetPrivateSettingFile();
			if (pPrivate)
			{
				if (m_Choices.szProcessingRoleName[0])""", "SaveLoginChoice ghi ten nhan vat sang tep chung")

print("XONG dot va 10.")
