// ---------------------------------------------------------------------------
// Cua so PHAT NGAN LUONG / DIEM CONG HIEN theo CHUC VU (bang hoi JX2).
// Port tu blueprint goc (2 che do dung chung: tieu de doi sprite theo loai
// tien, moi nhan co bien the TextMoney/TextContribution). Khung ma theo
// KUiTongAssignBox. Gui defTONG_JX2_COP_GRANT_GROUP: 3 so luong dong goi
// trong m_szText "e|c|m", co trong nParam1 (bit0-2 tong-so tung nhom,
// bit3 chi-online, bit4 ngan luong).
// ---------------------------------------------------------------------------
#ifndef UITONGGRANT_H
#define UITONGGRANT_H

#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/wndedit.h"
#include "../elem/wndbutton.h"


class KUiTongGrant : public KWndImage
{
public:
	KUiTongGrant();
	virtual ~KUiTongGrant();

	static KUiTongGrant*	OpenWindow(bool bMoney);
	static KUiTongGrant*	GetIfVisible();
	static void				CloseWindow(bool bDestory = true);
	static void				LoadScheme(const char* pScheme);

	virtual int	WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	static KUiTongGrant*	ms_pSelf;

	void	Initialize();
	void	ApplyMode();		// doi sprite tieu de + mo ta theo m_bMoney
	void	UpdateDescription();	// mo ta + DON VI (luong)
	void	OnConfirm();

private:
	KWndImage		m_ImgTitle;
	KWndText32		m_TxtDirector;
	KWndText32		m_TxtManager;
	KWndText32		m_TxtMember;
	KWndEdit32		m_EditDirector;
	KWndEdit32		m_EditManager;
	KWndEdit32		m_EditMember;
	KWndButton		m_BtnDirectorTotal;	// ca nhan / tong so (CheckBox)
	KWndButton		m_BtnManagerTotal;
	KWndButton		m_BtnMemberTotal;
	KWndButton		m_BtnTotalMember;	// chi phat nguoi online / ca bang
	KWndText512		m_TextDescription;
	KWndButton		m_BtnConfirm;
	KWndButton		m_BtnCancel;

	bool	m_bMoney;
	// chuoi [Settings] cua blueprint (TCVN3, doc tu ini)
	char	m_szForOnline[128];
	char	m_szForWhole[128];
	char	m_szImgMoney[128];
	char	m_szImgContribution[128];
};

#endif
