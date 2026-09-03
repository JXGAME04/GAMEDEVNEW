/*******************************************************************************
File        : UiMail.h
create data : 03-09-2026
Description : [MAIL 03/09] Cua so HOP THU (viet lai theo client VLTK 2.0: KUiMailManager /
              KUiMailInBox / KUiMailList / KUiMailHeader / KUiMailDetail / KUiMailIcon).

  Bo cuc lay tu 6 tep .ini cua client JX1 cu (\ui\ui3\mail\, cung dong voi 2.0):
    mail_manager.ini    khung 624x445 [Main] + [Title] + tab [InBox] + [Close]
    mail_list.ini       trang InBox: danh sach ben trai ([MailListScroll] 250x315 + thanh cuon),
                        nut Nhan / Xoa thu / Xoa toan bo, o Chon het / Tu dong xoa, So luong thu, bo loc
    mail_header.ini     MOT hang thu 245x48 (o chon, bieu tuong doc/chua doc, nguoi gui, tieu de, so ngay, kep dinh kem)
    mail_detail.ini     khung chi tiet 348x345 ben phai (tieu de, gio gui, nguoi gui, noi dung, o phan thuong)
    mail_award_item.ini mot o phan thuong 26x26
    mail_icon.ini       bieu tuong "bo cau dua thu" 25x25 gan mep phai

  Logic nam trong Lua (\script\ui\uimail.lua cua 2.0). Cua so chi:
    - nhan lenh tu Core: GDCNI_MAIL_UI (uParam = MAILUI_CMD_*, nParam = con tro/so) - KUiMail_OnCoreCmd
    - bao nguoc: g_pCoreShell->OperationRequest(GOI_MAIL_UI, MAILUI_OP_MAKE(op, extra), nParam)
  Hop dong: Core\Src\KMailUiDef.h. Cau noi Lua: Core\Src\KMailClient.cpp.
*******************************************************************************/
#pragma once

#include "../elem/wndimage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndlabeledbutton.h"
#include "../elem/WndPureTextBtn.h"
#include "../elem/wndtext.h"
#include "../elem/WndScrollBar.h"
#include "../elem/WndPage.h"
#include "../../../core/src/KMailUiDef.h"

#define MAILUI_ROW_COUNT		6		// 315 / 48
#define MAILUI_FILTER_MAX		5

class KIniFile;

// Noi dung thu dai (KWndText chi co ban 32/80/256/512)
class KWndTextMail : public KWndText
{
private:
	char	m_Text[MAILUI_CONTENT_LEN];
public:
	KWndTextMail();
};

//------------------------------------------------------------------
// Mot hang trong danh sach (mail_header.ini). La KWndPage de chuyen WND_N_BUTTON_CLICK len cha.
//------------------------------------------------------------------
class KUiMailRow : public KWndPage
{
public:
	KUiMailRow();
	void	Build();							// AddChild theo thu tu ve (duoi -> tren)
	void	LoadScheme(KIniFile* pIni);
	void	Fill(const KMailUiHeader* p, const char* szDayUnit, int bSelected, int bChecked);
	void	ClearRow();

	int					m_nId;					// 0 = hang trong
	KWndImage			m_Selection;			// [SelectionImg] chi hien khi dang chon
	KWndImage			m_Divider;				// [DividingLine]
	KWndText80			m_Sender;				// [MailSenderLable]
	KWndText80			m_Title;				// [MailTitleLable]
	KWndText32			m_Date;					// [MailDateLable]
	KWndImage			m_ReadIcon;				// [MailHaveReadIcon]
	KWndImage			m_UnreadIcon;			// [MailNotReadIcon]
	KWndImage			m_Attach;				// [AttachmentImg]
	KWndButton			m_Click;				// [ForClickImg] phu ca hang, bam = chon
	KWndButton			m_Check;				// [CheckBox]
};

//------------------------------------------------------------------
// Khung chi tiet ben phai (mail_detail.ini + mail_award_item.ini)
//------------------------------------------------------------------
class KUiMailDetail : public KWndImage
{
public:
	KUiMailDetail();
	void	Build();
	void	LoadScheme(const char* pScheme);
	void	Update(const KMailUiDetail* p);
	void	Clean();

private:
	KWndText256			m_BigTitle;				// [MailBigTitleValue]
	KWndText80			m_SendTime;				// [MailSendTime]
	KWndText80			m_SenderLabel;			// [MailSenderLable]
	KWndText80			m_TitleLabel;			// [MailTitleLable]
	KWndText80			m_SenderValue;			// [MailSenderValue]
	KWndText256			m_TitleValue;			// [MailTitleValue]
	KWndImage			m_Divider;				// [DividingLine]
	KWndText80			m_ContentLabel;			// [MailContentLable]
	KWndTextMail		m_Content;				// [MailContentValue]
	KWndImage			m_AwardBg[MAILUI_MAX_AWARD];		// [MailAwardBgSpr]
	KWndButton			m_AwardSpr[MAILUI_MAX_AWARD];		// [MailAwardItemSpr] anh vat pham + tooltip
	KWndText32			m_AwardCount[MAILUI_MAX_AWARD];		// [MailAwardItemCount]
	int					m_nAwardLeft, m_nAwardTop, m_nAwardInterval;	// [MailAwardPos]
	int					m_nAwardW, m_nAwardH;
};

