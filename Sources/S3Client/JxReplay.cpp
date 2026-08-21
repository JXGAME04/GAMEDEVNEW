//---------------------------------------------------------------------------
// File     : JxReplay.cpp
// Muc dich : Tang dieu phoi he GHI / PHAT LAI ban dien .jxr phia client.
//
// Dung lai theo dung ban tham chieu (D:\ServerLinux\Patch\game_y.exe):
//   - nap JXReplay.dll trong luong khoi tao Represent      (0x0056D4A0 / 0x0056D615)
//   - gan hai chieu shell <-> replay                       (0x0056D630 / 0x0056D653)
//   - hoi trang thai MOI VONG BOM                          (0x0056E3BD)
//   - day (nTime, nStatus) xuong shell MOI KHUNG           (0x0056E307)
//   - 8 dong tu cua ham Lua Replay()                       (0x0040FF30)
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KWin32Wnd.h"		// g_GetMainHWnd
#include <commdlg.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include "JxReplay.h"
#include "JxrDrawAdapter.h"
#include "../Represent/iRepresent/iRepresentShell.h"
#include "../Represent/iRepresent/iJxReplay.h"
#include "../Core/Src/CoreShell.h"
#include "../Core/Src/GameDataDef.h"
#include "Ui/UiBase.h"
#include "Ui/UiCase/UiSysMsgCentre.h"
#include "KIniFile.h"

extern struct iRepresentShell*	g_pRepresentShell;
extern struct iCoreShell*		g_pCoreShell;

//--- Khoa chuoi trong ui\Setting.ini muc [InfoString] --------------------
// 23 / 43 da co san trong ban phat hanh (tieu de game / "O dia da day...").
// 51..54 la khoa moi cua he replay, them kem trong dot nay.
// LUU Y: 45/46 DA BI DUNG cho thong bao gioi han dang nhap - dung nham se in sai.
#define JXR_STR_TITLE			"23"	// tieu de hop thoai
#define JXR_STR_DISK_FULL		"43"	// O dia da day khong the luu them!
#define JXR_STR_REC_START		"51"	// Hien dang quay phim
#define JXR_STR_REC_PAUSE		"52"	// Tam dung ghi hinh
#define JXR_STR_SAVE_REPFILE	"53"	// File anh da duoc luu tai
#define JXR_STR_JXREPLAY_FILE	"54"	// Ghi hinh anh!

static HMODULE		l_hJxReplayModule	= NULL;
static IJXReplay*	l_pJxReplay			= NULL;
static int			l_nReplayState		= JXR_STATUS_IDLE;

// Dong ho replay: bo dem khung chay o JXR_REPLAY_FPS tick/giay.
static DWORD		l_dwClockBase		= 0;
static __int64		l_nReplayFrame		= 0;

//===========================================================================
// Tien ich
//===========================================================================

// Lay thu muc chua Game.exe, ket thuc bang dau '\'.
static void JxrLog(const char* pszFmt, ...)
{
	char szPath[MAX_PATH];
	szPath[0] = 0;
	GetModuleFileNameA(NULL, szPath, sizeof(szPath));
	char* p = strrchr(szPath, 92);
	if (p)
		*(p + 1) = 0;
	strncat(szPath, "jx_replay.log", sizeof(szPath) - strlen(szPath) - 1);

	FILE* fp = fopen(szPath, "a");
	if (fp == NULL)
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);
	fprintf(fp, "[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);

	va_list ap;
	va_start(ap, pszFmt);
	vfprintf(fp, pszFmt, ap);
	va_end(ap);

	fprintf(fp, "\n");
	fclose(fp);
}

static void JxrGetExeDir(char* pszBuf, int nSize)
{
	pszBuf[0] = 0;
	GetModuleFileNameA(NULL, pszBuf, nSize);
	char* p = strrchr(pszBuf, '\\');
	if (p)
		*(p + 1) = 0;
	else
		pszBuf[0] = 0;
}

