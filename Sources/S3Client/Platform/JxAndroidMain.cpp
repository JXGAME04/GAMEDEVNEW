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
	const char* pszDir = getenv("JX_DATA_DIR");
#ifdef __ANDROID__
	if (!pszDir || !*pszDir) pszDir = SDL_GetAndroidExternalStoragePath();
	if (!pszDir || !*pszDir) pszDir = SDL_GetAndroidInternalStoragePath();
#endif
	if (!pszDir || !*pszDir) pszDir = ".";
	JxPosix_SetDataDir(pszDir);
	if (chdir(pszDir) != 0)
		JxAndroidLog("[ANDROID] chdir(%s) that bai: %s", pszDir, strerror(errno));
	JxAndroidLog("[ANDROID] thu muc du lieu: %s (SDL %d.%d.%d)", pszDir, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	if (jx_access("config.ini", 0) != 0)
	{
		char sz[1200];
		snprintf(sz, sizeof(sz), "Khong thay config.ini trong thu muc du lieu:\n%s\n\nHay chep du lieu game (ten tep ha chu thuong) vao day roi mo lai.", pszDir);
		JxAndroidLog("[ANDROID] %s", sz);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JX1 Mobile", sz, NULL);
		return 1;
	}
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
	int nRet = JxPosixMain(argc, argv);
	JxAndroidLog("[ANDROID] JxPosixMain tra ve %d", nRet);
	return nRet;
}
#endif // JX_POSIX
