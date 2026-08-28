#pragma once
#include "WndWindow.h"

class KWndLine : public KWndWindow
{
public:
	virtual int	Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		PtInWindow(int x, int y);	//判断一个点是否在窗口范围内,传入的是绝对坐标
	void	SetColor(unsigned int uColor);
	void	Clone(KWndLine* pCopy);
	// [KM 27/08b] endX/endY TRUOC DAY KHONG duoc khoi tao. Init() lai luon dat
	// m_uColor != 0, nen mot KWndLine chi Init ma khong SetEnd se ve mot doan
	// thang toi toa do RAC: g_DrawLineAlpha di TUNG DIEM MOT (while x != x2) nen
	// vong lap chay hang ti nhip moi khung hinh => game dung hinh. Dung goc do.
	KWndLine() { m_uColor = 0; endX = 0; endY = 0; }
	void SetEnd(int x, int y);
protected:
	void	PaintWindow();
private:
	unsigned int	m_uColor;
	int endX, endY;
};
