/*****************************************************************************************
//	½çÃæ´°¿ÚÌåÏµ½á¹¹--ÎÄ±¾´°¿Ú
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-23
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndText.h"
#include "../../../Engine/Src/Text.h"
#include "GameDataDef.h"
#include <time.h>

#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
KWndText::KWndText()
{
	m_nFontSize = 12;
	m_pText = NULL;
	m_nTextLen   = 0;
	m_nBuffLen   = 0;
	m_nTopLine   = 0;
	m_nLineCount = 0;
	m_BorderColor = 0;
}

void KWndText::Clear(bool bSafe)
{
	if (m_pText)
	{
		m_nTextLen = 0;
		if (bSafe == false)
			m_pText[0] = 0;
		else
			memset(m_pText, 0, m_nTextLen + 1);
	}
}

void KWndText::SetText(const char* pText, int nLen/*= -1*/)
{
	if (m_pText && g_pRepresentShell)
	{
		if (pText && pText[0])
		{
			if (nLen < 0)
				nLen = strlen(pText);
			if (nLen > m_nBuffLen)
				nLen = TSplitString(pText, m_nBuffLen - 1, true);
			memcpy(m_pText, pText, nLen);
			m_pText[nLen] = 0;
			m_nTextLen = nLen;
			if (m_Style & WNDTEXT_ES_MULTILINE)
			{
				int nLineLen;
				m_nLineCount = TGetEncodedTextLineCount(
					m_pText, m_nTextLen, (m_Width * 2) / m_nFontSize, nLineLen, m_nFontSize);
			}
		}
		else
		{
			m_pText[0] = 0;
			m_nTextLen = 0;
			m_nLineCount = 0;
		}
		m_nTopLine = 0;
	}
}

//--------------------------------
// hien thi trung sinh
void KWndText::Set4IntText(int nNumber, char Separator)
{
	char	Buff[64];
	int		nLen;
	if (m_pText)
	{
		if (Separator)
			nLen = sprintf(Buff, "%d%c", nNumber, Separator);
		else
		{
			
			if (nNumber == 0)
			nLen = sprintf(Buff, "Ch­a trïng sinh");
			else
				nLen = sprintf(Buff, "Trïng sinh %d lÇn", nNumber);
				
		}
		//SetText(Buff, nLen);
	}
}

int KWndText::GetText(char* pBuffer, int nSize)
{
	int nRet = 0;
	if (m_pText && pBuffer)
	{
		if (m_nTextLen + 1 <= nSize)
		{
			memcpy(pBuffer, m_pText, m_nTextLen + 1);
			nRet = m_nTextLen;
		}
		else
			nRet = m_nTextLen + 1;
	}
	return nRet;
}

int	KWndText::SetTopLine(int nTopLine)
{
	if (nTopLine >= 0 && nTopLine < m_nLineCount)
		m_nTopLine = nTopLine;
	return 0;
}

void KWndText::SetTextPtr(char* pText, int nMaxLen)
{
	m_pText = pText;
	m_nBuffLen = nMaxLen;
	m_nTextLen = 0;
	m_nTopLine = 0;
	m_nLineCount = 0;
}

void KWndText::Clone(KWndText* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_TextColor = m_TextColor;
		pCopy->m_BorderColor = m_BorderColor;
	}
}

int KWndText::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 12, &m_nFontSize);
		m_Style &= ~WNDTEXT_ES_ALIGN_FILTER;
		int	nValue;
		pIniFile->GetInteger(pSection, "HAlign", 0, &nValue);		
		if (nValue == 1)
			m_Style |= WNDTEXT_ES_HALIGN_CENTRE;
		else if (nValue == 2)
			m_Style |= WNDTEXT_ES_HALIGN_RIGHT;
		pIniFile->GetInteger(pSection, "VAlign", 0, &nValue);
		if (nValue == 1)
			m_Style |= WNDTEXT_ES_VALIGN_CENTRE;	
		else if (nValue == 2)
			m_Style |= WNDTEXT_ES_VALIGN_BOTTOM;

		char	Buff[256];
		pIniFile->GetString(pSection, "Color", "0,0,0", Buff, 16);
		m_TextColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "0,0,0", Buff, 16);
		m_BorderColor = GetColor(Buff);
		if (m_pText && m_pText[0] == 0)
		{
			if (pIniFile->GetString(pSection, "Text", "", Buff, 256))
			{
				strncpy(m_pText, Buff, m_nBuffLen - 1);
				m_pText[m_nBuffLen - 1] = 0;
				m_nTextLen = strlen(m_pText);
			}
		}
		pIniFile->GetInteger(pSection, "MultiLine", 0, &nValue);
		if (nValue)
		{
			m_Style |= WNDTEXT_ES_MULTILINE;
			if (g_pRepresentShell)
			{
				int nLineLen;
				m_nLineCount = TGetEncodedTextLineCount(
					m_pText, m_nTextLen, (m_Width * 2) / m_nFontSize, nLineLen, m_nFontSize);
			}
			else
				m_nLineCount = 0;
			if (m_nTopLine >= m_nLineCount)
				m_nTopLine = 0;
		}
		else
		{
			m_Style &= ~ WNDTEXT_ES_MULTILINE;
			m_nLineCount = 0;
			m_nTopLine = 0;
		}

		return true;
	}
	return false;
}

