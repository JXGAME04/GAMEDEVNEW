// -------------------------------------------------------------------------
//	文件名		：	UiGetNumber.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-1-7
//	功能描述	：	取钱界面
// -------------------------------------------------------------------------
#ifndef __UiGetNumber_H__
#define __UiGetNumber_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndLabeledButton.h"
#include "../../../core/src/GameDataDef.h"

#define STRING_MAX_VALUE	999999999

class KUiGetNumber : protected KWndImage
{
public:
	static KUiGetNumber*	OpenWindow(const char* pszTitle, const char* nFunc);
	static KUiGetNumber*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
	static void			CloseWindow(bool bDestroy);
private:
	KUiGetNumber();
	~KUiGetNumber() {}
	int		Initialize();
	void	Show();
	void	Hide();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
	void	OnCheckInput();
private:
	static KUiGetNumber*	m_pSelf;
	KWndText32			m_Title;	
	KWndEdit32			m_NumIn;
	KWndLabeledButton	m_OkBtn;
	KWndLabeledButton	m_CancelBtn;
	char				szFunc[64];	
};


#endif // __UiGetNumber_H__