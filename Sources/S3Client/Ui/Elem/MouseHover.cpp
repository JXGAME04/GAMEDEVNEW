ï»¿/*****************************************************************************************
//	æµ®å¨æç¤ºçª—å£
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "MouseHover.h"
#include "AutoLocateWnd.h"
#include "WndWindow.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Engine/Src/Text.h"
#include "SpecialFuncs.h"
extern iRepresentShell*	g_pRepresentShell;

#include "../../../core/src/CoreShell.h"
extern iCoreShell*	g_pCoreShell;

KMouseOver	g_MouseOver;
KMouseOver	g_MouseOverCompare;

#define	SCHEME_INI				"UiMouseHover.ini"
#define	INFO_MIN_LEN			26
#define FOLLOW_CURSOR_OFFSET_X	16
#define FOLLOW_CURSOR_OFFSET_Y	8

static unsigned int	s_uHoverObjDestTextColor = 0xffffffff;	//å¯¹è±¡æç¤ºæ–‡å­—ç„é¢œè‰²

void SetHoverObjDescColor(unsigned int uColor)
{
	s_uHoverObjDestTextColor = uColor;
}

void SetMouseHoverObjectDesc(void* pWnd, int nObj, unsigned int uGenre,
			unsigned int uId, int nContainer, int x, int y)
{
	KGameObjDesc	Desc;
	KGameObjDesc	DescCompare;
	int		nLenTitle = 0, nLenProp = 0, nLenDesc = 0;
	
	g_MouseOver.CancelMouseHoverInfo();
	g_MouseOverCompare.CancelMouseHoverInfo();
	if (g_pCoreShell)
	{
		KUiObjAtContRegion	Obj;
		KUiObjAtContRegion	ObjCompare;

		Obj.Obj.uGenre = uGenre;
		Obj.Obj.uId = uId;
		Obj.Region.h = Obj.Region.v = 0;
		Obj.Region.Width = Obj.Region.Height = 0;
		Obj.nContainer = nContainer;

		ObjCompare.Obj.uGenre = uGenre;
		ObjCompare.Region.h = Obj.Region.v = 0;
		ObjCompare.Region.Width = Obj.Region.Height = 0;
		ObjCompare.nContainer = nContainer;

		Desc.szDesc[0] = 0;
		Desc.szProp[0] = 0;
		Desc.szTitle[0] = 0;

		DescCompare.szDesc[0] = 0;
		DescCompare.szProp[0] = 0;
		DescCompare.szTitle[0] = 0;
		unsigned uIndex = GDI_GAME_OBJ_DESC;
		if (g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO;
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_LOCKITEM)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO;
		else if (g_UiBase.GetStatus() != UIS_S_IDLE)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO;

		g_pCoreShell->GetGameData(uIndex, (unsigned int)&Obj, (int)&Desc);

		g_MouseOver.SetMouseHoverInfo(pWnd, nObj, x, y, true, false);
		if (Desc.szTitle[0])
		{
			nLenTitle = TEncodeText(Desc.szTitle, strlen(Desc.szTitle));
			g_MouseOver.SetMouseHoverTitle(Desc.szTitle, nLenTitle,s_uHoverObjDestTextColor);
		}
		if (Desc.szProp[0])
		{
			nLenProp = TEncodeText(Desc.szProp, strlen(Desc.szProp));
			g_MouseOver.SetMouseHoverProp(Desc.szProp, nLenProp, s_uHoverObjDestTextColor);
		}
		if (Desc.szDesc[0])
		{
			nLenDesc = TEncodeText(Desc.szDesc, strlen(Desc.szDesc));
			g_MouseOver.SetMouseHoverDesc(Desc.szDesc, nLenDesc, s_uHoverObjDestTextColor);
		}
		if (nContainer == UOC_ITEM_TAKE_WITH) {
			int compareIdx = 0;
			compareIdx = g_pCoreShell->GetGameData(GDI_ITEM_EQUIP_SAME_GERNE, NULL, Obj.Obj.uId);
			if (compareIdx != -1) {
				ObjCompare.Obj.uId = compareIdx;
				g_pCoreShell->GetGameData(uIndex, (unsigned int)&ObjCompare, (int)&DescCompare);
				g_MouseOverCompare.SetMouseHoverInfo(pWnd, nObj, x, y, true, false);
				if (DescCompare.szTitle[0])
				{
					char tmp[GOD_MAX_OBJ_TITLE_LEN];
					sprintf_s(tmp, "[Trang b\xde]%s", DescCompare.szTitle);
					nLenTitle = TEncodeText(tmp, strlen(tmp));
					g_MouseOverCompare.SetMouseHoverTitle(tmp, nLenTitle, s_uHoverObjDestTextColor);
				}
				if (DescCompare.szProp[0])
				{
					nLenProp = TEncodeText(DescCompare.szProp, strlen(DescCompare.szProp));
					g_MouseOverCompare.SetMouseHoverProp(DescCompare.szProp, nLenProp, s_uHoverObjDestTextColor);
				}
				if (DescCompare.szDesc[0])
				{
					nLenDesc = TEncodeText(DescCompare.szDesc, strlen(DescCompare.szDesc));
					g_MouseOverCompare.SetMouseHoverDesc(DescCompare.szDesc, nLenDesc, s_uHoverObjDestTextColor);
				}
			}
		}
	}	
}

int DrawDraggingGameObjFunc(int x, int y, const KUiDraggedObject& Obj, int nDropQueryResult)
{
	g_pCoreShell->DrawGameObj(Obj.uGenre, Obj.uId, x, y, 0, 0, 0);
	return false;
}

KMouseOver::KMouseOver()
{
	m_nImgWidth  = 0;
	m_nImgHeight = 0;
	m_nLeft      = 0;
	m_nTop       = 0;
	m_nWndWidth  = 0;
	m_nWndHeight = 0;
	m_nIndent    = 0;
	m_nFontSize  = 12;
	m_pMouseHoverWnd = NULL;
	m_nObj = 0;
	m_ObjTitle[0]  =0;
	m_nTitleLen    = 0;
	m_ObjProp[0]   = 0;
	m_nPropLen     = 0;
	m_ObjDesc[0]   = 0;
	m_nDescLen     = 0;
	m_uTitleColor  = 0;
	m_uPropColor   = 0;
	m_uDescColor   = 0;
	m_uTitleBgColor= 0;
	m_uPropBgColor = 0;
	m_uDescBgColor = 0;
	memset(&m_Image, 0, sizeof(KRUImage));
	m_bHeadTailImg = true;
	m_bFollowCursor = true;
	m_bShow = false;
}

int  KMouseOver::IsMoseHoverWndObj(void* pWnd, int nObj)
{
	return ((pWnd == m_pMouseHoverWnd) && (nObj == m_nObj));
}

void KMouseOver::CancelMouseHoverInfo()
{
	m_pMouseHoverWnd = 0;
	m_nObj = 0;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_ObjProp[0] = 0;
	m_nPropLen = 0;
	m_ObjDesc[0] = 0;
	m_nDescLen = 0;
	m_bShow = false;
}

/***********************************************************************
*åŸèƒ½ï¼è®¾ç½®é¼ æ ‡æç¤ºçª—å£ç„åŸºæœ¬ä¿¡æ¯
************************************************************************/
void KMouseOver::SetMouseHoverInfo(void* pWnd, int nObj, int x, int y,
		                        bool bHeadTailImg, bool bFollowCursor)
{
    m_pMouseHoverWnd = pWnd;
	m_nObj = nObj;
	m_bHeadTailImg = bHeadTailImg;
	m_bFollowCursor = bFollowCursor;
	m_nApplyX = x;
	m_nApplyY = y;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_ObjProp[0] = 0;
	m_nPropLen = 0;
	m_ObjDesc[0] = 0;
	m_nDescLen = 0;
	m_bShow = false;
}


