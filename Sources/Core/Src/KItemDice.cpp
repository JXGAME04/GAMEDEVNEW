// KItemDice.cpp - he XUC XAC chia do (DICEITEM). Xem KItemDice.h + dac ta
//     D:\GAMEDEVNEW\DACTA_HE_XUCXAC_DICEITEM.md
//
// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KNpc.h"		// Npc[] - CallScript dat SCRIPT_SUBWORLDINDEX
#include "KPlayer.h"
#include "KPlayerDef.h"
#include "KSubWorldSet.h"
#include "KSortScript.h"
#include "KItemDice.h"
#include <string.h>
#include <stdio.h>

KItemDiceSet    g_ItemDiceSet;

extern int GetPlayerIndex(Lua_State* L);

//===========================================================================
// tien ich
//===========================================================================

// nguoi choi con hop le va con dung danh tinh cu khong.
// Khe nguoi choi CO THE bi tai dung cho nguoi khac trong luc phien dang chay,
// nen chi so khong du - phai doi chieu ca dwID.
static BOOL sDicePlayerAlive(int nPlayerIndex, DWORD dwPlayerId)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return FALSE;
    if (Player[nPlayerIndex].m_dwID != dwPlayerId)
        return FALSE;
    if (Player[nPlayerIndex].m_nNetConnectIdx < 0)
        return FALSE;
    return TRUE;
}

// ha chu thuong + doi '/' thanh '\' truoc khi tra script: loader bam duong dan
// dang '\' va chu thuong (KSortScript.cpp:161) - y het KJx2League.cpp:255.
static void sScriptPathLower(char* szOut, int nOutSize, const char* szIn)
{
    int i = 0;
    if (nOutSize <= 0)
        return;
    for (; szIn && szIn[i] && i < nOutSize - 1; i++)
    {
        char c = szIn[i];
        if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        else if (c == '/')
            c = '\\';
        szOut[i] = c;
    }
    szOut[i] = 0;
}

//===========================================================================
// KItemDice
//===========================================================================

void KItemDice::Reset()
{
    // KHONG giai phong vat pham o day - Reset dung ca luc khoi tao mang.
    // Nguoi goi phai tu quyet dinh so phan vat pham truoc khi Reset.
    memset(this, 0, sizeof(KItemDice));
}

BOOL KItemDice::Setup(int nType, int nMaxRoll, int nTime, int nPlayerCount)
{
    // = ban Linux 0x81ceef0: nMaxRoll == 0 hoac nType == 0 thi bo
    if (nType <= 0 || nMaxRoll <= 0)
        return FALSE;
    if (nTime <= 0)
        nTime = 1;
    if (nPlayerCount <= 0)
    {
        // = ban Linux 0x81cef23: nPlayerCount <= 0 -> [0x28] = 0x7fffffff, tuc
        // "khong biet truoc so nguoi" -> KHONG chot som, chi chot khi het gio.
        nPlayerCount = 0x7fffffff;
    }
    else if (nPlayerCount > MAX_DICE_PLAYER)
    {
        // ban Linux cap phat dong nen khong co tran; ta chan cung va ghi nhat ky
        // de con biet ma nang MAX_DICE_PLAYER neu that su co doi dong hon.
        g_DebugLog((LPSTR)"KItemDice: nPlayerCount %d > MAX_DICE_PLAYER, cat bot",
                   nPlayerCount);
        nPlayerCount = MAX_DICE_PLAYER;
    }
    m_nType     = nType;
    m_nMaxRoll  = nMaxRoll;
    m_nTimeLeft = nTime;
    m_nSize     = nPlayerCount;
    m_nState    = DICE_ST_RUN;
    return TRUE;
}

int KItemDice::FindPlayer(int nPlayerIndex)
{
    for (int i = 0; i < m_nPlayerCount; i++)
    {
        if (m_Players[i].nPlayerIndex == nPlayerIndex)
            return i;
    }
    return -1;
}

