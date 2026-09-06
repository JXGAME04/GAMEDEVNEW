# -*- coding: ascii -*-
"""b234_engine_xetieu.py -- [LMBC 06/09] BUOC B2+B3+B4: lop xe tieu + AI bam chu kieu Linux.

B2  them truong  : KNpc.h (4), KPlayer.h (2), khoi tao o KNpc::Init / AddNpcSet1 / KPlayer.cpp
B3  he xe tieu   : TAO KBiaoChe.h + KBiaoChe.cpp, moc BC_Breathe vao nhip, dang ky 6 ham Lua,
                   moc BC_OnPlayerLogout vao KPlayerSet::PrepareRemove, them vao Core.vcxproj
B4  AI bam chu   : sua KNpcAI::Activate (tiet che nhip) + ProcessAIFollow (nhanh xe Long Mon)

HANH VI BAM CHU LAY DUNG THEO BAN LINUX (do tu nhi phan jx_linux_y):
  - bam chu khi binh phuong khoang cach > 46224 (0xb490), so sanh CO DAU
  - bao "qua xa" khi > 262143 (0x3ffff), so sanh KHONG DAU (de gia tri -1 cua
    khac-ban-do roi dung nhanh nay, y het lenh ja cua ban goc)
  - lac chu qua 5400 tick (300 giay) thi xe bien mat
  - xe theo chu qua ban do; chu chet/hoi sinh thi xe DUNG CHO, khong tinh la lac chu

VI SAO VONG DOI PHAI O BC_Breathe CHU KHONG O AI: NPC chi duoc Activate khi VUNG cua no
dang co nguoi choi (KRegion::IsActive). Xe mo coi o ban do vang se khong bao gio chay AI,
nen moi dong ho (het han, lac chu, keo qua ban do, don xac) phai o nhip toan cuc.

LUAT: tep nguon ANSI/TCVN3 -> doc/ghi latin-1, CAM dung cong cu Edit/Write.
Chay lai duoc. So byte cao moi tep KHONG duoc doi.

dung: python b234_engine_xetieu.py [<goc worktree>]
"""
import io
import os
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_vantieu"
SRC = os.path.join(ROOT, "Sources", "Core", "Src")
PROJ = os.path.join(ROOT, "Sources", "Core", "Core.vcxproj")
MARK = "[LMBC 06/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def patch(name, edits, marker=MARK, base=None):
    path = os.path.join(base or SRC, name)
    d = rd(path)
    if marker in d:
        print("  da va tu truoc, bo qua:", name)
        return
    eol = "\r\n" if "\r\n" in d else "\n"
    before = hb(d)
    for old, new in edits:
        assert all(ord(ch) < 0x80 for ch in new), "ma chen phai ASCII: %s" % name
        o = old.replace("\n", eol)
        n = new.replace("\n", eol)
        c = d.count(o)
        assert c == 1, "NEO KHONG DUY NHAT (%d) trong %s:\n%r" % (c, name, old[:150])
        d = d.replace(o, n)
    assert hb(d) == before, "SO BYTE CAO DOI trong %s" % name
    assert marker in d
    bak = path + ".truoc_lmbc"
    if not os.path.isfile(bak):
        wr(bak, rd(path))
    wr(path, d)
    print("  da va:", name)


def newfile(name, body):
    path = os.path.join(SRC, name)
    assert all(ord(ch) < 0x80 for ch in body), "tep moi phai ASCII thuan"
    if os.path.isfile(path) and MARK in rd(path):
        print("  da co:", name)
        return
    wr(path, body.replace("\n", "\r\n"))
    print("  tao moi:", name)


