//---------------------------------------------------------------------------
// [IOS 11/09] Diem vao tren iOS. Doi xung voi Sources/S3Client/Platform/JxAndroidMain.cpp:
// SDL nap chuong trinh roi goi SDL_main (= ham main() o day). Chon thu muc du lieu, chdir vao do,
// roi goi JxPosixMain() (S3Client.cpp:263, nhanh JX_POSIX) = phan WinMain khong dinh Windows.
//
// Thu muc du lieu, thu theo thu tu, lay thu muc dau tien co config.ini:
//   1. bien moi truong JX_DATA_DIR (dat duoc trong so do chay cua Xcode khi go loi)
//   2. dong dau tien cua <Documents>/jx_data_dir.txt
//   3. <Documents> cua ung dung  - bo tai (ios/JxTaiDuLieu.mm) ghi vao day; dia chi kho du lieu
//      doc tu dong dau <Documents>/may_chu_tai.txt
//   4. <Library/Application Support>/jx1
//   5. thu muc tai nguyen trong goi ung dung (chi doc, dung khi nhet san du lieu vao goi de thu)
//
// Phan hoi he dieu hanh nam o JxIosDuongDan.mm (khong the chung tep - xem ghi chu trong do).
//---------------------------------------------------------------------------
#include "KWin32.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>	// realpath
#include <stdarg.h>
#include <errno.h>

extern int JxPosixMain(int argc, char* argv[]);   // KHONG extern "C" (S3Client.cpp:263)

extern "C" const char* JxIos_ThuMucTaiLieu(char* pszRa, size_t nRa);
extern "C" const char* JxIos_ThuMucHoTro(char* pszRa, size_t nRa);
extern "C" const char* JxIos_ThuMucGoi(char* pszRa, size_t nRa);
// [IOS-TAI 11/09] buoc F: bo tai du lieu trong app (ios/JxTaiDuLieu.mm)
extern "C" int JxTaiDuLieu_Chay(const char* pszThuMuc, const char* pszGoc, char* pszLoi, int nLoi);
// [BAOMAT 12/09 PHIENBAN] kho du lieu khai so phien ban client toi thieu trong phienban.txt
extern "C" int JxTaiDuLieu_KiemPhienBan(const char* pszThuMuc, int nPhienBanApp, char* pszLoi, int nLoi);
#define JX_PHIEN_BAN_APP  20260912   // tang moi lan phat hanh (dang ngay)
extern "C" int JxIosAnGame_Co(void);   // [IOS-AN 11/09] nut an game co day duoc xuong nen khong

// [IOS-KYHIEU 11/09] cac ham ma tren Windows/Android nam trong DLL/.so rieng; tren iOS chung link tinh
// vao cung mot nhi phan nen chi can khai bao roi lay dia chi. Voi ham lien ket kieu C thi trinh lien ket
// chi doi dung TEN, nen khai bao rut gon la du.
struct iRepresentShell;
struct ITextFilter;
extern "C" iRepresentShell* CreateRepresentShell();
extern "C" int             Rep3_NapTruoc2(const char*, int);   // Represent3/KRepresentShell3.cpp:579 (Core goi qua GetProcAddress)                 // Represent3/KRepresentShell3.cpp:552
extern "C" HRESULT          CreateInterface(const GUID&, void**);   // Rainbow/ClientStage.cpp:23 (STDAPI)
extern "C" HRESULT          CreateTextFilter(ITextFilter**);        // FilterText/FilterText.cpp:218
// Ghi chu: Rep3_JxEpTrinhChieu nam trong "#ifdef JX_ANDROID" (D3D9onGPUDev.cpp:1731) nen ban iOS
// KHONG co ham do; noi goi no trong KSdlApp cung rao JX_ANDROID nen khong can dang ky.

static void JxIosLog(const char* fmt, ...)
{
	char sz[1024]; va_list va; va_start(va, fmt); vsnprintf(sz, sizeof(sz), fmt, va); va_end(va);
	SDL_Log("%s", sz);
	FILE* f = jx_fopen("jx_ios.log", "ab");
	if (f) { fprintf(f, "[%u] %s\n", (unsigned)SDL_GetTicks(), sz); fclose(f); }
}

