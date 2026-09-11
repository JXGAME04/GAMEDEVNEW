#include "precompile.h"
#include <crtdbg.h>
#include "TextureRes.h"
#include "TextureResMgr.h"
#include <tlhelp32.h>	// [REP3 08/09 q] dem so client dang mo
#include <process.h>	// [NAP 08/09 b] _beginthreadex


// [REP3 03/09 LOAD] ghi ten anh nap that bai vao jx_rep3.log (toi da 200 dong) - truoc day im lang va cache NULL vinh vien
static void Rep3LogLoadFail(const char* pszImage, int nType)
{
	static int s_nCount = 0;
	if (s_nCount >= 200)
		return;
	s_nCount++;
	Rep3Log("[REP3] LoadImage FAIL (%d/200) type=%d: %s", s_nCount, nType, pszImage ? pszImage : "");
}
// [REP3 03/09 LAG] anh nap that bai: nhip thu nap lai, tinh bang mili giay.
#define REP3_RELOAD_COOLDOWN	10000
#define REP3_RELOAD_COOLDOWN_LAU	600000	// [NAP 08/09 e] sau 3 lan hong: 10 phut
#ifdef JX_ANDROID
TextureResMgr* g_pJxTexMgr = NULL;	// [VE 11/09] bo quan ly duy nhat (KRepresentShell3::m_TextureResMgr) cho TextureRes.cpp / KRepresentShell3.cpp
#endif

TextureResMgr::TextureResMgr()
{
	m_uCheckPoint = ISBP_CHECK_POINT_DEF;
	m_tmLastCheckBalance = timeGetTime();
	m_bDoProfile = false;
	m_nLoadCount = 0;
	m_nReleaseCount = 0;
	m_uTexCacheMemUsed = 0;
    m_nMaxReleaseCount = 0;
	m_uBudgetFloorMB = 60;	// [REP3 08/09 q]
	m_bVeDangDien = false; m_nNapNenGui = 0; m_nNapNenXong = 0; m_nNapNenHong = 0; m_nNapNenBoVe = 0;	// [NAP 08/09 b]
	m_nNapTruocKip[0] = m_nNapTruocKip[1] = m_nNapTruocKip[2] = 0; m_nNapTruocTre[0] = m_nNapTruocTre[1] = m_nNapTruocTre[2] = 0;
	m_dwKhoaNgoai = 0;	// [VE 09/09 d]	// [NAPCHIEU 09/09 b] [NAPNPC 09/09]
	m_hNapLuong = NULL; m_hNapCo = NULL; m_lNapDung = 0; m_bNapNenLoi = false;
#ifdef JX_ANDROID
	m_pJxKhungDangChay = NULL; m_uJxApKhungCuoi = 0; m_dJxApCuoi = 0.0; g_pJxTexMgr = this;	// [VE 11/09]
#endif
	
	// 根据物理内存大小决定资源缓冲区的大小
	// [REP3 03/09] ngan sach cache texture theo RAM (2.0: 30/50/80/120 MB); may 4 GB+ cho rong hon vi texture 8888
	// [REP3 03/09 RAM] ctor chay TRUOC khi Create() doc config.ini -> chi dat mac dinh, SetBudget() tinh lai sau
	m_nBalanceNum = 256 * 1024 * 1024;
	m_nMaxReleaseCount = 64;
	m_uCheckPoint = 25;
}

TextureResMgr::~TextureResMgr()
{
	Free();
#ifdef JX_ANDROID
	if (g_pJxTexMgr == this) g_pJxTexMgr = NULL;	// [VE 11/09]
#endif
}

void TextureResMgr::SetBalanceParam(int32 nNumImage, uint32 uCheckPoint)
{
//	m_nBalanceNum = nNumImage;
//	m_uCheckPoint = uCheckPoint;
}

void TextureResMgr::CheckBalance()
{
	// [REP3 03/09] theo represent3free.dll 2.0 (0x10024F80): duyet tu cuoi, bo qua tai nguyen vua ve khung truoc,
	// chi xet tai nguyen nghi > 10 s; moi luot BO DUNG MOT KHUNG texture (ReleaseAFrameData) - het khung
	// moi xoa ca tai nguyen. Khong con cu xoa hang loat 16/32/64 cai gay khung.
	// [REP3 03/09 RAM] vuot ngan sach VRAM: bo toi da 8 khung/luot, chi can nghi > 1 s (Release() texture re, ton khi tao lai thoi)
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	// [REP3 03/09 SOC] Theo DUNG 2.0: chi MOT che do. Moi luot bo DUNG MOT muc roi thoi, va chi
	// xet tai nguyen da nghi qua 10 giay (0x10025096 cmp ebx,0x2710). Toan than CheckBalance cua
	// 2.0 (0x10024F80..0x100251E4) khong doc mot tran byte nao - no khong he co che do khan cap.
	// Che do bOver cu (8 khung/luot, ha nguong nghi xuong 1 giay) la cua rieng ta, va da do duoc
	// tac hai: voi Rep3CacheMB=120 no gay 508 luot bo MOI GIAY (so voi 30/giay o ngan sach cao),
	// va bo dung nhung khung DANG VE: "ve khung nay" 34-124 MB tren cache chi 105-155 MB.
	const int nMax = 1;
	const uint32 uIdle = 10000;
	uint32 nTickCount = GetTickCount();
	int nDone = 0;
	for (int i = (int)m_TextureResList.size() - 1; i >= 0 && nDone < nMax; i--)
	{
		ResNode& node = m_TextureResList[i];
		if (!node.m_bCacheable || !node.m_pTextureRes)
			continue;
		if (node.m_pTextureRes->m_bLastFrameUsed)
			continue;
		if ((nTickCount - node.m_nLastUsedTime) <= uIdle)
			continue;
		m_nReleaseCount++;
		nDone++;
		{
			// [FX 07/09] tru ngay phan vua tha (truoc: m_uTexCacheMemUsed chi tinh lai o EndProfile nen 'vuot ngan sach' keo dai ca khung)
			unsigned long uTruoc = node.m_pTextureRes->m_nTexMemUsed;
			if (node.m_pTextureRes->ReleaseAFrameData())
			{
				unsigned long uSau = node.m_pTextureRes->m_nTexMemUsed;
				if (uTruoc > uSau && m_uTexCacheMemUsed >= (uTruoc - uSau))
					m_uTexCacheMemUsed -= (uint32)(uTruoc - uSau);
				continue;
			}
		}
		if (m_uTexCacheMemUsed >= node.m_pTextureRes->m_nTexMemUsed)
			m_uTexCacheMemUsed -= node.m_pTextureRes->m_nTexMemUsed;
		node.m_pTextureRes->Release();
		SAFE_DELETE(node.m_pTextureRes);
		m_TextureResList.erase(m_TextureResList.begin() + i);
	}
}

