# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 9: DO PHAN GIAI CHUAN CHO DIEN THOAI + NHO MAT MA / TU DANG NHAP.
#
# Chay lai vo hai (moi va deu kiem "da co chua"). Doc/ghi latin-1, giu nguyen xuong dong CRLF
# -> KHONG lam hong byte TCVN3 nao. Ban Windows khong doi hanh vi: moi khoi moi deu trong #ifdef JX_ANDROID.
#
#   1. D3D9onGPUDev.cpp  ApplyWindowMode: tren Android KHONG goi SDL_SetWindowFullscreen(false).
#      Day chinh la thu lam khung ve bi co 6 % (Android tra lai thanh trang thai: 1040x604 -> 1040x568)
#      = muc 3.1 "con lam" cua BANGIAO_ANDROID_PHA4_0809.md.
#   2. KSdlApp.cpp       chinh sach do phan giai: khung ve = co man hinh / HE SO GIAO DIEN (giu dung ti le).
#   3. UiLogin.cpp       o "Ghi nho" tren dien thoai = nho CA mat ma (ban bam MD5, khong phai chu thuong).
#   4. Login.cpp         luu NGAY sau khi chon nhan vat (Android hay bi he thong giet, khong kip luu luc thoat).
#   5. UiInit.cpp/.h + UiShell.cpp  da nho du tai khoan+mat ma+nhan vat -> tu dang nhap tu man hinh chinh.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SUA = []

def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)

def nl(s, crlf):
    return s.replace("\n", "\r\n") if crlf else s

def va(duong, dau_da_co, cu, moi, ten):
    """Thay 'cu' bang 'moi'; bo qua neu 'dau_da_co' da co trong tep. Tu doi \\n -> \\r\\n theo tep."""
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
    SUA.append(ten)
    print("  va xong: %s" % ten)


# ---------------------------------------------------------------------------
# 1. Represent3: tren Android cua so LUON toan man hinh
# ---------------------------------------------------------------------------
print("1. Represent3 ApplyWindowMode (Android luon toan man hinh)")
va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", "toan man hinh (Android)",
"""void CDevGpu::ApplyWindowMode()
{
	const bool bFull = (m_pp.Windowed == FALSE);""",
"""void CDevGpu::ApplyWindowMode()
{
#ifdef JX_ANDROID
	// [ANDROID 09/09 DPG] Dien thoai KHONG co che do cua so: cua so da toan man hinh tu KSdlApp::Init.
	// Vi config.ini de FullScreen=0 nen ham nay tung goi SDL_SetWindowFullscreen(false), lam Android HIEN LAI
	// thanh trang thai ngay sau khung dau tien: cua so tut 1040x604 -> 1040x568, khung ve 604 bi ep xuong 568
	// (co 6 %, chu mo). Do la muc 3.1 "con lam" cua BANGIAO_ANDROID_PHA4_0809.md.
	// (Ghi chep pha 4 con canh: goi ham nay voi false sau khi be mat da co con co the lam Activity tao lai.)
	int pwA = 0, phA = 0; SDL_GetWindowSizeInPixels(m_pWin, &pwA, &phA);
	RgLog("cua so: toan man hinh (Android), %dx%d px (backbuffer %ux%u)", pwA, phA, m_bbW, m_bbH);
	return;
#else
	const bool bFull = (m_pp.Windowed == FALSE);""", "ApplyWindowMode dau")

va("Sources/Represent/Represent3/D3D9onGPUDev.cpp", "bFull ? \"toan man hinh\" : \"cua so\", pw, ph, m_bbW, m_bbH);\n#endif",
"""	RgLog("cua so: %s, %dx%d px (backbuffer %ux%u)", bFull ? "toan man hinh" : "cua so", pw, ph, m_bbW, m_bbH);
}""",
"""	RgLog("cua so: %s, %dx%d px (backbuffer %ux%u)", bFull ? "toan man hinh" : "cua so", pw, ph, m_bbW, m_bbH);
#endif
}""", "ApplyWindowMode cuoi")