// Bao dam mot thu muc con canh Game.exe ton tai. Tra ve duong dan day du.
static void JxrEnsureSubDir(const char* pszSub, char* pszOut, int nSize)
{
	char szDir[MAX_PATH];
	JxrGetExeDir(szDir, sizeof(szDir));
	_snprintf(pszOut, nSize, "%s%s", szDir, pszSub);
	pszOut[nSize - 1] = 0;

	// CreateDirectory chi tao duoc MOT cap, nen tao lan luot tung cap.
	char szTmp[MAX_PATH];
	strncpy(szTmp, pszOut, sizeof(szTmp));
	szTmp[sizeof(szTmp) - 1] = 0;
	for (char* q = szTmp + strlen(szDir); *q; q++)
	{
		if (*q == '\\')
		{
			*q = 0;
			CreateDirectory(szTmp, NULL);
			*q = '\\';
		}
	}
}

// Doc mot chuoi trong ui\Setting.ini muc [InfoString].
static bool JxrGetStr(const char* pszKey, char* pszBuf, int nSize)
{
	pszBuf[0] = 0;
	KIniFile* pIni = g_UiBase.GetCommConfigFile();
	if (pIni == NULL)
		return false;
	pIni->GetString("InfoString", pszKey, "", pszBuf, nSize);
	return pszBuf[0] != 0;
}

// In mot dong thong bao he thong cho nguoi choi.
static void JxrSysMsg(const char* pszKey, const char* pszExtra)
{
	char szMsg[128];
	if (!JxrGetStr(pszKey, szMsg, sizeof(szMsg) - 1))
		return;

	KSystemMessage Msg;
	Msg.byConfirmType	= SMCT_NONE;
	Msg.byParamSize		= 0;
	Msg.byPriority		= 0;
	Msg.eType			= SMT_NORMAL;
	Msg.uReservedForUi	= 0;
	if (pszExtra && pszExtra[0])
		_snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s %s", szMsg, pszExtra);
	else
		_snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", szMsg);
	Msg.szMessage[sizeof(Msg.szMessage) - 1] = 0;
	KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
}

// Kiem o dia chua Game.exe con it nhat JXR_MIN_FREE_BYTES byte trong.
static bool JxrHasEnoughDisk()
{
	char szRoot[MAX_PATH];
	szRoot[0] = 0;
	GetModuleFileNameA(NULL, szRoot, sizeof(szRoot));
	// Ban tham chieu cat cung ve goc o dia "X:\" (szRoot[3] = 0).
	if (strlen(szRoot) < 3)
		return false;
	szRoot[3] = 0;

	ULARGE_INTEGER uAvail, uTotal, uFree;
	if (!GetDiskFreeSpaceEx(szRoot, &uAvail, &uTotal, &uFree))
		return false;
	return uFree.QuadPart >= (unsigned __int64)JXR_MIN_FREE_BYTES;
}

//===========================================================================
// Vong doi
//===========================================================================

// FIX 21/08 (jx_crash.log 20/08 19:45:38): game SAP NGAY O MAN HINH DAU voi
// 0xC0000005. Ba khung dau cua ngan xep nam BEN TRONG chinh DLL dang duoc nap
// (0x0DBF3D51 / 0x0DD27AF6 / 0x0DBFF062 - khong co ten module, EBX = 0x0DBF0000
// la dia chi nap cua no), roi moi den LdrLoadDll -> LoadLibraryA -> JxReplay_Init.
// Tuc DllMain cua JXReplay.dll no ra loi ==> LoadLibrary KHONG BAO GIO TRA VE,
// cua chan "l_hJxReplayModule == NULL" ben duoi hoan toan vo dung. Chi can chep
// nham hoac hong mot tep JXReplay.dll la ca game chet truoc khi vao duoc man dang nhap.
// Boc SEH: DLL hong thi chi TAT rieng tinh nang quay phim, game van chay.
// (CL_Vectored o CrashLog.cpp:370 tra 0 = EXCEPTION_CONTINUE_SEARCH nen jx_crash.log
// VAN duoc ghi day du truoc khi __except nuot loi - khong mat dau vet de truy.)
static HMODULE JxrSafeLoadLibrary()
{
	HMODULE hMod = NULL;
	__try
	{
		hMod = LoadLibrary(JXREPLAY_MODULE_NAME);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		hMod = NULL;
		JxrLog("LOI NGHIEM TRONG: %s no ngay trong DllMain (ma loi 0x%08X)."
			   " Da TAT tinh nang replay, game van chay tiep binh thuong.",
			   JXREPLAY_MODULE_NAME, (unsigned int)GetExceptionCode());
	}
	return hMod;
}

