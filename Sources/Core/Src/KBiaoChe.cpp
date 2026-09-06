//===========================================================================
// KBiaoChe.cpp - [LMBC 06/09] Long Mon Tieu Cuc: xe tieu bam theo chu.
// Xem KBiaoChe.h cho luat kien truc va nguon goc cac hang so.
//===========================================================================
#include "KCore.h"
#include "KBiaoChe.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KLuaScript.h"
#include "KSortScript.h"

#ifdef _SERVER

extern int GetPlayerIndex(Lua_State* L);

int g_nBiaoCheOn          = 1;
int g_nBiaoCheGiuKhiThoat = 1;

//---------------------------------------------------------------------------
// SO DANG KY. CO Y quet theo XE chu khong theo NGUOI CHOI: xe mo coi (chu vua
// dang xuat) khong con nguoi choi nao de quet toi, va o ban do vang thi AI cua
// no cung khong chay - chi so dang ky nay moi thay duoc no.
//---------------------------------------------------------------------------
struct KBiaoCheSlot
{
    int     nNpcIdx;
    DWORD   dwNpcID;
};
static KBiaoCheSlot s_Cart[BC_MAX_CART];
static int          s_nCart = 0;

static void BC_ClearSlot(int s)
{
    if (s < 0 || s >= BC_MAX_CART)
        return;
    if (s_Cart[s].nNpcIdx > 0 && s_nCart > 0)
        s_nCart--;
    s_Cart[s].nNpcIdx = 0;
    s_Cart[s].dwNpcID = 0;
}

static int BC_FindSlot(int nNpcIdx)
{
    for (int i = 0; i < BC_MAX_CART; i++)
        if (s_Cart[i].nNpcIdx == nNpcIdx)
            return i;
    return -1;
}

//---------------------------------------------------------------------------
// Tra xe theo NGUOI CHOI - O(1) qua cap the (chi so, m_dwID).
// TU LANH: KNpc::Init dat m_dwID = 0 va xoa Owner, nen khe NPC bi dung lai cho
// viec khac KHONG THE bi nham la xe cu.
//---------------------------------------------------------------------------
int BC_GetCart(int nPlayerIdx)
{
    if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
        return 0;
    KPlayer* pP = &Player[nPlayerIdx];
    int nIdx = pP->m_nBiaoCheIdx;
    if (nIdx > 0 && nIdx < MAX_NPC)
    {
        KNpc* pN = &Npc[nIdx];
        if (pN->m_dwID == pP->m_dwBiaoCheID && pN->m_btBiaoChe &&
            strcmp(pN->Owner, Npc[pP->m_nIndex].Name) == 0)
            return nIdx;
    }
    pP->m_nBiaoCheIdx = 0;
    pP->m_dwBiaoCheID = 0;
    return 0;
}

//---------------------------------------------------------------------------
// Tra chu theo XE. Bo nho dem truoc; truot moi quet mang nguoi choi.
// CO Y khong dung PlayerSet.GetFirstPlayer/GetNextPlayer: do la con tro duyet
// DUNG CHUNG co trang thai, ham khac dang xai.
//---------------------------------------------------------------------------
int BC_GetOwnerIdx(int nNpcIdx)
{
    if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
        return 0;
    KNpc* pN = &Npc[nNpcIdx];
    if (!pN->Owner[0])
        return 0;

    int n = pN->m_nBiaoCheOwner;
    if (n > 0 && n < MAX_PLAYER && Player[n].m_nIndex > 0 &&
        strcmp(Npc[Player[n].m_nIndex].Name, pN->Owner) == 0)
        return n;

    for (n = 1; n < MAX_PLAYER; n++)
    {
        if (Player[n].m_nIndex <= 0)
            continue;
        if (strcmp(Npc[Player[n].m_nIndex].Name, pN->Owner) == 0)
        {
            pN->m_nBiaoCheOwner = n;
            return n;
        }
    }
    pN->m_nBiaoCheOwner = 0;
    return 0;
}

