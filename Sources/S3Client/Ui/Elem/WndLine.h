#pragma once
#include "WndWindow.h"

class KWndLine : public KWndWindow
{
public:
	virtual int	Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		PtInWindow(int x, int y);	//判断一个点是否在窗口范围内,传入的是绝对坐标
	void	SetColor(unsigned int uColor);
	void	Clone(KWndLine* pCopy);
	KWndLine() { m_uColor = 0; }
	void SetEnd(int x, int y);
protected:
	void	PaintWindow();
private:
	unsigned int	m_uColor;
	int endX, endY;
};
