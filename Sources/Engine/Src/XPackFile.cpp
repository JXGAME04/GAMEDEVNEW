/*****************************************************************************************
//	读取打包文件
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-9-16
*****************************************************************************************/
#include "KWin32.h"
#include "XPackFile.h"
#include "ucl/ucl.h"
#ifdef JX_APPLE	// [IOS-RUTKHUNG 12/09] ghi chan doan
#include <SDL3/SDL.h>
#endif
#include <crtdbg.h>
#ifdef JX_PLATFORM_SDL
// [SDL 08/09 2b-1b] CRITICAL_SECTION cua tep nay chua SDL_Mutex* o 8 byte dau; HANDLE m_hFile chua SDL_IOStream*
#undef InitializeCriticalSection
#undef DeleteCriticalSection
#undef EnterCriticalSection
#undef LeaveCriticalSection
#define InitializeCriticalSection(p)  (*(SDL_Mutex**)(p) = SDL_CreateMutex())
#define DeleteCriticalSection(p)      SDL_DestroyMutex(*(SDL_Mutex**)(p))
#define EnterCriticalSection(p)       SDL_LockMutex(*(SDL_Mutex**)(p))
#define LeaveCriticalSection(p)       SDL_UnlockMutex(*(SDL_Mutex**)(p))
static inline HANDLE XP_Open(const char* pszName)
{
#ifdef JX_POSIX
	char szPosix[1024]; pszName = JxPathPosix(pszName, szPosix, sizeof(szPosix));	// [ANDROID 08/09] \ -> /, ha chu thuong
#endif
	SDL_IOStream* p = SDL_IOFromFile(pszName, "rb"); return p ? (HANDLE)p : INVALID_HANDLE_VALUE;
}
static inline unsigned int XP_Size(HANDLE h) { Sint64 s = SDL_GetIOSize((SDL_IOStream*)h); return (s < 0) ? (unsigned int)INVALID_FILE_SIZE : (unsigned int)s; }
static inline bool XP_Read(HANDLE h, void* pBuf, unsigned int uLen) { return SDL_ReadIO((SDL_IOStream*)h, pBuf, uLen) == (size_t)uLen; }
static inline bool XP_ReadAt(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen) { return SDL_SeekIO((SDL_IOStream*)h, (Sint64)uOff, SDL_IO_SEEK_SET) == (Sint64)uOff && XP_Read(h, pBuf, uLen); }
static inline void XP_Close(HANDLE h) { SDL_CloseIO((SDL_IOStream*)h); }
#endif

#ifdef JX_POSIX
// [ANDROID 11/09 PAK] Bo dem KHOI cho DirectRead (chi Android). Moi khung sprite truoc day = 1 seek + 1 read xuong he thong tep
// (thu muc chia se cua may ao: ~0,3-0,5 ms/lan; dien thoai: mot syscall). Cac khung cua cung sprite nam ke nhau
// trong pak -> doc theo khoi 64 KB, giu 512 khoi (32 MB) LRU. Chi goi trong pham vi khoa ms_ReadCritical
// (DirectRead chi duoc goi tu GetSprFrame/GetSprHeader/ReadElemFile - deu da giu khoa) nen khong can khoa rieng.
// Tat de A/B: tao tep <thu muc du lieu>\jx_pak_khoi.off. Thong ke [PAK] moi 30 s ra logcat (SDL/APP).
#define XP_KHOI_CO	(64u * 1024u)
#define XP_KHOI_SO	512
struct XpKhoi { HANDLE h; unsigned int uKhoi; unsigned int uCo; unsigned int uDung; unsigned char* p; };
static XpKhoi		s_xpKhoi[XP_KHOI_SO];
static int			s_nXpKhoiSo = 0;		// so o da cap
static unsigned int	s_uXpTick = 0;			// dong ho LRU
static int			s_nXpBat = -1;			// -1 chua xet, 0 tat, 1 bat
static unsigned		s_uXpDoc = 0, s_uXpTrung = 0, s_uXpTruot = 0, s_uXpThang = 0, s_uXpDiaLan = 0;
static unsigned long long s_uXpDiaByte = 0;
static double		s_dXpDiaMs = 0.0;
static Uint64		s_uXpLogLuc = 0;

static void XP_KhoiBo(HANDLE h)	// pak dong: bo moi khoi cua handle (handle co the duoc cap lai cho pak khac)
{
	for (int i = 0; i < s_nXpKhoiSo; i++)
		if (s_xpKhoi[i].h == h) { s_xpKhoi[i].h = INVALID_HANDLE_VALUE; s_xpKhoi[i].uCo = 0; }
}