# ===========================================================================
# KBiaoChe.h
# ===========================================================================
KBIAOCHE_H = r'''//===========================================================================
// KBiaoChe.h - [LMBC 06/09] Long Mon Tieu Cuc: xe tieu bam theo chu.
//
// LUAT KIEN TRUC - DOC TRUOC KHI SUA:
//   NPC chi duoc Activate khi VUNG cua no dang co nguoi choi (KRegion::IsActive).
//   Vi vay:
//     * AI cua xe (KNpcAI::ProcessAIFollow) chi lo NGAN HAN: bam sat, di chuyen.
//     * VONG DOI (het han, lac chu, keo qua ban do, don xac) BAT BUOC chay o
//       BC_Breathe - nhip toan cuc, KHONG phu thuoc vung co nguoi hay khong.
//   Moi thiet ke dat dong ho 30 phut / 5 phut trong KNpcAI deu CHET CUNG khi
//   chu di sang ban do khac va vung cua xe vang nguoi.
//
// CAC HANG SO LAY TU NHI PHAN BAN LINUX (jx_linux_y), giu dung kieu so sanh.
//===========================================================================
#ifndef _KBIAOCHE_H_
#define _KBIAOCHE_H_

#ifdef _SERVER

#define BC_MAX_CART         256         // so xe song cung luc
#define BC_FOLLOW_DIST2     46224       // Linux 0xb490 - so sanh CO DAU (jg)
#define BC_FAR_DIST2        262143u     // Linux 0x3ffff - so sanh KHONG DAU (ja)
#define BC_LOST_TICK        5400        // 300 giay o 18 tick/giay
#define BC_DEFAULT_LIVE     36000       // = MAX_FIND_PATH_NPC_TIME cua KNpcAI.cpp
#define BC_CORPSE_TICK      1200        // ~66 giay: xac chua duoc go thi ta tu don
#define BC_RETRY_TICK       18          // 1 giay: gian cach thu lai ChangeWorld
#define BC_AI_TICK          3           // ~6 lan/giay (ban goc chay 18 lan/giay)
#define BC_SCRIPT_DEFAULT   "\\script\\event\\lmbiaoche\\npc_lmbiaoche.lua"

extern int  g_nBiaoCheOn;           // cong tat nong (BC_SetEnable)
extern int  g_nBiaoCheGiuKhiThoat;  // 1 = giu xe 5 phut sau khi chu dang xuat (nhu Linux)

int     BC_GetCart(int nPlayerIdx);         // -> npc idx, 0 = khong co
int     BC_GetOwnerIdx(int nNpcIdx);        // -> player idx, 0 = chu khong online
BOOL    BC_Attach(int nPlayerIdx, int nNpcIdx);
void    BC_UnlinkPlayer(int nNpcIdx);       // chi cat the ben KPlayer
void    BC_HardRemove(int nNpcIdx);         // go NPC theo khuon nha (chong DecRef kep)
void    BC_FireCartScript(int nNpcIdx, char* szFun, char* szOwnerName);
void    BC_Breathe();                       // CoreServerShell.cpp nhip chinh
void    BC_OnPlayerLogout(int nPlayerIdx);  // KPlayerSet::PrepareRemove

#endif  // _SERVER
#endif  // _KBIAOCHE_H_
'''

# ===========================================================================
# KBiaoChe.cpp
# ===========================================================================
KBIAOCHE_CPP = r'''//===========================================================================
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
'''

# ===========================================================================
# B2: them truong
# ===========================================================================
KNPC_H = [
    ("\tint\t\t\t\tm_nPartnerNo;\t\t\t// con so may cua chu (1..3)\n",
     "\tint\t\t\t\tm_nPartnerNo;\t\t\t// con so may cua chu (1..3)\n"
     "\t// [LMBC 06/09] Long Mon Tieu Cuc - xe tieu. m_btBiaoChe la CONG DUY NHAT bat\n"
     "\t// moi hanh vi moi; van tieu cu (SetNpcOwner) khong dat co nay nen chay duong cu.\n"
     "\tBYTE\t\t\tm_btBiaoChe;\t\t\t// 1 = xe Long Mon Tieu Cuc\n"
     "\tBYTE\t\t\tm_btBiaoCheFlag;\t\t// bit 0x01 = da bao 'qua xa'; 0x02 = da xu ly chet\n"
     "\tint\t\t\t\tm_nBiaoCheOwner;\t\t// bo nho dem player idx cua chu (LUON kiem lai bang ten)\n"
     "\tDWORD\t\t\tm_dwBiaoCheLostTick;\t// moc lac chu / moc bat dau dem xac\n"),
]

