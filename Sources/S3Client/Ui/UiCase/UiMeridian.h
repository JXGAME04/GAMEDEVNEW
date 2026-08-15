#pragma once
#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"
#include <GameDataDef.h>
#include "../Elem/WndLine.h"


enum {
	normalway = 0,
	enhancedway,
	protectedway,
};
class KUiMeridian : public KWndToolBar
{
public:
	static KUiMeridian* OpenWindow();
	static void				CloseWindow();
	static KUiMeridian* GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiMeridian* GetSelf() { return m_pSelf; }
	void UpdateMeridianLevel();
	void UpdateMeridian();
private:
	~KUiMeridian() {}
	void	Initialize();							
	void	Breathe();
	void	LoadScheme(KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	int btnNo = 1;
private:
	static KUiMeridian* m_pSelf;
	KWndText80	m_LevelText;
	KWndText80	m_RankWorldText;

	KWndLabeledButton	m_Btn[MAX_MERIDIAN+1];
	KWndButton			m_Points[MAX_MERIDIAN_LEVEL];
	KWndButton	m_Close;
	KWndText80	m_MerLevelLabel[MAX_MERIDIAN_LEVEL];
	KWndImage	m_Background;
	BYTE			m_MeridianLevel[MAX_MERIDIAN];
	KWndLine	m_Connections[15];
	int lastTick = 0;
};

class KUiMeridianConfirm : public KWndToolBar
{
public:
	static KUiMeridianConfirm* OpenWindow();
	static void				CloseWindow();
	static KUiMeridianConfirm* GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiMeridianConfirm* GetSelf() { return m_pSelf; }
	void				SetMeridian(int Type, int Level);
	void	SetTitle(char* title);
	void	SetInfo();
private:
	KUiMeridianConfirm() {}
	~KUiMeridianConfirm() {}
	void	Initialize();
	void	Breathe();
	void	LoadScheme(KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	static KUiMeridianConfirm* m_pSelf;

	KWndLabeledButton	m_btnOk;
	KWndLabeledButton	cbNormalWay;
	KWndLabeledButton	cbProtectionWay;
	KWndLabeledButton	cbEnhanceRateWay;
	KWndButton	m_btnClose;
	KWndText80 ProtectTxt[2];
	int m_nType;
	int m_nLevel;
	int m_nWayProtect = normalway;
	int m_nWayEnhanced = normalway;
	KWndText256 Title;
	KWndText256	SuccessRate;
	KWndText80	Requirement;
	KWndText256	ProtectedTxt;
	KWndText256	EnhancedTxt;
	KWndText256	EffectTxt;
};