BOOL BC_Attach(int nPlayerIdx, int nNpcIdx)
{
    if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)    return FALSE;
    if (nNpcIdx    <= 0 || nNpcIdx    >= MAX_NPC)       return FALSE;

    int s = -1;
    for (int i = 0; i < BC_MAX_CART; i++)
        if (s_Cart[i].nNpcIdx <= 0) { s = i; break; }
    if (s < 0)
    {
        AUTOLOG("[LMBC] so dang ky day %d xe - tu choi sinh xe", BC_MAX_CART);
        return FALSE;
    }
    s_Cart[s].nNpcIdx = nNpcIdx;
    s_Cart[s].dwNpcID = Npc[nNpcIdx].m_dwID;
    s_nCart++;

    Player[nPlayerIdx].m_nBiaoCheIdx = nNpcIdx;
    Player[nPlayerIdx].m_dwBiaoCheID = Npc[nNpcIdx].m_dwID;
    Npc[nNpcIdx].m_nBiaoCheOwner     = nPlayerIdx;
    Npc[nNpcIdx].m_dwBiaoCheLostTick = 0;
    Npc[nNpcIdx].m_btBiaoCheFlag     = 0;
    return TRUE;
}

void BC_UnlinkPlayer(int nNpcIdx)
{
    if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
        return;
    int n = Npc[nNpcIdx].m_nBiaoCheOwner;
    if (n > 0 && n < MAX_PLAYER && Player[n].m_nBiaoCheIdx == nNpcIdx)
    {
        Player[n].m_nBiaoCheIdx = 0;
        Player[n].m_dwBiaoCheID = 0;
    }
}

//---------------------------------------------------------------------------
// GO NPC. Chep DUNG khuon nha o LuaDelNpc: xac dang do_revive DA duoc tra bo
// dem o tu truoc, tru lan hai se AN MAT phan dem cua NPC KHAC dung chung o
// -> quai TANG HINH truoc moi phep va cham (loi da phai va ngay 31/08).
//---------------------------------------------------------------------------
void BC_HardRemove(int nNpcIdx)
{
    if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
        return;
    KNpc* pN = &Npc[nNpcIdx];
    if (pN->m_SubWorldIndex >= 0 && pN->m_RegionIndex >= 0)
    {
        SubWorld[pN->m_SubWorldIndex].m_Region[pN->m_RegionIndex].RemoveNpc(nNpcIdx);
        if (pN->m_Doing != do_revive)
            SubWorld[pN->m_SubWorldIndex].m_Region[pN->m_RegionIndex].DecRef(
                pN->m_MapX, pN->m_MapY, obj_npc);
    }
    NpcSet.Remove(nNpcIdx);
}

//---------------------------------------------------------------------------
// GOI NGUOC ENGINE -> LUA tren SCRIPT CUA XE, dang tham so (so, chuoi).
// Script Linux khai: function OnBiaoCheDisapper(nNpcIndex, szPlayerName)
// nen dinh dang phai la "ds". KNpc::ExecuteScript chi co "sd" nen tu goi.
// PlayerIndex/PlayerID dat = 0 (ban Linux ghi ro "PlayerIndex VO HIEU").
//---------------------------------------------------------------------------
void BC_FireCartScript(int nNpcIdx, char* szFun, char* szOwnerName)
{
    if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC || !szFun || !szFun[0])
        return;
    KNpc* pN = &Npc[nNpcIdx];

    DWORD dwId = pN->m_ActionScriptID;
    if (!dwId && pN->ActionScript[0])
        dwId = g_FileName2Id(pN->ActionScript);
    if (!dwId)
        return;

    KLuaScript* pS = (KLuaScript*)g_GetScript(dwId);
    if (!pS || !pS->m_LuaState)
        return;

    int nTop = 0;
    try
    {
        Lua_PushNumber(pS->m_LuaState, pN->m_Index);
        pS->SetGlobalName(SCRIPT_NPCINDEX);
        Lua_PushNumber(pS->m_LuaState, pN->m_dwID);
        pS->SetGlobalName(SCRIPT_NPCID);
        Lua_PushNumber(pS->m_LuaState, pN->m_SubWorldIndex);
        pS->SetGlobalName(SCRIPT_SUBWORLDINDEX);
        Lua_PushNumber(pS->m_LuaState, 0);
        pS->SetGlobalName(SCRIPT_PLAYERINDEX);
        Lua_PushNumber(pS->m_LuaState, 0);
        pS->SetGlobalName(SCRIPT_PLAYERID);

        pS->SafeCallBegin(&nTop);
        pS->CallFunction(szFun, 0, (LPSTR)"ds", pN->m_Index,
                         szOwnerName ? szOwnerName : (char*)"");
        pS->SafeCallEnd(nTop);
    }
    catch (...)
    {
        AUTOLOG("[LMBC] loi khi chay %s cua script xe %d", szFun, nNpcIdx);
    }
}