bool JxReplay_Init()
{
	if (l_pJxReplay)
		return true;
	if (g_pRepresentShell == NULL)
		return false;

	l_hJxReplayModule = JxrSafeLoadLibrary();
	if (l_hJxReplayModule == NULL)
	{
		JxrLog("LOI: LoadLibrary(%s) that bai, GetLastError=%lu"
			   "  (126 = khong thay tep hoac thieu DLL phu thuoc;"
			   " 1114 = ham khoi tao cua DLL phu thuoc bi loi;"
			   " 14001 = thieu CRT VC80 8.0.50727.6195)",
			   JXREPLAY_MODULE_NAME, GetLastError());
		return false;
	}

	fnCreateJxReplayInterface pCreate = (fnCreateJxReplayInterface)GetProcAddress(
			l_hJxReplayModule, JXREPLAY_CREATE_FUN_NAME);
	if (pCreate == NULL)
	{
		JxrLog("LOI: khong thay ham %s trong %s (chep nham DLL?)",
			   JXREPLAY_CREATE_FUN_NAME, JXREPLAY_MODULE_NAME);
		FreeLibrary(l_hJxReplayModule);
		l_hJxReplayModule = NULL;
		return false;
	}

	l_pJxReplay = pCreate();
	if (l_pJxReplay == NULL)
	{
		FreeLibrary(l_hJxReplayModule);
		l_hJxReplayModule = NULL;
		return false;
	}

	// Thu muc tep tam BAT BUOC ton tai cho CA chieu ghi LAN chieu phat.
	// Thieu no thi fopen hong va ca hai chieu that bai IM LANG.
	char szTmp[MAX_PATH];
	JxrEnsureSubDir(JXREPLAY_TEMP_SUBDIR, szTmp, sizeof(szTmp));

	// Gan hai chieu, dung thu tu ban tham chieu.
	g_pRepresentShell->SetJxReplay((void*)l_pJxReplay);
	l_pJxReplay->SetDrawInterface((void*)JxrGetDrawAdapter());

	// BAT CHE DO CHUP TRANG THAI BAN DAU - THIEU CAI NAY THI PHAT LAI DEN NEN.
	// jxreplay.dll co mot bo theo doi (REC vtable o 6, 0x10007700) chay MOI lan ve,
	// KE CA khi chua bam quay. No ghi nho cac anh da tao / da ve len anh, roi StartRec
	// xa toan bo vao dau tep. Nhung moi nhanh bi KEP boi mot co rieng:
	//    nType 1  CreateImage           kiem [K2DREC+0x5F4] <- SetParam(1, x)
	//    nType 12 DrawPrimitivesOnImage kiem [K2DREC+0x5F0] <- SetParam(12, x)
	// Mac dinh ca hai co deu = 0 nen bang chup RONG: khi phat lai, cac anh nen dat
	// ('_*PlaceGround*_#~N~#_') chua he duoc tao => GetImage tra NULL => chi con chu
	// va hinh hoc, nen den thui.
	// KHAC ban tham chieu CO CHU Y: ban goc KHONG goi SetParam o dau ca (da doi chieu
	// ca InitRepresent 0x0056D520 lan nhanh 'rec' 0x0040FFAD) - va do cung la ly do
	// CA HAI loi vao UI cua no deu bi chu thich: tinh nang von dang hong.
	// SetParam bat buoc status == 0, o day vua tao xong nen dung luc.
	l_pJxReplay->SetParam(JXR_PARAM_TRACE_CREATEIMAGE,     1);
	l_pJxReplay->SetParam(JXR_PARAM_TRACE_DRAWPRIMONIMAGE, 1);

	g_pRepresentShell->SetReplayTimeAndStatus(1, JXR_STATUS_IDLE);

	l_nReplayState	= JXR_STATUS_IDLE;
	l_dwClockBase	= 0;
	l_nReplayFrame	= 0;
	JxrLog("OK: da nap %s, giao dien=%p, thu muc tam=%s",
			   JXREPLAY_MODULE_NAME, l_pJxReplay, szTmp);
	return true;
}