BOOL KItemDice::AddPlayer(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return FALSE;
    if (m_nPlayerCount >= MAX_DICE_PLAYER)
        return FALSE;
    if (FindPlayer(nPlayerIndex) >= 0)
        return TRUE;            // goi RollItem hai lan cho cung nguoi - bo qua

    KDicePlayer* p = &m_Players[m_nPlayerCount];
    p->nPlayerIndex = nPlayerIndex;
    p->dwPlayerId   = Player[nPlayerIndex].m_dwID;
    p->nNumber      = 0;
    p->nRollState   = DICE_ROLL_WAITING;
    strncpy(p->szName, Player[nPlayerIndex].m_PlayerName, MAX_DICE_NAME_LEN - 1);
    p->szName[MAX_DICE_NAME_LEN - 1] = 0;
    m_nPlayerCount++;
    return TRUE;
}

// Nguoi choi bam nut. nChoice: DICE_CHOICE_GIVEUP / DICE_CHOICE_NEED.
BOOL KItemDice::SetChoice(int nPlayerIndex, int nChoice)
{
    if (m_nState != DICE_ST_RUN)
        return FALSE;
    int k = FindPlayer(nPlayerIndex);
    if (k < 0)
        return FALSE;
    KDicePlayer* p = &m_Players[k];
    if (p->nRollState != DICE_ROLL_WAITING)
        return FALSE;           // da chon roi - chan bam hai lan
    if (!sDicePlayerAlive(p->nPlayerIndex, p->dwPlayerId))
        return FALSE;

    if (nChoice == DICE_CHOICE_NEED)
    {
        p->nNumber    = (int)g_Random(m_nMaxRoll) + 1;   // 1..nMaxRoll
        p->nRollState = DICE_ROLL_DONE;
    }
    else
    {
        p->nNumber    = 0;
        p->nRollState = DICE_ROLL_GIVEUP;
    }
    m_nDecided++;

    // bao diem cho ca nhom (client ve dong "<ten> gieo duoc <n> diem")
    SendAll(DICE_ACT_NUMBER, p->nNumber, p->szName);

    // goi lai kich ban: szCallFunc(nId, nNumber) voi PlayerIndex = nguoi vua gieo.
    // = ban Linux 0x81cf390, chuoi kieu tham so "dd".
    if (m_szCallFunc[0])
        CallScript(m_szCallFunc, "dd", p->nPlayerIndex, m_nId, p->nNumber, 0);

    // = ban Linux: chot SOM khi moi nguoi da chon, khong cho het gio
    if (m_nDecided >= m_nSize || m_nDecided >= m_nPlayerCount)
        Finish();
    return TRUE;
}

void KItemDice::Breathe()
{
    if (m_nState != DICE_ST_RUN)
        return;
    m_nTimeLeft--;
    if (m_nTimeLeft <= 0)
        Finish();
}

