/*****************************************************************************************
//	Copyright : VLUS 2025
//	Author	:   X
//	CreateTime:	2025-10-15
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiSkillsNew.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "crtdbg.h"
#include "../UiSoundSetting.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;

extern iCoreShell*		g_pCoreShell;

#define 	SCHEME_INI_SHEET			"UiSkillNew.ini"
#define 	SCHEME_INI_LIVE				"UiSkillLive.ini"
#define 	SCHEME_INI_FIGHT			"UiSkillFly.ini"
#define 	SCHEME_INI_FIGHT_SUB_PAGE	"UiSkillFlySub.ini"
#define		SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE	9
#define		AUTO_SET_IMMED_SKILL_MSG_ID					"24"

KUiFightSkillSubPageNew::KUiFightSkillSubPageNew()
{
	m_nRemainSkillPoint = 0;
//	m_nSubPagIndex = 0;
}

void KUiFightSkillSubPageNew::Initialize(/*int nSubPageIndex*/)
{
	for (int i = 0; i < FIGHT_SKILL_UI_MAX; i ++)
	{
		AddChild(&m_FightSkills[i]);
		AddChild(&m_btnFightSkills[i]);
		m_FightSkills[i].Celar();
		m_FightSkills[i].SetContainerId((int)UOC_SKILL_LIST);
	}
//	m_nSubPagIndex = nSubPageIndex;
}

void KUiFightSkillSubPageNew::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_FIGHT_SUB_PAGE);
	if (Ini.Load(Buff))
	{
		KWndPage::Init(&Ini, "Main");
		for (int i = 0; i < FIGHT_SKILL_UI_MAX; i++)
		{
			sprintf(Buff, "Skill_%d_%d", m_nFaction, i);
			m_FightSkills[i].Init(&Ini, Buff);
			m_FightSkills[i].EnablePickPut(true);
			sprintf(Buff, "SkillBtn_%d_%d", m_nFaction, i);
			m_btnFightSkills[i].Init(&Ini, Buff);
			m_btnFightSkills[i].Hide();
		}

		Ini.GetInteger("SkillText_", "Font", 12, &m_SkillTextParam.nFont);
		Ini.GetInteger2("SkillText_", "Offset", (int*)&m_SkillTextParam.Offset.cx, (int*)&m_SkillTextParam.Offset.cy);
		Ini.GetString("SkillText_", "Color", "", Buff, 16);
		m_SkillTextParam.Color = GetColor(Buff);
	}
}

void KUiFightSkillSubPageNew::UpdateRemainPoint(int nPoint)
{
	m_nRemainSkillPoint = nPoint;
}

void KUiFightSkillSubPageNew::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	_ASSERT(pSkill && nIndex >= 0 && nIndex < FIGHT_SKILL_UI_MAX);
	m_FightSkills[nIndex].HoldObject(pSkill->uGenre, pSkill->uId, pSkill->nLevel, 0);
}
#include <unordered_map>
#include "../ShortcutKey.h"

