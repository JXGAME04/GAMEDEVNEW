# DAC TA THI CONG — ENGINE C++ CHO "LONG MON TIEU CUC" (XE TIEU BANG)

Nen: **PA-B "Nan ProcessAIFollow"** (diem cao nhat 8.0) + ghep cac muc `de_xuat_ghep` cua ca 6 giam khao.
Da mo nguon kiem chung lai truoc khi viet (ket qua kiem chung o muc 0).

---

## 0. NHUNG DIEU DA KIEM CHUNG LAI TRONG LUOT NAY (thay doi 3 quyet dinh so voi PA goc)

| Kiem tra | Ket qua doc nguon | He qua cho dac ta |
|---|---|---|
| `grep -rl "KNpc\.h"` toan `Sources/` | Chi **Core/Src (51 tep) + Core/KMeridian.h + Engine/Src/LuaFuns.cpp**. Ma `Engine/Src/LuaFuns.cpp:1` la `#ifdef USEOLD`, va `USEOLD` **khong duoc dinh nghia o bat ky dau** (grep toan cay chi ra dung 2 dong `#ifdef USEOLD`). | **Noi `MAX_NPCPARAM` 4→8 LA AN TOAN.** Khong phai swap dong bo 4 nhi phan. Chi can **Rebuild ca hai cau hinh cua project Core** (Win32 client + x64 server). Ly le "ABI Engine.dll" cua PA "toi thieu rui ro" khong con hieu luc. |
| `ScriptFuns.cpp:4172 / 4183 / 16114-16115` | `LuaNPCINFO_GetNpcCurrentLife` / `LuaNPCINFO_SetNpcCurrentLife` **DA VIET, DA DANG KY, DA KIEM BIEN va da kep [0, m_CurrentLifeMax]**. | **KHONG viet lai `SetNpcCurLife`/`GetNpcCurLife`.** Chi them 2 dong **BI DANH** vao bang. Tiet kiem ~30 dong va mot duong loi. |
| `ScriptFuns.cpp:7238-7243` (LuaDelNpc) | Khuon chot nha da co: `if (m_Doing != do_revive)` moi `DecRef`, kem chu thich `[REFOAN-VS 31/08]`. | `BC_HardRemove` **bat buoc chep dung khuon nay**. Day la loi nghiem trong ma **ca 6 giam khao** deu bat. |
| `KNpc.cpp:4769-4788` | Cong Owner thu hai co **13 lenh `ZeroMemory` + `IgnoreState(TRUE)`** truoc `return FALSE`, khac han cong 4054. | Ban va bien chi duoc **them cong kiem bien**, **CAM** chep chung mot mau cho hai cho. |
| `KNpcAI.cpp:66-70` | Nhanh `Owner[0] && m_bNpcFollowFindPath` nam **truoc** cong `m_NextAITime`, va toan bo khoi da nam trong `#ifdef _SERVER` (49→130). | Sua tai cho, **khong can them guard moi**. |

---

## 1. DANH SACH TEP (10 tep sua + 2 tep tao + 2 tep du an)

Goc: `D:\GAMEDEVNEW_wt_vantieu\Sources\Core\Src\`

| # | Tep | Moi | Viec — kem dong neo |
|---|---|---|---|
| 1 | `GameDataDef.h` | khong | **:437** `MAX_NPCPARAM 4 → 8`. Script Linux dung toi chi so 6. **Phai cung mot commit voi muc 8** (kiem bien). |
| 2 | `KNpc.h` | khong | Them **4 truong** vao khoi `#ifdef _SERVER` san co **:516-524** (ngay truoc `#endif` dong 524, canh `m_nPartnerNo`). Khong dung phan dung chung client. |
| 3 | `KNpc.cpp` | khong | (a) `Init()` **~:280** (canh `ZeroMemory(m_nNpcParam...)`): xoa 4 truong moi, boc `#ifdef _SERVER`. (b) **VA LOI CO SAN** cong Owner **:4054-4064** — them kiem bien `m_nPlayerIdx`. (c) **VA LOI CO SAN** cong Owner **:4769-4788** — them kiem bien **NHUNG GIU NGUYEN** 13 `ZeroMemory` + `IgnoreState(TRUE)`. |
| 4 | `KNpcSet.cpp` | khong | `AddNpcSet1` **:501-505** (cho dang reset `m_bNpcFollowFindPath`): reset 4 truong moi trong khoi `#ifdef _SERVER` ke duoi. |
| 5 | `KNpcAI.cpp` | khong | (a) **:66-70** cho xe Long Mon di qua cong `m_NextAITime`. (b) **:139-223** `ProcessAIFollow`: them nhanh `bLM` — tra chu O(1), bo lat phe, giu tran khoang cach, khong tu go NPC. Xe tieu CU (`event_vantieu\tieudau.lua:169`) chay y nguyen duong cu. |
| 6 | `KPlayer.h` | khong | Them **2 truong** vao khoi `#ifdef _SERVER` **:747+** (cuoi khoi WLLS/TONG). |
| 7 | `KPlayer.cpp` | khong | **:327** (canh `m_dwLogoutScriptID = 0`): khoi tao 2 truong = 0. |
| 8 | `KPlayerSet.cpp` | khong | `PrepareRemove` **:357**, ngay sau `m_cChat.OffLine(...)`: goi `BC_OnPlayerLogout(nIndex)`. |
| 9 | `ScriptFuns.cpp` | khong | (a) **:7468-7484** + **:7486-7502** kiem bien `m_nNpcParam`. (b) **:6995** `strcpy(Owner)` → `g_StrCpyLen`. (c) **:7452** `strcpy(Name)` → `g_StrCpyLen`. **KHONG dung :7449** (`GetString` da co `sizeof`). (d) Khai bao extern + **8 dong dang ky** vao `GameScriptFuns[]` (**:14896**, chen canh **:15296-15299**). (e) `#include "KBiaoChe.h"`. |
| 10 | `CoreServerShell.cpp` | khong | **:1203**, ngay sau `KPartner_Breathe();`: them `BC_Breathe();` — **dat NGOAI** cap `QueryPerformanceCounter(&t1)/(&t2)` de khong lam ban o dem `s_tPB` cua bo do BotPerf. |
| 11 | `KBiaoChe.h` | **TAO** | Khai bao + hang so. Ca tep boc `#ifdef _SERVER`. |
| 12 | `KBiaoChe.cpp` | **TAO** | So dang ky 256 xe, `BC_Breathe`, 5 ham Lua moi, `BC_FireCartScript`, `BC_CallOwner`. Ca than tep boc `#ifdef _SERVER`. |
| 13 | `Core.vcxproj` (+`.filters`) | khong | Them `Src\KBiaoChe.cpp` / `Src\KBiaoChe.h` theo khuon `KPlayerPartner.cpp`. **Mot project nay bien dich CA hai cau hinh.** |
| 14 | `bin\server\script\event\lmbiaoche\lib_lmbiaoche.lua` | **TAO** | `WriteYunBiaoLog` **thuan Lua** boc `WriteStringToFile`. Khong can C++. |