static bool XP_DocDia(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen)	// doc thang + do thoi gian
{
	Uint64 t0 = SDL_GetPerformanceCounter();
	bool bOk = XP_ReadAt(h, uOff, pBuf, uLen);
	Uint64 t1 = SDL_GetPerformanceCounter();
	s_uXpDiaLan++; s_uXpDiaByte += uLen;
	s_dXpDiaMs += (double)(t1 - t0) * 1000.0 / (double)SDL_GetPerformanceFrequency();
	return bOk;
}

static void XP_ThongKe()
{
	Uint64 uNay = SDL_GetTicks();
	if (s_uXpLogLuc == 0) { s_uXpLogLuc = uNay; return; }
	if (uNay - s_uXpLogLuc < 30000) return;
	unsigned uGiay = (unsigned)((uNay - s_uXpLogLuc) / 1000);
	SDL_Log("[PAK] %us: doc %u lan (trung khoi %u, truot %u, doc thang %u) | xuong dia %u lan %.1f MB %.1f ms | khoi dang giu %d x %u KB",
		uGiay, s_uXpDoc, s_uXpTrung, s_uXpTruot, s_uXpThang, s_uXpDiaLan, (double)s_uXpDiaByte / 1048576.0, s_dXpDiaMs, s_nXpKhoiSo, XP_KHOI_CO / 1024u);
	s_uXpLogLuc = uNay;
	s_uXpDoc = s_uXpTrung = s_uXpTruot = s_uXpThang = s_uXpDiaLan = 0; s_uXpDiaByte = 0; s_dXpDiaMs = 0.0;
}

// Lay khoi uKhoi cua handle h (doc tu dia neu chua co). Tra NULL neu doc hong.
static XpKhoi* XP_LayKhoi(HANDLE h, unsigned int uKhoi, unsigned int uFileSize)
{
	int i, nChon = -1;
	s_uXpTick++;
	for (i = 0; i < s_nXpKhoiSo; i++)
	{
		if (s_xpKhoi[i].h == h && s_xpKhoi[i].uKhoi == uKhoi && s_xpKhoi[i].uCo)
		{
			s_xpKhoi[i].uDung = s_uXpTick;
			s_uXpTrung++;
			return &s_xpKhoi[i];
		}
	}
	s_uXpTruot++;
	if (s_nXpKhoiSo < XP_KHOI_SO)
	{
		nChon = s_nXpKhoiSo;
		s_xpKhoi[nChon].p = (unsigned char*)malloc(XP_KHOI_CO);
		if (!s_xpKhoi[nChon].p)
			return NULL;
		s_nXpKhoiSo++;
	}
	else
	{	// LRU: o co uDung nho nhat; o trong (uCo = 0) uu tien
		for (i = 0; i < s_nXpKhoiSo; i++)
		{
			if (!s_xpKhoi[i].uCo) { nChon = i; break; }
			if (nChon < 0 || s_xpKhoi[i].uDung < s_xpKhoi[nChon].uDung) nChon = i;
		}
	}
	XpKhoi& k = s_xpKhoi[nChon];
	unsigned int uDau = uKhoi * XP_KHOI_CO;
	unsigned int uCo = (uFileSize > uDau) ? (uFileSize - uDau) : 0;
	if (uCo > XP_KHOI_CO) uCo = XP_KHOI_CO;
	k.h = INVALID_HANDLE_VALUE; k.uCo = 0;
	if (uCo == 0 || !XP_DocDia(h, uDau, k.p, uCo))
		return NULL;
	k.h = h; k.uKhoi = uKhoi; k.uCo = uCo; k.uDung = s_uXpTick;
	return &k;
}

static bool XP_DocKhoi(HANDLE h, unsigned int uOff, void* pBuf, unsigned int uLen, unsigned int uFileSize)
{
	if (s_nXpBat < 0)
	{
		char szTat[1024];
		s_nXpBat = (access(JxPathPosix("\\jx_pak_khoi.off", szTat, sizeof(szTat)), F_OK) == 0) ? 0 : 1;
		SDL_Log("[PAK] bo dem khoi doc pak: %s (%d x %u KB)", s_nXpBat ? "BAT" : "TAT (co jx_pak_khoi.off)", XP_KHOI_SO, XP_KHOI_CO / 1024u);
	}
	s_uXpDoc++;
	XP_ThongKe();
	if (!s_nXpBat || uLen > XP_KHOI_CO || uLen == 0)
	{
		s_uXpThang++;
		return XP_DocDia(h, uOff, pBuf, uLen);
	}
	unsigned char* pRa = (unsigned char*)pBuf;
	unsigned int uCon = uLen, uTai = uOff;
	while (uCon)
	{
		XpKhoi* k = XP_LayKhoi(h, uTai / XP_KHOI_CO, uFileSize);
		if (!k)
			return false;
		unsigned int uTrong = uTai - (uTai / XP_KHOI_CO) * XP_KHOI_CO;
		if (uTrong >= k->uCo)
			return false;		// ngoai cuoi tep
		unsigned int uLay = k->uCo - uTrong;
		if (uLay > uCon) uLay = uCon;
		memcpy(pRa, k->p + uTrong, uLay);
		pRa += uLay; uTai += uLay; uCon -= uLay;
	}
	return true;
}
#endif // JX_POSIX