# ---------------------------------------------------------------------------
# 2. KSdlApp: chinh sach do phan giai theo man hinh + he so giao dien
# ---------------------------------------------------------------------------
print("2. KSdlApp chinh sach do phan giai")
KHOI2 = r'''#ifdef JX_ANDROID
// [ANDROID 09/09 DPG] DO PHAN GIAI CHUAN CHO DIEN THOAI.
//
// Giao dien JX1 la giao dien "diem anh co dinh": chu 12 px, nut ~100 px, thanh ky nang ~500 px.
// Nen co hai cach SAI va mot cach DUNG:
//   SAI 1 - ve co dinh 1024x768 roi keo cang len man hinh: chu to nhung MO va MEO (dien thoai 20:9,
//           khong phai 4:3). Day la trang thai truoc pha nay.
//   SAI 2 - ve dung 1:1 theo diem anh that (2400x1080): sac tuyet doi nhung chu chi cao ~1,5 mm,
//           khong ai doc noi tren man 6,5 inch.
//   DUNG  - ve o KHUNG NHO HON theo mot HE SO GIAO DIEN, GIU DUNG TI LE khung hinh cua may
//           (nen khong co vien den, khong meo), roi de Represent3 phong len man hinh.
//
// He so duoc chon sao cho chieu cao khung ve roi vao quanh CHIEU CAO MUC TIEU (mac dinh 640 px: giua
// 604 px da chay tot tren LDPlayer va 768 px goc cua game). Vi du:
//     1040x604  (LDPlayer)      -> he so 1,00 -> khung ve 1040x604  (ve 1:1, net tuyet doi)
//     2400x1080 (dien thoai)    -> he so 1,75 -> khung ve 1370x616  (chu to gap 1,75 lan so voi ve 1:1)
//     1920x1080                 -> he so 1,75 -> khung ve 1096x616
//     2048x1536 (may tinh bang) -> he so 2,50 -> khung ve  818x614
//
// Represent3 (CDevGpu::Letterbox) tu phong khung ve len swapchain va KSdlApp::SdlToLogical doi toa do cham
// nguoc lai -> khong cho nao khac trong game phai biet den chuyen nay.
//
// Goi TRE, ngay truoc khi tao thiet bi ve (KMyApp::GameInit): luc KSdlApp::Init vua tao cua so thi Android
// chua dan trang xong.
//
// config.ini:
//   [Resolution] TheoManHinh=1       ; 0 = tat han, dung Width/Height nhu ban PC
//                ChieuCaoMucTieu=640 ; nho hon = giao dien TO hon (mo hon); lon hon = net hon (nho hon)
//                HeSoGiaoDien=0      ; 0 = tu chon theo muc tieu; 100/125/150/175/200/250/300 = ep (he so x100)
extern "C" void JxSdl_ChotDoPhanGiaiTheoManHinh(void)
{
	SDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();
	if (!pWin)
		return;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	if (!GetPrivateProfileInt("Resolution", "TheoManHinh", 1, szCfg))
		return;
	// Doi co cua so DUNG YEN moi chot (Android tra co that sau khi dan trang xong: tai tho, thanh dieu huong...).
	// KHONG doi che do toan man hinh o day: tren Android doi che do = Activity bi tao lai -> game khoi dong vong lap.
	int nW = 0, nH = 0, nWTruoc = -1, nHTruoc = -1, nYen = 0;
	for (int nLan = 0; nLan < 150; nLan++)
	{
		SDL_PumpEvents();
		SDL_Delay(10);
		SDL_GetWindowSizeInPixels(pWin, &nW, &nH);
		if (nW == nWTruoc && nH == nHTruoc) { if (++nYen >= 25 && nLan >= 40) break; }
		else { nYen = 0; nWTruoc = nW; nHTruoc = nH; }
	}
	if (nW < 640 || nH < 360)
	{
		g_DebugLog("[DPG] cua so %dx%d qua nho, giu %dx%d cua config.ini", nW, nH, SCREEN_WIDTH, SCREEN_HEIGHT);
		return;
	}
	int nMucTieu = GetPrivateProfileInt("Resolution", "ChieuCaoMucTieu", 640, szCfg);
	if (nMucTieu < 400) nMucTieu = 400;
	if (nMucTieu > 1200) nMucTieu = 1200;
	int nHeSo = GetPrivateProfileInt("Resolution", "HeSoGiaoDien", 0, szCfg);	// x100
	if (nHeSo <= 0)
	{
		// Chi lay cac nac "chan" cho phep phong it rang cua nhat: 1,00 1,25 1,50 1,75 2,00 2,50 3,00.
		// Hoa thi giu nac NHO hon (khung ve to hon = net hon) vi vong lap chi doi khi lech NHO HON HAN.
		static const int aNac[] = { 100, 125, 150, 175, 200, 250, 300 };
		int nChon = 100, nLechTotNhat = -1;
		for (int i = 0; i < (int)(sizeof(aNac) / sizeof(aNac[0])); i++)
		{
			int nCao = nH * 100 / aNac[i];
			int nLech = (nCao > nMucTieu) ? (nCao - nMucTieu) : (nMucTieu - nCao);
			if (nLechTotNhat < 0 || nLech < nLechTotNhat) { nLechTotNhat = nLech; nChon = aNac[i]; }
		}
		nHeSo = nChon;
	}
	if (nHeSo < 100) nHeSo = 100;	// khong bao gio ve LON hon man hinh: ton bo nho ma khong net them
	if (nHeSo > 400) nHeSo = 400;
	// Giao dien JX1 can it nhat 800x480 moi bay du (thanh ky nang, tui do, cua so chat) -> ha he so cho du cho.
	while (nHeSo > 100 && (nW * 100 / nHeSo < 800 || nH * 100 / nHeSo < 480))
		nHeSo -= 25;
	int nVeW = (nW * 100 / nHeSo) & ~1;
	int nVeH = (nH * 100 / nHeSo) & ~1;
	{
		SDL_Rect rcCa = { 0, 0, 0, 0 };
		SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &rcCa);
		g_DebugLog("[DPG] man hinh %dx%d | cua so %dx%d px | cao muc tieu %d -> he so %d,%02d -> khung ve %dx%d",
			rcCa.w, rcCa.h, nW, nH, nMucTieu, nHeSo / 100, nHeSo % 100, nVeW, nVeH);
	}
	SCREEN_WIDTH = nVeW; SCREEN_HEIGHT = nVeH;
	SetEngineResolution(nVeW, nVeH);
	g_nDoPhanGiaiTheoManHinh = 1;	// S3Client.cpp: chan LoadResolutionFromConfig doc lai config.ini
}
#endif
'''
s = doc("Sources/S3Client/Platform/KSdlApp.cpp")
crlf = ("\r\n" in s)
if "[DPG]" in s:
    print("  bo qua (da co): chinh sach do phan giai")