void TextureResMgr::SetBudget()
{
	// [REP3 03/09 RAM] ngan sach cache texture = VRAM (texture o POOL_DEFAULT, khong chiem RAM tien trinh): RAM/16 kep 60..384 MB;
	// 2.0 dung toi da 120 MB 4444 (= 240 MB 8888). [Client] Rep3CacheMB ghi de.
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);
	unsigned __int64 uPhysMB = stat.ullTotalPhys / (1024 * 1024);
	unsigned __int64 uBudgetMB = uPhysMB / 16;
	if (uBudgetMB < 60)  uBudgetMB = 60;
	if (uBudgetMB > 1024) uBudgetMB = 1024;	// [FX 08/09] 512 -> 1024: tran 512 lam cache KET (508-511/512, 07/09 17:xx) va nhanh vuot
											// ngan sach bao het khung hieu ung nghi > 10 s -> 'mat het hinh anh ky nang'. Chu xac nhan
											// Rep3CacheMB=1500 het loi qua 5 tran (dinh 768 MB, RAM rieng 789 MB). 1024 MB ~ 240 + 0,66 x 1024
											// = ~920 MB RAM rieng, duoi 2 GB (Game.exe chua LARGEADDRESSAWARE). Kep them theo VRAM: CapBudgetByVram.
											// [REP3 03/09 RAM2] DINH CHINH: chu thich cu noi POOL_DEFAULT
											// "khong an RAM" la SAI. Do 12 mau trong jx_rep3.log cua game that:
											//   RAM rieng = 240,9 + 0,657 x (texture + raw spr)
											// tuc moi MB texture o VRAM VAN keo theo ~0,66 MB RAM tien trinh, do WDDM
											// cap bo dem he thong cho moi vung nho D3D9. Tran 768 cho phep RAM len ~775 MB
											// tren tran dia chi 2048 MB cua tien trinh 32-bit. Tran 512 -> ~607 MB.
											// Van tren muc cache thuc te (438-451 MB) nen khong quay lai loi giat cua tran 384.
	if (g_nRep3CacheMB > 0)
		uBudgetMB = (unsigned __int64)g_nRep3CacheMB;
#ifdef JX_POSIX
	{	// [ANDROID 11/09 CACHE] dien thoai: texture nam trong RAM chung CPU/GPU (khong co VRAM rieng), he thong giet app khi
		// tien trinh qua lon -> kep <= RAM/3 KE CA khi [Client] Rep3CacheMB ep (config Android tung chep 1500 MB tu PC 32 GB).
		if (g_nRep3CacheMB <= 0)
		{	// tu tinh: RAM/8 kep [128, 512] MB (PC: RAM/16 kep 60..1024 - dien thoai it RAM hon nhieu, ma texture la thu an nhieu nhat;
			// ban CPU cua texture da bo (BOCPU) nen so nay ~ RAM that texture chiem). CapBudgetByVram con kep theo 'VRAM' ao cua SDL_GPU.
			uBudgetMB = uPhysMB / 8;
			if (uBudgetMB < 128) uBudgetMB = 128;
			if (uBudgetMB > 512) uBudgetMB = 512;
		}
		unsigned __int64 uTranMB = uPhysMB / 3;
		if (uTranMB < 128) uTranMB = 128;
		if (uBudgetMB > uTranMB)
		{
			Rep3Log("[REP3] cache texture: %llu MB (Rep3CacheMB=%d) qua lon so voi RAM %llu MB cua may -> kep %llu MB", uBudgetMB, g_nRep3CacheMB, uPhysMB, uTranMB);
			uBudgetMB = uTranMB;
		}
	}
#endif
	m_nBalanceNum = (int32)(uBudgetMB * 1024 * 1024);
	Rep3Log("[REP3] cache texture: RAM %llu MB -> ngan sach %llu MB (%s)", uPhysMB, uBudgetMB, g_nRep3Pool ? "VRAM, POOL_DEFAULT" : "RAM+VRAM, POOL_MANAGED");
}

// [FX 08/09] Kep them theo VRAM con trong luc tao device (texture o POOL_DEFAULT = VRAM): toi da 1/2 VRAM con.
// Chi ap khi KHONG co [Client] Rep3CacheMB (ini ghi de thi giu nguyen y chu). Goi tu KRepresentShell3::Create sau CreateDevice.
// [REP3 08/09 q] dem tien trinh cung ten exe (4 tab game): moi client chia nhau VRAM
static int Rep3DemClient()
{
#ifdef JX_POSIX
	return 1;	// [ANDROID 08/09] mot tien trinh
#else
	char szMe[MAX_PATH] = ""; GetModuleFileNameA(NULL, szMe, MAX_PATH);
	const char* pMe = strrchr(szMe, '\\'); pMe = pMe ? pMe + 1 : szMe;
	int n = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) return 1;
	PROCESSENTRY32 pe; pe.dwSize = sizeof(pe);
	if (Process32First(hSnap, &pe)) { do { if (_stricmp(pe.szExeFile, pMe) == 0) n++; } while (Process32Next(hSnap, &pe)); }
	CloseHandle(hSnap);
	return n < 1 ? 1 : n;