//一个Pack文件具有的头结构:
struct XPackFileHeader
{
	unsigned char cSignature[4];		//四个字节的文件的头标志，固定为字符串'PACK'
	JX_ULONG uCount;				//数据的条目数
	JX_ULONG uIndexTableOffset;	//索引的偏移量
	JX_ULONG uDataOffset;			//数据的偏移量
	JX_ULONG uCrc32;				//校验和
	unsigned char cReserved[12];		//保留的字节
};

#define	XPACKFILE_SIGNATURE_FLAG		0x4b434150	//'PACK'

//Pack中对应每个子文件的索引信息项
struct XPackIndexInfo
{
	JX_ULONG	uId;				//子文件id
	JX_ULONG	uOffset;			//子文件在包中的偏移位置
	JX_LONG			lSize;				//子文件的原始大小
	JX_LONG			lCompressSizeFlag;	//子文件压缩后的大小和压缩方法
										//最高字节表示压缩方法，见XPACK_METHOD
										//低的三个字节表示子文件压缩后的大小
};

//包文件的压缩方式
enum XPACK_METHOD
{
	TYPE_NONE	= 0x00000000,			//没有压缩
	TYPE_UCL	= 0x01000000,			//UCL压缩
	TYPE_BZIP2	= 0x02000000,			//bzip2压缩
	TYPE_FRAME	= 0x10000000,			//使用了独立帧压缩,子文件为spr类型时才可能用到

	TYPE_METHOD_FILTER = 0x0f000000,	//过滤标记
	TYPE_FILTER = 0xff000000,			//过滤标记
};

//pak包中保存的spr帧信息项
struct XPackSprFrameInfo
{
	JX_LONG lCompressSize;
	JX_LONG lSize;
} ;

XPackFile::XPackElemFileCache	XPackFile::ms_ElemFileCache[MAX_XPACKFILE_CACHE];
int								XPackFile::ms_nNumElemFileCache = 0;
CRITICAL_SECTION				XPackFile::ms_ReadCritical;		// [NAP 08/09 c] khoa chung moi pak
long							XPackFile::ms_lReadCriticalInit = 0;

XPackFile::XPackFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_uFileSize = 0;
	m_pIndexList = NULL;
	m_nElemFileCount = 0;
	InitializeCriticalSection(&m_ReadCritical);
	if (InterlockedCompareExchange(&ms_lReadCriticalInit, 1, 0) == 0)	// [NAP 08/09 c] khoa chung khoi tao mot lan, khong xoa
		InitializeCriticalSection(&ms_ReadCritical);
}

XPackFile::~XPackFile()
{
	Close();
	DeleteCriticalSection(&m_ReadCritical);
}

