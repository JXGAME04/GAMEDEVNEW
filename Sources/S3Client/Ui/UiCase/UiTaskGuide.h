/*
 * File:     UiTaskGuide.h
 * Desc:     Bang "Chi nam nhiem vu" (port tu KUiTaskGuide ban Linux/JX2, F12).
 *           Du an gan vao phim F11 thay cho so tay KUiTaskNote cu; ten cua so
 *           "NewTask" van duoc dang ky de F12 (autoexec.lua) dung chung.
 *           Du lieu: task value dong bo qua UI_TASKVALUE (id >= 256) + bang
 *           settings\task\tasklink_*.txt doc bang KTabFile (dia thang pak).
 * Creation: 2026/08/19
 */
#pragma once

#if !defined _UITASKGUIDE
#define _UITASKGUIDE

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"

#define TASKGUIDE_MAX_ENTRY		32
#define TASKGUIDE_NAME_LEN		64

// mot dong trong \UI\uitasklist.ini
struct KTaskGuideEntry
{
	char	szName[TASKGUIDE_NAME_LEN];	// ten hien thi (TCVN3, giu nguyen dau '+')
	int		nTaskId;					// -1 = dong tieu de nhom
};

class KUiTaskGuide : public KWndShowAnimate
{
public:
	static KUiTaskGuide*	OpenWindow();
	static void				CloseWindow(bool bDestroy);
	static KUiTaskGuide*	GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	// goi tu GameSpaceChangedNotify khi nhan GDCNI_TASK_VALUE_UPDATE
	static void				OnTaskValueChanged(int nTaskId);
	static void				AutoTraceOnTask(int nTaskId);			// [C35] nhan nhiem vu -> tu theo doi
	// ban RUT GON trang thai Da Tau (dung cho khung Theo doi nhiem vu)
	static void				BuildBriefLine(char* pOut, int nSize);
	static void				BuildTraceLine(char* pOut, int nSize);	// [C18] theo tab dang theo doi
	static int				GetTracedTaskId();						// [C18]
	static bool				IsTracedTask(int nTaskId);				// [C33] he nay dang theo doi?
	static void				SetTracedTask(int nTaskId, bool bOn);	// [C33]
	static int				GetTracedList(int* pOut, int nMax);		// [C33] danh sach he dang theo doi
	static void				BuildTraceLineOf(int nTaskId, char* pOut, int nSize);	// [C33]
	static const char*		GetTaskTitle(int nTaskId);				// [C33] ten he (mau vang)
	// dong bo lai trang thai 3 nut khi trace doi tu ben ngoai (icon mep man hinh)
	static void				RefreshButtons();
private:
	KUiTaskGuide();
	virtual ~KUiTaskGuide();
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	LoadSchemeSelf(const char* pScheme);
	void	LoadTaskList();				// doc \UI\uitasklist.ini -> m_Entries
	void	FillTaskList();				// do m_Entries vao khung trai
	void	ShowTask(int nEntry);		// chon 1 dong -> dung noi dung khung phai
	void	BuildDaTauText();			// noi dung nhiem vu Da Tau (TaskId 6)
	void	BuildTinSuText();
	void					BuildBangChienText();	// [CHI NAM 24/08]
	void					BuildBachNhanText();	// [CHI NAM 24/08]			// [TIN SU 21/08] noi dung nhiem vu Tin Su (TaskId 7)
	void					BuildSatThuText();	// [3HD 25/08] san boss sat thu (TaskId 10)
	void	AddLine(const char* pText);	// them 1 dong (tu ma hoa <color>/<enter>)
	void	TryGoXaFu();				// nhiem vu loai 4 dang lam -> nho CoreShell dan den Xa Phu
	void	UpdateButtons();			// bat/tat 3 nut theo trang thai nhiem vu + theo doi
private:
	static KUiTaskGuide*	m_pSelf;

	KWndText256				m_Title;
	KWndText256				m_SelTaskName;
	KScrollMessageListBox	m_TaskList;			// khung trai: danh muc nhiem vu
	KWndMessageListBox		m_Content;			// khung phai: mo ta nhiem vu
	KWndScrollBar			m_ContentScroll;
	KWndButton				m_BtnClose;
	KWndLabeledButton		m_BtnQuit;			// hien thi giong ban goc; chua co
	KWndLabeledButton		m_BtnTrace;			// nghiep vu -> Enable(false)
	KWndLabeledButton		m_BtnCancelTrace;

	char					m_szTaskIniPath[128];	// khoa TaskIni trong [Main]
	KTaskGuideEntry			m_Entries[TASKGUIDE_MAX_ENTRY];
	int						m_nEntryCount;
	int						m_nCurEntry;
};

#endif
