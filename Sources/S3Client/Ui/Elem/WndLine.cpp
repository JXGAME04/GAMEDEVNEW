/*****************************************************************************************
//	界面窗口体系结构--切换显示状态时带移动控制的图形的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-17
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "WndLine.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell* g_pRepresentShell;

void KWndLine::Clone(KWndLine* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_uColor = m_uColor;
	}
}

void KWndLine::SetEnd(int x, int y)
{
	endX = x;
	endY = y;
}

//初始化
int KWndLine::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		char	Buff[16];
		pIniFile->GetString(pSection, "ShadowColor", "", Buff, sizeof(Buff));
		m_uColor = (GetColor(Buff) & 0xffffff);
		int nAlpha;
		pIniFile->GetInteger(pSection, "ShadowColorAlpha", 128, &nAlpha);
		if (nAlpha > 255 || nAlpha < 0)
			m_uColor |= 0xff000000;
		else
			m_uColor |= (((255 - nAlpha) << 21) & 0xff000000);
		return true;
	}
	return false;
}

//窗体绘制
void KWndLine::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_uColor && g_pRepresentShell)
	{
		KRULine	Lines[2];

		Lines[0].Color.Color_dw = m_uColor;
		Lines[0].oPosition.nX = m_nAbsoluteLeft;
		Lines[0].oPosition.nY = m_nAbsoluteTop;
		Lines[0].oEndPos.nX = m_nAbsoluteLeft + endX;
		Lines[0].oEndPos.nY = m_nAbsoluteTop + endY;
		Lines[0].oEndPos.nZ = Lines[0].oPosition.nZ = 0;

		Lines[1].Color.Color_dw = Lines[0].Color.Color_dw;
		Lines[1].oPosition.nX = Lines[0].oPosition.nX+1;
		Lines[1].oPosition.nY = Lines[0].oPosition.nY+1;
		Lines[1].oEndPos.nX = Lines[0].oEndPos.nX+1;
		Lines[1].oEndPos.nY = Lines[0].oEndPos.nY+1;
		Lines[1].oEndPos.nZ = Lines[0].oEndPos.nZ = 0;

		g_pRepresentShell->DrawPrimitives(2, &Lines[0], RU_T_LINE, true);
	}
}

//判断一个点是否在窗口范围内,传入的是绝对坐标
int KWndLine::PtInWindow(int x, int y)
{
	return 0;
}

void KWndLine::SetColor(unsigned int uColor)
{
	m_uColor = uColor;
}