// Static map to map uId to indices of m_FightSkills
//<skillid, index>
//index config toa do spr trong file Uiskillflysub.ini
static const std::unordered_map<unsigned int, int> g_uIdToIndexMap = {
	//210 Khinh cong
	// Look up trong skills.txt lay skillid cho tung mon phai
	// Faction 1 Thien Vuong

	{34,   0}, // Kinh loi Tram
	{30,   1}, // Hoi Phong Lac Nhan
	{29,   2}, // Tram Long Quyet
	{40,   3}, // Doan Hon Thich
	{708,  4},
	{24,   5}, // Thien Vuong Dao phap
	{23,   6}, // Thien Vuong Thuong phap
	{26,   7}, // Thien Vuong Chuy phap
	{33,   8}, // Tinh Tam Quyet
	//{210,   9}, // Khinh cong
	{37,   10}, // Bat Phong Tram
	{35,  11}, // Duong Quang Tam Diep
	{31,  12}, // Hang Van Quyet
	{42,  13}, // Kim Chung Trao
	{32,  14}, // Vo Tam Tram
	{41,  15}, // Huyet Chien Bat Phuong
	{324, 16}, // Thua Long Quyet
	{36,  17}, // Thien Vuong Chien Y - Tran phai
	{322, 18},
	{323, 19},
	{325, 20},
	{1058,21},
	{1059,22},
	{1060,23},
	{210,   30}, // Khinh cong


	// Faction 2 duong mon
	{45, 0}, //Phich Lich Don
	{347, 1}, //Dia Diem Hoa
	{43, 2}, //Duong Mon Am Khi
	{303, 3}, //Doc Thich Cot
	{50, 4}, //Truy Tam Tien
	{47, 5}, //Doat Hon Tieu
	{54, 6}, //Man Thien Hoa Vu
	{343, 7}, //Xuyen Tam Thich
	{345, 8}, //Han Bang Thich
	{349, 9}, //Loi Kich Thuat
	{249, 10}, //Tieu Ly Phi Dao
	{341, 11}, //Tan Hoa Tieu
	{58, 12}, //Thien La Dia Vong
	{48, 13}, //Tam Nhan - tran phai
	{710, 14},
	{339,15},
	{342,16},
	{302,17},
	{351,18},
	{1069,19},
	{1070,20},
	{1071,21},
	//{210,22},
	//{210,23}, // Khinh cong


	// Faction 3 ngu doc
	{ 63, 0 },   // Doc Sa Chuong
	{ 65, 1 },   // Huyet Dao Doc Sat
	{ 62, 2 },   // Ngu Doc Chuong Phap
	{ 60, 3 },   // Ngu Doc Dao phap
	{ 67, 4 },   // Cuu Thien Cuong Loi
	{ 70, 5 },   // Xich Diem Thuc Thien
	{ 66, 6 },   // Tap Nan Duoc kinh
	{ 68, 7 },   // U Minh Kho Lau
	{ 384, 8 },  // Bach Doc Xuyen Tam
	{ 64, 9 },   // Bang Lam Huyen Tinh
	{ 69, 10 },  // Vo Hinh Doc
	{ 356, 11 }, // Xuyen Y Pha Giap
	{ 73, 12 },  // Van Co Thuc Tam
	{ 72, 13 },  // Xuyen Tam Doc Thich
	{ 71, 14 },  // Thien Cuong Dia Sat
	{ 74, 15 },  // Chu Cap Thanh Minh
	{ 75, 16 },  // Ngu Doc Ky Kinh - tran phai
	{ 711, 17 },
	{ 353, 18 },
	{ 355, 19 },
	{ 390, 20 },
	{ 1066, 21 },
	{ 1067, 22 },
	//{210,   23}, // Khinh cong


	// Faction 4 nga mi
	{ 85, 0 },    // Nhat Diep Tri Thu
	{ 80, 1 },    // Phieu Tuyet Xuyen Van
	{ 77, 2 },    // Nga My Kiem Phap
	{ 79, 3 },    // Nga My Chuong Phap
	{ 93, 4 },    // Tu Hang Pho Do
	{ 385, 5 },   // Thoi Song Vong Nguyet
	{ 82, 6 },    // Tu Tuong Dong Quy
	{ 89, 7 },    // Mong Diep
	{ 86, 8 },    // Luu Thuy
	{ 92, 9 },    // Phat Tam Tu Huu
	{ 88, 10 },   // Bat Diet Bat Tuyet
	{ 91, 11 },   // Phat Quang Pho Chieu
	{ 282, 12 },  // Thanh Am Phan Xuong
	{ 252, 13 },  // Phat Phap Vo Bien - tran phai
	{ 712, 14 },
	{ 328, 15 },
	{ 380, 16 },
	{ 332, 17 },
	{ 1061, 18 },
	{ 1062, 19 },
	//{ 210,   20 }, // Khinh cong


	// Faction 5 thuy yen
	{ 99, 0 },    // Phong Hoa Tuyet Nguyet
	{ 102, 1 },   // Phong Quyen Tan Tuyet
	{ 95, 2 },    // Thuy Yen Dao Phap
	{ 97, 3 },    // Thuy Yen Song Dao
	{ 269, 4 },   // Bang Tam Trai Anh
	{ 105, 5 },   // Vu Da Le Hoa
	{ 113, 6 },   // Phu Van Tan Tuyet
	{ 100, 7 },   // Ho The Han Bang
	{ 109, 8 },   // Tuyet Anh
	{ 108, 9 },   // Muc Da Luu Tinh
	{ 111, 10 },  // Bich Hai Trieu Sinh
	{ 114, 11 },  // Bang Cot Tuyet Tam - tran phai
	{ 713, 12 },
	{ 336, 13 },
	{ 337, 14 },
	{ 1063, 15 },
	{ 1065, 16 },
	//{ 210,   17 }, // Khinh cong


	// Faction 6 cai bang
	{ 122, 0 },   // Kien Nhan Than Thu
	{ 119, 1 },   // Dieu mon Thac Bat
	{ 116, 2 },   // Cai Bang Chuong Phap
	{ 115, 3 },   // Cai Bang Bong Phap
	{ 129, 4 },   // Hoa Hiem Vi Di
	{ 274, 5 },   // Giang Long Chuong
	{ 124, 6 },   // Da Cau Tran (Da cau bong phap)
	{ 277, 7 },   // Hoat Bat Luu Thu
	{ 128, 8 },   // Khang Long Huu Hoi
	{ 125, 9 },   // Bong Da Ac Cau
	{ 360, 10 },  // Tieu Dieu Cong
	{ 130, 11 },  // Tuy Diep Cuong Vu - tran phai
	{ 714, 12 },
	{ 357, 13 },
	{ 359, 14 },
	{ 1073, 15 },
	{ 1074, 16 },
	//{ 210,   17 }, // Khinh cong


	// Faction 7 thien nhan
	{ 135, 0 },   // Tan Duong Nhu Huyet
	{ 145, 1 },   // Don Chi Liet Diem
	{ 132, 2 },   // Thien Nhan Mau Phap
	{ 131, 3 },   // Thien Nhan Dao Phap
	{ 136, 4 },   // Hoa lien Phan Hoa
	{ 137, 5 },   // Ao Anh Phi Ho
	{ 141, 6 },   // Liet Hoa Tinh Thien
	{ 138, 7 },   // Thoi Son Dien Hai
	{ 140, 8 },   // Phi Hong Vo Tich
	{ 364, 9 },   // Bi To Thanh Phong
	{ 143, 10 },  // Lich Ma Doat Hon
	{ 142, 11 },  // Thau Thien Hoan Nhat
	{ 148, 12 },  // Ma Diem That Sat
	{ 150, 13 },  // Thien Ma Giai The - tran phai
	{ 715, 14 },
	{ 361, 15 },
	{ 362, 16 },
	{ 391, 17 },
	{ 1075, 18 },
	{ 1076, 19 },
	//{ 210,   20 }, // Khinh cong


	// Faction 8 vo dang
	{ 153, 0 },   // No Loi Chi
	{ 155, 1 },   // Thuong Hai Minh Nguyet
	{ 152, 2 },   // Vo Dang Quyen Phap
	{ 151, 3 },   // Vo Dang Kiem Phap
	{ 159, 4 },   // That Tinh Tran
	{ 164, 5 },   // Bac Cap Nhi Phuc
	{ 158, 6 },   // Kiem Phi Kinh Thien
	{ 160, 7 },   // The Van Tung
	{ 157, 8 },   // Toa Vong Vo Nga
	{ 165, 9 },   // Vo Nga Vo Kiem
	{ 267, 10 },  // Tam Hoan Thao Nguyet
	{ 166, 11 },  // Thai Cuc Than Cong - tran phai
	{ 716, 12 },
	{ 365, 13 },
	{ 368, 14 },
	{ 1078, 15 },
	{ 1079, 16 },
	//{ 210,   17 }, // Khinh cong


	// Faction 9 con lon
	{ 169, 0 },   // Ho Phong Phap
	{ 179, 1 },   // Cuong Loi Chan Dia
	{ 167, 2 },   // Con Lon Dao Phap
	{ 168, 3 },   // Con Lon Kiem Phap
	{ 392, 4 },   // Thuc Phuoc Chu
	{ 171, 5 },   // Thanh Phong Phu
	{ 174, 6 },   // Ki Ban Phu
	{ 178, 7 },   // Nhat Khi Tam Thanh
	{ 172, 8 },   // Thien Te Tan Loi
	{ 393, 9 },   // Bac Minh Dao Hai
	{ 173, 10 },  // Thien Thanh Dia Troc
	{ 175, 11 },  // Khi Han Ngao Tuyet
	{ 181, 12 },  // Khi Tam Phu
	{ 176, 13 },  // Cuong Phong Sau Dien
	{ 90, 14 },   // Me Tung Ao Anh
	{ 182, 15 },  // Ngu Loi Chanh Phap
	{ 275, 16 },  // Suong Ngao Con Luan - tran phai
	{ 630, 17 },  // Huyen Thien Vo cuc
	{ 717, 18 },
	{ 372, 19 },
	{ 375, 20 },
	{ 394, 21 },
	{ 1080, 22 },
	{ 1081, 23 },
	//{ 210,   24 }, // Khinh cong


	// Faction 0 Thieu lam
	{ 14, 0 },    // Han Long Bat Vu
	{ 10, 1 },    // Kim Cang Phuc Ma
	{ 15, 3 },    // Bat Dong Minh Vuong
	{ 709, 4 },   // 5
	{ 4, 6 },     // Thieu Lam Con Phap
	{ 6, 7 },     // Thieu Lam Dao Phap
	{ 8, 5 },     // Thieu Lam Quyen Phap
	{ 16, 8 },    // La Han Tran
	//{ 210, 8 },   // Khinh cong
	{ 271, 10 },   // Long Trao ho trao
	{ 11, 11 },   // Hoanh Tao Luc Hop
	{ 19, 12 },   // Ma Ha Vo Luong
	{ 20, 13 },   // Su Tu Hong
	{ 318, 14 },  // Dat ma do giang 1
	{ 319, 15 },  // hoanh toa thien quan 2
	{ 321, 16 },  // V? T-íng Tr?m 3
	{ 21, 17 },   // Dich Can Kinh
	{ 1055, 18 }, // dat ma kim cuong chuong 1
	{ 1056, 19 }, // 15x bong 2
	{ 1057, 20 },  // Tam Gi阨 Quy Thi阯 3
	{ 273, 21 },  // Nhu Lai Thien Diep - Tran phai
	//{ 210,   22 }, // Khinh cong

	// Faction 10 Hoa Son [HOASON 01/09] - o theo [Skill_10_i] trong UiSkillFlySub.ini
	{ 1347, 0 },   // Bach Hong Quan Nhat (kiem tong, nhap mon)
	{ 1351, 1 },   // Kim Nhan Hoanh Khong (30)
	{ 1355, 2 },   // Thien Than Dao Huyen (50)
	{ 1360, 3 },   // Thuong Tung Nghenh Khach (60)
	{ 1364, 4 },   // Doat Menh Lien Hoan Tam Tien Kiem (90 kiem)
	{ 1369, 5 },   // Cuu Kiem Hop Nhat (150 kiem)
	{ 1372, 6 },   // Thanh Phong Tong Sang (khi tong, nhap mon)
	{ 1376, 7 },   // Long Huyen Kiem Khi (30)
	{ 1380, 8 },   // Ma Van Kiem Khi (60)
	{ 1382, 9 },   // Phach Thach Pha Ngoc (90 khi)
	{ 1384, 10 },  // Than Quang Toan Nhieu (150 khi)
	{ 1358, 11 },  // Huyen Nhan Van Yen - tran phai
	{ 1349, 12 },  // Kiem Tong Tong Quyet (10)
	{ 1350, 13 },  // Duong Ngo Kiem Phap (20)
	{ 1354, 14 },  // Hi Di Kiem Phap (40)
	{ 1374, 15 },  // Long Nhieu Than (10)
	{ 1375, 16 },  // Hai Nap Bach Xuyen (20)
	{ 1378, 17 },  // Khi Chan Son Ha (40)
	{ 1379, 18 },  // Khi Quan Truong Hong (50)
	{ 1365, 19 },  // Tu Ha Kiem Khi (120)
	{ 1370, 20 }   // Hao Nhien Chi Khi (tien giai)
	//{ 210,   22 }, // Khinh cong
};