KPLAYER_H = [
    ("\tint\t\t\t\tTongIsForbidSkill(int nSkillId);\t\t\t\t// 1 = bi cam (toan bo hoac rieng)\n",
     "\tint\t\t\t\tTongIsForbidSkill(int nSkillId);\t\t\t\t// 1 = bi cam (toan bo hoac rieng)\n"
     "\t// [LMBC 06/09] the yeu tro toi xe tieu. TU LANH: KNpc::Init dat m_dwID = 0 va\n"
     "\t// xoa Owner, nen khe NPC bi dung lai cho viec khac KHONG THE bi nham la xe cu.\n"
     "\tint\t\t\tm_nBiaoCheIdx;\t\t\t// chi so NPC cua xe (0 = khong co)\n"
     "\tDWORD\t\t\tm_dwBiaoCheID;\t\t\t// m_dwID cua xe\n"),
]

KNPC_CPP_INIT = [
    ("\tZeroMemory(m_nNpcParam, sizeof(m_nNpcParam));\n",
     "\tZeroMemory(m_nNpcParam, sizeof(m_nNpcParam));\n"
     "#ifdef _SERVER\n"
     "\t// [LMBC 06/09] xoa trang thai xe tieu khi khe NPC duoc tai su dung\n"
     "\tm_btBiaoChe = 0;\n"
     "\tm_btBiaoCheFlag = 0;\n"
     "\tm_nBiaoCheOwner = 0;\n"
     "\tm_dwBiaoCheLostTick = 0;\n"
     "#endif\n"),
]

KNPCSET_CPP = [
    ("\tNpc[i].m_bNpcFollowFindPath = FALSE;\n",
     "#ifdef _SERVER\n"
     "\t// [LMBC 06/09] reset trang thai xe tieu cung luc voi cac truong bam duong\n"
     "\tNpc[i].m_btBiaoChe = 0;\n"
     "\tNpc[i].m_btBiaoCheFlag = 0;\n"
     "\tNpc[i].m_nBiaoCheOwner = 0;\n"
     "\tNpc[i].m_dwBiaoCheLostTick = 0;\n"
     "#endif\n"
     "\tNpc[i].m_bNpcFollowFindPath = FALSE;\n"),
]

KPLAYER_CPP = [
    ("\tm_dwLogoutScriptID = 0;\n",
     "\tm_dwLogoutScriptID = 0;\n"
     "#ifdef _SERVER\n"
     "\tm_nBiaoCheIdx = 0;\t\t// [LMBC 06/09]\n"
     "\tm_dwBiaoCheID = 0;\n"
     "#endif\n"),
]

# ===========================================================================
# B3: moc nhip + moc dang xuat + dang ky ham Lua
# ===========================================================================
CORESERVERSHELL = [
    ("\tKPartner_Breathe();",
     "\tBC_Breathe();\t// [LMBC 06/09] xe tieu Long Mon: vong doi + keo qua ban do\n"
     "\tKPartner_Breathe();"),
]

KPLAYERSET_CPP = [
    ("\tPlayer[nIndex].ExecuteScript(\"\\\\script\\\\player\\\\playerlogout.lua\", \"main\", 0);\n",
     "\tBC_OnPlayerLogout(nIndex);\t// [LMBC 06/09] xe tieu: bat dau dem 5 phut cho chu vao lai\n"
     "\n"
     "\tPlayer[nIndex].ExecuteScript(\"\\\\script\\\\player\\\\playerlogout.lua\", \"main\", 0);\n"),
]

