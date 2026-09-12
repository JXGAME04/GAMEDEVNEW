//---------------------------------------------------------------------------
// [MACOS 11/09] Diem vao tren macOS. Doi xung voi ios/JxIosMain.cpp nhung don gian hon:
// macOS la may tinh nen khong can UIKit, khong can vung chua rieng cua ung dung.
//
// Thu muc du lieu, theo thu tu, lay cai dau tien co config.ini:
//   1. tham so dong lenh thu nhat      ./jx1mac <thu muc du lieu>
//   2. bien moi truong JX_DATA_DIR
//   3. thu muc hien hanh
//   4. ~/Library/Application Support/jx1
//
// KHONG dinh nghia JX_MOBILE -> chay giao dien ban PC (chuot + ban phim), khong phai giao dien
// dien thoai. Dinh nghia JX_APPLE -> dung chung phan Metal / duong dan / bang tra ky hieu voi iOS.
//---------------------------------------------------------------------------
#include "KWin32.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

extern int JxPosixMain(int argc, char* argv[]);   // KHONG extern "C" (S3Client.cpp)

// cac ham ma ma chung van tim bang LoadLibrary + GetProcAddress (tren macOS cung link tinh mot nhi phan)
struct iRepresentShell;
struct ITextFilter;
extern "C" iRepresentShell* CreateRepresentShell();
extern "C" int              Rep3_NapTruoc2(const char*, int);
extern "C" HRESULT          CreateInterface(const GUID&, void**);
extern "C" HRESULT          CreateTextFilter(ITextFilter**);

static void JxMacLog(const char* fmt, ...)
{
	char sz[1024]; va_list va; va_start(va, fmt); vsnprintf(sz, sizeof(sz), fmt, va); va_end(va);
	SDL_Log("%s", sz);
	FILE* f = jx_fopen("jx_mac.log", "ab");
	if (f) { fprintf(f, "[%u] %s\n", (unsigned)SDL_GetTicks(), sz); fclose(f); }
}

int main(int argc, char* argv[])
{
	char szNha[1024] = "", szCwd[1024] = "";
	const char* pszNha = getenv("HOME");
	if (pszNha && *pszNha) snprintf(szNha, sizeof(szNha), "%s/Library/Application Support/jx1", pszNha);
	if (!getcwd(szCwd, sizeof(szCwd))) szCwd[0] = 0;

	const char* cand[6]; int nc = 0;
	if (argc > 1 && argv[1] && argv[1][0] != '-') cand[nc++] = argv[1];
	const char* pszEnv = getenv("JX_DATA_DIR");
	if (pszEnv && *pszEnv) cand[nc++] = pszEnv;
	if (szCwd[0])          cand[nc++] = szCwd;
	if (szNha[0])          cand[nc++] = szNha;

	const char* pszDir = NULL;
	char szKiem[3072] = "";
	for (int i = 0; i < nc && !pszDir; i++)
	{
		char szC[1200]; snprintf(szC, sizeof(szC), "%s/config.ini", cand[i]);
		if (access(szC, R_OK) == 0) pszDir = cand[i];
		else { size_t k = strlen(szKiem); snprintf(szKiem + k, sizeof(szKiem) - k, "\n  %s", cand[i]); }
	}
	if (!pszDir)
	{
		char sz[3600];
		snprintf(sz, sizeof(sz),
			"Khong thay config.ini o cac thu muc:%s\n\nChay lai kem duong dan thu muc du lieu:\n  ./jx1mac <thu muc>", szKiem);
		fprintf(stderr, "%s\n", sz);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JX1 macOS", sz, NULL);
		return 1;
	}

	static char s_szDir[1024] = "";
	if (!realpath(pszDir, s_szDir))		// [IOS-DUONGDAN] /var la lien ket mem -> phai phan giai
		strncpy(s_szDir, pszDir, sizeof(s_szDir) - 1);
	s_szDir[sizeof(s_szDir) - 1] = 0;
	JxPosix_SetDataDir(s_szDir);
	if (chdir(s_szDir) != 0)
		JxMacLog("[MACOS] chdir(%s) that bai: %s", s_szDir, strerror(errno));
	JxMacLog("[MACOS] thu muc du lieu: %s (SDL %d.%d.%d)", s_szDir, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);

	JxPosix_DangKyKyHieu("Represent3.dll", "CreateRepresentShell", (void*)&CreateRepresentShell);
	JxPosix_DangKyKyHieu("Represent3.dll", "Rep3_NapTruoc2",       (void*)&Rep3_NapTruoc2);
	JxPosix_DangKyKyHieu("Rainbow.dll",    "CreateInterface",      (void*)&CreateInterface);
	JxPosix_DangKyKyHieu("FilterText.dll", "CreateTextFilter",     (void*)&CreateTextFilter);

	int nRet = JxPosixMain(argc, argv);
	JxMacLog("[MACOS] JxPosixMain tra ve %d", nRet);
	return nRet;
}