void KUiFightSkillSubPageNew::UpdateData(KUiSkillData* pSkills)
{
	_ASSERT(pSkills);

	for (int i = 0; i < FIGHT_SKILL_UI_MAX; i++)
	{
		// Check if the uId exists in the map
		auto it = g_uIdToIndexMap.find(pSkills[i].uId);
		if (it != g_uIdToIndexMap.end())
		{
			int mappedIndex = it->second;

			// Ensure the mapped index is within bounds
			if (mappedIndex >= 0 && mappedIndex < FIGHT_SKILL_UI_MAX)
			{
				m_FightSkills[mappedIndex].HoldObject(
					pSkills[i].uGenre, pSkills[i].uId, pSkills[i].nLevel, 0);
			}
		}
	}
}

void KUiFightSkillSubPageNew::UpdateFaction(int faction)
{
	m_nFaction = faction;
}

int	KUiFightSkillSubPageNew::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK && uParam && m_nRemainSkillPoint)
	{
		for (int i = 0; i < FIGHT_SKILL_UI_MAX; i++)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_btnFightSkills[i])
			{
				KUiDraggedObject	Obj;
				m_FightSkills[i].GetObject(Obj);
				if (Obj.uGenre != CGOG_NOTHING)
				{
					m_nRemainSkillPoint --;	// 使用技能点数
					g_pCoreShell->OperationRequest(GOI_TONE_UP_SKILL, CGOG_SKILL_FIGHT, Obj.uId);
				}
			}
		}
		return 0;
	}
	if(uMsg == WND_N_ITEM_PICKDROP && uParam)
	{
		OnSkillPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		return 0;
	}

	return KWndPage::WndProc(uMsg, uParam, nParam);
}