---

## 2. MA C++ COT LOI (viet san de chep)

> Tat ca chu thich **ASCII thuan**. Tep nguon la ANSI/TCVN3 → sua bang **python latin-1**, CAM dung Edit/Write tool.

### 2.1 `KNpc.h` — them vao khoi `#ifdef _SERVER` :516-524 (truoc `#endif`)

```cpp
	// [LMBC 06/09] Long Mon Tieu Cuc - xe tieu bang. m_btBiaoChe la CONG DUY NHAT
	// bat moi hanh vi moi; van tieu ca nhan cu (SetNpcOwner) khong dat co nay nen
	// chay y nguyen duong cu.
	BYTE			m_btBiaoChe;			// 1 = xe Long Mon Tieu Cuc
	BYTE			m_btBiaoCheFlag;		// bit 0x01 = da bao "qua xa"; 0x02 = da xu ly chet
	int				m_nBiaoCheOwner;		// bo nho dem player idx cua chu (LUON kiem lai bang ten)
	DWORD			m_dwBiaoCheLostTick;	// moc lac chu / moc bat dau dem xac (0 = dang thay chu)
```

### 2.2 `KPlayer.h` — them vao cuoi khoi `#ifdef _SERVER` :747+

```cpp
	// [LMBC 06/09] the yeu tro toi xe tieu. TU LANH: KNpc::Remove -> Init() dat
	// m_dwID = 0 (KNpc.cpp:255) va memset(Owner) (KNpc.cpp:441), nen khe NPC bi
	// dung lai KHONG THE bi nham. Khong can moc "khi xe chet".
	int				m_nBiaoCheIdx;			// chi so NPC cua xe (0 = khong co)
	DWORD			m_dwBiaoCheID;			// m_dwID cua xe
```

### 2.3 `KBiaoChe.h` (TEP MOI)

```cpp
//===========================================================================
// KBiaoChe.h - Long Mon Tieu Cuc (van tieu BANG).
//
// LUAT KIEN TRUC - DOC TRUOC KHI SUA:
//   NPC chi duoc Activate khi VUNG cua no dang co nguoi choi
//   (KRegion::IsActive, KRegion.h:127; KSubWorld::Activate, KSubWorld.cpp:1130-1141
//   con cho ca ban do TRONG ngu SUBWORLD_KHUNG_NGU khung). Vi vay:
//     * AI cua xe (KNpcAI::ProcessAIFollow) chi lo NGAN HAN: bam sat, di chuyen.
//     * VONG DOI (het han, lac chu, keo qua ban do, thu don xac) BAT BUOC chay o
//       BC_Breathe - nhip toan cuc theo so dang ky, KHONG phu thuoc vung co nguoi.
//   Moi thiet ke dat dong ho 30 phut / 5 phut trong KNpcAI deu CHET CUNG.
//===========================================================================
#ifndef _KBIAOCHE_H_
#define _KBIAOCHE_H_

#ifdef _SERVER

#define BC_MAX_CART			256			// so xe song cung luc
#define BC_FOLLOW_DIST2		46224		// Linux 0xb490 - so sanh CO DAU (jg)
#define BC_FAR_DIST2		262143u		// Linux 0x3ffff - so sanh KHONG DAU (ja)
#define BC_LOST_TICK		5400		// 300 giay o 18 tick/giay
#define BC_DEFAULT_LIVE		36000		// quy uoc engine (KNpcAI.cpp:137 MAX_FIND_PATH_NPC_TIME)
#define BC_CORPSE_TICK		1200		// ~66 giay: xac chua duoc OnRevive go thi ta tu don
#define BC_RETRY_TICK		18			// 1 giay: gian cach thu lai ChangeWorld
#define BC_AI_TICK			3			// ~6 lan/giay (ban goc 18 lan/giay)
#define BC_SCRIPT_DEFAULT	"\\script\\event\\lmbiaoche\\npc_lmbiaoche.lua"

extern int	g_nBiaoCheOn;			// cong tat nong (BC_SetEnable)
extern int	g_nBiaoCheGiuKhiThoat;	// 1 = giu xe 5 phut sau khi chu dang xuat (nhu Linux)

int		BC_GetCart(int nPlayerIdx);					// -> npc idx, 0 = khong co
int		BC_GetOwnerIdx(int nNpcIdx);				// -> player idx, 0 = chu khong online
BOOL	BC_Attach(int nPlayerIdx, int nNpcIdx);
void	BC_UnlinkPlayer(int nNpcIdx);				// chi cat the ben KPlayer
void	BC_HardRemove(int nNpcIdx);					// go NPC theo khuon nha (chong DecRef kep)
void	BC_FireCartScript(int nNpcIdx, char* szFun, char* szOwnerName);
void	BC_Breathe();								// CoreServerShell.cpp:1203
void	BC_OnPlayerLogout(int nPlayerIdx);			// KPlayerSet.cpp:357

#endif	// _SERVER
#endif	// _KBIAOCHE_H_
```

### 2.4 `KBiaoChe.cpp` (TEP MOI) — phan cot loi

