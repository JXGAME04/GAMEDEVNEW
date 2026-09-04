// rep_harness.cpp - [REP3 03/09] bo thu doc lap: ve cung mot bo sprite bang Represent2.dll / Represent3.dll,
// chup anh de so, va do thoi gian ve. Chay trong thu muc co package.ini + data\*.pak + Engine.dll.
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include "KEngine.h"
#include "KPakList.h"
#include "KWin32Wnd.h"
#include "iRepresentShell.h"
#include "KRepresentUnit.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

// [RAM] bo nho rieng cua tien trinh (PrivateUsage) va working set, MB
// REP_OFFSCREEN=1: cua so dat ngoai man hinh, khong chiem foreground, khong chup BitBlt (chi do RAM / SaveScreenToFile)
// REP_OFFSCREEN=2: cua so tren man hinh, TOPMOST nhung KHONG chiem focus (SWP_NOACTIVATE), co chup BitBlt + SaveScreenToFile
static int RepMode() { const char* e = getenv("REP_OFFSCREEN"); return e ? atoi(e) : 0; }
#define REP_OFF (RepMode() == 1)
#define REP_NOACT (RepMode() != 0)

static void PrintMem(const char* label)
{
	PROCESS_MEMORY_COUNTERS_EX pmc; memset(&pmc, 0, sizeof(pmc)); pmc.cb = sizeof(pmc);
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	printf("[RAM] %-28s private %6.1f MB  ws %6.1f MB\n", label, pmc.PrivateUsage / 1048576.0, pmc.WorkingSetSize / 1048576.0);
}

static LRESULT CALLBACK HarnessWndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (m == WM_CLOSE) { PostQuitMessage(0); return 0; }
	return DefWindowProcA(h, m, w, l);
}
static void Pump()
{
	MSG msg;
	while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageA(&msg); }
}

// chup vung client cua cua so bang PrintWindow (co PW_RENDERFULLCONTENT de lay noi dung D3D)
static bool SaveClientBmp(HWND hwnd, const char* path)
{
	RECT rc; GetClientRect(hwnd, &rc);
	int w = rc.right - rc.left, h = rc.bottom - rc.top;
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	BITMAPINFO bi; memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h;
	bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
	void* pBits = NULL;
	HBITMAP hbm = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
	HGDIOBJ old = SelectObject(hdcMem, hbm);
	BOOL ok = PrintWindow(hwnd, hdcMem, PW_CLIENTONLY | 0x2 /*PW_RENDERFULLCONTENT*/);
	GdiFlush();
	bool bRet = false;
	if (ok && pBits)
	{
		FILE* f = fopen(path, "wb");
		if (f)
		{
			int rowBytes = ((w * 3 + 3) / 4) * 4;
			BITMAPFILEHEADER fh; memset(&fh, 0, sizeof(fh));
			BITMAPINFOHEADER ih; memset(&ih, 0, sizeof(ih));
			fh.bfType = 0x4D42; fh.bfOffBits = sizeof(fh) + sizeof(ih); fh.bfSize = fh.bfOffBits + rowBytes * h;
			ih.biSize = sizeof(ih); ih.biWidth = w; ih.biHeight = h; ih.biPlanes = 1; ih.biBitCount = 24; ih.biCompression = BI_RGB;
			fwrite(&fh, sizeof(fh), 1, f); fwrite(&ih, sizeof(ih), 1, f);
			std::vector<unsigned char> row(rowBytes, 0);
			for (int y = h - 1; y >= 0; y--)
			{
				unsigned char* src = (unsigned char*)pBits + y * w * 4;
				for (int x = 0; x < w; x++) { row[x*3] = src[x*4]; row[x*3+1] = src[x*4+1]; row[x*3+2] = src[x*4+2]; }
				fwrite(&row[0], rowBytes, 1, f);
			}
			fclose(f); bRet = true;
		}
	}
	SelectObject(hdcMem, old); DeleteObject(hbm); DeleteDC(hdcMem); ReleaseDC(NULL, hdcScreen);
	return bRet;
}