void KUiFightSkillSubPageNew::OnSkillPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	UISYS_STATUS eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);
        ((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_SKILL_TAKE_WITH;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_SKILL_TAKE_WITH;		
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, pPickPos ? (unsigned int)&Pick : 0, pDropPos ? (int)&Drop : 0);
}

void KUiFightSkillSubPageNew::PaintWindow()
{
	KWndPage::PaintWindow();
	for (int i = 0; i < FIGHT_SKILL_UI_MAX; i++)
	{
		KUiDraggedObject	Obj;
		m_FightSkills[i].GetObject(Obj);
		if (Obj.uGenre != CGOG_NOTHING)
		{				
			int nLeft, nTop, nWidth, nHeight;
			m_FightSkills[i].GetAbsolutePos(&nLeft, &nTop);
			m_FightSkills[i].GetSize(&nWidth, &nHeight);
			m_btnFightSkills[i].Show();
			g_pCoreShell->DrawGameObj(CGOG_SKILL_FIGHT, Obj.uId, nLeft, nTop, nWidth, nHeight, 0);
			if (m_nRemainSkillPoint)
				m_btnFightSkills[i].Enable(true);
			else
				m_btnFightSkills[i].Enable(false);
			unsigned int dwColor = 0; 
			int nLevel = 0;
			if (g_pCoreShell->GetSkillData(Obj.uId, &nLevel) == TRUE)
				dwColor = 0xff33ffff;
			else
				dwColor = m_SkillTextParam.Color;	
			if (nLevel)
			{
				char	szLevel[8];
				int		nLen;
				itoa(nLevel, szLevel, 10);
				nLen = strlen(szLevel);
				nLeft += m_SkillTextParam.Offset.cx;
				nTop += m_SkillTextParam.Offset.cy;
				nLeft += (nWidth - nLen * m_SkillTextParam.nFont / 2) / 2;

				g_pRepresentShell->OutputText(m_SkillTextParam.nFont, szLevel, nLen, nLeft, nTop, dwColor, 0);
			}
		}
	}
}