```cpp
#include "KCore.h"
#include "KBiaoChe.h"

#ifdef _SERVER

extern int GetPlayerIndex(Lua_State* L);		// ScriptFuns.cpp:10135

int g_nBiaoCheOn			= 1;
int g_nBiaoCheGiuKhiThoat	= 1;

//---------------------------------------------------------------------------
// SO DANG KY. CO Y quet theo XE chu khong theo NGUOI CHOI: xe mo coi (chu vua
// dang xuat) khong con nguoi choi nao de quet toi, va o ban do vang thi AI cua
// no cung khong chay -> chi so dang ky nay moi thay duoc no.
//---------------------------------------------------------------------------
struct KBiaoCheSlot
{
	int		nNpcIdx;
	DWORD	dwNpcID;
};
static KBiaoCheSlot	s_Cart[BC_MAX_CART];
static int			s_nCart = 0;

static void BC_FreeSlot(int s)
{
	if (s < 0 || s >= BC_MAX_CART || s_Cart[s].nNpcIdx <= 0)
		return;
	BC_UnlinkPlayer(s_Cart[s].nNpcIdx);
	s_Cart[s].nNpcIdx = 0;
	s_Cart[s].dwNpcID = 0;
	if (s_nCart > 0)
		s_nCart--;
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
			strcmp(pN->Owner, pP->m_PlayerName) == 0)
			return nIdx;
	}
	pP->m_nBiaoCheIdx = 0;
	pP->m_dwBiaoCheID = 0;
	return 0;
}

//---------------------------------------------------------------------------
// Tra chu theo XE. Bo nho dem truoc; truot moi quet mang nguoi choi.
// CO Y khong dung PlayerSet.GetFirstPlayer/GetNextPlayer: do la con tro duyet
// DUNG CHUNG co trang thai (LuaFindPlayer, ScriptFuns.cpp:5985 cung xai).
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
		strcmp(Player[n].m_PlayerName, pN->Owner) == 0)
		return n;

	for (n = 1; n < MAX_PLAYER; n++)
	{
		if (Player[n].m_nIndex <= 0)
			continue;
		if (strcmp(Player[n].m_PlayerName, pN->Owner) == 0)
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
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)	return FALSE;
	if (nNpcIdx    <= 0 || nNpcIdx    >= MAX_NPC)	return FALSE;

	int s = -1;
	for (int i = 0; i < BC_MAX_CART; i++)
		if (s_Cart[i].nNpcIdx <= 0) { s = i; break; }
	if (s < 0)
	{
		AUTOLOG("[LMBC] so dang ky day %d xe - tu choi sinh xe cho %s",
				BC_MAX_CART, Player[nPlayerIdx].m_PlayerName);
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
// GO NPC. Chep DUNG khuon nha o LuaDelNpc (ScriptFuns.cpp:7231-7244):
// xac dang do_revive DA duoc DoRevive tra bo dem o (KNpc.cpp:2530) 18 khung
// truoc, m_RegionIndex con >= 0 chi vi NpcChangeRegion CO Y khong reset. Tru
// lan hai se AN MAT phan dem cua NPC KHAC dung chung o -> quai TANG HINH truoc
// moi phep va cham (loi da phai va ngay 31/08).
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
// KNpc::ExecuteScript2 chi co "dd" nen phai tu goi CallFunction dinh dang "ds"
// (Engine\Src\KLuaScript.cpp:232 'd', :240 's').
// PlayerIndex/PlayerID dat = 0 (ban Linux ghi ro "PlayerIndex VO HIEU"), va
// PHAI dat CA HAI de bien toan cuc cu khong ro ri sang lan goi nay.
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
		dwId = g_FileName2Id((LPSTR)BC_SCRIPT_DEFAULT);
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
		Lua_PushNumber(pS->m_LuaState, (double)pN->m_dwID);
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
// GOI NGUOC tren NGUOI CHOI. Sao khuon KPlayer::ExecuteScript2
// (KPlayer.cpp:7409-7435) nhung CO Y BO dong "Npc[m_nIndex].m_ActionScriptID =
// dwScriptId" - dong do se dam vao hoi thoai NPC dang mo cua nguoi choi.
//---------------------------------------------------------------------------
static void BC_CallOwner(int nPlayerIdx, char* szFun, DWORD dwScriptId)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)	return;
	if (Player[nPlayerIdx].m_nIndex <= 0)			return;
	if (!dwScriptId)								return;

	KLuaScript* pS = (KLuaScript*)g_GetScript(dwScriptId);
	if (!pS || !pS->m_LuaState)
		return;

	int nTop = 0;
	try
	{
		Lua_PushNumber(pS->m_LuaState, Player[nPlayerIdx].m_nPlayerIndex);
		pS->SetGlobalName(SCRIPT_PLAYERINDEX);
		Lua_PushNumber(pS->m_LuaState, (double)Player[nPlayerIdx].m_dwID);
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
	// KHONG go xe o day: ban Linux cho chu 5 phut de vao lai. Neu chu game muon
	// go ngay thi dat g_nBiaoCheGiuKhiThoat = 0.
	BC_UnlinkPlayer(nCart);		// the ben KPlayer phai cat, khe Player se duoc cap lai
}

//===========================================================================
// NHIP TOAN CUC - 2 lan/giay, quet SO DANG KY (khong quet MAX_NPC, khong quet
// MAX_PLAYER). Mac o CoreServerShell.cpp:1203 canh KPartner_Breathe().
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
			s_Cart[s].nNpcIdx = 0; s_Cart[s].dwNpcID = 0;
			if (s_nCart > 0) s_nCart--;
			continue;
		}

		// (B2) XE DA CHET. Xu ly o NHANH CHINH (khong chi o nhanh doi map):
		//      xac o vung ngu KHONG bao gio duoc OnRevive nen phai tu don.
		if (pC->m_Doing == do_death || pC->m_Doing == do_revive)
		{
			if (!(pC->m_btBiaoCheFlag & 0x02))
			{
				pC->m_btBiaoCheFlag |= 0x02;
				pC->m_dwBiaoCheLostTick = dwGT;			// dung lam moc dem xac
				BC_UnlinkPlayer(nCart);					// chu duoc phep tao xe moi NGAY
				BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
			}
			if ((dwGT - pC->m_dwBiaoCheLostTick) > BC_CORPSE_TICK)
			{
				s_Cart[s].nNpcIdx = 0; s_Cart[s].dwNpcID = 0;
				if (s_nCart > 0) s_nCart--;
				BC_HardRemove(nCart);					// KHONG DecRef vi do_revive
			}
			continue;
		}

		// (B3) HET HAN SONG - dat TRUOC moi dieu kien khac.
		//      Quy uoc engine: 0 = mac dinh BC_DEFAULT_LIVE (KHONG phai bat tu).
		BOOL bHetHan = FALSE;
		if (pC->m_uFindPathTime)
		{
			DWORD dwLive = pC->m_uFindPathMaxTime ? pC->m_uFindPathMaxTime
												  : (DWORD)BC_DEFAULT_LIVE;
			if ((dwGT - pC->m_uFindPathTime) > dwLive)
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
				s_Cart[s].nNpcIdx = 0; s_Cart[s].dwNpcID = 0;
				if (s_nCart > 0) s_nCart--;
				BC_UnlinkPlayer(nCart);
				BC_FireCartScript(nCart, (char*)"OnBiaoCheDisapper", pC->Owner);
				if (Npc[nCart].m_dwID == dwID)		// script co the da go xe roi
					BC_HardRemove(nCart);
			}
			continue;
		}

		pC->m_nBiaoCheOwner = nOwner;
		KPlayer* pP = &Player[nOwner];
		KNpc*    pO = &Npc[pP->m_nIndex];

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
			s_Cart[s].nNpcIdx = 0; s_Cart[s].dwNpcID = 0;
			if (s_nCart > 0) s_nCart--;
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
		//      KNpc::ChangeWorld da go chot cam NPC thuong doi ban do
		//      (KNpc.cpp:11091) va da chay san xuat qua KPartnerSys::Breathe.
		if (pO->m_SubWorldIndex < 0)
			continue;								// chu dang chuyen map
		if (pC->m_SubWorldIndex != pO->m_SubWorldIndex)
		{
			// gian cach thu lai 1 giay: tai dung m_uLastFindPathTime (truong nay
			// da thanh vo dung voi xe Long Mon) lam moc, khoi them truong moi.
			if (pC->m_uLastFindPathTime &&
				(dwGT - pC->m_uLastFindPathTime) < BC_RETRY_TICK)
				continue;
			pC->m_uLastFindPathTime = dwGT;

			int nX = 0, nY = 0;
			pO->GetMpsPos(&nX, &nY);
			if (pC->ChangeWorld((DWORD)SubWorld[pO->m_SubWorldIndex].m_SubWorldID,
								nX + 32, nY + 32) > 0)
			{
				// XOA LENH TON DONG: ChangeWorld chi xoa m_Command cho NGUOI CHOI
				// that (S13_IsRealPlayer). Khong xoa thi xe se chay ve toa do cua
				// BAN DO CU ngay sau khi sang map moi (lop loi "truot lui").
				pC->m_Command.CmdKind   = do_none;
				pC->m_dwBiaoCheLostTick = 0;
				pC->m_btBiaoCheFlag    &= ~0x01;
				BC_CallOwner(nOwner, (char*)"OnBiaoCheChangeMapNotice",
							 pC->m_ActionScriptID ? pC->m_ActionScriptID
												  : g_FileName2Id((LPSTR)BC_SCRIPT_DEFAULT));
			}
			else if (pC->m_dwBiaoCheLostTick == 0)
				pC->m_dwBiaoCheLostTick = dwGT;
			continue;
		}

		// (B7) CUNG BAN DO - do binh phuong khoang cach.
		//      GetDistanceSquare tra -1 khi khac SubWorld (KNpcSet.cpp:699) ->
		//      so sanh KHONG DAU cho -1 roi dung nhanh "qua xa", y het lenh ja.
		DWORD dwD2 = (DWORD)NpcSet.GetDistanceSquare(nCart, pP->m_nIndex);
		if (dwD2 > BC_FAR_DIST2)
		{
			if (!(pC->m_btBiaoCheFlag & 0x01))		// BAN MOT LAN, khong 2 lan/giay
			{
				pC->m_btBiaoCheFlag |= 0x01;
				BC_FireCartScript(nCart, (char*)"OnBiaoCheFarAwayPlayerDisapper",
								  pC->Owner);
			}
			if (pC->m_dwBiaoCheLostTick == 0)
				pC->m_dwBiaoCheLostTick = dwGT;
			if ((dwGT - pC->m_dwBiaoCheLostTick) > BC_LOST_TICK)
			{
				s_Cart[s].nNpcIdx = 0; s_Cart[s].dwNpcID = 0;
				if (s_nCart > 0) s_nCart--;
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
#endif	// _SERVER
```