#endif
}
void TextureResMgr::CapBudgetByVram(unsigned __int64 uVramFreeMB)
{
	if (g_nRep3CacheMB > 0 || uVramFreeMB == 0)
		return;
	unsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;
	// [REP3 08/09 q] chia cho so client dang mo va he so trang atlas (1,3); san 256 MB (khung TK nang ~200 MB) hoac VRAM/2
	int nClients = Rep3DemClient();
#ifdef JX_PLATFORM_SDL
	unsigned __int64 uHeSo10 = ((g_nRep3ApiOn == 11 && g_nRep3Atlas) || (g_nRep3ApiOn == 100 && g_nRep3AtlasGpu)) ? 13 : 10;	// [GPU 11/09 ATLAS] trang atlas SDL_GPU cung ton them ~1,3x
#else
	unsigned __int64 uHeSo10 = (g_nRep3ApiOn == 11 && g_nRep3Atlas) ? 13 : 10;
#endif
	unsigned __int64 uCapMB = uVramFreeMB * 10 / (unsigned __int64)nClients / 2 / uHeSo10;
	unsigned __int64 uFloorMB = 256;
	if (uFloorMB > uVramFreeMB / 2) uFloorMB = uVramFreeMB / 2;
	if (uFloorMB < 60) uFloorMB = 60;
	m_uBudgetFloorMB = uFloorMB;
	if (uCapMB < uFloorMB)
		uCapMB = uFloorMB;
	Rep3Log("[REP3] cache texture: VRAM con %llu MB, %d client dang mo, he so atlas %llu/10 -> tran %llu MB (san %llu)", uVramFreeMB, nClients, uHeSo10, uCapMB, uFloorMB);
	if (uBudgetMB > uCapMB)
	{
		m_nBalanceNum = (int32)(uCapMB * 1024 * 1024);
		Rep3Log("[REP3] cache texture: VRAM con %llu MB -> kep ngan sach %llu -> %llu MB", uVramFreeMB, uBudgetMB, uCapMB);
	}
}

// [REP3 08/09 q] luc chay: VRAM con < 128 MB (may yeu, nhieu tab) -> ha ngan sach 25 % toi san, dep bot khung ngay. Chi che do tu dong.
void TextureResMgr::PressureByVram(unsigned __int64 uVramFreeMB)
{
	if (g_nRep3CacheMB > 0 || uVramFreeMB == 0 || uVramFreeMB >= 128)
		return;
	unsigned __int64 uBudgetMB = ((unsigned __int64)(uint32)m_nBalanceNum) >> 20;
	if (uBudgetMB <= m_uBudgetFloorMB)
		return;
	unsigned __int64 uNewMB = uBudgetMB * 3 / 4;
	if (uNewMB < m_uBudgetFloorMB) uNewMB = m_uBudgetFloorMB;
	m_nBalanceNum = (int32)(uNewMB * 1024 * 1024);
	Rep3Log("[REP3] cache texture: VRAM con %llu MB thap -> ha ngan sach %llu -> %llu MB", uVramFreeMB, uBudgetMB, uNewMB);
	m_tmLastCheckBalance = 0;
	CheckBalance();
}
void TextureResMgr::GetStat(uint32& uNodes, uint32& uTexMB, uint32& uRawMB, uint32& uDrawMB, uint32& uBudgetMB)
{
	// [REP3 03/09 RAM] cho dong thong ke trong jx_rep3.log
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	unsigned __int64 uRaw = 0;
	for (int i = 0; i < (int)m_TextureResList.size(); i++)
		if (m_TextureResList[i].m_pTextureRes)
			uRaw += m_TextureResList[i].m_pTextureRes->m_nSprMemUsed;
	uNodes = (uint32)m_TextureResList.size();
	uTexMB = m_uTexCacheMemUsed >> 20;
	uDrawMB = m_uMemDrawingUsed >> 20;
	uRawMB = (uint32)(uRaw >> 20);
	uBudgetMB = ((uint32)m_nBalanceNum) >> 20;
}

void TextureResMgr::CheckBalanceFrame()
{
	DWORD tmCur = timeGetTime();
	if ((tmCur - m_tmLastCheckBalance) <= m_uCheckPoint)
		return;
	m_tmLastCheckBalance = tmCur;
	CheckBalance();
}

uint32 TextureResMgr::CreateImage(const char* pszName, int32 nWidth, int32 nHeight, int32 nType)
{
	if (!pszName || !pszName[0])
		return 0;

	if (nType != ISI_T_BITMAP16)
		return 0;
	
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	// 将名字映射为ID
	uint32 uImage = g_FileName2Id((LPSTR)pszName);
	// 如果资源已经存在则返回
	int nIdx = FindImage(uImage, 0);
	if (nIdx >= 0)
		return 0;
	
	// 创建贴图资源
	TextureResBmp *pTexRes = new TextureResBmp;
	if (pTexRes == NULL)
		return 0;
	if (!pTexRes->CreateImage(pszName, nWidth, nHeight, nType))
	{
		delete pTexRes;
		return 0;
	}

	ResNode node;
	node.m_bCacheable = false;
	node.m_nLastUsedTime = GetTickCount();
	node.m_nType = nType;
	node.m_nID = uImage;
	node.m_pTextureRes = pTexRes;

	// 将贴图资源加入数组
	nIdx = - nIdx - 1;
	m_TextureResList.insert(m_TextureResList.begin() + nIdx, node);
	
	return uImage;
}

int TextureResMgr::FindImage(uint32 uImage, int nPossiblePosition)
{
	int nPP = nPossiblePosition;
	int nNumImages = m_TextureResList.size();
	if (nPP < 0 || nPP >= nNumImages)
	{
		if (nNumImages <= 0)
		{
			return -1;
		}
		else
		{
			nPP = nNumImages / 2;
		}
	}

	if (m_TextureResList[nPP].m_nID == uImage)
		return nPP;

	int nFrom, nTo, nTryRange;
	nTryRange = ISBP_TRY_RANGE_DEF;

	if (m_TextureResList[nPP].m_nID > uImage)
	{
		nFrom = 0;
		nTo = nPP - 1;
		nPP -= nTryRange;
	}
	else
	{
		nFrom = nPP + 1;
		nTo = nNumImages - 1;
		nPP += nTryRange;
	}

	if (nFrom + nTryRange >= nTo)
		nPP = (nFrom + nTo) >> 1;

	while (nFrom < nTo)
	{
		if (m_TextureResList[nPP].m_nID < uImage)
		{
			nFrom = nPP + 1;
		}
		else if (m_TextureResList[nPP].m_nID > uImage)
		{
			nTo = nPP - 1;
		}
		else
		{
			return nPP;
		}
		nPP = (nFrom + nTo) >> 1;
	}

	if (nFrom == nTo)
	{
		if (m_TextureResList[nPP].m_nID > uImage)
		{
			nPP = - nPP - 1;
		}
		else if (m_TextureResList[nPP].m_nID < uImage)
		{
			nPP = - nPP - 2;
		}
	}
	else
	{
		nPP = - nFrom -1;
	}
	
	return nPP;
}