/***********************************************************************
*åŸèƒ½ï¼è®¾ç½®é¼ æ ‡æç¤ºçª—å£ç„æ ‡é¢˜(ç‰©ä»¶ç„åå­—)
************************************************************************/
void KMouseOver::SetMouseHoverTitle(const char *pTitleText, int nTitleTextLen, UINT uColor)
{
	if (nTitleTextLen > 0 && pTitleText && nTitleTextLen <= GOD_MAX_OBJ_TITLE_LEN)
	{
		memcpy(m_ObjTitle, pTitleText, nTitleTextLen);
		m_nTitleLen = nTitleTextLen;
		m_uTitleColor = uColor;
		const char* prefix = "[Trang ";
		if(std::strncmp(pTitleText, prefix, std::strlen(prefix)) == 0)
			Update(m_nApplyX, m_nApplyY, true);
		else
			Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjTitle[0] = 0;
		m_nTitleLen = 0;
	}
}


/***********************************************************************
*åŸèƒ½ï¼è®¾ç½®é¼ æ ‡æç¤ºçª—å£ç„ç‰©ä»¶å±æ€§
************************************************************************/
void KMouseOver::SetMouseHoverProp(const char *pPropText, int nPropTextLen, UINT uColor)
{
	if (nPropTextLen > 0 && pPropText && nPropTextLen <= GOD_MAX_OBJ_PROP_LEN)
	{
		memcpy(m_ObjProp, pPropText, nPropTextLen);
		m_nPropLen = nPropTextLen;
		m_uPropColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjProp[0] = 0;
		m_nPropLen = 0;
	}
}


