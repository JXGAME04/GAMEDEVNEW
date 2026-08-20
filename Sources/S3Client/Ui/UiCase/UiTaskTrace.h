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
	void	SnapToButton();		// neo sat trai + ngang hang nut theo doi
private:
	// KHONG giu co "dang theo doi" rieng: trang thai = cua so co dang hien hay khong.
	// (Truoc day co co m_bTraced rieng, ESC/UiCloseWndsInGame an cua so nhung khong
	//  xoa co => bam nut lan sau chi "tat" mot cua so von da an, phai bam 2 lan.)
	static KUiTaskTrace*	m_pSelf;

	KWndText256				m_Title;
	KScrollMessageListBox	m_List;		// [TaskInfo] + _List + _Scroll
};

// Nut bat/tat khung theo doi nam TRONG thanh KUiPlayerBar (m_TraceBtn, section
// [TraceBtn] cua UiPlayerBar.ini - dung vi tri nut Zalo cu, neo 1024-30 nhu cu).
// Khung theo doi mo ra ngang hang nut qua KUiPlayerBar::GetTraceBtnPos.

#endif