SCRIPTFUNS_REG = [
    # khai bao extern - dat truoc bang dang ky
    ("\t{\"SetNpcValue\",\t\tLuaSetNpcParam},\n",
     "\t{\"SetNpcValue\",\t\tLuaSetNpcParam},\n"),  # giu nguyen, chi de neo ton tai
    ("\t{\"GetNpcValue\",\t\tLuaGetNpcParam},\n",
     "\t{\"GetNpcValue\",\t\tLuaGetNpcParam},\n"
     "#ifdef _SERVER\n"
     "\t// == [LMBC 06/09] Long Mon Tieu Cuc (xe tieu bam theo chu) ==\n"
     "\t{\"CreateBiaoChe\",\t\t\t\tLuaCreateBiaoChe},\t\t\t\t// (series, setting, level, ten, ticks) -> npcidx | nil\n"
     "\t{\"DeleteBiaoChe\",\t\t\t\tLuaDeleteBiaoChe},\t\t\t\t// () -> 1/0\n"
     "\t{\"GetBiaoChePos\",\t\t\t\tLuaGetBiaoChePos},\t\t\t\t// () -> x, y, subworld | -1\n"
     "\t{\"IsBiaoCheAlive\",\t\t\t\tLuaIsBiaoCheAlive},\t\t\t\t// () -> 1/0\n"
     "\t{\"SyncBiaoCheDeathInfoToRelay\",\tLuaSyncBiaoCheDeathInfoToRelay},\t// (npcidx) -> 1/0\n"
     "\t{\"BC_SetEnable\",\t\t\t\tLuaBC_SetEnable},\t\t\t\t// cong tat nong\n"
     "#endif\n"),
]