//------------------------------------------------------------------
// Trang InBox: danh sach + nut + bo loc (mail_list.ini), chua ca KUiMailDetail
//------------------------------------------------------------------
class KUiMailList : public KWndImage
{
public:
	KUiMailList();
	void	Build();
	void	LoadScheme(const char* pScheme);
	virtual int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	// lenh tu Core
	void	AddHeader(const KMailUiHeader* p);
	void	SetHeader(const KMailUiHeader* p);
	void	DelOne(int nId);
	void	CleanList();
	void	UpdateCount();
	void	SetBtnStatus(const KMailUiBtnStatus* p);
	void	Select(int nId);
	void	SetFilterText(int nIndex);			// 1-based
	void	Refresh();

	KUiMailDetail		m_Detail;

private:
	int		FindHeader(int nId);
	int		RowOf(KWndWindow* pWnd, int* pnKind);	// tim hang chua nut vua bam; kind 0 = click, 1 = check
	void	SendOp(int nOp, int nExtra, int nParam);
	void	PopupFilter();

	KWndLabeledButton	m_BtnAccept;			// [ItemAccept_Btn]
	KWndLabeledButton	m_BtnDelOne;			// [DeleteMail_Btn]
	KWndLabeledButton	m_BtnDelAll;			// [DeleteAllMail_Btn]
	KWndText80			m_AutoDelLabel;			// [AutoDeleteLable]
	KWndButton			m_AutoDelChk;			// [AutoDelete_Btn]
	KWndText80			m_SelAllLabel;			// [SelectAllMailLable]
	KWndButton			m_SelAllChk;			// [SelectAllMail_Btn]
	KWndText80			m_CountLabel;			// [MailCountLable]
	KWndText32			m_CountValue;			// [MailCountValue]
	KWndText80			m_FilterTitle;			// [MailFilterTitle]
	KWndImage			m_FilterBg;				// [MailFilterBg]
	KWndPureTextBtn		m_FilterBtn;			// [MailFilterBtn]
	KWndImage			m_ScrollWnd;			// [MailListScroll] chua cac hang
	KWndScrollBar		m_Scroll;				// [MailListScroll_ScrollBar]
	KUiMailRow			m_Row[MAILUI_ROW_COUNT];

	KMailUiHeader		m_Header[MAILUI_MAX_MAIL];
	unsigned char		m_bChecked[MAILUI_MAX_MAIL];
	int					m_nCount;
	int					m_nTop;
	int					m_nSelId;
	char				m_szFilter[MAILUI_FILTER_MAX][64];
	int					m_nFilterCount;
	int					m_nFilterCur;
	char				m_szDayUnit[32];		// "ngay" lay tu [MailDateLable] Text
};

//------------------------------------------------------------------
// Khung ngoai (mail_manager.ini): tieu de, tab InBox, nut dong
//------------------------------------------------------------------
class KUiMailManager : public KWndImage
{
public:
	KUiMailManager();
	virtual ~KUiMailManager();

	static KUiMailManager*	OpenWindow();
	static KUiMailManager*	GetIfVisible();
	static KUiMailManager*	GetSelf() { return ms_pSelf; }
	static void				CloseWindow(bool bDestory = false);
	static void				Toggle();
	static void				LoadScheme(const char* pScheme);
	static void				Confirm(const KMailUiConfirm* p);

	KUiMailList			m_List;

private:
	static KUiMailManager* ms_pSelf;
	void	Initialize();
	virtual int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	KWndText80			m_Title;				// [Title]
	KWndLabeledButton	m_InBox;				// [InBox]
	KWndButton			m_Close;				// [Close]
};

//------------------------------------------------------------------
// Bieu tuong bo cau dua thu (mail_icon.ini)
//------------------------------------------------------------------
class KUiMailIcon : public KWndImage
{
public:
	KUiMailIcon();
	static void	SetVisible(int bVisible);
	static void	Blink();
	static void	LoadScheme(const char* pScheme);

private:
	static KUiMailIcon* ms_pSelf;
	void	Initialize();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void Breathe();

	KWndButton			m_Btn;					// [MailBtn]
	int					m_bBlink;
	int					m_nFrame;
};

// GameSpaceChangedNotify.cpp: case GDCNI_MAIL_UI
void KUiMail_OnCoreCmd(unsigned int uCmd, int nParam);