void JxReplay_Exit()
{
	if (l_pJxReplay)
	{
		// Dang ghi ma thoat dot ngot se lam hong tep .jxr: dong lai cho tu te.
		int nState = l_pJxReplay->GetStatus();
		if (nState == JXR_STATUS_RECORDING || nState == JXR_STATUS_PAUSEREC)
			l_pJxReplay->EndRec(NULL);
		else if (nState == JXR_STATUS_PLAYING)
			l_pJxReplay->RequestStopPlay();

		if (g_pRepresentShell)
			g_pRepresentShell->SetJxReplay(NULL);
		l_pJxReplay->SetDrawInterface(NULL);
		l_pJxReplay->ReleaseInterface();
		l_pJxReplay = NULL;
	}
	if (l_hJxReplayModule)
	{
		FreeLibrary(l_hJxReplayModule);
		l_hJxReplayModule = NULL;
	}
	l_nReplayState = JXR_STATUS_IDLE;
}

//===========================================================================
// Truy van
//===========================================================================
IJXReplay*	JxReplay_Get()			{ return l_pJxReplay; }
int			JxReplay_GetState()		{ return l_pJxReplay ? l_nReplayState : -1; }
bool		JxReplay_IsPlaying()	{ return l_pJxReplay && l_nReplayState == JXR_STATUS_PLAYING; }
bool		JxReplay_IsRecording()
{
	return l_pJxReplay &&
		(l_nReplayState == JXR_STATUS_RECORDING || l_nReplayState == JXR_STATUS_PAUSEREC);
}

//===========================================================================
// Nhip
//===========================================================================
void JxReplay_Breathe()
{
	if (l_pJxReplay == NULL)
		return;
	// Ban tham chieu KHONG tu nuoi bien trang thai ma hoi lai DLL moi khung.
	l_nReplayState = l_pJxReplay->GetStatus();
}

void JxReplay_OnGameFrame()
{
	if (l_pJxReplay == NULL || g_pRepresentShell == NULL)
		return;

	DWORD dwNow = timeGetTime();
	if (l_dwClockBase == 0)
		l_dwClockBase = dwNow;

	// Chi tang bo dem khi dong ho thuc da bat kip - giong cong thuc
	// `frame * 1000 <= elapsed * FPS` cua ban tham chieu.
	__int64 nElapsed = (__int64)(dwNow - l_dwClockBase);
	if (l_nReplayFrame * 1000 <= nElapsed * JXR_REPLAY_FPS)
		l_nReplayFrame++;

	g_pRepresentShell->SetReplayTimeAndStatus((int)l_nReplayFrame, l_nReplayState);
}