//-------------------------------------------------
//功能：打开包文件
//返回：成功与否
//-------------------------------------------------
bool XPackFile::Open(const char* pszPackFileName, int nSelfIndex)
{
	bool bResult = false;
	Close();
	EnterCriticalSection(&ms_ReadCritical);
	m_nSelfIndex = nSelfIndex;
#ifdef JX_PLATFORM_SDL
	m_hFile = XP_Open(pszPackFileName);
#else
	m_hFile = ::CreateFile(pszPackFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
#endif
	while (m_hFile != INVALID_HANDLE_VALUE)
	{
#ifdef JX_PLATFORM_SDL
		m_uFileSize = XP_Size(m_hFile);
#else
		m_uFileSize = ::GetFileSize(m_hFile, NULL);
#endif
		if (m_uFileSize == 0 || m_uFileSize == INVALID_FILE_SIZE ||
			m_uFileSize <= sizeof(XPackFileHeader))
		{
			break;
		}
		XPackFileHeader	Header;
		DWORD			dwListSize, dwReaded;
		//--读取包文件头--
#ifdef JX_PLATFORM_SDL
		dwReaded = XP_Read(m_hFile, &Header, sizeof(Header)) ? (DWORD)sizeof(Header) : 0;
		if (dwReaded == 0)
#else
		if (::ReadFile(m_hFile, &Header, sizeof(Header), &dwReaded, NULL) == FALSE)
#endif
			break;
		//--包文件标记与内容的合法性判断--
		if (dwReaded != sizeof(Header) ||
			*(int*)(&Header.cSignature) != XPACKFILE_SIGNATURE_FLAG ||
			Header.uCount == 0 ||
			Header.uIndexTableOffset < sizeof(XPackFileHeader) ||
			Header.uIndexTableOffset >= m_uFileSize ||
			Header.uDataOffset < sizeof(XPackFileHeader) ||
			Header.uDataOffset >= m_uFileSize)
		{
			break;
		}

		//--读取索引信息表--
		dwListSize = sizeof(XPackIndexInfo) * Header.uCount;
		m_pIndexList = (XPackIndexInfo*)malloc(dwListSize);
#ifdef JX_PLATFORM_SDL
		if (m_pIndexList == NULL)
			break;
		dwReaded = XP_ReadAt(m_hFile, Header.uIndexTableOffset, m_pIndexList, dwListSize) ? dwListSize : 0;
#else
		if (m_pIndexList == NULL ||
			::SetFilePointer(m_hFile, Header.uIndexTableOffset, NULL, FILE_BEGIN) != Header.uIndexTableOffset)
		{
			break;
		}
		if (::ReadFile(m_hFile, m_pIndexList, dwListSize, &dwReaded, NULL) == FALSE)
			break;
#endif
		if (dwReaded != dwListSize)
			break;
		m_nElemFileCount = Header.uCount;
		bResult = true;
		break;
	};
	if (bResult == false)
		Close();
	LeaveCriticalSection(&ms_ReadCritical);
	return bResult;
}

//-------------------------------------------------
//功能：关闭包文件
//-------------------------------------------------
void XPackFile::Close()
{
	EnterCriticalSection(&ms_ReadCritical);

	if (m_pIndexList)
	{
		//----清除cache中缓存到的（可能）是此包中的子文件----
		for (int i = ms_nNumElemFileCache - 1; i >=0; i--)
		{
			if (ms_ElemFileCache[i].nPackIndex == m_nSelfIndex)
			{
				FreeElemCache(i);
				ms_nNumElemFileCache --;
				for (int j = i; j < ms_nNumElemFileCache; j++)
					ms_ElemFileCache[j] = ms_ElemFileCache[j + 1];
			}
		}
		free (m_pIndexList);
		m_pIndexList = NULL;
	}
	m_nElemFileCount = 0;

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
#ifdef JX_PLATFORM_SDL
#ifdef JX_POSIX
		XP_KhoiBo(m_hFile);	// [ANDROID 11/09 PAK] handle sap dong co the duoc cap lai cho pak khac
#endif
		XP_Close(m_hFile);
#else
		::CloseHandle(m_hFile);
#endif
		m_hFile = INVALID_HANDLE_VALUE;
	}
	m_uFileSize = 0;

	LeaveCriticalSection(&ms_ReadCritical);
}

//-------------------------------------------------
//功能：释放一个cache结点的内容
//返回：成功与否
//-------------------------------------------------
void XPackFile::FreeElemCache(int nCacheIndex)
{
	_ASSERT(nCacheIndex >= 0 && nCacheIndex < ms_nNumElemFileCache);
	if (ms_ElemFileCache[nCacheIndex].pBuffer)
	{
		free(ms_ElemFileCache[nCacheIndex].pBuffer);
		ms_ElemFileCache[nCacheIndex].pBuffer = NULL;
	}
	ms_ElemFileCache[nCacheIndex].uId = 0;
	ms_ElemFileCache[nCacheIndex].lSize = 0;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0;
	ms_ElemFileCache[nCacheIndex].nPackIndex = -1;
}

//-------------------------------------------------
//功能：直接读取包文件数据中的数据到缓冲区
//返回：成功与否
//-------------------------------------------------
bool XPackFile::DirectRead(void* pBuffer, unsigned int uOffset, unsigned int uSize) const
{
	bool bResult = false;
	DWORD dwReaded;
	_ASSERT(pBuffer && m_hFile != INVALID_HANDLE_VALUE);
#ifdef JX_PLATFORM_SDL
	dwReaded = 0;
#ifdef JX_POSIX
	if (uOffset + uSize <= m_uFileSize && XP_DocKhoi(m_hFile, uOffset, pBuffer, uSize, m_uFileSize))	// [ANDROID 11/09 PAK]
		bResult = true;
#else
	if (uOffset + uSize <= m_uFileSize && XP_ReadAt(m_hFile, uOffset, pBuffer, uSize))
		bResult = true;
#endif
#else
	if (uOffset + uSize <= m_uFileSize &&
		::SetFilePointer(m_hFile, uOffset, 0, FILE_BEGIN) == uOffset)
	{
		if (ReadFile(m_hFile, pBuffer, uSize, &dwReaded, NULL))
		{
			if (dwReaded == uSize)
				bResult = true;
		}
	}
#endif
	return bResult;
}