void KUiFightSkillSubNew::Initialize()
{
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
	{
		m_SubPages[i].Initialize();
		AddPage(&m_SubPages[i], &m_SubPageBtn[i]);
	}
}

void KUiFightSkillSubNew::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_FIGHT);
	if (Ini.Load(Buff))
	{
		KWndPageSet::Init(&Ini, "Main");
		m_oFixPos.x = 0;
		m_oFixPos.y = 0;
		SetPosition(0, 0);
		for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		{
			m_SubPages[i].LoadScheme(pScheme);
			//sprintf(Buff, "SubPageBtn_%d", i);
			//m_SubPageBtn[i].Init(&Ini, Buff);
		}
		Show();
	}
}

void KUiFightSkillSubNew::UpdateRemainPoint(int nPoint)
{
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		m_SubPages[i].UpdateRemainPoint(nPoint);
}

void KUiFightSkillSubNew::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	_ASSERT(pSkill); 
	if(!pSkill) return;
	int nPage = nIndex / FIGHT_SKILL_UI_MAX;
	nIndex = nIndex % FIGHT_SKILL_UI_MAX;
	_ASSERT(nPage >= 0 && nPage < FIGHT_SKILL_SUB_PAGE_COUNT);
	m_SubPages[nPage].UpdateSkill(pSkill, nIndex);
}

