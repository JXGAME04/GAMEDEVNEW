//---------------------------------------------------------------------------
// [ANDROID 08/09] Stub cho cac module S3Client chi Windows bi loai khoi ban Android nhung van duoc goi tu ma chung:
//  - JxReplay.cpp (plugin ghi/phat lai tran qua DLL)  -> khong co, tra "chua nap"
//  - Ui/PerfHud.cpp (dem hieu nang qua psapi/pdh)      -> tat
//  - Ui/TrayMode.cpp (bieu tuong khay he thong)         -> khong lam gi
// Chi bien dich khi JX_POSIX (CMake Android).
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "KWin32.h"
#include "../JxReplay.h"
#include "../Ui/PerfHud.h"
#include "../Ui/TrayMode.h"

// ---- JxReplay ----
bool       JxReplay_Init() { return false; }
void       JxReplay_Exit() {}
IJXReplay* JxReplay_Get() { return NULL; }
int        JxReplay_GetState() { return -1; }
bool       JxReplay_IsPlaying() { return false; }
bool       JxReplay_IsRecording() { return false; }
void       JxReplay_Breathe() {}
void       JxReplay_OnGameFrame() {}
void       JxReplay_DoVerb(const char* pszVerb) { (void)pszVerb; }
bool       JxReplay_OpenFileAndPlay() { return false; }
bool       JxReplay_OnQuitRequest() { return false; }

// ---- PerfHud ---- [ANDROID 11/09 HUD] Android co bang do rieng (Platform/JxPerfHudAndroid.cpp); stub chi cho POSIX khac
#ifdef JX_IOS	// [IOS-HUD 11/09] iOS co ban do rieng o ios/JxIosHud.cpp -> khong lam stub o day
#else
#ifndef JX_ANDROID
void PerfHud_SetEnable(int nOn) { (void)nOn; }
int  PerfHud_IsEnable() { return 0; }
void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing) { (void)nPaintFps; (void)nLogicFps; (void)dwPing; }
#endif
#endif

// ---- TrayMode ----
TrayMode gTrayMode;
void TrayMode::HideNotify(HINSTANCE hInst) { (void)hInst; }
void TrayMode::ShowNotify() {}
#endif // JX_POSIX