/***********************************************************************
*åŸèƒ½ï¼è®¾ç½®é¼ æ ‡æç¤ºçª—å£ç„ç‰©ä»¶è¯´æ˜
************************************************************************/
void KMouseOver::SetMouseHoverDesc(const char *pDescText, int nDescTextLen, UINT uColor)
{
	if (nDescTextLen > 0 && pDescText && nDescTextLen <= GOD_MAX_OBJ_DESC_LEN)
	{
		memcpy(m_ObjDesc, pDescText, nDescTextLen);
		m_nDescLen = nDescTextLen;
		m_uDescColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjDesc[0] = 0;
		m_nDescLen = 0;
	}
}


void KMouseOver::Update(int nX, int nY, bool compare /* = false*/)
{
	m_bShow = false;

	if (g_pRepresentShell == NULL)
		return;

	int nMaxTitleLen, nMaxPropLen, nMaxDescLen;

	m_nMaxLineLen = 0;
	if (m_nTitleLen > 0)
	{
		m_nTitleLineNum = TGetEncodedTextLineCount(
			m_ObjTitle, m_nTitleLen, 0, nMaxTitleLen, m_nFontSize);
		m_nMaxLineLen = nMaxTitleLen;
	}
	else
	{
		m_nTitleLineNum = 0;
	}
	if (m_nPropLen > 0)
	{
		m_nPropLineNum = TGetEncodedTextLineCount(
			m_ObjProp, m_nPropLen, 0, nMaxPropLen, m_nFontSize);
		if(m_nMaxLineLen < nMaxPropLen + 3)	//æ­¤+3ç”¨äºæ‰©å±•tabå­—ç¬¦å ç„ç©ºé—´
			m_nMaxLineLen = nMaxPropLen + 3;
	}
	else
	{
		m_nPropLineNum = 0;
	}
	if (m_nDescLineNum > 0)
	{
		m_nDescLineNum = TGetEncodedTextLineCount(
			m_ObjDesc, m_nDescLen, 0, nMaxDescLen, m_nFontSize);
		if(m_nMaxLineLen < nMaxDescLen)
		    m_nMaxLineLen = nMaxDescLen;
	}
	else
	{
		m_nDescLineNum = 0;
	}


	int nNumLine = m_nTitleLineNum + m_nPropLineNum + m_nDescLineNum;
	if (nNumLine == 0)
		return;

	if (m_bFollowCursor == false && m_nMaxLineLen < INFO_MIN_LEN)
		m_nMaxLineLen = INFO_MIN_LEN;
	m_nWndWidth = m_nFontSize * m_nMaxLineLen / 2 + m_nIndent * 2;
	m_nWndHeight = (m_nFontSize + 1) * nNumLine;
	if (m_bHeadTailImg)
		m_nWndHeight += m_nImgHeight * 2;
	if (m_bFollowCursor)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
	else
	{
		ALW_GetWndPosition(m_nLeft, m_nTop, m_nWndWidth, m_nWndHeight, false, true);
		if (compare) {
			m_nLeft -= 320;
		}
	}

	//å¯ä»¥æ˜¾ç¤ºæµ®å¨çª—å£å•¦!
	m_bShow = true;
}


