//---------------------------------------------------------------------------
// [IOS 11/09] Diem vao tren iOS. Doi xung voi Sources/S3Client/Platform/JxAndroidMain.cpp:
// SDL nap chuong trinh roi goi SDL_main (= ham main() o day). Chon thu muc du lieu, chdir vao do,
// roi goi JxPosixMain() (S3Client.cpp:263, nhanh JX_POSIX) = phan WinMain khong dinh Windows.
//
// Thu muc du lieu, thu theo thu tu, lay thu muc dau tien co config.ini:
//   1. bien moi truong JX_DATA_DIR (dat duoc trong so do chay cua Xcode khi go loi)
//   2. dong dau tien cua <Documents>/jx_data_dir.txt
//   3. <Documents> cua ung dung  - cho bo tai se ghi vao (buoc F)
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
#include <stdarg.h>
#include <errno.h>

extern int JxPosixMain(int argc, char* argv[]);   // KHONG extern "C" (S3Client.cpp:263)

extern "C" const char* JxIos_ThuMucTaiLieu(char* pszRa, size_t nRa);
extern "C" const char* JxIos_ThuMucHoTro(char* pszRa, size_t nRa);
extern "C" const char* JxIos_ThuMucGoi(char* pszRa, size_t nRa);

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
	strncpy(s_szDir, pszDir, sizeof(s_szDir) - 1);
	JxPosix_SetDataDir(s_szDir);
	if (chdir(s_szDir) != 0)
		JxIosLog("[IOS] chdir(%s) that bai: %s", s_szDir, strerror(errno));
	JxIosLog("[IOS] thu muc du lieu: %s (SDL %d.%d.%d)", s_szDir, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

	int nRet = JxPosixMain(argc, argv);
	JxIosLog("[IOS] JxPosixMain tra ve %d", nRet);
	return nRet;
}