### 2.5 `KBiaoChe.cpp` — 5 ham Lua moi (dat cuoi tep, trong `#ifdef _SERVER`)

```cpp
// CreateBiaoChe(nSeries, nNpcSettingID, nLevel, szName, nSurviveTicks)
//   -> nNpcIndex hoac NIL   (extend.lua:1036 dung truc tiep)
int LuaCreateBiaoChe(Lua_State* L)
{
	if (!g_nBiaoCheOn || Lua_GetTopIndex(L) < 5)		return 0;	// nil
	int nPlayer = GetPlayerIndex(L);
	if (nPlayer <= 0)									return 0;
	if (BC_GetCart(nPlayer) > 0)						return 0;	// mot chu mot xe

	int   nSeries  = (int)Lua_ValueToNumber(L, 1);
	int   nSetting = (int)Lua_ValueToNumber(L, 2);
	int   nLevel   = (int)Lua_ValueToNumber(L, 3);
	char* pszName  = Lua_IsString(L, 4) ? (char*)Lua_ValueToString(L, 4) : (char*)"";
	int   nTicks   = (int)Lua_ValueToNumber(L, 5);

	if (nSetting <= 0)								return 0;
	if (nSeries < 0 || nSeries >= series_num)		nSeries = series_metal;
	if (nLevel  < 1)								nLevel  = 1;
	if (nLevel  > 127)								nLevel  = 127;
	if (nTicks  < 0)								nTicks  = 0;

	KNpc* pO = &Npc[Player[nPlayer].m_nIndex];
	if (pO->m_SubWorldIndex < 0)					return 0;
	int nX = 0, nY = 0;
	pO->GetMpsPos(&nX, &nY);

	// LUU Y THU TU: AddNpcSet2(info, series, subworld, mpsX, mpsY) - DAO so voi
	// AddNpcSet1 cua ban Linux. HIWORD = setting, LOWORD = level (KNpcSet.cpp:483).
	int nIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nSetting), nSeries,
								 pO->m_SubWorldIndex, nX + 32, nY + 32);
	if (nIdx <= 0 || nIdx >= MAX_NPC)				return 0;

	KNpc* pC = &Npc[nIdx];

	// Ten xe bang Linux "Tieu Xa bang cua [<bang>]<nguoi>" CHAC CHAN > 31 byte.
	// Owner[32] nam NGAY SAU Name[32] (KNpc.h:503-504) -> strcpy la tran de.
	if (pszName[0])
		g_StrCpyLen(pC->Name,  pszName, sizeof(pC->Name));
	g_StrCpyLen(pC->Owner, Player[nPlayer].m_PlayerName, sizeof(pC->Owner));

	pC->m_btBiaoChe          = 1;
	pC->m_btBiaoCheFlag      = 0;
	pC->m_bNpcFollowFindPath = TRUE;
	pC->m_uFindPathTime      = (DWORD)g_SubWorldSet.GetGameTime();
	pC->m_uFindPathMaxTime   = (DWORD)nTicks;	// 0 = mac dinh BC_DEFAULT_LIVE
	pC->m_uLastFindPathTime  = 0;
	pC->m_bNoRevive          = 1;				// chet la bien mat (KNpc.cpp:2562)
	pC->m_dwBiaoCheLostTick  = 0;

	// Mau 2233 co Camp = camp_event -> GenOneRelation tra relation_none
	// (KNpcSet.cpp:136) = KHONG AI DANH DUOC. Chi ep phe khi mau la camp_event;
	// cac mau ca nhan (2146/2147/2148 da la camp_animal) giu nguyen bang.
	// Script van co the doi tiep bang SetNpcCurCamp (ScriptFuns.cpp:8363).
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
	if (s >= 0) BC_FreeSlot(s);
	else        BC_UnlinkPlayer(nCart);
	BC_HardRemove(nCart);				// tu chan DecRef kep khi xe dang do_revive
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

// SyncBiaoCheDeathInfoToRelay(nNpcIndex) - ban Linux imul TRAN TRUI, khong kiem bien.
// JX1 la MOT cum (bin\server\maps\WorldSet_GameServer.ini nap 933 map trong mot
// GameServer) nen KHONG co goi relay tuong duong: o dot nay chi cat lien ket +
// ghi nhat ky. Neu chu duyet lam bang xep hang cuop tieu lien may chu thi noi
// vao S3Relay o dot sau.
int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)					{ Lua_PushNumber(L, 0); return 1; }
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)		{ Lua_PushNumber(L, 0); return 1; }
	if (!Npc[nNpcIdx].m_btBiaoChe)				{ Lua_PushNumber(L, 0); return 1; }
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
```

### 2.6 `KNpcAI.cpp` — ban va

**(a) `KNpcAI::Activate`, thay khoi :66-70** (da nam trong `#ifdef _SERVER`):