//===========================================================================
// 8 dong tu cua ham Lua Replay()
//===========================================================================
static void JxrVerb_Rec()
{
	// Chi chay khi RANH hoac DANG-TAM-DUNG-GHI.
	if (l_nReplayState != JXR_STATUS_IDLE && l_nReplayState != JXR_STATUS_PAUSEREC)
		return;

	if (!JxrHasEnoughDisk())
	{
		char szMsg[128], szTitle[64];
		if (JxrGetStr(JXR_STR_DISK_FULL, szMsg, sizeof(szMsg)) &&
			JxrGetStr(JXR_STR_TITLE, szTitle, sizeof(szTitle)))
			MessageBox(g_GetMainHWnd(), szMsg, szTitle, MB_OK | MB_ICONWARNING);
		return;
	}

	// Bao dam thu muc tep tam con do (nguoi choi co the da xoa tay).
	char szTmp[MAX_PATH];
	JxrEnsureSubDir(JXREPLAY_TEMP_SUBDIR, szTmp, sizeof(szTmp));

	// Ten phien = ten nhan vat. Truong header chi 32 byte va DLL dung strcpy
	// KHONG gioi han, nen PHAI tu kep lai truoc khi truyen vao.
	char szName[32];
	szName[0] = 0;
	if (g_pCoreShell)
	{
		KUiPlayerBaseInfo Info;
		memset(&Info, 0, sizeof(Info));
		g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Info, 0);
		strncpy(szName, Info.Name, sizeof(szName) - 1);
		szName[sizeof(szName) - 1] = 0;
	}
	if (szName[0] == 0)
		strcpy(szName, "Replay");

	l_pJxReplay->StartRec(szName, (unsigned int)time(NULL));

	// EP VE LAI TOAN BO NEN NGAY SAU KHI BAT DAU GHI.
	// Ly do: moi anh nen (_*PlaceGround*_#~N~#_) duoc GHEP tu NHIEU lenh
	// DrawPrimitivesOnImage (tung o dat + tung vat the, xem
	// KScenePlaceRegionC::PrerenderGround). Bo chup trang thai cua jxreplay.dll
	// chi giu duoc khoang MOT lenh cho moi anh, nen phat lai chi hien mot mang
	// cua moi vung - dung trieu chung "cho thay nen cho khong".
	// SetRepresentShell() goi thang KScenePlaceC::RepresentShellReset(), thu ha
	// GROUND_IMG_OK_FLAG cua MOI anh nen => khung ke tiep engine dung lai toan bo
	// nen, va lan nay moi lenh deu roi vao trong phien ghi nen duoc luu DAY DU.
	// Dung API san co, khong sua Core; chi ton mot lan ve lai ngay luc bam quay.
	if (g_pCoreShell && g_pRepresentShell)
		g_pCoreShell->SetRepresentShell(g_pRepresentShell);

	JxrSysMsg(JXR_STR_REC_START, NULL);
}

static void JxrVerb_EndRec()
{
	if (l_nReplayState != JXR_STATUS_RECORDING)
		return;

	// <thu muc Game.exe>\JxRep\<2|3>D_%y-%m-%d_%H-%M.jxr
	char szDir[MAX_PATH];
	JxrEnsureSubDir(JXREPLAY_SAVE_SUBDIR, szDir, sizeof(szDir));

	char szStamp[64];
	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	if (pTm == NULL)
		return;
	strftime(szStamp, sizeof(szStamp), "%y-%m-%d_%H-%M.jxr", pTm);

	char szPath[MAX_PATH];
	_snprintf(szPath, sizeof(szPath), "%s%dD_%s", szDir,
			(g_pRepresentShell && g_pRepresentShell->IsRep3D()) ? 3 : 2, szStamp);
	szPath[sizeof(szPath) - 1] = 0;

	l_pJxReplay->EndRec(szPath);
	JxrSysMsg(JXR_STR_SAVE_REPFILE, szPath);
}

static void JxrVerb_PauseRec()
{
	if (l_nReplayState != JXR_STATUS_RECORDING)
		return;
	l_pJxReplay->PauseRec();
	JxrSysMsg(JXR_STR_REC_PAUSE, NULL);
}

