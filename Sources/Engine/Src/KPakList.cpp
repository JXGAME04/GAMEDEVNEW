//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
#include <windows.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "KPakList.h"
#include "crtdbg.h"

//---------------------------------------------------------------------------
ENGINE_API KPakList* g_pPakList = NULL;

//---------------------------------------------------------------------------
// 功能:	购造函数
//---------------------------------------------------------------------------
KPakList::KPakList()
{
	g_pPakList = this;
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// 功能:	分造函数
//---------------------------------------------------------------------------
KPakList::~KPakList()
{
	Close();
}

//---------------------------------------------------------------------------
// 功能:	关闭所有文件
//---------------------------------------------------------------------------
void KPakList::Close()
{
	for (int i = 0; i < m_nPakNumber; i++)
		delete m_PakFilePtrList[i];
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定文件
// 参数:	uId			文件名ID
//			ElemRef		用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	for (int i = 0; i < m_nPakNumber; i++)
	{
		if (m_PakFilePtrList[i]->FindElemFile(uId, ElemRef))
		{
			bFounded = true;
			break;
		}
	}
	return bFounded;
}

//---------------------------------------------------------------------------
// 功能:	把文件名转换为包中的id
// 参数:	pszFileName	文件名
// 返回:	文件名对应的包中的id
//---------------------------------------------------------------------------
unsigned long KPakList::FileNameToId(const char* pszFileName)
{
	_ASSERT(pszFileName && pszFileName[0]);
	JX_ULONG id = 0;	// [ANDROID 08/09] 32 bit nhu Windows: unsigned long LP64 (8 byte) khong tran -> id khac chi muc pak
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
		if(*ptr >= 'A' && *ptr <= 'Z') id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定文件
// 参数:	pszFileName	文件名
//			ElemRef	用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(const char* pszFileName, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
		// g_GetPackPath dung g_StrCpy/g_StrCat KHONG gioi han do dai; moi noi goi khac
		// deu cap MAX_PATH. De 128 thi duong dan tai nguyen dai >127 ky tu se tran stack
		// (ban debug bao "Stack around the variable 'szPackName' was corrupted").
		char szPackName[MAX_PATH];
		// [ANDROID 08/09] Ten trong CHI MUC pak luon kieu Windows (dau nguoc): id bam luc dong goi tren Windows.
		// Ban Android (JX_POSIX) truoc day di nhanh #else -> ghep dau xuoi -> id khac -> KHONG tra duoc tep nao
		// trong pak (chi tep roi tren dia mo duoc); font gbk_fs*.fnt chi nam trong pak nen mat het chu.
		#if defined(WIN32) || defined(JX_POSIX)
			szPackName[0] = '\\';
		#else
			szPackName[0] = '/';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);
		#ifdef JX_POSIX	// duong dan hien hanh tren POSIX dung dau xuoi -> doi lai cho khop chi muc
		for (char* pS = szPackName; *pS; pS++) if (*pS == '/') *pS = '\\';
		#endif
		unsigned long uId = FileNameToId(szPackName);
		bFounded = FindElemFile(uId, ElemRef);
#ifdef JX_MOBILE
		if (bFounded)
		{
			extern void JxTepDung_Ghi(char cLoai, unsigned int uId, const char* pszTen);	// [ANDROID 12/09 TEPDUNG] (KFile.cpp)
			JxTepDung_Ghi('P', (unsigned int)uId, szPackName);
		}
#endif
	}
	return bFounded;
}

//--------------------------------------------------------------------
// 功能:	Open package ini file
// 参数:	char* filename
// 返回:	BOOL
//---------------------------------------------------------------------------
bool KPakList::Open(const char* pPakListFile)
{
	Close();

	KIniFile IniFile;
	#define	SECTION "Package"

	bool bResult = false;
	if (IniFile.Load(pPakListFile))
	{
		char	szBuffer[32], szKey[16], szFile[MAX_PATH];

		if (IniFile.GetString(SECTION, "Path", "", szBuffer, sizeof(szBuffer)))
		{
			g_GetFullPath(szFile, szBuffer);
			int nNameStartPos = strlen(szFile);
			if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
			{
				#ifdef WIN32
					szFile[nNameStartPos++] = '\\';
				#else
					szFile[nNameStartPos++] = '/';
				#endif
				szFile[nNameStartPos] = 0;
			}

			for (int i = 0; i < MAX_PAK; i++)
			{
				_itoa(i, szKey, 10);
				if (!IniFile.GetString(SECTION, szKey, "", szBuffer, sizeof(szBuffer)))
					break;
				if (szBuffer[0] == 0)
					break;
				strcpy(szFile + nNameStartPos, szBuffer);
				m_PakFilePtrList[m_nPakNumber] = new XPackFile;
				if (m_PakFilePtrList[m_nPakNumber])
				{
					if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber))
					{
						m_nPakNumber++;
						g_DebugLog("PakList Open : %s ... Ok", szFile);
					}
					else
					{
						delete (m_PakFilePtrList[m_nPakNumber]);
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
}

//读取包内的子文件
int KPakList::ElemFileRead(XPackElemFileRef& ElemRef,
					void* pBuffer, unsigned uSize)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[ElemRef.nPackIndex]->ElemFileRead(ElemRef, pBuffer, uSize);
	return 0;
}

//读取spr文件头部或整个spr
SPRHEAD* KPakList::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return (m_PakFilePtrList[ElemRef.nPackIndex]->GetSprHeader(ElemRef, pOffsetTable));
	return NULL;
}

//读取按帧压缩的spr的一帧的数据
SPRFRAME* KPakList::GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame)
{
	if (nPackIndex >= 0 && nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[nPackIndex]->GetSprFrame(pSprHeader, nFrame);
	return NULL;
}