# ===========================================================================
# B4: AI bam chu
# ===========================================================================
KNPCAI_CPP = [
    # (a) tiet che nhip cho xe Long Mon
    ("\tif (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)",
     "\t// [LMBC 06/09] duong nay nam TRUOC cong m_NextAITime nen chay 18 lan/giay, moi\n"
     "\t// lan FindAroundPlayer quet ten qua 9 vung. Xe Long Mon di tiet che nhu moi AI\n"
     "\t// khac; XE TIEU CU giu nguyen duong cu de khong doi hanh vi tinh nang dang chay.\n"
     "\tif (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)"),
    ("\t{\n\t\tProcessAIFollow();\n\t\treturn;\n\t}\n",
     "\t{\n"
     "\t\tif (Npc[m_nIndex].m_btBiaoChe)\n"
     "\t\t{\n"
     "\t\t\tint nCurTimeB = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime;\n"
     "\t\t\tif (Npc[m_nIndex].m_NextAITime > nCurTimeB)\n"
     "\t\t\t\treturn;\n"
     "\t\t\tNpc[m_nIndex].m_NextAITime = nCurTimeB + BC_AI_TICK;\n"
     "\t\t}\n"
     "\t\tProcessAIFollow();\n"
     "\t\treturn;\n"
     "\t}\n"),

    # (b) nhanh xe Long Mon trong ProcessAIFollow
    ("void\tKNpcAI::ProcessAIFollow()",
     "// [LMBC 06/09] Nhanh xe Long Mon Tieu Cuc duoc chen o dau ham. Phan con lai\n"
     "// (xe tieu CU cua du an) giu nguyen tung dong.\n"
     "void\tKNpcAI::ProcessAIFollow()"),
    ("\tint nIdx = Npc[m_nIndex].FindAroundPlayer(Npc[m_nIndex].Owner);\n",
     "\tKNpc*\tpMeLM = &Npc[m_nIndex];\n"
     "\tBOOL\tbLM   = pMeLM->m_btBiaoChe;\t// [LMBC 06/09] cong duy nhat\n"
     "\n"
     "\tint nIdx = 0;\n"
     "\tif (bLM)\n"
     "\t{\n"
     "\t\t// [LMBC 06/09] O(1): chu da biet -> khoi di bo danh sach lien ket + strcmp\n"
     "\t\t// qua MOI NPC cua toi da 9 vung. Day moi la cho nghen that su.\n"
     "\t\tint nPLM = pMeLM->m_nBiaoCheOwner;\n"
     "\t\tif (nPLM > 0 && nPLM < MAX_PLAYER && Player[nPLM].m_nIndex > 0 &&\n"
     "\t\t\tstrcmp(Npc[Player[nPLM].m_nIndex].Name, pMeLM->Owner) == 0)\n"
     "\t\t\tnIdx = Player[nPLM].m_nIndex;\n"
     "\t}\n"
     "\tif (nIdx <= 0)\n"
     "\t\tnIdx = Npc[m_nIndex].FindAroundPlayer(Npc[m_nIndex].Owner);\n"),
    ("\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)\n\t\treturn;\n",
     "\tif (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)\n\t\treturn;\n"
     "\n"
     "\tif (bLM)\n"
     "\t{\n"
     "\t\t// [LMBC 06/09] Ba lenh return o tren von co nghia 'BO CUOC VINH VIEN'. Voi xe\n"
     "\t\t// Long Mon chung chi con nghia 'nhip nay khong di chuyen': het han, lac chu,\n"
     "\t\t// keo qua ban do va don xac deu do BC_Breathe lo - vi AI KHONG chay khi vung\n"
     "\t\t// cua xe vang nguoi choi.\n"
     "\t\t//\n"
     "\t\t// KHONG dong vao phe: da dat MOT LAN luc sinh. Ban goc lat phe MOI KHUNG, moi\n"
     "\t\t// lan SetCurrentCamp phat s2c_npcchgcurcamp ra 9 vung (~324 goi vung/giay/xe).\n"
     "\t\t// KHONG tu go NPC: vong doi thuoc ve BC_Breathe.\n"
     "\t\tint nD2LM = NpcSet.GetDistanceSquare(m_nIndex, nIdx);\n"
     "\n"
     "\t\t// GIU TRAN TREN: qua BC_FAR_DIST2 thi NGUNG phat do_walk (xe ket sau tuong hoac\n"
     "\t\t// chu dich chuyen trong cung map se khong tim duong 6 lan/giay suot 30 phut).\n"
     "\t\tif (nD2LM > BC_FOLLOW_DIST2 && (DWORD)nD2LM <= BC_FAR_DIST2)\n"
     "\t\t{\n"
     "\t\t\tif (pMeLM->m_FightMode != Npc[nIdx].m_FightMode)\n"
     "\t\t\t\tpMeLM->m_FightMode = Npc[nIdx].m_FightMode;\n"
     "\n"
     "\t\t\tint nOwnerXLM, nOwnerYLM, nMpsXLM, nMpsYLM;\n"
     "\t\t\tNpc[nIdx].GetMpsPos(&nOwnerXLM, &nOwnerYLM);\n"
     "\t\t\tpMeLM->GetMpsPos(&nMpsXLM, &nMpsYLM);\n"
     "\t\t\tint nXGoLM = nOwnerXLM + ((nOwnerXLM - nMpsXLM) > 0 ? -1 : 1) * 50;\n"
     "\t\t\tint nYGoLM = nOwnerYLM + ((nOwnerYLM - nMpsYLM) > 0 ? -1 : 1) * 50;\n"
     "\t\t\tpMeLM->SendCommand(do_walk, nXGoLM, nYGoLM);\n"
     "\t\t}\n"
     "\t\treturn;\n"
     "\t}\n"),
]


def patch_includes():
    """Them #include KBiaoChe.h vao 4 tep dung ham BC_*."""
    for name, anchor in (
        ("CoreServerShell.cpp", None),
        ("KPlayerSet.cpp", None),
        ("ScriptFuns.cpp", None),
        ("KNpcAI.cpp", None),
    ):
        path = os.path.join(SRC, name)
        d = rd(path)
        if "KBiaoChe.h" in d:
            continue
        eol = "\r\n" if "\r\n" in d else "\n"
        before = hb(d)
        lines = d.split(eol)
        last = -1
        for i, l in enumerate(lines[:200]):
            if l.startswith("#include"):
                last = i
        assert last >= 0, "%s: khong thay #include o dau tep" % name
        lines.insert(last + 1, '#include "KBiaoChe.h"\t// [LMBC 06/09] xe tieu Long Mon')
        d = eol.join(lines)
        assert hb(d) == before
        bak = path + ".truoc_lmbc"
        if not os.path.isfile(bak):
            wr(bak, rd(path))
        wr(path, d)
        print("  da chen include:", name)


