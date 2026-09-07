#pragma once

#include "../Elem/WndImage.h"
#include "../Elem/UiImage.h"
#include "../Elem/WndPureTextBtn.h"


// [BANDO20 06/09] Bang dia diem tren anh ban do (settings\MapList.ini: N_MapPos / N_name / N_MapType),
// dung chung cho ban do the gioi + ban do son dong (cung anh 752x576, cung toa do): tro chuot ->
// ve ten Viet cua dia diem; bam -> GOI_WORLDMAP_GOTO (Core tu chay bo toi map do, nhu 2.0).
// Tam dia diem tren anh = N_MapPos + (30,17) - cung do lech voi ky hieu 'nguoi o day' cua KUiWorldmap.
class KIniFile;
#define WORLDMAP_MAX_LOC	320
struct KWorldMapLoc
{
	int		nMapId;
	int		nX, nY;			// tam tren anh (da cong +30,+17)
	char	szName[48];		// ten Viet (TCVN3)
	char	szType[16];		// City / Capital / Cave / Field / Battlefield / Tong / Country / Others
};
class KWorldMapLocs
{
public:
	KWorldMapLocs() : m_nCount(0), m_nHover(-1) {}
	void	Load(KIniFile* pIni);							// doc N_MapPos / N_name / N_MapType (N = 1..1200)
	int		Hit(int nLocalX, int nLocalY) const;			// dia diem gan nhat trong 14 px, -1 = khong
	int		OnClick(int nLocalX, int nLocalY);				// -1 khong trung; con lai = ket qua GOI_WORLDMAP_GOTO (1 di / 2 huy / 0 khong)
	void	PaintHover(int nAbsLeft, int nAbsTop, int nWndWidth);	// tu do vi tri chuot, ve ten dia diem dang tro
	int		GetCount() const { return m_nCount; }
private:
	KWorldMapLoc	m_aLoc[WORLDMAP_MAX_LOC];
	int				m_nCount;
	int				m_nHover;
};

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
	void	PaintWindow();		// [BANDO20 06/09] ve anh + ten dia diem dang tro chuot
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
	KWorldMapLocs		m_Locs;		// [BANDO20 06/09] bang dia diem (tro/bam)

};