//é¼ æ ‡ç„ä½ç½®æ›´æ–°äº†
void KMouseOver::UpdateCursorPos(int nX, int nY)
{
	if (m_bFollowCursor && m_bShow)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
}


void KMouseOver::OnWndClosed(void* pWnd)
{
	if (pWnd && pWnd == m_pMouseHoverWnd)
		CancelMouseHoverInfo();
}

//è½½å…¥ç•Œé¢æ–¹æ¡ˆ
void KMouseOver::LoadScheme(const char* pScheme)
{
	if (pScheme == NULL)
		return;
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Ini.GetInteger("Main", "ImgWidth",    0, &m_nImgWidth);
		Ini.GetInteger("Main", "ImgHeight",   0, &m_nImgHeight);
		Ini.GetInteger("Main", "Indent",      0, &m_nIndent);
		Ini.GetInteger("Main", "Font",        0, &m_nFontSize);

		if(m_nImgWidth < 0)
			m_nImgWidth = 0;
		if(m_nImgHeight < 0)
			m_nImgWidth = 0;
		if(m_nFontSize < 8)
			m_nFontSize = 8;
		int nValue;
		Ini.GetInteger("Main", "ImgType", 0, &nValue);
		if (nValue == 1)
		{
			m_Image.nType = ISI_T_BITMAP16;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		}
		else
		{
			m_Image.nType = ISI_T_SPR;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_Image.Color.Color_b.a = 255;
		}
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
		Ini.GetString("Main", "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		Ini.GetInteger("Main", "Frame", 0, &nValue);
		m_Image.nFrame = nValue;

		Ini.GetString("Main", "TitleBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uTitleBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Ini.GetString("Main", "PropBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uPropBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Ini.GetString("Main", "DescBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uDescBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Update((m_nLeft + m_nWndWidth) / 2, m_nTop);
	}
}


void KMouseOver::PaintMouseHoverInfo()
{
	if (m_bShow == false || g_pRepresentShell == NULL)
		return;

	//ç”»èƒŒæ™¯é˜´å½±å’Œè¾¹æ¡†
	KRUShadow Shadow;
	//å†™æ–‡å­—åˆå§‹åŒ–
	KOutputTextParam	Param;
	Param.BorderColor = 0;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	//====æ ‡é¢˜(åå­—)éƒ¨åˆ†====
	Shadow.oPosition.nX = m_nLeft;
	Shadow.oPosition.nY = m_nTop;
	if (m_bHeadTailImg)
		Shadow.oPosition.nY = m_nTop + m_nImgHeight;
	else
		Shadow.oPosition.nY = m_nTop;
	Shadow.oEndPos.nX = m_nLeft + m_nWndWidth;
	Shadow.oEndPos.nY = Shadow.oPosition.nY;
	if(m_nTitleLen > 0)
	{
		Shadow.Color.Color_dw = m_uTitleBgColor;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nTitleLineNum;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.nNumLine = 1;
		Param.Color = m_uTitleColor;
		Param.nY = Shadow.oPosition.nY;
		int nLineLen;
		while(true)
		{
			if (TGetEncodedTextLineCount(m_ObjTitle, m_nTitleLen, 0, nLineLen, m_nFontSize, Param.nSkipLine, 1) == 0)
				break;
			Param.nX = m_nLeft + m_nWndWidth / 2 - (nLineLen * m_nFontSize) / 4;
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjTitle, m_nTitleLen, 0);
			Param.nSkipLine ++;
			Param.nY += m_nFontSize + 1;
		};
	}

	//====å±æ€§éƒ¨åˆ†====
	if(m_nPropLen > 0)
	{
		Shadow.Color.Color_dw = m_uPropBgColor;
		Shadow.oPosition.nY = Shadow.oEndPos.nY;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nPropLineNum;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.Color = m_uPropColor;
		Param.nNumLine = m_nPropLineNum;
		Param.nX = m_nLeft + m_nIndent;
		Param.nY = Shadow.oPosition.nY;
		OutputTabSplitText(m_ObjProp, m_nPropLen, m_nWndWidth - m_nIndent * 2, m_nFontSize, &Param);
	}

	//====æè¿°éƒ¨åˆ†====
	if(m_nDescLen > 0)
	{
		Shadow.Color.Color_dw = m_uDescBgColor;
		Shadow.oPosition.nY = Shadow.oEndPos.nY;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nDescLineNum;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		
		Param.nSkipLine = 0;
		Param.Color = m_uDescColor;
		Param.nNumLine = m_nDescLineNum;
		Param.nX = m_nLeft + m_nIndent;
		Param.nY = Shadow.oPosition.nY;
		g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjDesc, m_nDescLen, 0);
	}

	//ç”»åº•å›¾å’Œè¾¹æ¡†å›¾
	if (m_bHeadTailImg && m_nImgWidth > 0)
	{
		m_Image.oPosition.nX = m_nLeft;
		while(m_Image.oPosition.nX < m_nLeft + m_nWndWidth)
		{			
			m_Image.oPosition.nY = m_nTop;
			g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nY = Shadow.oEndPos.nY;
			g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nX += m_nImgWidth;
		};
	}
}


