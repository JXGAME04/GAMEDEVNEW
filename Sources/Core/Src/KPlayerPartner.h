//---------------------------------------------------------------------------
// KPlayerPartner.h  -  He BAN DONG HANH (partner / dong hanh)
//
// Port 100% ngu nghia tu ban Linux VNG (jx_linux_y):
//   - moi nguoi choi toi da PARTNER_MAX_COUNT dong hanh (binary goc:
//     KPartnerArray<KPlayerPartner,3>)
//   - sinh chi so tu GenInfo(feature,series,character,6 tu chat)+GenTime+RandSeed
//     theo bang settings\partner\ (aptitude_range/attrib_range/aptitude_mode/...)
//   - AI 4 tinh cach (character.txt AIMode 21..24), di theo chu, hon me khi chet
//   - 51 ham Lua PARTNER_* dang ky o ScriptFuns.cpp
//   - luu ben: TOAN BO trang thai ghi vao dai task value cua nguoi choi
//     (4600..5099) -> blob roledb KHONG doi dinh dang (danh sach (id,value) thua)
//
// Tai lieu do dac: D:\GAMEDEVNEW\PHANTICH_BANDONGHANH_2708.md
// File stub 2003 cu (#ifdef _PARTNER, hong cu phap) da duoc thay toan bo.
//---------------------------------------------------------------------------
#ifndef KPLAYERPARTNER_H
#define KPLAYERPARTNER_H

#include "KWin32.h"

//---------------------------------------------------------------------------
// Hang so chung (ca client lan server deu thay duoc)
//---------------------------------------------------------------------------
#define PARTNER_MAX_COUNT        3      // tran ban goc (KPartnerArray<,3>); script goc chan 5
#define PARTNER_UI_SLOT          5      // UI ve 5 the (BtnPartner_0..4)
#define PARTNER_APT_NUM          6      // 6 tu chat / 6 thuoc tinh
#define PARTNER_RESIST_NUM       5      // physics, cold, lighting, fire, poison (thu tu resist.txt)
#define PARTNER_TASKVALUE_NUM    81     // bien task rieng cua tung dong hanh (partner_task_def.txt)
#define PARTNER_NAME_LEN         16
#define PARTNER_SERIES_NUM       5
#define PARTNER_CHARACTER_NUM    4      // 1 dung manh / 2 ve chu / 3 luu manh / 4 nhu nhuoc
#define PARTNER_FEATURE_NUM      5      // 5 ngoai hinh (feature.txt)
#define PARTNER_PERIOD_NUM       3      // 3 thoi ky (lv 1/60/100)
#define PARTNER_MAX_LEVEL        100

// 4 loai ky nang (init_skill.ini): 0 khang / 1 tien thien / 2 tu hoc / 3 tuyet ky
#define PARTNER_SKTYPE_RESIST    0
#define PARTNER_SKTYPE_GIVEN     1
#define PARTNER_SKTYPE_LEARNT    2
#define PARTNER_SKTYPE_ULTIMATE  3
#define PARTNER_SK_RESIST_MAX    5
#define PARTNER_SK_GIVEN_MAX     8
#define PARTNER_SK_LEARNT_MAX    16
#define PARTNER_SK_ULT_MAX       1

// 6 thuoc tinh (thu tu attrib_range.txt): LIFE STRENGTH HITTARGETRATE DEFENCE SPEED LUCK
enum PARTNER_ATTRIB_IDX
{
	pattr_life = 0,
	pattr_strength,
	pattr_hitrate,
	pattr_defence,          // ne tranh (duobi)
	pattr_speed,
	pattr_luck,
	pattr_num,
};

//---------------------------------------------------------------------------
// So do luu ben tren dai TASK VALUE 4600..5099 (MAX_TASK da noi 4600->5200).
// Moi o 1 int, chi o khac 0 moi ton cho trong blob (dinh dang (id,value) thua).
// So thuc (attrib) luu fixed-point x10000.
//---------------------------------------------------------------------------
#define PARTNER_TASK_BASE        4600
#define PARTNER_TASK_END         5099   // het 4600+20+3*160-1 = 5099, ne 5100 (chuyen sinh)

// khoi CHUNG (10 o): 4600..4619
#define PTG_VERSION              (PARTNER_TASK_BASE + 0)   // =1 khi da khoi tao
#define PTG_CURPARTNER           (PARTNER_TASK_BASE + 1)   // 1-based, 0 = chua chon
#define PTG_CALLOUT              (PARTNER_TASK_BASE + 2)   // 1 = dang goi ra
#define PTG_BAGLEVEL             (PARTNER_TASK_BASE + 3)   // cap tui dong hanh 0..10
#define PTG_LASTCALLTIME         (PARTNER_TASK_BASE + 4)   // time_t lan goi gan nhat
#define PTG_CALLOUT_SWITCH       (PARTNER_TASK_BASE + 5)   // 1 = map cam goi (PARTNER_OFF)
#define PTG_FIGHTMODE            (PARTNER_TASK_BASE + 6)   // [BDH-G4] 0 = chu dong danh, 1 = chi theo