void KUiFightSkillSubNew::UpdateData()
{
	KUiSkillData	Skills[FIGHT_SKILL_COUNT];
	g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (unsigned int)Skills, 0);
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		m_SubPages[i].UpdateData(&Skills[i * FIGHT_SKILL_UI_MAX]);
}

void KUiFightSkillSubNew::UpdateFaction(int faction)
{
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		m_SubPages[i].UpdateFaction(faction);

}

void KUiFightSkillNew::Initialize()
{
	m_InternalPad.Initialize();
	AddChild(&m_InternalPad);
	AddChild(&m_RemainSkillPoint);
}

void KUiFightSkillNew::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	char		Buff1[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_FIGHT);
	if (Ini.Load(Buff))
	{
		KWndPage::Init(&Ini, "Main");
		m_InternalPad.LoadScheme(pScheme);
		sprintf(Buff1, "%s%d", "RemainPoint_", m_nFaction);
		m_RemainSkillPoint.Init(&Ini, Buff1);

	}
}

void KUiFightSkillNew::UpdateData()
{
	int nRemainSkillPoint = g_pCoreShell->GetGameData(GDI_FIGHT_SKILL_POINT, 0, 0);
	m_InternalPad.UpdateRemainPoint(nRemainSkillPoint);
	m_RemainSkillPoint.SetIntText(nRemainSkillPoint);
	m_InternalPad.UpdateData();
}

void KUiFightSkillNew::UpdateFaction(int faction)
{
	m_InternalPad.UpdateFaction(faction);
	m_nFaction = faction;
}

void KUiFightSkillNew::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	m_InternalPad.UpdateSkill(pSkill, nIndex);
}

void KUiFightSkillNew::UpdateRemainPoint(int nPoint)
{
	m_RemainSkillPoint.SetIntText(nPoint);
	m_InternalPad.UpdateRemainPoint(nPoint);

}

KUiLiveSkillNew::KUiLiveSkillNew()
{
	m_nRemainSkillPoint = 0;
}

void KUiLiveSkillNew::Initialize()
{
	AddChild(&m_RemainSkillPoint);

	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
	{
		m_LiveSkill[i].Celar();
		AddChild(&m_LiveSkill[i]);
		m_LiveSkill[i].SetContainerId((int)UOC_SKILL_LIST);
	}
}

