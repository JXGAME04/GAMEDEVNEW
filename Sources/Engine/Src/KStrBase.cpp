//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KStrBase.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	String Utility Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include <string>
ENGINE_API int g_StrLen(LPCSTR lpStr)
{
	//#ifdef WIN32
	//	register int nLen;
	//
	//	__asm
	//	{
	//		mov		edi, lpStr
	//		mov		ecx, 0xffffffff
	//		xor al, al
	//		repne	scasb
	//		not ecx
	//		dec		ecx
	//		mov		nLen, ecx
	//	}
	//	return nLen;
	//#else
	return strlen(lpStr);
	//#endif
}
ENGINE_API int g_ExtractChar(const char* inStr, char scrchar, char destchar, char* outStr, char* outStra, int nMoedel)
{
	char* tmp = NULL, * tmpa = NULL;         //定义一个临时数组空间，存放字符；
	tmp = outStr, tmpa = outStra;
	int nLen = 0;
	while (*inStr != '\0')
	{//一直前行,直到有结束符号将停止。
		if (nMoedel == 0 && *inStr == scrchar)
		{//提取两个字符之间的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。
			while (*inStr != '\0')
			{//接着前行
				if (*inStr == destchar)   //数组中的第二个元素与结束字符相同
				{
					//*inStr='|';       //替换掉结束字符
					inStr++;            //过滤掉结束符，指向下个数。
					break;              //找到结束符号 过滤掉 跳出循环。
				}
				*tmpa = *inStr;           //提取字符放进tempa中
				tmpa++;	              //下一个元素
				inStr++;                //下一个元素
			}
			*tmpa = '\0';                  //提取完后，加个结束符号
		}
		else if (nMoedel == 1 && *inStr == scrchar)
		{//提取字符前面的内容 存放在 tmp中
			break;
		}
		else if (nMoedel == 2 && *inStr == scrchar)
		{//提取字符后面的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。
			while (*inStr != '\0')
			{//接着前行,继续提取后面的内容
				  /*if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {
					  //*inStr='|';       //替换掉结束字符
					  inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }*/
				*tmpa = *inStr;           //提取字符放进tempa中
				tmpa++;	              //下一个元素
				inStr++;                //下一个元素
			}
			*tmpa = '\0';                  //提取完后，加个结束符号
		}

		if (*inStr == '\0')                //如果没有找到结束符，并且已经到了结尾，就跳出了！！
			break;

		*tmp = *inStr;                     //元数不相同，则存放到tmp中。
		tmp++;                           //进行下一个元数的比较
		inStr++;
		nLen++;
	}
	*tmp = '\0';                           //源字符串 提取完后 加个结束符 防止乱码
	return nLen;
}

ENGINE_API std::string GbkToUtf8(const char* src_str)
{
	//ASSERT(src_str!=NULL);
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
	/*
	ASSERT(gb2312!=NULL);
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];//需要在外面析构，可以改成传指针进来的方式
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
	*/
}

ENGINE_API const char* Utf8ToGbk(const char* src_str, char* pcDes)
{
	/*int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
	*/
	enum { GB2312 = 936 };
	//unsigned long len;
	int len = -1;
	len = ::MultiByteToWideChar(CP_UTF8, NULL, src_str, -1, NULL, NULL);
	if (len == 0)
		return NULL;
	wchar_t* wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(CP_UTF8, NULL, src_str, -1, wide_char_buffer, len);
	len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		if (wide_char_buffer)
			delete[] wide_char_buffer;
		return NULL;
	}
	char* multi_byte_buffer = new char[len];
	::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);
	strcpy(pcDes, multi_byte_buffer);
	//std::string dest(multi_byte_buffer);
	if (multi_byte_buffer) delete[] multi_byte_buffer;
	if (wide_char_buffer) delete[] wide_char_buffer;

	return pcDes;
}