// khoi TUNG CON (160 o): con i (0..2) bat dau tai PARTNER_TASK_P(i)
#define PARTNER_TASK_BLOCK       160
#define PARTNER_TASK_P(i)        (PARTNER_TASK_BASE + 20 + (i) * PARTNER_TASK_BLOCK)
// offset trong khoi:
#define PTP_USED                 0      // =1 co dong hanh
#define PTP_FEATURE              1      // GenInfo[0] 1..5
#define PTP_SERIES               2      // GenInfo[1] 0..4
#define PTP_CHARACTER            3      // GenInfo[2] 1..4
#define PTP_APT0                 4      // GenInfo[3..8] tu chat 1..10 (6 o)
#define PTP_GENTIME              10     // time_t luc tao
#define PTP_RANDSEED             11
#define PTP_LEVEL                12
#define PTP_EXP                  13     // exp CONG DON toan bo (bang level_exp.txt la nguong tong)
#define PTP_EMOTION              14     // 0..100
#define PTP_PUNISHUNTIL          15     // time_t het phat hon me (khong duoc goi ra)
#define PTP_STANDBYSKILL         16
#define PTP_NAME0                17     // 16 byte ten = 4 int (17..20)
#define PTP_ATTRINIT0            21     // 6 o x10000 (21..26)
#define PTP_ATTRINC0             27     // 6 o x10000 (27..32)
#define PTP_SK_RESIST0           33     // 5 o, pack id*1000+level (33..37)
#define PTP_SK_GIVEN0            38     // 8 o pack (38..45)
#define PTP_SK_LEARNT0           46     // 16 o pack (46..61)
#define PTP_SK_ULT               62     // 1 o pack
#define PTP_SK_GIVENEXP0         63     // 8 o exp cua ky nang tien thien (63..70)
#define PTP_EMO_LASTDEC          71     // time_t lan giam than mat gan nhat
#define PTP_TASKVALUE0           77     // 81 o: bien task k (1..81) -> +77+k-1 (77..157)
// 158..159 du phong

//---------------------------------------------------------------------------
#ifdef _SERVER

// ---- cau hinh doc tu settings\partner\ (nap 1 lan luc boot) ----
struct KPartnerSetting            // partner_setting.ini
{
	int nEmotionMin, nEmotionMax, nEmotionInit, nEmotionDegBase, nEmotionDegStep;
	int nPunishTime;              // giay hon me khong duoc goi
	int nCalloutInterval;         // giay cooldown goi
	int nLifeRestore;             // van phan / 5 giay
};

struct KPartnerFeatureRow         // feature.txt (1..5)
{
	int nNpcIdxPeriod[PARTNER_PERIOD_NUM];
	int nSex;                     // 0 nam 1 nu
};

struct KPartnerCharacterRow       // character.txt (1..4)
{
	int nVisionRadius, nActiveRadius, nForceSync, nAIMaxTime, nAIMode;
	int nAIParam[9];
};

struct KPartnerResistRow          // resist.txt (series 0..4)
{
	float fInit[PARTNER_RESIST_NUM];
	float fInc[PARTNER_RESIST_NUM];
};

struct KPartnerAptRangeRow        // aptitude_range.txt (apt 1..10)
{
	float fMin[PARTNER_APT_NUM];
	float fMax[PARTNER_APT_NUM];
};

struct KPartnerAttribRangeRow     // attrib_range.txt (series 0..4)
{
	float fInitMin[PARTNER_APT_NUM], fInitMax[PARTNER_APT_NUM];
	float fIncMin[PARTNER_APT_NUM],  fIncMax[PARTNER_APT_NUM];
};

struct KPartnerAptModeRow         // aptitude_mode.txt (mode 1..2)
{
	int nWeight[PARTNER_APT_NUM][10];   // trong so bac tu chat 1..10
};

struct KPartnerInitSkillRow       // init_skill.ini (series 0..4)
{
	int nCount;
	int nType[4], nSkillId[4], nLevel[4], nExp[4];
};

#define PARTNER_EVENT_MAX        32
#define PARTNER_EVENT_DIALOG     5
struct KPartnerEventRow           // partner_event.ini (26 su kien)
{
	char szName[32];
	int  nOdds;                   // % phat thoai; 0 = tat
	int  nParam1;
	int  nCountM, nCountF;
	char szDialogM[PARTNER_EVENT_DIALOG][256];
	char szDialogF[PARTNER_EVENT_DIALOG][256];
};

struct KPartnerTables
{
	BOOL bLoaded;
	KPartnerSetting        Setting;
	KPartnerFeatureRow     Feature[PARTNER_FEATURE_NUM + 1];        // 1-based
	KPartnerCharacterRow   Character[PARTNER_CHARACTER_NUM + 1];    // 1-based
	KPartnerResistRow      Resist[PARTNER_SERIES_NUM];
	KPartnerAptRangeRow    AptRange[11];                            // 1-based 1..10
	KPartnerAttribRangeRow AttribRange[PARTNER_SERIES_NUM];
	KPartnerAptModeRow     AptMode[3];                              // 1-based 1..2
	KPartnerInitSkillRow   InitSkill[PARTNER_SERIES_NUM];
	int                    nLevelExp[PARTNER_MAX_LEVEL + 2];        // [1..100] nguong exp tong
	int                    nBagGrid[11][2];                         // [cap][0]=cot [1]=hang
	unsigned int           uUnActiveColor;
	int                    nEventCount;
	KPartnerEventRow       Event[PARTNER_EVENT_MAX];
	char                   szHostNameMan[32], szHostNameWoman[32], szFormat[32];