//---------------------------------------------------------------------------
// GOI NGUOC tren NGUOI CHOI (OnBiaoCheChangeMapNotice - khong tham so).
// CO Y KHONG dat m_ActionScriptID cua nguoi choi: dong do se dam vao hoi thoai
// NPC dang mo.
//---------------------------------------------------------------------------
static void BC_CallOwner(int nPlayerIdx, char* szFun, DWORD dwScriptId)
{
    if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)    return;
    if (Player[nPlayerIdx].m_nIndex <= 0)               return;
    if (!dwScriptId)                                    return;

    KLuaScript* pS = (KLuaScript*)g_GetScript(dwScriptId);
    if (!pS || !pS->m_LuaState)
        return;

    int nTop = 0;
    try
    {
        Lua_PushNumber(pS->m_LuaState, nPlayerIdx);
        pS->SetGlobalName(SCRIPT_PLAYERINDEX);
        Lua_PushNumber(pS->m_LuaState, Player[nPlayerIdx].m_dwID);
        pS->SetGlobalName(SCRIPT_PLAYERID);
        Lua_PushNumber(pS->m_LuaState,
                       Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex);
        pS->SetGlobalName(SCRIPT_SUBWORLDINDEX);

        pS->SafeCallBegin(&nTop);
        pS->CallFunction(szFun, 0, (LPSTR)"");
        pS->SafeCallEnd(nTop);
    }
    catch (...) { }
}

void BC_OnPlayerLogout(int nPlayerIdx)
{
    int nCart = BC_GetCart(nPlayerIdx);
    if (nCart <= 0)
        return;
    Npc[nCart].m_nBiaoCheOwner = 0;
    if (Npc[nCart].m_dwBiaoCheLostTick == 0)
        Npc[nCart].m_dwBiaoCheLostTick = (DWORD)g_SubWorldSet.GetGameTime();
    // KHONG go xe o day: ban Linux cho chu 5 phut de vao lai.
    // Dat g_nBiaoCheGiuKhiThoat = 0 neu muon go ngay.
    BC_UnlinkPlayer(nCart);
}