void JxReplay_DoVerb(const char* pszVerb)
{
	if (l_pJxReplay == NULL)
	{
		JxrLog("BO QUA dong tu '%s': chua nap duoc JXReplay.dll",
			   pszVerb ? pszVerb : "(null)");
		return;
	}
	if (pszVerb == NULL)
		return;
	JxrLog("dong tu '%s', trang thai hien tai = %d", pszVerb, l_nReplayState);

	if (strcmp(pszVerb, "rec") == 0)
	{
		JxrVerb_Rec();
	}
	else if (strcmp(pszVerb, "endrec") == 0)
	{
		JxrVerb_EndRec();
	}
	else if (strcmp(pszVerb, "pauserec") == 0)
	{
		JxrVerb_PauseRec();
	}
	else if (strcmp(pszVerb, "play") == 0)
	{
		// GIU NGUYEN theo ban tham chieu: nhanh "play" la MA CHET, khong lam gi.
		// Duong phat lai di qua hop thoai chon tep (JxReplay_OpenFileAndPlay).
	}
	else if (strcmp(pszVerb, "stop") == 0)
	{
		if (l_nReplayState == JXR_STATUS_PLAYING)
			l_pJxReplay->RequestStopPlay();
	}
	else if (strcmp(pszVerb, "pause") == 0)
	{
		if (l_nReplayState == JXR_STATUS_PLAYING)
			l_pJxReplay->Pause();
	}
	else if (strcmp(pszVerb, "speedup") == 0)
	{
		// CANH BAO: o vtable nay la HAM RONG (1 byte 0xC3) trong jxreplay.dll.
		// Giu lai cho dung ban tham chieu; goi khong co tac dung.
		if (l_nReplayState == JXR_STATUS_PLAYING)
			l_pJxReplay->SpeedUp();
	}
	else if (strcmp(pszVerb, "slowdown") == 0)
	{
		// CANH BAO: cung la ham rong trong jxreplay.dll.
		if (l_nReplayState == JXR_STATUS_PLAYING)
			l_pJxReplay->SlowDown();
	}
}

//===========================================================================
// Hop thoai chon tep .jxr roi phat
//===========================================================================
bool JxReplay_OpenFileAndPlay()
{
	if (l_pJxReplay == NULL)
		return false;
	if (l_nReplayState != JXR_STATUS_IDLE)
		return false;

	char szDir[MAX_PATH];
	JxrEnsureSubDir(JXREPLAY_SAVE_SUBDIR, szDir, sizeof(szDir));

	char szFile[MAX_PATH];
	szFile[0] = 0;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(ofn);
	ofn.hwndOwner		= g_GetMainHWnd();
	ofn.lpstrFilter		= "*.jxr\0*.jxr\0All\0*.*\0\0";
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= szFile;
	ofn.nMaxFile		= sizeof(szFile);
	ofn.lpstrInitialDir	= szDir;
	ofn.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (!GetOpenFileName(&ofn))
		return false;
	if (szFile[0] == 0)
		return false;

	if (!l_pJxReplay->Play(szFile))
		return false;

	JxrSysMsg(JXR_STR_JXREPLAY_FILE, NULL);
	return true;
}

//===========================================================================
// Nguoi choi bam thoat game
//===========================================================================
bool JxReplay_OnQuitRequest()
{
	if (l_pJxReplay == NULL)
		return false;

	if (l_nReplayState == JXR_STATUS_RECORDING || l_nReplayState == JXR_STATUS_PAUSEREC)
	{
		// Ban tham chieu NUOT lenh thoat va luu ban dien lai.
		JxrVerb_EndRec();
		return true;
	}
	if (l_nReplayState == JXR_STATUS_PLAYING)
	{
		// Ban tham chieu goi Replay([[rec]]) o day - gan nhu chac chan la LOI SAO CHEP
		// cua ban goc (nhanh rec chi chay o state 0/5 nen no khong lam gi ca).
		// O day dung "stop" cho dung y do: dung phat roi moi cho thoat.
		l_pJxReplay->RequestStopPlay();
		return true;
	}
	return false;
}