void KItemDice::Finish()
{
    if (m_nState != DICE_ST_RUN)
        return;
    m_nState = DICE_ST_DONE;

    // chon nguoi diem cao nhat trong so nhung nguoi DA GIEO
    m_nWinner    = 0;
    m_nWinNumber = 0;
    for (int i = 0; i < m_nPlayerCount; i++)
    {
        KDicePlayer* p = &m_Players[i];
        if (p->nRollState != DICE_ROLL_DONE)
            continue;
        if (!sDicePlayerAlive(p->nPlayerIndex, p->dwPlayerId))
            continue;           // thoat game giua chung -> khong duoc nhan
        if (p->nNumber > m_nWinNumber)
        {
            m_nWinNumber = p->nNumber;
            m_nWinner    = p->nPlayerIndex;
        }
    }

    // dong cua so tren may moi nguoi TRUOC khi trao, de client khoi bam them
    SendAll(DICE_ACT_CLOSE, 0);

    // trao vat pham. Khong kich ban Linux nao trao ca (xem dac ta muc 6.4)
    // nen engine phai la ben trao.
    if (m_nItemIndex > 0)
    {
        if (m_nWinner > 0)
        {
            int x = 0, y = 0;
            if (Player[m_nWinner].m_ItemList.CheckCanPlaceInEquipment(
                    Item[m_nItemIndex].GetWidth(), Item[m_nItemIndex].GetHeight(), &x, &y))
            {
                Player[m_nWinner].m_ItemList.AddKIL(m_nItemIndex, pos_equiproom,
                                                   x, y, false, true);
            }
            else
            {
                // tui day: giu nguyen loi JX1 van lam o LuaAddItem - dat vao tay.
                // Neu tay dang cam thu khac thi KHONG day xuong dat (khac LuaAddItem):
                // o day nguoi choi khong chu dong xin vat pham nen lam roi do cua ho
                // la sai; thay vao do bo phien thuong va bao ro.
                if (Player[m_nWinner].m_ItemList.Hand())
                {
                    ItemSet.Remove(m_nItemIndex);
                    m_nItemIndex = 0;
                    g_DebugLog((LPSTR)"KItemDice: tui day + tay dang cam, bo vat pham phien %d",
                               m_nId);
                }
                else
                {
                    Player[m_nWinner].m_ItemList.AddKIL(m_nItemIndex, pos_hand, 0, 0);
                }
            }
        }
        else
        {
            // khong ai nhan -> PHAI giai phong, neu khong la ro chi so vat pham
            ItemSet.Remove(m_nItemIndex);
            m_nItemIndex = 0;
        }
    }

    // Chon NGU CANH nguoi choi cho loi goi kich ban. Khi khong ai nhan thi
    // m_nWinner = 0; neu cu truyen 0 thi CallScript se KHONG dat PlayerIndex va
    // kich ban chay voi PlayerIndex CON SOT tu loi goi truoc (saizi.lua doc
    // YDBZ_sdl_getTaskByte ngay truoc vong lap -> doc nham nguoi khac).
    int nCtx = m_nWinner;
    if (nCtx <= 0)
    {
        for (int i = 0; i < m_nPlayerCount; i++)
        {
            if (sDicePlayerAlive(m_Players[i].nPlayerIndex, m_Players[i].dwPlayerId))
            {
                nCtx = m_Players[i].nPlayerIndex;
                break;
            }
        }
    }

    // goi kich ban chot: szDoneFunc(nId, nWinner, nNumber).
    // Ban Linux chi truyen 1 tham so (xem dac ta muc 6.3) - ta truyen du 3 cho dung
    // chu ky ma chinh kich ban goc khai bao.
    if (m_szDoneFunc[0])
        CallScript(m_szDoneFunc, "ddd", nCtx, m_nId, m_nWinner, m_nWinNumber);

    m_nState = DICE_ST_FREE;    // tra khe
}

// Do mo ta vat pham vao ChatItem - y het duong client dung o CoreShell.cpp:2310
// (GDI_ITEM_CHAT), de ben kia dung lai duoc vat pham bang ItemSet.Add*.
void KItemDice::FillItemDesc(ChatItem* pDesc)
{
    memset(pDesc, 0, sizeof(ChatItem));
    int nIdx = m_nItemIndex;
    if (nIdx <= 0)
        return;

    pDesc->m_nID          = Item[nIdx].GetID();
    pDesc->m_btGenre      = (BYTE)Item[nIdx].GetGenre();
    if (Item[nIdx].GetNature() >= NATURE_GOLD)
        pDesc->m_btDetail = (short)Item[nIdx].GetRow();
    else
        pDesc->m_btDetail = (short)Item[nIdx].GetDetailType();
    pDesc->m_btParticur   = (short)Item[nIdx].GetParticular();
    pDesc->m_btSeries     = (BYTE)Item[nIdx].GetSeries();
    pDesc->m_btLevel      = (BYTE)Item[nIdx].GetLevel();
    pDesc->m_bStack       = (BYTE)Item[nIdx].GetStackNum();
    pDesc->m_bEnChance    = (BYTE)Item[nIdx].GetEnChance();
    pDesc->m_nGoldId      = (short)Item[nIdx].GetGoldId();
    pDesc->m_btLuck       = (BYTE)Item[nIdx].m_GeneratorParam.nLuck;
    pDesc->m_dwRandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
    pDesc->m_wVersion     = (WORD)Item[nIdx].m_GeneratorParam.nVersion;
    pDesc->m_YearExp      = Item[nIdx].GetTime()->bYear;
    pDesc->m_Lock         = Item[nIdx].GetPlayerItemLock();
    pDesc->m_HLock        = Item[nIdx].GetPlayerItemHLock();
    pDesc->m_nNature      = Item[nIdx].GetNature();
    pDesc->m_nMaxOptMultiply = Item[nIdx].GetMaxOptMultiply();
    pDesc->m_uPrice       = Item[nIdx].GetPrice();
    pDesc->m_bPoint       = (BYTE)Item[nIdx].IsPurple();
    pDesc->m_nDurability  = Item[nIdx].GetDurability();
    for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        pDesc->m_btMagicLevel[i] = (short)Item[nIdx].m_GeneratorParam.nGeneratorLevel[i];
        if (Item[nIdx].m_GeneratorParam.nGeneratorLevel[i] > 10)
            pDesc->m_btMagicLevel[i + MAX_ITEM_MAGICATTRIB] =
                (short)Item[nIdx].m_aryMagicAttrib[i].nValue[0];
    }
}

