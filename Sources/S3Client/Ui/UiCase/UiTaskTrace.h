/*
 * File:     UiTaskTrace.h
 * Desc:     Khung "Theo doi nhiem vu" (port y tuong KUiTaskTrace ban Linux/JX2):
 *           cua so nho treo goc man hinh, hien ban RUT GON nhiem vu Da Tau dang
 *           theo doi, tu cap nhat khi task value doi; bam vao dong -> mo bang
 *           Chi nam nhiem vu. Bat/tat bang 2 nut Theo doi / Huy theo doi cua
 *           bang chinh (KUiTaskGuide).
 * Creation: 2026/08/19
 */
#pragma once

#if !defined _UITASKTRACE
#define _UITASKTRACE

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndText.h"
#include "../Elem/WndMessageListBox.h"

#include "../Elem/WndButton.h"

class KUiTaskTrace : public KWndShowAnimate
{
public:
	static KUiTaskTrace*	OpenWindow();
	static void				CloseWindow(bool bDestroy);
	static KUiTaskTrace*	GetIfVisible();
	static void				SetTraced(bool bTraced);	// nut Theo doi / Huy theo doi
	static bool				IsTraced();
	static void				OnTaskValueChanged(int nTaskId);
private:
	KUiTaskTrace();
	virtual ~KUiTaskTrace();
	void	Initialize();
	void	LoadSchemeSelf(const char* pScheme);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateView();
private:
	static KUiTaskTrace*	m_pSelf;
	static bool				m_bTraced;

	KWndText256				m_Title;
	KScrollMessageListBox	m_List;		// [TaskInfo] + _List + _Scroll
};

// Nut icon nho 25x25 neo mep phai man hinh (anh opentracebtn.spr cua ban tham
// chieu, co san trong pak): bam = bat/tat khung theo doi nhiem vu. Mo cung HUD
// khi vao game (UiShell), dong khi thoat.
class KUiTaskTraceIcon : public KWndShowAnimate
{
public:
	static KUiTaskTraceIcon*	OpenWindow();
	static void					CloseWindow(bool bDestroy);
	// vi tri nut (de khung theo doi mo ra NGANG HANG voi nut); tra false neu chua co
	static bool					GetAnchor(int& nX, int& nY);
private:
	KUiTaskTraceIcon();
	virtual ~KUiTaskTraceIcon();
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	static KUiTaskTraceIcon*	m_pSelf;
	static int					ms_nAnchorX;
	static int					ms_nAnchorY;
	static bool					ms_bAnchor;
	KWndButton					m_Btn;
};

#endif