void TextureResMgr::FreeImage( const char* pszImage)
{
	if (!pszImage || !pszImage[0])
	    return;

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	unsigned int uImage = g_FileName2Id((LPSTR)pszImage);
	int nIdx = FindImage(uImage, 0);

	if (nIdx < 0)
		return;

	SAFE_DELETE(m_TextureResList[nIdx].m_pTextureRes);
	m_TextureResList.erase(m_TextureResList.begin() + nIdx);	
}

void TextureResMgr::Free()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	NapNenDung();	// [NAP 08/09 b]

	for(int i=0; i<m_TextureResList.size(); i++)
		SAFE_DELETE(m_TextureResList[i].m_pTextureRes);

	m_TextureResList.clear();
	m_tmLastCheckBalance = timeGetTime();
}

bool TextureResMgr::GetImageParam( const char* pszImage, KImageParam* pImageData, int nType)
{
	if (!pszImage || !pszImage[0])
		return false;

	unsigned int uImage = 0;
	short nPos = 0;
	const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false;	// [NAP 08/09 b] hoi dong bo: nap ngay
	void* pTemp = GetImage(pszImage, uImage, nPos, 0, nType, false);
	m_bVeDangDien = bVeLuu;
	if (!pTemp)
		return false;

	switch(nType)
	{
	case ISI_T_SPR:
		pImageData->nHeight = ((TextureResSpr *)pTemp)->GetHeight();
		pImageData->nInterval = ((TextureResSpr *)pTemp)->GetInterval();
		pImageData->nNumFrames = ((TextureResSpr *)pTemp)->GetFrameNum();
		pImageData->nNumFramesGroup = ((TextureResSpr *)pTemp)->GetDirections();
		pImageData->nReferenceSpotX = ((TextureResSpr *)pTemp)->GetCenterX();
		pImageData->nReferenceSpotY = ((TextureResSpr *)pTemp)->GetCenterY();
		pImageData->nWidth = ((TextureResSpr *)pTemp)->GetWidth();
		break;
	case ISI_T_BITMAP16:
		pImageData->nHeight = ((TextureResBmp *)pTemp)->GetHeight();
		pImageData->nInterval = 0;
		pImageData->nNumFrames = 1;
		pImageData->nNumFramesGroup = 1;
		pImageData->nReferenceSpotX = 0;
		pImageData->nReferenceSpotY = 0;
		pImageData->nWidth = ((TextureResBmp *)pTemp)->GetWidth();
		break;
	}
	return true;
}

bool TextureResMgr::GetImageFrameParam(const char* pszImage,	int nFrame,
		KRPosition2* pOffset, KRPosition2* pSize, int nType)
{
	if (!pszImage || !pszImage[0])
		return false;

	unsigned int uImage = 0;
	short nPos = 0;
	const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false;	// [NAP 08/09 b] hoi dong bo: nap ngay
	void* pTemp = GetImage(pszImage, uImage, nPos, 0, nType, false);
	m_bVeDangDien = bVeLuu;
	if (!pTemp)
		return false;
	bool bRet = false;
	switch(nType)
	{
	case ISI_T_SPR:
		{
			TextureResSpr* pSpr = (TextureResSpr *)pTemp;
			if(nFrame >= pSpr->m_nFrameNum)
				break;

			if (pOffset)
			{
				pOffset->nX = pSpr->m_pFrameInfo[nFrame].nOffX;
				pOffset->nY = pSpr->m_pFrameInfo[nFrame].nOffY;
			}
			if (pSize)
			{
				pSize->nX = pSpr->m_pFrameInfo[nFrame].nWidth;
				pSize->nY = pSpr->m_pFrameInfo[nFrame].nHeight;
			}
			bRet = true;
		}
		break;
	case ISI_T_BITMAP16:
		if (pOffset)
		{
			pOffset->nX = 0;
			pOffset->nY = 0;
		}
		if (pSize)
		{
			pSize->nX = ((TextureResBmp *)pTemp)->GetWidth();
			pSize->nY = ((TextureResBmp *)pTemp)->GetHeight();
		}
		bRet = true;
		break;
	}
	return bRet;
}

