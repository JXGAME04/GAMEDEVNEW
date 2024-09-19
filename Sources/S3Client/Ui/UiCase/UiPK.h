#pragma once
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndText.h"
#include "../Elem/WndPureTextBtn.h"

class KUiPK : protected KWndShowAnimate
{
public:
	static KUiPK* OpenWindow();				
	static void			 CloseWindow();
	static KUiPK*	GetIfVisible();		
private:
	KUiPK();
	~KUiPK() {}
	void	Initialize();					
	void	LoadScheme(const char* pScheme);	
	void	CancelMenu();

	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);


private:
	static KUiPK* m_pSelf;
private:
	KWndPureTextBtn	LuyenCong;
	KWndPureTextBtn	ChienDau;
	KWndPureTextBtn	DoSat;
	KWndButton	LCBT, CDBT, DSBT;
	KPopupMenuData*	m_pSkinMenu;
};