// szWho: ten nguoi vua gieo, chi dung khi btAction = DICE_ACT_NUMBER
void KItemDice::SendTo(int nPlayerIndex, BYTE btAction, int nNumber,
                       const char* szWho)
{
    if (!g_pServer)
        return;
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return;
    // BOT khong co ket noi (m_nNetConnectIdx = -1) - gui la hong
    if (Player[nPlayerIndex].m_nNetConnectIdx < 0)
        return;

    DICE_ITEM_SYNC sync;
    memset(&sync, 0, sizeof(sync));
    sync.ProtocolType = s2c_diceitem;
    sync.m_btAction   = btAction;
    sync.m_nDiceId    = m_nId;
    sync.m_nTimeLeft  = m_nTimeLeft;
    sync.m_nNumber    = nNumber;
    if (szWho && szWho[0])
    {
        strncpy(sync.m_szName, szWho, MAX_DICE_NAME_LEN - 1);
        sync.m_szName[MAX_DICE_NAME_LEN - 1] = 0;
    }
    FillItemDesc(&sync.m_Item);
    g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx,
                                &sync, sizeof(DICE_ITEM_SYNC));
}

void KItemDice::SendAll(BYTE btAction, int nNumber, const char* szWho)
{
    for (int i = 0; i < m_nPlayerCount; i++)
    {
        KDicePlayer* p = &m_Players[i];
        if (!sDicePlayerAlive(p->nPlayerIndex, p->dwPlayerId))
            continue;
        SendTo(p->nPlayerIndex, btAction, nNumber, szWho);
    }
}

// Goi ham Lua trong tep kich ban co ten. szTypes = chuoi kieu tham so cua
// KLuaScript::CallFunction ("dd", "ddd", ...). nPlayerIndex > 0 thi dat
// SCRIPT_PLAYERINDEX truoc khi goi - y het KPlayer::ExecuteItemScriptJX2
// (KPlayer.cpp:6960).
void KItemDice::CallScript(const char* szFunc, const char* szTypes,
                           int nPlayerIndex, int nA, int nB, int nC)
{
    if (!szFunc || !szFunc[0] || !m_szFile[0])
        return;
    char szLow[MAX_PATH];
    sScriptPathLower(szLow, sizeof(szLow), m_szFile);

    KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
    if (!pScript)
    {
        // %.60s: g_DebugLog dem 256B + vsprintf khong chan
        g_DebugLog((LPSTR)"KItemDice: khong tim thay kich ban [%.60s]", szLow);
        return;
    }

    // LUON dat ngu canh, KE CA khi khong xac dinh duoc nguoi choi: de treo thi
    // kich ban se dung PlayerIndex CON SOT tu loi goi truoc do.
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(pScript->m_LuaState, nPlayerIndex);
        pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
        Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIndex].m_dwID);
        pScript->SetGlobalName(SCRIPT_PLAYERID);
        // moi loi goi kich ban khac trong cay deu dat ca bien nay
        // (KPlayer.cpp:6975/:7397, KMission.cpp:89, ScriptFuns.cpp:11287)
        if (Player[nPlayerIndex].m_nIndex > 0 && Player[nPlayerIndex].m_nIndex < MAX_NPC)
        {
            Lua_PushNumber(pScript->m_LuaState,
                           Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex);
            pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
        }
    }
    else
    {
        Lua_PushNumber(pScript->m_LuaState, 0);
        pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
        Lua_PushNumber(pScript->m_LuaState, 0);
        pScript->SetGlobalName(SCRIPT_PLAYERID);
    }

    int nTopIndex = 0;
    BOOL bOk;
    pScript->SafeCallBegin(&nTopIndex);
    if (strcmp(szTypes, "ddd") == 0)
        bOk = pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB, nC);
    else
        bOk = pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB);
    pScript->SafeCallEnd(nTopIndex);
    if (!bOk)
        g_DebugLog((LPSTR)"KItemDice: kich ban [%.40s] ham [%.30s] chay hong",
                   szLow, (char*)szFunc);
}