//-------------------------------------------------
//功能：带解压地读取包文件到缓冲区
//参数：pBuffer --> 缓冲区指针
//		uExtractSize  --> 数据（期望）解压后的大小，pBuffer缓冲区的大小不小于此数
//		lCompressType --> 直接从包中度取得原始（/压缩）大小
//		uOffset  --> 从包中的此偏移位置开始读取
//		uSize    --> 从包中直接读取得（压缩）数据的大小
//返回：成功与否
//-------------------------------------------------
bool XPackFile::ExtractRead_backup(void* pBuffer, unsigned int uExtractSize,
							long lCompressType, unsigned int uOffset, unsigned int uSize) const
{
	_ASSERT(pBuffer);
	bool bResult = false;
	if (lCompressType == TYPE_NONE)
	{
		if (uExtractSize == uSize)
			bResult = DirectRead(pBuffer, uOffset, uSize);
	}
	else
	{
		void*	pReadBuffer = malloc(uSize);
		if (pReadBuffer)
		{
		    if (lCompressType == TYPE_UCL && DirectRead(pReadBuffer, uOffset, uSize))
			{
				unsigned int uDestLength;
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);
				bResult =  (uDestLength == uExtractSize);
			}
			free (pReadBuffer);
		}
	}
	return bResult;
}

//edit by phong kieu load pak vng
bool XPackFile::ExtractRead(void* pBuffer, unsigned int uExtractSize,
							long lCompressType, unsigned int uOffset, unsigned int uSize) const
{
	_ASSERT(pBuffer);
	bool bResult = false;
	if (lCompressType == TYPE_NONE)
	{
		if (uExtractSize == uSize)
			bResult = DirectRead(pBuffer, uOffset, uSize);
	}
	else
	{
		void*	pReadBuffer = malloc(uSize);
		if (pReadBuffer)
		{
			//Load Pak VNG
		    if ((lCompressType == TYPE_UCL || lCompressType == 0x20000000) && DirectRead(pReadBuffer, uOffset, uSize))
			{
				unsigned int uDestLength;
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);
				bResult =  (uDestLength == uExtractSize);
			}
			free (pReadBuffer);
		}
	}
	return bResult;
}

//-------------------------------------------------
//功能：在索引表中查找子文件项(二分法找)
//返回：如找到返回在索引表中的位置(>=0)，如未找到返回-1
//-------------------------------------------------
int XPackFile::FindElemFile(unsigned long ulId) const
{
	int nBegin, nEnd, nMid;
	nBegin = 0;
	nEnd = m_nElemFileCount - 1;
	while (nBegin <= nEnd)
	{
		nMid = (nBegin + nEnd) / 2;
		if (ulId < m_pIndexList[nMid].uId)
			nEnd = nMid - 1;
		else if (ulId > m_pIndexList[nMid].uId)
			nBegin = nMid + 1;
		else
			break;
	}
	return ((nBegin <= nEnd) ? nMid : -1);
}

//-------------------------------------------------
//功能：查找包内的子文件
//参数：uId --> 子文件的id
//		ElemRef -->如果找到则在此结构里填上子文件的相关信息
//返回：是否找到
//-------------------------------------------------
bool XPackFile::FindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	ElemRef.nElemIndex = -1;
	if (uId)
	{
		EnterCriticalSection(&ms_ReadCritical);
		ElemRef.nCacheIndex = FindElemFileInCache(uId, -1);
		if (ElemRef.nCacheIndex >= 0)
		{
			ElemRef.uId = uId;
			ElemRef.nPackIndex = ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex;
			ElemRef.nElemIndex = ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex;
			ElemRef.nSize = ms_ElemFileCache[ElemRef.nCacheIndex].lSize;
			ElemRef.nOffset = 0;
		}
		else
		{
			ElemRef.nElemIndex = FindElemFile(uId);
			if (ElemRef.nElemIndex >= 0)
			{
				ElemRef.uId = uId;
				ElemRef.nPackIndex = m_nSelfIndex;
				ElemRef.nOffset = 0;
				ElemRef.nSize = m_pIndexList[ElemRef.nElemIndex].lSize;
			}
		}
		LeaveCriticalSection(&ms_ReadCritical);
	}
	return (ElemRef.nElemIndex >= 0);
}