ENGINE_API UINT g_Atoui(LPSTR str)
{
	UINT result = 0, i = 0;

	char* tmp = NULL;

	for (i = 0; isspace(str[i]) && i < strlen(str); i++)//跳过空白符;
		;
	tmp = str + i;

	while (*tmp)
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}

	return result;
}
//---------------------------------------------------------------------------
// 函数:	StrEnd
// 功能:	返回字符串结尾指针
// 参数:	lpStr	:	字符串开头的指针
// 返回:	lpEnd	:	字符串末尾的指针
//---------------------------------------------------------------------------
ENGINE_API LPSTR g_StrEnd(LPCSTR lpStr)
{
	return (char*)lpStr + strlen(lpStr);
}
//---------------------------------------------------------------------------
// 函数:	StrCpy
// 功能:	字符串拷贝
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCpy(LPSTR lpDest, LPCSTR lpSrc)
{
	//#ifdef WIN32
	//	__asm
	//	{
	//		mov		edi, lpSrc
	//		mov		ecx, 0xffffffff
	//		xor al, al
	//		repne	scasb
	//		not ecx
	//		mov		edi, lpDest
	//		mov		esi, lpSrc
	//		mov		edx, ecx
	//		shr		ecx, 2
	//		rep		movsd
	//		mov		ecx, edx
	//		and ecx, 3
	//		rep		movsb
	//	};
	//#else
	strcpy(lpDest, lpSrc);
	//#endif
}
//---------------------------------------------------------------------------
// function: StrCpyLen
// Function: String copy, with a maximum length limit
// Parameters: lpDest : destination string
// lpSrc : source string
// nMaxLen : maximum length
// return: void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCpyLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	//#ifdef WIN32
	//	__asm
	//	{
	//		xor al, al
	//		mov		edx, nMaxLen
	//		dec		edx
	//		jg		copy_section
	//
	//		jl		finished
	//		mov		edi, lpDest
	//		stosb
	//		jmp		finished
	//
	//		copy_section :
	//		mov		edi, lpSrc
	//			mov		ecx, 0xffffffff
	//			repne	scasb
	//			not ecx
	//			dec		ecx
	//			cmp		ecx, edx
	//			jle		loc_little_equal
	//			mov		ecx, edx
	//
	//			loc_little_equal :
	//
	//		mov		edi, lpDest
	//			mov		esi, lpSrc
	//			mov		edx, ecx
	//			shr		ecx, 2
	//			rep		movsd
	//			mov		ecx, edx
	//			and ecx, 3
	//			rep		movsb
	//			stosb
	//
	//			finished :
	//	};
	//#else
	strncpy(lpDest, lpSrc, nMaxLen);
	lpDest[nMaxLen - 1] = '\0';
	//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCat
// 功能:	字符串末尾追加另一个字符串
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCat(LPSTR lpDest, LPCSTR lpSrc)
{
	register LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpy(lpEnd, lpSrc);
	//strcat(lpDest, lpSrc);
}
//---------------------------------------------------------------------------
// 函数:	StrCatLen
// 功能:	字符串末尾追加另一个字符串,有最大长度限制
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
//			nMaxLen	:	最大长度
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCatLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	register LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpyLen(lpEnd, lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrCmp
// 功能:	字符串比较
// 参数:	lpDest	:	字符串1
//			lpSrc	:	字符串2
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
ENGINE_API BOOL g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc)
{
	register int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nLen1 != nLen2)
		return FALSE;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nLen1);
}
//---------------------------------------------------------------------------
// 函数:	StrCmpLen
// 功能:	字符串比较,限定长度
// 参数:	lpDest	:	字符串1
//			lpSrc	:	字符串2
//			nLen	:	长度
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
ENGINE_API BOOL g_StrCmpLen(LPCSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	register int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nMaxLen > nLen1)
		nMaxLen = nLen1;
	if (nMaxLen > nLen2)
		nMaxLen = nLen2;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrUpper
// 功能:	小写字母转大写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrUpper(LPSTR lpDest)
{
	char* ptr = lpDest;
	while (*ptr) {
		if (*ptr >= 'a' && *ptr <= 'z') *ptr += 'A' - 'a';
		//          *ptr = toupper(*ptr);
		ptr++;
	}
}
//---------------------------------------------------------------------------
// 函数:	StrLower
// 功能:	大写字母转小写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrLower(LPSTR lpDest)
{
	char* ptr = lpDest;
	while (*ptr) {
		if (*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
		//          *ptr = tolower(*ptr);
		ptr++;
	}
}
//---------------------------------------------------------------------------
ENGINE_API void g_StrRep(LPSTR lpDest, LPSTR lpSrc, LPSTR lpRep)
{
	int		nSrcLen = g_StrLen(lpSrc);
	int		nDestLen = g_StrLen(lpDest);
	int		nMaxLen = nDestLen - nSrcLen + g_StrLen(lpRep) + 1;
	char* pStart = NULL;
	int i;
	for (i = 0; i < nDestLen - nSrcLen; i++)
	{
		if (g_StrCmpLen(&lpDest[i], lpSrc, nSrcLen))
			break;
	}
	if (i == nDestLen - nSrcLen)
		return;

	pStart = new char[nMaxLen];

	if (i != 0)
	{
		g_StrCpyLen(pStart, lpDest, i);
		g_StrCat(pStart, lpRep);
		g_StrCat(pStart, &lpDest[i + nSrcLen]);
	}
	else
	{
		g_StrCpy(pStart, lpRep);
		g_StrCat(pStart, &lpDest[nSrcLen]);
	}
	g_StrCpy(lpDest, pStart);
	if (pStart)
	{
		delete[] pStart;
		pStart = NULL;
	}
}