void KWndText::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_pText == NULL || m_pText[0] == 0 || g_pRepresentShell == NULL)
		return;
	KOutputTextParam	Param;
	Param.nX = m_nAbsoluteLeft;
	Param.nY = m_nAbsoluteTop;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	if ((m_Style & WNDTEXT_ES_MULTILINE) == 0)
	{
		switch(m_Style & WNDTEXT_ES_HALIGN_FILTER)
		{
		case WNDTEXT_ES_HALIGN_CENTRE:// ¾ÓÖÐ
			Param.nX += (m_Width - m_nTextLen * m_nFontSize / 2) / 2;
			if (Param.nX < m_nAbsoluteLeft)
				Param.nX = m_nAbsoluteLeft;
			break;
		case WNDTEXT_ES_HALIGN_RIGHT:// ¾ÓÓÒ
			Param.nX += m_Width - m_nTextLen * m_nFontSize / 2;
			if (Param.nX < m_nAbsoluteLeft)
				Param.nX = m_nAbsoluteLeft;
			break;
		}
		switch(m_Style & WNDTEXT_ES_VALIGN_FILTER)
		{
		case WNDTEXT_ES_VALIGN_CENTRE:
			Param.nY += (m_Height - m_nFontSize - 1) / 2;
			break;
		case WNDTEXT_ES_VALIGN_BOTTOM:
			Param.nY += m_Height - m_nFontSize - 1;
			break;
		}
		Param.nNumLine = 1;
	}
	else	//¶àÐÐ		
		Param.nNumLine = m_Height / (m_nFontSize + 1);

	Param.nSkipLine = m_nTopLine;
	Param.Color = m_TextColor;
	Param.BorderColor = m_BorderColor;
	g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_pText, m_nTextLen, m_Width);
}

void KWndText::SetTextColor(unsigned int Color)
{
	m_TextColor = Color;
}

void KWndText::SetIntText(int nNumber, char Separator)
{
	char	Buff[16];
	int		nLen;
	if (m_pText)
	{
		if (Separator)
			nLen = sprintf(Buff, "%d%c", nNumber, Separator);
		else
			nLen = sprintf(Buff, "%d", nNumber);
		SetText(Buff, nLen);
	}
}

void KWndText::SetCoinText(int nNumber, char Saparator)
{
	char	Buff[64];
	int		nLen;
	if (m_pText)
	{
		if (nNumber/10000 == 0)
			nLen = sprintf(Buff, "%d Xu", nNumber%10000);
		else
		{
			if (nNumber%10000 == 0)
				nLen = sprintf(Buff, "%d Xu", nNumber/10000);
			else
				nLen = sprintf(Buff, "%d v¹n %d Xu", nNumber/10000, nNumber%10000);
		}
		SetText(Buff, nLen);
	}
}

void KWndText::SetMoneyText(int nNumber, char Saparator)
{
	char	Buff[64];
	int		nLen;
	if (m_pText)
	{
		if (nNumber/10000 == 0)
			nLen = sprintf(Buff, "%d l­îng", nNumber%10000);
		else
		{
			if (nNumber%10000 == 0)
				nLen = sprintf(Buff, "%d v¹n", nNumber/10000);
			else
				nLen = sprintf(Buff, "%d v¹n %d l­îng", nNumber/10000, nNumber%10000);
		}
		SetText(Buff, nLen);
	}
}

void KWndText::Set2DoubleText(double nNumber1, double nNumber2, char Separator)
{
	char	Buff[32];
	int		nLen;
	if (m_pText)
	{
		nLen = sprintf(Buff, "%2.0f%c%2.0f", nNumber1, Separator, nNumber2);
		SetText(Buff, nLen);
	}
}

void KWndText::Set2IntText(int nNumber1, int nNumber2, char Separator)
{
	char	Buff[32];
	int		nLen;
	if (m_pText)
	{
		nLen = sprintf(Buff, "%d%c%d", nNumber1, Separator, nNumber2);
		SetText(Buff, nLen);
	}
}

void KWndText::SetTimeText(int nNumber)
{
	char	Buff[64];
	int		nLen;
	if (m_pText)
	{
		time_t seconds(nNumber);
		tm *p = gmtime(&seconds);

		int fkminus = p->tm_min / 18;
		int fkseconds = p->tm_sec / 18;

		if(nNumber >= 18)
			nLen = sprintf(Buff, "%02d:%02d", fkminus, fkseconds);
		else
			nLen = sprintf(Buff, "N/A");

		SetText(Buff, nLen);
	}
}