```cpp
	if (Npc[m_nIndex].Owner[0] && Npc[m_nIndex].m_bNpcFollowFindPath)//add by phong kieu 19/08/2021
	{
		// [LMBC] duong nay nam TRUOC cong m_NextAITime nen chay 18 lan/giay,
		// moi lan FindAroundPlayer quet ten qua 9 vung (KRegion::SearchNpcName).
		// Xe Long Mon di tiet che nhu moi AI khac; XE TIEU CU giu nguyen duong cu
		// de khong doi hanh vi cua tinh nang dang chay (event_vantieu).
		if (Npc[m_nIndex].m_btBiaoChe)
		{
			int nCurTimeB = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime;
			if (Npc[m_nIndex].m_NextAITime > nCurTimeB)
				return;
			Npc[m_nIndex].m_NextAITime = nCurTimeB + BC_AI_TICK;
		}
		ProcessAIFollow();
		return;
	}
```

**(b) `ProcessAIFollow` :139-223** — chi them nhanh `bLM`, phan cu giu nguyen tung dong:

```cpp
void	KNpcAI::ProcessAIFollow()										//add by phong kieu using van tieu
{
	KNpc*	pMe = &Npc[m_nIndex];
	BOOL	bLM = pMe->m_btBiaoChe;			// [LMBC] cong duy nhat

	int nIdx = 0;
	if (bLM)
	{
		// [LMBC] O(1): chu da biet -> khoi di bo danh sach lien ket + strcmp qua
		// MOI NPC cua toi da 9 vung (KRegion::SearchNpcName, KRegion.cpp:1371).
		// Day moi la cho nghen that su, khong phai ban than tan suat.
		int nP = pMe->m_nBiaoCheOwner;
		if (nP > 0 && nP < MAX_PLAYER && Player[nP].m_nIndex > 0 &&
			strcmp(Player[nP].m_PlayerName, pMe->Owner) == 0)
			nIdx = Player[nP].m_nIndex;
	}
	if (nIdx <= 0)
		nIdx = pMe->FindAroundPlayer(pMe->Owner);

	// [LMBC] ba lenh return duoi day von co nghia "BO CUOC VINH VIEN". Voi xe
	// Long Mon chung chi con nghia "nhip nay khong di chuyen": het han, lac chu,
	// keo qua ban do va thu don xac deu do BC_Breathe lo - vi AI KHONG chay khi
	// vung cua xe vang nguoi (KRegion::IsActive, KRegion.h:127).
	if (nIdx <= 0 || Npc[nIdx].m_dwID <= 0)
		return;
	if (pMe->m_SubWorldIndex != Npc[nIdx].m_SubWorldIndex)
		return;
	if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
		return;

	if (bLM)
	{
		// KHONG dong vao phe: da dat MOT LAN luc sinh. Ma cu lat camp_animal <->
		// phe chu MOI KHUNG, moi lan SetCurrentCamp phat s2c_npcchgcurcamp ra 9
		// vung (KNpc.cpp:592-625) => ~324 goi vung/giay/xe bi cat bo.
		// KHONG tu go NPC: vong doi thuoc ve BC_Breathe.
		int nD2 = NpcSet.GetDistanceSquare(m_nIndex, nIdx);

		// GIU TRAN TREN: qua BC_FAR_DIST2 thi NGUNG phat do_walk (xe ket sau
		// tuong / chu dich chuyen trong cung map se khong tim duong xuyen ban do
		// 6 lan/giay suot 30 phut). BC_Breathe dem gio lac chu o do.
		if (nD2 > BC_FOLLOW_DIST2 && (DWORD)nD2 <= BC_FAR_DIST2)
		{
			if (pMe->m_FightMode != Npc[nIdx].m_FightMode)
				pMe->m_FightMode = Npc[nIdx].m_FightMode;

			int nOwnerX, nOwnerY, nMpsX, nMpsY;
			Npc[nIdx].GetMpsPos(&nOwnerX, &nOwnerY);
			pMe->GetMpsPos(&nMpsX, &nMpsY);
			int nXGo = nOwnerX + ((nOwnerX - nMpsX) > 0 ? -1 : 1) * 50;
			int nYGo = nOwnerY + ((nOwnerY - nMpsY) > 0 ? -1 : 1) * 50;
			pMe->SendCommand(do_walk, nXGo, nYGo);
		}
		return;
	}

	... TOAN BO PHAN CON LAI (dong 152-223 hien nay) GIU NGUYEN TUNG DONG ...
}
```

### 2.7 `KNpc.cpp` — VA 2 LOI CO SAN (kieu sua KHAC NHAU cho 2 cho)

**:4054-4064 (CalcDamage) — chi `return FALSE`:**

```cpp
	if(Owner[0]) //add by phong kieu npc van tieu
	{
		// FIX 06/09: doc Player[Npc[nAttacker].m_nPlayerIdx] khong kiem bien.
		// Quai / bay / dan deu co m_nPlayerIdx == 0 -> doc Player[0] la khe CHUA
		// KHOI TAO. Xe tieu cu it ra khoi thanh nen chua lo; xe Long Mon di khap
		// the gioi voi camp_animal thi MOI cu danh cua quai deu di qua day.
		int nAtkP = (nAttacker > 0 && nAttacker < MAX_NPC) ? Npc[nAttacker].m_nPlayerIdx : 0;
		if (nAtkP > 0 && nAtkP < MAX_PLAYER)
		{
			if(strcmp(Player[nAtkP].m_PlayerName, Owner) == 0)
				return FALSE;
			if(Player[nAtkP].m_cPK.GetNormalPKState() == 0)
				return FALSE;
		}
	}
```

**:4769-4788 (duong nhan don) — GIU NGUYEN 13 `ZeroMemory` + `IgnoreState(TRUE)`:**

```cpp
	if (Owner[0]) //add by phong kieu npc van tieu
	{
		int nAtkP = (nLauncher > 0 && nLauncher < MAX_NPC) ? Npc[nLauncher].m_nPlayerIdx : 0;
		if (nAtkP > 0 && nAtkP < MAX_PLAYER)
		{
			if (strcmp(Player[nAtkP].m_PlayerName, Owner) == 0)
				return FALSE;

			if (Player[nAtkP].m_cPK.GetNormalPKState() == 0)
			{
				... GIU NGUYEN 13 dong ZeroMemory + this->IgnoreState(TRUE); ...
				return FALSE;
			}
		}
	}
```

### 2.8 `ScriptFuns.cpp` — kiem bien `m_nNpcParam` (BAT BUOC cung commit voi `MAX_NPCPARAM = 8`)

```cpp
	// LuaSetNpcParam, thay dong 7480:
	if (nParamNum > 2)
	{
		int nP = (int)Lua_ValueToNumber(L, 2);
		if (nP < 0 || nP >= MAX_NPCPARAM)		// FIX 06/09: truoc day KHONG kiem
			return 0;							// = cua ghi de bo nho tuy y qua SetNpcValue
		Npc[nNpcIndex].m_nNpcParam[nP] = (int)Lua_ValueToNumber(L, 3);
	}
	else
		Npc[nNpcIndex].m_nNpcParam[0] = (int)Lua_ValueToNumber(L, 2);

	// LuaGetNpcParam, thay dong 7498:
	if (nParamNum > 1)
	{
		int nP = (int)Lua_ValueToNumber(L, 2);
		if (nP < 0 || nP >= MAX_NPCPARAM)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[nP]);
	}
	else
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[0]);
```