//-------------------------------------------------
//功能：分配缓冲区，并读包内的子文件的内容到其中
//参数：子文件在包内的索引
//返回：成功则返回缓冲区的指针，否则返回空指针
//-------------------------------------------------
void* XPackFile::ReadElemFile(int nElemIndex) const
{
	_ASSERT(nElemIndex >= 0 && nElemIndex < m_nElemFileCount);
	void*	pDataBuffer = malloc(m_pIndexList[nElemIndex].lSize);
	if (pDataBuffer)
	{
		if (ExtractRead(pDataBuffer,
				m_pIndexList[nElemIndex].lSize,
				(m_pIndexList[nElemIndex].lCompressSizeFlag & TYPE_FILTER),
				m_pIndexList[nElemIndex].uOffset,
				(m_pIndexList[nElemIndex].lCompressSizeFlag & (~TYPE_FILTER))) == false)
		{
			free (pDataBuffer);
			pDataBuffer = NULL;
		}
	}
	return pDataBuffer;
}

//-------------------------------------------------
//功能：在cache里查找子文件
//参数：uId --> 子文件id
//		nDesireIndex --> 在cache中的可能位置
//返回：成功则返回cache节点索引(>=0),失败则返回-1
//-------------------------------------------------
int XPackFile::FindElemFileInCache(unsigned int uId, int nDesireIndex)
{
	if (nDesireIndex >= 0 && nDesireIndex < ms_nNumElemFileCache &&
		uId == ms_ElemFileCache[nDesireIndex].uId)
	{
		ms_ElemFileCache[nDesireIndex].uRefFlag = 0xffffffff;
		return nDesireIndex;
	}

	nDesireIndex = -1;
	for (int i = 0; i < ms_nNumElemFileCache; i++)
	{
		if (uId == ms_ElemFileCache[i].uId)
		{
			ms_ElemFileCache[i].uRefFlag = 0xffffffff;
			nDesireIndex = i;
			break;
		}
	}
	return nDesireIndex;
}

//-------------------------------------------------
//功能：把子文件数据添加到cache
//参数：pBuffer --> 存有子文件数据的缓冲区
//		nElemIndex --> 子文件在包中的索引
//返回：添加到cache的索引位置
//-------------------------------------------------
int XPackFile::AddElemFileToCache(void* pBuffer, int nElemIndex)
{
	_ASSERT(pBuffer && nElemIndex >= 0 && nElemIndex < m_nElemFileCount);
	int nCacheIndex;
	if (ms_nNumElemFileCache < MAX_XPACKFILE_CACHE)
	{	//找到一个空位置
		nCacheIndex = ms_nNumElemFileCache++;
	}
	else
	{	//释放一个旧的cache节点
		nCacheIndex = 0;
		if (ms_ElemFileCache[0].uRefFlag)
			ms_ElemFileCache[0].uRefFlag --;
		for (int i = 1; i < MAX_XPACKFILE_CACHE; i++)
		{
			if (ms_ElemFileCache[i].uRefFlag)
				ms_ElemFileCache[i].uRefFlag --;
			if (ms_ElemFileCache[i].uRefFlag < ms_ElemFileCache[nCacheIndex].uRefFlag)
				nCacheIndex = i;

		}
		FreeElemCache(nCacheIndex);
	}
	ms_ElemFileCache[nCacheIndex].pBuffer = pBuffer;
	ms_ElemFileCache[nCacheIndex].uId = m_pIndexList[nElemIndex].uId;
	ms_ElemFileCache[nCacheIndex].lSize = m_pIndexList[nElemIndex].lSize;
	ms_ElemFileCache[nCacheIndex].nPackIndex = m_nSelfIndex;
	ms_ElemFileCache[nCacheIndex].nElemIndex = nElemIndex;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0xffffffff;
	return nCacheIndex;
}