//===========================================================================
// KItemDiceSet
//===========================================================================

void KItemDiceSet::Init()
{
    for (int i = 0; i < MAX_ITEM_DICE; i++)
        m_Dice[i].Reset();
    m_nNextId = 1;
}

KItemDice* KItemDiceSet::Alloc()
{
    for (int i = 0; i < MAX_ITEM_DICE; i++)
    {
        if (m_Dice[i].m_nState == DICE_ST_FREE)
        {
            m_Dice[i].Reset();
            if (m_nNextId <= 0)
                m_nNextId = 1;          // tran thi quay ve 1
            m_Dice[i].m_nId = m_nNextId++;
            return &m_Dice[i];
        }
    }
    g_DebugLog((LPSTR)"KItemDice: het khe (MAX_ITEM_DICE = %d)", MAX_ITEM_DICE);
    return NULL;
}

KItemDice* KItemDiceSet::Find(int nId)
{
    if (nId <= 0)
        return NULL;
    for (int i = 0; i < MAX_ITEM_DICE; i++)
    {
        if (m_Dice[i].m_nId == nId && m_Dice[i].m_nState != DICE_ST_FREE)
            return &m_Dice[i];
    }
    return NULL;
}

void KItemDiceSet::Breathe()
{
    for (int i = 0; i < MAX_ITEM_DICE; i++)
    {
        if (m_Dice[i].m_nState == DICE_ST_RUN)
            m_Dice[i].Breathe();
    }
}

// Goi tu CoreServerShell::Breathe() moi tick. Tu do nhip 1 GIAY o day de khoi
// phu thuoc nhip khung hinh (18 fps) - dem nguoc cua xuc xac tinh bang giay.
void KItemDice_Breathe()
{
    static DWORD s_dwLast = 0;
    DWORD dwNow = GetTickCount();
    if (s_dwLast == 0)
    {
        s_dwLast = dwNow;
        return;
    }
    if (dwNow - s_dwLast < 1000)
        return;
    s_dwLast = dwNow;
    g_ItemDiceSet.Breathe();
}

//===========================================================================
// 8 ham Lua. Hop dong tung ham doc tu ma may ban Linux - xem dac ta muc 3.
//===========================================================================

// ApplyItemDice(nType, nMaxRoll, nTime, szFile, szDoneFunc, szCallFunc, nPlayerCount)
//   -> nId  (hoac 0 GIA TRI neu thieu tham so, y het ban Linux)
int LuaApplyItemDice(Lua_State* L)
{
    // ban Linux 0x81c1c40: cmp eax,5 / jle -> tra 0 gia tri  => doi >= 6
    if (Lua_GetTopIndex(L) <= 5)
        return 0;

    int nType     = (int)Lua_ValueToNumber(L, 1);
    int nMaxRoll  = (int)Lua_ValueToNumber(L, 2);
    int nTime     = (int)Lua_ValueToNumber(L, 3);
    const char* szFile = Lua_ValueToString(L, 4);
    const char* szDone = Lua_ValueToString(L, 5);
    const char* szCall = Lua_ValueToString(L, 6);
    // tham so 7 co the vang (ban Linux chi doi >= 6): vang -> 0 -> Setup coi la
    // "khong biet truoc so nguoi", chi chot khi HET GIO.
    int nCount    = (int)Lua_ValueToNumber(L, 7);

    KItemDice* pDice = g_ItemDiceSet.Alloc();
    if (!pDice)
        return 0;
    if (!pDice->Setup(nType, nMaxRoll, nTime, nCount))
    {
        pDice->Reset();
        return 0;
    }
    if (szFile)
    {
        strncpy(pDice->m_szFile, szFile, DICE_FILE_LEN - 1);
        pDice->m_szFile[DICE_FILE_LEN - 1] = 0;
    }
    if (szDone)
    {
        strncpy(pDice->m_szDoneFunc, szDone, DICE_FUNC_LEN - 1);
        pDice->m_szDoneFunc[DICE_FUNC_LEN - 1] = 0;
    }
    if (szCall)
    {
        strncpy(pDice->m_szCallFunc, szCall, DICE_FUNC_LEN - 1);
        pDice->m_szCallFunc[DICE_FUNC_LEN - 1] = 0;
    }
    Lua_PushNumber(L, pDice->m_nId);
    return 1;
}