### 2.9 `lib_lmbiaoche.lua` — `WriteYunBiaoLog` (thuan Lua, khong can C++)

```lua
-- GetLocalDate (ScriptFuns.cpp:3464) nhan MOT CHUOI dinh dang strftime va tra ve
-- MOT CHUOI. KHONG duoc dung nhu bang 6 so - loi Lua se giet timer VINH VIEN.
function WriteYunBiaoLog(szMsg)
	local szNgay = GetLocalDate("%Y-%m-%d %H:%M:%S")
	WriteStringToFile("\\Logs\\KSG_YunBiaoLog", "[" .. szNgay .. "]\t" .. szMsg .. "\r\n")
end
```

---

## 3. THU TU THI CONG — 5 BUOC, MOI BUOC LUI DUOC

| Buoc | Noi dung | Nhi phan doi | Cach lui |
|---|---|---|---|
| **B0** — *kiem tra tien de, khong sua ma* | (1) `grep -rn "SetNpcParam\|SetNpcValue\|GetNpcParam\|GetNpcValue" bin\server\script bin\server\scriptjx2` loc doi so thu 2 ≥ 4 → xac nhan lai danh sach 6 tep `tong_disciple` / `tong_springfestival` / `shrewmouse` (ghi & doc **doi xung cung chi so**, ca hai o deu duoc `AddNpcSet1` xoa ve 0 → noi mang khong doi hanh vi). (2) `grep -rl "KNpc\.h" Sources/` xac nhan lai chi Core + `Engine/Src/LuaFuns.cpp` (chet vi `USEOLD`). (3) `grep -rn "SetNpcOwner" bin\server\script` xac nhan chi `event_vantieu\tieudau.lua:169`. (4) Sao luu `.truoc` cho moi tep se dung. | khong | — |
| **B1** — *ba ban va an toan + noi mang, MOT COMMIT* | `GameDataDef.h:437` 4→8, kiem bien `LuaSet/GetNpcParam`, `g_StrCpyLen` o `:6995` va `:7452`, kiem bien `Player[]` o `KNpc.cpp:4054` va `:4769`. **Rebuild TOAN BO project Core (ca Win32 lan x64) — cam build gia tang, cam hai phien build dung chung thu muc obj.** Live thu **khong co mot dong van tieu nao**. | `CoreServer.dll` (+ `CoreClient.dll` neu swap cap) | dat lai `.truoc`, restart |
| **B2** — *them truong, chua co hanh vi* | `KNpc.h` 4 truong, `KPlayer.h` 2 truong, reset o `KNpc::Init` + `AddNpcSet1` + `KPlayer.cpp:327`. Build + chay 1 gio. Khong hanh vi nao doi (0 cho doc cac truong nay). | `CoreServer.dll` | revert 5 tep |
| **B3** — *he van tieu, NGU DONG* | Tao `KBiaoChe.h/.cpp`, them vao `Core.vcxproj`, moc `BC_Breathe()` o `CoreServerShell.cpp:1203`, dang ky 8 ham Lua. **Chua script nao goi `CreateBiaoChe` → `s_nCart == 0` → `BC_Breathe` thoat sau 1 phep so sanh.** | `CoreServer.dll` | `BC_SetEnable(0)` (nong), hoac revert |
| **B4** — *bat AI xe* | Sua `KNpcAI.cpp` (cong `m_NextAITime` + nhanh `bLM`). Thu tren may thu bang mot script toi thieu: `CreateBiaoChe` → chay 3 map → `DeleteBiaoChe`. | `CoreServer.dll` | revert 1 tep (`KNpcAI.cpp`) — he van tieu ca nhan khong bi dung toi o buoc nao |
| **B5** — *du lieu + script* | Ghi de 7 hang `npcs.txt` (2145-2148, 2156-2158) tren **CA server lan client**, them 2 hang `npc_res_kind_file_name.txt` (`enemy243`, `passerby379`), noi `magicscript.txt` tu `ParticularType 4978`, port script Lua 5.4 + `lib_lmbiaoche.lua`. | tep du lieu | dat lai ban `.truoc` cua `npcs.txt` / `magicscript.txt` |

---

## 4. DANG KY HAM LUA VAO `GameScriptFuns[]`

Bang la `TLua_Funcs GameScriptFuns[]` tai **`ScriptFuns.cpp:14896`**; so luong tinh bang `sizeof` tai **:16349**. Cac ham chi ton tai o server **PHAI** boc `#ifdef _SERVER` **ca khai bao lan dong dang ky** — neu khong, build Client bao `C2065` roi keo theo `C2070` o `sizeof(GameScriptFuns)` (bay da tung xay ra, chu thich :14931).

**Khai bao extern** — dat canh khoi extern NPC san co:

```cpp
#ifdef _SERVER
extern int LuaCreateBiaoChe(Lua_State* L);
extern int LuaDeleteBiaoChe(Lua_State* L);
extern int LuaGetBiaoChePos(Lua_State* L);
extern int LuaIsBiaoCheAlive(Lua_State* L);
extern int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L);
extern int LuaBC_SetEnable(Lua_State* L);
#endif
```

**Dong dang ky** — chen ngay sau **`:15299`** (`{"GetNpcValue", LuaGetNpcParam},`):

```cpp
#ifdef _SERVER
	// == [LMBC 06/09] Long Mon Tieu Cuc (xe tieu bang) ==
	{"CreateBiaoChe",				LuaCreateBiaoChe},					// (series, setting, level, ten, ticks) -> npcidx | nil
	{"DeleteBiaoChe",				LuaDeleteBiaoChe},					// () -> 1/0
	{"GetBiaoChePos",				LuaGetBiaoChePos},					// () -> x, y, subworld | -1
	{"IsBiaoCheAlive",				LuaIsBiaoCheAlive},					// () -> 1/0
	{"SyncBiaoCheDeathInfoToRelay",	LuaSyncBiaoCheDeathInfoToRelay},	// (npcidx) -> 1/0
	{"BC_SetEnable",				LuaBC_SetEnable},					// cong tat nong
	// BI DANH: hai ham nay DA VIET + DA KIEM BIEN san (ScriptFuns.cpp:4172/4183,
	// dang ky :16114-16115 duoi ten NPCINFO_*). Chung thao tac RIENG m_CurrentLife -
	// khac SetNpcLife (:7552) von ghi CA m_CurrentLifeMax, va khac GetNpcLife
	// (:8237) von tra MAX. KHONG viet ham moi.
	{"SetNpcCurLife",				LuaNPCINFO_SetNpcCurrentLife},
	{"GetNpcCurLife",				LuaNPCINFO_GetNpcCurrentLife},
#endif
```

`WriteYunBiaoLog` **khong dang ky o day** — viet thuan Lua boc `WriteStringToFile` (muc 2.9).

---

## 5. BA GOI NGUOC ENGINE → LUA

