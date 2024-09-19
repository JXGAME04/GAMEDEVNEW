#pragma once

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndLabeledButton.h"
#include "../../../core/src/GameDataDef.h"

struct KUiObjAtRegion;

class KUiGetString2 : protected KWndImage
{
public:
	
	static KUiGetString2* OpenWindow(const char* pszTitle, const char* nScriptIndex);
	static KUiGetString2*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	
	static void			CloseWindow(bool bDestroy);		
private:
	KUiGetString2() {};
	~KUiGetString2() {}
	void	Initialize();								
	void	Show();
	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
//	void	Breathe();	
private:
	static KUiGetString2*	m_pSelf;
	KWndText32				m_Title;
	short					m_nMinLen, m_nMaxLen;
	KWndEdit512				m_StringEdit;
	KWndLabeledButton		m_OkBtn;
	KWndLabeledButton		m_CancelBtn;
	char 					m_nScriptIndex[64], m_szTitle[64];
	int 					nTextLen, nTextPos;	
};