// chup vung client tu man hinh (BitBlt tu DC toan man hinh) - hop ca DirectDraw lan D3D khi cua so dang hien
static bool SaveScreenBlt(HWND hwnd, const char* path)
{
	RECT rc; GetClientRect(hwnd, &rc);
	POINT pt = {0, 0}; ClientToScreen(hwnd, &pt);
	int w = rc.right - rc.left, h = rc.bottom - rc.top;
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	BITMAPINFO bi; memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h;
	bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
	void* pBits = NULL;
	HBITMAP hbm = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
	HGDIOBJ old = SelectObject(hdcMem, hbm);
	BOOL ok = BitBlt(hdcMem, 0, 0, w, h, hdcScreen, pt.x, pt.y, SRCCOPY | CAPTUREBLT);
	GdiFlush();
	bool bRet = false;
	if (ok && pBits)
	{
		FILE* f = fopen(path, "wb");
		if (f)
		{
			int rowBytes = ((w * 3 + 3) / 4) * 4;
			BITMAPFILEHEADER fh; memset(&fh, 0, sizeof(fh));
			BITMAPINFOHEADER ih; memset(&ih, 0, sizeof(ih));
			fh.bfType = 0x4D42; fh.bfOffBits = sizeof(fh) + sizeof(ih); fh.bfSize = fh.bfOffBits + rowBytes * h;
			ih.biSize = sizeof(ih); ih.biWidth = w; ih.biHeight = h; ih.biPlanes = 1; ih.biBitCount = 24; ih.biCompression = BI_RGB;
			fwrite(&fh, sizeof(fh), 1, f); fwrite(&ih, sizeof(ih), 1, f);
			std::vector<unsigned char> row(rowBytes, 0);
			for (int y = h - 1; y >= 0; y--)
			{
				unsigned char* src = (unsigned char*)pBits + y * w * 4;
				for (int x = 0; x < w; x++) { row[x*3] = src[x*4]; row[x*3+1] = src[x*4+1]; row[x*3+2] = src[x*4+2]; }
				fwrite(&row[0], rowBytes, 1, f);
			}
			fclose(f); bRet = true;
		}
	}
	SelectObject(hdcMem, old); DeleteObject(hbm); DeleteDC(hdcMem); ReleaseDC(NULL, hdcScreen);
	return bRet;
}

static void SetImg(KRUImage& img, const char* name, int x, int y, int style, DWORD color, unsigned char flag)
{
	img.oPosition.nX = x; img.oPosition.nY = y; img.oPosition.nZ = 0;
	img.Color.Color_dw = color;
	img.bRenderStyle = (unsigned char)style;
	img.bRenderFlag = flag;
	img.nType = ISI_T_SPR;
	strncpy(img.szImage, name, sizeof(img.szImage) - 1); img.szImage[sizeof(img.szImage) - 1] = 0;
	img.uImage = 0; img.nISPosition = -1; img.nFrame = 0;
}