TextureRes* TextureResMgr::GetImage( const char* pszImage, unsigned int& uImage, short& nImagePosition, 
								int nFrame, int nType, bool bPrepareTex)
{
#ifdef JX_ANDROID
	g_nJxAnhBoVeNen = 0;	// [VE 11/09]
#endif
	if (!pszImage || !pszImage[0])
		return NULL;
	
	if (!uImage)
	{
		uImage = g_FileName2Id((LPSTR)pszImage);	// const char to LPSTR, maybe problem.
	}

    KhoaTuyChon AutoLock(m_ImageProcessLock, m_dwKhoaNgoai != GetCurrentThreadId());	// [VE 09/09 d] DrawPrimitives dang giu khoa ngoai tren luong nay -> bo
	{ extern int g_nRep3VeMau; if (m_bVeDangDien && g_nRep3VeMau) Rep3VeDem(pszImage); }	// [VE 08/09 a] dem don vi ve theo loai; [VE 09/09 d] chi khung mau (1/8)

	TextureRes* pObject = NULL;
	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{
		if (m_TextureResList[nImagePosition].m_nType == nType)
		{
			m_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();
			if (m_TextureResList[nImagePosition].m_nNapTruoc)	// [NAPCHIEU 09/09 b] lan hoi dau tien cua muc nap truoc; [NAPNPC 09/09] dem theo nguon
			{
				const int nNg = (m_TextureResList[nImagePosition].m_nNapTruoc < 3) ? (int)m_TextureResList[nImagePosition].m_nNapTruoc : 0;
				m_TextureResList[nImagePosition].m_nNapTruoc = 0;
				if (m_TextureResList[nImagePosition].m_bDangNap) m_nNapTruocTre[nNg]++;
				else if (m_TextureResList[nImagePosition].m_pTextureRes) m_nNapTruocKip[nNg]++;
			}
			if (m_TextureResList[nImagePosition].m_bDangNap)	// [NAP 08/09 b] dang nap o luong nen
			{
				if (m_bVeDangDien) { m_nNapNenBoVe++; return NULL; }	// dang ve: bo anh nay khung nay, khung sau co
				// hoi dong bo (kich thuoc / alpha): nap ngay; ket qua luong nen ve sau se bi bo (NapNenNhan thay muc het 'dang nap')
				m_TextureResList[nImagePosition].m_bDangNap = false;
				m_TextureResList[nImagePosition].m_pTextureRes = LoadImage(pszImage, nType);
				if (m_TextureResList[nImagePosition].m_pTextureRes) { m_nLoadCount++; m_uTexCacheMemUsed += m_TextureResList[nImagePosition].m_pTextureRes->m_nTexMemUsed; }
				else Rep3LogLoadFail(pszImage, nType);
			}
			pObject = m_TextureResList[nImagePosition].m_pTextureRes;
			if (!pObject)	// [REP3 03/09 LAG] muc NULL = lan truoc nap that bai. CHI thu lai moi 10 giay.
			{				// Truoc day thu lai MOI KHUNG VE: 200 anh thieu x 62 fps = ~19.000 luot
								// quet 40 pak moi giay -> chinh la nguyen nhan giat.
				uint32 tmNow = GetTickCount();
				if ((tmNow - m_TextureResList[nImagePosition].m_nRetryTime) >= (m_TextureResList[nImagePosition].m_nLanHong >= 3 ? (uint32)REP3_RELOAD_COOLDOWN_LAU : (uint32)REP3_RELOAD_COOLDOWN))	// [NAP 08/09 e]
				{
					m_TextureResList[nImagePosition].m_nRetryTime = tmNow;
					if (g_nRep3NapNen && m_bVeDangDien && NapNenGiao(pszImage, uImage, nType))	// [NAP 08/09 b]
					{
						m_TextureResList[nImagePosition].m_bDangNap = true;
						return NULL;
					}
					pObject = LoadImage(pszImage, nType);
					if (pObject)
					{
						m_nLoadCount++;
						m_TextureResList[nImagePosition].m_pTextureRes = pObject;
						Rep3Log("[REP3] LoadImage OK sau khi that bai: %s", pszImage);
						m_TextureResList[nImagePosition].m_nLanHong = 0;
					}
					else
					{
						if (m_TextureResList[nImagePosition].m_nLanHong < 255) m_TextureResList[nImagePosition].m_nLanHong++;	// [NAP 08/09 e]
						Rep3LogLoadFail(pszImage, nType);
					}
				}
			}
		}
		else if (m_TextureResList[nImagePosition].m_bCacheable == true &&
				(pObject = LoadImage(pszImage, nType)))
		{			
			m_nLoadCount++;
    
            // [REP3 03/09 SOC] 2.0 KIEM NULL o dung cho nay (represent3free.dll 0x10025A31:
            //   cmp dword [eax+0x10], 0 / je) truoc khi dung TextureRes cu. Ta thieu.
            // Tu khi ta chen lai muc NULL cho anh nap hong (dung nhu 2.0 va nhu ma goc), mot
            // anh hong bi hoi lai bang KIEU KHAC se roi vao day, tro vao con tro NULL -> sap.
            if (m_TextureResList[nImagePosition].m_pTextureRes)
            {
                if (m_uTexCacheMemUsed >= m_TextureResList[nImagePosition].m_pTextureRes->m_nTexMemUsed)
                    m_uTexCacheMemUsed -= m_TextureResList[nImagePosition].m_pTextureRes->m_nTexMemUsed;
                SAFE_DELETE(m_TextureResList[nImagePosition].m_pTextureRes);
            }
			m_TextureResList[nImagePosition].m_pTextureRes = pObject;
			m_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();
            m_uTexCacheMemUsed += pObject->m_nTexMemUsed;

		}
		if(pObject && m_bDoProfile)
		{
			pObject->m_bLastFrameUsed = true;
			m_nUseCount++;
		}
	}
	else
	{
		const bool bNapNen = (g_nRep3NapNen && m_bVeDangDien && NapNenGiao(pszImage, uImage, nType));	// [NAP 08/09 b] dang ve: giao luong nen
		if (!bNapNen)
		{
		pObject = LoadImage(pszImage, nType);
		m_nLoadCount++;
		if (!pObject)	// [REP3 03/09 LAG] VAN chen muc NULL: lan sau FindImage thay ngay, khoi quet lai 40 pak.
			Rep3LogLoadFail(pszImage, nType);	// van nap lai duoc, nhung theo nhip REP3_RELOAD_COOLDOWN
		}

		ResNode node;
		node.m_bDangNap = bNapNen;
		node.m_bCacheable = true;
		node.m_nLastUsedTime = GetTickCount();
		node.m_nRetryTime = GetTickCount();	// [REP3 03/09 LAG]
		node.m_nLanHong = (!bNapNen && !pObject) ? 1 : 0;	// [NAP 08/09 e]
		node.m_nType = nType;
		node.m_nID = uImage;
		node.m_pTextureRes = pObject;
		nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
		m_TextureResList.insert(m_TextureResList.begin() + nImagePosition, node);
		if (bNapNen)
			return NULL;	// [NAP 08/09 b] khung sau NapNenNhan gan ket qua vao muc

        
		if(pObject && m_bDoProfile)
		{
            m_uTexCacheMemUsed += pObject->m_nTexMemUsed;

			pObject->m_bLastFrameUsed = true;
			m_nUseCount++;
		}
	}

	DWORD tmCur = timeGetTime();
	//为了执行效率所以未把下面的判断放在CheckBalance函数体里面。下同。
	if (
        (m_uTexCacheMemUsed > m_nBalanceNum) && 
		(
            (tmCur < m_tmLastCheckBalance) || 	// [FX 07/09] truoc la <= : cung mili giay goi bao nhieu lan bo bay nhieu khung
            ((tmCur - m_tmLastCheckBalance) > m_uCheckPoint)
        )
    )
	{
		m_tmLastCheckBalance = tmCur;
		CheckBalance();
	}
	
	if(pObject && nType == ISI_T_SPR)
	{
		if(!((TextureResSpr*)pObject)->PrepareFrameData(pszImage, nFrame, bPrepareTex))
			return NULL;
	}

	return pObject;
}