| Ham Lua | Goi tu dau | Script nao | Tham so | `SCRIPT_PLAYERINDEX` / `PLAYERID` |
|---|---|---|---|---|
| `OnBiaoCheDisapper(nNpcIndex, szPlayerName)` | `BC_Breathe` **B2** (xe chet), **B3/B4** (het han), **B4** (lac chu > 5400 tick), **B7** (qua xa > 5400 tick) — qua `BC_FireCartScript` | **ActionScript CUA XE**; khong co thi roi ve `BC_SCRIPT_DEFAULT` | `"ds"` = (chi so NPC, ten chu) | **CA HAI = 0** (ban Linux ghi ro "PlayerIndex VO HIEU"). Dat **ca `PLAYERID`** de bien toan cuc cua lan goi truoc khong ro ri. Kem `SCRIPT_NPCINDEX` / `SCRIPT_NPCID` / `SCRIPT_SUBWORLDINDEX` cua xe. |
| `OnBiaoCheFarAwayPlayerDisapper(nNpcIndex, szPlayerName)` | `BC_Breathe` **B7**, **BAN DUNG MOT LAN** (co `m_btBiaoCheFlag & 0x01`), reset khi xe ve gan hoac doi map thanh cong | nhu tren | nhu tren | nhu tren |
| `OnBiaoCheChangeMapNotice()` | `BC_Breathe` **B6**, chi khi `ChangeWorld > 0` — qua `BC_CallOwner` | ActionScript cua xe (hoac mac dinh) | **khong tham so** | `PLAYERINDEX = Player[i].m_nPlayerIndex` (== `i`, xac nhan `KPlayerSet.cpp:73/217`), `PLAYERID = Player[i].m_dwID`, `SUBWORLDINDEX` cua nguoi choi. **`BC_CallOwner` CO Y khong gan `Npc[].m_ActionScriptID`** (khac `KPlayer::ExecuteScript2`, `KPlayer.cpp:7418`) de khong dam vao hoi thoai NPC dang mo. |

**Thu tu bat buoc khi xe bien mat:** cat lien ket (so dang ky + the KPlayer) → ban script → go NPC. Neu ban script truoc, mot script sinh xe thay the se bi chinh lenh go phia sau xoa mat. Sau khi ban script, kiem lai `Npc[nCart].m_dwID == dwID` truoc khi go (script co the da `DeleteBiaoChe`).

---

## 6. NHUNG CHO **PHAI** BOC `#ifdef _SERVER`

`Core\Src` bien dich vao **CA client lan server**. Bat buoc:

1. **Ca than `KBiaoChe.h`** (sau `#ifndef` guard) va **ca than `KBiaoChe.cpp`**. Ban client se ra don vi bien dich rong.
2. **4 truong moi cua `KNpc.h`** → dat trong khoi `#ifdef _SERVER` san co `:516-524`. Neu de ngoai, `sizeof(KNpc)` ban client cung doi (vo hai nhung vo ich) va lam ban muc dich.
3. **2 truong moi cua `KPlayer.h`** → khoi `#ifdef _SERVER` `:747+`.
4. **Doan reset trong `KNpc::Init()`** (`KNpc.cpp` ~:280) — ham nay bien dich ca hai phia.
5. **Doan reset trong `KNpcSet::AddNpcSet1`** (`:501+`) — dat trong khoi `#ifdef _SERVER` ke duoi.
6. **Loi goi `BC_OnPlayerLogout`** trong `KPlayerSet.cpp:357`.
7. **Toan bo khai bao `extern` + toan bo 8 dong dang ky** trong `ScriptFuns.cpp` — day la cho de sai nhat (tien le `C2065`/`C2070` o :14931).
8. **`#include "KBiaoChe.h"`** trong `ScriptFuns.cpp` va `CoreServerShell.cpp` — header tu boc nen an toan, nhung dat trong `#ifdef _SERVER` cho ro rang.
9. **KHONG can guard moi** cho: `KNpcAI::Activate` khoi :49-130 va `ProcessAIFollow` :133-224 — ca hai **da nam trong `#ifdef _SERVER`**.
10. **KHONG guard** hai cong Owner o `KNpc.cpp:4054` / `:4769` — chung von chay ca hai phia va ban va chi la kiem bien.

---

## 7. DANH SACH KIEM THU RIENG PHAN ENGINE

**Nhom A — hoi quy (chay TRUOC khi bat van tieu, ngay sau B1/B2):**
1. `tong_disciple` va `tong_springfestival`: chay het hai chuoi nhiem vu (4 tep dung `SetNpcParam(idx,4,1)` + `shrewmouse`/`festival_shrewmouse` doc lai chi so 4). Ket qua phai **giong het truoc khi noi mang**.
2. He van tieu **CA NHAN** dang chay (`event_vantieu\tieudau.lua`): sinh xe, di theo, het `MAX_FIND_PATH_NPC_TIME`, tu go — **khong doi mot hanh vi nao** (xe cu khong co `m_btBiaoChe`).
3. Ban dong hanh + pet + SimCity bot: khong doi (dung chung `KNpcAI::Activate` va `m_nNpcParam`).
4. `SetNpcValue(idx, 99, x)` tu script thu → **khong sap, khong ghi gi** (truoc ban va la ghi de bo nho).
5. `SetNpcName(idx, <chuoi 80 ky tu>)` → `Name` bi cat 31 byte, **`Owner` khong bi dam** (doc lai bang `GetNpcOwner`).

**Nhom B — vong doi xe:**

6. Sinh xe → chay 3 ban do lien tiep → xe theo sang du (kiem `GetBiaoChePos` tra dung `SubWorldIndex`).
7. **Xe khong truot lui sau khi doi map**: quan sat 5 giay dau sau `ChangeWorld` — xe khong duoc chay ve toa do cua ban do cu (`m_Command.CmdKind = do_none`).
8. Chu dich chuyen (Phu Ve Thanh) trong **cung** ban do, cach > 512 Mps: xe **ngung** phat `do_walk`, dem gio lac chu; ve gan lai thi tiep tuc di theo va co `0x01` duoc reset.
9. `nSurviveTicks = 0` → xe song **36000 tick** roi bien mat (**khong bat tu**). `nSurviveTicks = 360` → 20 giay.
10. Chu **dang xuat** → xe dung yen, `g_nBiaoCheGiuKhiThoat = 1` thi 5 phut sau tu bien mat kem `OnBiaoCheDisapper`; **dang nhap lai truoc 5 phut** thi xe **tu noi lai** va di theo tiep.
11. Chu **chet** → xe dung cho, **khong** tinh la lac chu; chu hoi sinh xong xe di theo lai.

**Nhom C — cac cho da tung no (bat buoc, moi cai mot lan chay rieng):**