	BOOL Load();                  // doc \settings\partner\* ; tra FALSE neu thieu bang
};
extern KPartnerTables g_PartnerTables;

//---------------------------------------------------------------------------
// KPartnerSys - gan vao KPlayer (m_cPartner). KHONG giu trang thai ben nao
// ngoai task value; chi giu runtime (npc index cua con dang goi ra...).
//---------------------------------------------------------------------------
class KPartnerSys
{
public:
	int   m_nPlayerIdx;
	int   m_nNpcIdx;              // npc cua con dang goi ra (0 = khong)
	DWORD m_dwNpcID;              // xac thuc khe npc (chong tai dung)
	int   m_nNpcOfPartner;        // 1-based: npc dang la con so may
	DWORD m_dwLastRegenTick;      // hoi mau 5s
	DWORD m_dwLastEmoTick;        // kiem giam than mat (moi 60s kiem 1 lan)

	void  Init(int nPlayerIdx);   // goi khi khe player duoc cap / logout

	// ---- truy xuat kho task value ----
	int   GetG(int nId) const;                  // doc o chung (id tuyet doi)
	void  SetG(int nId, int nVal);
	int   GetP(int nP, int nOff) const;         // doc o cua con nP (1-based)
	void  SetP(int nP, int nOff, int nVal);

	int   Count() const;                        // so dong hanh (0.., -1 loi)
	int   GetCur() const;                       // con hien tai 1-based (0 = chua)
	BOOL  IsCallOut() const;
	BOOL  IsUsed(int nP) const;

	// ---- vong doi ----
	int   AddFightPartner(int nFeature, int nSeries, int nCharacter,
	                      const int* pnApt /*6 gia tri*/, int nAptMode);
	                                            // tra index 1-based, 0 = that bai
	int   RemovePartner(int nP);
	int   SetCurPartner(int nP);
	int   CallOut(int nFlag);                   // trieu hoi / thu ve con hien tai
	void  Recall(BOOL bCorpseToo);              // go npc khoi map (im lang)
	void  OnPlayerLogout();
	void  OnPlayerChangeWorld();                // goi sau khi chu sang map moi
	void  Breathe();                            // moi tick (goi tu KPartner_Breathe)

	// ---- chi so ----
	void  RollAttribs(int nP);                  // sinh init+inc tu tu chat (randseed)
	void  ReGenAttribsInc(int nP);              // roll lai tang truong (lv 10/30/50/90)
	void  ApplyToNpc(int nP);                   // ap chi so + skill vao npc dang goi ra
	int   AddExp(int nP, int nExp, int nShowMsg);
	void  LevelUpTo(int nP, int nNewLevel);     // xu ly len cap (goi script)
	int   GetName(int nP, char* szOut /*>=17*/) const;
	void  SetName(int nP, const char* szName);

	// ---- ky nang ----
	int   AddSkill(int nP, int nType, int nSkillId, int nLevel, int nExp);
	int   RemoveSkill(int nP, int nType, int nSkillId);
	void  RemoveAllSkill(int nP, int nType);
	int   GetSkillInfo(int nP, int nSkillId, int* pnLevel, int* pnExp) const;
	int   GetSkillSlot(int nP, int nType, int nSlot) const;   // pack id*1000+lv

	// ---- than mat / su kien thoai ----
	void  AddEmotion(int nP, int nDelta);
	void  FireEvent(const char* szEvent);       // phat thoai theo partner_event.ini

	// ---- tien ich ----
	int   NpcTemplateOf(int nP) const;          // template theo feature+level(period)
	int   PeriodOfLevel(int nLevel) const;      // 1..3
	KPartnerSys() { m_nPlayerIdx = 0; m_nNpcIdx = 0; m_dwNpcID = 0; m_nNpcOfPartner = 0;
	                m_dwLastRegenTick = 0; m_dwLastEmoTick = 0; }
};

// ---- moc goi tu ngoai ----
void KPartner_Breathe();                        // CoreServerShell::Breathe
void Partner_ProcessAI(int nNpcIdx);            // KNpcAI::Activate (kind_partner)
void Partner_OnNpcDeath(int nNpcIdx);
void Partner_RunTalkScript(int nPlayerIdx);   // [BDH-G4] mo doi thoai partner_talk.lua           // KNpc::OnDeath nhanh kind_partner
void Partner_WriteLog(const char* fmt, ...);    // Logs\KSG_PartnerLog_*.txt
int  Partner_BagCellActive(int nPlayerIdx, int nX, int nY); // o (x,y) da mo theo cap tui?

#endif // _SERVER

#endif // KPLAYERPARTNER_H