void KUiLiveSkillNew::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_LIVE);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		m_RemainSkillPoint.	Init(&Ini, "RemainPoint");

		for (int i = 0; i < LIVE_SKILL_COUNT; i++)
		{
			sprintf(Buff, "Skill_%d", i);
			m_LiveSkill[i].Init(&Ini, Buff);
			m_LiveSkill[i].EnablePickPut(false);
		}

		Ini.GetInteger("SkillText", "Font", 12, &m_SkillTextParam.nFont);
		Ini.GetInteger2("SkillText", "Offset",
			(int*)&m_SkillTextParam.Offset.cx, (int*)&m_SkillTextParam.Offset.cy);
		Ini.GetString("SkillText", "Color", "", Buff, 16);
		m_SkillTextParam.Color = GetColor(Buff);
	}
}

int KUiLiveSkillNew::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_LEFT_CLICK_ITEM && uParam && m_nRemainSkillPoint)
	{
		KUiDraggedObject* pObj = (KUiDraggedObject*)uParam;
		if (pObj->uGenre != CGOG_NOTHING)
		{
			m_nRemainSkillPoint--;	// 使用技能点数
			g_pCoreShell->OperationRequest(GOI_TONE_UP_SKILL, CGOG_SKILL_LIVE, pObj->uId);
		}
		return 0;
	}
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

void KUiLiveSkillNew::PaintWindow()
{
	KWndPage::PaintWindow();
	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
	{
		KUiDraggedObject	Obj;
		m_LiveSkill[i].GetObject(Obj);
		if (Obj.uGenre != CGOG_NOTHING)
		{
			int nLeft, nTop, nWidth, nHeight;
			m_LiveSkill[i].GetAbsolutePos(&nLeft, &nTop);
			m_LiveSkill[i].GetSize(&nWidth, &nHeight);
			g_pCoreShell->DrawGameObj(CGOG_SKILL_LIVE, Obj.uId, nLeft, nTop, nWidth, nHeight, Obj.DataW);
			if (Obj.DataW)
			{
				char	szLevel[8];
				int		nLen;
				itoa(Obj.DataW, szLevel, 10);
				nLen = strlen(szLevel);
				nLeft += m_SkillTextParam.Offset.cx;
				nTop += m_SkillTextParam.Offset.cy;
				nLeft += (nWidth - nLen * m_SkillTextParam.nFont / 2) / 2;
				g_pRepresentShell->OutputText(m_SkillTextParam.nFont, szLevel, nLen, nLeft, nTop,
					m_SkillTextParam.Color, 0);
			}
		}
	}
}

void KUiLiveSkillNew::UpdateBaseData()
{
	KUiPlayerLiveSkillBase	Info;
	g_pCoreShell->GetGameData(GDI_LIVE_SKILL_BASE, (unsigned int)&Info, 0);
	m_RemainSkillPoint.SetIntText(m_nRemainSkillPoint = Info.nRemainPoint);
}

void KUiLiveSkillNew::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	if (pSkill && nIndex >= 0 && nIndex < LIVE_SKILL_COUNT)
	{
		m_LiveSkill[nIndex].HoldObject(pSkill->uGenre, pSkill->uId, pSkill->nLevel, 0);
	}
}

void KUiLiveSkillNew::UpdateData()
{
	UpdateBaseData();
	KUiSkillData	Skills[LIVE_SKILL_COUNT];
	g_pCoreShell->GetGameData(GDI_LIVE_SKILLS, (unsigned int)Skills, 0);
	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
		m_LiveSkill[i].HoldObject(Skills[i].uGenre, Skills[i].uId, Skills[i].nLevel, 0);
}

KUiSkillsNew* KUiSkillsNew::m_pSelf = NULL;

KUiSkillsNew* KUiSkillsNew::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSkillsNew;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_FightSkillPad.UpdateData();
		m_pSelf->m_LiveSkillPad.UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiSkillsNew* KUiSkillsNew::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiSkillsNew::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiSkillsNew::Initialize()
{
	m_FightSkillPad.Initialize();
	AddPage(&m_FightSkillPad, &m_FightSkillPadBtn);
	//m_LiveSkillPad.Initialize();
	//AddPage(&m_LiveSkillPad, &m_LiveSkillPadBtn);

	AddChild(&m_Close);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}
