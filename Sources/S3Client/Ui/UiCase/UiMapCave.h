/* 
 * File:     UiCaveMap.h
 * Desc:     Hien thi ban do son dong VNG
 * Author:   Phong Kieu
 * Creation: 2020/12/01
 */
//-----------------------------------------------------------------------------
#pragma once

#include "../Elem/WndImage.h"
#include "../Elem/UiImage.h"
#include "UiWorldMap.h"	// [BANDO20 06/09] KWorldMapLocs (bang dia diem dung chung)

class KUiMapCave : protected KWndImage
{
public:
	static KUiMapCave* OpenWindow();		
	static void			CloseWindow();		
	static KUiMapCave*	GetIfVisible();
	void				LoadScheme(const char* pScheme);

private:
	KUiMapCave() {}
	~KUiMapCave() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Breathe();	
	void	PaintWindow();		// [BANDO20 06/09] ve anh + ten dia diem dang tro chuot
	void	UpdateData();
private:
	static KUiMapCave* m_pSelf;
	KWndImage			m_Sign;
	KWorldMapLocs		m_Locs;		// [BANDO20 06/09] bang dia diem (tro/bam)
};