//===========================================================================
// NHIP TOAN CUC - 2 lan/giay, quet SO DANG KY (khong quet MAX_NPC/MAX_PLAYER).
//===========================================================================
void BC_Breathe()
{
    if (!g_nBiaoCheOn || s_nCart <= 0)
        return;

    static DWORD s_dwLast = 0;
    DWORD dwNow = GetTickCount();
    if (dwNow - s_dwLast < 500)
        return;
    s_dwLast = dwNow;

    const DWORD dwGT = (DWORD)g_SubWorldSet.GetGameTime();

    for (int s = 0; s < BC_MAX_CART; s++)
    {
        int   nCart = s_Cart[s].nNpcIdx;
        DWORD dwID  = s_Cart[s].dwNpcID;
        if (nCart <= 0)
            continue;

        KNpc* pC = &Npc[nCart];

        // (B1) khe da bi thu hoi / tai dung -> ban ghi tu don
        if (pC->m_dwID != dwID || !pC->m_btBiaoChe)
        {
            BC_ClearSlot(s);
            continue;
        }

        // (B2) XE DA CHET. Xu ly o NHANH CHINH: xac o vung ngu khong bao gio
        //      duoc OnRevive go nen phai tu don.
        if (pC->m_Doing == do_death || pC->m_Doing == do_revive)
        {
            if (!(pC->m_btBiaoCheFlag & 0x02))
            {
                pC->m_btBiaoCheFlag |= 0x02;
                pC->m_dwBiaoCheLostTick = dwGT;     // dung lam moc dem xac
                BC_UnlinkPlayer(nCart);             // chu duoc phep tao xe moi NGAY
                BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
            }
            if ((dwGT - pC->m_dwBiaoCheLostTick) > BC_CORPSE_TICK)
            {
                BC_ClearSlot(s);
                BC_HardRemove(nCart);               // KHONG DecRef khi do_revive
            }
            continue;
        }

        // (B3) HET HAN SONG - dat TRUOC moi dieu kien khac.
        //      Quy uoc: m_uFindPathMaxTime = 0 nghia la dung mac dinh, KHONG phai bat tu.
        BOOL bHetHan = FALSE;
        if (pC->m_uFindPathTime)
        {
            DWORD dwLive = pC->m_uFindPathMaxTime ? (DWORD)pC->m_uFindPathMaxTime
                                                  : (DWORD)BC_DEFAULT_LIVE;
            if ((dwGT - (DWORD)pC->m_uFindPathTime) > dwLive)
                bHetHan = TRUE;
        }

        int nOwner = BC_GetOwnerIdx(nCart);

        // (B4) CHU VANG MAT (dang xuat / chua vao lai)
        if (nOwner <= 0)
        {
            if (!g_nBiaoCheGiuKhiThoat)
                bHetHan = TRUE;
            if (pC->m_dwBiaoCheLostTick == 0)
                pC->m_dwBiaoCheLostTick = dwGT;
            if (bHetHan || (dwGT - pC->m_dwBiaoCheLostTick) > BC_LOST_TICK)
            {
                BC_ClearSlot(s);
                BC_UnlinkPlayer(nCart);
                BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
                if (Npc[nCart].m_dwID == dwID)      // script co the da go xe roi
                    BC_HardRemove(nCart);
            }
            continue;
        }

        pC->m_nBiaoCheOwner = nOwner;
        KPlayer* pP = &Player[nOwner];
        if (pP->m_nIndex <= 0 || pP->m_nIndex >= MAX_NPC)
            continue;
        KNpc* pO = &Npc[pP->m_nIndex];

        // noi lai the sau khi chu dang nhap lai
        if (pP->m_nBiaoCheIdx != nCart)
        {
            pP->m_nBiaoCheIdx = nCart;
            pP->m_dwBiaoCheID = dwID;
        }

        if (bHetHan)
        {
            // THU TU BAT BUOC: cat lien ket TRUOC -> ban script -> go NPC.
            // Neu ban script truoc, script co the sinh xe THAY THE va lenh go
            // sau do se xoa dung cai xe moi.
            BC_ClearSlot(s);
            BC_UnlinkPlayer(nCart);
            BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
            if (Npc[nCart].m_dwID == dwID)
                BC_HardRemove(nCart);
            continue;
        }

        // (B5) chu chet / hoi sinh -> xe DUNG CHO, khong tinh la lac chu
        if (pO->m_Doing == do_death || pO->m_Doing == do_revive)
        {
            pC->m_dwBiaoCheLostTick = 0;
            continue;
        }

        // (B6) CHU DOI BAN DO -> keo xe sang.
        if (pO->m_SubWorldIndex < 0)
            continue;                               // chu dang chuyen map
        if (pC->m_SubWorldIndex != pO->m_SubWorldIndex)
        {
            // gian cach thu lai 1 giay: tai dung m_uLastFindPathTime lam moc
            // (truong nay da thanh vo dung voi xe Long Mon).
            if (pC->m_uLastFindPathTime &&
                (dwGT - (DWORD)pC->m_uLastFindPathTime) < BC_RETRY_TICK)
                continue;
            pC->m_uLastFindPathTime = dwGT;

            int nX = 0, nY = 0;
            pO->GetMpsPos(&nX, &nY);
            if (pC->ChangeWorld((DWORD)SubWorld[pO->m_SubWorldIndex].m_SubWorldID,
                                nX + 32, nY + 32) > 0)
            {
                // XOA LENH TON DONG: ChangeWorld chi xoa lenh cho NGUOI CHOI that.
                // Khong xoa thi xe se chay ve toa do cua BAN DO CU ngay sau khi sang
                // map moi (lop loi "truot lui" da gap ngay 04/09).
                // m_Command la private nen dung duong cong khai SendCommand(do_none)
                // - dung dieu engine tu lam trong KNpc::Init (KNpc.cpp:268).
                pC->SendCommand(do_none);
                pC->m_dwBiaoCheLostTick = 0;
                pC->m_btBiaoCheFlag    &= ~0x01;
                BC_CallOwner(nOwner, (char*)"OnBiaoCheChangeMapNotice",
                             pC->m_ActionScriptID);
            }
            else if (pC->m_dwBiaoCheLostTick == 0)
                pC->m_dwBiaoCheLostTick = dwGT;
            continue;
        }

        // (B7) CUNG BAN DO - do binh phuong khoang cach.
        //      GetDistanceSquare tra -1 khi khac SubWorld -> so sanh KHONG DAU
        //      cho -1 roi dung nhanh "qua xa", y het lenh ja cua ban Linux.
        DWORD dwD2 = (DWORD)NpcSet.GetDistanceSquare(nCart, pP->m_nIndex);
        if (dwD2 > BC_FAR_DIST2)
        {
            if (!(pC->m_btBiaoCheFlag & 0x01))      // BAN MOT LAN, khong 2 lan/giay
            {
                pC->m_btBiaoCheFlag |= 0x01;
                BC_FireCartScript(nCart, (char*)"OnBiaoCheFarAwayPlayerDisapper",
                                  pC->Owner);
            }
            if (pC->m_dwBiaoCheLostTick == 0)
                pC->m_dwBiaoCheLostTick = dwGT;
            if ((dwGT - pC->m_dwBiaoCheLostTick) > BC_LOST_TICK)
            {
                BC_ClearSlot(s);
                BC_UnlinkPlayer(nCart);
                BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
                if (Npc[nCart].m_dwID == dwID)
                    BC_HardRemove(nCart);
            }
        }
        else
        {
            pC->m_dwBiaoCheLostTick = 0;
            pC->m_btBiaoCheFlag    &= ~0x01;
        }
    }
}

