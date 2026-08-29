/*******************************************************************************
File        : UiPartnerCommon.h
create data : 27-08-2026
Description : [BDH-G4] He BAN DONG HANH - phan dung chung cua 4 cua so client.
  Du lieu lay tu bang task value da duoc may chu dong bo tung o
  (KPlayer::SyncTaskValueToClient, id >= 256 qua kenh UI_TASKVALUE).
  So do o: KPlayerPartner.h (PTP_* / PTG_* nam ngoai #ifdef _SERVER).
*******************************************************************************/
#ifndef UIPARTNERCOMMON_H
#define UIPARTNERCOMMON_H

#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/KPlayerPartner.h"
#include "../../../core/src/KProtocol.h"    // keo theo Headers\KPartnerProtocol.h (PARTNER_OP_*)

extern iCoreShell* g_pCoreShell;

// doc mot o task value (ban sao phia client)
inline int PT_TaskVal(int nId)
{
    return g_pCoreShell ? g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, nId, 0) : 0;
}

// o rieng cua dong hanh nP (1-based)
inline int PT_P(int nP, int nOff)
{
    if (nP < 1 || nP > PARTNER_MAX_COUNT)
        return 0;
    return PT_TaskVal(PARTNER_TASK_P(nP - 1) + nOff);
}

inline int PT_Count()
{
    int n = 0;
    for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
        if (PT_P(i, PTP_USED) > 0)
            n++;
    return n;
}

inline int PT_Cur()
{
    int n = PT_TaskVal(PTG_CURPARTNER);
    if (n < 0 || n > PARTNER_MAX_COUNT)
        n = 0;
    return n;
}

// ten 16 byte nam trong 4 o int
inline void PT_GetName(int nP, char* szOut17)
{
    int* p = (int*)szOut17;
    for (int i = 0; i < 4; i++)
        p[i] = PT_P(nP, PTP_NAME0 + i);
    szOut17[16] = 0;
}

// id trong mot o ky nang dang pack id*1000+level
inline int PT_SkillId(int nPack)  { return nPack / 1000; }
inline int PT_SkillLv(int nPack)  { return nPack % 1000; }

// mot cua so partner co dang xem du lieu -> bao refresh khi task value doi
void UiPartner_OnTaskValueChanged(int nTaskId);

// phim tat thao tac: 0 talk / 1 callout / 2 attack / 3 follow / 4 select next
void UiPartner_HotKey(int nWhat);

#endif // UIPARTNERCOMMON_H