int32 TextureResMgr::GetImagePixelAlpha( const char* pszImage, int nFrame, int nX, int nY, int nType)
{
	if (!pszImage || !pszImage[0])
		return 0;

	int nRet = 0;

	unsigned int uImage = 0;
	short nPos = 0;
	void* pTemp;

	switch(nType)
	{
	case ISI_T_SPR:
		{ const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false; pTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_SPR); m_bVeDangDien = bVeLuu; }	// [NAP 08/09 b]
		if (!pTemp)
			break;
		nRet = ((TextureResSpr *)pTemp)->GetPixelAlpha(nFrame, nX, nY);	// 在Spr的方法里已经处理了范围
		break;
	case ISI_T_BITMAP16:
		{ const bool bVeLuu = m_bVeDangDien; m_bVeDangDien = false; pTemp = GetImage(pszImage, uImage, nPos, nFrame, ISI_T_BITMAP16); m_bVeDangDien = bVeLuu; }	// [NAP 08/09 b]
		if (!pTemp)
			break;
		if (nX < 0 || nY < 0 || nX >= ((TextureResBmp *)pTemp)->GetWidth() || nY >= ((TextureResBmp *)pTemp)->GetHeight())
			nRet = 0;
		else
			nRet = 255;
		break;
	default:
		break;
	}
	
	return nRet;
}

TextureRes* TextureResMgr::LoadImage( const char* pszImageFile, uint32 nType) const
{
	TextureRes* pRet = NULL;
	switch(nType)
	{
	case ISI_T_SPR:
		pRet = new TextureResSpr;
		if (!pRet)
			break;
		{	LARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);	// [NAP 08/09 a] tep spr: pak + giai nen
			bool bNap = pRet->LoadImage((LPSTR)pszImageFile, nType);
			QueryPerformanceCounter(&liB); Rep3NapCong(g_napSpr, Rep3NapMs(liA, liB));
		if (!bNap)
		{
			delete pRet;
			pRet = NULL;
			break;
		}
		}
		break;
	case ISI_T_BITMAP16:
		{
			pRet = new TextureResBmp;
			if (!pRet)
				break;
			LARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);	// [NAP 08/09 a] jpeg nen
			bool bNap = pRet->LoadImage((LPSTR)pszImageFile, nType);
			QueryPerformanceCounter(&liB); Rep3NapCong(g_napJpeg, Rep3NapMs(liA, liB));
			if (!bNap)
			{
				delete pRet;
				pRet = NULL;
				break;
			}
		}
		break;
	default:
		break;
	}

	return pRet;
}


void TextureResMgr::StartProfile()
{
	m_nHitCount = 0;
	m_nUseCount = 0;
	m_bDoProfile = true;
	m_uMemDrawingUsed = 0;

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for(int i=0; i<m_TextureResList.size(); i++)
		if(m_TextureResList[i].m_pTextureRes)
			m_TextureResList[i].m_pTextureRes->m_bLastFrameUsed = false;
}

void TextureResMgr::EndProfile()
{
	m_uTexCacheMemUsed = 0;
	m_bDoProfile = false;

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for(int i=0; i<m_TextureResList.size(); i++)
	{
		if(m_TextureResList[i].m_pTextureRes)
		{
			m_uTexCacheMemUsed += m_TextureResList[i].m_pTextureRes->m_nTexMemUsed;
			if(m_TextureResList[i].m_pTextureRes->m_bLastFrameUsed)
            {
				m_uMemDrawingUsed += m_TextureResList[i].m_pTextureRes->m_nTexMemUsed;
            }
		}
	}
}

void TextureResMgr::GetProfileString(char* str, int BufLen)
{
	if(!str)
		return;

	char cc[200];
	sprintf(
        cc,
        "CacheMemUsed: %dM\n MemDrawingUsed: %d\n TextureNumber: %d\n",
			m_uTexCacheMemUsed / 1024 / 1024, m_uMemDrawingUsed / 1024 / 1024, m_TextureResList.size());

	if(BufLen > strlen(cc))
		strcpy(str, cc);
}

bool TextureResMgr::InvalidateDeviceObjects()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for(int i=0; i<m_TextureResList.size(); i++)
	{
		if(m_TextureResList[i].m_pTextureRes)
		{
			if(!m_TextureResList[i].m_pTextureRes->InvalidateDeviceObjects())
				return false;
		}
	}
	return true;
}

bool TextureResMgr::RestoreDeviceObjects()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for(int i=0; i<m_TextureResList.size(); i++)
	{
		if(m_TextureResList[i].m_pTextureRes)
		{
			if(!m_TextureResList[i].m_pTextureRes->RestoreDeviceObjects())
				return false;
		}
	}
	return true;
}
// ============================ [NAP 08/09 b] nap tai nguyen o luong nen ============================
// Chay o luong nen: chi doc pak + giai ma, KHONG dung device (ZCache/XPackFile co khoa san; JPEG khoa rieng trong LoadJpegDecode).
static TextureRes* NapNenTai(const char* pszTen, uint32 nType)
{
	if (nType == ISI_T_SPR)
	{
		TextureResSpr* p = new TextureResSpr;
		if (!p->LoadImage((LPSTR)pszTen, nType)) { delete p; return NULL; }
		return p;
	}
	if (nType == ISI_T_BITMAP16)
	{
		TextureResBmp* p = new TextureResBmp;
		p->Release();
		p->m_nType = nType;
		if (!p->LoadJpegDecode((LPSTR)pszTen)) { delete p; return NULL; }
		return p;
	}
	return NULL;
}

unsigned __stdcall TextureResMgr::NapNenLuong(void* p)
{
	((TextureResMgr*)p)->NapNenChay();
	return 0;
}

void TextureResMgr::NapNenChay()
{
	for (;;)
	{
		WaitForSingleObject(m_hNapCo, INFINITE);
		if (m_lNapDung)
			break;
		for (;;)
		{
			NapViec v;
#ifdef JX_ANDROID
			JxKhungViec kv; bool bKhung = false;	// [VE 11/09]
#endif
			{
				KAutoCriticalSection k(m_napKhoa);
#ifdef JX_ANDROID
				if (m_napViec.empty() && !m_jxKhungViec.empty())	// [VE 11/09] hang TRUOC (anh dang ve can) > KHUNG (dang ve can / nap truoc) > hang SAU
				{
					kv = m_jxKhungViec.front(); m_jxKhungViec.erase(m_jxKhungViec.begin()); m_pJxKhungDangChay = kv.pSpr; bKhung = true;
				}
				else
				{
#endif
				vector<NapViec>& q = m_napViec.empty() ? m_napViecSau : m_napViec;	// [NAPNPC 09/09] hang TRUOC (anh dang ve can) het roi moi den hang SAU (nap truoc)
				if (q.empty())
					break;
				v = q.front();
				q.erase(q.begin());
#ifdef JX_ANDROID
				}
#endif
			}
#ifdef JX_ANDROID
			if (bKhung)
			{
				JxKhungXong kq; memset(&kq, 0, sizeof(kq)); kq.pSpr = kv.pSpr; kq.nFrame = kv.nFrame; kq.nBpp = kv.nBpp; kq.eFmt = kv.eFmt; kq.bPal = kv.bPal; kq.uLuc = kv.uLuc;
				LARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);
				kq.bHong = kv.pSpr->JxGiaiMaNen(kv.nFrame, kv.nBpp, (D3DFORMAT)kv.eFmt, kv.bPal != 0, kq) ? 0 : 1;
				QueryPerformanceCounter(&liB);
				{
					KAutoCriticalSection k(m_napKhoa);
					m_jxKhungXong.push_back(kq); m_pJxKhungDangChay = NULL; g_dJxNapNenBan += Rep3NapMs(liA, liB);
				}
				if (m_lNapDung)
					break;
				continue;
			}
#endif
			NapKetQua kq;
			memcpy(kq.szTen, v.szTen, sizeof(kq.szTen)); kq.uId = v.uId; kq.nType = v.nType;
			kq.pRes = NapNenTai(v.szTen, v.nType);
			{
				KAutoCriticalSection k(m_napKhoa);
				m_napXong.push_back(kq);
			}
			if (m_lNapDung)
				break;
		}
	}
}