struct SprInfo { std::string name; int w, h, frames, res1; };

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		printf("dung: rep_harness <represent.dll> <out.bmp> <spr_list.txt> [nSprMax] [nPerfFrames]\n");
		return 1;
	}
	const char* szDll = argv[1]; const char* szOut = argv[2]; const char* szList = argv[3];
	int nSprMax = argc > 4 ? atoi(argv[4]) : 6;
	int nPerfFrames = argc > 5 ? atoi(argv[5]) : 60;

	WNDCLASSA wc; memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = HarnessWndProc; wc.hInstance = GetModuleHandle(NULL); wc.lpszClassName = "RepHarness";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	RegisterClassA(&wc);
	RECT rc = {0, 0, 1024, 768};
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	AdjustWindowRectEx(&rc, style, FALSE, 0);
	HWND hwnd = CreateWindowExA(0, "RepHarness", szDll, style, REP_OFF ? -3000 : 0, REP_OFF ? -3000 : 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, wc.hInstance, NULL);
	ShowWindow(hwnd, REP_NOACT ? SW_SHOWNOACTIVATE : SW_SHOW); UpdateWindow(hwnd); Pump();

	g_SetMainHWnd(hwnd);
	g_SetDrawHWnd(hwnd);	// engine tinh vung Blt theo cua so nay (KWin32App.cpp:204)
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	KPakList pak;
	if (!pak.Open("\\package.ini")) printf("[WARN] khong mo duoc package.ini\n");

	HMODULE hDll = LoadLibraryA(szDll);
	if (!hDll) { printf("[ERR] LoadLibrary %s: %lu\n", szDll, GetLastError()); return 2; }
	fnCreateRepresentShell pCreate = (fnCreateRepresentShell)GetProcAddress(hDll, "CreateRepresentShell");
	iRepresentShell* rs = pCreate ? pCreate() : NULL;
	if (!rs) { printf("[ERR] CreateRepresentShell\n"); return 3; }
	if (!rs->Create(1024, 768, false)) { printf("[ERR] Create(1024,768,false)\n"); return 4; }
	Pump();
	printf("[OK] %s IsRep3D=%d\n", szDll, rs->IsRep3D() ? 1 : 0);

	PrintMem("sau Create shell");
	int nFontId = 0;
	{
		KIniFile ini;
		if (ini.Load("\\ui\\ui3\\UiBasePublicSetting.ini"))
		{
			char szFile[256] = ""; int nId = 0;
			if (ini.GetInteger("FontList", "0", 0, &nId) && ini.GetString("FontList", "0_File", "", szFile, sizeof(szFile)) && szFile[0])
			{
				if (rs->CreateAFont(szFile, CHARACTER_CODE_SET_GBK, nId)) nFontId = nId;
				printf("[FONT] %s id=%d ok=%d\n", szFile, nId, nFontId != 0);
			}
		}
		else printf("[WARN] khong doc duoc UiBasePublicSetting.ini\n");
	}

	std::vector<SprInfo> sprs; int nNewIdx = -1;
	FILE* fl = fopen(szList, "r");
	char line[512];
	while (fl && fgets(line, sizeof(line), fl))
	{
		char* p = line; while (*p == ' ' || *p == '\t') p++;
		char* e = p + strlen(p); while (e > p && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t')) *--e = 0;
		if (!*p) continue;
		SPROFFS* pOff = NULL;
		SPRHEAD* ph = SprGetHeader(p, pOff);
		if (!ph) { printf("[MISS] %s\n", p); continue; }
		SprInfo si; si.name = p; si.w = ph->Width; si.h = ph->Height; si.frames = ph->Frames; si.res1 = ph->Reserved[1];
		printf("[SPR] %dx%d frames=%d res1=%d %s\n", si.w, si.h, si.frames, si.res1, p);
		if (si.res1 == 1 && nNewIdx < 0) nNewIdx = (int)sprs.size();
		sprs.push_back(si);
		SprReleaseHeader(ph);
		if ((int)sprs.size() >= 60) break;
	}
	if (fl) fclose(fl);
	if (sprs.empty()) { printf("[ERR] khong co sprite nao\n"); return 5; }

	// ---------- khung 1: bang kieu ve ----------
	int styles[7] = { IMAGE_RENDER_STYLE_ALPHA, IMAGE_RENDER_STYLE_ALPHA, IMAGE_RENDER_STYLE_OPACITY,
		IMAGE_RENDER_STYLE_3LEVEL, IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT, IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST, IMAGE_RENDER_STYLE_BORDER };
	DWORD colors[7] = { 0xff000000, 0x80000000, 0xff000000, 0xff000000, 0xff000000, 0xffff8040, 0xff000000 };
	const char* labels[7] = { "ALPHA a=255", "ALPHA a=128", "OPACITY", "3LEVEL", "NOT_BE_LIT", "COLOR_ADJUST ff8040", "BORDER" };

	// chon sprite vua (<= 200 px) cho bang kieu ve
	std::vector<int> tbl;
	for (int k = 0; k < (int)sprs.size() && (int)tbl.size() < nSprMax; k++)
		if (sprs[k].w <= 200 && sprs[k].h <= 200 && sprs[k].res1 == 0) tbl.push_back(k);
	if (tbl.empty()) tbl.push_back(0);
	int nCols = (int)tbl.size();
	int nRowH = 0; for (int k = 0; k < nCols; k++) if (sprs[tbl[k]].h > nRowH) nRowH = sprs[tbl[k]].h;
	nRowH += 8; if (nRowH > 95) nRowH = 95;
	rs->RepresentBegin(1, 0x00000000);
	for (int s = 0; s < 7; s++)
	{
		int x = 10;
		for (int k = 0; k < nCols; k++)
		{
			KRUImage img;
			SetImg(img, sprs[tbl[k]].name.c_str(), x, 10 + s * nRowH, styles[s], colors[s], 0);
			rs->DrawPrimitives(1, &img, RU_T_IMAGE, 1);
			x += sprs[tbl[k]].w + 12;
		}
		if (nFontId)
			rs->OutputText(nFontId, labels[s], (int)strlen(labels[s]), x + 10, 10 + s * nRowH, 0xffffff00, 0, TEXT_IN_SINGLE_PLANE_COORD, 0);
	}
	// spr moi (Reserved[1]==1) neu co: 3 kieu
	if (nNewIdx >= 0)
	{
		for (int s = 0; s < 3; s++)
		{
			KRUImage img;
			SetImg(img, sprs[nNewIdx].name.c_str(), 10 + s * 150, 690, s == 2 ? IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST : IMAGE_RENDER_STYLE_ALPHA,
				s == 1 ? 0x80000000 : (s == 2 ? 0xff4080ff : 0xff000000), 0);
			rs->DrawPrimitives(1, &img, RU_T_IMAGE, 1);
		}
		if (nFontId) rs->OutputText(nFontId, "spr moi Reserved[1]=1: a255 / a128 / adjust", 44, 470, 690, 0xffffff00, 0, TEXT_IN_SINGLE_PLANE_COORD, 0);
	}
	// cat theo khung (RU_T_IMAGE_4) va IMAGE_PART
	{
		KRUImage4 i4; memset(&i4, 0, sizeof(i4));
		SetImg(i4, sprs[0].name.c_str(), 700, 620, IMAGE_RENDER_STYLE_ALPHA, 0xff000000, 0);
		i4.oImgLTPos.nX = 8; i4.oImgLTPos.nY = 8; i4.oImgRBPos.nX = sprs[0].w - 16; i4.oImgRBPos.nY = sprs[0].h - 16;
		rs->DrawPrimitives(1, &i4, RU_T_IMAGE_4, 1);
		KRUImagePart ip; memset(&ip, 0, sizeof(ip));
		SetImg(ip, sprs[0].name.c_str(), 820, 620, IMAGE_RENDER_STYLE_ALPHA, 0xff000000, 0);
		ip.oImgLTPos.nX = 0; ip.oImgLTPos.nY = 0; ip.oImgRBPos.nX = sprs[0].w / 2; ip.oImgRBPos.nY = sprs[0].h / 2;
		rs->DrawPrimitives(1, &ip, RU_T_IMAGE_PART, 1);
	}
	// duong, khung, bong, diem
	{
		KRULine ln; ln.oPosition.nX = 700; ln.oPosition.nY = 560; ln.oPosition.nZ = 0; ln.oEndPos.nX = 1000; ln.oEndPos.nY = 600; ln.oEndPos.nZ = 0; ln.Color.Color_dw = 0xff00ff00;
		rs->DrawPrimitives(1, &ln, RU_T_LINE, 1);
		KRURect rt; rt.oPosition.nX = 700; rt.oPosition.nY = 500; rt.oPosition.nZ = 0; rt.oEndPos.nX = 800; rt.oEndPos.nY = 550; rt.oEndPos.nZ = 0; rt.Color.Color_dw = 0xffff0000;
		rs->DrawPrimitives(1, &rt, RU_T_RECT, 1);
		KRUShadow sh; sh.oPosition.nX = 850; sh.oPosition.nY = 500; sh.oPosition.nZ = 0; sh.oEndPos.nX = 950; sh.oEndPos.nY = 550; sh.oEndPos.nZ = 0; sh.Color.Color_dw = 0x10000000;
		rs->DrawPrimitives(1, &sh, RU_T_SHADOW, 1);
	}
	// toa do khong gian (LookAt) : don vi o giua man hinh
	{
		rs->LookAt(400000, 300000, 0);
		KRUImage img;
		SetImg(img, sprs[nCols > 1 ? 1 : 0].name.c_str(), 400000, 300000, IMAGE_RENDER_STYLE_ALPHA, 0xff000000, RUIMAGE_RENDER_FLAG_REF_SPOT);
		img.oPosition.nZ = 0;
		rs->DrawPrimitives(1, &img, RU_T_IMAGE, 0);
		if (nFontId) rs->OutputText(nFontId, "LookAt/REF_SPOT giua man hinh", 29, 400000, 300000 - 40, 0xff00ffff, 0, 0, 0);
	}
	if (nFontId) rs->OutputText(nFontId, "Represent test ABC xyz 0123", 27, 10, 745, 0xffffffff, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xff000000);
	if (nFontId) { char szVN[] = "KiÕm tra ch÷ ViÖt TCVN3: ThÖ Giíedi Vâ L©m"; rs->OutputVNText(nFontId, szVN, (int)strlen(szVN), 400, 745, 0xffffff80, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xff000000); }
	rs->RepresentEnd();
	Pump(); Sleep(300); Pump();

	std::string outPw = szOut; outPw.insert(outPw.size() - 4, "_pw");
	if (RepMode() == 2) { SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE); Pump(); Sleep(300); Pump(); }
	else if (!REP_OFF) { SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); SetForegroundWindow(hwnd); Pump(); Sleep(200); Pump(); }
	PrintMem("sau ve bang kieu (1 khung)");
	bool b1 = rs->SaveScreenToFile(szOut, SCRFILETYPE_BMP, 0);
	bool b2 = REP_OFF ? false : SaveScreenBlt(hwnd, outPw.c_str());
	printf("[SHOT] SaveScreenToFile=%d ScreenBlt=%d\n", b1 ? 1 : 0, b2 ? 1 : 0);

	// ---------- do toc do: N khung, moi khung ve tat ca sprite mau lap 8 lan ----------
	LARGE_INTEGER f, t0, t1; QueryPerformanceFrequency(&f);
	int nDraws = 0;
	QueryPerformanceCounter(&t0);
	for (int fr = 0; fr < nPerfFrames; fr++)
	{
		rs->RepresentBegin(1, 0x00000000);
		for (int rep = 0; rep < 8; rep++)
		{
			for (int k = 0; k < (int)sprs.size(); k++)
			{
				KRUImage img;
				SetImg(img, sprs[k].name.c_str(), (k * 97 + rep * 131 + fr * 3) % 900, (k * 53 + rep * 77 + fr * 2) % 650, IMAGE_RENDER_STYLE_ALPHA, 0xff000000, 0);
				img.nFrame = (short)(fr % (sprs[k].frames > 0 ? sprs[k].frames : 1));
				rs->DrawPrimitives(1, &img, RU_T_IMAGE, 1);
				nDraws++;
			}
		}
		rs->RepresentEnd();
		Pump();
	}
	QueryPerformanceCounter(&t1);
	PrintMem("sau PERF (moi sprite, moi khung)");
	double ms = (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / (double)f.QuadPart;
	printf("[PERF] %d khung, %d sprite/khung: %.2f ms/khung (%.1f fps), %.3f us/sprite\n",
		nPerfFrames, nDraws / (nPerfFrames > 0 ? nPerfFrames : 1), ms / nPerfFrames, 1000.0 * nPerfFrames / ms, 1000.0 * ms / nDraws);

	rs->Release();
	PrintMem("sau Release shell");
	FreeLibrary(hDll);
	DestroyWindow(hwnd);
	return 0;
}