//-------------------------------------------------
//功能：读取子文件一定长度的数据到缓冲区
//参数：pBuffer --> 用来读取数据的缓冲区
//		uSize --> 要读取的数据的长度
//返回：成功读取得字节数
//-------------------------------------------------
int XPackFile::ElemFileRead(XPackElemFileRef& ElemRef, void* pBuffer, unsigned uSize)
{
	int nResult = 0;
	if (pBuffer && ElemRef.uId &&ElemRef.nElemIndex >= 0)
	{
		EnterCriticalSection(&ms_ReadCritical);

		//--先看是否已经在cache里了---
		ElemRef.nCacheIndex = FindElemFileInCache(ElemRef.uId, ElemRef.nCacheIndex);

		if (ElemRef.nCacheIndex < 0 &&								//在cache里未找到
			ElemRef.nElemIndex < m_nElemFileCount &&
			m_pIndexList[ElemRef.nElemIndex].uId == ElemRef.uId)
		{
			void*	pDataBuffer = ReadElemFile(ElemRef.nElemIndex);
			if (pDataBuffer)
				ElemRef.nCacheIndex = AddElemFileToCache(pDataBuffer, ElemRef.nElemIndex);
		}

		if (ElemRef.nCacheIndex >= 0 &&
			//此下面三项应该展开检查，防止被模块外部改变，引起错误。
			//为效率可考虑省略，但需外部按照规则随便改变ElemRef的内容。
			ElemRef.nPackIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex &&
			ElemRef.nElemIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex &&
			ElemRef.nSize == ms_ElemFileCache[ElemRef.nCacheIndex].lSize
			)
		{
			//_ASSERT(ElemRef.nPackIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex);
			//_ASSERT(ElemRef.nElemIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex);
			//_ASSERT(ElemRef.nSize == ms_ElemFileCache[ElemRef.nCacheIndex].lSize);

			if (ElemRef.nOffset < 0)
				ElemRef.nOffset = 0;
			if (ElemRef.nOffset < ElemRef.nSize)
			{
				if (ElemRef.nOffset + (int)uSize <= ElemRef.nSize)
					nResult = uSize;
				else
					nResult = ElemRef.nSize - ElemRef.nOffset;
				if ((unsigned)nResult > uSize)
					nResult = uSize;
				memcpy(pBuffer, (char*)ms_ElemFileCache[ElemRef.nCacheIndex].pBuffer + ElemRef.nOffset, nResult);
				ElemRef.nOffset += nResult;
			}
			else
			{
				ElemRef.nOffset = ElemRef.nSize;
			}
		}
		LeaveCriticalSection(&ms_ReadCritical);
	}
	return nResult;
}

#define	NODE_INDEX_STORE_IN_RESERVED	2
// [PAK 12/09 CHISOMUC] nua CAO cua chi so muc. Reserved[3] da bi khoa giai ma bang mau chiem
// (TextureRes.cpp:559) nen phai dung o [4]; [4] va [5] hien khong ai dung.
#define	NODE_INDEX_HI_IN_RESERVED	4

SPRHEAD* XPackFile::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	SPRHEAD*		pSpr = NULL;
	bool			bOk = false;

	pOffsetTable = NULL;
	if (ElemRef.uId == 0 || ElemRef.nElemIndex < 0)
		return NULL;

	EnterCriticalSection(&ms_ReadCritical);
	if(ElemRef.nElemIndex < m_nElemFileCount &&
		m_pIndexList[ElemRef.nElemIndex].uId == ElemRef.uId)
	{
        //首先检查这个id是什么类型压缩方式
		if ((m_pIndexList[ElemRef.nElemIndex].lCompressSizeFlag & TYPE_FRAME) == 0)
		{
			pSpr = (SPRHEAD*)ReadElemFile(ElemRef.nElemIndex);
			if (pSpr)
			{
				if ((*(int*)&pSpr->Comment[0]) == SPR_COMMENT_FLAG)
				{
					pOffsetTable = (SPROFFS*)(((char*)pSpr) + sizeof(SPRHEAD) + pSpr->Colors * 3);
					bOk = true;
				}
			}
		}
		else
		{
			SPRHEAD Header;
			if (DirectRead(&Header, m_pIndexList[ElemRef.nElemIndex].uOffset, sizeof(SPRHEAD)))
			{
				if (*(int*)&(Header.Comment[0]) == SPR_COMMENT_FLAG)
				{
					unsigned int	u2ListSize = Header.Colors * 3 + Header.Frames * sizeof(XPackSprFrameInfo);
					pSpr = (SPRHEAD*)malloc(sizeof(SPRHEAD) + u2ListSize);
					if (pSpr)
					{
						if (DirectRead((char*)(&pSpr[1]),
							m_pIndexList[ElemRef.nElemIndex].uOffset + sizeof(SPRHEAD),	u2ListSize))
						{
							memcpy(pSpr, &Header, sizeof(SPRHEAD));
							bOk = true;
						}
					}
				}
			}
		}

		if (pSpr)
		{
			if (bOk)
			{
				// [PAK 12/09 CHISOMUC] chi so muc CO THE vuot 65535 (mobile_13.pak co 70 602 muc) nen mot WORD
				// khong du: nua thap giu o Reserved[2] nhu cu, nua cao de o Reserved[4] (o trong).
				// KHONG dung int 32 bit o Reserved[2] vi se de len Reserved[3] = khoa giai ma bang mau.
				*((WORD*)&pSpr->Reserved[NODE_INDEX_STORE_IN_RESERVED]) = (WORD)(ElemRef.nElemIndex & 0xFFFF);
				pSpr->Reserved[NODE_INDEX_HI_IN_RESERVED] = (WORD)(((unsigned int)ElemRef.nElemIndex >> 16) & 0xFFFF);
			}
			else
			{
				free (pSpr);
				pSpr = NULL;
			}
		}
	}
	LeaveCriticalSection(&ms_ReadCritical);
    return pSpr;
}