void KWndText::SetTimeText2(int nNumber)
{
	char	Buff[64];
	int		nLen;
	if (m_pText)
	{
		if(nNumber >= 64800)
			nLen = sprintf(Buff, "%dh", (nNumber/18/60/60) + 1);
		else if (nNumber >= 1080)
			nLen = sprintf(Buff, "%dm", (nNumber/18/60) + 1);
		else if (nNumber >= 18)
			nLen = sprintf(Buff, "%ds", nNumber/18);
		else
			nLen = sprintf(Buff, "N/A");
		SetText(Buff, nLen);
	}
}

void KWndText::SetMoneyUnitText(int nMoneyUnit)
{
	char    Buff[64];
	int        nLen;
	if (m_pText)
	{
		if (nMoneyUnit == moneyunit_extpoint)
			nLen = sprintf(Buff, "Xu");
		else if (nMoneyUnit == moneyunit_fuyuan)
			nLen = sprintf(Buff, "p.duyªn");
		else if (nMoneyUnit == moneyunit_repute)
			nLen = sprintf(Buff, "d.väng");
		else if (nMoneyUnit == moneyunit_accum)
			nLen = sprintf(Buff, "t.lòy");
		else if (nMoneyUnit == moneyunit_honor)
			nLen = sprintf(Buff, "vinh dù");
		else if (nMoneyUnit == moneyunit_respect)
			nLen = sprintf(Buff, "uy danh");
		else
			nLen = sprintf(Buff, "l­îng");
	   SetText(Buff, nLen);
	}
}

void KWndText::SetMoneyUnitPrice(int nPrice, int nMoneyUnit)
{
	char    Buff[64];
	int        nLen;
	if (m_pText)
	{
		if (nMoneyUnit == moneyunit_extpoint)//tien xu
			nLen = sprintf(Buff, "%d Xu", nPrice);
		else if (nMoneyUnit == moneyunit_fuyuan)//phuc duyen
			nLen = sprintf(Buff, "%d P.Duyªn", nPrice);
		else if (nMoneyUnit == moneyunit_repute)//danh vong
			nLen = sprintf(Buff, "%d D.Väng", nPrice);
		else if (nMoneyUnit == moneyunit_accum)//tong kim
			nLen = sprintf(Buff, "%d T.Lòy", nPrice);
		else if (nMoneyUnit == moneyunit_honor)//lien dau
			nLen = sprintf(Buff, "%d V.Dù", nPrice);
		else if (nMoneyUnit == moneyunit_respect)//uy danh c«ng tr¹ng
			nLen = sprintf(Buff, "%d Uy Danh", nPrice);
		else
			nLen = sprintf(Buff, "%d l­îng", nPrice);//tiÒn v¹n
	   SetText(Buff, nLen);
	}
}

void KWndText::SetFindIntText(int nNumber1, int nNumber2, char Separator, char Find)
{
	char	Buff[32];
	int		nLen;
	if (m_pText)
	{
		nLen = sprintf(Buff, "%d%c%d %s", nNumber1, Separator, nNumber2, "T×m");
		SetText(Buff, nLen);
	}
}

void KWndText::Set6IntText(int nNumber, char Separator)
{
	char	Buff[5];
	int		nLen;
	if (m_pText)
	{
		if (Separator)
			nLen = sprintf(Buff, "%d%c", nNumber, Separator);
		else
		{
			
			if (nNumber == -1)
				nLen = sprintf(Buff, "");
			else
				nLen = sprintf(Buff, "%d", nNumber);
				
		}
		SetText(Buff, nLen);
	}
}

void KWndText::Set6IntText(int nNumber1, int nNumber2, char Separator1, char Separator2)
{
	char	Buff[15];
	int		nLen;
	if (m_pText)
	{
		if (Separator1)
			nLen = sprintf(Buff, "%d%c%c%d%c", nNumber1, Separator2, Separator1, nNumber2, Separator2);
		else
		{	
			nLen = sprintf(Buff, "%d/%d", nNumber1, nNumber2);		
		}
		SetText(Buff, nLen);
	}
}
void KWndText::SetResistPlus(int nNumber, char Separator)
{
	char Buff[16];
	int	nLen;
	if (m_pText)
	{
		if (Separator)
		{
			nLen = sprintf(Buff, "%c%d%%", Separator, nNumber);
		}
		if (nLen)
			SetText(Buff, nLen);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
KWndText32::KWndText32()
{
	m_Text[0] = m_Text[31] = 0;
	SetTextPtr(m_Text, 31);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
KWndText80::KWndText80()
{
	m_Text[0] = m_Text[79] = 0;
	SetTextPtr(m_Text, 79);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
KWndText256::KWndText256()
{
	m_Text[0] = m_Text[255] = 0;
	SetTextPtr(m_Text, 255);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
KWndText512::KWndText512()
{
	m_Text[0] = m_Text[511] = 0;
	SetTextPtr(m_Text, 511);
}