//===========================================================================
// SAU HAM LUA
//===========================================================================

// CreateBiaoChe(nSeries, nNpcSettingID, nLevel, szName, nSurviveTicks)
//   -> nNpcIndex hoac nil
int LuaCreateBiaoChe(Lua_State* L)
{
    if (!g_nBiaoCheOn || Lua_GetTopIndex(L) < 5)        return 0;   // nil
    int nPlayer = GetPlayerIndex(L);
    if (nPlayer <= 0 || nPlayer >= MAX_PLAYER)          return 0;
    if (Player[nPlayer].m_nIndex <= 0)                  return 0;
    if (BC_GetCart(nPlayer) > 0)                        return 0;   // mot chu mot xe

    int   nSeries  = (int)Lua_ValueToNumber(L, 1);
    int   nSetting = (int)Lua_ValueToNumber(L, 2);
    int   nLevel   = (int)Lua_ValueToNumber(L, 3);
    char* pszName  = Lua_IsString(L, 4) ? (char*)Lua_ValueToString(L, 4) : (char*)"";
    int   nTicks   = (int)Lua_ValueToNumber(L, 5);

    if (nSetting <= 0)                              return 0;
    if (nSeries < 0 || nSeries >= series_num)       nSeries = series_metal;
    if (nLevel  < 1)                                nLevel  = 1;
    if (nLevel  > 127)                              nLevel  = 127;
    if (nTicks  < 0)                                nTicks  = 0;

    KNpc* pO = &Npc[Player[nPlayer].m_nIndex];
    if (pO->m_SubWorldIndex < 0)                    return 0;
    int nX = 0, nY = 0;
    pO->GetMpsPos(&nX, &nY);

    // LUU Y THU TU THAM SO: AddNpcSet2(info, series, subworld, mpsX, mpsY) - DAO
    // so voi AddNpcSet1 cua ban Linux. HIWORD = setting, LOWORD = level.
    int nIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nSetting), nSeries,
                                 pO->m_SubWorldIndex, nX + 32, nY + 32);
    if (nIdx <= 0 || nIdx >= MAX_NPC)               return 0;

    KNpc* pC = &Npc[nIdx];

    // Ten xe bang cua Linux "Tieu Xa bang cua [<bang>]<nguoi>" chac chan > 31 byte.
    // Owner[32] nam NGAY SAU Name[32] nen phai dung g_StrCpyLen.
    if (pszName[0])
        g_StrCpyLen(pC->Name, pszName, sizeof(pC->Name));
    g_StrCpyLen(pC->Owner, pO->Name, sizeof(pC->Owner));

    pC->m_btBiaoChe          = 1;
    pC->m_btBiaoCheFlag      = 0;
    pC->m_bNpcFollowFindPath = TRUE;
    pC->m_uFindPathTime      = g_SubWorldSet.GetGameTime();
    pC->m_uFindPathMaxTime   = nTicks;      // 0 = mac dinh BC_DEFAULT_LIVE
    pC->m_uLastFindPathTime  = 0;
    pC->m_bNoRevive          = 1;           // chet la bien mat
    pC->m_dwBiaoCheLostTick  = 0;

    // Mau NPC xe bang co Camp = camp_event -> GenOneRelation tra relation_none
    // = KHONG AI DANH DUOC. Chi ep phe khi mau la camp_event; cac mau xe ca nhan
    // von da la camp_animal thi giu nguyen bang.
    if (pC->m_CurrentCamp == camp_event)
        pC->SetCurrentCamp(camp_animal);

    if (!BC_Attach(nPlayer, nIdx))
    {
        BC_HardRemove(nIdx);
        return 0;
    }
    Lua_PushNumber(L, nIdx);
    return 1;
}

