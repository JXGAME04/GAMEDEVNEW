//---------------------------------------------------------------------------
// [ANDROID 08/09] Diem vao tren Android: SDLActivity (Java) nap libSDL3.so + libmain.so roi goi SDL_main
// (= ham main() o day, SDL_main.h doi ten). Chon thu muc du lieu, chdir vao do, roi goi JxPosixMain()
// (S3Client.cpp, nhanh JX_POSIX) = phan WinMain khong dinh Windows: LoadResolutionFromConfig -> MyApp.Init -> MyApp.Run.
// Thu muc du lieu (theo thu tu): bien moi truong JX_DATA_DIR -> bo nho ngoai rieng cua app
// (/storage/emulated/0/Android/data/<goi>/files) -> bo nho trong. Du lieu game (9,6 GB) chep vao do, TEN TEP/THU MUC
// HA CHU THUONG (lop tuong thich ha chu thuong moi duong dan tuong doi - xem JxPathPosix).
// Chi bien dich khi JX_POSIX (CMake Android).
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "KWin32.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef __ANDROID__
#include <android/log.h>
#endif

extern int JxPosixMain(int argc, char* argv[]);

static void JxAndroidLog(const char* fmt, ...)
{
	char sz[1024]; va_list va; va_start(va, fmt); vsnprintf(sz, sizeof(sz), fmt, va); va_end(va);
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "JX1", "%s", sz);
#else
	fprintf(stderr, "%s\n", sz);
#endif
	FILE* f = jx_fopen("jx_android.log", "ab");
	if (f) { fprintf(f, "[%u] %s\n", (unsigned)SDL_GetTicks(), sz); fclose(f); }
}

int main(int argc, char* argv[])
{
	// Thu muc du lieu: thu theo thu tu, lay thu muc dau tien co config.ini:
	//  1. bien moi truong JX_DATA_DIR (meta-data SDL_ENV.JX_DATA_DIR trong AndroidManifest)
	//  2. dong dau tien cua <thu muc app>/jx_data_dir.txt (dat bang adb: adb shell "echo /mnt/shared/Misc > .../files/jx_data_dir.txt")
	//  3. thu muc rieng cua app: /storage/emulated/0/Android/data/vn.jx1.mobile/files
	//  4. thu muc chia se cua may ao (LDPlayer: /mnt/shared/Misc|Pictures ↔ thu muc Windows), /sdcard/jx1, /sdcard/Download/jx1
	static char s_szDir[1024] = "";
	const char* pszApp = NULL;
#ifdef __ANDROID__
	pszApp = SDL_GetAndroidExternalStoragePath();
	if (!pszApp || !*pszApp) pszApp = SDL_GetAndroidInternalStoragePath();
#endif
	const char* pszEnv = getenv("JX_DATA_DIR");
	char szTxt[1024] = "";
	if (pszApp && *pszApp)
	{
		char szP[1200]; snprintf(szP, sizeof(szP), "%s/jx_data_dir.txt", pszApp);
		FILE* f = fopen(szP, "rb");
		if (f) { if (fgets(szTxt, sizeof(szTxt), f)) { size_t n = strlen(szTxt); while (n && (szTxt[n - 1] == '\n' || szTxt[n - 1] == '\r' || szTxt[n - 1] == ' ')) szTxt[--n] = 0; } fclose(f); }
	}
	const char* cand[16]; int nc = 0;
	if (pszEnv && *pszEnv) cand[nc++] = pszEnv;
	if (szTxt[0]) cand[nc++] = szTxt;
	if (pszApp && *pszApp) cand[nc++] = pszApp;
	cand[nc++] = "/mnt/shared/Misc/jx1"; cand[nc++] = "/mnt/shared/Misc"; cand[nc++] = "/mnt/shared/Pictures/jx1"; cand[nc++] = "/mnt/shared/Applications/jx1";
	cand[nc++] = "/sdcard/jx1"; cand[nc++] = "/sdcard/Download/jx1"; cand[nc++] = "/storage/emulated/0/jx1";
	const char* pszDir = NULL;
	char szKiem[4096] = "";
	for (int i = 0; i < nc && !pszDir; i++)
	{
		char szC[1200]; snprintf(szC, sizeof(szC), "%s/config.ini", cand[i]);
		if (access(szC, R_OK) == 0) pszDir = cand[i];
		else { size_t k = strlen(szKiem); snprintf(szKiem + k, sizeof(szKiem) - k, "\n  %s", cand[i]); }
	}
	if (!pszDir)
	{
		const char* pszLog = (pszApp && *pszApp) ? pszApp : ".";
		JxPosix_SetDataDir(pszLog); chdir(pszLog);
		char sz[4600];
		snprintf(sz, sizeof(sz), "Khong thay config.ini o cac thu muc:%s\n\nHay chep du lieu game (ten tep ha chu thuong) vao thu muc app, hoac ghi duong dan thu muc du lieu vao jx_data_dir.txt trong thu muc app.", szKiem);
		JxAndroidLog("[ANDROID] %s", sz);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JX1 Mobile", sz, NULL);
		return 1;
	}
	strncpy(s_szDir, pszDir, sizeof(s_szDir) - 1);
	JxPosix_SetDataDir(s_szDir);
	if (chdir(s_szDir) != 0)
		JxAndroidLog("[ANDROID] chdir(%s) that bai: %s", s_szDir, strerror(errno));
	JxAndroidLog("[ANDROID] thu muc du lieu: %s (SDL %d.%d.%d)", s_szDir, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
	int nRet = JxPosixMain(argc, argv);
	JxAndroidLog("[ANDROID] JxPosixMain tra ve %d", nRet);
	return nRet;
}
#endif // JX_POSIX