// AddDiceItemInfo(nId, nQuality, nGenre, nDetail, nParticular, nLevel, nSeries,
//                 nRandomSeed, nLucky)   -> nItemIndex (0 = that bai)
// Ban Linux doi >= 9 tham so va CHI doc 9 tham so dau; Viem De truyen 12 - 3 cai
// cuoi la rac, bo qua (saizi.lua:8).
int LuaAddDiceItemInfo(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 9)
        return 0;

    int nId          = (int)Lua_ValueToNumber(L, 1);
    int nQuality     = (int)Lua_ValueToNumber(L, 2);
    int nGenre       = (int)Lua_ValueToNumber(L, 3);
    int nDetail      = (int)Lua_ValueToNumber(L, 4);
    int nParticular  = (int)Lua_ValueToNumber(L, 5);
    int nLevel       = (int)Lua_ValueToNumber(L, 6);
    int nSeries      = (int)Lua_ValueToNumber(L, 7);
    int nRandomSeed  = (int)Lua_ValueToNumber(L, 8);
    int nLucky       = (int)Lua_ValueToNumber(L, 9);

    KItemDice* pDice = g_ItemDiceSet.Find(nId);
    if (!pDice)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (pDice->m_nItemIndex > 0)
    {
        // goi hai lan cho cung mot phien: giu vat pham cu, khong tao them
        Lua_PushNumber(L, pDice->m_nItemIndex);
        return 1;
    }

    int nItemLevel[MAX_ITEM_MAGICATTRIB];
    ZeroMemory(nItemLevel, sizeof(nItemLevel));

    int nIndex;
    if (nQuality >= NATURE_GOLD)
    {
        // nQuality mang nghia NATURE khi >= NATURE_GOLD - cung cach client dung
        // o CoreShell.cpp GDI_ITEM_CHAT (dong 2240).
        nIndex = ItemSet.Add(nQuality, nGenre, nSeries, nLevel, nLucky,
                             nDetail, nParticular, nItemLevel,
                             g_SubWorldSet.GetGameVersion(), (UINT)nRandomSeed);
    }
    else
    {
        int xOpt = ItemSet.genXOpt(nLucky);
        nIndex = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLucky,
                                     nDetail, nParticular, nItemLevel,
                                     g_SubWorldSet.GetGameVersion(), (UINT)nRandomSeed,
                                     1, 0, 0, 0, 0, 0, 0, 0, 0, xOpt);
    }
    if (nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    pDice->m_nItemIndex = nIndex;
    Lua_PushNumber(L, nIndex);
    return 1;
}

// RollItem(nId) - chay voi PlayerIndex = nguoi se nhan cua so.
// Ghi nguoi choi vao phien roi mo o xuc xac tren may ho.
int LuaRollItem(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;

    int nId = (int)Lua_ValueToNumber(L, 1);
    KItemDice* pDice = g_ItemDiceSet.Find(nId);
    if (!pDice)
        return 0;
    if (!pDice->AddPlayer(nPlayerIndex))
        return 0;
    if (pDice->m_nItemIndex <= 0)
        return 0;       // = ban Linux: chua co vat pham thi KHONG gui goi
    pDice->SendTo(nPlayerIndex, DICE_ACT_OPEN, 0);
    return 0;
}

// GetItemDiceState(nId) -> trang thai (xem KDICE_STATE)
int LuaGetItemDiceState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    KItemDice* pDice = g_ItemDiceSet.Find((int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, pDice ? pDice->m_nState : DICE_ST_FREE);
    return 1;
}

// DiceLootItem(nId) -> (nItemIndex, szItemName)
//
// Ban Linux tra 2 gia tri va co doi trang thai. KHONG kich ban nao (Viem De lan
// Co Thap) goi ham nay. O ban JX1 engine DA tu trao vat pham luc chot (xem dac ta
// muc 6.4 va 7), nen neu ham nay cung trao thi se NHAN DOI vat pham. Vi vay o day
// no chi TRA THONG TIN, khong trao. Neu ve sau co kich ban that su can "tu doi"
// thi phai doi ca Finish() cho khoi trao hai lan.
int LuaDiceLootItem(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    KItemDice* pDice = g_ItemDiceSet.Find((int)Lua_ValueToNumber(L, 1));
    if (!pDice || pDice->m_nItemIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        Lua_PushString(L, (char*)"");
        return 2;
    }
    Lua_PushNumber(L, pDice->m_nItemIndex);
    Lua_PushString(L, Item[pDice->m_nItemIndex].GetName());
    return 2;
}