else:
    D1 = s.find(nl("#ifdef JX_ANDROID\n// [ANDROID 08/09] Do phan giai game", crlf))
    D2 = s.find("BOOL KSdlApp::Init(HINSTANCE hInstance, char* AppName)")
    if D1 < 0 or D2 < 0 or D2 < D1:
        print("  !! KHONG TIM THAY khoi JxSdl_ChotDoPhanGiaiTheoManHinh")
        sys.exit(1)
    ghi("Sources/S3Client/Platform/KSdlApp.cpp", s[:D1] + nl(KHOI2, crlf) + s[D2:])
    SUA.append("KSdlApp chinh sach do phan giai")
    print("  va xong: KSdlApp chinh sach do phan giai")


# ---------------------------------------------------------------------------
# 3. UiLogin: tren Android o "Ghi nho" = nho ca mat ma
# ---------------------------------------------------------------------------
print("3. UiLogin: Android nho ca mat ma")
va("Sources/S3Client/Ui/UiCase/UiLogin.cpp", "ANDROID 09/09 LOGIN",
"""	if (m_pSelf)
	{
		g_LoginLogic.SetRememberAccountFlag(m_pSelf->m_RememberAccount.IsButtonChecked() != 0);""",
"""	if (m_pSelf)
	{
		bool bGhiNho = (m_pSelf->m_RememberAccount.IsButtonChecked() != 0);
		g_LoginLogic.SetRememberAccountFlag(bGhiNho);
#ifdef JX_ANDROID
		// [ANDROID 09/09 LOGIN] Tren dien thoai o "Ghi nho" = nho CA mat ma, nhu moi game mobile: bat go lai
		// mat ma bang ban phim mem moi lan vao game la cuc hinh. Cai luu xuong dia KHONG phai mat ma chu thuong
		// ma la ban bam MD5 (KUiLogin::OnLogin da bam truoc khi goi AccountLogin), lai duoc EDOneTimePad_Encipher.
		// Ban PC khong doi: o do "Ghi nho" van chi nho tai khoan (nho ca mat ma phai Alt+A + AutoLogin=6323).
		if (bGhiNho)
			g_LoginLogic.SetRememberAllFlag(true);
#endif""", "UiLogin ghi nho mat ma")


# ---------------------------------------------------------------------------
# 4. Login.cpp: luu ngay sau khi chon nhan vat (Android)
# ---------------------------------------------------------------------------
print("4. Login.cpp: luu lua chon ngay sau khi chon nhan vat")
va("Sources/S3Client/Login/Login.cpp", "ANDROID 09/09 LOGIN",
"""		strcpy(m_Choices.szProcessingRoleName, NetCommand.szRoleName);
		m_Status = LL_S_WAIT_TO_LOGIN_GAMESERVER;
		m_Result = LL_R_NOTHING;
		nRet = true;""",
"""		strcpy(m_Choices.szProcessingRoleName, NetCommand.szRoleName);
		m_Status = LL_S_WAIT_TO_LOGIN_GAMESERVER;
		m_Result = LL_R_NOTHING;
		nRet = true;
#ifdef JX_ANDROID
		// [ANDROID 09/09 LOGIN] Luu NGAY chu khong doi luc thoat (UiShell.cpp UiExit): tren dien thoai nguoi choi
		// vuot tat app hoac he thong giet tien trinh nen SaveLoginChoice luc thoat rat hay khong duoc chay, the la
		// lan sau van phai go lai tu dau. Day cung la luc dau tien du CA BA thu de tu dang nhap:
		// tai khoan + mat ma (da bam MD5) + ten nhan vat.
		SaveLoginChoice();
#endif""", "Login luu ngay")