bool TextureResMgr::NapNenGiao(const char* pszImage, uint32 uId, uint32 nType, bool bSau)	// [NAPNPC 09/09] bSau = hang sau (nap truoc)
{
	if (m_bNapNenLoi || !pszImage)
		return false;
	if (!m_hNapLuong)
	{
		m_hNapCo = CreateEventA(NULL, FALSE, FALSE, NULL);
		m_lNapDung = 0;
		unsigned uTid = 0;
		m_hNapLuong = m_hNapCo ? (HANDLE)_beginthreadex(NULL, 0, NapNenLuong, this, 0, &uTid) : NULL;
		if (!m_hNapLuong)
		{
			m_bNapNenLoi = true;
			if (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }
			Rep3Log("[REP3] nap nen: khong tao duoc luong -> nap ngay tren luong ve");
			return false;
		}
		SetThreadPriority(m_hNapLuong, THREAD_PRIORITY_BELOW_NORMAL);
		Rep3Log("[REP3] nap nen: luong nen da chay (Rep3NapNen=1)");
	}
	NapViec v;
	strncpy(v.szTen, pszImage, MAX_PATH - 1); v.szTen[MAX_PATH - 1] = 0; v.uId = uId; v.nType = nType;
	{
		KAutoCriticalSection k(m_napKhoa);
		if (bSau) m_napViecSau.push_back(v); else m_napViec.push_back(v);	// [NAPNPC 09/09]
	}
	m_nNapNenGui++;
	SetEvent(m_hNapCo);
	return true;
}

// [NAPCHIEU 09/09] Nap truoc mot anh o luong nen (luong ve/luong chinh goi, vd ngay khi nhan goi 95 'phong chieu').
// Giong duong chen muc cua GetImage khi bNapNen, nhung KHONG phu thuoc m_bVeDangDien va KHONG nap dong bo:
// nap truoc chi la goi y, khong giao duoc thi thoi (GetImage se nap nhu cu). Tra 1 = da co muc, 2 = da giao, 0 = khong.
int TextureResMgr::NapTruoc(const char* pszImage, uint32 nType, int nNguon)	// [NAPNPC 09/09] nNguon 1 = chieu, 2 = NPC; vao hang SAU
{
	if (!pszImage || !pszImage[0] || !g_nRep3NapNen)
		return 0;
	KAutoCriticalSection AutoLock(m_ImageProcessLock);
	const uint32 uImage = g_FileName2Id((LPSTR)pszImage);
	const int nIdx = FindImage(uImage, 0);	// >= 0: da co; < 0: -(vi tri chen)-1
	if (nIdx >= 0)
		return 1;	// da co (dang nap, da nap, hoac muc nap hong dang cho thu lai)
	if (!NapNenGiao(pszImage, uImage, nType, true))
		return 0;
	ResNode node;
	node.m_bDangNap = true;
	node.m_nNapTruoc = (unsigned char)((nNguon >= 1 && nNguon <= 2) ? nNguon : 0);	// [NAPCHIEU 09/09 b] [NAPNPC 09/09]
	node.m_bCacheable = true;
	node.m_nLastUsedTime = GetTickCount();
	node.m_nRetryTime = GetTickCount();
	node.m_nLanHong = 0;
	node.m_nType = nType;
	node.m_nID = uImage;
	node.m_pTextureRes = NULL;
	m_TextureResList.insert(m_TextureResList.begin() + (-nIdx - 1), node);
	return 2;
}

// Luong ve, dau moi khung (RepresentBegin): gan ket qua vao muc; BMP tao texture tai day (can device).
void TextureResMgr::NapNenNhan()
{
	vector<NapKetQua> xong;
	{
		KAutoCriticalSection k(m_napKhoa);
		if (m_napXong.empty())
			return;
		xong.swap(m_napXong);
	}
	KAutoCriticalSection AutoLock(m_ImageProcessLock);
	for (size_t i = 0; i < xong.size(); i++)
	{
		NapKetQua& kq = xong[i];
		int nIdx = FindImage(kq.uId, 0);
		if (nIdx < 0 || m_TextureResList[nIdx].m_nType != kq.nType || !m_TextureResList[nIdx].m_bDangNap)
		{	// muc da bi bo (CheckBalance/Free) hoac da nap dong bo trong luc cho -> bo ket qua
			if (kq.pRes) delete kq.pRes;
			continue;
		}
		ResNode& node = m_TextureResList[nIdx];
		node.m_bDangNap = false;
		TextureRes* pRes = kq.pRes;
		if (pRes && !pRes->NapNenHoanTat()) { delete pRes; pRes = NULL; }
		if (pRes)
		{
			node.m_pTextureRes = pRes;
			node.m_nLastUsedTime = GetTickCount();
			m_uTexCacheMemUsed += pRes->m_nTexMemUsed;
			m_nLoadCount++;
			m_nNapNenXong++;
			node.m_nLanHong = 0;	// [NAP 08/09 e]
		}
		else
		{
			node.m_pTextureRes = NULL;
			node.m_nRetryTime = GetTickCount();
			if (node.m_nLanHong < 255) node.m_nLanHong++;	// [NAP 08/09 e]
			m_nNapNenHong++;
			Rep3LogLoadFail(kq.szTen, kq.nType);
		}
	}
}

