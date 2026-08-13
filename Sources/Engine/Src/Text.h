/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2020-8-31
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef TEXT_H
#define TEXT_H

enum KTEXT_CTRL_CODE
{
	KTC_INVALID			=	0,
	KTC_ENTER			=	0x0a,
	KTC_COLOR			=	0x02,	//后面跟随三个BYTE数据分别为RGB三色分量
	KTC_COLOR_RESTORE	=	0x03,	//回复到原来设置的颜色
	KTC_BORDER_COLOR	=	0x04,	//设置边缘色，后面跟随三个BYTE数据分别为RGB三色分量
	KTC_BORDER_RESTORE	=	0x05,	//设置边缘色还原
	KTC_INLINE_PIC		=	0x06,	//嵌入式图片[wxb 2003-6-19]
	KTC_TAB				=	0x09,	//tab
	KTC_SPACE			=	0x20,	//空格
	KTC_TAIL			=	0xFF,	//字符串结束
};

struct KTP_CTRL
{
	unsigned char	cCtrl;	//此成员取值为Text.h中梅举KTEXT_CTRL_CODE中的一个值
	union
	{
	    struct
	    {
   			unsigned char cParam0;	//此及以下两个成员的取值与含义依据cCtrl的取值而定
		    unsigned char cParam1;
		    unsigned char cParam2;
	    };
	    unsigned short wParam;
   };
};

#define MAX_SYSTEM_INLINE_PICTURES	4096	//系统预留的嵌入式图片个数
struct IInlinePicEngineSink
{

	virtual long GetPicSize(unsigned short wIndex, int& cx, int& cy) = 0;

	virtual long DrawPic(unsigned short wIndex, int x, int y) = 0;

	virtual long AddCustomInlinePic(unsigned short& wIndex, const char* szSprPathName) = 0;

	virtual long RemoveCustomInlinePic(unsigned short wIndex) = 0;
};

#ifndef ENGINE_EXPORTS

//接口 IInlinePicEngineSink 由应用层实现并挂接进 Engine 模块 [wxb 2003-6-19]
//相关挂接函数:
// AdviseEngine(IInlinePicEngineSink*);
// UnAdviseEngine(IInlinePicEngineSink*);
extern "C"
{
	unsigned int TGetColor(const char* pColor);
	void TReplaceText(char* pBuffer, const char* pszKey, const char* pszText);
	int EGetBit(int nIntValue, int nBitNumber);
	int ESetBit(int nIntValue, int nBitNumber, int nBitValue);
	int EGetByte(int nIntValue, int nByteNumber);
	int ESetByte(int nIntValue, int nByteNumber, int nByteValue);
	const char* TGetSecondVisibleCharacterThisLine(const char* pCharacter, int nPos, int nLen);
	
	int TIsCharacterNotAlowAtLineHead(const char* pCharacter);

	const char* TGetLimitLenString(const char* pOrigString, int nOrigLen, char* pLimitLenString, int nLimitLen);

	const char* TGetLimitLenEncodedString(const char* pOrigString, int nOrigLen, int nFontSize,
		int nWrapCharaNum, char* pLimitLenString, int& nShortLen, int nLineLimit, int bPicPackInSingleLine = false);

	int	TSplitString(const char* pString, int nDesirePos, int bLess);

	int	TSplitEncodedString(const char* pString, int nCount, int nDesirePos, int bLess);

	int TGetEncodeStringLineHeadPos(const char* pBuffer, int nCount, int nLine, int nWrapCharaNum, int nFontSize, int bPicPackInSingleLine = false);

	int	TEncodeText(char* pBuffer, int nCount);

	int TFilterEncodedText(char* pBuffer, int nCount);

	int	TRemoveCtrlInEncodedText(char* pBuffer, int nCount);

	int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine = 0, int nLineLimit = 0, int bPicSingleLine = false);

	int	TFindSpecialCtrlInEncodedText(const char* pBuffer, int nCount, int nStartPos, char cControl);

	int	TClearSpecialCtrlInEncodedText(char* pBuffer, int nCount, char cControl);

	int TGetEncodedTextOutputLenPos(const char* pBuffer, int nCount, int& nLen, bool bLess, int nFontSize);

	int TGetEncodedTextEffectCtrls(const char* pBuffer, int nSkipCount, KTP_CTRL& Ctrl0, KTP_CTRL& Ctrl1);

	int TEnterTextFromCharArray(const char* pBuffer, char* szIntroEnter, int nCount);

	long AdviseEngine(IInlinePicEngineSink*);
	long UnAdviseEngine(IInlinePicEngineSink*);

	int	TGetItemChatLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int& nFace, int& nLastPos,
	int& nTotalLen, int nSkipLine = 0, int nLineLimit = 0, int bPicSingleLine = false );//itemchat
}

#endif

#endif