// DeleteBiaoChe() - KHONG tham so
int LuaDeleteBiaoChe(Lua_State* L)
{
    int nPlayer = GetPlayerIndex(L);
    int nCart   = (nPlayer > 0) ? BC_GetCart(nPlayer) : 0;
    if (nCart <= 0) { Lua_PushNumber(L, 0); return 1; }

    int s = BC_FindSlot(nCart);
    if (s >= 0) BC_ClearSlot(s);
    BC_UnlinkPlayer(nCart);
    BC_HardRemove(nCart);
    Lua_PushNumber(L, 1);
    return 1;
}

// GetBiaoChePos() -> MpsX, MpsY, SubWorldIndex   |   -1
int LuaGetBiaoChePos(Lua_State* L)
{
    int nPlayer = GetPlayerIndex(L);
    int nCart   = (nPlayer > 0) ? BC_GetCart(nPlayer) : 0;
    if (nCart <= 0) { Lua_PushNumber(L, -1); return 1; }
    int nX = 0, nY = 0;
    Npc[nCart].GetMpsPos(&nX, &nY);
    Lua_PushNumber(L, nX);
    Lua_PushNumber(L, nY);
    Lua_PushNumber(L, Npc[nCart].m_SubWorldIndex);
    return 3;
}

// IsBiaoCheAlive() -> 1/0. Tra SO, KHONG tra TRUE/FALSE (Lua 5.4 tra 1/nil).
int LuaIsBiaoCheAlive(Lua_State* L)
{
    int nPlayer = GetPlayerIndex(L);
    int nCart   = (nPlayer > 0) ? BC_GetCart(nPlayer) : 0;
    int nAlive  = (nCart > 0 && Npc[nCart].m_Doing != do_death &&
                   Npc[nCart].m_Doing != do_revive) ? 1 : 0;
    Lua_PushNumber(L, nAlive);
    return 1;
}

// SyncBiaoCheDeathInfoToRelay(nNpcIndex)
// JX1 la MOT cum GameServer nen KHONG co goi relay tuong duong: o dot nay chi
// cat lien ket + ghi nhat ky. Ban Linux imul TRAN TRUI khong kiem bien - ta kiem.
int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)                 { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)     { Lua_PushNumber(L, 0); return 1; }
    if (!Npc[nNpcIdx].m_btBiaoChe)              { Lua_PushNumber(L, 0); return 1; }
    AUTOLOG("[LMBC] xe %d cua %s bi pha", nNpcIdx, Npc[nNpcIdx].Owner);
    BC_UnlinkPlayer(nNpcIdx);
    Lua_PushNumber(L, 1);
    return 1;
}

// BC_SetEnable(0/1) - cong tat nong, khong phai lui binary
int LuaBC_SetEnable(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
        g_nBiaoCheOn = ((int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
    Lua_PushNumber(L, g_nBiaoCheOn);
    return 1;
}

#endif  // _SERVER