extern int SCREEN_WIDTH;
void KUiSkillsNew::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	char		Buff1[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SHEET);
	if (m_pSelf && Ini.Load(Buff))
	{
		
		KUiPlayerBaseInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
		if (Info.nFirstAddFaction == -1) {
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SKILLS);
			return;
		}
		m_pSelf->UpdateFaction(Info.nFirstAddFaction);
		if (SCREEN_WIDTH == 1024) {
			sprintf(Buff1, "Main%d1024", Info.nFirstAddFaction);
		}
		else {
			sprintf(Buff1, "Main%d", Info.nFirstAddFaction);
		}
		m_pSelf->Init(&Ini, Buff1);
		//m_pSelf->m_FightSkillPadBtn.Init(&Ini, "FightBtn");
		//m_pSelf->m_LiveSkillPadBtn .Init(&Ini, "LiveBtn");
		m_pSelf->m_Close           .Init(&Ini, "CloseBtn");

		m_pSelf->m_LiveSkillPad.LoadScheme(pScheme);
		m_pSelf->m_FightSkillPad.LoadScheme(pScheme);
		if (SCREEN_WIDTH == 1024) {
			int nX, nY;
			int dX, dY;
			int wndWidth, wndHeight;
			int btnWidth, btnHeight;
			dX = (1024 - 800) / 2 + 8;
			dY = (768 - 600)/2;

			m_pSelf->GetPosition(&nX, &nY);
			m_pSelf->SetPosition(nX + dX, nY + dY);
			if (Info.nFirstAddFaction == 8 || Info.nFirstAddFaction == 4	
				|| Info.nFirstAddFaction == 5 || Info.nFirstAddFaction == 6 || Info.nFirstAddFaction == 10) {	// [HOASON 01/09]
				m_pSelf->m_Close.GetPosition(&nX, &nY);
				m_pSelf->m_Close.SetPosition(nX - 30, nY);
			}
		}
	}
}

void KUiSkillsNew::UpdateFaction(int nFaction) {
	m_FightSkillPad.UpdateFaction(nFaction);
}
void KUiSkillsNew::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	if (pSkill)
	{
		if (m_pSelf)
		{
			if (pSkill->uGenre == CGOG_SKILL_LIVE)
				m_pSelf->m_LiveSkillPad.UpdateSkill(pSkill, nIndex);
			else if (pSkill->uGenre == CGOG_SKILL_FIGHT)
				m_pSelf->m_FightSkillPad.UpdateSkill(pSkill, nIndex);
		}
		if (g_pCoreShell)
		{
			KUiPlayerAttribute	Info;
			memset(&Info, 0, sizeof(KUiPlayerAttribute));
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
			if (Info.nLevel <= SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE)
			{
				g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (unsigned int)pSkill, 1);
				KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.byParamSize = 0;
				Msg.byPriority = 0;
				Msg.eType = SMT_NORMAL;
				Msg.uReservedForUi = 0;

				KIniFile*	pIni = g_UiBase.GetCommConfigFile();
				if (pIni)
				{
					if (pIni->GetString("InfoString", AUTO_SET_IMMED_SKILL_MSG_ID, "", Msg.szMessage, sizeof(Msg.szMessage)))
					{
						KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
					}
					g_UiBase.CloseCommConfigFile();
				}
			}
		}
	}
}

void KUiSkillsNew::UpdateFightRemainPoint(int nPoint)
{
	m_FightSkillPad.UpdateRemainPoint(nPoint);
}

void KUiSkillsNew::UpdateLiveBaseData()
{
	m_LiveSkillPad.UpdateBaseData();
}

int KUiSkillsNew::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK && (KWndWindow*)uParam == (KWndWindow*)&m_Close)
	{
		Hide();	// 关闭装备框
		return 0;
	}
	return KWndPageSet::WndProc(uMsg, uParam, nParam);
}