void TextureResMgr::NapNenDung()
{
	if (m_hNapLuong)
	{
		InterlockedExchange(&m_lNapDung, 1);
		if (m_hNapCo) SetEvent(m_hNapCo);
		WaitForSingleObject(m_hNapLuong, 5000);
		CloseHandle(m_hNapLuong); m_hNapLuong = NULL;
	}
	if (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }
	KAutoCriticalSection k(m_napKhoa);
	for (size_t i = 0; i < m_napXong.size(); i++)
		if (m_napXong[i].pRes) delete m_napXong[i].pRes;
	m_napXong.clear(); m_napViec.clear(); m_napViecSau.clear();	// [NAPNPC 09/09]
#ifdef JX_ANDROID
	for (size_t i = 0; i < m_jxKhungXong.size(); i++) if (m_jxKhungXong[i].pDiem) free(m_jxKhungXong[i].pDiem);	// [VE 11/09]
	for (size_t i = 0; i < m_jxKhungCho.size(); i++) if (m_jxKhungCho[i].pDiem) free(m_jxKhungCho[i].pDiem);
	m_jxKhungXong.clear(); m_jxKhungViec.clear(); m_jxKhungCho.clear(); m_pJxKhungDangChay = NULL;
#endif
	m_lNapDung = 0;
}

#ifdef JX_ANDROID
// ============================ [VE 11/09] nap KHUNG sprite o luong nen ============================
bool TextureResMgr::JxNapLuongBat()	// giong doan dau NapNenGiao (giu nguyen ham do cho Windows)
{
	if (m_bNapNenLoi)
		return false;
	if (m_hNapLuong)
		return true;
	m_hNapCo = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_lNapDung = 0;
	unsigned uTid = 0;
	m_hNapLuong = m_hNapCo ? (HANDLE)_beginthreadex(NULL, 0, NapNenLuong, this, 0, &uTid) : NULL;
	if (!m_hNapLuong)
	{
		m_bNapNenLoi = true;
		if (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }
		Rep3Log("[REP3] nap nen: khong tao duoc luong -> nap ngay tren luong ve");
		return false;
	}
	SetThreadPriority(m_hNapLuong, THREAD_PRIORITY_BELOW_NORMAL);
	Rep3Log("[REP3] nap nen: luong nen da chay (Rep3NapNen=1)");
	return true;
}

bool TextureResMgr::JxKhungXep(const JxKhungViec& v)
{
	if (!JxNapLuongBat())
		return false;
	{
		KAutoCriticalSection k(m_napKhoa);
		m_jxKhungViec.push_back(v);
		if ((unsigned)m_jxKhungViec.size() > g_uJxNapKhungChoMax) g_uJxNapKhungChoMax = (unsigned)m_jxKhungViec.size();
	}
	SetEvent(m_hNapCo);
	return true;
}

// Luong ve, dau khung (RepresentBegin, sau NapNenNhan): tao texture tu khung da giai ma, toi da NapKhungApMs ms; phan con lai de khung sau.
void TextureResMgr::JxNapKhungNhan()
{
	{
		KAutoCriticalSection k(m_napKhoa);
		if (!m_jxKhungXong.empty()) { m_jxKhungCho.insert(m_jxKhungCho.end(), m_jxKhungXong.begin(), m_jxKhungXong.end()); m_jxKhungXong.clear(); }
	}
	m_uJxApKhungCuoi = 0; m_dJxApCuoi = 0.0;
	if (m_jxKhungCho.empty())
		return;
	KAutoCriticalSection AutoLock(m_ImageProcessLock);
	LARGE_INTEGER li0, li1; QueryPerformanceCounter(&li0);
	const unsigned uNow = (unsigned)timeGetTime();
	size_t i = 0;
	for (; i < m_jxKhungCho.size(); i++)
	{
		JxKhungXong& kq = m_jxKhungCho[i];
		kq.pSpr->JxNhanKhungNen(kq);	// giai phong kq.pDiem
		m_uJxApKhungCuoi++;
		{ const double dTre = (double)(uNow - kq.uLuc); g_dJxNapKhungTre += dTre; if (dTre > g_dJxNapKhungTreMax) g_dJxNapKhungTreMax = dTre; }
		QueryPerformanceCounter(&li1);
		if (Rep3NapMs(li0, li1) >= (double)g_nJxNapKhungApMs) { i++; break; }
	}
	m_jxKhungCho.erase(m_jxKhungCho.begin(), m_jxKhungCho.begin() + i);
	QueryPerformanceCounter(&li1);
	m_dJxApCuoi = Rep3NapMs(li0, li1); g_dJxNapKhungAp += m_dJxApCuoi; if (m_dJxApCuoi > g_dJxNapKhungApMax) g_dJxNapKhungApMax = m_dJxApCuoi; g_uJxNapKhungApKhung += m_uJxApKhungCuoi;
}

// Luong ve (TextureResSpr::Release): sprite sap bi xoa -> bo viec chua chay, cho viec dang chay xong (vai ms), bo ket qua cua no.
// Luong nen KHONG bao gio giu m_ImageProcessLock nen cho o day khong ket.
void TextureResMgr::JxNapKhungHuy(TextureResSpr* p)
{
	if (!p)
		return;
	{
		KAutoCriticalSection k(m_napKhoa);
		for (size_t i = m_jxKhungViec.size(); i > 0; i--)
			if (m_jxKhungViec[i - 1].pSpr == p) m_jxKhungViec.erase(m_jxKhungViec.begin() + (i - 1));
	}
	while (m_pJxKhungDangChay == p)
		Sleep(1);
	{
		KAutoCriticalSection k(m_napKhoa);
		for (size_t i = m_jxKhungXong.size(); i > 0; i--)
			if (m_jxKhungXong[i - 1].pSpr == p) { if (m_jxKhungXong[i - 1].pDiem) free(m_jxKhungXong[i - 1].pDiem); m_jxKhungXong.erase(m_jxKhungXong.begin() + (i - 1)); }
	}
	for (size_t i = m_jxKhungCho.size(); i > 0; i--)
		if (m_jxKhungCho[i - 1].pSpr == p) { if (m_jxKhungCho[i - 1].pDiem) free(m_jxKhungCho[i - 1].pDiem); m_jxKhungCho.erase(m_jxKhungCho.begin() + (i - 1)); }
}
#endif
