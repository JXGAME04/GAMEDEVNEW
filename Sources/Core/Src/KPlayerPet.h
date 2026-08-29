//---------------------------------------------------------------------------
// KPlayerPet.h - He BAN DONG HANH ban PC (petsys), port 100% tu Linux
// D:\ServerLinux\server1\script\petsys + client VLTK pet.ini (UiPet).
// KHAC he partner mobile (KPlayerPartner) - hai he song song nhu ban goc.
// Luu bang TASK VALUE 5110..5139 (tu sync client qua UI_TASKVALUE).
//---------------------------------------------------------------------------
#ifndef KPLAYERPET_H
#define KPLAYERPET_H

#include "KWin32.h"

// so do task value (client doc qua GDI_TASK_SAVE_VALUE)
#define PET_TV_BASE        5110
#define PET_TV_CREATE      (PET_TV_BASE + 0)    // =1 da co pet
#define PET_TV_LEVEL       (PET_TV_BASE + 1)    // 1..20
#define PET_TV_UPGRADE     (PET_TV_BASE + 2)    // diem thang cap
#define PET_TV_GROWN       (PET_TV_BASE + 3)    // diem tang truong
#define PET_TV_TAME        (PET_TV_BASE + 4)    // diem tu luyen
#define PET_TV_XIUZHEN     (PET_TV_BASE + 5)    // diem Tu Chan
#define PET_TV_FEATURE     (PET_TV_BASE + 6)    // npc template id (ngoai quan)
#define PET_TV_SUMMON      (PET_TV_BASE + 7)    // 1 = dang goi ra (persist)
#define PET_TV_ATTRIB0     (PET_TV_BASE + 8)    // 6 o: STR DEX VIT ENG HP MP
#define PET_TV_SKILL0      (PET_TV_BASE + 14)   // 4 o; o 1 = loai aura 1..4
#define PET_TV_NAME0       (PET_TV_BASE + 18)   // 4 int = 16 byte ten

#define PET_ATTRIB_COUNT   6
#define PET_SKILL_COUNT    4
#define PET_MAX_LEVEL      20
#define PET_PARTNER_NO     100                  // m_nPartnerNo danh dau pet petsys
#define PET_AURA_SKILL0    1600                 // 4 skill aura 1600..1603 (p19a)

#ifdef _SERVER
void Pet_ProcessAI(int nNpcIdx);        // KPlayerPartner.cpp re sang khi nNo==100
void Pet_Breathe();                     // KPartner_Breathe goi (moi ~0.5s)
void Pet_RunProtocol(int nPlayerIdx, int nOp);  // c2s_partnerop op PETSYS
void Pet_OnLogout(int nPlayerIdx);      // go npc khi thoat
#endif

#endif // KPLAYERPET_H
