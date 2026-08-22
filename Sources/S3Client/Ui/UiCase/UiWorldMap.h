#pragma once

#include "../Elem/WndImage.h"
#include "../Elem/UiImage.h"
#include "../Elem/WndPureTextBtn.h"

class KUiWorldmap : protected KWndImage
{
public:
	static KUiWorldmap* OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static void			CloseWindow();		//关闭窗口
	static KUiWorldmap*	GetIfVisible();
	void				LoadScheme(const char* pScheme);	//载入界面方案
	void SetCityOwnTong(const char* szTongName);
	void RefreshCityLabels();	// [CITYINFO 21/08] 7 nhan tu GDI_CITY_INFO
private:
	KUiWorldmap() {}
	~KUiWorldmap() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Breathe();				
	void	UpdateData();
private:
	static KUiWorldmap* m_pSelf;
	KWndImage			m_Country;
	KWndImage			m_City;
	KWndImage			m_Capital;
	KWndImage			m_Tong;
	KWndImage			m_Battlefield;
	KWndImage			m_Field;
	KWndImage			m_Cave;
	KWndImage			m_Others;
	
	KWndPureTextBtn		m_fengxiang;
	KWndPureTextBtn		m_bianjing;
	KWndPureTextBtn		m_yangzhou;
	KWndPureTextBtn		m_linan;
	KWndPureTextBtn		m_xiangyang;
	KWndPureTextBtn		m_chengdu;
	KWndPureTextBtn		m_dali;

};