// GetItemDiceRollInfo(nId) -> (t, nSize, nWinner)
//   t[i] = { szName, nNumber, nTrangThai, laNguoiThang, nPlayerIndex }
//
// Ban Linux day DUNG 3 thu nay len ngan xep nhung `return 2` nen BANG bi roi mat
// (byte tho b8 02 00 00 00 @ 0x81c0c65). Ta tra du 3 - `local t, nSize = ...`
// cua saizi.lua:30 lay dung 2 gia tri dau nen van khop.
int LuaGetItemDiceRollInfo(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    KItemDice* pDice = g_ItemDiceSet.Find((int)Lua_ValueToNumber(L, 1));
    if (!pDice)
        return 0;

    Lua_NewTable(L);
    int nOut = 0;
    for (int i = 0; i < pDice->m_nPlayerCount; i++)
    {
        KDicePlayer* p = &pDice->m_Players[i];
        if (!sDicePlayerAlive(p->nPlayerIndex, p->dwPlayerId))
            continue;       // = ban Linux: nguoi da thoat khong xuat hien trong bang

        Lua_NewTable(L);
        Lua_PushString(L, p->szName);
        Lua_RawSetI(L, -2, 1);
        Lua_PushNumber(L, p->nNumber);
        Lua_RawSetI(L, -2, 2);
        Lua_PushNumber(L, p->nRollState);
        Lua_RawSetI(L, -2, 3);
        Lua_PushNumber(L, (p->nPlayerIndex == pDice->m_nWinner) ? 1 : 0);
        Lua_RawSetI(L, -2, 4);
        Lua_PushNumber(L, p->nPlayerIndex);
        Lua_RawSetI(L, -2, 5);

        Lua_RawSetI(L, -2, ++nOut);
    }
    Lua_PushNumber(L, pDice->m_nSize);
    Lua_PushNumber(L, pDice->m_nWinner);
    return 3;
}

// GetItemDiceItemInfo(nId)
//   -> nItemIndex, szItem, nQuality, nGenre, nDetail, nParticular, nLevel, nSeries
// Ban Linux tra dung 8 gia tri theo dung thu tu nay (0x81c04a0).
int LuaGetItemDiceItemInfo(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    KItemDice* pDice = g_ItemDiceSet.Find((int)Lua_ValueToNumber(L, 1));
    if (!pDice)
        return 0;
    int nIdx = pDice->m_nItemIndex;
    if (nIdx <= 0)
        return 0;

    Lua_PushNumber(L, nIdx);
    Lua_PushString(L, Item[nIdx].GetName());
    Lua_PushNumber(L, Item[nIdx].GetNature());      // "quality" cua ban Linux
    Lua_PushNumber(L, Item[nIdx].GetGenre());
    Lua_PushNumber(L, Item[nIdx].GetDetailType());
    Lua_PushNumber(L, Item[nIdx].GetParticular());
    Lua_PushNumber(L, Item[nIdx].GetLevel());
    Lua_PushNumber(L, Item[nIdx].GetSeries());
    return 8;
}

// GetItemDicePlayerList(nId) -> bang cac PlayerIndex CON ONLINE
// Ban Linux loc theo dai hop le 1..1199 va bo qua nguoi da thoat, nen getn(t) co
// the NHO HON nSize - saizi.lua:78 dua vao dung dieu do de in "Nguoi choi khac ve
// qua han thoi gian".
int LuaGetItemDicePlayerList(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    KItemDice* pDice = g_ItemDiceSet.Find((int)Lua_ValueToNumber(L, 1));
    if (!pDice)
        return 0;

    Lua_NewTable(L);
    int nOut = 0;
    for (int i = 0; i < pDice->m_nPlayerCount; i++)
    {
        KDicePlayer* p = &pDice->m_Players[i];
        if (!sDicePlayerAlive(p->nPlayerIndex, p->dwPlayerId))
            continue;
        Lua_PushNumber(L, p->nPlayerIndex);
        Lua_RawSetI(L, -2, ++nOut);
    }
    return 1;
}

#endif  // _SERVER