12. **DecRef kep**: giet xe bang quai → sau khi xac bien mat, dung `PB_TrapLog`/quet `KRegion::FindNpc` doi chieu bo dem o cua **cac NPC khac dung chung o**. Lam **3 duong**: (a) script goi `DeleteBiaoChe` trong ham xu ly xe chet, (b) xe chet dung luc het han, (c) chu dang xuat luc xe dang `do_revive`. Bo dem **khong duoc lech**.
13. **Xac o map vang**: giet xe → chu chay sang map khac ngay → sau ~66 giay kiem `Npc[]`: khe phai duoc thu hoi (`m_dwID == 0`), va chu **tao duoc xe moi ngay lap tuc** (khong phai cho).
14. **Tai nhap**: viet `OnBiaoCheDisapper` co goi `DeleteBiaoChe()` **va** `CreateBiaoChe(...)` → chay het han → **khong sap**, xe moi song, xe cu bien mat, `s_nCart` dung.
15. **Bao qua xa ban dung 1 lan**: dem so dong log trong 60 giay xe o xa → **1**, khong phai 120.
16. **`ChangeWorld` that bai** (chu dung o o bi chan / map dich khong nap): so lan `[Map]Change Pos Invalid!` trong `Logs` ≤ **1 lan/giay**, khong phai 5400 lan.
17. **Tran so dang ky**: sinh 257 xe → xe thu 257 tra `nil`, co dong log, khong sap.
18. **Khe NPC bi dung lai**: giet xe, ep sinh nhieu NPC de khe do duoc cap lai, roi goi `GetBiaoChePos()` → phai tra `-1` (the `m_dwID` chan dung).

**Nhom D — hieu nang:**
19. Do `PERF`/`hethong.log` voi **20 xe cung luc tren map dong (Tong Kim, 1000 bot)**: so goi `s2c_npcchgcurcamp`/giay phai **≈ 0** (truoc: ~324/giay/xe), `BC_Breathe` **< 0,3 ms/nhip**.
20. Xac nhan chi phi `BC_Breathe` **khong** roi vao o dem `s_tPB` cua bo do BotPerf (`CoreServerShell.cpp:1191-1210`).

---

## 8. NHUNG GI **CO Y KHONG LAM**

1. **Khong dung bang bam phu tham so NPC.** Ly le "phai swap 4 nhi phan" cua PA "toi thieu rui ro" da bi bac bo bang nguon: tep duy nhat ngoai Core include `KNpc.h` la `Engine/Src/LuaFuns.cpp`, ma **ca tep nam trong `#ifdef USEOLD`** khong bao gio duoc dinh nghia. Noi mang chi ton **16 B × 98 000 ≈ 1,5 MB** va giu **mot duong doc/ghi tham so duy nhat** cho nguoi sau. Doi lai: **bat buoc Rebuild toan bo project Core**, cam build gia tang.
2. **Khong viet `SetNpcCurLife`/`GetNpcCurLife` moi** — `LuaNPCINFO_Set/GetNpcCurrentLife` da ton tai, da kiem bien, da kep `[0, m_CurrentLifeMax]`. Chi them 2 dong bi danh.
3. **Khong viet `WriteYunBiaoLog` bang C++** — `WriteStringToFile` da co, mo `"ab"`, tu tao thu muc. It ma hon, khong them diem nghen I/O trong nhip may chu.
4. **Khong lam nhip dong bo relay 36 tick, khong ghi `SetSaveVal`.** JX1 hien la **mot cum** (933 map trong mot GameServer) nen xe va chu luon cung tien trinh — khong co gi de dong bo. `KPlayerTask::SetSaveVal` con **ban goi ve client moi lan** va ghi vao blob task **co luu DB**: 2 giay/xe = bao goi tin vo ich. `SyncBiaoCheDeathInfoToRelay` **khai bao ro la stub** o dot nay. *Rang buoc phai ghi vao giao kem*: he nay chi dung khi **mot GameServer giu toan bo tuyen ap tieu**; neu chia may chu (`bin\multiserver` co `WorldSet_GameServer1.ini` 61 map / `_2.ini` 108 map) thi phan ban giao relay phai lam lai.
5. **Khong dung dai task 4178-4187 / 3500-3545** cua `variables.lua` Linux — chua doi chieu voi ban do task JX1. Bay `4635-4651` cua Chien Lenh de len Ban Dong Hanh da xay ra mot lan. Trang thai van tieu nam trong `m_nNpcParam[0..7]` (khong qua mang, khong luu DB) va bien nhiem vu do **script** tu chon sau khi quet.
6. **Khong dung mot dong nao vao he van tieu CA NHAN.** `m_btBiaoChe` la cong duy nhat; `event_vantieu\tieudau.lua:169` (noi duy nhat goi `SetNpcOwner` trong ca cay) khong dat co nay nen chay y nguyen duong cu, ke ca hai khoi tu-go va viec lat phe moi khung.
7. **Khong ep phe `camp_animal` vo dieu kien.** Chi ep khi mau NPC la `camp_event` (mau **2233**, `GenOneRelation` tra `relation_none` = khong ai danh duoc). Cac mau ca nhan **2146/2147/2148 da la `camp_animal`** trong `npcs.txt` nen khong dung toi. Script van doi tiep duoc bang `SetNpcCurCamp`.
8. **Khong dung `PlayerSet.GetFirstPlayer/GetNextPlayer`** — do la con tro duyet **dung chung** co trang thai (`LuaFindPlayer` cung xai). `BC_GetOwnerIdx` quet `Player[1..MAX_PLAYER]` cuc bo, va chi khi bo nho dem truot.
9. **Khong dung `KPlayer::ExecuteScript2`** cho `OnBiaoCheChangeMapNotice` — no gan `Npc[m_nIndex].m_ActionScriptID` (`KPlayer.cpp:7418`), se dam vao hoi thoai NPC dang mo. Dung `BC_CallOwner` sao khuon nhung bo dong do.
10. **Khong noi `Name[32]`.** Ten xe bang Linux se bi cat con 31 byte. Noi truong nay la dung vao bo cuc dung chung; giai phap la **doi cach dat ten trong script** (bo tien to "Tieu Xa bang cua ").

---

## 9. BON CAU **PHAI HOI CHU** TRUOC KHI LEN THAT (luat choi, khong phai ky thuat)

1. **Chu dang xuat**: giu xe them 5 phut nhu ban Linux (`g_nBiaoCheGiuKhiThoat = 1`, mac dinh cua dac ta nay) hay go ngay?
2. **PK luyen cong**: cong co san `KNpc.cpp:4054`/`:4769` khien nguoi o trang thai PK luyen cong (`GetNormalPKState() == 0`) danh xe **khong len damage** — tuc **ke cuop bat buoc phai bat co PK**. Giu, hay boc `if (!m_btBiaoChe)`?
3. **Phe xe**: `camp_animal` khien xe **ally voi quai `camp_animal`** va **thu dich voi moi quai khac tren lo trinh** (chung se aggro xe suot duong di). Chap nhan, hay muon mot phe trung lap khac?
4. **Nguong khoang cach**: dac ta giu **dung hang so Linux** (bam theo khi `d2 > 46224` ≈ 215 Mps ≈ 6,7 o; qua xa khi `d2 > 262143` ≈ 512 Mps ≈ 16 o). JX1 hien dung `MAX_FIND_PATH_NPC_DISTANCE = 750` tuyen tinh. **Xe se lu di ro ret** so voi cam giac JX1 hien nay — do thuc te tren may thu roi chot.