SPRFRAME* XPackFile::GetSprFrame(SPRHEAD* pSprHeader, int nFrame)
{
	SPRFRAME*	pFrame = NULL;
#ifdef JX_APPLE	// [IOS-RUTKHUNG 12/09] rut khung tu pak that bai thi PrepareFrameData tra false -> GetImage tra NULL, KHONG bao gi. Ghi ro nhanh nao hong (toi da 12 dong).
	int nJxLyDo = 0;	// 1 = tham so xau, 2 = chi so nut ngoai khoang, 3 = khong co co TYPE_FRAME, 4 = doc/giai nen hong
	long lJxCo = 0; unsigned int uJxCo = 0, uJxNen = 0;
	if (!(pSprHeader && nFrame >= 0 && nFrame < pSprHeader->Frames)) nJxLyDo = 1;
#endif
	if (pSprHeader && nFrame >= 0 && nFrame < pSprHeader->Frames)
	{
		EnterCriticalSection(&ms_ReadCritical);
		// [PAK 12/09 CHISOMUC] ghep lai hai nua; pak duoi 65 536 muc thi nua cao = 0 -> y het truoc
		int nNodeIndex = (int)(*((WORD*)&pSprHeader->Reserved[NODE_INDEX_STORE_IN_RESERVED])
		                       | ((unsigned int)pSprHeader->Reserved[NODE_INDEX_HI_IN_RESERVED] << 16));
#ifdef JX_APPLE
		if (!(nNodeIndex >= 0 && nNodeIndex < m_nElemFileCount)) nJxLyDo = 2;
#endif
		if (nNodeIndex >= 0 && nNodeIndex < m_nElemFileCount)
		{
			long lCompressType = m_pIndexList[nNodeIndex].lCompressSizeFlag;
#ifdef JX_APPLE
			lJxCo = lCompressType;
			if ((lCompressType & TYPE_FRAME) == 0) nJxLyDo = 3;
#endif
			if ((lCompressType & TYPE_FRAME) != 0)
			{
				bool bOk = false;
				lCompressType &= TYPE_METHOD_FILTER;
				long	lTempValue = sizeof(SPRHEAD) + pSprHeader->Colors * 3;
				//读出指定帧的信息
				XPackSprFrameInfo* pFrameList = (XPackSprFrameInfo *)((char*)pSprHeader + lTempValue);
				unsigned long	uSrcOffset = m_pIndexList[nNodeIndex].uOffset + lTempValue +pSprHeader->Frames * sizeof(XPackSprFrameInfo);
			    while(nFrame > 0)
				{
					uSrcOffset += (pFrameList++)->lCompressSize;
					nFrame--;
				};
				lTempValue = pFrameList->lSize;
				if (lTempValue < 0)
				{
					lTempValue = -lTempValue;
					if (pFrame = (SPRFRAME*)malloc(lTempValue))
						bOk = DirectRead(pFrame, uSrcOffset, lTempValue);
				}
				else
				{
					if (pFrame = (SPRFRAME*)malloc(lTempValue))
					{
						bOk = ExtractRead(pFrame, lTempValue, lCompressType, uSrcOffset, pFrameList->lCompressSize);
					}
				}
#ifdef JX_APPLE
				uJxCo = (unsigned int)lTempValue; uJxNen = (unsigned int)pFrameList->lCompressSize;
				if (bOk == false) nJxLyDo = 4;
#endif
				if (bOk == false && pFrame != NULL)
				{
					free(pFrame);
					pFrame = NULL;
				}
			}
		}
		LeaveCriticalSection(&ms_ReadCritical);
	}
#ifdef JX_APPLE
	if (nJxLyDo)
	{
		static int s_nJxDem = 0;
		if (s_nJxDem < 12)
		{
			s_nJxDem++;
			static const char* s_szLyDo[5] = { "", "tham so xau", "chi so nut ngoai khoang", "khong co co TYPE_FRAME", "doc / giai nen hong" };
			SDL_Log("[RUTKHUNG] hong (%d/12): ly do %d = %s | co nen 0x%08lX | co that %u, co nen %u", s_nJxDem, nJxLyDo, s_szLyDo[nJxLyDo], lJxCo, uJxCo, uJxNen);
		}
	}
#endif
	return pFrame;	
}