int main(int argc, char* argv[])
{
	char szTaiLieu[1024] = "", szHoTro[1024] = "", szGoi[1024] = "";
	JxIos_ThuMucTaiLieu(szTaiLieu, sizeof(szTaiLieu));
	JxIos_ThuMucHoTro(szHoTro, sizeof(szHoTro));
	JxIos_ThuMucGoi(szGoi, sizeof(szGoi));

	char szTxt[1024] = "";
	if (szTaiLieu[0])
	{
		char szP[1200]; snprintf(szP, sizeof(szP), "%s/jx_data_dir.txt", szTaiLieu);
		FILE* f = fopen(szP, "rb");
		if (f)
		{
			if (fgets(szTxt, sizeof(szTxt), f))
			{
				size_t n = strlen(szTxt);
				while (n && (szTxt[n - 1] == '\n' || szTxt[n - 1] == '\r' || szTxt[n - 1] == ' ')) szTxt[--n] = 0;
			}
			fclose(f);
		}
	}

	const char* cand[8]; int nc = 0;
	const char* pszEnv = getenv("JX_DATA_DIR");
	if (pszEnv && *pszEnv) cand[nc++] = pszEnv;
	if (szTxt[0])          cand[nc++] = szTxt;
	if (szTaiLieu[0])      cand[nc++] = szTaiLieu;
	if (szHoTro[0])        cand[nc++] = szHoTro;
	if (szGoi[0])          cand[nc++] = szGoi;

	const char* pszDir = NULL;
	char szKiem[4096] = "";
	for (int i = 0; i < nc && !pszDir; i++)
	{
		char szC[1200]; snprintf(szC, sizeof(szC), "%s/config.ini", cand[i]);
		if (access(szC, R_OK) == 0)
			pszDir = cand[i];
		else
		{
			size_t k = strlen(szKiem);
			snprintf(szKiem + k, sizeof(szKiem) - k, "\n  %s", cand[i]);
		}
	}

	// [IOS-TAI 11/09] buoc F: neu <Documents>/may_chu_tai.txt co dia chi kho du lieu thi tai ve.
	// Chay CA KHI da co du lieu: manifest cho biet tep nao doi, chi tai phan chenh lech.
	// Khong noi duoc may chu ma da co du lieu san thi ghi log roi choi tiep (choi duoc khi khong mang).
	if (szTaiLieu[0])
	{
		char szMc[1200]; snprintf(szMc, sizeof(szMc), "%s/may_chu_tai.txt", szTaiLieu);
		char szGoc[512] = "";
		FILE* fm = fopen(szMc, "rb");
		if (fm)
		{
			if (fgets(szGoc, sizeof(szGoc), fm))
			{
				size_t n = strlen(szGoc);
				while (n && (szGoc[n-1] == '\n' || szGoc[n-1] == '\r' || szGoc[n-1] == ' ' || szGoc[n-1] == '\t')) szGoc[--n] = 0;
			}
			fclose(fm);
		}
		if (szGoc[0])
		{
			char szLoi[512] = "";
			JxIosLog("[IOS-TAI] kho du lieu: %s -> %s", szGoc, szTaiLieu);
			int nT = JxTaiDuLieu_Chay(szTaiLieu, szGoc, szLoi, sizeof(szLoi));
			JxIosLog("[IOS-TAI] ket qua %d%s%s", nT, szLoi[0] ? ": " : "", szLoi);
			if (nT == 0 && !pszDir)
			{	// vua tai xong: kiem lai Documents
				char szC[1200]; snprintf(szC, sizeof(szC), "%s/config.ini", szTaiLieu);
				if (access(szC, R_OK) == 0)
					pszDir = szTaiLieu;
			}
			if (nT != 0 && !pszDir)
			{
				JxPosix_SetDataDir(szTaiLieu); chdir(szTaiLieu);
				char sz[1024];
				snprintf(sz, sizeof(sz), "Tai du lieu that bai:\n\n%s", szLoi);
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JX1 Mobile", sz, NULL);
				return 1;
			}
		}
	}

	if (!pszDir)
	{
		const char* pszLog = szTaiLieu[0] ? szTaiLieu : ".";
		JxPosix_SetDataDir(pszLog); chdir(pszLog);
		char sz[4600];
		snprintf(sz, sizeof(sz),
			"Khong thay config.ini o cac thu muc:%s\n\nHay chep du lieu game (ten tep ha chu thuong) vao thu muc "
			"Documents cua ung dung, hoac ghi duong dan thu muc du lieu vao jx_data_dir.txt trong thu muc do.", szKiem);
		JxIosLog("[IOS] %s", sz);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JX1 Mobile", sz, NULL);
		return 1;
	}

	static char s_szDir[1024] = "";
	// [IOS-DUONGDAN 11/09] PHAI luu duong dan DA PHAN GIAI (realpath). Tren iOS, /var la lien ket
	// mem toi /private/var, nen g_GetFullPath (ghep tu thu muc hien hanh sau chdir) cho ra
	// "/private/var/...", con NSSearchPathForDirectoriesInDomains cho ra "/var/...".
	// Hai chuoi khac nhau -> JxPathPosix khong nhan ra day la duong DUOI thu muc du lieu ->
	// bo qua buoc ha chu thuong -> he tep iOS (phan biet hoa thuong) khong tim thay tep roi
	// co chu hoa trong ten (\Ui\UiToaDo_DanhSach.ini, \settings\npcres\ManLeftWeapon_effect.txt ...).
	if (!realpath(pszDir, s_szDir))
		strncpy(s_szDir, pszDir, sizeof(s_szDir) - 1);
	s_szDir[sizeof(s_szDir) - 1] = 0;
	JxPosix_SetDataDir(s_szDir);
	{	// [BAOMAT 12/09 PHIENBAN] kho du lieu doi ban moi hon -> dung han, khong vao game voi du lieu khong khop
		char szLoiPb[512] = "";
		if (JxTaiDuLieu_KiemPhienBan(s_szDir, JX_PHIEN_BAN_APP, szLoiPb, sizeof(szLoiPb)))
		{
			JxIosLog("[PHIENBAN] %s", szLoiPb);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "JX1 Mobile", szLoiPb, NULL);
			return 1;
		}
	}
	if (chdir(s_szDir) != 0)
		JxIosLog("[IOS] chdir(%s) that bai: %s", s_szDir, strerror(errno));
	JxIosLog("[IOS] thu muc du lieu: %s (SDL %d.%d.%d)", s_szDir, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	JxIosLog("[IOS-AN] nut an game: %s", JxIosAnGame_Co() ? "day han xuong nen duoc" : "KHONG, chi mo duoc dia chi ngoai");
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

	// [IOS-KYHIEU 11/09] iOS khong nap duoc thu vien dong: dang ky truoc cac ham ma ma chung van
	// tim bang LoadLibrary + GetProcAddress. Thieu buoc nay thi S3Client.cpp:389 khong tao duoc
	// tang ve -> MyApp.Init() that bai -> thoat ngay, man hinh den.
	JxPosix_DangKyKyHieu("Represent3.dll", "CreateRepresentShell", (void*)&CreateRepresentShell);
	JxPosix_DangKyKyHieu("Represent3.dll", "Rep3_NapTruoc2",       (void*)&Rep3_NapTruoc2);
	JxPosix_DangKyKyHieu("Rainbow.dll",    "CreateInterface",      (void*)&CreateInterface);
	JxPosix_DangKyKyHieu("FilterText.dll", "CreateTextFilter",     (void*)&CreateTextFilter);


	int nRet = JxPosixMain(argc, argv);
	JxIosLog("[IOS] JxPosixMain tra ve %d", nRet);
	return nRet;
}