/***********************************************************************
*åŸèƒ½ï¼æŒ‰æ—¢å®ç„æœ€å¤§å®½åº¦æå±æ€§éƒ¨åˆ†æ ¼å¼åŒ–(å·¦å³é )
************************************************************************/
/*void KMouseOver::FormatProp()
{
    if(m_ObjProp[0])
	{
		char szBuffer[MAX_OBJ_PROP_LEN], *pPos = NULL, *pHead = NULL, *pTail = NULL;
	    int nLeftLen = 0, nRightLen = 0, i, j, k;
        //åˆå§‹å‡†å¤‡
		memset(szBuffer, 0, MAX_OBJ_PROP_LEN);
		memcpy(szBuffer, m_ObjProp, m_nPropLen);
		memset(m_ObjProp, 0, MAX_OBJ_PROP_LEN);
	    szBuffer[MAX_OBJ_PROP_LEN - 1] = 0;
		pHead = szBuffer;
		//å¼€å§‹å¾ªç¯å¤„ç†
        while(pHead[0])
	    {
            pPos = strchr(pHead, 0x20);
			if(pPos == NULL) break;
			nLeftLen = pPos - pHead;

			pTail = strchr(pPos, 0x0a);
			if(pTail == NULL)
			{
				pTail = strlen(pPos) + pPos;
				if((pTail - pPos) <= 1)
				    break;
			}
			nRightLen = pTail - pPos - 1;

			*pPos = *pTail = 0;
			strcat(m_ObjProp, pHead);
			j = m_nMaxLineLen - nLeftLen - nRightLen;
			k = strlen(m_ObjProp);
			for(i=0;i < j;i++)
			{
				m_ObjProp[k] = ' ';
				k++;
			}
			m_ObjProp[k] = 0;
			strcat(m_ObjProp, pPos + 1);
			k = strlen(m_ObjProp);
			m_ObjProp[k] = 0x0a;
			m_ObjProp[k+1] = 0;

			pHead = pTail + 1;
			pPos =  NULL;
			pTail = NULL;
	    };

		j = strlen(m_ObjProp);
		m_ObjProp[j] = 0;
	    m_nPropLen = strlen(m_ObjProp);
    }
}*/