def patch_extern():
    """Khai bao extern 6 ham Lua trong ScriptFuns.cpp (truoc bang dang ky)."""
    path = os.path.join(SRC, "ScriptFuns.cpp")
    d = rd(path)
    if "LuaCreateBiaoChe" in d and "extern int LuaCreateBiaoChe" in d:
        return
    eol = "\r\n" if "\r\n" in d else "\n"
    before = hb(d)
    anchor = "TLua_Funcs GameScriptFuns[]"
    i = d.find(anchor)
    assert i > 0, "khong thay bang GameScriptFuns[]"
    block = (
        "#ifdef _SERVER" + eol +
        "// [LMBC 06/09] Long Mon Tieu Cuc - hien thuc o KBiaoChe.cpp" + eol +
        "extern int LuaCreateBiaoChe(Lua_State* L);" + eol +
        "extern int LuaDeleteBiaoChe(Lua_State* L);" + eol +
        "extern int LuaGetBiaoChePos(Lua_State* L);" + eol +
        "extern int LuaIsBiaoCheAlive(Lua_State* L);" + eol +
        "extern int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L);" + eol +
        "extern int LuaBC_SetEnable(Lua_State* L);" + eol +
        "#endif" + eol + eol
    )
    d = d[:i] + block + d[i:]
    assert hb(d) == before
    wr(path, d)
    print("  da chen extern 6 ham Lua: ScriptFuns.cpp")


def patch_vcxproj():
    d = rd(PROJ)
    if "KBiaoChe.cpp" in d:
        print("  da co trong Core.vcxproj")
        return
    eol = "\r\n" if "\r\n" in d else "\n"
    a1 = '    <ClCompile Include="Src\\base64.cpp" />'
    a2 = '    <ClInclude Include="Src\\base64.h" />'
    assert d.count(a1.replace("\n", eol)) == 1, "khong thay neo ClCompile base64.cpp"
    d = d.replace(a1, '    <ClCompile Include="Src\\KBiaoChe.cpp" />' + eol + a1)
    if d.count(a2) == 1:
        d = d.replace(a2, '    <ClInclude Include="Src\\KBiaoChe.h" />' + eol + a2)
    wr(PROJ, d)
    print("  da them vao Core.vcxproj")


def main():
    assert os.path.isdir(SRC), SRC
    print("goc:", ROOT)
    print("-- B3a tep moi --")
    newfile("KBiaoChe.h", KBIAOCHE_H)
    newfile("KBiaoChe.cpp", KBIAOCHE_CPP)
    print("-- B2 them truong --")
    patch("KNpc.h", KNPC_H)
    patch("KPlayer.h", KPLAYER_H)
    patch("KNpc.cpp", KNPC_CPP_INIT, marker="m_btBiaoChe = 0;")
    patch("KNpcSet.cpp", KNPCSET_CPP)
    patch("KPlayer.cpp", KPLAYER_CPP)
    print("-- B3b moc + dang ky --")
    patch_includes()
    patch_extern()
    patch("ScriptFuns.cpp", SCRIPTFUNS_REG, marker="LuaCreateBiaoChe},")
    patch("CoreServerShell.cpp", CORESERVERSHELL, marker="BC_Breathe();")
    patch("KPlayerSet.cpp", KPLAYERSET_CPP, marker="BC_OnPlayerLogout")
    patch_vcxproj()
    print("-- B4 AI bam chu --")
    patch("KNpcAI.cpp", KNPCAI_CPP, marker="m_btBiaoChe")
    print("XONG B2+B3+B4")


if __name__ == "__main__":
    main()