# ---------------------------------------------------------------------------
# 5. Tu dang nhap tren Android (goi tu nhip UiHeartBeat, KHONG goi tu ham cua cua so)
# ---------------------------------------------------------------------------
print("5. Tu dang nhap tren Android")
va("Sources/S3Client/Ui/UiCase/UiInit.cpp", "KDebug.h",
"""#include "UiNotice.h\"""",
"""#include "UiNotice.h"
#ifdef JX_ANDROID
#include "KDebug.h"		// [ANDROID 09/09 LOGIN] g_DebugLog cho JxUi_TuDangNhapAndroid()
#endif""", "UiInit include KDebug")

va("Sources/S3Client/Ui/UiCase/UiInit.cpp", "JxUi_TuDangNhapAndroid",
"""void KUiInit::OnAutoLogin()
{""",
"""#ifdef JX_ANDROID
// [ANDROID 09/09 LOGIN] TU DANG NHAP TREN DIEN THOAI.
// Ban PC: chi chay khi bam Alt+A VA Ui\\Setting.ini [Main] AutoLogin=6323 (loi tat cua nguoi lam game).
// Dien thoai: khong co phim Alt, va nguoi choi da chu dong bam "Ghi nho" o man dang nhap roi -> cu nho du
// tai khoan + mat ma + nhan vat thi vao thang, khong bat go lai bang ban phim mem.
// Muon tat: bo dau "Ghi nho" o man dang nhap, hoac dat config.ini [Login] TuDongDangNhap=0.
//
// Goi tu UiHeartBeat (nhip cua vong lap game), KHONG goi tu ShowCompleted/WndProc cua chinh KUiInit:
// o day duoc phep dong cua so dang hien va mo cua so khac, giong het KMyApp::ExtAutoLogin cua WAuto.
// CHI THU MOT LAN moi lan chay: dang xuat ve man hinh chinh thi khong tu dang nhap lai (tranh vong lap).
void JxUi_TuDangNhapAndroid()
{
	static bool s_bDaThu = false;
	if (s_bDaThu)
		return;
	if (!KUiInit::GetIfVisible())	// chi lam khi man hinh chinh dang hien (da nap xong giao dien)
		return;
	s_bDaThu = true;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\\\Config.ini");
	if (!GetPrivateProfileInt("Login", "TuDongDangNhap", 1, szCfg))
		return;
	g_LoginLogic.LoadLoginChoice();
	if (!g_LoginLogic.IsAutoLoginEnable())
	{
		g_DebugLog("[LOGIN] chua nho du tai khoan/mat ma/nhan vat -> vao man hinh chinh nhu thuong");
		return;
	}
	g_DebugLog("[LOGIN] da nho du dang nhap -> vao thang game");
	KUiInit::CloseWindow();
	KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_IN_GAME);
	g_LoginLogic.AutoLogin();
}
#endif

void KUiInit::OnAutoLogin()
{""", "UiInit ham tu dang nhap")

va("Sources/S3Client/Ui/UiCase/UiInit.h", "JxUi_TuDangNhapAndroid",
"""#endif // __UIINIT_H__""",
"""#ifdef JX_ANDROID
void JxUi_TuDangNhapAndroid();	// [ANDROID 09/09 LOGIN] UiInit.cpp - goi moi nhip tu UiHeartBeat
#endif

#endif // __UIINIT_H__""", "UiInit.h khai bao")

va("Sources/S3Client/Ui/UiShell.cpp", "JxUi_TuDangNhapAndroid",
"""		if (g_bDisconnect == false)
		{
			IR_UpdateTime();
			Wnd_Heartbeat();
		}""",
"""		if (g_bDisconnect == false)
		{
			IR_UpdateTime();
			Wnd_Heartbeat();
#ifdef JX_ANDROID
			JxUi_TuDangNhapAndroid();	// [ANDROID 09/09 LOGIN] da nho dang nhap thi vao thang, khong dung o man hinh chinh
#endif
		}""", "UiShell goi tu dang nhap")

print("")
print("XONG. Da va %d cho: %s" % (len(SUA), ", ".join(SUA) if SUA else "(khong co gi moi)"))
