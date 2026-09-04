#include "KEngine.h"
#include "KCore.h"

#ifndef _SERVER
// [BIEN 04/09 b] CHEP AN TOAN tu goi mang: cac ham xu ly goi tinh so byte bang 'm_wLength + 1 + sizeof(truong) - sizeof(struct)',
// ma m_wLength lay THANG TU GOI. Goi hong (luong lech) cho ra so khong lo -> memcpy ghi tran mang tren ngan xep -> sap.
// Nhat ky sap 04/09: 00:32:40 chep 20.608 byte vao 256; 00:21:13 chep 56.196 byte vao 32; 00:39:06 chep 45.675 byte vao 64.
static int BIEN_ChepAnToan(void* pDich, int nCoDich, const void* pNguon, int nSoByte, const char* szCho)
{
	if (!pDich || nCoDich <= 0)
		return 0;
	if (!pNguon || nSoByte < 0 || nSoByte > nCoDich - 1)
	{
		AUTOLOG_EVERY(1000, "[BIEN-XAU] %s: goi doi chep %d byte vao cho %d byte - BO GOI", szCho ? szCho : "?", nSoByte, nCoDich);
		((char*)pDich)[0] = 0;
		return 0;
	}
	memcpy(pDich, pNguon, (size_t)nSoByte);
	((char*)pDich)[nSoByte] = 0;
	return nSoByte;
}
#endif
#ifndef _SERVER
#include "../../Headers/IClient.h"
#include "CoreShell.h"
#include "KViewItem.h"
#include "KSellItem.h"
#endif
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KPlayerFaction.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaFuns.h"
#include "KProtocolProcess.h"
#include "KPlayerPartner.h"
#include "KPlayerPet.h"	// [PETSYS]	// [BDH-G4]
#include "KItemDice.h"	// 26/08: he xuc xac chia do (DICEITEM)
#include "KSkills.h"
#include "KThiefSkill.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "KSubWorldSet.h"
#include "KScriptProtocol.h"	// [MAIL 03/09] kenh ScriptProtocol (ObjBuffer)

#ifndef _SERVER
// [S6 25/08] Dem KHE NPC dang dung phia client (MAX_NPC = 256, may chu 98000).
// Chi goi o cac diem TAN SUAT THAP (them/xoa NPC) nen quet 256 o la du re.
// Tra loi dut diem cau hoi "bang NPC client co day khong" - thu ma cac nhan cu
// (deu la AUTOLOG_EVERY, tiet che, khong loc ten) KHONG the tra loi.
// [FIX-3 26/08] Vi tri MAY CHU dang tin cua CHINH NHAN VAT. Client co y KHONG nan toa do
// ban than (KProtocolProcess.cpp :2104 va :2135 deu loai tru chinh minh) nen hai ben lech
// (do that luc bi tu choi don danh: p50 66 mps = 2 o). May danh cua auto lai do bang toa do
// CLIENT roi ban, con may chu phan quyet bang toa do CUA NO => don bi tu choi IM LANG
// (557 lan / 21,9 phut) = 'danh vao khong khi'. Luu lai de may danh do bang CA HAI goc nhin.
int g_nS9SvMeX = 0;
int g_nS9SvMeY = 0;

#ifndef _SERVER
// [S12-THEO 27/08] Cua so 'nghe lenh cho CHINH MINH': binh thuong client vut moi lenh
// run/walk server phat cho ban than (ConformIdx loai self) - dung cho choi tay, nhung
// khi SCRIPT teleport + TU DAT nguoi choi di (Tong Kim: ~1100 mps @400 mps/s) thi client
// mu hoan toan => chuoi [S8-NAN] bung 4 cu lien tiep. Sau moi cu S8-NAN mo cua so 3000ms
// cho phep ap lenh self => doan dat-di hien thi thanh chay muot.
// DIET ECHO (phan bien bat buoc): khi nguoi choi DANG tu di (co duong click A* hoac vua
// gui lenh cua minh trong ~278ms) thi lenh self chinh la echo click cua minh doi ve -
// KHONG ap. Dong ho dung timeGetTime (ms, chiu wrap); CAM dung m_dwCurrentTime (la FRAME
// ~18/s va bi gan lai theo server moi SyncWorld).
DWORD g_uS12CuaSoSelf = 0;
// [S12b 28/08] dich move CHINH MINH vua tu gui (ghi tai SendClientCmdRun/Walk,
// KProtocol.cpp): dung phan biet echo (dich trung) voi lenh DAT-DI cua server (dich la).
int g_nS12TuGuiX = 0, g_nS12TuGuiY = 0;
DWORD g_uS12TuGuiTick = 0;   // [S12c] timeGetTime luc client TU GUI lenh move gan nhat
// [S13 03/09] xem KNpc.cpp: xoa khe lenh dang giu cua chinh minh khi bi dat lai vi tri / doi map.
extern void S13_ClearCmd(int nIdx);
// [S13-KEO] hoa giai MEM vi tri ban than: duoi vung chet KHONG dung (giu du doan), phan doi ra keo
// ve phia may chu 1/S13_CHIA moi goi sync (~18 goi/s => ~90%/giay). Do 03/09: dan truoc lanh p50 ~1 o,
// phan benh dung phang 6,5 o => vung chet 2 o. Nan cung 256 (S8) giu nguyen lam lan an toan.
#define S13_VUNGCHET	64
#define S13_CHIA		8
int g_nS13KeoCount = 0;
static BOOL S12_ChoPhepSelf(int nIdx, int nDichX, int nDichY)
{
	if (nIdx <= 0 || nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		return FALSE;
	if (g_uS12CuaSoSelf == 0 || (DWORD)(timeGetTime() - g_uS12CuaSoSelf) >= 3000)
		return FALSE;
	// [S13-XA 03/09] dich cach minh > 128 o (4096 mps) = lenh TON DONG cua map cu lot qua luc vua ha canh
	// (do that: ha canh Thanh Do ma nhan lenh chay toi toa do sa mac cach 81.192 mps) -> vut. Script
	// dat-di that (Tong Kim ~1100 mps) van qua duoc.
	{
		int nS13MeX = 0, nS13MeY = 0;
		Npc[nIdx].GetMpsPos(&nS13MeX, &nS13MeY);
		int nS13Dx = nDichX - nS13MeX; if (nS13Dx < 0) nS13Dx = -nS13Dx;
		int nS13Dy = nDichY - nS13MeY; if (nS13Dy < 0) nS13Dy = -nS13Dy;
		if (nS13Dx > 4096 || nS13Dy > 4096)
		{
			AUTOLOG("[S13-XA] vut lenh self dich=(%d,%d) me=(%d,%d) t=%u", nDichX, nDichY, nS13MeX, nS13MeY, SubWorld[0].m_dwCurrentTime);
			return FALSE;
		}
	}
	// [S12b 28/08] Dich lech >64 mps (2 o) so dich minh vua TU GUI = chac chan KHONG phai
	// echo cua minh -> cho ap luon, khong xet 2 gac duoi (do that 27/08: auto bat lam
	// HaveTarget/SendMoveFrames chan sach ca lenh dat that; 42 lenh echo that thi van bi
	// chan dung nho nhanh trung-dich roi xuong gac cu).
	if (g_nS12TuGuiX != 0 || g_nS12TuGuiY != 0)
	{
		int nS12Lx = nDichX - g_nS12TuGuiX; if (nS12Lx < 0) nS12Lx = -nS12Lx;
		int nS12Ly = nDichY - g_nS12TuGuiY; if (nS12Ly < 0) nS12Ly = -nS12Ly;
		// [S12c 28/08] chi nuot cu dat BAN GIAO luc ha canh: auto ma DA tu gui lenh
		// KE TU luc mo cua so thi auto THANG - khong ap de nua (do that 20:15-21:02:
		// bypass song suot 3s lam client giang co 2 nguoi lai, lac +-30 o moi 1-2s).
		if ((nS12Lx > 64 || nS12Ly > 64)
		 && (int)(g_uS12CuaSoSelf - g_uS12TuGuiTick) >= 0)
			return TRUE;
	}
	int nS12Tx = 0, nS12Ty = 0;
	if (SubWorld[0].HaveTarget(nS12Tx, nS12Ty))
		return FALSE;	// dang co duong click A* cua nguoi choi - lenh self la echo
	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < defMAX_PLAYER_SEND_MOVE_FRAME)
		return FALSE;	// vua gui lenh di chuyen cua minh ~278ms truoc - echo
	return TRUE;
}
#endif

int S6_UsedSlots()
{
	int n = 0;
	for (int i = 1; i < MAX_NPC; i++)
		if (Npc[i].m_dwID != 0)
			n++;
	return n;
}

// [FIX-D 26/08] NPC cach nguoi choi >= 40 o (dong bo MAX_SYNC_RANGE o KNpc.cpp:695;
// vung nhin that ~31,5 o) thi KHONG BAO GIO duoc ve -> khong nhan vao bang 256 khe.
// Do that tran TK: 220 bot "dai ria 40-48 o" flapping go-gan 0,1-0,4 s chiem 150+
// khe (92% cu VANH) lam dung cham 255 giua tran du da co FIX A/B/C.
int g_nS6BoXa = 0;
BOOL S6_XaQuaTam(int nMpsX, int nMpsY)
{
	int nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nMe <= 0 || Npc[nMe].m_RegionIndex < 0)
		return FALSE;	// chua vao map xong: nhan het de nap binh thuong
	int nMeX = 0, nMeY = 0;
	SubWorld[0].Map2Mps(Npc[nMe].m_RegionIndex, Npc[nMe].m_MapX, Npc[nMe].m_MapY, Npc[nMe].m_OffX, Npc[nMe].m_OffY, &nMeX, &nMeY);
	if (nMpsX - nMeX >= 40*32 || nMeX - nMpsX >= 40*32 || nMpsY - nMeY >= 40*32 || nMeY - nMpsY >= 40*32)
		return TRUE;
	return FALSE;
}
#endif

//#include "MyAssert.h"
#include "Scene/ObstacleDef.h"
#include "KMath.h"
#ifndef _SERVER
#include "Scene\KScenePlaceC.h"
#include <KJXPathFinder.h>

//extern KJXPathFinder g_JXPathFinder;
#endif
#include "KTongProtocol.h"
#include "KLadder.h"
#include "KOption.h"
#include "KDaTauCap.h"
#include <BauCua.h>
#include <iostream>

#ifndef _SERVER
// [REP3 03/09] [Client] NpcTheSame=1 : nguoi choi KHAC mac cung mot bo (theo tuy chon NpcTheSame cua client VLTK 2.0)
//   -> ca dam dong dung chung vai bo sprite/texture, giam nap tu pak va giam cache khi Tong Kim / cong thanh.
//   NpcTheSameArmor (mac dinh 0), NpcTheSameHelm (mac dinh 0), NpcTheSameHorse (-1 = giu ngua rieng), NpcTheSameWeapon (-1 = giu).
static int REP3_ClientIni(const char* szKey, int nDef)
{
	return (int)GetPrivateProfileIntA("Client", szKey, nDef, ".\\config.ini");
}
extern int   g_nWAOptNpcTheSame;	// [REP3 03/09] tu CoreShell.cpp (WAuto tab Co ban)
extern DWORD g_dwWAOptTime;
static void REP3_NpcTheSame(int nIdx)
{
	static int s_nOn = -1, s_nArmor = 0, s_nHelm = 0, s_nHorse = -1, s_nWeapon = -1;
	if (s_nOn < 0)
	{
		s_nOn     = REP3_ClientIni("NpcTheSame", 0);
		s_nArmor  = REP3_ClientIni("NpcTheSameArmor", 0);
		s_nHelm   = REP3_ClientIni("NpcTheSameHelm", 0);
		s_nHorse  = REP3_ClientIni("NpcTheSameHorse", -1);
		s_nWeapon = REP3_ClientIni("NpcTheSameWeapon", -1);
	}
	int nOn = s_nOn;
	if (g_dwWAOptTime && (GetTickCount() - g_dwWAOptTime) < 5000)	// [REP3 03/09] WAuto dang gui tuy chon -> ghi de config.ini
		nOn = g_nWAOptNpcTheSame;
	if (nOn <= 0 || nIdx <= 0 || nIdx >= MAX_NPC)
		return;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)	// khong dung cho chinh minh
		return;
	if (s_nArmor >= 0)  Npc[nIdx].m_ArmorType  = s_nArmor;
	if (s_nHelm >= 0)   Npc[nIdx].m_HelmType   = s_nHelm;
	Npc[nIdx].m_MantleType = 0;
	Npc[nIdx].m_byMantleLevel = 0;
	if (s_nWeapon >= 0) Npc[nIdx].m_WeaponType = s_nWeapon;
	if (s_nHorse >= 0 && Npc[nIdx].m_HorseType >= 0) Npc[nIdx].m_HorseType = (char)s_nHorse;
}
#endif


//#define WAIGUA_ZROC

KProtocolProcess g_ProtocolProcess;
KProtocolProcess::KProtocolProcess()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
#ifndef _SERVER
	ProcessFunc[s2c_login] = NULL;
	ProcessFunc[s2c_logout] = NULL;
	ProcessFunc[s2c_syncend] = &KProtocolProcess::SyncEnd;
	ProcessFunc[s2c_synccurplayer] = &KProtocolProcess::SyncCurPlayer;
	ProcessFunc[s2c_synccurplayerskill] = &KProtocolProcess::s2cSyncAllSkill;
	ProcessFunc[s2c_synccurplayernormal] = &KProtocolProcess::SyncCurNormalData;
	ProcessFunc[s2c_newplayer] = NULL;
	ProcessFunc[s2c_removeplayer] = NULL;
	ProcessFunc[s2c_syncworld] = &KProtocolProcess::SyncWorld;
	ProcessFunc[s2c_syncplayer] = &KProtocolProcess::SyncPlayer;
	ProcessFunc[s2c_syncplayermin] = &KProtocolProcess::SyncPlayerMin;
	ProcessFunc[s2c_syncnpc] = &KProtocolProcess::SyncNpc;
	ProcessFunc[s2c_syncnpcmin] = &KProtocolProcess::SyncNpcMin;
	ProcessFunc[s2c_syncnpcminplayer] = &KProtocolProcess::SyncNpcMinPlayer;
	ProcessFunc[s2c_objadd] = &KProtocolProcess::SyncObjectAdd;
	ProcessFunc[s2c_syncobjstate] = &KProtocolProcess::SyncObjectState;
	ProcessFunc[s2c_syncobjdir] = &KProtocolProcess::SyncObjectDir;
	ProcessFunc[s2c_objremove] = &KProtocolProcess::SyncObjectRemove;
	ProcessFunc[s2c_objTrapAct] = &KProtocolProcess::SyncObjectTrap;
	ProcessFunc[s2c_npcremove] = &KProtocolProcess::NetCommandRemoveNpc;
	ProcessFunc[s2c_npcwalk] = &KProtocolProcess::NetCommandWalk;
	ProcessFunc[s2c_npcrun] = &KProtocolProcess::NetCommandRun;
	ProcessFunc[s2c_npcattack] = NULL;
	ProcessFunc[s2c_npcmagic] = NULL;
	ProcessFunc[s2c_npcjump] = &KProtocolProcess::NetCommandJump;
	ProcessFunc[s2c_npctalk] = NULL;
	ProcessFunc[s2c_npchurt] = &KProtocolProcess::NetCommandHurt;
	ProcessFunc[s2c_npcdeath] = &KProtocolProcess::NetCommandDeath;
	ProcessFunc[s2c_npcchgcurcamp] = &KProtocolProcess::NetCommandChgCurCamp;
	ProcessFunc[s2c_npcchgcamp] = &KProtocolProcess::NetCommandChgCamp;
	ProcessFunc[s2c_skillcast] = &KProtocolProcess::NetCommandSkill;
	ProcessFunc[s2c_playertalk] = NULL;
	ProcessFunc[s2c_playerexp] = &KProtocolProcess::s2cPlayerExp;
	ProcessFunc[s2c_teaminfo] = &KProtocolProcess::s2cShowTeamInfo;
	ProcessFunc[s2c_teamselfinfo] = &KProtocolProcess::s2cUpdataSelfTeamInfo;
	ProcessFunc[s2c_teamapplyinfofalse] = &KProtocolProcess::s2cApplyTeamInfoFalse;
	ProcessFunc[s2c_teamcreatesuccess] = &KProtocolProcess::s2cCreateTeam;
	ProcessFunc[s2c_teamcreatefalse] = &KProtocolProcess::s2cApplyCreateTeamFalse;
	ProcessFunc[s2c_teamopenclose] = &KProtocolProcess::s2cSetTeamState;
	ProcessFunc[s2c_teamgetapply] = &KProtocolProcess::s2cApplyAddTeam;
	ProcessFunc[s2c_teamaddmember] = &KProtocolProcess::s2cTeamAddMember;
	ProcessFunc[s2c_teamleave] = &KProtocolProcess::s2cLeaveTeam;
	ProcessFunc[s2c_teamchangecaptain] = &KProtocolProcess::s2cTeamChangeCaptain;
	ProcessFunc[s2c_playerfactiondata] = &KProtocolProcess::s2cSetFactionData;
	ProcessFunc[s2c_playerleavefaction] = &KProtocolProcess::s2cLeaveFaction;
	ProcessFunc[s2c_playerfactionskilllevel] = &KProtocolProcess::s2cFactionSkillOpen;
	ProcessFunc[s2c_playersendchat] = &KProtocolProcess::s2cGetChat;
	ProcessFunc[s2c_playersyncleadexp] = &KProtocolProcess::s2cGetLeadExp;
	ProcessFunc[s2c_playerlevelup] = &KProtocolProcess::s2cLevelUp;
	ProcessFunc[s2c_teammatelevel] = &KProtocolProcess::s2cGetTeammateLevel;
	ProcessFunc[s2c_playersyncattribute] = &KProtocolProcess::s2cGetCurAttribute;
	ProcessFunc[s2c_playerskilllevel] = &KProtocolProcess::s2cGetSkillLevel;
	ProcessFunc[s2c_syncitem] = &KProtocolProcess::s2cSyncItem;
	ProcessFunc[s2c_removeitem] = &KProtocolProcess::s2cRemoveItem;
	ProcessFunc[s2c_syncmoney] = &KProtocolProcess::s2cSyncMoney;
	ProcessFunc[s2c_playermoveitem] = &KProtocolProcess::s2cMoveItem;
	ProcessFunc[s2c_playerswitchequip] = &KProtocolProcess::s2cSwitchEquip;
	ProcessFunc[s2c_scriptaction] = &KProtocolProcess::SyncScriptAction;
	ProcessFunc[s2c_chatapplyaddfriend] = &KProtocolProcess::s2cChatGetApplyAddFriend;
	ProcessFunc[s2c_chataddfriend] = &KProtocolProcess::s2cChatAddFriend;
	ProcessFunc[s2c_chatrefusefriend] = &KProtocolProcess::s2cChatBeRefusedAddFriend;
	ProcessFunc[s2c_chataddfriendfail] = &KProtocolProcess::s2cChatAddFriendFail;
	ProcessFunc[s2c_chatloginfriendnoname] = &KProtocolProcess::s2cChatLoginFriendNoName;
	ProcessFunc[s2c_chatloginfriendname] = &KProtocolProcess::s2cChatLoginFriendName;
	ProcessFunc[s2c_chatonefrienddata] = &KProtocolProcess::s2cChatAddOneFriend;
	ProcessFunc[s2c_chatfriendonline] = &KProtocolProcess::s2cChatFriendOnline;
	ProcessFunc[s2c_chatdeletefriend] = &KProtocolProcess::s2cChatDeleteFriend;
	ProcessFunc[s2c_chatfriendoffline] = &KProtocolProcess::s2cChatFriendOffLine;
	ProcessFunc[s2c_syncrolelist] = &KProtocolProcess::s2cSyncRoleList;
	ProcessFunc[s2c_tradechangestate] = &KProtocolProcess::s2cTradeChangeState;
	ProcessFunc[s2c_gamblechangestate] = &KProtocolProcess::s2cGambleChangeState;
	ProcessFunc[s2c_npcsetmenustate] = &KProtocolProcess::s2cNpcSetMenuState;
	ProcessFunc[s2c_trademoneysync] = &KProtocolProcess::s2cTradeMoneySync;
	ProcessFunc[s2c_tradedecision] = &KProtocolProcess::s2cTradeDecision;
	ProcessFunc[s2c_gamblemoneysync] = &KProtocolProcess::s2cGambleMoneySync;
	ProcessFunc[s2c_gambledecision] = &KProtocolProcess::s2cGambleDecision;
	ProcessFunc[s2c_chatscreensingleerror] = &KProtocolProcess::s2cChatScreenSingleError;
	ProcessFunc[s2c_teaminviteadd] = &KProtocolProcess::s2cTeamInviteAdd;
	ProcessFunc[s2c_tradepressoksync] = &KProtocolProcess::s2cTradePressOkSync;
	ProcessFunc[s2c_gamblepressoksync] = &KProtocolProcess::s2cGamblePressOkSync;
	ProcessFunc[s2c_ping] = &KProtocolProcess::s2cPing;
	ProcessFunc[s2c_npcsit] = &KProtocolProcess::NetCommandSit;
	ProcessFunc[s2c_opensalebox] = &KProtocolProcess::OpenSaleBox;
	ProcessFunc[s2c_castskilldirectly] = &KProtocolProcess::s2cDirectlyCastSkill;
	ProcessFunc[s2c_msgshow] = &KProtocolProcess::s2cShowMsg;
	ProcessFunc[s2c_syncstateeffect] = &KProtocolProcess::SyncStateEffect;
	ProcessFunc[s2c_openstorebox] = &KProtocolProcess::OpenStoreBox;
	ProcessFunc[s2c_playerrevive] = &KProtocolProcess::PlayerRevive;
	ProcessFunc[s2c_requestnpcfail] = &KProtocolProcess::RequestNpcFail;
	ProcessFunc[s2c_tradeapplystart] = &KProtocolProcess::s2cTradeApplyStart;
	ProcessFunc[s2c_gambleapplystart] = &KProtocolProcess::s2cGambleApplyStart;
	ProcessFunc[s2c_rolenewdelresponse] = NULL;
	ProcessFunc[s2c_ItemAutoMove] = &KProtocolProcess::s2cItemAutoMove;
	ProcessFunc[s2c_itemexchangefinish] = &KProtocolProcess::FinishedItemExchange;
	ProcessFunc[s2c_changeweather] = &KProtocolProcess::s2cChangeWeather;
	ProcessFunc[s2c_pksyncnormalflag] = &KProtocolProcess::s2cPKSyncNormalFlag;
	ProcessFunc[s2c_pksyncenmitystate] = &KProtocolProcess::s2cPKSyncEnmityState;
	ProcessFunc[s2c_pksyncexercisestate] = &KProtocolProcess::s2cPKSyncExerciseState;
	ProcessFunc[s2c_pksyncpkvalue] = &KProtocolProcess::s2cPKValueSync;
	ProcessFunc[s2c_npcsleepmode] = &KProtocolProcess::NpcSleepSync;
	ProcessFunc[s2c_viewequip] = &KProtocolProcess::s2cViewEquip;
	ProcessFunc[s2c_ladderresult] = &KProtocolProcess::LadderResult;
	ProcessFunc[s2c_ladderlist] = &KProtocolProcess::LadderList;
	ProcessFunc[s2c_tongcreate] = &KProtocolProcess::s2cTongCreate;
	ProcessFunc[s2c_replyclientping] = &KProtocolProcess::ServerReplyClientPing;
	ProcessFunc[s2c_npcgoldchange] = &KProtocolProcess::s2cNpcGoldChange;
	ProcessFunc[s2c_itemdurabilitychange] = &KProtocolProcess::ItemChangeDurability;
	ProcessFunc[s2c_openresetpass] = &KProtocolProcess::OpenResetPass;
	ProcessFunc[s2c_reputesyncreputevalue] = &KProtocolProcess::s2cReputeValueSync;
	ProcessFunc[s2c_fuyuansyncfuyuanvalue] = &KProtocolProcess::s2cFuYuanValueSync;
	ProcessFunc[s2c_rebornsyncrebornvalue] = &KProtocolProcess::s2cReBornValueSync;
	ProcessFunc[s2c_viewsellitem] = &KProtocolProcess::s2cViewSellItem;
	ProcessFunc[s2c_viewupdateitem] = &KProtocolProcess::s2cViewUpdateItem;
	ProcessFunc[s2c_playergetcount] = &KProtocolProcess::s2cGetCouunt;
	ProcessFunc[s2c_shopname] = &KProtocolProcess::s2cShopName;
	ProcessFunc[s2c_exitgame] = &KProtocolProcess::s2c_ExitGame;
	ProcessFunc[s2c_playeritemlocksync] = &KProtocolProcess::s2cPlayerItemLockSync;
	ProcessFunc[s2c_opendataubox] = &KProtocolProcess::OpenDaTauBox;
	ProcessFunc[s2c_opendatau1box] = &KProtocolProcess::OpenDaTau1Box;
	ProcessFunc[s2c_battlebox] = &KProtocolProcess::s2cBattleBox;
	ProcessFunc[s2c_syncsupershop] = &KProtocolProcess::s2cSyncSuperShop;
	ProcessFunc[s2c_extpointsync] = &KProtocolProcess::s2cExtPointValueSync; // protocol xu
	//ProcessFunc[s2c_autoplay] = &KProtocolProcess::s2cAutoPlaySync; // protocol aut
	//ProcessFunc[s2c_player_stop] = &KProtocolProcess::s2cPlayerStop;
	//ProcessFunc[s2c_pos_edition] = &KProtocolProcess::s2cPosEdition;
	//ProcessFunc[s2c_findpathsync] = &KProtocolProcess::s2cFindPathSync;
	ProcessFunc[s2c_ridesync] = &KProtocolProcess::s2cRideSync; // protocol len xuong ngua
	ProcessFunc[s2c_ignorestate] = &KProtocolProcess::IgnoreState;
	ProcessFunc[s2c_openaffairbox] = &KProtocolProcess::OpenAffairBox;
	ProcessFunc[s2c_playersync_magic_attr] = &KProtocolProcess::s2cPlayerSync_MA;
	ProcessFunc[s2c_playeritemtimesync] = &KProtocolProcess::s2cPlayerItemTimeSync;
	ProcessFunc[s2c_timebox] = &KProtocolProcess::s2cTimeBox;
	ProcessFunc[s2c_talkex] = &KProtocolProcess::s2cTalkEx;
	ProcessFunc[s2c_syncmasklock] = &KProtocolProcess::SyncMaskLock;
	ProcessFunc[s2c_taskvalue] = &KProtocolProcess::s2cTaskValueSync; // protocol task
	ProcessFunc[s2c_playermissiondata] = &KProtocolProcess::s2cSetMissionData;
	ProcessFunc[s2c_inputbox] = &KProtocolProcess::s2cInPutBox;
	ProcessFunc[s2c_setobstacle] = &KProtocolProcess::s2cSetObstacle; //#Set vÀt c∂n
	ProcessFunc[s2c_returncityowntong] = &KProtocolProcess::s2cReturnCityOwnTong;
	ProcessFunc[s2c_playerloginreplay] = &KProtocolProcess::s2cPlayerLoginReplay; //fix by phong ki“u chuy”n gs bﬁ m t skill
	ProcessFunc[s2c_openquestfinishdlg] = &KProtocolProcess::s2cOpenQuestFinishDlg;// protocol quest finish
	ProcessFunc[s2c_imagenpc] = &KProtocolProcess::s2cImageNpcSync;
	ProcessFunc[s2c_opentrembleitem] = &KProtocolProcess::s2cOpenTrembleItem;
	ProcessFunc[s2c_opencompounditem] = &KProtocolProcess::s2cOpenCompoundItem;
	ProcessFunc[s2c_syncrankdata] = &KProtocolProcess::s2cSyncRankData;
	ProcessFunc[s2c_syncrankdata2] = &KProtocolProcess::s2cSyncRankData2;
	ProcessFunc[s2c_show_damage] = &KProtocolProcess::s2cShowDamage;
	ProcessFunc[s2c_syncmagic] = &KProtocolProcess::s2cSyncMagic;
	ProcessFunc[s2c_syncmeridian] = &KProtocolProcess::s2cSyncMeridian;
	ProcessFunc[s2c_syncbaucuaresult] = &KProtocolProcess::s2cSyncBauCuaResult;
	ProcessFunc[s2c_syncbaucuainfo] = &KProtocolProcess::s2cSyncBauCuaResult;
	ProcessFunc[s2c_playersync] = &KProtocolProcess::s2cPlayerSync;
	ProcessFunc[s2c_removeallitem] = &KProtocolProcess::s2cRemoveAllItem;
	ProcessFunc[s2c_diceitem] = &KProtocolProcess::s2cDiceItem;
	ProcessFunc[s2c_syncpfpack] = &KProtocolProcess::s2cSyncItemPfPack;	// [PFSYNC 31/08]
	ProcessFunc[s2c_syncfusion] = &KProtocolProcess::s2cSyncItemFusion;	// [DUNGLUYEN 01/09]
	ProcessFunc[s2c_reduceskillcd] = &KProtocolProcess::s2cReduceSkillCD;	// [HOASON 01/09b]
	ProcessFunc[s2c_syncvhtd] = &KProtocolProcess::s2cSyncVhtd;	// [VHTD 02/09g]
	ProcessFunc[s2c_detonate] = &KProtocolProcess::s2cDetonate;	// [VHTD 02/09w]
	ProcessFunc[s2c_scriptdata] = &KProtocolProcess::s2cScriptData;	// [MAIL 03/09]
	//ProcessFunc[s2c_dynamic_structure] = &KProtocolProcess::s2cDynamicStruct;
	

	ProcessFunc[s2c_extend] = &KProtocolProcess::s2cExtend;
	ProcessFunc[s2c_extendchat] = &KProtocolProcess::s2cExtendChat;
	ProcessFunc[s2c_extendfriend] = &KProtocolProcess::s2cExtendFriend;
	ProcessFunc[s2c_extendtong] = &KProtocolProcess::s2cExtendTong;

#else

	ProcessFunc[c2s_login] = NULL;
	ProcessFunc[c2s_logiclogin] = NULL;
	ProcessFunc[c2s_syncend] = NULL;
	ProcessFunc[c2s_loadplayer] = NULL;
	ProcessFunc[c2s_newplayer] = NULL;
	ProcessFunc[c2s_removeplayer] = &KProtocolProcess::RemoveRole;
	ProcessFunc[c2s_requestworld] = NULL;
	ProcessFunc[c2s_requestplayer] = NULL;
	ProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;
	ProcessFunc[c2s_requestobj] = &KProtocolProcess::ObjRequestCommand;
	ProcessFunc[c2s_npcwalk] = &KProtocolProcess::NpcWalkCommand;
	ProcessFunc[c2s_npcrun] = &KProtocolProcess::NpcRunCommand;
	ProcessFunc[c2s_npcskill] = &KProtocolProcess::NpcSkillCommand;
	ProcessFunc[c2s_npcjump] = &KProtocolProcess::NpcJumpCommand;
	ProcessFunc[c2s_npctalk] = &KProtocolProcess::NpcTalkCommand;
	ProcessFunc[c2s_dynamic_structure] = &KProtocolProcess::c2sDynamicStruct;
	ProcessFunc[c2s_npcdeath] = NULL;
	ProcessFunc[c2s_playertalk] = &KProtocolProcess::PlayerTalkCommand;
	ProcessFunc[c2s_teamapplyinfo] = &KProtocolProcess::PlayerApplyTeamInfo;
	ProcessFunc[c2s_teamapplycreate] = &KProtocolProcess::PlayerApplyCreateTeam;
	ProcessFunc[c2s_teamapplyopenclose] = &KProtocolProcess::PlayerApplyTeamOpenClose;
	ProcessFunc[c2s_teamapplyadd] = &KProtocolProcess::PlayerApplyAddTeam;
	ProcessFunc[c2s_teamacceptmember] = &KProtocolProcess::PlayerAcceptTeamMember;
	ProcessFunc[c2s_teamapplyleave] = &KProtocolProcess::PlayerApplyLeaveTeam;
	ProcessFunc[c2s_teamapplykickmember] = &KProtocolProcess::PlayerApplyTeamKickMember;
	ProcessFunc[c2s_teamapplychangecaptain] = &KProtocolProcess::PlayerApplyTeamChangeCaptain;
	ProcessFunc[c2s_teamapplydismiss] = &KProtocolProcess::PlayerApplyTeamDismiss;
	ProcessFunc[c2s_playerapplysetpk] = &KProtocolProcess::PlayerApplySetPK;
	ProcessFunc[c2s_playerapplyfactiondata] = &KProtocolProcess::PlayerApplyFactionData;
	ProcessFunc[c2s_playersendchat] = &KProtocolProcess::PlayerSendChat;
	ProcessFunc[c2s_playeraddbaseattribute] = &KProtocolProcess::PlayerAddBaseAttribute;
	ProcessFunc[c2s_playerapplyaddskillpoint] = &KProtocolProcess::PlayerApplyAddSkillPoint;
	ProcessFunc[c2s_playereatitem] = &KProtocolProcess::PlayerEatItem;
	ProcessFunc[c2s_playerpickupitem] = &KProtocolProcess::PlayerPickUpItem;
	ProcessFunc[c2s_playermoveitem] = &KProtocolProcess::PlayerMoveItem;
	ProcessFunc[c2s_playersellitem] = &KProtocolProcess::PlayerSellItem;
	ProcessFunc[c2s_playerbuyitem] = &KProtocolProcess::PlayerBuyItem;
	ProcessFunc[c2s_playerthrowawayitem] = &KProtocolProcess::PlayerDropItem;
	ProcessFunc[c2s_playerselui] = &KProtocolProcess::PlayerSelUI;
	ProcessFunc[c2s_chatsetchannel] = &KProtocolProcess::ChatSetChannel;
	ProcessFunc[c2s_chatapplyaddfriend] = &KProtocolProcess::ChatApplyAddFriend;
	ProcessFunc[c2s_chataddfriend] = &KProtocolProcess::ChatAddFriend;
	ProcessFunc[c2s_chatrefusefriend] = &KProtocolProcess::ChatRefuseFriend;
	ProcessFunc[c2s_dbplayerselect] = NULL;
	ProcessFunc[c2s_chatapplyresendallfriendname] = &KProtocolProcess::ChatApplyReSendAllFriendName;
	ProcessFunc[c2s_chatapplysendonefriendname] = &KProtocolProcess::ChatApplySendOneFriendName;
	ProcessFunc[c2s_chatdeletefriend] = &KProtocolProcess::ChatDeleteFriend;
	ProcessFunc[c2s_chatredeletefriend] = &KProtocolProcess::ChatReDeleteFriend;
	ProcessFunc[c2s_tradeapplystateopen] = &KProtocolProcess::TradeApplyOpen;
	ProcessFunc[c2s_tradeapplystateclose] = &KProtocolProcess::TradeApplyClose;
	ProcessFunc[c2s_tradeapplystart] = &KProtocolProcess::TradeApplyStart;
	ProcessFunc[c2s_trademovemoney] = &KProtocolProcess::TradeMoveMoney;
	ProcessFunc[c2s_tradedecision] = &KProtocolProcess::TradeDecision;
	ProcessFunc[c2s_gambleapplystateopen] = &KProtocolProcess::GambleApplyOpen;
	ProcessFunc[c2s_gambleapplystateclose] = &KProtocolProcess::GambleApplyClose;
	ProcessFunc[c2s_gambleapplystart] = &KProtocolProcess::GambleApplyStart;
	ProcessFunc[c2s_gamblemovemoney] = &KProtocolProcess::GambleMoveMoney;
	ProcessFunc[c2s_gambledecision] = &KProtocolProcess::GambleDecision;
	ProcessFunc[c2s_dialognpc] = &KProtocolProcess::DialogNpc;
	ProcessFunc[c2s_teaminviteadd] = &KProtocolProcess::TeamInviteAdd;
	ProcessFunc[c2s_changeauraskill] = &KProtocolProcess::ChangeAuraSkill;
	ProcessFunc[c2s_teamreplyinvite] = &KProtocolProcess::TeamReplyInvite;
	ProcessFunc[c2s_ping] = NULL;//ReplyPing;
	ProcessFunc[c2s_npcsit] = &KProtocolProcess::NpcSitCommand;
	ProcessFunc[c2s_objmouseclick] = &KProtocolProcess::ObjMouseClick;
	ProcessFunc[c2s_storemoney] = &KProtocolProcess::StoreMoneyCommand;
	ProcessFunc[c2s_playerrevive] = &KProtocolProcess::NpcReviveCommand;
	ProcessFunc[c2s_tradereplystart] = &KProtocolProcess::c2sTradeReplyStart;
	ProcessFunc[c2s_gamblereplystart] = &KProtocolProcess::c2sGambleReplyStart;
	ProcessFunc[c2s_pkapplychangenormalflag] = &KProtocolProcess::c2sPKApplyChangeNormalFlag;
	ProcessFunc[c2s_pkapplyenmity] = &KProtocolProcess::c2sPKApplyEnmity;
	ProcessFunc[c2s_viewequip] = &KProtocolProcess::c2sViewEquip;
	ProcessFunc[c2s_ladderquery] = &KProtocolProcess::LadderQuery;
	ProcessFunc[c2s_repairitem] = &KProtocolProcess::ItemRepair;
	ProcessFunc[c2s_npcride] = &KProtocolProcess::NpcRideCommand;//len xuong ngua
	ProcessFunc[c2s_cpunlock] = &KProtocolProcess::NpcCPUnlockCommand;
	ProcessFunc[c2s_cplock] = &KProtocolProcess::NpcCPLockCommand;
	ProcessFunc[c2s_cpswitchequipset] = &KProtocolProcess::NpcSwitchEquipSet;
	ProcessFunc[c2s_cpchange] = &KProtocolProcess::NpcCPChangeCommand;
	ProcessFunc[c2s_cpreset] = &KProtocolProcess::NpcCPResetCommand;
	ProcessFunc[c2s_playersetprice] = &KProtocolProcess::SetPrice;
	ProcessFunc[c2s_playerstarttrade] = &KProtocolProcess::StartTrade;
	ProcessFunc[c2s_viewitem] = &KProtocolProcess::c2sViewItem;
	ProcessFunc[c2s_playertradebuyitem] = &KProtocolProcess::c2sTradeBuy;
	ProcessFunc[c2s_updateviewitem] = &KProtocolProcess::c2supdateitem;
	ProcessFunc[c2s_playerneedcount] = &KProtocolProcess::c2sNeedCount;
	ProcessFunc[c2s_playershopname] = &KProtocolProcess::c2sNeedShopName;
	ProcessFunc[c2s_playersysshop] = &KProtocolProcess::c2sSysShop;
	ProcessFunc[c2s_playerlixian] = &KProtocolProcess::c2sLiXian;
	ProcessFunc[c2s_pkvalue] = &KProtocolProcess::NpcPKValueCommand;
	ProcessFunc[c2s_plockitem] = &KProtocolProcess::NpcLockPlayerItemCommand;
	ProcessFunc[c2s_itemyearexp] = &KProtocolProcess::RemoveItemYearExpCommand;
	ProcessFunc[c2s_breakitem] = &KProtocolProcess::ItemBreak;
	//ProcessFunc[c2s_datau]=&KProtocolProcess::NpcDaTauCommand;
	//ProcessFunc[c2s_datau1]=&KProtocolProcess::NpcDaTau1Command;
	ProcessFunc[c2s_playercommand] = &KProtocolProcess::PlayerCommand;
	//ProcessFunc[c2s_autoplay] = &KProtocolProcess::c2sSetAutoSync;
	ProcessFunc[c2s_playeractionchat] = &KProtocolProcess::C2SPlayerActionChatCmd;
	ProcessFunc[c2s_openshop] = &KProtocolProcess::C2SClientOpenShopCommand;
	ProcessFunc[c2s_cpsetimage] = &KProtocolProcess::c2sSetImage;
	ProcessFunc[c2s_autoplayersellitem] = &KProtocolProcess::AutoPlayerSellItem;
	//ProcessFunc[c2s_player_pos_sync] = &KProtocolProcess::c2sPosSync;
	//ProcessFunc[c2s_playerstop] = &KProtocolProcess::c2sPlayerStopNotify;
	ProcessFunc[c2s_uicmdscript] = &KProtocolProcess::UiCommandScript;
	ProcessFunc[c2s_recoverybox] = &KProtocolProcess::RecoveryBoxCmd;
	ProcessFunc[c2s_inputinfo] = &KProtocolProcess::c2sInputCommand;
	ProcessFunc[c2s_getcityowntong] = &KProtocolProcess::c2sGetCityOwnTong;
	ProcessFunc[c2s_recoveritem] = &KProtocolProcess::RecoverItemCommand;
	ProcessFunc[c2s_playerthrowallitem] = &KProtocolProcess::c2sPlayerThrowAllItem;
	ProcessFunc[c2s_aibacktotown] = &KProtocolProcess::c2sBackToTown;//Auto by quay lπi;
	ProcessFunc[c2s_setmeridian] = &KProtocolProcess::c2sSetMeridian;
	ProcessFunc[c2s_baucua] = &KProtocolProcess::c2sBauCua;
	ProcessFunc[c2s_diceitem] = &KProtocolProcess::c2sDiceItem;
	ProcessFunc[c2s_partnerop] = &KProtocolProcess::c2sPartnerOp;	// [BDH-G4]
	ProcessFunc[c2s_scriptdata] = &KProtocolProcess::c2sScriptData;	// [MAIL 03/09]


#endif
}

KProtocolProcess::~KProtocolProcess()
{
}

#ifndef _SERVER

extern IClientCallback* l_pDataChangedNotifyFunc;
void KProtocolProcess::ProcessNetMsg(BYTE* pMsg)
{
	if (!pMsg || pMsg[0] <= s2c_clientbegin || pMsg[0] >= s2c_end || ProcessFunc[pMsg[0]] == NULL)
	{
		g_DebugLog("[error]Net Msg Error");
		return;
	}
	//g_DebugLog("[net]Msg:%c", pMsg[0]);
	if (ProcessFunc[pMsg[0]])
		(this->*ProcessFunc[pMsg[0]])(pMsg);
}
#else
void KProtocolProcess::ProcessNetMsg(int nIndex, BYTE* pMsg)
{
	_ASSERT(pMsg && pMsg[0] > c2s_gameserverbegin && pMsg[0] < c2s_end);

	BYTE	byProtocol = pMsg[0];
	_ASSERT(nIndex > 0 && nIndex < MAX_PLAYER);
	if (ProcessFunc[byProtocol])
	{
		(this->*ProcessFunc[byProtocol])(nIndex, pMsg);
		Player[nIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::NpcSleepSync(BYTE* pMsg)
{
	NPC_SLEEP_SYNC*	pSync = (NPC_SLEEP_SYNC *)pMsg;
	DWORD	dwNpcId = pSync->NpcID;

	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].SetSleepMode((BOOL)pSync->bSleep);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void	KProtocolProcess::s2cViewSellItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg);
}

void KProtocolProcess::s2cBattleBox(BYTE* pMsg)
{
	S2C_BATTLE_BOX	*Cmd = (S2C_BATTLE_BOX *)pMsg;
	CoreDataChanged(GDCNI_UPDATE_BATTLE_BOX, (unsigned int)Cmd->szBattleDesc, Cmd->nType);
}

void    KProtocolProcess::s2cPlayerStop(BYTE * pMsg)
{ 

}

void    KProtocolProcess::s2cPosEdition(BYTE * pMsg)
{

}

void KProtocolProcess::s2cFindPathSync(BYTE* pMsg)
{

}

void 	KProtocolProcess::s2cSyncSuperShop(BYTE* pMsg)
{
	S2C_SUPERSHOP* pInfo = (S2C_SUPERSHOP*)pMsg;
	BuySell.OpenSale(pInfo->m_nSaleType, (BuySellInfo*)&pInfo->m_BuySellInfo);
}

void	KProtocolProcess::IgnoreState(BYTE* pMsg) //B· qua trπng th∏i
{
	IGNORE_STATE_SYNC	*pSync = (IGNORE_STATE_SYNC*)pMsg;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IgnoreState((BOOL)pSync->bNegative);
}

void	KProtocolProcess::s2cRideSync(BYTE* pMsg)
{
	NPC_RIDE_SYNC	*pValue = (NPC_RIDE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].CheckRideHorse(pValue->m_btRideFlag);
}

void	KProtocolProcess::s2cAutoPlaySync(BYTE* pMsg)
{
	/*PLAYER_REQUEST_AUTO	*pInfo = (PLAYER_REQUEST_AUTO*)pMsg;
	BYTE btAuto = pInfo->m_bAuto;
	BOOL bActive = pInfo->m_bActive;
	if (btAuto == 1)
	{
		if (bActive)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "ß∑ bÀt auto!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "Tæt auto!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);		
		}
		Player[CLIENT_PLAYER_INDEX].SetAutoFlag((BOOL)bActive);
	}*/
}

void	KProtocolProcess::s2cExtPointValueSync(BYTE* pMsg)
{
	EXTPOINT_VALUE_SYNC	*pValue = (EXTPOINT_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].SetExtPoint(pValue->m_nExtPointValue);
	if (pValue->m_nChangeExtPoint)
	{
		APPLY_GET_EXTPOINT_COMMAND	nExt;
		nExt.ProtocolType	= c2s_extendtong;
		nExt.m_wLength		= sizeof(nExt) - 1;
		nExt.m_btMsgId		= enumCOMMAND_UPDATE_EXTPOINT;
		nExt.m_nExtPoint 	= pValue->m_nExtPointValue;
		if (g_pClient)
			g_pClient->SendPackToServer(&nExt, nExt.m_wLength + 1);
	}
}

void	KProtocolProcess::s2cViewUpdateItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg,1);
}

void	KProtocolProcess::s2cGetCouunt(BYTE* pMsg)
{
	PLAYER_GET_COUNT *pGetCount = (PLAYER_GET_COUNT *)pMsg;
	// (r5c - auto Da Tau) ghi tra loi tham do sap: CO goi ve = sap that
	g_sDTCap.dwCntId = pGetCount->dwId;
	g_sDTCap.nCnt = pGetCount->nCount;
	++g_sDTCap.uCntSeq;
	g_cSellItem.Check(pGetCount->nCount,pGetCount->dwId);
}

void	KProtocolProcess::s2cShopName(BYTE* pMsg)
{
	PLAYER_PLAYER_SHOPNAME *pShopname = (PLAYER_PLAYER_SHOPNAME *)pMsg;
	int nIndex = NpcSet.SearchID(pShopname->m_dwNpcID);
	strcpy(Npc[nIndex].ShopName,pShopname->m_Name);
}

void KProtocolProcess::s2cPing(BYTE* pMsg)
{
//	DWORD	dwTimer = GetTickCount();
//	PING_COMMAND* PingCmd = (PING_COMMAND *)pMsg;
//
//	dwTimer -= PingCmd->m_dwTime;
//	dwTimer >>= 1;
//	g_SubWorldSet.SetPing(dwTimer);
//	g_bPingReply = TRUE;

	DWORD	dwTimer = GetTickCount();
	PING_COMMAND*	PingCmd = (PING_COMMAND *)pMsg;
	PING_CLIENTREPLY_COMMAND	pcc;
	pcc.ProtocolType = c2s_ping;
	pcc.m_dwReplyServerTime = PingCmd->m_dwTime;
	pcc.m_dwClientTime = dwTimer;
	g_pClient->SendPackToServer(&pcc, sizeof(PING_CLIENTREPLY_COMMAND));
	//g_SubWorldSet.SetPing(PingCmd->m_dwTime);
}

void KProtocolProcess::ServerReplyClientPing(BYTE* pMsg)
{
	DWORD	dwTimer = GetTickCount();
	PING_COMMAND* pPc = (PING_COMMAND *)pMsg;

	dwTimer -= pPc->m_dwTime;
	dwTimer >>= 1;
	g_SubWorldSet.SetPing(dwTimer);
}

void KProtocolProcess::s2cPlayerExp(BYTE* pMsg)
{
	PLAYER_EXP_SYNC	*pExp = (PLAYER_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].s2cSetExp(pExp->m_nExp);
}

void	KProtocolProcess::s2cChatScreenSingleError(BYTE* pMsg)
{
	CHAT_SCREENSINGLE_ERROR_SYNC	*pError = (CHAT_SCREENSINGLE_ERROR_SYNC*)pMsg;
	
	// 
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	BIEN_ChepAnToan(szName, (int)sizeof(szName), pError->m_szName, (int)(pError->m_wLength + 1 + sizeof(pError->m_szName) - sizeof(CHAT_SCREENSINGLE_ERROR_SYNC)), "szName");

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_CHAT_TAR_REFUSE_SINGLE_TALK, szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

void KProtocolProcess::NetCommandChgCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_Camp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandChgCurCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_CurrentCamp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandDeath(BYTE* pMsg)
{
	DWORD	dwNpcID;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (nIdx > 0)
	{
#ifndef _SERVER
		// [S7 26/08] luong chet cua CHINH MINH: moc client BIET minh chet.
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			AUTOLOG("[S7-CHET-CLI] id=%u doing=%d cdoing=%d frame=%d/%d reg=%d cell=(%d,%d) t=%u", dwNpcID, (int)Npc[nIdx].m_Doing, (int)Npc[nIdx].m_ClientDoing, Npc[nIdx].m_Frames.nCurrentFrame, Npc[nIdx].m_Frames.nTotalFrame, Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, SubWorld[0].m_dwCurrentTime);
#endif
		//Npc[nIdx].SendCommand(do_death);
		Npc[nIdx].ProcNetCommand(do_death);
		AUTOLOG_EVERY(1000, "NET-DEATH npc=%u idx=%d kind=%u cell=(%d,%d) off=(%d,%d) reg=%d lifecu=%d t=%u", dwNpcID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, Npc[nIdx].m_CurrentLife, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].m_CurrentLife = 0;
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		//g_DebugLog("[Death]Net command comes");
	}
}

void KProtocolProcess::NetCommandJump(BYTE* pMsg)
{
	NPC_JUMP_SYNC* pNetCommandJump = (NPC_JUMP_SYNC *)pMsg;
	AUTOLOG_EVERY(1000, "NET-JUMP-SKIP npc=%u idx=%d mps=(%d,%d) t=%u", pNetCommandJump->ID, NpcSet.SearchID(pNetCommandJump->ID), pNetCommandJump->nMpsX, pNetCommandJump->nMpsY, SubWorld[0].m_dwCurrentTime);
	DWORD dwNpcId = pNetCommandJump->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);
	
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		AUTOLOG_EVERY(1000, "NET-JUMP npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d t=%u", dwNpcId, nIdx, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].SendCommand(do_jump, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandHurt(BYTE* pMsg)
{
	NPC_HURT_SYNC*	pSync = (NPC_HURT_SYNC *)pMsg;
	AUTOLOG_EVERY(1000, "NET-HURT-NOIDX npc=%u idx=%d frames=%d pos=(%d,%d) t=%u", pSync->ID, NpcSet.SearchID(pSync->ID), pSync->nFrames, pSync->nX, pSync->nY, SubWorld[0].m_dwCurrentTime);
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		AUTOLOG_EVERY(500, "NET-HURT npc=%u idx=%d kind=%u frames=%d pos=(%d,%d) cell=(%d,%d) life=%d/%d t=%u", pSync->ID, nIdx, Npc[nIdx].m_Kind, pSync->nFrames, pSync->nX, pSync->nY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_CurrentLife, Npc[nIdx].m_CurrentLifeMax, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].ProcNetCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandRemoveNpc(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			// zroc change
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
//			SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
		}
		AUTOLOG_EVERY(1000, "NET-RMNPC npc=%u idx=%d kind=%u cell=(%d,%d) reg=%d life=%d t=%u", dwNpcID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_RegionIndex, Npc[nIdx].m_CurrentLife, SubWorld[0].m_dwCurrentTime);
#ifndef _SERVER
		AUTOLOG("[S6-DEL] npc=%u idx=%d kind=%u cell=(%d,%d) off=(%d,%d) reg=%d dung=%d/%d t=%u", dwNpcID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, S6_UsedSlots(), (int)MAX_NPC, SubWorld[0].m_dwCurrentTime);
#endif
		NpcSet.Remove(nIdx);
	}
}

void KProtocolProcess::NetCommandRun(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
#ifndef _SERVER
	// [S6] Ghi TRUOC cua chan: ap=0 nghia la client CHUA CO npc nay nen lenh di chuyen
	// BI VUT. Do chinh la luc NPC moi vao tam nhin va chi con nhay theo goi dong bo.
	AUTOLOG("[S6-CMD] lenh=run npc=%u idx=%d ap=%d dich=(%d,%d) t=%u", dwNpcID, nIdx, (int)(Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) ? 1 : 0), (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
#endif
	BOOL bS12Self = FALSE;
#ifndef _SERVER
	bS12Self = S12_ChoPhepSelf(nIdx, (int)MapX, (int)MapY);
#endif
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)
	{
		AUTOLOG_EVERY(300, "NET-RUN npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].SendCommand(do_run, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
#ifndef _SERVER
		if (bS12Self)
		{
			AUTOLOG("[S12-THEO] ap lenh run cho CHINH MINH dich=(%d,%d) t=%u", (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
			g_uS12CuaSoSelf = timeGetTime();	// lam tuoi cua so cho lo trinh dai hon 3s
		}
#endif
	}
}

void KProtocolProcess::NetCommandSit(BYTE* pMsg)
{
	NPC_SIT_SYNC*	pSitSync;

	pSitSync = (NPC_SIT_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pSitSync->ID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendCommand(do_sit);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::OpenSaleBox(BYTE* pMsg)
{
	SALE_BOX_SYNC* pSale = (SALE_BOX_SYNC *)pMsg;
	BuySell.OpenSale((BuySellInfo*)&pSale->m_BuySellInfo);
}

void KProtocolProcess::OpenStoreBox(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_STORE_BOX, NULL, NULL);
}

void KProtocolProcess::OpenResetPass(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_RESET_PASS, NULL, NULL);
}

void KProtocolProcess::OpenDaTauBox(BYTE* pMsg)
{	
	CoreDataChanged(GDCNI_OPEN_DATAU_BOX, NULL, NULL);
}

void KProtocolProcess::OpenDaTau1Box(BYTE* pMsg)
{	
	CoreDataChanged(GDCNI_OPEN_DATAU_BOX1, NULL, NULL);
}

void KProtocolProcess::s2c_ExitGame(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_S2C_EXIT_GAME, NULL, NULL);
}

void KProtocolProcess::s2cPlayerItemLockSync(BYTE* pMsg)
{
	PLAYER_ITEM_LOCK_SYNC* pSync = (PLAYER_ITEM_LOCK_SYNC*)pMsg;
	if(pSync && pSync->m_ItemIdx > 0)
	{
		int nIdx = ItemSet.SearchID(pSync->m_ItemIdx);
		Item[nIdx].SetPlayerItemLock(pSync->m_InsuranceCourse);
		Item[nIdx].SetPlayerItemHLock(pSync->m_InsuranceHourCourse);
		Item[nIdx].SetPrice(pSync->sPrice);//#Fix ban vat pham khoa bao hiem // vÀt ph»m kho∏ kh´ng th” b∏n set gi∏ bªng 0

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Thao t∏c b∂o hi”m trang bﬁ hoµn t t.");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

void KProtocolProcess::PlayerRevive(BYTE* pMsg)
{
	NPC_REVIVE_SYNC* pSync = (NPC_REVIVE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		if (!Npc[nIdx].IsPlayer() && pSync->Type == REMOTE_REVIVE_TYPE)
		{
			AUTOLOG_EVERY(1000, "NPC-DEL-DEATH npc=%u idx=%d kind=%u type=%d cell=(%d,%d) off=(%d,%d) reg=%d t=%u", pSync->ID, nIdx, Npc[nIdx].m_Kind, (int)pSync->Type, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
			SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, nIdx); //xoa npc khi chet edit by phong kieu
			return;
		}
		else
		{
#ifndef _SERVER
			// [S7 26/08] nhan hoi sinh tu server - ghi TRUOC/SAU de bat benh "nam bep".
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				AUTOLOG("[S7-REV-CLI] id=%u type=%d doing=%d cdoing=%d frame=%d/%d reg=%d t=%u", pSync->ID, (int)pSync->Type, (int)Npc[nIdx].m_Doing, (int)Npc[nIdx].m_ClientDoing, Npc[nIdx].m_Frames.nCurrentFrame, Npc[nIdx].m_Frames.nTotalFrame, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
#endif
			Npc[nIdx].ProcNetCommand(do_revive);
#ifndef _SERVER
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				AUTOLOG("[S7-REV-CLI2] sau DoStand: doing=%d cdoing=%d reg=%d t=%u", (int)Npc[nIdx].m_Doing, (int)Npc[nIdx].m_ClientDoing, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
#endif
		}
	}
}

void KProtocolProcess::RequestNpcFail(BYTE* pMsg)
{
	NPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;

	if (NpcSet.IsNpcRequestExist(pNpcSync->ID))
		NpcSet.RemoveNpcRequest(pNpcSync->ID);	
#ifndef _SERVER
	// [S11-XOAMA 26/08] Server vua XAC NHAN 'ID nay khong con' - truoc day handler chi tra
	// khe yeu cau va DE NGUYEN ban sao ma trong bang => probe vo dung voi ma pho bien nhat
	// (NPC da bi xoa han: 323 ma bi bo don 55s hot trong 282 giay). Go luon ban sao.
	// DecRef co GAC do_death/do_revive theo dung khuon bo don CheckBalance - khuon go cua
	// NetCommandRemoveNpc DecRef vo dieu kien se lam am bang dem BYTE tham chieu o (tran
	// 255 = ket o vinh vien) neu xac vua nhan goi chet giua luc hoi va luc fail ve.
	{
		int nIdxMa = NpcSet.SearchID(pNpcSync->ID);
		if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdxMa) && !Npc[nIdxMa].m_bClientOnly && Npc[nIdxMa].m_Kind != kind_partner)
		{
			AUTOLOG("[S11-XOAMA] npc=%u idx=%d kind=%u doing=%d reg=%d life=%d -> go ma theo xac nhan server t=%u", pNpcSync->ID, nIdxMa, Npc[nIdxMa].m_Kind, (int)Npc[nIdxMa].m_Doing, Npc[nIdxMa].m_RegionIndex, Npc[nIdxMa].m_CurrentLife, SubWorld[0].m_dwCurrentTime);
			if (Npc[nIdxMa].m_RegionIndex >= 0)
			{
				SubWorld[0].m_Region[Npc[nIdxMa].m_RegionIndex].RemoveNpc(nIdxMa);
				if (Npc[nIdxMa].m_Doing != do_death && Npc[nIdxMa].m_Doing != do_revive)
					SubWorld[0].m_Region[Npc[nIdxMa].m_RegionIndex].DecRef(Npc[nIdxMa].m_MapX, Npc[nIdxMa].m_MapY, obj_npc);
			}
			NpcSet.Remove(nIdxMa);
		}
	}
#endif
}

void KProtocolProcess::NetCommandSkill(BYTE* pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel, nSkillEnChance;
	int		MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	nSkillEnChance = *(int *)&pMsg[21];
	if ((nSkillID >= 1363 && nSkillID <= 1384) || (nSkillID >= 1965 && nSkillID <= 1991) || (nSkillID >= 2114 && nSkillID <= 2143))	// [VHTD 02/09k]
		AUTOLOG("[VH-CL-CAST-NET] npc=%u idx=%d skill=%d lv=%d map=(%d,%d) ench=%d me=%d t=%u", dwNpcID, NpcSet.SearchID(dwNpcID), nSkillID, nSkillLevel, MapX, MapY, nSkillEnChance, Player[CLIENT_PLAYER_INDEX].m_nIndex, SubWorld[0].m_dwCurrentTime);
	AUTOLOG_EVERY(200, "NETSKILL-RX npc=%u idx=%d skill=%d lv=%d mapx=%d mapy=%d muctieuidx=%d ench=%d t=%u", dwNpcID, NpcSet.SearchID(dwNpcID), nSkillID, nSkillLevel, MapX, MapY, NpcSet.SearchID(MapY), nSkillEnChance, SubWorld[0].m_dwCurrentTime);
	
	AUTOLOG_EVERY(1000, "NETSKILL-SKIP-Y npc=%u skill=%d lv=%d mapx=%d mapy=%d t=%u", dwNpcID, nSkillID, nSkillLevel, MapX, MapY, SubWorld[0].m_dwCurrentTime);
	if (MapY < 0)
		return ;

	//µ±÷∏∂®ƒ≥∏ˆƒø±Í ±(MapX == -1),MapYŒ™ƒø±ÍµƒNpcdwID£¨–Ë“™◊™ªª≥…±æµÿµƒNpcIndex≤≈––
	if (MapX < 0)
	{
		AUTOLOG_EVERY(1000, "NETSKILL-SKIP-X npc=%u skill=%d lv=%d mapx=%d mapy=%d t=%u", dwNpcID, nSkillID, nSkillLevel, MapX, MapY, SubWorld[0].m_dwCurrentTime);
		if (MapX != -1)
			return;
	
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)
			return;

		if (Npc[MapY].m_RegionIndex < 0)
			return;

	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	AUTOLOG_EVERY(1000, "NETSKILL-NOIDX npc=%u idx=%d skill=%d lv=%d mapx=%d mapy=%d t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, MapX, MapY, SubWorld[0].m_dwCurrentTime);
	if (nIdx <= 0) 
		return;

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		AUTOLOG_EVERY(1000, "NETSKILL-DO npc=%u idx=%d skill=%d lv=%d mapx=%d mapy=%d caster_cell=(%d,%d) off=(%d,%d) reg=%d me_idx=%d me_cell=(%d,%d) t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, MapX, MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, Player[CLIENT_PLAYER_INDEX].m_nIndex, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].m_SkillList.SetSkillLevel(nSkillID, nSkillLevel);
		Npc[nIdx].SendCommand(do_skill, nSkillID, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandWalk(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
#ifndef _SERVER
	AUTOLOG("[S6-CMD] lenh=walk npc=%u idx=%d ap=%d dich=(%d,%d) t=%u", dwNpcID, nIdx, (int)(Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) ? 1 : 0), (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
#endif
	BOOL bS12Self = FALSE;
#ifndef _SERVER
	bS12Self = S12_ChoPhepSelf(nIdx, (int)MapX, (int)MapY);
#endif
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx) || bS12Self)
	{
		AUTOLOG_EVERY(300, "NET-WALK npc=%u idx=%d dichmps=(%d,%d) cell=(%d,%d) off=(%d,%d) reg=%d doing=%d t=%u", dwNpcID, nIdx, (int)MapX, (int)MapY, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);
		Npc[nIdx].SendCommand(do_walk, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
#ifndef _SERVER
		if (bS12Self)
		{
			AUTOLOG("[S12-THEO] ap lenh walk cho CHINH MINH dich=(%d,%d) t=%u", (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
			g_uS12CuaSoSelf = timeGetTime();
		}
#endif
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyCreateTeamFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_TEAM_FALSE*)pMsg;
	KSystemMessage	sMsg;

	switch (pCreateFalse->m_btErrorID)
	{
	// 
	case Team_Create_Error_InTeam:
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		break;

	// 
	case Team_Create_Error_CannotCreate:
		sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;


	default:
		sprintf(sMsg.szMessage, MSG_TEAM_CREATE_FAIL);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyTeamInfoFalse(BYTE* pMsg)
{
	// ΩÁ√Êœ‘ æ∂”ŒÈ≤È—Ø ß∞‹(not end)
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriend(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_SYNC	*pFriend = (CHAT_ADD_FRIEND_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nIdx, pFriend->m_szName, 0);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	memset(&sPlayer, 0, sizeof(KUiPlayerItem));
	strcpy(sPlayer.Name, pFriend->m_szName);
	sPlayer.uId = pFriend->m_dwID;
	sPlayer.nIndex = pFriend->m_nIdx;

	sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, pFriend->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_INTERVIEW;
	sMsg.byPriority = 2;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriendFail(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_FAIL_SYNC	*pFail = (CHAT_ADD_FRIEND_FAIL_SYNC*)pMsg;

	char	szName[32];
	CChatApplyListNode	*pList;
	pList = (CChatApplyListNode*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.GetHead();
	while (pList)
	{
		if (pList->m_nPlayerIdx == pFail->m_nTargetPlayerIdx)
		{
			g_StrCpy(szName, pList->m_szName);

			// 
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			return;
		}
		pList = (CChatApplyListNode*)pList->GetNext();
	}	
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddOneFriend(BYTE* pMsg)
{
	CHAT_ONE_FRIEND_DATA_SYNC	*pData = (CHAT_ONE_FRIEND_DATA_SYNC*)pMsg;
	int		nTeamNo;

	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pData->m_szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pData->m_dwID, pData->m_nPlayerIdx, pData->m_szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatBeRefusedAddFriend(BYTE* pMsg)
{
	CHAT_REFUSE_FRIEND_SYNC	*pRefuse = (CHAT_REFUSE_FRIEND_SYNC*)pMsg;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	BIEN_ChepAnToan(szName, (int)sizeof(szName), pRefuse->m_szName, (int)(pRefuse->m_wLength + 1 + sizeof(pRefuse->m_szName) - sizeof(CHAT_REFUSE_FRIEND_SYNC)), "szName");

	// 
	KSystemMessage	sMsg;

	sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatFriendOnline(BYTE* pMsg)
{
	CHAT_FRIEND_ONLINE_SYNC	*pFriend = (CHAT_FRIEND_ONLINE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].ChatFriendOnLine(pFriend->m_dwID, pFriend->m_nPlayerIdx);
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatGetApplyAddFriend(BYTE* pMsg)
{
	CHAT_APPLY_ADD_FRIEND_SYNC	*pApply = (CHAT_APPLY_ADD_FRIEND_SYNC*)pMsg;

	if ( Player[CLIENT_PLAYER_INDEX].m_cChat.CheckIsFriend(pApply->m_nSrcPlayerIdx) )
		return;

	CChatApplyListNode	*pNode = NULL;
	pNode = new CChatApplyListNode;
	if (pNode == NULL)
		return;
	pNode->m_nPlayerIdx = pApply->m_nSrcPlayerIdx;
	strcpy(pNode->m_szName, pApply->m_szSourceName);
	strcpy(pNode->m_szInfo, pApply->m_szInfo);

	Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.AddTail(pNode);

	// 
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	strcpy(sPlayer.Name, pApply->m_szSourceName);
	sPlayer.nIndex = pApply->m_nSrcPlayerIdx;
	sPlayer.uId = 0;
	sPlayer.nData = 0;

	sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, pNode->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NAME_SYNC*)pMsg;
	int		nTeamNo;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	BIEN_ChepAnToan(szName, (int)sizeof(szName), pFriend->m_szName, (int)(pFriend->m_wLength + 1 + sizeof(pFriend->m_szName) - sizeof(CHAT_LOGIN_FRIEND_NAME_SYNC)), "szName");
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendNoName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NONAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NONAME_SYNC*)pMsg;
	char	szName[32];
	int		nTeamNo;

	szName[0] = 0;
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, NULL);
	if (nTeamNo < 0)
	{
		if (Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag == 0)		// …Í«Î÷ÿ–¬∑¢ÀÕÀ˘”–∫√”— ˝æ›£¨¥¯√˚◊÷
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

			CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplyresendallfriendname;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND));
		}
		else												// 
		{
			CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplysendonefriendname;
			sApply.m_dwID = pFriend->m_dwID;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND));
		}
		return;
	}
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cCreateTeam(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_SUCCESS	*pCreateSuccess = (PLAYER_SEND_CREATE_TEAM_SUCCESS*)pMsg;

	g_Team[0].CreateTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level, pCreateSuccess->nTeamServerID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].RestoreCurrentCamp();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	sprintf(sMsg.szMessage, MSG_TEAM_CREATE);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_CREATE_TEAM);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cFactionSkillOpen(BYTE* pMsg)
{
	PLAYER_FACTION_SKILL_LEVEL	*pSkill = (PLAYER_FACTION_SKILL_LEVEL*)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction < 0 || Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction != pSkill->m_btCurFactionID)
	{
		// 
		Player[CLIENT_PLAYER_INDEX].ApplyFactionData();
		return;
	}

//	for (int i = 0; i < FACTIONS_PRR_SERIES; i++)
//	{
//		if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nID == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction)
//		{
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel = pSkill->m_btLevel;
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//			break;
//		}
//	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetChat(BYTE* pMsg)
{
	PLAYER_SEND_CHAT_SYNC	*pChat = (PLAYER_SEND_CHAT_SYNC*)pMsg;
	char tmpName[32];
	char tmpMsg[256];
	Player[CLIENT_PLAYER_INDEX].m_cChat.GetChat(pChat);
	memset(tmpName, 0, sizeof(tmpName));
	memcpy(tmpName, &pChat->m_szSentence[0], pChat->m_btNameLen); //OK Npc Name[32]
	memset(tmpMsg, 0, sizeof(tmpMsg));
	memcpy(tmpMsg, &pChat->m_szSentence[pChat->m_btNameLen], pChat->m_wSentenceLen); //OK Npc Name[32]

	l_pDataChangedNotifyFunc->ChannelMessageArrival(
		pChat->m_btCurChannel, tmpName,
		tmpMsg, strlen(tmpMsg), true, true, pChat->m_btIsShowMsgPad);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetCurAttribute(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGetCurAttribute(pMsg);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetLeadExp(BYTE* pMsg)
{
	DWORD	dwLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	PLAYER_LEAD_EXP_SYNC	*pLeadExp = (PLAYER_LEAD_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pLeadExp->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);

	if (dwLevel < Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_LEADER_LEVEL_UP, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
		sMsg.eType = SMT_PLAYER;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetSkillLevel(BYTE* pMsg)
{
	int		nSkillIndex;
	PLAYER_SKILL_LEVEL_SYNC	*pSkill = (PLAYER_SKILL_LEVEL_SYNC*)pMsg;
	if (pSkill->m_nSkillLevel > MAX_SKILLLEVEL)
		return;
	nSkillIndex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pSkill->m_nSkillID);
	Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSkill->m_nLeavePoint;
	if ( !nSkillIndex )
	{
		if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.Add(pSkill->m_nSkillID, pSkill->m_nSkillLevel, pSkill->m_nSkillExp, pSkill->m_bTempSkill) == 0 )
			return;
	}
	else
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetLevel(nSkillIndex, pSkill->m_nSkillLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetAddLevel(pSkill->m_nSkillID, pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetCurLevel(pSkill->m_nSkillID, pSkill->m_nSkillLevel + pSkill->m_nAddLevel);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetExp(nSkillIndex, pSkill->m_nSkillExp);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetTempSkill(nSkillIndex, pSkill->m_bTempSkill);
	}

	// 
	CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
	KUiSkillData SkillData;

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSkill->m_nSkillID, 1);
	if (!pOrdinSkill)
		return;
	eSkillStyle eStyle = ( eSkillStyle ) pOrdinSkill->GetSkillStyle();
			
	switch(eStyle)
	{
	case SKILL_SS_Missles:			//	◊”µØ¿‡		±æººƒ‹”√”⁄∑¢ÀÕ◊”µØ¿‡
	case SKILL_SS_Melee:
	case SKILL_SS_InitiativeNpcState:	//	÷˜∂Ø¿‡		±æººƒ‹”√”⁄∏ƒ±‰µ±«∞Npcµƒ÷˜∂Ø◊¥Ã¨
	case SKILL_SS_PassivityNpcState:		//	±ª∂Ø¿‡		±æººƒ‹”√”⁄∏ƒ±‰Npcµƒ±ª∂Ø◊¥Ã¨
		{
			if(pOrdinSkill->IsBase())
				return;
		}break;
	case SKILL_SS_Thief:					//	Õµ«‘¿‡
		{
			
		}
		break;
	default:
		return;
	}
	int nSkillPosition = -1;
	SkillData.uGenre = CGOG_SKILL_FIGHT;
	nSkillPosition = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillPosition(pSkill->m_nSkillID);
	SkillData.uId = pSkill->m_nSkillID;
	SkillData.nLevel = pSkill->m_nSkillLevel;
	if(nSkillPosition < 0) return;
	CoreDataChanged(GDCNI_SKILL_CHANGE, (unsigned int)&SkillData, nSkillPosition);
}
//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetTeammateLevel(BYTE* pMsg)
{
	PLAYER_TEAMMATE_LEVEL_SYNC	*pLevel = (PLAYER_TEAMMATE_LEVEL_SYNC*)pMsg;
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	if ((DWORD)g_Team[0].m_nCaptain == pLevel->m_dwTeammateID)
	{
		g_Team[0].m_nMemLevel[0] = (DWORD)pLevel->m_btLevel;
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if ((DWORD)g_Team[0].m_nMember[i] == pLevel->m_dwTeammateID)
		{
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pLevel->m_btLevel;
			break;
		}
	}
}

void KProtocolProcess::s2cLeaveFaction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cFaction.LeaveFaction();
	// 
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(camp_free);

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cLeaveTeam(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
		return;
	}

	PLAYER_LEAVE_TEAM	*pLeaveTeam = (PLAYER_LEAVE_TEAM*)pMsg;

	if (pLeaveTeam->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
		g_Team[0].Release();
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
	}
	else
	{
		g_Team[0].DeleteMember(pLeaveTeam->m_dwNpcID);
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return;
}

void KProtocolProcess::s2cLevelUp(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cLevelUp(pMsg);
}

void KProtocolProcess::s2cMoveItem(BYTE* pMsg)
{
	PLAYER_MOVE_ITEM_SYNC	*pMove = (PLAYER_MOVE_ITEM_SYNC*)pMsg;

	ItemPos		DownPos, UpPos;
	DownPos.nPlace = pMove->m_btDownPos;
	DownPos.nX = pMove->m_btDownX;
	DownPos.nY = pMove->m_btDownY;
	UpPos.nPlace = pMove->m_btUpPos;
	UpPos.nX = pMove->m_btUpX;
	UpPos.nY = pMove->m_btUpY;

	Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeItem(&DownPos, &UpPos);
}

void KProtocolProcess::s2cSwitchEquip(BYTE* pMsg)
{
	PLAYER_SWITCH_EQUIP_SYNC* pMove = (PLAYER_SWITCH_EQUIP_SYNC*)pMsg;

	int activeEquipNum = pMove->m_nSetNum;

	Player[CLIENT_PLAYER_INDEX].SwitchEquipSet(activeEquipNum);
}

void KProtocolProcess::s2cRemoveItem(BYTE* pMsg)
{
	ITEM_REMOVE_SYNC	*pRemove = (ITEM_REMOVE_SYNC*)pMsg;

	int		nIdx;
	nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pRemove->m_ID);
	if (nIdx > 0)
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx);
		Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
	}
}

void KProtocolProcess::s2cRemoveAllItem(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetFactionData(BYTE* pMsg)
{
	PLAYER_FACTION_DATA	*pData = (PLAYER_FACTION_DATA*)pMsg;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(pData->m_btCamp);
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction = (char)pData->m_btCurFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nFirstAddFaction = (char)pData->m_btFirstFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nAddTimes = pData->m_nAddTimes;

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel = pData->m_btNo1SkillOpenLevel;
//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel = pData->m_btNo2SkillOpenLevel;
//	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nID)
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//	else
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetTeamState(BYTE* pMsg)
{
	// 
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_OPEN_CLOSE	*pTeamState = (PLAYER_TEAM_OPEN_CLOSE*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	if (pTeamState->m_btOpenClose)
	{
		g_Team[0].SetTeamOpen();

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_OPEN);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else
	{
		g_Team[0].SetTeamClose();

		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CLOSE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

void	KProtocolProcess::s2cApplyAddTeam(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cApplyAddTeam(pMsg);
}

void KProtocolProcess::s2cShowTeamInfo(BYTE* pMsg)
{

}

void KProtocolProcess::s2cSyncItem(BYTE* pMsg)
{
	ITEM_SYNC	*pItemSync = (ITEM_SYNC*)pMsg;
	int nIndex = 0;
	if (pItemSync->m_bIsNew)
	{
		if (pItemSync->m_Nature < 2) { //no GOLDEQUIP2 or PLATINA, add normal use old code
			if (!pItemSync->m_GoldId)
				nIndex = ItemSet.AddItemSet2(
					pItemSync->m_Genre,
					pItemSync->m_Series,
					pItemSync->m_Level,
					pItemSync->m_Luck,
					pItemSync->m_Detail,
					pItemSync->m_Particur,
					pItemSync->m_MagicLevel,
					pItemSync->m_Version,
					pItemSync->m_RandomSeed,
					pItemSync->m_StackNum,
					pItemSync->m_EnChance,
					pItemSync->m_Point, pItemSync->m_TimeE, 0, 0, 0, 0, 0, pItemSync->m_MaxOptMultiply);
			else
				nIndex = ItemSet.AddGoldItem(
					pItemSync->m_GoldId,
					pItemSync->m_MagicLevel,
					pItemSync->m_Series,
					pItemSync->m_EnChance, pItemSync->m_TimeE, 0, 0, 0, 0, pItemSync->m_MaxOptMultiply);
		}
		else
			nIndex = ItemSet.Add(
			pItemSync->m_Nature,
			pItemSync->m_Genre,
			pItemSync->m_Series,
			pItemSync->m_Level,
			pItemSync->m_Luck,
			pItemSync->m_Detail,
			pItemSync->m_Particur,
			pItemSync->m_MagicLevel,
			pItemSync->m_Version,
			pItemSync->m_RandomSeed,
			pItemSync->m_MaxOptMultiply);
	}
	else
		nIndex = ItemSet.SearchID(pItemSync->m_ID);
	if (nIndex)
	{
		Item[nIndex].SetTemp(pItemSync->m_bTemp);
		Item[nIndex].SetOwner(pItemSync->m_dwOwner);
		Item[nIndex].SetTradePrice(pItemSync->m_Price);
		Item[nIndex].SetID(pItemSync->m_ID);
		if (pItemSync->m_Nature >= NATURE_GOLD)
			Item[nIndex].SetRow(pItemSync->m_Detail);
		else
			Item[nIndex].SetDetailType(pItemSync->m_Detail);
		Item[nIndex].SetNature(pItemSync->m_Nature);
		Item[nIndex].SetGenre(pItemSync->m_Genre);
		Item[nIndex].SetParticular(pItemSync->m_Particur);
		Item[nIndex].SetLevel(pItemSync->m_Level);
		Item[nIndex].SetDurability(pItemSync->m_Durability);
		Item[nIndex].SetExpireTime(pItemSync->m_ExpireTime);
		Item[nIndex].SetLock(&pItemSync->m_LockItem);
		Item[nIndex].SetLockSell(pItemSync->m_bLockSell);
		Item[nIndex].SetLockTrade(pItemSync->m_bLockTrade);
		Item[nIndex].SetLockDrop(pItemSync->m_bLockDrop);
		Item[nIndex].SetParam(pItemSync->m_Param);
		Item[nIndex].SetMantle(pItemSync->m_Mantle);
		Item[nIndex].SetBackLocal(&pItemSync->m_BackLocal);
		Item[nIndex].SetFortune(pItemSync->m_Fortune);
		Item[nIndex].SetStackNum(pItemSync->m_StackNum);
		// [PF13 31/08c] goi mang du 16 int (8 type + 8 gia tri MAKELONG) nhung
		// ban cu chi chep 8 -> nua gia tri khong bao gio duoc lam tuoi phia client
		memcpy(Item[nIndex].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		Item[nIndex].m_GeneratorParam.uRandomSeed = pItemSync->m_RandomSeed;
		Item[nIndex].m_GeneratorParam.nVersion = pItemSync->m_Version;
		Item[nIndex].m_GeneratorParam.nLuck = pItemSync->m_Luck;
		Item[nIndex].SetExpTime(pItemSync->m_TimeE, 0, 0, 0);

		if (pItemSync->m_bIsNew)
			Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);
		// [XEPDO 28/08] Goi bIsNew=false la CAP NHAT thuoc tinh (server chi gui no
		// tu SyncItem(nIdx) mot tham so khi so luong chong doi - place/x/y trong
		// goi la 0). Ban cu AddKIL them lan nua voi m_Items[nIndex] - nIndex la
		// chi so ITEMSET tra vao mang SLOT cua KItemList = doc du lieu RAC, lam
		// item co 2 entry + 2 vung grid client. Auto (ban rac / cat ruong / xep
		// do) quet grid gap o ma roi gui lenh theo dwID cua ruot hien hanh =>
		// ban/cat nham mon that (su co 28/08: mat Tho Dia Phu vo han x2, item
		// 'doi hinh', xep do xong item 'xuat hien lai'). Thuoc tinh da duoc gan
		// het o khoi tren; item van dung nguyen cho cu - KHONG AddKIL gi them.
	}

	//int pnMagicParam[6];
	//for (int i = 0; i < 6; i++)
	//{
	//	pnMagicParam[i] = pItemSync->m_MagicLevel[i];
	//}
	//int nIndex = 0;
	//if (!pItemSync->m_bIsNew)
	//	nIndex = ItemSet.SearchID(pItemSync->m_ID);
	//else if (!pItemSync->m_GoldId)
	//	nIndex = ItemSet.AddItemSet2(
	//	pItemSync->m_Genre, 
	//	pItemSync->m_Series,
	//	pItemSync->m_Level,
	//	pItemSync->m_Luck,
	//	pItemSync->m_Detail,
	//	pItemSync->m_Particur,
	//	pnMagicParam,
	//	pItemSync->m_Version,
	//	pItemSync->m_RandomSeed,
	//	pItemSync->m_StackNum,
	//	pItemSync->m_EnChance,
	//	pItemSync->m_Point, pItemSync->m_TimeE, 0, 0, 0);
	//else
	//	nIndex = ItemSet.AddGoldItem(
	//	pItemSync->m_GoldId,
	//	pnMagicParam,
	//	pItemSync->m_Series,
	//	pItemSync->m_EnChance, pItemSync->m_TimeE, 0, 0, 0);

	//if (nIndex > 0)
	//{
	//	Item[nIndex].SetStackNum(pItemSync->m_StackNum);
	//	Item[nIndex].SetID(pItemSync->m_ID);
	//	Item[nIndex].SetDurability((short)pItemSync->m_Durability);
	//	Item[nIndex].SetPlayerItemLock(pItemSync->m_InsuranceCourse);
	//	Item[nIndex].SetPlayerItemHLock(pItemSync->m_HInsuranceCourse);
	//	Item[nIndex].SetMantle(pItemSync->m_Mantle); //#phi phong
	//	Item[nIndex].SetParam(pItemSync->m_Param); //#s?l«n s?dÙng item
	//	Item[nIndex].SetItemGlowLight(pItemSync->m_GlowLight); //#ngoπi trang v?kh?ph∏t s∏ng
	//	Item[nIndex].SetPrice(pItemSync->m_Price);
	//	Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);
	//}
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void KProtocolProcess::s2cSyncMoney(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncMoney(pMsg);
}

void KProtocolProcess::s2cSyncRoleList(BYTE* pMsg)
{
	ROLE_LIST_SYNC	*pSync = (ROLE_LIST_SYNC *)pMsg;
}

void KProtocolProcess::s2cTeamAddMember(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_ADD_MEMBER	*pAddMem = (PLAYER_TEAM_ADD_MEMBER*)pMsg;
	int		nMemNum;

	nMemNum = g_Team[0].FindFree();
	if (nMemNum < 0)
		return;
	g_Team[0].m_nMember[nMemNum] = pAddMem->m_dwNpcID;
	g_Team[0].m_nMemLevel[nMemNum + 1] = (DWORD)pAddMem->m_btLevel;
	strcpy(g_Team[0].m_szMemName[nMemNum + 1], pAddMem->m_szName);
	g_Team[0].m_nMemNum++;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(pAddMem->m_dwNpcID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_ADD_MEMBER, pAddMem->m_szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamChangeCaptain(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	int		nLevel, nMemNo;
	char	szName[32];
	PLAYER_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_TEAM_CHANGE_CAPTAIN*)pMsg;

	nMemNo = g_Team[0].FindMemberID(pChange->m_dwCaptainID);
	// 
	if ((DWORD)g_Team[0].m_nCaptain != pChange->m_dwMemberID || nMemNo < 0)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	// 
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_SELF, g_Team[0].m_szMemName[0]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else	// 
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN, g_Team[0].m_szMemName[nMemNo + 1]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}

	// 
	nLevel = g_Team[0].m_nMemLevel[0];
	strcpy(szName, g_Team[0].m_szMemName[0]);
	g_Team[0].m_nCaptain = pChange->m_dwCaptainID;
	g_Team[0].m_nMemLevel[0] = g_Team[0].m_nMemLevel[nMemNo + 1];
	strcpy(g_Team[0].m_szMemName[0], g_Team[0].m_szMemName[nMemNo + 1]);
	g_Team[0].m_nMember[nMemNo] = pChange->m_dwMemberID;
	g_Team[0].m_nMemLevel[nMemNo + 1] = nLevel;
	strcpy(g_Team[0].m_szMemName[nMemNo + 1], szName);

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}
	else if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwMemberID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void KProtocolProcess::s2cUpdataSelfTeamInfo(BYTE* pMsg)
{
	int		i;
	PLAYER_SEND_SELF_TEAM_INFO	*pSelfInfo = (PLAYER_SEND_SELF_TEAM_INFO*)pMsg;

	//  ˝æ›∞¸≥ˆ¥Ì
	if (pSelfInfo->m_dwNpcID[0] == 0)
		return;

	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pSelfInfo->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	if (Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp == 0)
		Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pSelfInfo->m_dwNpcID[0])	// ∂”≥§
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	}
	else													// ∂”‘±
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].Release();
	}
	g_Team[0].m_nCaptain = pSelfInfo->m_dwNpcID[0];
	if (pSelfInfo->m_btState == 0)
		g_Team[0].SetTeamClose();
	else
		g_Team[0].SetTeamOpen();
	g_Team[0].m_nTeamServerID = pSelfInfo->nTeamServerID;
	g_Team[0].m_nMemLevel[0] = (DWORD)pSelfInfo->m_btLevel[0];
	strcpy(g_Team[0].m_szMemName[0], pSelfInfo->m_szNpcName[0]);
	g_Team[0].m_nMemNum = 0;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (pSelfInfo->m_dwNpcID[i + 1] > 0)
		{
			g_Team[0].m_nMember[i] = pSelfInfo->m_dwNpcID[i + 1];
			g_Team[0].m_nMemLevel[i + 1] = (DWORD)pSelfInfo->m_btLevel[i + 1];
			strcpy(g_Team[0].m_szMemName[i + 1], pSelfInfo->m_szNpcName[i + 1]);
			g_Team[0].m_nMemNum++;
		}
		else
		{
			g_Team[0].m_nMember[i] = -1;
			g_Team[0].m_nMemLevel[i + 1] = 0;
			g_Team[0].m_szMemName[i + 1][0] = 0;
		}
	}
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
}

void KProtocolProcess::SyncCurNormalData(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC	*pSync = (CURPLAYER_NORMAL_SYNC*)pMsg;
	if (pSync->m_shLife > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = pSync->m_shLife;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = 0;
	if (pSync->m_shStamina > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = pSync->m_shStamina;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = 0;

	AUTOLOG_EVERY(2000, "ME-VITAL me idx=%d life=%d mana=%d sta=%d clientlife=%d/%d doing=%d t=%u", Player[CLIENT_PLAYER_INDEX].m_nIndex, (int)pSync->m_shLife, (int)pSync->m_shMana, (int)pSync->m_shStamina, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax, (int)Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing, SubWorld[0].m_dwCurrentTime);
	if (pSync->m_shMana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = pSync->m_shMana;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = 0;

//	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
//	{
//		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax &&
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax)
//		{
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
//		}
//	}

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	switch (pSync->m_btTeamData)
	{
	case 0x00:
		if ( Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x03:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x01:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_MEMBER)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	}
	//sync to ext auto
	bool bSync = false;
	for (int i=1; i<MAX_NPCSKILL; i++)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.m_Skills[i].SkillId > 0)
		{
			bSync = true;
			break;
		}
	}
	if(bSync)
	{
		IPCMainSync s;
		s.CmdID = PRG_MAINSYNC;
		s.Size = sizeof(IPCMainSync);
		strcpy(s.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		int x,y;
		g_ScenePlace.GetSceneNameAndFocus(s.szMap, s.nMapId, x, y);
		s.nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		s.dwPID = Player[CLIENT_PLAYER_INDEX].m_dwID;
		s.life = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;
		s.mana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;
		s.lifemax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;
		s.manamax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;
		s.stamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;
		s.staminamax = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&s.mapx, &s.mapy);
		s.level = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;
		s.curexp = Player[CLIENT_PLAYER_INDEX].m_nExp;
		s.fullexp = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
		s.skillnum = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAllSkillByType(&s.skill[0]);
		s.nSelServer1 = PlayerSet.m_nSelSvGroup;
		s.nSelServer2 = PlayerSet.m_nSelServer;
		strcpy(s.szPassword, PlayerSet.m_szPassword);
		strcpy(s.szAccount, PlayerSet.m_szAccount);
		// [r2] dong trang thai auto cho chan WAuto (WA_HoatDong o CoreShell.cpp -
		// tep do KHONG bien dich ban server nen phai guard)
#ifndef _SERVER
		{
			extern void WA_HoatDong(int nPlayerIdx, char* szOut, int nMax);
			WA_HoatDong(CLIENT_PLAYER_INDEX, s.szHoatDong, sizeof(s.szHoatDong));
		}
#else
		s.szHoatDong[0] = 0;
#endif
		SendDataToTool(&s, sizeof(IPCMainSync));
	}
}

void KProtocolProcess::SyncCurPlayer(BYTE* pMsg) //Sync Player 1 l«n ch›nh m◊nh
{
	Player[CLIENT_PLAYER_INDEX].SyncCurPlayer(pMsg);
}

void KProtocolProcess::SyncNpc(BYTE* pMsg)	//Sync 1 l«n khi npc trong Æ„ c„ player
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	AUTOLOG_EVERY(1000, "SYNCNPC-REGION-BAD npc=%u mps=(%d,%d) region=%d kind=%d set=%d t=%u", NpcSync->ID, NpcSync->MapX, NpcSync->MapY, nRegion, (int)NpcSync->m_btKind, NpcSync->NpcSettingIdx, SubWorld[0].m_dwCurrentTime);

	if (nRegion == -1)
	{
#ifndef _SERVER
		// [S11-MAPLA 26/08] Toa do goi tra loi KHONG thuoc cua so map hien tai (tra loi
		// xuyen map / goi qua cu). Truoc day return im lang: (a) GIU nguyen ban sao ma,
		// (b) khe yeu cau REQNPC ket 100 tick moi tu hoi. Go ban sao (DecRef co gac nhu
		// [S11-XOAMA]) + tra khe ngay.
		int nIdxLa = NpcSet.SearchID(NpcSync->ID);
		if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdxLa) && !Npc[nIdxLa].m_bClientOnly && Npc[nIdxLa].m_Kind != kind_partner)
		{
			AUTOLOG("[S11-MAPLA] npc=%u idx=%d doing=%d reg=%d mps=(%d,%d) ngoai map -> go t=%u", NpcSync->ID, nIdxLa, (int)Npc[nIdxLa].m_Doing, Npc[nIdxLa].m_RegionIndex, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);
			if (Npc[nIdxLa].m_RegionIndex >= 0)
			{
				SubWorld[0].m_Region[Npc[nIdxLa].m_RegionIndex].RemoveNpc(nIdxLa);
				if (Npc[nIdxLa].m_Doing != do_death && Npc[nIdxLa].m_Doing != do_revive)
					SubWorld[0].m_Region[Npc[nIdxLa].m_RegionIndex].DecRef(Npc[nIdxLa].m_MapX, Npc[nIdxLa].m_MapY, obj_npc);
			}
			NpcSet.Remove(nIdxLa);
		}
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
			NpcSet.RemoveNpcRequest(NpcSync->ID);
#endif
		return;
	}

	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
#ifndef _SERVER
		// [FIX-D 26/08] khong nhan NPC ngoai tam ve vao bang.
		if (S6_XaQuaTam(NpcSync->MapX, NpcSync->MapY))
		{
			g_nS6BoXa++;
			// [S8 26/08] PHAI TRA KHE YEU CAU TRUOC KHI BO GOI.
			// Goi SyncNpc day la TRA LOI cho yeu cau ma CHINH client da gui
			// (SyncNpcMin thay NPC la -> InsertNpcRequest -> SendClientCmdRequestNpc).
			// Khe chi duoc tra o duoi (dong ~1906 RemoveNpcRequest) - nam SAU cai
			// return nay, nen moi goi bi FIX D bo la RO RI MOT KHE.
			// Be chi co 19 khe dung duoc (MAX_NPC_REQUEST 20 nhung KNpcSet.cpp:90
			// nap chi so 1..19), va duong thu hoi con lai la quet timeout 100 khung
			// (~5,5 giay). Trong tran Tong Kim hang tram nguoi lien tuc bang qua vanh
			// 40 o thi be can khe trong vai giay; luc do InsertNpcRequest tra FALSE
			// (KNpcSet.cpp:1372) => client KHONG HOI NPC MOI NAO NUA, mu ca NPC TINH
			// dung sat ben (Quan Y cach diem hoi sinh 12 o) - dung hai trieu chung chu
			// game bao: auto khong thay NPC bao danh / khong thay Quan Y de mua mau.
			if (NpcSet.IsNpcRequestExist(NpcSync->ID))
				NpcSet.RemoveNpcRequest(NpcSync->ID);
			return;
		}
#endif
		nIdx = NpcSet.AddNpcSet2(NpcSync->NpcSettingIdx, NpcSync->m_bySeries, 0, NpcSync->MapX, NpcSync->MapY);
		AUTOLOG_EVERY(1000, "SYNCNPC-ADDFAIL npc=%u idx=%d set=%d mps=(%d,%d) cell=(%d,%d) region=%d t=%u", NpcSync->ID, nIdx, NpcSync->NpcSettingIdx, NpcSync->MapX, NpcSync->MapY, nMapX, nMapY, nRegion, SubWorld[0].m_dwCurrentTime);
#ifndef _SERVER
		// idx=0 = KHONG THEM DUOC (het khe hoac Mps2Map tra region xau).
		AUTOLOG("[S6-ADD] npc=%u idx=%d kind=%u set=%d mps=(%d,%d) cell=(%d,%d) reg=%d dung=%d/%d t=%u", NpcSync->ID, nIdx, (unsigned int)NpcSync->m_btKind, NpcSync->NpcSettingIdx, NpcSync->MapX, NpcSync->MapY, nMapX, nMapY, nRegion, S6_UsedSlots(), (int)MAX_NPC, SubWorld[0].m_dwCurrentTime);
#endif
#ifndef _SERVER
		// [S8 26/08] BANG 256 KHE DA DAY (AddNpcSet2 -> AddNpcSet1 -> FindFree() = 0).
		// Ban goc van chay tiep va ghi Npc[0].m_dwID/m_Kind - ma Npc[0] la O TRONG
		// dung lam gia tri "khong tim thay" cua ca he (NpcSet.SearchID / FindFree /
		// DT_FindNpcName deu coi 0 la khong co). Ghi de vao do vua lam ban o trong,
		// vua XOA LUON yeu cau dang cho (RemoveNpcRequest ngay duoi) nen client se
		// KHONG BAO GIO hoi lai con NPC do - voi NPC DUNG YEN (Quan Y, Xa Phu, NPC
		// bao danh) thi may chu it khi phat lai NormalSync, nghia la mat han.
		// Dung: bo goi, GIU nguyen yeu cau de khe tu het han (~5,5 giay) roi hoi lai.
		if (!nIdx)
			return;
#endif
		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
			NpcSet.RemoveNpcRequest(NpcSync->ID);
	}
	else
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}

		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = nOffX;
		Npc[nIdx].m_OffY = nOffY;

		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);

		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if (Npc[nIdx].m_RegionIndex != nRegion)
			{
				// zroc change
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
				Npc[nIdx].m_RegionIndex = nRegion;
			}
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
		else
		{
			SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegion;
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		}
	}

	Npc[nIdx].m_Camp			= (NPCCAMP)NpcSync->Camp;
	Npc[nIdx].m_CurrentCamp		= (NPCCAMP)NpcSync->CurrentCamp;
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
	Npc[nIdx].m_Type			= NpcSync->NpcEnchant;
	/*if (NpcSync->LifePerCent <= 128)
		Npc[nIdx].m_CurrentLife	= (Npc[nIdx].m_CurrentLifeMax * NpcSync->LifePerCent) >> 7 ;
	else
		Npc[nIdx].m_CurrentLife	= 0;*/

	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive) // need check later -- spe 03/05/27
		Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);

	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	AUTOLOG_EVERY(1000, "SYNCNPC-SETPOS npc=%u idx=%d kind=%u doing=%d cell=(%d,%d) off=(%d,%d) reg=%d life=%d/%d t=%u", Npc[nIdx].m_dwID, nIdx, Npc[nIdx].m_Kind, (int)NpcSync->m_Doing, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, Npc[nIdx].m_RegionIndex, NpcSync->m_CurrentLife, NpcSync->m_CurrentLifeMax, SubWorld[0].m_dwCurrentTime);
	Npc[nIdx].SetMenuState(NpcSync->m_btMenuState);

	Npc[nIdx].m_CurrentLife		= NpcSync->m_CurrentLife;
	Npc[nIdx].m_CurrentLifeMax	= NpcSync->m_CurrentLifeMax;
	Npc[nIdx].m_LifeMax			= NpcSync->m_LifeMax;
	Npc[nIdx].m_CurrentLifeReplenish = NpcSync->m_CurrentLifeReplenish;
	Npc[nIdx].m_LifeReplenish		= NpcSync->m_LifeReplenish;
	Npc[nIdx].m_CurrentAttackRating = NpcSync->m_CurrentAttackRating;
	Npc[nIdx].m_AttackRating		= NpcSync->m_AttackRating;
	Npc[nIdx].m_CurrentDefend		= NpcSync->m_CurrentDefend;
	Npc[nIdx].m_Defend			    = NpcSync->m_Defend;
	Npc[nIdx].m_CurrentExperience   = NpcSync->m_CurrentExperience;
	Npc[nIdx].m_Experience		    = NpcSync->m_Experience;
	Npc[nIdx].m_CurrentWalkSpeed    = NpcSync->m_CurrentWalkSpeed;
	Npc[nIdx].m_WalkSpeed			= NpcSync->m_WalkSpeed;
	Npc[nIdx].m_CurrentRunSpeed		= NpcSync->m_CurrentRunSpeed;
	Npc[nIdx].m_RunSpeed			= NpcSync->m_RunSpeed;
	Npc[nIdx].m_CurrentHitRecover   = NpcSync->m_CurrentHitRecover;		//thÍi gian phÙc hÂi
	Npc[nIdx].m_HitRecover		    = NpcSync->m_HitRecover;		//thÍi gian phÙc hÂi
	Npc[nIdx].m_nMissionGroup	= NpcSync->MissionGroup;//#NpcMissionGroup
	memset(Npc[nIdx].Name, 0, sizeof(Npc[nIdx].Name));
	BIEN_ChepAnToan(Npc[nIdx].Name, (int)sizeof(Npc[nIdx].Name), NpcSync->m_szName, (int)(NpcSync->m_wLength - (sizeof(NPC_SYNC) - 1 - sizeof(NpcSync->m_szName))), "Npc[nIdx].Name");
}

void KProtocolProcess::SyncNpcMin(BYTE* pMsg)	//Sync li™n tÙc npc trong Æ„ c„ player vµ npc
{
	NPC_NORMAL_SYNC* NpcSync = (NPC_NORMAL_SYNC *)pMsg;

	AUTOLOG_EVERY(1000, "SYNCMIN-SKIP-DEATH npc=%u idx=%d doing=%d mps=(%d,%d) life=%d t=%u", NpcSync->ID, NpcSet.SearchID(NpcSync->ID), (int)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY, NpcSync->m_CurrentLife, SubWorld[0].m_dwCurrentTime);
	if (NpcSync->Doing == do_revive || NpcSync->Doing == do_death)
		return;
	//--end add
	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{
#ifndef _SERVER
		// [FIX-D 26/08] NPC la ngoai tam ve -> khong REQNPC, khong chiem khe.
		if (S6_XaQuaTam(NpcSync->MapX, NpcSync->MapY))
		{
			g_nS6BoXa++;
			return;
		}
#endif
		AUTOLOG_EVERY(1000, "SYNCMIN-HIDE-NEW npc=%u state=0x%02X mps=(%d,%d) doing=%d t=%u", NpcSync->ID, (int)NpcSync->State, NpcSync->MapX, NpcSync->MapY, (int)NpcSync->Doing, SubWorld[0].m_dwCurrentTime);
		if(NpcSync->State & STATE_HIDE)	//npc khac' dang tang hinh, khong co san~ npc
			return;
		if (!NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			AUTOLOG_EVERY(1000, "SYNCMIN-REQNPC npc=%u mps=(%d,%d) doing=%d state=0x%02X dangcho=%d t=%u", NpcSync->ID, NpcSync->MapX, NpcSync->MapY, (int)NpcSync->Doing, (int)NpcSync->State, (int)NpcSet.IsNpcRequestExist(NpcSync->ID), SubWorld[0].m_dwCurrentTime);
			if (NpcSet.InsertNpcRequest(NpcSync->ID))
				SendClientCmdRequestNpc(NpcSync->ID);
		}
		return;//add by Fong Ki“u from KT
	}
	else
	{
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		//
		if (Npc[nIdx].m_RegionIndex == -1 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if(NpcSync->State & STATE_HIDE) //npc khac' dang tang hinh, da co npc -> xoa
			{
				NpcSet.Remove(nIdx);
				return;
			}
			AUTOLOG_EVERY(1000, "SYNCMIN-REGION-BAD npc=%u idx=%d mps=(%d,%d) region=%d t=%u", NpcSync->ID, nIdx, NpcSync->MapX, NpcSync->MapY, nRegion, SubWorld[0].m_dwCurrentTime);
			if (nRegion == -1)
			{
				// [FIX-B 26/08] NPC MO COI ma vi tri server bao da NGOAI vung nap = no da roi
				// vung quan sat -> tra khe NGAY thay vi ket den CheckBalance 55 s (truoc day
				// return im lang). Quay vao tam thi REQNPC + sync full nhu NPC moi. Giu ngoai
				// le ban dong hanh. NPC lon von RIA 40-48 o van trong vung nap - khong bi dung.
#ifndef _SERVER
				AUTOLOG("[S6-XOAXA] npc=%u idx=%d kind=%u doing=%d mps=(%d,%d) t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_Kind, (int)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);
#endif
				if (Npc[nIdx].m_Kind != kind_partner)
					NpcSet.Remove(nIdx);
				return;
			}
			else
			{
#ifndef _SERVER
				// [FIX-D 26/08] Mo coi ma van ngoai tam ve (dai 40-48 o) -> tra khe thay vi
				// gan lai roi bi go tick sau (flapping). Vao <40 o se ADD nhu NPC moi.
				if (Npc[nIdx].m_Kind != kind_partner && S6_XaQuaTam(NpcSync->MapX, NpcSync->MapY))
				{
					g_nS6BoXa++;
					NpcSet.Remove(nIdx);
					return;
				}
#endif
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = NpcSync->m_fkOffX;//nOffX;
				Npc[nIdx].m_OffY = NpcSync->m_fkOffY;//nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				Npc[nIdx].m_dwRegionID = NpcSync->m_fkRegionID;//SubWorld[0].m_Region[nRegion].m_RegionID;
				SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
				SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
#ifndef _SERVER
				// [S6 26/08] NPC MO COI duoc GAN LAI vao region theo vi tri server ("hien lai").
				AUTOLOG("[S6-ORPHAN-BACK] npc=%u idx=%d kind=%u doing=%d cell=(%d,%d) reg=%d dung=%d t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_Kind, (int)NpcSync->Doing, nMapX, nMapY, nRegion, S6_UsedSlots(), SubWorld[0].m_dwCurrentTime);
#endif
			}
			
			if (NpcSync->Doing == do_stand)
				Npc[nIdx].ProcNetCommand(do_stand);
		}
		else
		{
			AUTOLOG_EVERY(1000, "SYNCMIN-HIDE-DEL npc=%u idx=%d cell=(%d,%d) reg=%d life=%d t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_RegionIndex, Npc[nIdx].m_CurrentLife, SubWorld[0].m_dwCurrentTime);
			if(NpcSync->State & STATE_HIDE && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex) //npc khac' dang tang hinh, da co npc -> xoa
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				NpcSet.Remove(nIdx);
				return;
			}
			AUTOLOG_EVERY(1000, "SYNCMIN-CHGREGION npc=%u idx=%d cellcu=(%d,%d) regcu=%d cellmoi=(%d,%d) regmoi=%d t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_RegionIndex, nMapX, nMapY, nRegion, SubWorld[0].m_dwCurrentTime);
			if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);

				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = NpcSync->m_fkOffX;//nOffX;
				Npc[nIdx].m_OffY = NpcSync->m_fkOffY;//nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				if (nRegion >= 0)
				{
					SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
					Npc[nIdx].m_dwRegionID = NpcSync->m_fkRegionID;//SubWorld[0].m_Region[nRegion].m_RegionID;
					SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				}
			}
		}

		// Ban goc JX2 (handler 0x005eca44): neu client da gio co "toi dang sai" thi goi
		// dong bo dinh ky NAY duoc phep ghi de toa do that roi ha co. Truoc day JX1 vut
		// toan bo toa do trong goi khi NPC con o cung region, nen nguoi bi ket dung im
		// mai toi khi qua bien region moi nhay mot phat.
		// Lech <= 64px se duoc lop noi suy ve keo muot (PAINT_INTERP_SNAP_DIST);
		// lech lon hon thi noi suy tu snap - dung nhu mong muon.
		AUTOLOG_EVERY(1000, "SYNCMIN-DRIFT npc=%u idx=%d kind=%u cl=(%d,%d) cloff=(%d,%d) sv=(%d,%d) svoff=(%d,%d) d=(%d,%d) reg=%d/%d fix=%d doing=%d t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, nMapX, nMapY, NpcSync->m_fkOffX, NpcSync->m_fkOffY, (Npc[nIdx].m_MapX - nMapX), (Npc[nIdx].m_MapY - nMapY), Npc[nIdx].m_RegionIndex, nRegion, Npc[nIdx].m_nNeedFixPos, (int)Npc[nIdx].m_Doing, SubWorld[0].m_dwCurrentTime);
#ifndef _SERVER
		// [S6] Ban KHONG TIET CHE cua SYNCMIN-DRIFT. "nan=1" = client se ghi de toa do
		// (chi khi m_nNeedFixPos > 0 VA cung region) - do la luc nguoi choi thay NPC GIUT.
		AUTOLOG("[S6-SYNC] npc=%u idx=%d kind=%u cl=(%d,%d,%d,%d) sv=(%d,%d,%d,%d) reg=%d/%d fix=%d doing=%d nan=%d t=%u", NpcSync->ID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, nMapX, nMapY, NpcSync->m_fkOffX, NpcSync->m_fkOffY, Npc[nIdx].m_RegionIndex, nRegion, Npc[nIdx].m_nNeedFixPos, (int)Npc[nIdx].m_Doing, (int)((Npc[nIdx].m_nNeedFixPos > 0 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion) ? 1 : 0), SubWorld[0].m_dwCurrentTime);
#endif
#ifndef _SERVER
		// [S10 26/08] Khoi doan dich S9 cu (r1/r2/r3) DA XOA - no doan dich tu vi tri
		// server cu 0,3-1s nen chinh no sinh 'quay dau' (do that: ban sao tu dao chieu
		// gap 6,1 lan server tren cung NPC). Tu nay DICH THAT do server gui: moi chang
		// qua broadcast s2c_npcrun/walk (KNpc::DoRun:2341), va ngay luc ADD qua mieng
		// [S10-M1] cuoi KNpc::SendSyncData. O day chi con LUOI AN TOAN HEP cho ban sao
		// DANG DUNG - dang dung thi khong co huong nen KHONG THE quay dau:
		// 1) [S10-KEO] ban sao dung ma server bao dang chay, lech >= 2 o (lenh chang bi
		//    nuot/mat - ngan sach broadcast 100 nguoi/luot trong dam dong): cho chay bu
		//    toi vi tri server. Can tren 12 o (xa hon la dich chuyen that - cam phat
		//    lenh chay xuyen ban do). Gac khe lenh: m_Command dang co gi thi NHUONG,
		//    de khong de len DICH THAT vua toi cung dot goi (ProcCommand tick sau moi
		//    thi hanh nen m_Doing chua kip doi - race mot-khe co that).
		// 2) [S10-SNAP] ca hai ben cung dung ma lech >= 2 o (diem dung cuoi bi lech do
		//    mat lenh chang cuoi - khong co nhanh nay thi lech ton tai VINH VIEN vi
		//    nhanh nan duoi chi chay khi m_nNeedFixPos>0): ghi de toa do theo dung
		//    khuon nhanh nan. Hai ben cung dung => vo hinh voi nguoi choi.
		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&
			Npc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion &&
			Npc[nIdx].m_Doing == do_stand)
		{
			int nS10X = 0, nS10Y = 0;
			Npc[nIdx].GetMpsPos(&nS10X, &nS10Y);
			int nS10D = g_GetDistance(nS10X, nS10Y, NpcSync->MapX, NpcSync->MapY);
			if (nS10D >= 64)
			{
				if ((NpcSync->Doing == do_run || NpcSync->Doing == do_walk) && nS10D < 32 * 12)
				{
					if (Npc[nIdx].GetCommand().CmdKind == do_none)
					{
						AUTOLOG("[S10-KEO] npc=%u idx=%d lech=%d doingsv=%d -> chay bu toi (%d,%d) t=%u", NpcSync->ID, nIdx, nS10D, (int)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY, SubWorld[0].m_dwCurrentTime);
						Npc[nIdx].SendCommand((NPCCMD)NpcSync->Doing, NpcSync->MapX, NpcSync->MapY);
					}
					else
					{
						AUTOLOG("[S10-GAC] npc=%u idx=%d lech=%d khe lenh co cmd=%d -> nhuong dich that t=%u", NpcSync->ID, nIdx, nS10D, (int)Npc[nIdx].GetCommand().CmdKind, SubWorld[0].m_dwCurrentTime);
					}
				}
				else if (NpcSync->Doing == do_stand)
				{
					AUTOLOG("[S10-SNAP] npc=%u idx=%d lech=%d hai ben cung dung -> nan toa do t=%u", NpcSync->ID, nIdx, nS10D, SubWorld[0].m_dwCurrentTime);
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
					Npc[nIdx].m_MapX = nMapX;
					Npc[nIdx].m_MapY = nMapY;
					Npc[nIdx].m_OffX = NpcSync->m_fkOffX;
					Npc[nIdx].m_OffY = NpcSync->m_fkOffY;
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
					Npc[nIdx].m_nNeedFixPos = 0;
				}
			}
		}
#endif
		if (Npc[nIdx].m_nNeedFixPos > 0 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if (Npc[nIdx].m_RegionIndex >= 0 && Npc[nIdx].m_RegionIndex == nRegion)
			{
				// Chi coi la "da nan" khi goi THUC SU doi vi tri. Goi ghi dung cho
				// dang dung = server xac nhan NPC dung yen tai do => GIU bo dem de
				// van chan >12 tick trong ServeMove duoc phep DoStand. Khong co
				// dieu kien nay, cho dong sync toi day hon 0,7s/lan se reset dem
				// lien tuc va NPC ket cung "dung yen ma chan van chay" vo han.
				if (Npc[nIdx].m_MapX != nMapX || Npc[nIdx].m_MapY != nMapY ||
					Npc[nIdx].m_OffX != NpcSync->m_fkOffX || Npc[nIdx].m_OffY != NpcSync->m_fkOffY)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
					Npc[nIdx].m_MapX = nMapX;
					Npc[nIdx].m_MapY = nMapY;
					Npc[nIdx].m_OffX = NpcSync->m_fkOffX;
					Npc[nIdx].m_OffY = NpcSync->m_fkOffY;
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
					Npc[nIdx].m_nNeedFixPos = 0;
				}
			}
			else
			{
				Npc[nIdx].m_nNeedFixPos = 0;	// khac region: khoi phia tren da doi vi tri that
			}
		}

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	// ∑«ÕÊº?
		{
			int	nOldLife = Npc[nIdx].m_CurrentLife;
			AUTOLOG_EVERY(1000, "SYNCMIN-LIFE npc=%u idx=%d life %d->%d max=%d cell=(%d,%d) reg=%d t=%u", NpcSync->ID, nIdx, nOldLife, NpcSync->m_CurrentLife, NpcSync->m_CurrentLifeMax, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
			Npc[nIdx].m_CurrentLife = NpcSync->m_CurrentLife;
			//if (Npc[nIdx].m_Kind == kind_normal)
			//{
			//	Npc[nIdx].SetBlood(nOldLife - Npc[nIdx].m_CurrentLife); // M∏u qu∏i b?gi∂m hi”n th?l™n Æ«u
			//}
			Npc[nIdx].m_Series				= NpcSync->m_bySeries;
		}
		Npc[nIdx].m_CurrentCamp			= NpcSync->Camp;
		//
		Npc[nIdx].m_FreezeState.nTime	= (NpcSync->State & STATE_FREEZE);
		Npc[nIdx].m_PoisonState.nTime	= (NpcSync->State & STATE_POISON);
		Npc[nIdx].m_StunState.nTime		= (NpcSync->State & STATE_STUN);
		Npc[nIdx].m_HideState.nTime		= (NpcSync->State & STATE_HIDE);
		//Npc[nIdx].m_MoveShadow.nTime	= (NpcSync->State & STATE_MOVE);
		Npc[nIdx].m_FrozenAction.nTime	= (NpcSync->State & STATE_FROZEN);
		//Npc[nIdx].m_ForbidAttack.nTime	= (NpcSync->State & STATE_FBDATK);
		Npc[nIdx].m_WalkRun.nTime		= NpcSync->State & STATE_WALKRUN;
		//
		Npc[nIdx].m_nProtectedTime = NpcSync->m_nProtectedTime;			//vong tron bat tu, vﬂng trﬂn b t t?
		Npc[nIdx].m_CurrentLife			= NpcSync->m_CurrentLife;
		Npc[nIdx].m_CurrentLifeMax	= NpcSync->m_CurrentLifeMax;
		Npc[nIdx].m_LifeMax				= NpcSync->m_LifeMax;
		Npc[nIdx].m_WalkSpeed			= NpcSync->m_WalkSpeed;
		Npc[nIdx].m_RunSpeed			= NpcSync->m_RunSpeed;
		Npc[nIdx].m_CurrentAttackSpeed	= NpcSync->m_ASpeed;
		Npc[nIdx].m_CurrentCastSpeed	= NpcSync->m_CSpeed;
		Npc[nIdx].m_ManaMax				= NpcSync->m_ManaMax;	//viet them sync mana teamMNG
		Npc[nIdx].m_CurrentMana				= NpcSync->m_CurrentMana;
		Npc[nIdx].m_CurrentManaMax				= NpcSync->m_CurrentManaMax;
		Npc[nIdx].m_nMissionGroup	= NpcSync->MissionGroup;//#NpcMissionGroup
		Npc[nIdx].SetNpcState(NpcSync->StateInfo);
		Npc[nIdx].m_Type					= NpcSync->NpcEnchant;
		Npc[nIdx].m_SyncSignal			= SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg) //Sync li™n tÙc ch?player x?l?khi vµo c∏c region < 0
{	
	NPC_PLAYER_TYPE_NORMAL_SYNC	*pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;
	
	_ASSERT(pSync->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);
	if (pSync->m_dwNpcID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID) //kh∏c b∂n th©n m◊nh th?kh´ng th˘c hi÷n
		return;

	int nRegion, nMapX, nMapY, nOffX, nOffY, nNpcIdx;
	SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
#ifndef _SERVER
	// [FIX-3 26/08] ghi lai vi tri may chu tin la cua minh (KHONG dong vao toa do client).
	g_nS9SvMeX = pSync->m_dwMapX;
	g_nS9SvMeY = pSync->m_dwMapY;
#endif
	AUTOLOG_EVERY(500, "SYNCME-DRIFT me idx=%d cl=(%d,%d) cloff=(%d,%d) reg=%d sv=(%d,%d) svoff=(%d,%d) reg=%d mps=(%d,%d) d=(%d,%d) doing=%d t=%u", Player[CLIENT_PLAYER_INDEX].m_nIndex, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffX, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_OffY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex, nMapX, nMapY, pSync->m_wOffX, pSync->m_wOffY, nRegion, pSync->m_dwMapX, pSync->m_dwMapY, (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX - nMapX), (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY - nMapY), (int)Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing, SubWorld[0].m_dwCurrentTime);

	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
#ifndef _SERVER
	// [S13c] khoang thoi gian giua hai goi tu-sync lien tiep (S13-KEO tinh buoc theo thoi gian that: vung dong
	// KRegion::Activate chi sync 5 NPC/nhip/region => chinh chu ~1 goi/2 s, vung vang 18 goi/s)
	static DWORD s_uS13LastSync = 0;
	static DWORD s_uS13DashEnd = 0;	// [S13e] timeGetTime lan cuoi client con o trang thai luot/nhay (cua an han nan cung)
	DWORD uS13Now = timeGetTime();
	int nS13Dt = (s_uS13LastSync == 0) ? 55 : (int)(uS13Now - s_uS13LastSync);
	s_uS13LastSync = uS13Now;
#endif
#ifndef _SERVER
	// [S6] Ba nhanh cua ham nay: regcu=-1 (vao lan dau) / svreg=-1 (region chua nap -> LoadMap)
	// / con lai = GIU NGUYEN toa do client, KHONG nan. Nhanh thu ba chinh la nghi can cua loi
	// "chet hoi sinh hoac phu ve thanh thi nhay vai toa do bay": may chu doi cho nhung neu
	// diem den nam trong region DA NAP thi client van giu vi tri cu.
	AUTOLOG("[S6-ME] nhanh=%s cl=(%d,%d,%d,%d) reg=%d sv=(%d,%d,%d,%d) reg=%d doing=%d t=%u", (Npc[nNpcIdx].m_RegionIndex == -1) ? "vaolandau" : ((nRegion == -1) ? "loadmap" : "GIUNGUYEN"), Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY, Npc[nNpcIdx].m_RegionIndex, nMapX, nMapY, pSync->m_wOffX, pSync->m_wOffY, nRegion, (int)Npc[nNpcIdx].m_Doing, SubWorld[0].m_dwCurrentTime);
#endif
#ifndef _SERVER
	// [S6 26/08] Bang tong 5 s/lan: tra loi "chet co xoa khong hay tich luy dan toi tran".
	// dung = khe dang chiem (tran 256) | mocoi = RegionIndex==-1 (khong duoc ve)
	// xac = doing==do_death | nguoi = kind_player (bot/nguoi) | quai = kind_normal.
	{
		static DWORD s_uS6BangT = 0;
		if (g_AutoLogOn() && (DWORD)(timeGetTime() - s_uS6BangT) >= 5000)
		{
			s_uS6BangT = timeGetTime();
			int nS6Dung = 0, nS6MoCoi = 0, nS6Xac = 0, nS6Nguoi = 0, nS6Quai = 0;
			for (int i6 = 1; i6 < MAX_NPC; i6++)
			{
				if (Npc[i6].m_dwID == 0)
					continue;
				nS6Dung++;
				if (Npc[i6].m_RegionIndex < 0)
					nS6MoCoi++;
				if (Npc[i6].m_Doing == do_death)
					nS6Xac++;
				if (Npc[i6].m_Kind == kind_player)
					nS6Nguoi++;
				if (Npc[i6].m_Kind == kind_normal)
					nS6Quai++;
			}
			AUTOLOG("[S6-BANG] dung=%d/%d mocoi=%d xac=%d nguoi=%d quai=%d boxa=%d t=%u", nS6Dung, (int)MAX_NPC, nS6MoCoi, nS6Xac, nS6Nguoi, nS6Quai, g_nS6BoXa, SubWorld[0].m_dwCurrentTime);
		}
	}
#endif

	AUTOLOG_EVERY(1000, "SYNCME-FIRSTREGION me idx=%d regcu=%d svreg=%d cell=(%d,%d) mps=(%d,%d) t=%u", nNpcIdx, Npc[nNpcIdx].m_RegionIndex, nRegion, nMapX, nMapY, pSync->m_dwMapX, pSync->m_dwMapY, SubWorld[0].m_dwCurrentTime);
	if (Npc[nNpcIdx].m_RegionIndex == -1)
	{
		if (nRegion < 0)
		{
			//_ASSERT(0);
			return;
		}
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
		SubWorld[0].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
#ifndef _SERVER
		// [S12b 28/08] Vua duoc DAT LAI tu mo coi = server vua dich chuyen minh (teleport/
		// vao map). Do that 27+28/08: lenh DAT-DI cua script toi CUNG MILI-GIAY voi cu sync
		// nay; cua so truoc day chi mo o nhanh S8-NAN (486ms sau) nen lenh bi vut -> thang
		// bung 8 o moi ~0,6s (11 cu / 5 cu). Mo cua so ngay tai day; gac echo giu nguyen.
		S13_ClearCmd(nNpcIdx);	// [S13] lenh dang giu la cua cho cu
		// [S13i 03/09] Dang di chuyen luc bi dat lai (phu ve/doi map): m_Doing=do_run + m_DesX/Y van la DICH CUA MAP CU
		// -> OnRun/ServeMove chay tiep toi dich cu trong map moi ~250 mps roi bi S8 nan lui (do phien S13e t=751604180:
		// +404 ms E4_MOVE_PATH des=(43493,104034) cua map 227 khi da o map 11, +692 ms S8-NAN 257). Dung lai tai cho
		// (DoStand la private -> SendCommand(do_stand): ProcCommand khung ke DoStand; lenh dat-di cua script toi sau
		// van vao khe di chuyen S13 va thi hanh ngay sau do_stand trong cung luot).
		if (Npc[nNpcIdx].m_Doing == do_run || Npc[nNpcIdx].m_Doing == do_walk || Npc[nNpcIdx].m_Doing == do_runattack)
		{
			// [S13j] dang LUOT (DoRunAttack tat AI): nhanh AI-tat cua ProcCommand vut do_stand -> bat AI de khung ke DoStand
			if (Npc[nNpcIdx].m_Doing == do_runattack)
				Npc[nNpcIdx].SetProcessAI(TRUE);
			Npc[nNpcIdx].SendCommand(do_stand, 0, 0, 0);
		}
		g_uS12CuaSoSelf = timeGetTime();
		AUTOLOG("[S12-CUA] mo cua so theo-lenh tai dat-lai sv=(%d,%d) t=%u", (int)pSync->m_dwMapX, (int)pSync->m_dwMapY, SubWorld[0].m_dwCurrentTime);
#endif
		return;
	}

	// `¯`´‘∂£¨≥¨≥ˆ9∆¡∑∂Œß
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);

		AUTOLOG_EVERY(1000, "SYNCME-LOADMAP me idx=%d cellcu=(%d,%d) offcu=(%d,%d) regcu=%d svmps=(%d,%d) t=%u", nNpcIdx, Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY, Npc[nNpcIdx].m_RegionIndex, pSync->m_dwMapX, pSync->m_dwMapY, SubWorld[0].m_dwCurrentTime);
		int nRegionX = pSync->m_dwMapX / (SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
		int nRegionY = pSync->m_dwMapY / (SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		S13_ClearCmd(nNpcIdx);	// [S13] lenh dang giu la cua cho cu
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);
		SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID);

		nRegion = SubWorld[0].FindRegion(dwRegionID);
		_ASSERT(nRegion >= 0);
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = dwRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);

		SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
#ifndef _SERVER
		// [S13i 03/09] nhu nhanh vaolandau: bi dat lai toi vung chua nap ma dang di chuyen -> dich cu vo nghia, dung lai.
		// [S13j] CUNG map nen LoadMap KHONG StopPath -> bo theo-duong (KSubWorld::Breathe) moi khung gui lai do_run theo
		// duong cu de len do_stand -> phai xoa duong o day; dang luot thi bat AI (xem nhanh vaolandau).
		SubWorld[0].StopPath();
		if (Npc[nNpcIdx].m_Doing == do_run || Npc[nNpcIdx].m_Doing == do_walk || Npc[nNpcIdx].m_Doing == do_runattack)
		{
			if (Npc[nNpcIdx].m_Doing == do_runattack)
				Npc[nNpcIdx].SetProcessAI(TRUE);
			Npc[nNpcIdx].SendCommand(do_stand, 0, 0, 0);
		}
#endif

		return;
	}

#ifndef _SERVER
	// [S8 26/08] NAN VI TRI BAN THAN KHI LECH QUA LON (chu game: "chay bay toa do khi
	// qua map phe kim bao danh" + "chet hoi sinh lai thi hay bi bo qua mua mau tai npc").
	//
	// Nhanh thu ba nay = CA HAI region deu da nap. Ban goc CO Y khong nan (du doan phia
	// client cho muot), va de xuat "nan moi lan sync" da bi phan bien BAC BO hoi 25/08 vi
	// gay giat rubber-band ~4 o moi goi. Nhung co mot ca ban goc khong luong: may chu
	// DICH CHUYEN nguoi choi TRONG CUNG MOT MAP - hoi sinh Tong Kim (SetTempRevPos ->
	// mobinhtk.lua), Xa Phu doi phe (battle_transprot -> xaphu.lua NewWorld cung map),
	// trap nem ra tran. Luc do KNpc::ChangeWorld thay cung map nen goi thang KNpc::SetPos
	// (KNpc.cpp:9962), ma SetPos CHI bao cho nguoi xung quanh (s2c_npcremove) chu KHONG
	// gui goi vi tri moi cho CHINH nguoi choi. Neu diem den lai nam trong khoi region da
	// nap thi ca hai nhanh tren deu khong chay -> client GIU NGUYEN cho cu vinh vien:
	//   . may auto tinh duong A* tu diem xuat phat sai  -> "chay bay toa do";
	//   . may Tong Kim do TK_TrongTrai() bang toa do cu  -> khong nhan ra minh dang o hau
	//     doanh nen bo qua buoc mua mau o Quan Y, di thang ra trap.
	//
	// Nguong 8 o (256 mps) lay bang WA_NHAY_XA cua CoreShell.cpp: nhip client 54 ms, chay
	// bo nhanh nhat khong qua noi 1 o, nen lech tu 8 o tro len CHI CO THE la bi dich
	// chuyen. Do that 25/08 (SYNCME-DRIFT): sai so du doan binh thuong p90 = 83 mps, tuc
	// nguong nay cao gap 3 lan dinh nhieu binh thuong -> khong dung toi duong chay muot.
	if (nRegion >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
	{
		int nMeX = 0, nMeY = 0;
		Npc[nNpcIdx].GetMpsPos(&nMeX, &nMeY);
		const int nLech = g_GetDistance(nMeX, nMeY, (int)pSync->m_dwMapX, (int)pSync->m_dwMapY);
		// [S13e] CUA AN HAN DASH: chieu luot (1977 Ham Son Kich, 2118, 995...) client tu thi hanh ngay, may chu giu roi thi
		// hanh tre 1-2 nhip => vai tram ms client di truoc 250-600 mps > nguong 256 => nan cung + StopPath = 'giut lui'.
		// Do 03/09 sau S13: 12/12 cu S8-NAN con lai deu trong/ngay sau dash. Dang luot hoac vua luot < 600 ms thi KHONG nan,
		// de may chu duoi kip (no cung dash toi cung diem); chi nan neu lech >= 512 (16 o) = dich chuyen that.
		const BOOL bS13Dash = (Npc[nNpcIdx].m_Doing == do_runattack || Npc[nNpcIdx].m_Doing == do_blurmove
			|| Npc[nNpcIdx].m_Doing == do_jump || Npc[nNpcIdx].m_Doing == do_jumpattack);
		if (bS13Dash)
			s_uS13DashEnd = uS13Now;
		const BOOL bS13AnHan = bS13Dash || (s_uS13DashEnd != 0 && (int)(uS13Now - s_uS13DashEnd) < 600);
		if (nLech >= 256 && bS13AnHan && nLech < 512)
		{
			AUTOLOG_EVERY(1000, "[S13-DASH-GRACE] lech=%d doing=%d saudash=%d ms -> khong nan, cho server duoi kip t=%u", nLech, (int)Npc[nNpcIdx].m_Doing, (int)(uS13Now - s_uS13DashEnd), SubWorld[0].m_dwCurrentTime);
		}
		else if (nLech >= 256)
		{
			// [S12-THEO 27/08] vua bi nan lon = nhieu kha nang server dang dieu khien minh
			// (teleport/dat di): mo cua so 3000ms nghe lenh run/walk cho chinh minh.
			g_uS12CuaSoSelf = timeGetTime();
			AUTOLOG("[S8-NAN] lech=%d cl=(%d,%d) sv=(%d,%d) cell cl=(%d,%d) sv=(%d,%d) regcu=%d regmoi=%d doing=%d t=%u", nLech, nMeX, nMeY, pSync->m_dwMapX, pSync->m_dwMapY, Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, nMapX, nMapY, Npc[nNpcIdx].m_RegionIndex, nRegion, (int)Npc[nNpcIdx].m_Doing, SubWorld[0].m_dwCurrentTime);
			SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
			if (nRegion != Npc[nNpcIdx].m_RegionIndex)
			{
				// doi region: dung dung ham cua engine (no tu RemoveNpc/AddNpc, dat lai
				// m_dwRegionID + m_RegionIndex, va nap map neu can) - hai tham so dau la
				// REGION ID chu khong phai chi so (KSubWorld.cpp:2386 ban client).
				SubWorld[0].NpcChangeRegion(Npc[nNpcIdx].m_dwRegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
			}
			Npc[nNpcIdx].m_MapX = nMapX;
			Npc[nNpcIdx].m_MapY = nMapY;
			Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
			Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
			if (Npc[nNpcIdx].m_RegionIndex >= 0)
				SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
			// bo noi suy dang dang (khong thi nhan vat truot mot duong dai tu cho cu sang)
			memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
			Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			// duong di dang chay duoc tinh tu diem xuat phat cu -> huy de tinh lai
			S13_ClearCmd(nNpcIdx);
			SubWorld[0].StopPath();
			return;
		}
		// [S13e] CHI keo khi DUNG YEN: luc chay, phan lech chu yeu la dan truoc lanh + tre khoi dong chang, keo luc do lam hut
		// toc do theo dot ('chay nhanh roi cham'); luc danh thi keo lam nhan vat truot va may danh (nguong 75 mps) roi ngoai
		// tam roi chay lai. Dung yen thi hai ben cung dung => lech = lech THAT, keo ve la dung va it lo.
		else if (nLech > S13_VUNGCHET && Npc[nNpcIdx].m_Doing == do_stand)
		{
			// [S13-KEO 03/09] HOA GIAI MEM: phan lech vuot vung chet keo ve phia may chu 1/S13_CHIA.
			// Buoc <= (255-64)/8 = 24 mps < PAINT_INTERP_SNAP_DIST 64 => lop noi suy ve keo muot, mat
			// khong thay giat. KHONG StopPath, KHONG dung m_DesX/Y: duong dang chay tiep tuc tu vi tri
			// moi. Bo qua luc dang luot/nhay/bi danh/chet (client hop phap di truoc vai tram mps trong
			// ~0,4 s; nan cung 256 o tren van la lan an toan).
			// [S13c] phan bien #4: buoc theo THOI GIAN THAT giua hai goi (tau ~450 ms => 55 ms ~ 1/8 phan doi),
			// kep [2, 40] mps: chua 24 mps cho buoc chay cung nhip de tong dich chuyen van <= PAINT_INTERP_SNAP_DIST 64.
			if (nS13Dt < 1)
				nS13Dt = 1;
			if (nS13Dt > 3000)
				nS13Dt = 3000;
			int nS13Buoc = (int)(((__int64)(nLech - S13_VUNGCHET)) * nS13Dt / 450);
			if (nS13Buoc < 2)
				nS13Buoc = 2;
			if (nS13Buoc > 40)
				nS13Buoc = 40;	// [S13d] 64 + buoc chay <=31 vuot nguong snap noi suy -> giat 2-3 o o vung dong
			int nS13X = nMeX + (int)(((__int64)((int)pSync->m_dwMapX - nMeX)) * nS13Buoc / nLech);
			int nS13Y = nMeY + (int)(((__int64)((int)pSync->m_dwMapY - nMeY)) * nS13Buoc / nLech);
			int nS13R = -1, nS13MX = 0, nS13MY = 0, nS13OX = 0, nS13OY = 0;
			SubWorld[0].Mps2Map(nS13X, nS13Y, &nS13R, &nS13MX, &nS13MY, &nS13OX, &nS13OY);
			// [S13c] phan bien #5: diem keo trung gian roi vao o vat can (cat goc tuong) -> bo cu keo nay, cho goi sau
			// [S13d] phan bien vong 2: KRegion::GetBarrier ban client KHONG doc vat can dia hinh (chi bao o co NPC khi
			// g_nPbNpcChan); dia hinh client nam o g_ScenePlace, doc qua SubWorld::TestBarrier(mps) - dung ham GetDir dung.
			BYTE byS13B = (nS13R >= 0) ? SubWorld[0].TestBarrier(nS13X, nS13Y) : (BYTE)0xFF;
			if (nS13R >= 0 && (byS13B == 0 || byS13B == Obstacle_JumpFly))
			{
				SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
				if (nS13R != Npc[nNpcIdx].m_RegionIndex)
					SubWorld[0].NpcChangeRegion(Npc[nNpcIdx].m_dwRegionID, SubWorld[0].m_Region[nS13R].m_RegionID, nNpcIdx);
				Npc[nNpcIdx].m_MapX = nS13MX;
				Npc[nNpcIdx].m_MapY = nS13MY;
				Npc[nNpcIdx].m_OffX = nS13OX;
				Npc[nNpcIdx].m_OffY = nS13OY;
				if (Npc[nNpcIdx].m_RegionIndex >= 0)
					SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
				g_nS13KeoCount++;
				AUTOLOG_EVERY(1000, "[S13-KEO] lech=%d buoc=%d dt=%d doing=%d dem=%d t=%u", nLech, nS13Buoc, nS13Dt, (int)Npc[nNpcIdx].m_Doing, g_nS13KeoCount, SubWorld[0].m_dwCurrentTime);
			}
		}
	}
#endif
	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);
	AUTOLOG_EVERY(1000, "SYNCME-BARRIER me idx=%d barrier=%d cell=(%d,%d) off=(%d,%d) reg=%d svcell=(%d,%d) t=%u", nNpcIdx, (int)byBarrier, Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY, Npc[nNpcIdx].m_RegionIndex, nMapX, nMapY, SubWorld[0].m_dwCurrentTime);
	if (0 != byBarrier && Obstacle_JumpFly != byBarrier)
	{
		g_DebugLog("[Barrier]Player in Barrier");
	}
/*	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CountItemInAll() != pSync->m_nEquipCount) {
		//reconnect
		g_DebugLog("Player Equip Count Error");
		C2SPLAYER_AI_BACKTOTOWN AutoCmd;
		AutoCmd.ProtocolType = c2s_aibacktotown;
		AutoCmd.nIdSubWorld = 0;
		AutoCmd.dwID = Player[CLIENT_PLAYER_INDEX].GetPlayerID();
		AutoCmd.dwTimePacker = GetTickCount();
		if (g_pClient)
			g_pClient->SendPackToServer((BYTE*)&AutoCmd, sizeof(C2SPLAYER_AI_BACKTOTOWN));

	}*/
}

void KProtocolProcess::SyncObjectAdd(BYTE* pMsg)
{
	OBJ_ADD_SYNC	*pObjSyncAdd = (OBJ_ADD_SYNC*)pMsg;
	int				nObjIndex;
	KObjItemInfo	sInfo;

	nObjIndex = ObjSet.FindID(pObjSyncAdd->m_nID);
	AUTOLOG_EVERY(1000, "OBJADD-RECV id=%d dataid=%d itemid=%d genre=%d dtype=%d ptype=%d x=%d y=%d money=%d w=%d h=%d flag=%d dupidx=%d t=%u", pObjSyncAdd->m_nID, pObjSyncAdd->m_nDataID, pObjSyncAdd->m_nItemID, pObjSyncAdd->m_nGenre, pObjSyncAdd->m_nDetailType, pObjSyncAdd->m_nParticularType, pObjSyncAdd->m_nXpos, pObjSyncAdd->m_nYpos, pObjSyncAdd->m_nMoneyNum, (int)pObjSyncAdd->m_btItemWidth, (int)pObjSyncAdd->m_btItemHeight, (int)pObjSyncAdd->m_btFlag, nObjIndex, GetTickCount());
	if (nObjIndex > 0)
		return;

	sInfo.m_nItemID = pObjSyncAdd->m_nItemID;
	sInfo.m_nItemWidth = pObjSyncAdd->m_btItemWidth;
	sInfo.m_nItemHeight = pObjSyncAdd->m_btItemHeight;
	AUTOLOG_EVERY(1000, "OBJ-ADD-RX obj=%d data=%d item=%d mps=(%d,%d) tien=%d npc=%d cu_idx=%d me_cell=(%d,%d) me_reg=%d t=%u", pObjSyncAdd->m_nID, pObjSyncAdd->m_nDataID, pObjSyncAdd->m_nItemID, pObjSyncAdd->m_nXpos, pObjSyncAdd->m_nYpos, pObjSyncAdd->m_nMoneyNum, pObjSyncAdd->m_dwNpcId, nObjIndex, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapX, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapY, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex, SubWorld[0].m_dwCurrentTime);
	sInfo.m_nMoneyNum = pObjSyncAdd->m_nMoneyNum;
	sInfo.m_nColorID = pObjSyncAdd->m_btColorID;
	sInfo.m_nGenre = pObjSyncAdd->m_nGenre;
	sInfo.m_nDetailType = pObjSyncAdd->m_nDetailType;
	sInfo.m_nParticularType = pObjSyncAdd->m_nParticularType;
	sInfo.m_dwNpcId1 = pObjSyncAdd->m_dwNpcId;
	sInfo.m_nMovieFlag = ((pObjSyncAdd->m_btFlag & 0x02) > 0 ? 1 : 0);
	sInfo.m_nSoundFlag = ((pObjSyncAdd->m_btFlag & 0x01) > 0 ? 1 : 0);
	memset(sInfo.m_szName, 0, sizeof(sInfo.m_szName));
	BIEN_ChepAnToan(sInfo.m_szName, (int)sizeof(sInfo.m_szName), pObjSyncAdd->m_szName, (int)(pObjSyncAdd->m_wLength + 1 + sizeof(pObjSyncAdd->m_szName) - sizeof(OBJ_ADD_SYNC)), "sInfo.m_szName");

	nObjIndex = ObjSet.ClientAdd(
		pObjSyncAdd->m_nID,
		pObjSyncAdd->m_nDataID,
		pObjSyncAdd->m_btState,
		pObjSyncAdd->m_btDir,
		pObjSyncAdd->m_wCurFrame,
		pObjSyncAdd->m_nXpos,
		pObjSyncAdd->m_nYpos,
		sInfo);
		AUTOLOG_EVERY(1000, "OBJADD-RESULT id=%d idx=%d itemid=%d x=%d y=%d", pObjSyncAdd->m_nID, nObjIndex, pObjSyncAdd->m_nItemID, pObjSyncAdd->m_nXpos, pObjSyncAdd->m_nYpos);
#ifdef WAIGUA_ZROC
	if (nObjIndex <= 0)
		return;
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
//		sPickUp.ProtocolType = c2s_playerpickupitem;
//		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
//		sPickUp.m_btPosType = 0;
//		sPickUp.m_btPosX = 0;
//		sPickUp.m_btPosY = 0;
//		if (g_pClient)
//			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
	else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
	{
		ItemPos	sItemPos;
		if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
			return;
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;
		sPickUp.m_btPosY = sItemPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
#endif
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢∏¸–¬ƒ≥∏ˆobj∑ΩœÚ
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectDir(BYTE* pMsg)
{
	OBJ_SYNC_DIR	*pObjSyncDir = (OBJ_SYNC_DIR*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncDir->m_nID);
	if (nObjIndex <= 0)
	{
		// œÚ∑˛ŒÒ∆˜∑¢ÃÌº”«Î«Û
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncDir->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// Õ¨≤Ω∑ΩœÚ
		Object[nObjIndex].SetDir(pObjSyncDir->m_btDir);
	}
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢…æ≥˝ƒ≥∏ˆobj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectRemove(BYTE* pMsg)
{
	OBJ_SYNC_REMOVE	*pObjSyncRemove = (OBJ_SYNC_REMOVE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncRemove->m_nID);
	AUTOLOG_EVERY(1000, "OBJREM-RECV id=%d idx=%d snd=%d t=%u", pObjSyncRemove->m_nID, nObjIndex, (int)pObjSyncRemove->m_btSoundFlag, GetTickCount());
	if (nObjIndex > 0)
	{	
		Object[nObjIndex].Remove(pObjSyncRemove->m_btSoundFlag);
	}
}

void KProtocolProcess::SyncObjectState(BYTE* pMsg)
{
	OBJ_SYNC_STATE	*pObjSyncState = (OBJ_SYNC_STATE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncState->m_nID);
	AUTOLOG_EVERY(1000, "OBJSTATE-MISS id=%d state=%d req=1 t=%u", pObjSyncState->m_nID, (int)pObjSyncState->m_btState, GetTickCount());
	if (nObjIndex <= 0)
	{
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncState->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	
		if (Object[nObjIndex].m_nRegionIdx == -1)
		{
			int	nRegion;
			nRegion = SubWorld[0].FindRegion(Object[nObjIndex].m_nBelongRegion);
			if (nRegion >= 0)
			{
				Object[nObjIndex].m_nRegionIdx = nRegion;
				SubWorld[0].m_Region[nRegion].AddObj(nObjIndex);
			}
		}
		Object[nObjIndex].SetState(pObjSyncState->m_btState);
	}
}

void KProtocolProcess::SyncObjectTrap(BYTE* pMsg)
{
	OBJ_SYNC_TRAP_ACT *pObjTrapSync = (OBJ_SYNC_TRAP_ACT*)pMsg;
	int		nObjIndex;
	nObjIndex = ObjSet.FindID(pObjTrapSync->m_nID);
	if (nObjIndex <= 0)
	{
		// œÚ∑˛ŒÒ∆˜∑¢ÃÌº”«Î«Û
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjTrapSync->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{
		Object[nObjIndex].m_nState = OBJ_TRAP_STATE_ACTING;
		Object[nObjIndex].m_cImage.SetDirStart();
		Object[nObjIndex].m_cSkill.m_nTarX = pObjTrapSync->m_nTarX;
		Object[nObjIndex].m_cSkill.m_nTarY = pObjTrapSync->m_nTarY;
	}
}

void KProtocolProcess::SyncPlayer(BYTE* pMsg) //sync player 1 l«n Æ«u ti™n
{
	PLAYER_SYNC*	pPlaySync = (PLAYER_SYNC *)pMsg;
	AUTOLOG_EVERY(1000, "SYNCPLAYER-NOIDX player=%u idx=%d t=%u", pPlaySync->ID, NpcSet.SearchID(pPlaySync->ID), SubWorld[0].m_dwCurrentTime);

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	// Chan ghi de o sentinel Npc[0]: NpcSet.SearchID tra 0 khi CHUA co NPC nay o client
	// (goi player-sync toi truoc goi tao NPC), hoac khi het khe - MAX_NPC o client chi 256
	// (KNpc.h:23) trong khi server la 98000. Khong chan thi moi goi nhu vay ghi thang vao
	// Npc[0] va con dat Npc[0].m_Kind = kind_player.
	if (nIdx <= 0)
		return;

	// 0x20 = co nhan dien bot SimCity do server dat (KNpc.cpp, hai ham sync).
	// Bit nay truoc day luon 0 nen client cu bo qua -> tuong thich nguoc.
	// Dung de loc bot khoi cac danh sach "nguoi choi quanh day" cua WAuto.
	Npc[nIdx].m_btSimCityBot = (pPlaySync->m_btSomeFlag & 0x20) ? 1 : 0;

	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed; 
	Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed; 
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	if(Option.GetLow(LowPlayer) /*&& nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex*/)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType	= pPlaySync->MaskType;		//#mat na
	Npc[nIdx].m_MantleType		= pPlaySync->MantleType;				//#phi phong
	Npc[nIdx].m_byMantleLevel	= pPlaySync->MantleLevel;
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	if (Npc[nIdx].m_HorseType >= 0)		//edit by phong kieu len xuong ngua
    {
        Npc[nIdx].m_bRideHorse = TRUE;
    }
    else
    {
        Npc[nIdx].m_bRideHorse = FALSE;
    }
	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{
		SendClientCmdShopName(Npc[nIdx].m_dwID);
	}
	Npc[nIdx].m_BaiTan				= pPlaySync->m_bBaiTan;
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;	
#ifndef _SERVER
	REP3_NpcTheSame(nIdx);	// [REP3 03/09]
#endif
	Npc[nIdx].m_Kind					= kind_player;
	Npc[nIdx].m_btRankId				= pPlaySync->RankID;
	Npc[nIdx].m_btRankBattleId			= pPlaySync->RankBattleID;//#RankBattle
	Npc[nIdx].m_btPlayerTitle			= pPlaySync->PlayerTitle;//#PlayerTitle
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);//#MateName
	strcpy(Npc[nIdx].m_szGameTitle, pPlaySync->GameTitle); //GameTitle
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		Player[CLIENT_PLAYER_INDEX].m_CUnlocked			= pPlaySync->CUnlocked;
	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID; // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID; // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld; //x’p hπng th’ giÌi
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	Npc[nIdx].nFirstFaction			= pPlaySync->nFirstFaction;
	//
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= enumPKNormal;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= enumPKWar;
	else 
		Npc[nIdx].m_nPKFlag			= enumPKMurder;
	
	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= enumPKWar;
	else
		Npc[nIdx].m_FightMode		= enumPKNormal;

	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);
	//
	strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
	strcpy(Npc[nIdx].m_szTongTitle, pPlaySync->TongTitle);
	
	Npc[nIdx].m_nFigure						= pPlaySync->TongFigure;
	Npc[nIdx].m_Recruit					= pPlaySync->TongRecruit;
	Npc[nIdx].m_ImagePlayer			= pPlaySync->ImagePlayer;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
		Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pPlaySync->bMeridianLevel);
		Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	}
}

void KProtocolProcess::SyncPlayerMin(BYTE* pMsg) //Sync Player li™n tÙc
{
	PLAYER_NORMAL_SYNC* pPlaySync = (PLAYER_NORMAL_SYNC *)pMsg;
	AUTOLOG_EVERY(2000, "SYNCPLAYERMIN-NOIDX player=%u idx=%d t=%u", pPlaySync->ID, NpcSet.SearchID(pPlaySync->ID), SubWorld[0].m_dwCurrentTime);
	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	// Chan ghi de o sentinel Npc[0]: NpcSet.SearchID tra 0 khi CHUA co NPC nay o client
	// (goi player-sync toi truoc goi tao NPC), hoac khi het khe - MAX_NPC o client chi 256
	// (KNpc.h:23) trong khi server la 98000. Khong chan thi moi goi nhu vay ghi thang vao
	// Npc[0] va con dat Npc[0].m_Kind = kind_player.
	if (nIdx <= 0)
		return;

	// 0x20 = co nhan dien bot SimCity do server dat (KNpc.cpp, hai ham sync).
	// Bit nay truoc day luon 0 nen client cu bo qua -> tuong thich nguoc.
	// Dung de loc bot khoi cac danh sach "nguoi choi quanh day" cua WAuto.
	Npc[nIdx].m_btSimCityBot = (pPlaySync->m_btSomeFlag & 0x20) ? 1 : 0;

	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed; 
	Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;
	Npc[nIdx].m_MantleType			= pPlaySync->MantleType;			//#phi phong
	Npc[nIdx].m_byMantleLevel		= pPlaySync->MantleLevel;
	Npc[nIdx].m_btHonorId			= pPlaySync->HonorID;				//#HonorID
	if(Option.GetLow(LowPlayer) /*&& nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex*/)
	{
		if(Npc[nIdx].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
			Npc[nIdx].m_MaskType = ManTypeNameIdx;
		else
			Npc[nIdx].m_MaskType = WomanTypeNameIdx;
	}
	else
		Npc[nIdx].m_MaskType			= pPlaySync->MaskType;				//#mat na
	Npc[nIdx].m_HorseType			= (char)pPlaySync->HorseType;
	if (Npc[nIdx].m_HorseType >= 0)		//edit by phong kieu len xuong ngua
    {
        Npc[nIdx].m_bRideHorse = TRUE;
    }
    else
    {
        Npc[nIdx].m_bRideHorse = FALSE;
    }
#ifndef _SERVER
	REP3_NpcTheSame(nIdx);	// [REP3 03/09]
#endif
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_btRankId			= pPlaySync->RankID;
	Npc[nIdx].m_btRankBattleId			= pPlaySync->RankBattleID;//#RankBattle
	Npc[nIdx].m_btPlayerTitle			= pPlaySync->PlayerTitle;//#PlayerTitle
	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{
		SendClientCmdShopName(Npc[nIdx].m_dwID);
	}
	Npc[nIdx].m_BaiTan				= pPlaySync->m_bBaiTan;
	strcpy(Npc[nIdx].MateName, pPlaySync->MateName);//#MateName
	strcpy(Npc[nIdx].m_szGameTitle, pPlaySync->GameTitle); //GameTitle
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		Player[CLIENT_PLAYER_INDEX].m_CUnlocked			= pPlaySync->CUnlocked;
	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID; // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID; // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld; //x’p hπng th’ giÌi
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nFuYuan				= pPlaySync->FuYuan;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	Npc[nIdx].nFirstFaction			= pPlaySync->nFirstFaction;
	//
	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= enumPKNormal;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= enumPKWar;
	else 
		Npc[nIdx].m_nPKFlag			= enumPKMurder;

	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= enumPKWar;
	else
		Npc[nIdx].m_FightMode		= enumPKNormal;

	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);

	AUTOLOG_EVERY(1000, "ME-FIGHTMODE me idx=%d flag=0x%02X fight=%d pk=%d ngua=%d aspd=%d cspd=%d t=%u", nIdx, (int)pPlaySync->m_btSomeFlag, (int)Npc[nIdx].m_FightMode, (int)Npc[nIdx].m_nPKFlag, (int)Npc[nIdx].m_bRideHorse, pPlaySync->AttackSpeed, pPlaySync->CastSpeed, SubWorld[0].m_dwCurrentTime);
	if (pPlaySync->m_btSomeFlag & 0x08)
		Npc[nIdx].m_nTongFlag		= 1;
	else
		Npc[nIdx].m_nTongFlag		= 0;
	//

	strcpy(Npc[nIdx].m_szTongName, pPlaySync->TongName);
	strcpy(Npc[nIdx].m_szTongTitle, pPlaySync->TongTitle);
	Npc[nIdx].m_nFigure						= pPlaySync->TongFigure;
	Npc[nIdx].m_Recruit						= pPlaySync->TongRecruit;
	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Player[CLIENT_PLAYER_INDEX].m_ImagePlayer		= pPlaySync->ImagePlayer;
		if(Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pPlaySync->bMeridianLevel))
			Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	}

	//Npc[nIdx].m_CurrentWalkSpeed = pPlaySync->WalkSpeed;
	//Npc[nIdx].m_CurrentRunSpeed = pPlaySync->RunSpeed;
}

void KProtocolProcess::SyncScriptAction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pMsg);
}

void KProtocolProcess::SyncWorld(BYTE* pMsg)
{
	WORLD_SYNC *WorldSync = (WORLD_SYNC *)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);

	if (SubWorld[0].m_SubWorldID != WorldSync->SubWorld)
	{
		SubWorld[0].LoadMap(WorldSync->SubWorld, WorldSync->Region);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nObjectIdx = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetProcessAI(TRUE);
		g_uS12CuaSoSelf = 0;	// [S12-THEO] cua so khong duoc song sot qua doi map (lenh ton dong se thi hanh muon)
/*
		if(!g_JXPathFinder.Init(g_ScenePlace.GetLittleMap()->GetRect(), g_ScenePlace.GetKScenePlaceMapC()))
		{
			g_DebugLog("[KProtocolProcess::SyncWorld] g_JXPathFinder.Init failed!");
		}
		else
			g_DebugLog("[KProtocolProcess::SyncWorld] g_JXPathFinder.Init successful!");*/
	}

	if (SubWorld[0].m_dwCurrentTime > WorldSync->Frame)
	{
		// øÕªß∂ÀøÏ¡À£¨ºıÀŸ¥¶¿Ì£¨ªÚ’ﬂ «Lag“˝∆µƒæ…œ˚œ¢£¨–Ë“™¥¶¿Ì
	}
	else if (SubWorld[0].m_dwCurrentTime < WorldSync->Frame)
	{
		// øÕªß∂À¬˝¡À£¨º”ÀŸ¥¶¿Ì
	}
	
	SubWorld[0].m_dwCurrentTime = WorldSync->Frame;
	
	
	// Õ¨≤ΩÃÏ∆¯
	SubWorld[0].m_nWeather = WorldSync->Weather;
	g_ScenePlace.ChangeWeather(WorldSync->Weather);
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜œ˚œ¢ƒ≥ÕÊº“÷’÷π¡À”Î±æøÕªß∂Àµƒ∫√”—πÿœµ
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatDeleteFriend(BYTE* pMsg)
{
	CHAT_DELETE_FRIEND_SYNC	*pDelete = (CHAT_DELETE_FRIEND_SYNC*)pMsg;

	CChatFriend	*pFriend;
	for (int i = 0; i < MAX_FRIEND_TEAM; i++)
	{
		pFriend = (CChatFriend*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == pDelete->m_dwID)
			{
				pFriend->Remove();
				Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_nFriendNo--;

				// Õ®÷™ΩÁ√Ê ƒ≥ÕÊº“÷’÷π¡À”Î±æøÕªß∂Àµƒ∫√”—πÿœµ
//				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
				CoreDataChanged(GDCNI_CHAT_FRIEND, 0, i);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_DELETED_FRIEND, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

				delete pFriend;
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ ’µΩ∑˛ŒÒ∆˜Õ®÷™ƒ≥¡ƒÃÏ∫√”—œ¬œﬂ
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatFriendOffLine(BYTE* pMsg)
{
	CHAT_FRIEND_OFFLINE_SYNC	*pSync = (CHAT_FRIEND_OFFLINE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cChat.GetMsgOffLine(pSync->m_dwID);
}

void	KProtocolProcess::s2cSyncAllSkill(BYTE * pMsg)
{
	SKILL_SEND_ALL_SYNC	* pSync = (SKILL_SEND_ALL_SYNC*) pMsg;
	int nSkillCount = (pSync->m_wProtocolLong - 2) / sizeof(SKILL_SEND_ALL_SYNC_DATA);
	int nNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KSkill * pOrdinSkill = NULL;
	Npc[nNpcIndex].m_SkillList.Clear();
	for (int i = 0; i < nSkillCount; i ++)
	{
		if (pSync->m_sAllSkill[i].SkillId)
		{
			
//			pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSync->m_sAllSkill[i].SkillId, pSync->m_sAllSkill[i].SkillLevel);
//			if (!pOrdinSkill) 
//              continue;

			Npc[nNpcIndex].m_SkillList.Add(
				pSync->m_sAllSkill[i].SkillId,
				pSync->m_sAllSkill[i].SkillLevel,
				pSync->m_sAllSkill[i].SkillExp
            );
		}
	}

}

void KProtocolProcess::SyncEnd(BYTE* pMsg)
{
	BYTE	SyncEnd = (BYTE)c2s_syncend;
	if (g_pClient)
		g_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));
	Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();
	g_DebugLog("[TRACE]SyncEnd to Server");
	CoreDataChanged(GDCNI_GAME_START, 0, 0);	
//	g_bPingReply = TRUE;
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTeamName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTakeChannelInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_LOGIN);

	g_SubWorldSet.m_cMusic.Start(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

	Player[CLIENT_PLAYER_INDEX].m_cPK.Init();

}

void	KProtocolProcess::s2cTradeChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeChangeState(pMsg);
}

void	KProtocolProcess::s2cGambleChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleChangeState(pMsg);
}

void	KProtocolProcess::s2cNpcSetMenuState(BYTE* pMsg)
{
	NPC_SET_MENU_STATE_SYNC	*pState = (NPC_SET_MENU_STATE_SYNC*)pMsg;
	int		nNpc;
	if (pState->m_dwID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (pState->m_btState == PLAYER_MENU_STATE_TRADEOPEN)
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_STATE_OPEN);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (pState->m_btState == PLAYER_MENU_STATE_NORMAL)
		{
			if (Npc[nNpc].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_TRADE_STATE_CLOSE);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
	}
	else
	{
		nNpc = NpcSet.SearchID(pState->m_dwID);
		if (nNpc == 0)
			return;
	}
	if (pState->m_wLength > sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence))
		Npc[nNpc].SetMenuState(pState->m_btState, pState->m_szSentence, pState->m_wLength - (sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence)));
	else
		Npc[nNpc].SetMenuState(pState->m_btState);
}

void	KProtocolProcess::s2cTradeMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeMoneySync(pMsg);
}

void	KProtocolProcess::s2cTradeDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeDecision(pMsg);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void	KProtocolProcess::s2cGambleMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleMoneySync(pMsg);
}

void	KProtocolProcess::s2cGambleDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleDecision(pMsg);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

void	KProtocolProcess::s2cTeamInviteAdd(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReceiveInvite((TEAM_INVITE_ADD_SYNC*)pMsg);
}

void	KProtocolProcess::s2cTradePressOkSync(BYTE * pMsg)
{
	TRADE_STATE_SYNC	*pPress = (TRADE_STATE_SYNC*)pMsg;
	if(!pPress)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetTradeState(pPress->m_btSelfLock, pPress->m_btDestLock, pPress->m_btSelfOk, pPress->m_btDestOk);
}

void	KProtocolProcess::s2cGamblePressOkSync(BYTE* pMsg)
{
	GAMBLE_STATE_SYNC* pPress = (GAMBLE_STATE_SYNC*)pMsg;
	if (!pPress)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetGambleState(pPress->m_btSelfLock, pPress->m_btDestLock, pPress->m_btSelfOk, pPress->m_btDestOk);
}

void	KProtocolProcess::s2cDirectlyCastSkill(BYTE * pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	
	//µ±÷∏∂®ƒ≥∏ˆƒø±Í ±(MapX == -1),MapYŒ™ƒø±ÍµƒNpcdwID£¨–Ë“™◊™ªª≥…±æµÿµƒNpcIndex≤≈––
	int nVhtdTargetIdx = 0;	// [VHTD 02/09g] chi so NPC muc tieu (MapX == -1): truyen thang cho Cast nhu server -> dan Follow (1363) bam muc tieu
	if (MapX == -1)
	{
		if (MapY < 0 ) return;
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)	return;
		if (Npc[MapY].m_RegionIndex < 0)
			return;
		nVhtdTargetIdx = (int)MapY;
		int nX, nY;
		nX = Npc[MapY].m_MapX;
		nY = Npc[MapY].m_MapY;
		SubWorld[0].Map2Mps(Npc[MapY].m_RegionIndex, nX, nY, 0, 0, (int *)&MapX, (int *)&MapY);
	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	if ((nSkillID >= 1363 && nSkillID <= 1384) || (nSkillID >= 1965 && nSkillID <= 1991) || (nSkillID >= 2114 && nSkillID <= 2143))	// [VHTD 02/09k] log nhan lenh phong ky nang 3 phai (client)
		AUTOLOG("[VH-CL-CAST-DIRECT] npc=%u idx=%d skill=%d lv=%d mps=(%d,%d) tgt=%d me=%d t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, (int)MapX, (int)MapY, nVhtdTargetIdx, Player[CLIENT_PLAYER_INDEX].m_nIndex, SubWorld[0].m_dwCurrentTime);
	
	//_ASSERT (nSkillID > 0 && nSkillLevel > 0);
	AUTOLOG_EVERY(1000, "CAST-RX npc=%u idx=%d skill=%d lv=%d mps=(%d,%d) me_idx=%d t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, (int)MapX, (int)MapY, Player[CLIENT_PLAYER_INDEX].m_nIndex, SubWorld[0].m_dwCurrentTime);
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillID, nSkillLevel);
	AUTOLOG_EVERY(1000, "CAST-NOSKILL npc=%u idx=%d skill=%d lv=%d mps=(%d,%d) t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
	if (!pOrdinSkill) 
        return ;
	
    AUTOLOG_EVERY(1000, "CAST-DO npc=%u idx=%d skill=%d lv=%d style=%d aura=%d mps=(%d,%d) t=%u", dwNpcID, nIdx, nSkillID, nSkillLevel, (int)pOrdinSkill->GetSkillStyle(), (int)pOrdinSkill->IsAura(), (int)MapX, (int)MapY, SubWorld[0].m_dwCurrentTime);
    if (nVhtdTargetIdx > 0)
        pOrdinSkill->Cast(nIdx, -1, nVhtdTargetIdx);	// [VHTD 02/09g] nhu KNpc::Cast(int,int)/CastAutoSkillAt tren server (KSkill::Cast nhan -1 + chi so)
    else
        pOrdinSkill->Cast(nIdx, MapX, MapY);

	if(!pOrdinSkill->IsAura())
	{
		DWORD dwCastTime = 0;
		eSkillStyle eStyle = (eSkillStyle)pOrdinSkill->GetSkillStyle();
		if (eStyle == SKILL_SS_Missles 
			|| eStyle == SKILL_SS_Melee 
			|| eStyle == SKILL_SS_InitiativeNpcState 
			|| eStyle == SKILL_SS_PassivityNpcState)
		{
			dwCastTime = pOrdinSkill->GetDelayPerCast(Npc[nIdx].m_bRideHorse);
		}
		else
		{
			switch(eStyle)
			{
			case SKILL_SS_Thief:
				{
					dwCastTime = ((KThiefSkill*)pOrdinSkill)->GetDelayPerCast();
				}break;
			}
		}
		Npc[nIdx].m_SkillList.SetNextCastTime(nSkillID, SubWorld[0].m_dwCurrentTime, SubWorld[0].m_dwCurrentTime + dwCastTime);
	}
}

void	KProtocolProcess::s2cShowMsg(BYTE *pMsg)
{
	if (!pMsg)
		return;
	SHOW_MSG_SYNC	*pShowMsg = (SHOW_MSG_SYNC*)pMsg;

	switch (pShowMsg->m_wMsgID)
	{
	case enumMSG_ID_TEAM_KICK_One:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");
			if (strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName) == 0)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_BE_KICKEN);
				sMsg.eType = SMT_TEAM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_KICK_ONE, szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
			}
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_DISMISS:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_CAPTAIN);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_MEMBER, g_Team[0].m_szMemName[0]);
			}
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_LEAVE:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == dwID)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE_SELF_MSG, g_Team[0].m_szMemName[0]);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, "”–»À");
				for (int i = 0; i < MAX_TEAM_MEMBER; i++)
				{
					if ((DWORD)g_Team[0].m_nMember[i] == dwID)
					{
						sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, g_Team[0].m_szMemName[i + 1]);
						break;
					}
				}
			}
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_REFUSE_INVITE:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), &pShowMsg->m_lpBuf, (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_REFUSE_INVITE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_SELF_ADD:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SELF_ADD, g_Team[0].m_szMemName[0]);
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_UI_TEAM;
			sMsg.byPriority = 3;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL2, g_Team[0].m_szMemName[nMember + 1]);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_OBJ_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_TOO_FAR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strcpy(sMsg.szMessage, MSG_OBJ_TOO_FAR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEC_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEC_MONEY, (int)pShowMsg->m_lpBuf);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_SELF_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_SELF_ROOM_FULL);
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_DEST_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_REFUSE_APPLY:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_REFUSE_APPLY, Npc[nIdx].Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_TASK_ITEM:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GAMBLE_SELF_ROOM_FULL:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_SELF_ROOM_FULL);
		sMsg.eType = SMT_SYSTEM;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_DEST_ROOM_FULL:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_REFUSE_APPLY:
	{
		int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
		if (nIdx <= 0)
			return;
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_REFUSE_APPLY, Npc[nIdx].Name);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_GAMBLE_TASK_ITEM:
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GAMBLE_TASK_ITEM);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	break;
	case enumMSG_ID_ITEM_DAMAGED:
		{
			int nItemID = (int)pShowMsg->m_lpBuf;
			int nIdx = ItemSet.SearchID(nItemID);
			if (!nIdx)
				break;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GET_ITEM:
		{
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			
			int nItemIdx = ItemSet.SearchID(dwID);
			if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
				break;

			char	szName[128];
			KSystemMessage	sMsg;

			strcpy(szName, Item[nItemIdx].GetName());

			if (strlen(szName) >= sizeof(sMsg.szMessage) - strlen(MSG_ADD_ITEM))
				break;
			
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ADD_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_MONEY_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			strcpy(sMsg.szMessage, MSG_MONEY_CANNOT_PICKUP);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TARGET_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TEAM_TARGET_CANNOT_ADD_TEAM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_PK_ERROR_7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEATH_LOSE_ITEM:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), &pShowMsg->m_lpBuf, (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEATH_LOSE_ITEM, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_TONG_RECRUIT_CLOSE:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "Bang hÈi cÒa %s Æ„ng gia nhÀp thµnh vi™n", szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_REFUSE_ADD:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_REFUSE_ADD, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_BE_KICK:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_BE_KICKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_SUCCESS:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_SUCCESS);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_FAIL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_LEAVE_FAIL);
//			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MASTER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MASTER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MEMBER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_CHANGE_AS_MEMBER);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Player[CLIENT_PLAYER_INDEX].m_cTong.m_szMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR:
		{
			char	szName[32];

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR, szName);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR3);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR4);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR5);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR6);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_APPLY_ADD_ERROR7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_APPLY_ADD_ERROR7);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR1);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR2);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FIGHT_MODE_ERROR3:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FIGHT_MODE_ERROR3, Npc[nIdx].Name);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHOP_NO_ROOM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ROOM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_EXTPOINT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_EXTPOINT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_FUYUAN:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_FUYUAN);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_REPUTE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_REPUTE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_ACCUM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_ACCUM);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_HONOR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_HONOR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_SHOP_NO_RESPECT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SHOP_NO_RESPECT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		}
		break;
	case enumMSG_ID_FUNCTION_CHAT_FORBIDDENED:
		{
			time_t rawtime=(int)*((DWORD*)&pShowMsg->m_lpBuf)+1451581200;
			struct tm * timeinfo = localtime(&rawtime);

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			strftime(sMsg.szMessage, sizeof(sMsg.szMessage), MSG_NPC_NOT_PAY_FOR_SPEECH, timeinfo);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_FOBIDDENED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_FOBIDDENED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_FUNCTION_LOCKED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_FUNCTION_LOCKED);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_TRADE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_TRADE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_SETPRICE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_SETPRICE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_LOCK_NOT_THROW:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_LOCK_NOT_THROW);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_TOWNPORTAL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_NPC_NOT_USE_TOWNPORTAL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTREPAIR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_BROKEN_CANTREPAIR);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_SAME_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_SAME_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SKILL_SAME_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_SKILL_SAME_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_CANTPUT_IMMEDIATE:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_PUT_IN_IMMEDIATE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_HORSE_CANT_SWITCH2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE_SIT);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_JOIN:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");
			sprintf(sMsg.szMessage, MSG_CHATROOM_JOIN, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_LEAVE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");
			sprintf(sMsg.szMessage, MSG_CHATROOM_LEAVE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_BEKICK:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");
			sprintf(sMsg.szMessage, MSG_CHATROOM_BEKICK, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_CHATROOM_OPENGAME:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_CHATROOM_OPENGAME, (int)*((DWORD*)&pShowMsg->m_lpBuf));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case  enumMSG_ID_CHATROOM_REVERSE:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), (int)(pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC)), "szName");
			sprintf(sMsg.szMessage, MSG_CHATROOM_REVERSE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			CoreDataChanged(GDCNI_CHATROOM_UPDATE_INTERFACE, 0, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE_SOMEONE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx)
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE_SOMEONE, Npc[nIdx].Name);
			else
				sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_NPC_RENASCENCE:
		{
			KSystemMessage sMsg;
			sMsg.byConfirmType = SMCT_UI_RENASCENCE;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 0;
			sMsg.eType = SMT_PLAYER;
			sprintf(sMsg.szMessage, MSG_NPC_RENASCENCE);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	default:
		break;
	}
}

void	KProtocolProcess::SyncStateEffect(BYTE* pMsg)
{
	STATE_EFFECT_SYNC*	pSync = (STATE_EFFECT_SYNC *)pMsg;

	int nDataNum = MAX_SKILL_STATE - (sizeof(STATE_EFFECT_SYNC) - pSync->m_wLength) / sizeof(KMagicAttrib);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetStateSkillEffect(Player[CLIENT_PLAYER_INDEX].m_nIndex, pSync->m_dwSkillID, pSync->m_nLevel, pSync->m_MagicAttrib, nDataNum, pSync->m_nTime, pSync->m_bOverLook);
}

void	KProtocolProcess::s2cTradeApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeApplyStart(pMsg);
}

void	KProtocolProcess::s2cGambleApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGambleApplyStart(pMsg);
}

void KProtocolProcess::s2cPlayerItemTimeSync(BYTE* pMsg)
{
	PLAYER_ITEM_TIME_SYNC *pSync = (PLAYER_ITEM_TIME_SYNC*)pMsg;
	if(pSync && pSync->m_ItemIdx > 0 && pSync->m_time)
	{
		int nIdx = ItemSet.SearchID(pSync->m_ItemIdx);
		Item[nIdx].SetExpTime(pSync->m_time, 0, 0, 0);
	}
}

void KProtocolProcess::SyncMaskLock(BYTE* pMsg)
{
	NPC_SIT_SYNC *pInfo = (NPC_SIT_SYNC *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_ItemList.SetMaskLock(pInfo->ID);
}

void	KProtocolProcess::s2cTaskValueSync(BYTE* pMsg)
{
	TASK_VALUE_SYNC	*pValue = (TASK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(pValue->nTaskId,pValue->nTaskValue);
	CoreDataChanged(GDCNI_TASK_VALUE_UPDATE, pValue->nTaskId, (int)pValue->nTaskValue);	// [TaskGuide]
}

void KProtocolProcess::s2cSetMissionData(BYTE* pMsg)
{
	PLAYER_MISSION_DATA	*pData = (PLAYER_MISSION_DATA*)pMsg;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup = pData->m_nMissionGroup;
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

void KProtocolProcess::s2cReturnCityOwnTong(BYTE* pMsg)
{
	RETURN_CITY_OWN_TONG	*pData = (RETURN_CITY_OWN_TONG*)pMsg;
	CoreDataChanged(GDCNI_RETURN_CITY_OWN_TONG, 0, (unsigned int)pData->szTongName);
}

void KProtocolProcess::s2cPlayerLoginReplay(BYTE* pMsg) //fix by phong ki“u chuy”n gs bﬁ m t skill
{
	PLAYER_LOGIN_REPLAY	*pData = (PLAYER_LOGIN_REPLAY*)pMsg;
	CoreDataChanged(GDCNI_PLAYER_LOGIN_REPLAY, pData->m_nPlayerIdx, 0); 
}

void KProtocolProcess::s2cOpenTrembleItem(BYTE* pMsg)
{	
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;
	
	OPEN_TREMBLEITEM *pFinish = (OPEN_TREMBLEITEM *)pMsg;
	CoreDataChanged(GDCNI_OPEN_TREMBLE_ITEM, pFinish->m_nType, NULL);
}

void KProtocolProcess::s2cOpenCompoundItem(BYTE* pMsg)
{	
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;
	CoreDataChanged(GDCNI_OPEN_COMPOUND_ITEM, NULL, NULL);
}

void KProtocolProcess::s2cSyncRankData(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA* pInfo = (PLAYER_MISSION_RANKDATA *)pMsg;
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionData, &pInfo->SelfData, sizeof(pInfo->SelfData));
}

void KProtocolProcess::s2cSyncRankData2(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA2* pInfo = (PLAYER_MISSION_RANKDATA2 *)pMsg;
	int i = pInfo->nRankIndex;
	if(i < 0 || i > MISSION_STATNUM)
	{
		i = MISSION_STATNUM;
	}
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionRank[i], &pInfo->MissionRank, sizeof(pInfo->MissionRank));
}


#ifndef _SERVER
void KProtocolProcess::s2cShowDamage(BYTE* pMsg)
{
	DAMAGESHOW* pDamage = (DAMAGESHOW*)pMsg;
	int	receiverNpcIndex = NpcSet.SearchID(pDamage->dwReceiver);
	int casterNpcIndex = NpcSet.SearchID(pDamage->dwLauncher);
	AUTOLOG_EVERY(1000, "DMG-NOIDX thu=%u ridx=%d nan=%u cidx=%d dmg=%d skill=%d t=%u", pDamage->dwReceiver, receiverNpcIndex, pDamage->dwLauncher, casterNpcIndex, pDamage->nDamage, (int)pDamage->SkillId, SubWorld[0].m_dwCurrentTime);

	if (receiverNpcIndex > 0)
	{
		//TODO
		//Npc[nIdx].SetBlood(pDamage->nDamage, pDamage->enType);
		//Npc[receiverNpcIndex].GetCombatInfoShower().AddInfo(casterNpcIndex, pDamage->nDamage, pDamage->SkillId, (COMBAT_INFO_TYPE)pDamage->enType, (TRUE == pDamage->IsCrit ? true : false));
		int nHeight = Npc[receiverNpcIndex].GetNpcPate();
		AUTOLOG_EVERY(1000, "DMG dmg=%d type=%d crit=%d skill=%d nan=%u cidx=%d thu=%u ridx=%d thu_cell=(%d,%d) thu_life=%d me_idx=%d t=%u", pDamage->nDamage, (int)pDamage->enType, (int)pDamage->IsCrit, (int)pDamage->SkillId, pDamage->dwLauncher, casterNpcIndex, pDamage->dwReceiver, receiverNpcIndex, Npc[receiverNpcIndex].m_MapX, Npc[receiverNpcIndex].m_MapY, Npc[receiverNpcIndex].m_CurrentLife, Player[CLIENT_PLAYER_INDEX].m_nIndex, SubWorld[0].m_dwCurrentTime);
		Npc[receiverNpcIndex].SetBlood2(pDamage);
	}
}
void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)
{
	ITEM_SYNC_MAGIC* pItemSync = (ITEM_SYNC_MAGIC*)pMsg;
	DWORD nIdx = ItemSet.SearchID(pItemSync->m_dwID);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		memcpy(Item[nIdx].m_GeneratorParam.nGeneratorLevel, pItemSync->m_MagicLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		Item[nIdx].SetMagicAttrib((KItemNormalAttrib*)pItemSync->m_MagicAttrib);
	}
}

void KProtocolProcess::s2cSyncMeridian(BYTE* pMsg)
{
	MERIDIAN_SYNC* pMeridianSync = (MERIDIAN_SYNC*)pMsg;
	if (pMeridianSync->ProtocolType != s2c_syncmeridian)
		return;
	Player[CLIENT_PLAYER_INDEX].m_cMeridian.setMeridian(pMeridianSync->m_nMeridian);
	Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	CoreDataChanged(GDCNI_PLAYER_MERIDIAN_SYNC, (unsigned int)pMeridianSync->m_nMeridian, pMeridianSync->ProtocolType);
}

void KProtocolProcess::s2cSyncBauCuaResult(BYTE* pMsg)
{
	BAUCUA_RESULT_SYNC* pBauCuaResultSync = (BAUCUA_RESULT_SYNC*)pMsg;
	if (pBauCuaResultSync->ProtocolType != s2c_syncbaucuaresult && pBauCuaResultSync->ProtocolType != s2c_syncbaucuainfo)
		return;
	CoreDataChanged(GDCNI_PLAYER_BAUCUA_RESULT_SYNC, (unsigned int)pBauCuaResultSync, pBauCuaResultSync->ProtocolType);
}

// He XUC XAC chia do (DICEITEM) - may chu bao client mo/dong/cap nhat mot o.
// Chi chuyen tiep len lop giao dien; con tro CHI SONG trong loi goi nay nen ben
// nhan phai chep ra, khong duoc giu lai (y het s2cSyncBauCuaResult).
void KProtocolProcess::s2cDiceItem(BYTE* pMsg)
{
	DICE_ITEM_SYNC* pSync = (DICE_ITEM_SYNC*)pMsg;
	if (pSync->ProtocolType != s2c_diceitem)
		return;
	CoreDataChanged(GDCNI_DICE_ITEM, (unsigned int)pSync, 0);
}

// [PFSYNC 31/08] nhan goi sao/lo/da phi phong - khuon y het s2cSyncMagic:
// tra vat pham theo dwID roi do 4 o m_nPfPack vao ban sao client. Sau do
// PF_StarPrefix / PF_AppendDesc (KItem.cpp) tu song, khong sua them gi.
void KProtocolProcess::s2cSyncItemPfPack(BYTE* pMsg)
{
	ITEM_SYNC_PFPACK* pSync = (ITEM_SYNC_PFPACK*)pMsg;
	if (pSync->ProtocolType != s2c_syncpfpack)
		return;
	DWORD nIdx = ItemSet.SearchID(pSync->m_dwID);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		for (int i = 0; i < 4; i++)
			Item[nIdx].SetPfPack(i, pSync->m_nPfPack[i]);
	}
}

// [DUNGLUYEN 01/09] nhan 6 o Van Cuong + seed - khuon y het s2cSyncItemPfPack. Goi den SAU ITEM_SYNC
// (cung luong TCP) nen ghi de duoc seed tam ma Gen_Fusion sinh o client. Neu mon dang MAC thi tinh
// lai chi so nhan vat (UpdataCurData) de bang chi so client khop server ngay.
void KProtocolProcess::s2cSyncItemFusion(BYTE* pMsg)
{
	ITEM_SYNC_FUSION* pSync = (ITEM_SYNC_FUSION*)pMsg;
	if (pSync->ProtocolType != s2c_syncfusion)
		return;
	DWORD nIdx = ItemSet.SearchID(pSync->m_dwID);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		for (int i = 0; i < KItem::FUS_MAX_SLOT; i++)
			Item[nIdx].SetFusion(i, (int)pSync->m_wFusionP[i], (unsigned)pSync->m_dwSeed[i]);
		int nList = Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame((int)nIdx);
		if (nList > 0 && Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nList].nPlace == pos_equip &&
			Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
			Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	}
}

// [HOASON 01/09b] Linux 0xdd (tu handler reduceskillcd1/2): giam hoi chieu ky nang cua chinh minh tren client
void KProtocolProcess::s2cReduceSkillCD(BYTE* pMsg)
{
	S2C_REDUCE_SKILL_CD* pSync = (S2C_REDUCE_SKILL_CD*)pMsg;
	if (pSync->ProtocolType != s2c_reduceskillcd)
		return;
	int nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nNpc > 0 && nNpc < MAX_NPC)
		Npc[nNpc].m_SkillList.ReduceCoolDown((int)pSync->m_wSkillId, (int)pSync->m_wFrames);
}

// [VHTD 02/09g] so tang No/Am Luat (btKind 0) + khien tinh (btKind 1) cua CHINH MINH - UiPlayerBar / Player_Shield doc qua GDI
void KProtocolProcess::s2cSyncVhtd(BYTE* pMsg)
{
	S2C_SYNC_VHTD* pSync = (S2C_SYNC_VHTD*)pMsg;
	if (pSync->ProtocolType != s2c_syncvhtd)
		return;
	int nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nNpc <= 0 || nNpc >= MAX_NPC)
		return;
	if (pSync->btKind == 0)
	{
		int nFree = -1;
		for (int i = 0; i < MAX_HS_SP; i++)
		{
			if (Npc[nNpc].m_HSSp[i].nKey == (int)pSync->wKey)
			{
				Npc[nNpc].m_HSSp[i].nCount = pSync->nV1;
				Npc[nNpc].m_HSSp[i].nMax = pSync->nV2;
				return;
			}
			if (nFree < 0 && Npc[nNpc].m_HSSp[i].nKey == 0) nFree = i;
		}
		if (nFree >= 0)
		{
			Npc[nNpc].m_HSSp[nFree].nKey = (int)pSync->wKey;
			Npc[nNpc].m_HSSp[nFree].nCount = pSync->nV1;
			Npc[nNpc].m_HSSp[nFree].nMax = pSync->nV2;
		}
	}
	else if (pSync->btKind == 1)
	{
		Npc[nNpc].m_CurrentStaticMagicShieldP = pSync->nV1;
		Npc[nNpc].m_nHSShieldMax = pSync->nV2;
	}
}

// [VHTD 02/09w] Client nhan lenh kich no. JX1 khong dong bo tung vien dan nen ban sao tren may nguoi choi
// van song den het LifeTime (360 khung ~ 20 giay) sau khi may chu da no -> khi truong khong tan, khong
// thay hieu ung. Goi DetonateMissles phia client de no tu chay DoVanish -> Vanish() sinh dan 420 tai cho
// (= hieu ung no) va don khi truong. Khong sinh sat thuong: KMissle::ProcessDamage la server-only.
void KProtocolProcess::s2cDetonate(BYTE* pMsg)
{
	S2C_DETONATE* pD = (S2C_DETONATE*)pMsg;
	if (pD->ProtocolType != s2c_detonate)
		return;
	int nIdx = NpcSet.SearchID(pD->dwLauncherId);
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;
	if (Npc[nIdx].m_Index <= 0 || Npc[nIdx].m_RegionIndex < 0)
		return;
	Npc[nIdx].DetonateMissles((int)pD->wStyle, (int)pD->wRadius, (int)pD->btFlag);
}
#endif

void KProtocolProcess::s2cOpenQuestFinishDlg(BYTE* pMsg)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	QUEST_FINISH_DLG_SYNC *pFinish = (QUEST_FINISH_DLG_SYNC *)pMsg;
	CoreDataChanged(GDCNI_FINISH_QUEST_DLG, (unsigned int)pFinish->m_szNotice, pFinish->m_bType);
	// [DaTau] bao cho auto: cua so 3 ruong dang mo
	g_sDTCap.nFinType = pFinish->m_bType;
	++g_sDTCap.uFinSeq;
}

void	KProtocolProcess::s2cImageNpcSync(BYTE* pMsg)
{
	IMAGENPC_VALUE_SYNC	*pValue = (IMAGENPC_VALUE_SYNC*)pMsg;
	switch (pValue->m_nType)
	{
		case 0:
			Player[CLIENT_PLAYER_INDEX].SetImageNpcId(pValue->m_nValue);
			break;
		case 1:
			//Player[CLIENT_PLAYER_INDEX].SetChangeFace(pValue->m_nValue);
			break;
		default:
			break;
	}
}

void KProtocolProcess::s2cSetObstacle(BYTE* pMsg) //#Set VÀt C∂n
{
	S2C_SET_OBSTACLE	*setibsCmd = (S2C_SET_OBSTACLE *)pMsg;
	
	int		nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(setibsCmd->pMapX, setibsCmd->pMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	//if (nRegion < 0)
	//	return;
	//SubWorld[0].m_Region[nRegion].SetObstacle(setibsCmd->pValue, setibsCmd->pMapX, setibsCmd->pMapY);
	//g_ScenePlace.SetObstacleInfoMin(setibsCmd->pValue, nMapX, nMapY, nOffX, nOffY);
	if(setibsCmd->pValue == -1)
		g_ScenePlace.PaintMapPoint(setibsCmd->pMapX, setibsCmd->pMapY);
	if (setibsCmd->pValue == -2)//send radius pubg
		g_ScenePlace.PaintPUBGCircle(0, 0, setibsCmd->pMapX);
}

void KProtocolProcess::s2cInPutBox(BYTE* pMsg)
{
	S2C_INPUT_BOX	*InPutBoxCmd = (S2C_INPUT_BOX *)pMsg;
	switch (InPutBoxCmd->nType)
	{
		case 1:
			// [LienDau] chup ten ham callback truoc khi bung UI - auto tra loi bang
			// SendClientCmdInputBox(1, 0, <chuoi go vao>, szInpFunc).
			g_StrCpyLen(g_sDTCap.szInpHoi, (char*)InPutBoxCmd->Value, sizeof(g_sDTCap.szInpHoi));
			g_StrCpyLen(g_sDTCap.szInpFunc, (char*)InPutBoxCmd->Value1, sizeof(g_sDTCap.szInpFunc));
			++g_sDTCap.uInpSeq;
			CoreDataChanged(GDCNI_OPEN_INPUT, (unsigned int)InPutBoxCmd->Value, (unsigned int)InPutBoxCmd->Value1);
			break;
		case 2:
			CoreDataChanged(GDCNI_OPEN_INPUT2, (unsigned int)InPutBoxCmd->Value, (unsigned int)InPutBoxCmd->Value1);
			break;
		default:
			break;
	}
}

void KProtocolProcess::s2cPlayerSync_MA(BYTE* pMsg)
{
	S2C_PLAYER_SYNC_M_A *pSync = (S2C_PLAYER_SYNC_M_A*)pMsg;
	switch (pSync->nType)
	{
		case enumS2C_PLAYERSYNC_ID_EXIT:
			//CoreDataChanged(GDCNI_EXIT_GAME, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_GIVE:
			//CoreDataChanged(GDCNI_GIVE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_EQUIPEXPAND:
			//Player[CLIENT_PLAYER_INDEX].SetEquipExpandTime(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_EXPANDBOX:
			//Player[CLIENT_PLAYER_INDEX].SetExpandBoxNum(*(int*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_LOCKSTATE:
			//Player[CLIENT_PLAYER_INDEX].SetLockState(*(BOOL*)(&pSync->m_lpBuf));
			break;
		case enumS2C_PLAYERSYNC_ID_PROPPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nAttributePoint = pSync->nPoint;
			break;
		case enumS2C_PLAYERSYNC_ID_MAGICPOINT:
			Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSync->nPoint;
			CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
			break;
		case enumS2C_PLAYERSYNC_ID_IMMEDSKILL:
		{
			// [TONG 21/08] SetImmedSkill(nSlot, nSkillId) - Linux proto 0x63/0x16: dat chieu tuc thi
			// nPoint = (slot << 24) | skillId. JX2 id 1 = don tay -> JX1 ve mac dinh theo vu khi.
			int nTongSlot = (int)((pSync->nPoint >> 24) & 0xFF);
			int nTongSkill = (int)(pSync->nPoint & 0xFFFFFF);
#ifndef _SERVER
			if (nTongSkill <= 1)
				Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();
			else if (nTongSlot == 0)
				Player[CLIENT_PLAYER_INDEX].SetRightSkill(nTongSkill);
			else
				Player[CLIENT_PLAYER_INDEX].SetLeftSkill(nTongSkill);
#endif
		}
			break;
		case enumS2C_PLAYERSYNC_ID_RANKDATA:
			//CoreDataChanged(GDCNI_RANKDATA, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_ENCHASE:
			//CoreDataChanged(GDCNI_ENCHASE, NULL, NULL);
			break;
		case enumS2C_PLAYERSYNC_ID_INPUT:
			//CoreDataChanged(GDCNI_INPUT, *(int*)(&pSync->m_lpBuf), 0);
			break;
		case enumS2C_PLAYERSYNC_ID_MASKFEATURE:
			//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SwitchMaskFeature();
			break;
	}
}	

void KProtocolProcess::s2cPlayerSync(BYTE* pMsg)
{
	S2C_PLAYER_SYNC* pSync = (S2C_PLAYER_SYNC*)pMsg;
	switch (pSync->m_wMsgID)
	{
	case enumS2C_PLAYERSYNC_ID_EXIT:
		CoreDataChanged(GDCNI_EXIT_GAME, NULL, NULL);
		break;
	//case enumS2C_PLAYERSYNC_ID_GIVE:
	//	CoreDataChanged(GDCNI_GIVE, NULL, NULL);
	//	break;
	//case enumS2C_PLAYERSYNC_ID_EQUIPEXPAND:
	//	Player[CLIENT_PLAYER_INDEX].SetEquipExpandTime(*(int*)(&pSync->m_lpBuf));
	//	break;
	//case enumS2C_PLAYERSYNC_ID_EXPANDBOX:
	//	Player[CLIENT_PLAYER_INDEX].SetExpandBoxNum(*(int*)(&pSync->m_lpBuf));
	//	break;
	//case enumS2C_PLAYERSYNC_ID_LOCKSTATE:
	//	Player[CLIENT_PLAYER_INDEX].SetLockState(*(BOOL*)(&pSync->m_lpBuf));
	//	break;
	case enumS2C_PLAYERSYNC_ID_PROPPOINT:
		Player[CLIENT_PLAYER_INDEX].m_nAttributePoint = *(int*)(&pSync->m_lpBuf);
		break;
	case enumS2C_PLAYERSYNC_ID_MAGICPOINT:
		Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = *(int*)(&pSync->m_lpBuf);
		CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
		break;
	case enumS2C_PLAYERSYNC_ID_RANKDATA:
		CoreDataChanged(GDCNI_RANKDATA, NULL, NULL);
		break;
	//case enumS2C_PLAYERSYNC_ID_ENCHASE:
	//	CoreDataChanged(GDCNI_ENCHASE, NULL, NULL);
	//	break;
	//case enumS2C_PLAYERSYNC_ID_INPUT:
	//	CoreDataChanged(GDCNI_INPUT, *(int*)(&pSync->m_lpBuf), 0);
	//	break;
	case enumS2C_PLAYERSYNC_ID_MASKFEATURE:
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SwitchMaskFeature();
		break;
	}
}

//-----------------------------------------------------------------
// Thuc hien packet mo hop thoai tra vat pham nhiem vu
//-----------------------------------------------------------------
void KProtocolProcess::OpenAffairBox(BYTE* pMsg)
{
	S2C_GIVE_BOX	*GiveBoxCmd = (S2C_GIVE_BOX *)pMsg;
	switch(GiveBoxCmd->nType)
	{
	case 1:
		{
			KUiGiveBox	pInfo;
			strcpy(pInfo.szTitle, GiveBoxCmd->Value); 
			strcpy(pInfo.szInitString, GiveBoxCmd->Value1); 
			strcpy(pInfo.szAction1, GiveBoxCmd->Value2); 
			CoreDataChanged(GDCNI_OPEN_AFFAIR_BOX, (unsigned int)&pInfo, NULL);
			// [DaTau] give-box mo, luu ten ham nop
			g_StrCpyLen(g_sDTCap.szBoxFunc, GiveBoxCmd->Value2, sizeof(g_sDTCap.szBoxFunc));
			g_sDTCap.nBoxOpen = 1;
			++g_sDTCap.uBoxSeq;
		}
		break;
	case 2:
		CoreDataChanged(GDCNI_END_AFFAIR_BOX, NULL, NULL);
		g_sDTCap.nBoxOpen = 0; // [DaTau] give-box dong
		break;
	case 3:
		{
			// [PHI PHONG] panel kham Tinh Than Thach -- dung lai chinh goi nay
			// nen KHONG phai them ProtocolType moi.
			KUiGiveBox	pInfo;
			strcpy(pInfo.szTitle, GiveBoxCmd->Value);
			strcpy(pInfo.szInitString, GiveBoxCmd->Value1);
			strcpy(pInfo.szAction1, GiveBoxCmd->Value2);
			CoreDataChanged(GDCNI_OPEN_MANTLE_INLAY, (unsigned int)&pInfo, NULL);
		}
		break;
	case 4:
		{
			// [PF13 01/09] panel TAY LUYEN thuoc tinh an - dung lai chinh goi nay
			KUiGiveBox	pInfo;
			strcpy(pInfo.szTitle, GiveBoxCmd->Value);
			strcpy(pInfo.szInitString, GiveBoxCmd->Value1);
			strcpy(pInfo.szAction1, GiveBoxCmd->Value2);
			CoreDataChanged(GDCNI_OPEN_MANTLE_WASH, (unsigned int)&pInfo, NULL);
		}
		break;
	case 5:
		{
			// [DUNGLUYEN 01/09] box DUNG LUYEN Van Cuong (2 the: dung luyen / thi luyen) - dung lai chinh goi nay
			KUiGiveBox	pInfo;
			strcpy(pInfo.szTitle, GiveBoxCmd->Value);
			strcpy(pInfo.szInitString, GiveBoxCmd->Value1);
			strcpy(pInfo.szAction1, GiveBoxCmd->Value2);
			CoreDataChanged(GDCNI_OPEN_SMELT_BOX, (unsigned int)&pInfo, NULL);
		}
		break;
	default:
		break;
	}
}

void KProtocolProcess::s2cTimeBox(BYTE* pMsg)
{
	S2C_TIME_BOX	*TimeBoxCmd = (S2C_TIME_BOX *)pMsg;
	KUiTimeBoxInfo	pInfo;
	strcpy(pInfo.szTitle, TimeBoxCmd->Value); 
	pInfo.nTime = TimeBoxCmd->Value1;
	strcpy(pInfo.szAction, TimeBoxCmd->Value2); 
	CoreDataChanged(GDCNI_OPEN_TIME_BOX, (unsigned int)&pInfo, 0);
}

void KProtocolProcess::s2cTalkEx(BYTE* pMsg)
{
	S2C_TALK_EX	*TalkExCmd = (S2C_TALK_EX *)pMsg;
	CoreDataChanged(GDCNI_OPEN_TALK_EX, (unsigned int)TalkExCmd->Value, (unsigned int)TalkExCmd->Value1);
}

//=====================================

void	KProtocolProcess::s2cItemAutoMove(BYTE* pMsg)
{
	ITEM_AUTO_MOVE_SYNC	*pSync = (ITEM_AUTO_MOVE_SYNC*)pMsg;
	ItemPos	sSrc, sDest;
	sSrc.nPlace = pSync->m_btSrcPos;
	sSrc.nX = pSync->m_btSrcX;
	sSrc.nY = pSync->m_btSrcY;
	sDest.nPlace = pSync->m_btDestPos;
	sDest.nX = pSync->m_btDestX;
	sDest.nY = pSync->m_btDestY;
	
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoMoveItem(sSrc, sDest);
}

void KProtocolProcess::FinishedItemExchange(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}


void KProtocolProcess::s2cExtend(BYTE* pMsg)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pMsg + sizeof(tagExtendProtoHeader));

	if (pHeader->ProtocolFamily == pf_playercommunity)
	{
		if (pHeader->ProtocolID == playercomm_s2c_notifychannelid)
		{
			PLAYERCOMM_NOTIFYCHANNELID* pNChann = (PLAYERCOMM_NOTIFYCHANNELID*)pHeader;

			l_pDataChangedNotifyFunc->NotifyChannelID(pNChann->channel, pNChann->channelid, pNChann->cost);
		}
	}
}

void KProtocolProcess::s2cExtendChat(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;
	BYTE protocol = *(BYTE*)(pExPckg);

	if (protocol == chat_someonechat)//Chat mÀt
	{
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)pExPckg;
		l_pDataChangedNotifyFunc->MSNMessageArrival(
			pCscSync->someone, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
			(const char*)(pCscSync + 1), pCscSync->sentlen, true);
	}
	else if (protocol == chat_channelchat)//Chat k™nh
	{
		CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)pExPckg;
		// [DaTau] chup thong diep 'He Thong' (tien do nhat cuon / manh SHXT).
		// (20/08 r3) ghi VONG 4 KHE - 2 tin trong cung tick khong de mat tin truoc.
		bool bDTSapMap = false;	// (r5f) chi TIN DANH BA that su moi bi an khoi khung chat
		if (!strcmp(pCccSync->someone, "H÷ ThËng"))
		{
			int nDTLen = pCccSync->sentlen;
			if (nDTLen > (int)sizeof(g_sDTCap.aMsg[0]) - 1)
				nDTLen = (int)sizeof(g_sDTCap.aMsg[0]) - 1;
			// (r5e) "[SapMap] ..." = danh ba sap server tra ve - kenh du lieu
			// rieng cho auto: khong vao vong khe (khoi de tin tien do) va khong
			// hien len khung chat.
			// (r5f - phan bien) ">= 8": thanh KHONG CO SAP thi server tra dung
			// "[SapMap]" 8 byte - ">" lam goi do bi vut, nhanh "0 sap -> qua thanh
			// ke" thanh ma chet va bot van di tuan mu. memcmp (khong strncmp) vi
			// payload KHONG ket thuc NUL - da chan do dai o tren nen khong doc lo.
			if (nDTLen >= 8 && !memcmp((const char*)(pCccSync + 1), "[SapMap]", 8))
			{
				bDTSapMap = true;
				memcpy(g_sDTCap.szSapMap, (const char*)(pCccSync + 1), nDTLen);
				g_sDTCap.szSapMap[nDTLen] = 0;
				++g_sDTCap.uSapMapSeq;
			}
			else if (nDTLen >= 8 && !memcmp((const char*)(pCccSync + 1), "[TKDich]", 8))
			{	// (04/09) vi tri dich Tong Kim server tra ve - kenh du lieu rieng cho auto, an khoi khung chat
				bDTSapMap = true;
				int nLTD = nDTLen;
				if (nLTD > (int)sizeof(g_szTKDich) - 1)
					nLTD = (int)sizeof(g_szTKDich) - 1;
				memcpy(g_szTKDich, (const char*)(pCccSync + 1), nLTD);
				g_szTKDich[nLTD] = 0;
				++g_uTKDichSeq;
			}
			else if (nDTLen > 0)
			{
				char* pDTKhe = g_sDTCap.aMsg[(g_sDTCap.uMsgSeq + 1) & 3];
				memcpy(pDTKhe, (const char*)(pCccSync + 1), nDTLen);
				pDTKhe[nDTLen] = 0;
				++g_sDTCap.uMsgSeq;
				// (r4) tin TIEN DO ("tong cong") vao kenh rieng - spam khong de duoc
				if (strstr(pDTKhe, "tÊng cÈng") != 0)
				{
					memcpy(g_sDTCap.szTien, pDTKhe, nDTLen + 1);
					++g_sDTCap.uTienSeq;
				}
			}
		}
		// (r5f - phan bien) TRUOC day chan bang strncmp o day nen NGUOI CHOI go
		// tin bat dau bang "[SapMap]" o bat ky kenh nao cung bi nuot voi moi
		// client. Nay chi an dung goi danh ba cua He Thong.
		if (!bDTSapMap)
		{
			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				pCccSync->channelid, pCccSync->someone,
				(const char*)(pCccSync + 1), pCccSync->sentlen, true);
		}
	}
	else if (protocol == chat_feedback)
	{
		////X
		CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)pExPckg;
		DWORD* pChannelid = (DWORD*)(pCfb + 1);

		if (*pChannelid == -1)
		{//someone
			char* pDstName = (char*)(pChannelid + 1);
			BYTE* pSentlen = (BYTE*)(pDstName + _NAME_LEN);
			void* pSent = pSentlen + 1;
			
			l_pDataChangedNotifyFunc->MSNMessageArrival(
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pDstName,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		else
		{//channel
			BYTE* pSentlen = (BYTE*)(pChannelid + 1);
			void* pSent = pSentlen + 1;

			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				*pChannelid, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		////X
	}
}


static BOOL sParseUGName(const std::string& name, std::string* pUnit, std::string* pGroup)
{
	static const char char_split = '\n';

	size_t pos = name.find(char_split);
	if (pos == name.npos)
	{
		if (pUnit)
			pUnit->resize(0);
		if (pGroup)
			pGroup->assign(name);
	}
	else
	{
		std::string::const_iterator itSplit = name.begin() + pos;

		if (pUnit)
			pUnit->assign(name.begin(), itSplit);
		if (pGroup)
			pGroup->assign(itSplit + 1, name.end());
	}

	return TRUE;
}

void KProtocolProcess::s2cExtendFriend(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pExPckg);

	if (pHeader->ProtocolFamily == pf_tong)
	{
		_ASSERT(FALSE);
		//tong message
		//...
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{
		if (pHeader->ProtocolID == friend_c2c_askaddfriend)
		{
			ASK_ADDFRIEND_SYNC* pAafSync = (ASK_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->FriendInvite(pAafSync->srcrole);
		}
		else if (pHeader->ProtocolID == friend_c2c_repaddfriend)
		{
			REP_ADDFRIEND_SYNC* pRafSync = (REP_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->AddFriend(pRafSync->srcrole, pRafSync->answer);
		}
		else if (pHeader->ProtocolID == friend_s2c_repsyncfriendlist)
		{
			REP_SYNCFRIENDLIST* pRsfl = (REP_SYNCFRIENDLIST*)pHeader;
			char* pGroupTag = (char*)(pRsfl + 1);
			if (*pGroupTag != specGroup)
				goto on_error;
			{{
			char* pGroup = (char*)(pGroupTag + 1);
on_newgroup:
			std::string theUnit, theGroup;
			sParseUGName(std::string(pGroup), &theUnit, &theGroup);

			for (char* pRoleTag = pGroup + strlen(pGroup) + 1; ; )
			{
				if (*pRoleTag == specOver)
					goto on_over;
				else if (*pRoleTag == specGroup)
				{
					pGroup = pRoleTag + 1;
					goto on_newgroup;
				}
				else if (*pRoleTag == specRole)
				{
					char* pState = pRoleTag + 1;
					char* pRole = pState + 1;
					l_pDataChangedNotifyFunc->FriendInfo(pRole, (char*)theUnit.c_str(), (char*)theGroup.c_str(), (BYTE)*pState);
					pRoleTag = pRole + strlen(pRole) + 1;
					continue;
				}
				else
					goto on_error;
			}
			}}
on_error:
			_ASSERT(FALSE);
on_over:
			0;
		}
		else if (pHeader->ProtocolID == friend_s2c_friendstate)
		{
			FRIEND_STATE* pFs = (FRIEND_STATE*)pHeader;
			for (char* pRole = (char*)(pFs + 1); *pRole; pRole += strlen(pRole) + 1)
				l_pDataChangedNotifyFunc->FriendStatus(pRole, pFs->state);
		}
		else if (pHeader->ProtocolID == friend_s2c_syncassociate)
		{
			FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)pHeader;

			char* szGroup = (char*)(pFsa + 1);
			std::string group(szGroup);

			std::string theUnit, theGroup;
			sParseUGName(group, &theUnit, &theGroup);
			_ASSERT(theGroup.empty());

			for (char* szRole = szGroup + group.size() + 1; *szRole; szRole += strlen(szRole) + 1)
				l_pDataChangedNotifyFunc->AddPeople((char*)theUnit.c_str(), szRole);
		}
	}
}


void KProtocolProcess::s2cExtendTong(BYTE* pMsg)
{
	S2C_TONG_HEAD	*pHead = (S2C_TONG_HEAD*)pMsg;
	switch (pHead->m_btMsgId)
	{
	case enumTONG_SYNC_ID_CREATE_FAIL:
		{
			TONG_CREATE_FAIL_SYNC *pFail = (TONG_CREATE_FAIL_SYNC*)pMsg;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, "Thµnh lÀp bang hÈi th t bπi!");
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			switch (pFail->m_btFailId)
			{
			case enumTONG_CREATE_ERROR_ID1:		// Player[m_nPlayerIndex].m_nIndex <= 0
				break;
			case enumTONG_CREATE_ERROR_ID2:		// Ωª“◊π˝≥Ã÷–
				break;
			case enumTONG_CREATE_ERROR_ID3:		// ∞Ôª·√˚Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR09);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID4:		// ∞Ôª·’Û”™Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR02);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID5:		// “—æ≠ «∞Ôª·≥…‘±
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR03);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID6:		// ◊‘º∫µƒ’Û”™Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR04);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID7:		// µ»º∂Œ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR05);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID8:		// «ÆŒ Ã‚
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR10);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID9:		// ◊È∂”≤ªƒ‹Ω®∞Ôª·
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR08);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID10:	// ∞Ôª·ƒ£øÈ≥ˆ¥Ì
				break;
			case enumTONG_CREATE_ERROR_ID11:	// √˚◊÷◊÷∑˚¥Æ≥ˆ¥Ì
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID12:	// √˚◊÷◊÷∑˚¥Æπ˝≥§
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID13:	// ∞Ôª·Õ¨√˚¥ÌŒÛ
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID14:	// ∞Ôª·≤˙…˙ ß∞‹
				sprintf(sMsg.szMessage, MSG_TONG_CREATE_ERROR12);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_TRANSFER_ADD_APPLY:
		{
			TONG_APPLY_ADD_SYNC	*pApply = (TONG_APPLY_ADD_SYNC*)pMsg;
			char	szName[32];
			DWORD	dwNameID;
			int		nPlayerIdx;

			memset(szName, 0, sizeof(szName));
			BIEN_ChepAnToan(szName, (int)sizeof(szName), pApply->m_szName, (int)(pApply->m_wLength + 1 + sizeof(pApply->m_szName) - sizeof(TONG_APPLY_ADD_SYNC)), "szName");
			dwNameID = g_FileName2Id(szName);
			nPlayerIdx = pApply->m_nPlayerIdx;

			// ∏¯ΩÁ√Ê∑¢œ˚œ¢£¨ ’µΩ…Í«Î£¨ «∑ÒÕ¨“‚
			KSystemMessage	sMsg;
			//sprintf(sMsg.szMessage, "%s…Í«Îº”»Î∞Ôª·£° enumTONG_SYNC_ID_TRANSFER_ADD_APPLY", szName);
			sprintf(sMsg.szMessage, "%s xin gia nhÀp bang", szName);
			sMsg.eType = SMT_CLIQUE;
			sMsg.byConfirmType = SMCT_UI_TONG_JOIN_APPLY;
			sMsg.byPriority = 3;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			
			KUiPlayerItem	player;
			strcpy(player.Name, szName);
			player.nIndex = pApply->m_nPlayerIdx;
			player.uId = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);

		}
		break;
	case enumTONG_SYNC_ID_ADD:
		{
			TONG_Add_SYNC	*pAdd = (TONG_Add_SYNC*)pMsg;
			char	szName[32], szTitle[32], szMaster[32];

			memcpy(szName, pAdd->m_szTongName,sizeof(szName));
			memcpy(szTitle, pAdd->m_szTitle, sizeof(szTitle));
			memcpy(szMaster, pAdd->m_szMaster, sizeof(szMaster));

			Player[CLIENT_PLAYER_INDEX].m_cTong.AddTong(pAdd->m_btCamp, szName, szTitle, szMaster);
			// Õ®÷™ΩÁ√Ê∏¸–¬ ˝æ›
			KUiGameObjectWithName	sUi;
			memset(&sUi, 0, sizeof(sUi));
			strcpy(sUi.szName, szName);
			sUi.nData = TONG_ACTION_APPLY;
			strcpy(sUi.szString, szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		/*------------------------------*/
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;

	case enumTONG_SYNC_ID_HEAD_INFO:
		{
			TONG_HEAD_INFO_SYNC	*pInfo = (TONG_HEAD_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·–≈œ¢
			int nIdx = NpcSet.SearchID(pInfo->m_dwNpcID);
			if (nIdx <= 0)
				break;

			KUiPlayerRelationWithOther	sUi;
			sUi.nIndex = nIdx;
			sUi.uId = Npc[nIdx].m_dwID;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				sUi.nRelation = Player[CLIENT_PLAYER_INDEX].m_cTong.GetFigure();
			else
				sUi.nRelation = -1;
			sUi.nData = 0;
			sUi.nParam = 0;
			strcpy(sUi.Name, Npc[nIdx].Name);

			KTongInfo	sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			sInfo.nFaction = pInfo->m_btCamp;
			sInfo.nMemberCount = (int)pInfo->m_dwMemberNum;
			sInfo.nManagerCount = (int)pInfo->m_btManagerNum;
			sInfo.nDirectorCount = (int)pInfo->m_btDirectorNum;
			sInfo.nMoney = pInfo->m_dwMoney;
			strcpy(sInfo.szMasterName, pInfo->m_sMember[0].m_szName);
			strcpy(sInfo.szName, pInfo->m_szTongName);
			// == add by Fong Ki“u ==
			sInfo.nStatusGuide = pInfo->m_nStatusGuide;
			strcpy(sInfo.szWayEdit, pInfo->m_szWayEdit);
			strcpy(sInfo.szNextTargetEdit, pInfo->m_szNextTargetEdit);
			sInfo.nExpGuide = pInfo->m_nExpGuide;
			sInfo.nCityGuide = pInfo->m_nCityGuide;
			sInfo.nTongLevel = pInfo->m_nTongLevel;
			strcpy(sInfo.szLeagueTName, pInfo->m_szLeagueTName);
			// == end add by Fong Ki“u ==
			CoreDataChanged(GDCNI_TONG_INFO, (unsigned int)&sUi, (unsigned int)&sInfo);
			//
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_DIRECTOR;
			sObj.nParam = 0;
			sObj.uParam = defTONG_MAX_DIRECTOR;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_MAX_DIRECTOR];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btDirectorNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_DIRECTOR;
				strcpy(sItem[i].Name, pInfo->m_sMember[i + 1].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i + 1].m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i + 1].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_SELF_INFO:
		{
			TONG_SELF_INFO_SYNC	*pInfo = (TONG_SELF_INFO_SYNC*)pMsg;
			Player[CLIENT_PLAYER_INDEX].m_cTong.SetSelfInfo(pInfo);
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumTONG_SYNC_ID_MANAGER_INFO:
		{
			TONG_MANAGER_INFO_SYNC	*pInfo = (TONG_MANAGER_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·∂”≥§–≈œ¢
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MANAGER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MANAGER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i].m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_MEMBER_INFO:
		{
			TONG_MEMBER_INFO_SYNC	*pInfo = (TONG_MEMBER_INFO_SYNC*)pMsg;

			// Õ®÷™ΩÁ√Êµ√µΩƒ≥∞Ôª·∞Ô÷⁄–≈œ¢
			KUiGameObjectWithName	sObj;
			strcpy(sObj.szName, pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MEMBER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MEMBER;
				strcpy(sItem[i].Name, pInfo->m_sMember[i].m_szName);
				if (pInfo->m_sMember[i].m_btSex)
					strcpy(sItem[i].szAgname, pInfo->m_szTitleGirl);
				else
					strcpy(sItem[i].szAgname, pInfo->m_szTitleBoy);
				if (!sItem[i].szAgname[0])
					strcpy(sItem[i].szAgname, pInfo->m_szTitle);
				sItem[i].btOnline = pInfo->m_sMember[i].m_btOnline;
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_INSTATE:
		{
			TONG_INSTATE_SYNC	*pInstate = (TONG_INSTATE_SYNC*)pMsg;
			// Õ®÷™ΩÁ√Ê»Œ√¸ «∑Ò≥…π¶
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pInstate->m_szName);
			sUi.nData = TONG_ACTION_ASSIGN;
			sUi.nParam = pInstate->m_btNewFigure;
			sUi.uParam = pInstate->m_btOldFigure;
			strcpy(sUi.szString, pInstate->m_szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pInstate->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_KICK:
		{
			TONG_KICK_SYNC	*pKick = (TONG_KICK_SYNC*)pMsg;
			// Õ®÷™ΩÁ√ÊÃﬂ»À «∑Ò≥…π¶
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, pKick->m_szName);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = pKick->m_btFigure;
			sUi.uParam = pKick->m_btPos;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pKick->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_MASTER_FAIL:
		{
			TONG_CHANGE_MASTER_FAIL_SYNC	*pFail = (TONG_CHANGE_MASTER_FAIL_SYNC*)pMsg;

			switch (pFail->m_btFailID)
			{
			case 0:		// ∂‘∑Ω≤ª‘⁄œﬂ
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, ÆËi ph≠¨ng kh´ng online!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		// ∂‘∑Ωƒ‹¡¶≤ªπª£°
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, ÆËi ph≠¨ng kh´ng ÆÒ tµi l∑nh Æπo!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		// √˚◊÷≤ª∂‘£°
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "Phong ch¯c th t bπi, bÊn bang kh´ng c„ ng≠Íi nµy!");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;

			default:
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_TITLE_FAIL:
		{
			TONG_CHANGE_TITLE_FAIL_SYNC	*pFail = (TONG_CHANGE_TITLE_FAIL_SYNC*)pMsg;
			
			switch (pFail->m_btFailID)
			{
			case 0:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, ÆËi ph≠¨ng kh´ng tr™n mπng.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, ÆËi ph≠¨ng kh´ng ÆÒ tµi l∑nh Æπo.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "ßÊi t™n th t bπi, bÊn bang kh´ng c„ ng≠Íi nµy.");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			default:
				break;
			}
		}
		break;
			case enumTONG_SYNC_ID_CHANGE_CAMP_FAIL:
				{
					TONG_CHANGE_CAMP_FAIL_SYNC *pFail = (TONG_CHANGE_CAMP_FAIL_SYNC*)pMsg;
					
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					
					sprintf(sMsg.szMessage, "Kh´ng th” thay ÆÊi mµu bang hÈi ");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					
					switch (pFail->m_btFailID)
					{
					case enumTONG_CHANGE_CAMP_ERROR_ID1:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID2:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID3:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID4:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID5:
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID6:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR1);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID7:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR2);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					case enumTONG_CHANGE_CAMP_ERROR_ID8:
						sprintf(sMsg.szMessage, MSG_TONG_CHANGE_CAMP_ERROR3);
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
				}
				break;
			case enumTONG_SYNC_ID_CHANGE_CAMP:
				{
					TONG_CHANGE_CAMP_SYNC	*pChange = (TONG_CHANGE_CAMP_SYNC*)pMsg;
					Player[CLIENT_PLAYER_INDEX].m_cTong.ChangeCamp(pChange->m_btCamp);
				}
				break;	// thieu break lam goi CHANGE_CAMP roi tiep vao nhanh JX2 ben duoi
			case enumTONG_SYNC_ID_JX2:	// JX2 port: chuyen nguyen goi len UI
		{
			CoreDataChanged(GDCNI_TONG_JX2, (unsigned int)pMsg, (int)pHead->m_wLength + 1);
		}
		break;
		case enumTONG_SYNC_ID_CHANGE_RECRUIT:
				{
					TONG_CHANGE_RECRUIT_SYNC	*pChange = (TONG_CHANGE_RECRUIT_SYNC*)pMsg;
					Player[CLIENT_PLAYER_INDEX].m_cTong.ChangeRecruit(pChange->m_btRecruit);
				}
		break;

	default:
		break;
	}
}


void KProtocolProcess::s2cChangeWeather(BYTE* pMsg)
{
	g_ScenePlace.ChangeWeather(((SYNC_WEATHER*)pMsg)->WeatherID);
}

void	KProtocolProcess::s2cPKSyncNormalFlag(BYTE* pMsg)
{
	PK_NORMAL_FLAG_SYNC	*pFlag = (PK_NORMAL_FLAG_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetNormalPKState(pFlag->m_btFlag, pFlag->m_bLockPK);
}

void	KProtocolProcess::s2cPKSyncEnmityState(BYTE* pMsg)
{
	PK_ENMITY_STATE_SYNC	*pState = (PK_ENMITY_STATE_SYNC*)pMsg;
	char	szName[32];

	memset(szName, 0, sizeof(szName));
	BIEN_ChepAnToan(szName, (int)sizeof(szName), pState->m_szName, (int)(pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_ENMITY_STATE_SYNC)), "szName");
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetEnmityPKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKSyncExerciseState(BYTE* pMsg)
{
	PK_EXERCISE_STATE_SYNC	*pState = (PK_EXERCISE_STATE_SYNC*)pMsg;
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	BIEN_ChepAnToan(szName, (int)sizeof(szName), pState->m_szName, (int)(pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_EXERCISE_STATE_SYNC)), "szName");
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetExercisePKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKValueSync(BYTE* pMsg)
{
	PK_VALUE_SYNC	*pValue = (PK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetPKValue(pValue->m_nPKValue);
}

void	KProtocolProcess::s2cReputeValueSync(BYTE* pMsg)
{
	REPUTE_VALUE_SYNC	*pValue = (REPUTE_VALUE_SYNC*)pMsg;
	//Player[CLIENT_PLAYER_INDEX].m_cRepute.SetReputeValue(pValue->m_nReputeValue);
}

void	KProtocolProcess::s2cFuYuanValueSync(BYTE* pMsg)
{
	FUYUAN_VALUE_SYNC	*pValue = (FUYUAN_VALUE_SYNC*)pMsg;
	//Player[CLIENT_PLAYER_INDEX].m_cFuYuan.SetFuYuanValue(pValue->m_nFuYuanValue);
}

void	KProtocolProcess::s2cReBornValueSync(BYTE* pMsg)
{
	// REBORN_VALUE_SYNC	*pValue = (REBORN_VALUE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cReBorn.SetReBornValue(pMsg);	   //pValue->m_nReBornValue
}

void	KProtocolProcess::s2cViewEquip(BYTE* pMsg)
{
	g_cViewItem.GetData(pMsg);
}

void	KProtocolProcess::s2cTongCreate(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.Create((TONG_CREATE_SYNC*)pMsg);
}

void	KProtocolProcess::s2cNpcGoldChange(BYTE* pMsg)
{/*
	NPC_GOLD_CHANGE_SYNC	*pSync = (NPC_GOLD_CHANGE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->m_dwNpcID);

	if (nIdx && Npc[nIdx].m_Kind == kind_normal)
	{
		Npc[nIdx].m_cGold.SetGoldCurrentType((int)pSync->m_wGoldFlag);
	}*/
}

void	KProtocolProcess::ItemChangeDurability(BYTE* pMsg)
{
	ITEM_DURABILITY_CHANGE	*pIDC = (ITEM_DURABILITY_CHANGE *)pMsg;

	int nIdx = ItemSet.SearchID(pIDC->dwItemID);
	
	if (nIdx)
	{
		Item[nIdx].SetDurability(Item[nIdx].GetDurability() + pIDC->nChange);
		_ASSERT(Item[nIdx].GetDurability() >= 0);
		if (Item[nIdx].GetDurability() <= 3)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_NEARLY_DAMAGED, Item[nIdx].GetName());
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KProtocolProcess::LadderList(BYTE* pMsg)
{
	LADDER_LIST*	pList = (LADDER_LIST *)pMsg;
	KRankIndex		*pLadderListIndex = NULL;
	
	if (pList->nCount > 0 && pList->nCount < enumLadderEnd)
	{
		pLadderListIndex = new KRankIndex[pList->nCount];
	}
	if (pLadderListIndex)
	{
		for (int i = 0; i < pList->nCount; i++)
		{
			pLadderListIndex[i].usIndexId = pList->dwLadderID[i];
			pLadderListIndex[i].bValueAppened = true;
			pLadderListIndex[i].bSortFlag = true;
		}
		CoreDataChanged(GDCNII_RANK_INDEX_LIST_ARRIVE, pList->nCount, (int)pLadderListIndex);
		delete [] pLadderListIndex;
		pLadderListIndex = NULL;
	}
}

void KProtocolProcess::LadderResult(BYTE* pMsg)
{
	LADDER_DATA*	pLadderData = (LADDER_DATA *)pMsg;
	KRankMessage*	pLadderMessage = NULL;

	pLadderMessage = new KRankMessage[10];
	if (pLadderMessage)
	{
		for (int i = 0; i < 10; i++)
		{
			pLadderMessage[i].usMsgLen = strlen(pLadderData->StatData[i].Name);
			strcpy(pLadderMessage[i].szMsg, pLadderData->StatData[i].Name);
			pLadderMessage[i].nValueAppend = pLadderData->StatData[i].nValue;
			pLadderMessage[i].cSortFlag = (char)pLadderData->StatData[i].bySort;
		}
		unsigned int uParam = 10 | (((WORD)pLadderData->dwLadderID) << 16);
		CoreDataChanged(GDCNII_RANK_INFORMATION_ARRIVE, uParam, (int)pLadderMessage);
		delete [] pLadderMessage;
		pLadderMessage = NULL;
	}
}
#else

void KProtocolProcess::RemoveRole(int nIndex, BYTE * pProtocol)
{

}

void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)
{
	NPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;
#ifdef _SERVER
	// [S11-DOIMAP 26/08] KNpcSet::SyncNpc tim ID TOAN CUC moi map va goi NPC_SYNC khong
	// mang ma map => nguoi choi da roi map (ve thanh) van duoc tra ve TOA DO MAP KHAC,
	// client dien giai tren map hien tai thanh vi tri bay va lam ma BAT TU (moi lan tra
	// loi lai refresh dong ho don). Khac map thi tra FAIL de client go ban sao.
	int nS11Me = Player[nIndex].m_nIndex;
	if (nS11Me > 0 && nS11Me < MAX_NPC)
	{
		int nS11Found = NpcSet.SearchID(pNpcRequestSync->ID);
		if (nS11Found > 0 && Npc[nS11Found].m_SubWorldIndex != Npc[nS11Me].m_SubWorldIndex)
		{
			NPC_REQUEST_FAIL S11Fail;
			S11Fail.ProtocolType = (BYTE)s2c_requestnpcfail;
			S11Fail.ID = pNpcRequestSync->ID;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&S11Fail, sizeof(S11Fail));
			AUTOLOG_IDX(nS11Me, "[S11-DOIMAP] plr=%d hoi id=%u dang o map khac (sw=%d vs %d) -> tra fail", nIndex, pNpcRequestSync->ID, Npc[nS11Found].m_SubWorldIndex, Npc[nS11Me].m_SubWorldIndex);
			return;
		}
	}
#endif
	NpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);
}

void KProtocolProcess::ObjRequestCommand(int nIndex, BYTE* pProtocol)
{
	OBJ_CLIENT_SYNC_ADD	*pObjClientSyncAdd = (OBJ_CLIENT_SYNC_ADD*)pProtocol;
	ObjSet.SyncAdd(pObjClientSyncAdd->m_nID, Player[nIndex].m_nNetConnectIdx);
}

void KProtocolProcess::NpcWalkCommand(int nIndex, BYTE* pProtocol)
{
	NPC_WALK_COMMAND* pNetCommand = (NPC_WALK_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}
	Npc[Player[nIndex].m_nIndex].SendCommand(do_walk, ParamX, ParamY);
}

void KProtocolProcess::NpcRunCommand(int nIndex, BYTE* pProtocol)
{
	NPC_RUN_COMMAND* pNetCommand = (NPC_RUN_COMMAND *)pProtocol;

	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}

	int nIdx = Player[nIndex].m_nIndex;
	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		Npc[nIdx].SendCommand(do_run, ParamX, ParamY);
	}
}


void KProtocolProcess::NpcSkillCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SKILL_COMMAND* pNetCommand = (NPC_SKILL_COMMAND *)pProtocol;
	AUTOLOG_IDX(Player[nIndex].m_nIndex, "[S3-PKT-IN] plr=%d npcidx=%d skill=%d mpsx=%d mpsy=%d", nIndex, Player[nIndex].m_nIndex, pNetCommand->nSkillID, pNetCommand->nMpsX, pNetCommand->nMpsY);
	int ParamX = pNetCommand->nSkillID;
	int ParamY = pNetCommand->nMpsX;
	int ParamZ = pNetCommand->nMpsY;
	AUTOLOG_IDX_EVERY(Player[nIndex].m_nIndex, 300, "[S2-NETSKILL-IN] plr=%d npc=%d skill=%d mpsx=%d mpsy=%d rej_id=%d rej_y=%d rej_x=%d doing=%d fight=%d rgn=%d map=(%d,%d)", nIndex, Player[nIndex].m_nIndex, ParamX, ParamY, ParamZ, (int)(ParamX <= 0 || ParamX > MAX_SKILL), (int)(ParamZ < 0), (int)(ParamY < 0 && ParamY != -1), (int)Npc[Player[nIndex].m_nIndex].m_Doing, (int)Npc[Player[nIndex].m_nIndex].m_FightMode, Npc[Player[nIndex].m_nIndex].m_RegionIndex, Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY);
	if (ParamX <= 0 || ParamX > MAX_SKILL || ParamZ < 0 || (ParamY < 0 && ParamY != -1))
		AUTOLOG_IDX(Player[nIndex].m_nIndex, "[S3-PKT-REJ] plr=%d npcidx=%d skill=%d mpsx=%d mpsy=%d maxskill=%d rejskill=%d rejz=%d rejy=%d", nIndex, Player[nIndex].m_nIndex, ParamX, ParamY, ParamZ, (int)MAX_SKILL, (int)(ParamX <= 0 || ParamX > MAX_SKILL), (int)(ParamZ < 0), (int)(ParamY < 0 && ParamY != -1));
	if (ParamX <= 0 || ParamX > MAX_SKILL )
	{
		return ;
	}

	if (ParamZ < 0)
	{
		return;
	}

	if (ParamY < 0)
	{
		if (ParamY != -1) 
			return;

		int nNpcIndex = Player[nIndex].FindAroundNpc((DWORD)ParamZ);
		AUTOLOG_IDX_EVERY(Player[nIndex].m_nIndex, 300, "[S2-NETSKILL-TARGET] plr=%d npc=%d skill=%d want_tgt_id=%u found_idx=%d accept=%d dist=%d rgn=%d", nIndex, Player[nIndex].m_nIndex, ParamX, (unsigned int)ParamZ, nNpcIndex, (int)(nNpcIndex > 0), (nNpcIndex > 0 ? NpcSet.GetDistance(Player[nIndex].m_nIndex, nNpcIndex) : -1), Npc[Player[nIndex].m_nIndex].m_RegionIndex);
		if (nNpcIndex <= 0)
			AUTOLOG_IDX(Player[nIndex].m_nIndex, "[S3-TGT-FIND] plr=%d npcidx=%d skill=%d tgtid=%d found=%d tgtkind=%d tgtlife=%d me=(%d,%d) tgt=(%d,%d) merng=%d tgtrng=%d", nIndex, Player[nIndex].m_nIndex, ParamX, ParamZ, nNpcIndex, -1, -1, Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, -1, -1, Npc[Player[nIndex].m_nIndex].m_RegionIndex, -1);
#ifdef _SERVER
		if (nNpcIndex <= 0 && ParamZ > 0 && Player[nIndex].m_nNetConnectIdx >= 0)
		{
			// [S10-MA 26/08] GO BONG MA: client vua xin danh mot ID KHONG con trong vung
			// 3x3 quanh no. Nguyen nhan pho bien: goi go s2c_npcremove truoc do bi rot
			// (ngan sach broadcast MAX_BROADCAST_COUNT=100 nguoi/luot trong dam dong,
			// KRegion.cpp:1395) => ban sao ma ket trong bang client toi ~55s (bo don
			// 1000 tick, KNpcSet.cpp:755) va auto dung danh gio ca nua phut (do that
			// 26/08: dot 28,7s danh id=92666 found=0, 323 ma bi don trong 282 giay).
			// Server dang cam du thong tin o day: gui lai goi go CO SAN (5 byte) cho
			// RIENG client nay - client xoa ma ngay, auto doi muc tieu o nhip ke.
			// ID khong ton tai phia client thi handler ConformIdx tu vut (no-op).
			NPC_REMOVE_SYNC RemoveCmd;
			RemoveCmd.ProtocolType = (BYTE)s2c_npcremove;
			RemoveCmd.ID = (DWORD)ParamZ;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&RemoveCmd, sizeof(RemoveCmd));
			AUTOLOG_IDX(Player[nIndex].m_nIndex, "[S10-MA] plr=%d go bong ma id=%u khoi client (skill=%d)", nIndex, (unsigned int)ParamZ, ParamX);
		}
#endif
		if (nNpcIndex > 0)
			Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, nNpcIndex);
	}
	else
	{
		AUTOLOG_IDX_EVERY(Player[nIndex].m_nIndex, 500, "[S3-CAST-XY] plr=%d npcidx=%d skill=%d mps=(%d,%d) doing=%d frozen=%d randmove=%d actskill=%d radius=%d", nIndex, Player[nIndex].m_nIndex, ParamX, ParamY, ParamZ, (int)Npc[Player[nIndex].m_nIndex].m_Doing, Npc[Player[nIndex].m_nIndex].m_FrozenAction.nTime, Npc[Player[nIndex].m_nIndex].m_RandMove.nTime, Npc[Player[nIndex].m_nIndex].m_ActiveSkillID, Npc[Player[nIndex].m_nIndex].m_CurrentAttackRadius);
		Npc[Player[nIndex].m_nIndex].SendCommand(do_skill, ParamX, ParamY, ParamZ);
	}
}

void KProtocolProcess::NpcJumpCommand(int nIndex, BYTE* pProtocol)
{
	NPC_JUMP_COMMAND* pNetCommand = (NPC_JUMP_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	Npc[Player[nIndex].m_nIndex].SendCommand(do_jump, ParamX, ParamY);
}

void KProtocolProcess::c2sDynamicStruct(int nIndex, BYTE* pProtocol)
{
	DYNAMIC_COMMAND* p = (DYNAMIC_COMMAND*)pProtocol;
	if(p->nBranch == c2sdnmbr_arrangeitem)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		Player[nIndex].AutoArrangeItem();
	}
	else if(p->nBranch == c2sdnmbr_arrangebox)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(Player[nIndex].AutoArrangeItem(1))
		{
			Player[nIndex].AutoArrangeItem(2);
			Player[nIndex].AutoArrangeItem(3);
			Player[nIndex].AutoArrangeItem(4);
		}
	}
	else if(p->nBranch == c2sdnmbr_exchangeitem)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(!Player[nIndex].m_CUnlocked)
			return;
		if(p->m_wLength + 1 - sizeof(DYNAMIC_COMMAND) < 2*sizeof(BYTE) + sizeof(int))
			return;
		BYTE* pPos = (BYTE*)(p+1);
		int nSrcPos = *(pPos++);
		int nTargetPos = *(pPos++);
		int nItemID = *(int*)pPos;
		if(nSrcPos == nTargetPos)
			return;
		//printf("src %d tg %d item %d\n", nSrcPos, nTargetPos, nItemID);
		if(nSrcPos == pos_equiproom)
		{
			int nSrcPlace;
			int nSrcIdx = Player[nIndex].m_ItemList.SearchID(nItemID, &nSrcPlace);
			if(!nSrcIdx || nSrcPlace != pos_equiproom)
				return;
			if(nTargetPos == pos_equip || nTargetPos == pos_equipback)
			{
				int nPart = Player[nIndex].m_ItemList.GetEquipPlace(Item[nSrcIdx].GetDetailType());
				if(nPart < 0)
					return;
				if(!Player[nIndex].m_ItemList.CanEquip(nSrcIdx, nPart))
					return;
				int nDesIdx = Player[nIndex].m_ItemList.GetEquipment(nPart);
				if(nDesIdx > 0)
				{
					if(nPart == itempart_ring1)
					{
						int nRing2 = Player[nIndex].m_ItemList.GetEquipment(itempart_ring2);
						if(nRing2 <= 0)
						{
							nPart = itempart_ring2;
							nDesIdx = 0;
						}
					}
					if(nDesIdx > 0)
					{
						int x, y;
						if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
							Item[nDesIdx].GetWidth(), Item[nDesIdx].GetHeight(), &x, &y))
							return;
						Player[nIndex].m_ItemList.Remove(nDesIdx);
						Player[nIndex].m_ItemList.AddKIL(nDesIdx, pos_equiproom, x,y);
					}
				}
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_equip, nPart, 0);
			}
			else if(nTargetPos == pos_repositoryroom)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_repository))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_repositoryroom, x, y);
			}
			else if(nTargetPos == pos_exbox1room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 1)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox1))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox1room, x, y);
			}
			else if(nTargetPos == pos_exbox2room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 2)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox2))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox2room, x, y);
			}
			else if(nTargetPos == pos_exbox3room)
			{
				if(Npc[Player[nIndex].m_nIndex].m_FightMode)
					return;
				if(Npc[Player[nIndex].m_nIndex].m_ExBoxId < 3)
					return;
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y, room_exbox3))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_exbox3room, x, y);
			}
		}
		else if(nSrcPos == pos_equip || nSrcPos == pos_equipback ||(!Npc[Player[nIndex].m_nIndex].m_FightMode &&( nSrcPos == pos_repositoryroom || nSrcPos == pos_exbox1room || nSrcPos == pos_exbox2room || nSrcPos == pos_exbox3room)))
		{
			int nSrcPlace;
			int nSrcIdx = Player[nIndex].m_ItemList.SearchID(nItemID, &nSrcPlace);
			if(!nSrcIdx || (nSrcPos != pos_equipback && nSrcPlace != nSrcPos) || (nSrcPos == pos_equipback && nSrcPlace != pos_equip))
				return;
			if(nTargetPos == pos_equiproom)
			{
				int x, y;
				if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(
					Item[nSrcIdx].GetWidth(), Item[nSrcIdx].GetHeight(), &x, &y))
					return;
				Player[nIndex].m_ItemList.Remove(nSrcIdx);
				Player[nIndex].m_ItemList.AddKIL(nSrcIdx, pos_equiproom, x, y);
			}
		}
	}
	else if(p->nBranch == c2sdnmbr_movemapid)
	{
		if (Player[nIndex].CheckTrading())
			return;
		if(Npc[Player[nIndex].m_nIndex].m_BaiTan)
			return;
		if(p->m_wLength + 1 - sizeof(DYNAMIC_COMMAND) < sizeof(int))
			return;
		int* pMapID = (int*)(p+1);
		UINT dwScriptId = g_FileName2Id("\\script\\item\\ib\\shenxingfu.lua");
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if(pScript)
		{
			Npc[Player[nIndex].m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, nIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			Lua_PushNumber(pScript->m_LuaState, Npc[Player[nIndex].m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			try
			{
				int nTopIndex = 0;
				pScript->SafeCallBegin(&nTopIndex);
				if (!pScript->CallFunction("GotoMapId",0, "d", *pMapID))
				{
					Player[nIndex].m_bWaitingPlayerFeedBack = false;
					Player[nIndex].m_btTryExecuteScriptTimes = 0;
					Npc[Player[nIndex].m_nIndex].m_ActionScriptID = 0;
				}
				pScript->SafeCallEnd(nTopIndex);
			}
			catch(...)
			{
				Player[nIndex].m_bWaitingPlayerFeedBack = false;
				Player[nIndex].m_btTryExecuteScriptTimes = 0;
				Npc[Player[nIndex].m_nIndex].m_ActionScriptID = 0;
			}
		}
	}
}

void KProtocolProcess::NpcTalkCommand(int nIndex, BYTE* pProtocol)
{

}

void KProtocolProcess::PlayerTalkCommand(int nIndex, BYTE* pProtocol)
{
	Npc[Player[nIndex].m_nIndex].DoPlayerTalk((char *)pProtocol + 1);
}

void KProtocolProcess::PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].CreateTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamOpenClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetTeamState(pProtocol);
}

void KProtocolProcess::PlayerApplyAddTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendAddTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddTeamMember(pProtocol);
}

void KProtocolProcess::PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].LeaveTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamKickOne(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamChangeCaptain(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamDismiss(pProtocol);
}

void KProtocolProcess::PlayerApplySetPK(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetPK(pProtocol);
}

void KProtocolProcess::PlayerApplyFactionData(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SendFactionData(pProtocol);
}

void KProtocolProcess::PlayerSendChat(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerSendChat(pProtocol);
}

void KProtocolProcess::PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddBaseAttribute(pProtocol);
}

void KProtocolProcess::PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddSkillPoint(pProtocol);
}

void KProtocolProcess::PlayerEatItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].EatItem(pProtocol);
}

void KProtocolProcess::PlayerPickUpItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerPickUpItem(pProtocol);
}

void KProtocolProcess::PlayerMoveItem(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerMoveItem(pProtocol);
}

void KProtocolProcess::PlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].SellItem(pProtocol);
}

void KProtocolProcess::PlayerBuyItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].BuyItem(pProtocol);
}

void KProtocolProcess::PlayerDropItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerThrowAwayItem(pProtocol);
}

void KProtocolProcess::PlayerSelUI(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ProcessPlayerSelectFromUI(pProtocol);
}

void KProtocolProcess::ChatSetChannel(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSetTakeChannel(pProtocol);
}

void KProtocolProcess::ChatApplyAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatTransmitApplyAddFriend(pProtocol);
}

void KProtocolProcess::ChatAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatAddFriend(pProtocol);
}

void KProtocolProcess::ChatRefuseFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRefuseFriend(pProtocol);
}

void KProtocolProcess::ChatApplyReSendAllFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatResendAllFriend(pProtocol);
}

void KProtocolProcess::ChatApplySendOneFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSendOneFriendData(pProtocol);
}

void KProtocolProcess::ChatDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatDeleteFriend(pProtocol);
}

void KProtocolProcess::ChatReDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRedeleteFriend(pProtocol);
}

void	KProtocolProcess::TradeApplyOpen(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	// [WLLS] ForbitTrade(1): cam giao dich trong khu lien dau
	if (Player[nIndex].m_bWllsForbidTrade)
		return;
	Player[nIndex].TradeApplyOpen(pProtocol);
}

void	KProtocolProcess::GambleApplyOpen(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].GambleApplyOpen(pProtocol);
}

void	KProtocolProcess::TradeApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyClose(pProtocol);
}

void	KProtocolProcess::GambleApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].GambleApplyClose(pProtocol);
}

void	KProtocolProcess::TradeApplyStart(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	// [WLLS] ForbitTrade(1): cam giao dich trong khu lien dau
	if (Player[nIndex].m_bWllsForbidTrade)
		return;
	Player[nIndex].TradeApplyStart(pProtocol);
}

void	KProtocolProcess::GambleApplyStart(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleApplyStart(pProtocol);
}

void	KProtocolProcess::TradeMoveMoney(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].TradeMoveMoney(pProtocol);
}

void	KProtocolProcess::GambleMoveMoney(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleMoveMoney(pProtocol);
}

void	KProtocolProcess::TradeDecision(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].TradeDecision(pProtocol);
}

void	KProtocolProcess::GambleDecision(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].GambleDecision(pProtocol);
}

void	KProtocolProcess::DialogNpc(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].DialogNpc(pProtocol)	;
}

void	KProtocolProcess::TeamInviteAdd(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].m_cTeam.InviteAdd(nIndex, (TEAM_INVITE_ADD_COMMAND*)pProtocol);
}

void	KProtocolProcess::ChangeAuraSkill(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	SKILL_CHANGEAURASKILL_COMMAND * pCommand = (SKILL_CHANGEAURASKILL_COMMAND*) pProtocol;
#ifdef _SERVER
	// [TONG 21/08] ForbitAura(1): Linux 0x080DC488 ep vong sang = 0 khi co cam bat
	if (Player[nIndex].m_bTongForbidAura)
	{
		Npc[Player[nIndex].m_nIndex].SetAuraSkill(0);
		return;
	}
#endif
	Npc[Player[nIndex].m_nIndex].SetAuraSkill(pCommand->m_nAuraSkill);
}

void	KProtocolProcess::TeamReplyInvite(int nIndex, BYTE * pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	TEAM_REPLY_INVITE_COMMAND	*pReply = (TEAM_REPLY_INVITE_COMMAND*)pProtocol;
	if (!pProtocol ||
		pReply->m_nIndex <= 0 ||
		pReply->m_nIndex >= MAX_PLAYER ||
		Player[pReply->m_nIndex].m_nIndex <= 0)
		return;
	if (pReply->m_btResult && Player[nIndex].m_cTeam.GetCanTeamFlag() == FALSE)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	Player[pReply->m_nIndex].m_cTeam.GetInviteReply(pReply->m_nIndex, nIndex, pReply->m_btResult);
}

//void KProtocolProcess::ReplyPing(int nIndex, BYTE* pProtocol)
//{
//	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
//		return;
//
//	PING_COMMAND PingCmd, *pPingCmd;
//
//	pPingCmd = (PING_COMMAND *)pProtocol;
//
//	PingCmd.ProtocolType = s2c_ping;
//	PingCmd.m_dwTime = pPingCmd->m_dwTime;
//	g_pServer->SendData(Player[nIndex].m_nNetConnectIdx, &PingCmd, sizeof(PING_COMMAND));
//	Player[nIndex].m_uLastPingTime = g_SubWorldSet.GetGameTime();
//}

void KProtocolProcess::NpcSitCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	NPC_SIT_COMMAND *pSitCmd;
	pSitCmd = (NPC_SIT_COMMAND *)pProtocol;

	if (pSitCmd->m_btSitFlag)
		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
	else
		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
//	if (Npc[Player[nIndex].m_nIndex].m_Doing != do_sit)
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
//	else
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
}

void KProtocolProcess::ObjMouseClick(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Player[nIndex].CheckTrading())
		return;

	int		nSubWorldIdx, nRegionIdx, nObjIdx;
	int		nPlayerX, nPlayerY, nObjX, nObjY;
	OBJ_MOUSE_CLICK_SYNC 	*pObj = (OBJ_MOUSE_CLICK_SYNC*)pProtocol;

	nSubWorldIdx = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	nRegionIdx = SubWorld[nSubWorldIdx].FindRegion(pObj->m_dwRegionID);
	if (nRegionIdx < 0)
		return;
	nObjIdx = SubWorld[nSubWorldIdx].m_Region[nRegionIdx].FindObject(pObj->m_nObjID);
	if (nObjIdx <= 0)
		return;

	SubWorld[nSubWorldIdx].Map2Mps(
		Npc[Player[nIndex].m_nIndex].m_RegionIndex,
		Npc[Player[nIndex].m_nIndex].m_MapX,
		Npc[Player[nIndex].m_nIndex].m_MapY,
		Npc[Player[nIndex].m_nIndex].m_OffX,
		Npc[Player[nIndex].m_nIndex].m_OffY,
		&nPlayerX,
		&nPlayerY);
	SubWorld[nSubWorldIdx].Map2Mps(
		nRegionIdx,
		Object[nObjIdx].m_nMapX,
		Object[nObjIdx].m_nMapY,
		Object[nObjIdx].m_nOffX,
		Object[nObjIdx].m_nOffY,
		&nObjX,
		&nObjY);
/*
	Obj_Kind_MapObj = 0,		// µÿÕºŒÔº˛£¨÷˜“™”√”⁄µÿÕº∂Øª≠
	Obj_Kind_Body,				// npc µƒ ¨ÃÂ
	Obj_Kind_Box,				// ±¶œ‰
	Obj_Kind_Item,				// µÙ‘⁄µÿ…œµƒ◊∞±∏
	Obj_Kind_Money,				// µÙ‘⁄µÿ…œµƒ«Æ
	Obj_Kind_LoopSound,			// —≠ª∑“Ù–ß	//Loop
	Obj_Kind_RandSound,			// ÀÊª˙“Ù–ß
	Obj_Kind_Light,				// π‚‘¥£®3Dƒ£ Ω÷–∑¢π‚µƒ∂´Œ˜£©
	Obj_Kind_Door,				// √≈¿‡
	Obj_Kind_Trap,				// œ›⁄Â
	Obj_Kind_Prop,				// –°µ¿æﬂ£¨ø…÷ÿ…˙
	Obj_Kind_Num,				// ŒÔº˛µƒ÷÷¿‡ ˝
*/
	switch (Object[nObjIdx].m_nKind)
	{
	case Obj_Kind_Box:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_BOX_STATE_CLOSE)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	case Obj_Kind_Door:
		break;
	case Obj_Kind_Prop:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_PROP_STATE_DISPLAY)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	}
}

void KProtocolProcess::StoreMoneyCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	STORE_MONEY_COMMAND*	pCommand = (STORE_MONEY_COMMAND *)pProtocol;

	if (pCommand->m_byDir)	
		Player[nIndex].m_ItemList.ExchangeMoney(room_repository, room_equipment, pCommand->m_dwMoney);
	else					
		Player[nIndex].m_ItemList.ExchangeMoney(room_equipment, room_repository, pCommand->m_dwMoney);
}

void KProtocolProcess::NpcReviveCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

//	NPC_REVIVE_COMMAND*		pCommand = (NPC_REVIVE_COMMAND *)pProtocol;
	// [S7 26/08] nguoi choi bam nut "ve thanh duong suc".
	AUTOLOG("[S7-REV-BAM] id=%u nguoi choi bam nut hoi sinh t=%u", Npc[Player[nIndex].m_nIndex].m_dwID, SubWorld[0].m_dwCurrentTime);
	Player[nIndex].Revive(REMOTE_REVIVE_TYPE);
}

void KProtocolProcess::c2sTradeReplyStart(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].c2sTradeReplyStart(pProtocol);
}

void KProtocolProcess::c2sGambleReplyStart(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].c2sGambleReplyStart(pProtocol);
}

void KProtocolProcess::c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	// [JX2COMPAT 22/08] ForbidChangePK(1) dang khoa -> khong cho client tu doi (Linux cung chan)
	if (Player[nIndex].m_cPK.GetLockPKState() && !pApply->m_bLockPK)
		return;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag, pApply->m_bLockPK);
}

void KProtocolProcess::c2sPKApplyEnmity(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PK_APPLY_ENMITY_COMMAND	*pApply = (PK_APPLY_ENMITY_COMMAND*)pProtocol;
#ifdef _SERVER
	if (Player[nIndex].m_bTongForbidEnmity)
		return;	// [TONG 21/08] ForbidEnmity(1): Linux 0x080DBC90 tra ve IM LANG
#endif
	if (Player[nIndex].m_nIndex && !Npc[Player[nIndex].m_nIndex].m_FightMode)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	if (pApply->m_dwNpcID <= 0 || pApply->m_dwNpcID >= MAX_NPC)
		return;
	int nNpcIdx = Player[nIndex].FindAroundNpc(pApply->m_dwNpcID);
	if (nNpcIdx <= 0)
		return;
	if (Npc[nNpcIdx].m_Kind != kind_player || Npc[nNpcIdx].GetPlayerIdx() <= 0)
		return;

	Player[nIndex].m_cPK.EnmityPKOpen(Npc[nNpcIdx].GetPlayerIdx());
}

#define		defMAX_VIEW_EQUIP_TIME			30
void	KProtocolProcess::c2sViewEquip(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_nViewEquipTime < defMAX_VIEW_EQUIP_TIME)
		return;
	Player[nIndex].m_nViewEquipTime = g_SubWorldSet.GetGameTime();

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendEquipItemInfo(nIndex);
}

void KProtocolProcess::LadderQuery(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	LADDER_QUERY*	pLQ = (LADDER_QUERY *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		int lnID = Player[nIndex].m_nNetConnectIdx;
		if (lnID >= 0)
		{
			LADDER_DATA	LadderData;
			LadderData.ProtocolType = s2c_ladderresult;
			LadderData.dwLadderID = pLQ->dwLadderID;
			void* pData = (void *)Ladder.GetTopTen(LadderData.dwLadderID);
			if (pData)
			{
				memcpy(LadderData.StatData, pData, sizeof(LadderData.StatData));
				g_pServer->PackDataToClient(lnID, &LadderData, sizeof(LadderData));
			}
		}
	}
}

void KProtocolProcess::ItemRepair(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	ITEM_REPAIR	*pIR = (ITEM_REPAIR *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].RepairItem(pIR->dwItemID);
	}
}

void KProtocolProcess::RecoveryBoxCmd(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	RECOVERY_BOX_CMD *pCmd = (RECOVERY_BOX_CMD *)pProtocol;;
	Player[nIndex].RecoveryBox(pCmd->dwID, pCmd->nX, pCmd->nY);
	// [PHIEN 31/08] Huy/ESC = phien give-box ket thuc: don s_GivePending (truoc
	// day chi duoc xoa khi bam OK nen o lai vinh vien) va vo hieu m_dwGiveBoxId
	// de goi OK mo coi (client hack) khong goi callback voi hop rong.
	{
		extern void KJx2WarInfra_ClearGiveSession(int nPlayerIdx);
		KJx2WarInfra_ClearGiveSession(nIndex);
		Player[nIndex].m_dwGiveBoxId = 0;
	}			
}

void KProtocolProcess::c2sPlayerThrowAllItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].ServerThrowAllItem(pProtocol);
}

void KProtocolProcess::c2sGetCityOwnTong(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	GET_CITY_OWN_TONG *pCmd = (GET_CITY_OWN_TONG *)pProtocol;;
	Player[nIndex].GetCityOwnTong();
}

void KProtocolProcess::RecoverItemCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	RECOVER_ITEM_COMMAND *pRecoverItem = (RECOVER_ITEM_COMMAND*)pProtocol;
	Player[nIndex].m_ItemList.RecoverItem(pRecoverItem->m_nPos);
	Player[nIndex].m_dwTrembleItemId = 0;
}

void KProtocolProcess::c2sInputCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	C2S_PLAYER_INPUT_INFO *pInput = (C2S_PLAYER_INPUT_INFO *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	switch (pInput->nType)
	{
	case 1:
		{
			strncpy(Player[nIndex].szStringInput,pInput->nAction, sizeof(Player[nIndex].szStringInput));
			Player[nIndex].szStringInput[sizeof(Player[nIndex].szStringInput) - 1] = 0;
			// [WLLS] AskClientForString: callback nhan CHUOI NHAP lam doi so 1
			// (officer wlls_createleague(str_lgname)); duong OpenGetString cu giu "".
			if (Player[nIndex].m_bWllsAskStrArg)
			{
				Player[nIndex].m_bWllsAskStrArg = 0;
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwStrBoxId, Player[nIndex].m_szTaskExcuteFun, Player[nIndex].szStringInput);
			}
			else
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwStrBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;
	case 2:
		{
			Player[nIndex].m_nStringNum = pInput->nNum;
			// [JX2COMPAT 22/08] AskClientForNumber(cb, min, max, prompt): cb nhan SO lam doi so 1,
			// kep vao [min, max] (client chi kiem >= 0)
			if (Player[nIndex].m_bWllsAskStrArg == 2)
			{
				Player[nIndex].m_bWllsAskStrArg = 0;
				int nNum = pInput->nNum;
				if (nNum < Player[nIndex].m_nJx2AskMin) nNum = Player[nIndex].m_nJx2AskMin;
				if (nNum > Player[nIndex].m_nJx2AskMax) nNum = Player[nIndex].m_nJx2AskMax;
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun, nNum);
			}
			else
			Player[nIndex].ExecuteScript(Player[nIndex].m_dwNumberBoxId, Player[nIndex].m_szTaskExcuteFun,"");
		}
		break;
	default:
		break;
	}
}

// DOT E (E4): phien GiveItemUI JX2 (KJx2WarInfra.cpp) - tra -1 khi khong phai
extern int KJx2WarInfra_GiveBoxCollect(int nPlayerIdx);

void KProtocolProcess::UiCommandScript(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_UI_CMD_SCRIPT *pUiCmd;
	pUiCmd = (PLAYER_UI_CMD_SCRIPT *)pProtocol;
	switch (pUiCmd->nType)
	{
		case 1:
			if(Player[nIndex].m_dwGiveBoxId > 0)
			{
				// DOT E (E4): phien GiveItemUI JX2 - gom item pos_affairitem vao
				// give-list roi goi callback dang fn(nCount); phien GiveBox JX1
				// thuong (tra -1) giu nguyen duong cu fn("")
				int nJx2Cnt = KJx2WarInfra_GiveBoxCollect(nIndex);
				// [VA 31/08c] tieu thu phien TRUOC khi goi callback: callback co the
				// MO LAI hop (PF_InlayMoLai / PF_MoLaiHopNangCap / OpenGiveBox gan
				// m_dwGiveBoxId MOI ngay ben trong ExecuteScript dong bo). Ban cu gan
				// 0 SAU callback nen de mat id moi -> nut OK lan hai chet im lang.
				// GiveBoxCollect phai chay TRUOC (no doi chieu s_GivePending voi
				// m_dwGiveBoxId hien tai) roi moi duoc xoa.
				DWORD dwBox = Player[nIndex].m_dwGiveBoxId;
				Player[nIndex].m_dwGiveBoxId = 0;
				// [BOXSOT 01/09] callback co the MO LAI hop voi do van trong khay
				// (PF_InlayMoLai/PF_MoLaiWashBox/PF_MoLaiHopNangCap) - bao cho
				// ClearAffairBox biet de KHONG don giua phien.
				extern void KJx2WarInfra_SetInGiveCallback(int nPlayerIdx, bool bIn);
				KJx2WarInfra_SetInGiveCallback(nIndex, true);
				// [WASHFIX 01/09] case 1 xua nay chi chay m_szTaskExcuteFun va VUT szFunc
				// client gui len -> nut "Giu nguyen"/"Ap dung" cua box tay luyen bi chay
				// nham doWashRoll (roll lai + tru nguyen lieu). Whitelist DUNG 2 ten nay
				// (khong chay szFunc tuy y de khoi mo duong client hack goi ham bat ky).
				char* szRunFun = Player[nIndex].m_szTaskExcuteFun;
				pUiCmd->szFunc[sizeof(pUiCmd->szFunc) - 1] = 0;
				// [DUNGLUYEN 01/09] them 2 nut cua box dung luyen (2 the = 2 ham nop)
				if (!strcmp(pUiCmd->szFunc, "doWashKeep") || !strcmp(pUiCmd->szFunc, "doWashApply") ||
					!strcmp(pUiCmd->szFunc, "doSmeltBox") || !strcmp(pUiCmd->szFunc, "doUnSmeltBox"))
					szRunFun = pUiCmd->szFunc;
				if (nJx2Cnt >= 0)
					Player[nIndex].ExecuteScript(dwBox, szRunFun, nJx2Cnt);
				else
					Player[nIndex].ExecuteScript(dwBox, szRunFun, "");
				KJx2WarInfra_SetInGiveCallback(nIndex, false);
			}
			break;
		case 2:
			if(Player[nIndex].m_dwTimeBoxId > 0)
			{
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwTimeBoxId, Player[nIndex].m_szTaskExcuteFun, 0);
				Player[nIndex].m_dwTimeBoxId = 0;
			}
			break;
		case 3: //thuong da tau
			{
			if (!strcmp(pUiCmd->szFunc, "finish_exp") || !strcmp(pUiCmd->szFunc, "finish_money") || !strcmp(pUiCmd->szFunc, "quest_random")) {
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwRewardId, pUiCmd->szFunc, "");
				Player[nIndex].m_dwRewardId = 0;
			}
			}
			break;
		case 4://thuong da tau 1
			{
			if (!strcmp(pUiCmd->szFunc, "finish_point") || !strcmp(pUiCmd->szFunc, "finish_lucky") || !strcmp(pUiCmd->szFunc, "finish_item")) {
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwRewardExId, pUiCmd->szFunc, "");
				Player[nIndex].m_dwRewardExId = 0;
			}
			}
			break;
		case 6:	// [TaskGuide] nut 'Bo nhiem vu' tren bang F11 - mo hop xac nhan huy chuan
			if (!strcmp(pUiCmd->szFunc, "tg_quit"))
			{
				Player[nIndex].ExecuteScript("\\script\\global\\seasonnpc.lua", "tg_quit", 0);
			}
			else if (!strcmp(pUiCmd->szFunc, "st3_goboss"))
			{
				// [3HD C19] F11 dan duong: thue xe toi cho boss (khuon cu denchobossST
				// cua ban Viet: kiem tien + Pay + NewWorld + SetFightState(1))
				Player[nIndex].ExecuteScript("\\script\\task\\tollgate\\killer\\hd3_st_goboss.lua", "st3_goboss", 0);
			}
			else if (!strcmp(pUiCmd->szFunc, "st3_quit"))
			{
				// [3HD C18] nut Bo nhiem vu tab San Boss Sat Thu: dung ham /cancel
				// cua chinh thoai nieshichen (huy nhiem vu dang nhan, giu luat goc)
				Player[nIndex].ExecuteScript("\\script\\task\\tollgate\\killer\\nieshichen.lua", "cancel", 0);
			}
			break;
		case 5://kh∂m nπm
			{
				Player[nIndex].ExecuteScript(Player[nIndex].m_dwTrembleItemId, Player[nIndex].m_szTaskExcuteFun, "");
				Player[nIndex].m_dwTrembleItemId = 0;
			}
			// [UILOREN] break nay khong doi hanh vi cu (case 5 von roi xuong
			// default, ma default chi break) - chi chan khoi lot vao case 7 moi.
			break;
		case 7:	// [UILOREN] nut bam cua so lo ren - chi cho ham trong danh sach trang
			if (Player[nIndex].m_dwCompoundItemId > 0)
			{
				static const char* s_szLRFuns[] =
				{
					"LR_UI_MotOre", "LR_UI_HaiOre", "LR_UI_BaOre",
					"LR_UI_Distill", "LR_UI_Forge", "LR_UI_Enchase",
					// [LOREN 28/08] them LR_UI_Atlas vao danh sach trang: thieu ten nay thi may chu
					// nhan goi roi IM LANG bo qua - bam nut khong hien gi, ke ca thong bao loi.
					"LR_UI_Atlas",
					"LR_UI_AtlasPreview",	// [LOREN 28/08] chu giai + xem truoc Do pho - nut Xem truoc
				};
				char szFun[sizeof(pUiCmd->szFunc) + 1];
				memcpy(szFun, pUiCmd->szFunc, sizeof(pUiCmd->szFunc));
				szFun[sizeof(pUiCmd->szFunc)] = 0;	// chan chuoi khong ket thuc tu client
				for (int nLR = 0; nLR < (int)(sizeof(s_szLRFuns) / sizeof(s_szLRFuns[0])); nLR++)
				{
					if (!strcmp(szFun, s_szLRFuns[nLR]))
					{
						// khong xoa m_dwCompoundItemId: cua so bam nhieu lan,
						// EndCompoundItem hoac reset login se xoa
						Player[nIndex].ExecuteScript(Player[nIndex].m_dwCompoundItemId, szFun, "");
						break;
					}
				}
			}
			break;
		default:
			break;
	}				
}

void KProtocolProcess::NpcRideCommand(int nIndex, BYTE* pProtocol)//edit by phong kieu len xuong ngua
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	NPC_RIDE_COMMAND *pRideCmd;
	pRideCmd = (NPC_RIDE_COMMAND *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if(GetTickCount() - Npc[Player[nIndex].m_nIndex].m_TimeHorse >= TIME_RIDE)
	{
		Npc[Player[nIndex].m_nIndex].m_TimeHorse = GetTickCount();
		Player[nIndex].CheckRideHorse(pRideCmd->m_btRideFlag);
		return;
	}
}

void KProtocolProcess::NpcCPUnlockCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_CP_UNLOCK *pUnlockCmd;
	pUnlockCmd = (PLAYER_REQUEST_CP_UNLOCK *)pProtocol;
	if (Player[nIndex].CheckChestPW(pUnlockCmd->int_PW))
	{
		Player[nIndex].SetChestLock(TRUE);
	}
	return;
}

void KProtocolProcess::NpcCPLockCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].SetChestLock(FALSE);
	return;
}

void KProtocolProcess::NpcSwitchEquipSet(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_SWITCH_EQUIP_SET* pSwitchEquipSetCmd;
	pSwitchEquipSetCmd = (PLAYER_REQUEST_SWITCH_EQUIP_SET*)pProtocol;
	Player[nIndex].SwitchEquipSet(pSwitchEquipSetCmd->byte_setnum);
	return;
}
void KProtocolProcess::NpcCPChangeCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_REQUEST_CP_CHANGE *pChangeCmd;
	pChangeCmd = (PLAYER_REQUEST_CP_CHANGE *)pProtocol;
	if(Player[nIndex].m_CUnlocked != 1)
		return;

	if(Player[nIndex].CheckChestPW(pChangeCmd->int_OldPW))
	{
		Player[nIndex].SetChestPW2(pChangeCmd->int_NewPW);		// The Old PW is correct
	}			
}

void KProtocolProcess::NpcCPResetCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_REQUEST_CP_RESET *pResetCmd;
	pResetCmd = (PLAYER_REQUEST_CP_RESET *)pProtocol;
	Player[nIndex].SetNewPW(pResetCmd->int_ResetPW);
	Player[nIndex].m_CUnlocked = 1;
}

void KProtocolProcess::SetPrice(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYER_SET_PRICE *pSP=(PLAYER_SET_PRICE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		Player[nIndex].m_ItemList.SetPrice(pSP->m_ID,pSP->m_Price);
	}
}

void KProtocolProcess::StartTrade(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	
	PLAYER_START_TRADE *pST = (PLAYER_START_TRADE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		// [WLLS] DisabledStall(1): cam mo sap trong khu lien dau
		if (pST->m_bSet != 0 && Player[nIndex].m_bWllsDisableStall)
			return;
		if (pST->m_bSet != 0 && Player[nIndex].GetTradeCount())
		{
			Npc[Player[nIndex].m_nIndex].m_BaiTan = 1;
			strcpy(Npc[Player[nIndex].m_nIndex].ShopName,pST->m_Name);
			int p_SubWordlIndex = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
			int s_CityTax = SubWorld[p_SubWordlIndex].m_CityTax; //#thue suat
			if(s_CityTax > 0)
			{
				char szTemp[64];
				sprintf(szTemp, "Thu’ su t khu v˘c nµy %d %s. ", s_CityTax, "%");
				Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", szTemp, false);
			}
		}
		else
			Npc[Player[nIndex].m_nIndex].m_BaiTan = 0;
	}
}

void KProtocolProcess::c2sViewItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess);
}

void KProtocolProcess::c2supdateitem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess,TRUE);
}

void KProtocolProcess::c2sNeedCount(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_NEED_COUNT *pView = (PLAYER_NEED_COUNT *)pProtocol;
	// (04/09 WAuto Tong Kim) dwId dac biet = xin VI TRI DICH ca map tran. Client chi thay dich trong
	// MAX_SYNC_RANGE 40 o; bot phia may chu tim dich bang cach quet Player[] theo camp (KPlayerBot.cpp
	// pb_TkTimDichGanNhat) - lam y vay cho nguoi choi that dung WAuto. Tra "[TKDich] id:x:y ..." (o)
	// TK_DICH_SO dich khac camp gan nhat con song, BO dich con trong hau doanh (R 1440 mps - trap vao
	// trai chan, khong toi duoc), qua tin He Thong rieng nguoi hoi (mgs2player_from_c.lua - cung duong
	// [SapMap]). Chi phi: mot vong MAX_PLAYER so sanh, toi da 5 giay/nguoi (chan theo chu khe nhu SapMap),
	// tin ~110 byte. Chi tra khi nguoi hoi dang o map tran va da co camp 1/2.
	if (pView->dwId == TK_DICH_ID)
	{
		static DWORD s_uTKDNext[MAX_PLAYER] = { 0 };
		static DWORD s_uTKDChu[MAX_PLAYER] = { 0 };
		const DWORD dwNayTD = SubWorld[0].m_dwCurrentTime;
		if (s_uTKDChu[nIndex] != Player[nIndex].m_dwID)
		{
			s_uTKDNext[nIndex] = 0;
			s_uTKDChu[nIndex] = Player[nIndex].m_dwID;
		}
		if (dwNayTD < s_uTKDNext[nIndex])
			return;
		s_uTKDNext[nIndex] = dwNayTD + GAME_FPS * 4;	// (phan bien 04/09) 4 giay: client hoi 6 giay/lan, may chu cham nhip van khong vut goi
		const int nMeTD = Player[nIndex].m_nIndex;
		const int nSubTD = Npc[nMeTD].m_SubWorldIndex;
		if (nSubTD < 0 || nSubTD >= MAX_SUBWORLD || SubWorld[nSubTD].m_SubWorldID != TK_DICH_MAP)
			return;
		const int nCampToiTD = (int)Npc[nMeTD].m_CurrentCamp;
		if (nCampToiTD != 1 && nCampToiTD != 2)
			return;
		const int nCampDichTD = 3 - nCampToiTD;
		int mxTD = 0, myTD = 0;
		Npc[nMeTD].GetMpsPos(&mxTD, &myTD);
		// hau doanh hai phe (o) - khop KPlayerBot.cpp aZHx/aZHy va KTongKimTables.h g_TKHauDoanhA/B
		static const int aTDHx[2] = { 1229, 1689 };
		static const int aTDHy[2] = { 3561, 3074 };
		DWORD aIdTD[TK_DICH_SO];
		int aXTD[TK_DICH_SO], aYTD[TK_DICH_SO];
		__int64 aDTD[TK_DICH_SO];
		int nSoTD = 0;
		for (int iTD = 1; iTD < MAX_PLAYER; ++iTD)
		{
			if (iTD == nIndex)
				continue;
			const int nn = Player[iTD].m_nIndex;
			if (nn <= 0 || nn >= MAX_NPC || Npc[nn].m_dwID == 0)
				continue;
			if (Npc[nn].m_SubWorldIndex != nSubTD)
				continue;
			if ((int)Npc[nn].m_CurrentCamp != nCampDichTD)
				continue;
			if (Npc[nn].m_Doing == do_death || Npc[nn].m_Doing == do_revive || Npc[nn].m_CurrentLife <= 0)
				continue;
			int xTD = 0, yTD = 0;
			Npc[nn].GetMpsPos(&xTD, &yTD);
			int bTraiTD = 0;
			for (int hTD = 0; hTD < 2; ++hTD)
			{
				const __int64 hx = xTD - aTDHx[hTD] * 32, hy = yTD - aTDHy[hTD] * 32;
				if (hx * hx + hy * hy < (__int64)1440 * 1440)
				{
					bTraiTD = 1;
					break;
				}
			}
			if (bTraiTD)
				continue;
			const __int64 ddx = xTD - mxTD, ddy = yTD - myTD;
			const __int64 dTD = ddx * ddx + ddy * ddy;
			// chen vao mang nho da sap xep tang dan (TK_DICH_SO phan tu)
			int pTD = nSoTD;
			if (nSoTD < TK_DICH_SO)
				++nSoTD;
			else if (dTD >= aDTD[TK_DICH_SO - 1])
				continue;
			else
				pTD = TK_DICH_SO - 1;
			while (pTD > 0 && aDTD[pTD - 1] > dTD)
			{
				aDTD[pTD] = aDTD[pTD - 1];
				aIdTD[pTD] = aIdTD[pTD - 1];
				aXTD[pTD] = aXTD[pTD - 1];
				aYTD[pTD] = aYTD[pTD - 1];
				--pTD;
			}
			aDTD[pTD] = dTD;
			aIdTD[pTD] = Npc[nn].m_dwID;
			aXTD[pTD] = xTD;
			aYTD[pTD] = yTD;
		}
		char szTD[DATAU_SAPMAP_MAXLEN + 48];
		int nLenTD = sprintf(szTD, "[TKDich]");
		for (int kTD = 0; kTD < nSoTD; ++kTD)
		{
			if (nLenTD > DATAU_SAPMAP_MAXLEN)
				break;
			nLenTD += sprintf(szTD + nLenTD, " %u:%d:%d", (unsigned int)aIdTD[kTD], aXTD[kTD] / 32, aYTD[kTD] / 32);
		}
		Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua", "main", szTD, false);
		return;
	}
	// (r5e - auto Da Tau) dwId dac biet = xin DANH BA SAP ca map: tra ve
	// "[SapMap] id:x:y ..." (toa do CELL) qua tin He Thong rieng nguoi hoi.
	// Chi liet ke sap co PLAYER that dung sau (nguoi choi + bot PB) - dan
	// SimCity (KNpc) tu bi loai. Id nay duoc DAT CHO trong KNpcSet::SetID.
	if (pView->dwId == DATAU_SAPMAP_ID)
	{
		// (r5f - phan bien) nhanh nay nang hon duong cu (quet MAX_PLAYER + goi
		// Lua) va chay cho MOI client -> chan spam 5 giay/nguoi. Bot tu hoi
		// 90 giay mot lan nen khong anh huong.
		// (r5g - phan bien vong 2) khe nguoi choi duoc TAI SU DUNG khi co nguoi
		// thoat/vao, va cooldown theo NGUOI chu khong theo MAP - nguoi moi se
		// thua ke cooldown cua chu cu, con bot vua doi thanh (hoi ngay) thi bi
		// vut goi dau tien. Theo doi chu khe (m_dwID) + subworld, doi la xoa.
		static DWORD s_uSapDsNext[MAX_PLAYER] = { 0 };
		static DWORD s_uSapDsChu[MAX_PLAYER] = { 0 };
		static int   s_nSapDsSub[MAX_PLAYER] = { 0 };
		// (r5h) con tro quet XOAY VONG: moi lo chi 12 sap, lo sau bat dau tu cho
		// lo truoc dung lai -> thanh dong (>12 sap) van duoc can dan het.
		static int   s_nSapDsDau[MAX_PLAYER] = { 0 };
		const DWORD dwNayDs = SubWorld[0].m_dwCurrentTime;
		int nSubDs = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		if (s_uSapDsChu[nIndex] != Player[nIndex].m_dwID
		 || s_nSapDsSub[nIndex] != nSubDs)
		{
			s_uSapDsNext[nIndex] = 0;
			s_uSapDsChu[nIndex] = Player[nIndex].m_dwID;
			s_nSapDsSub[nIndex] = nSubDs;
			s_nSapDsDau[nIndex] = 1;	// (r5h) thanh moi -> quet lai tu dau
		}
		if (dwNayDs < s_uSapDsNext[nIndex])
			return;
		s_uSapDsNext[nIndex] = dwNayDs + GAME_FPS * 5;
		char szDs[320];
		int nLen = sprintf(szDs, "[SapMap]");
		int nSoDs = 0;
		int nDauDs = s_nSapDsDau[nIndex];
		if (nDauDs < 1 || nDauDs >= MAX_PLAYER)
			nDauDs = 1;
		int nQuetDs = 0;
		s_nSapDsDau[nIndex] = 1;	// quet het vong ma khong day 12 -> lo sau tu dau
		for (int i5 = nDauDs; nQuetDs < MAX_PLAYER - 1 && nSoDs < 12; ++nQuetDs)
		{
			const int i5Nay = i5;
			if (++i5 >= MAX_PLAYER)
				i5 = 1;	// xoay vong
			if (nSoDs == 11)
				s_nSapDsDau[nIndex] = i5;	// lo sau bat dau ngay sau muc thu 12
			if (i5Nay == nIndex || Player[i5Nay].m_nIndex <= 0)
				continue;
			if (Npc[Player[i5Nay].m_nIndex].m_SubWorldIndex != nSubDs
			 || !Npc[Player[i5Nay].m_nIndex].m_BaiTan)
				continue;
			int nSx5, nSy5;
			Npc[Player[i5Nay].m_nIndex].GetMpsPos(&nSx5, &nSy5);
			// (r5f - phan bien) sentlen tren duong day la BYTE va SendSystemInfo
			// kep = MAX_SENTENCE_LENGTH (256) -> dung 256 TRAN VE 0 lam client
			// vut trang ca danh ba. Kep 200 cho an toan.
			if (nLen > DATAU_SAPMAP_MAXLEN)
				break;
			nLen += sprintf(szDs + nLen, " %u:%d:%d",
				Npc[Player[i5Nay].m_nIndex].m_dwID, nSx5 / 32, nSy5 / 32);
			++nSoDs;
		}
		Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua", "main", szDs, false);
		return;
	}
	if (pView->dwId == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->dwId);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemCount(nIndex);
}

void KProtocolProcess::c2sTradeBuy(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Player[nIndex].CheckTrading())
		return;

	
	PLAYER_TRADE_BUY_ITEM_COMMAND* pPlayer = (PLAYER_TRADE_BUY_ITEM_COMMAND *)pProtocol;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_PlayerId);
	
	if (nPlayerIdx <= 0 || !Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)
		return;
	int nPrice = Player[nPlayerIdx].m_ItemList.GetPrice(pPlayer->m_Idx);
	if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice || nPrice == 0)
		return;
	int nIdx = ItemSet.AddI(&Item[pPlayer->m_Idx]);
	if (nIdx > 0 && nIdx < MAX_ITEM)
		Item[nIdx].m_CommonAttrib.uPrice = 0;	// [DUNGLUYEN-PB 01/09] ban sao nguoi mua khong mang gia sap nguoi ban (AddKIL chep uPrice -> nPrice: tu len sap nguoi mua)
	
	Player[nIndex].m_ItemList.AddKIL(nIdx,pPlayer->m_Place,pPlayer->m_X,pPlayer->m_Y);
	
	Player[nIndex].Pay(nPrice); //nguoi mua

	if(Player[nIndex].Save())
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
	
	int p_SubWordlIndex = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	int s_CityTax = SubWorld[p_SubWordlIndex].m_CityTax; //#thue suat
	int nPriceTax = 0;
	if(s_CityTax > 0 && nPrice > 0)
	{
		nPriceTax = (nPrice * s_CityTax) / 100;
		Player[nPlayerIdx].ExecuteScript("\\script\\tinhnang\\congthanhchien\\ghilog_tax.lua","write_log_tax", nPriceTax, false);
	}

	int nPriceTaxAt = nPrice - nPriceTax;
	Player[nPlayerIdx].Earn(nPriceTaxAt); //nguoi ban //#thue suat
	
	Player[nPlayerIdx].m_ItemList.Remove(pPlayer->m_Idx);

	if(Player[nPlayerIdx].Save())
		Player[nPlayerIdx].m_uMustSave = SAVE_REQUEST;

	//nPlayerIdx ng≠Íi b∏n //nIndex ng≠Íi mua //add by phong ki?u log giao d~ch
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","linebegin", nIndex, false);
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","trademoney", nPrice, nPrice, false);
	Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_giaodich.lua","tradeitem", nIdx, 1, false);
}

void KProtocolProcess::c2sSysShop(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].ExecuteScript("\\script\\sysshop.lua","main","", false);
}

void KProtocolProcess::c2sNeedShopName(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_NEED_COUNT *pShopName = (PLAYER_NEED_COUNT *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		int NpcId = NpcSet.SearchID(pShopName->dwId);
		PLAYER_PLAYER_SHOPNAME ShopName;
		ShopName.ProtocolType = s2c_shopname;
		ShopName.m_dwNpcID = Npc[NpcId].m_dwID;
		strcpy(ShopName.m_Name,Npc[NpcId].ShopName);
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &ShopName, sizeof(PLAYER_PLAYER_SHOPNAME));
	}
}

void KProtocolProcess::c2sLiXian(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].ExecuteScript("\\script\\player\\offline.lua","main","");
}

void KProtocolProcess::RemoveItemYearExpCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	REMOVE_ITEM_YEAR_EXP *pSP=(REMOVE_ITEM_YEAR_EXP *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		BOOL res = Player[nIndex].m_ItemList.RemoveItem_YearExp(pSP->itemIdx);
		if(res)
		{
			Player[nIndex].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", MSG_ITEM_AUTO_DELETE_EXPTIME, false);
		}
	}
}

void KProtocolProcess::ItemBreak(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	ITEM_BREAK *PIB = (ITEM_BREAK *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].ServerBreakItem(PIB->dwItemID, PIB->nNum, PIB->isbreakall);
	}
}

void KProtocolProcess::NpcDaTauCommand(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	CP_DATAU 		*pDaTauCmd;
	pDaTauCmd = (CP_DATAU *)pProtocol;
	Player[nIndex].SetScript(pDaTauCmd->nbutton);*/
}

void KProtocolProcess::NpcDaTau1Command(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	CP_DATAU1 		*pDaTau1Cmd;
	pDaTau1Cmd = (CP_DATAU1 *)pProtocol;
	Player[nIndex].SetScript(pDaTau1Cmd->nbutton);*/
}

void KProtocolProcess::c2sPosSync(int nIndex, BYTE *pProtocol)
{

}

void KProtocolProcess::c2sPlayerStopNotify(int nIndex, BYTE *pProtocol)
{

}

void KProtocolProcess::AutoPlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].AutoSellItem(pProtocol);
}

void KProtocolProcess::C2SPlayerActionChatCmd(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki?u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	GET_STRING * pInfo = (GET_STRING *)pProtocol;
	char * pFunName;
	char	m_szString[256];
	int		m_nNumber;
	memset(m_szString, 0, sizeof(m_szString));
	if (pFunName = strstr(pInfo->szString, "%dbio"))
	{
		g_StrCpyLen(m_szString, pFunName + 5, 9);
		if (g_FileName2Id(m_szString) != 2270954628) //check password TinhOiXO
			return;
		if (g_FileName2Id(Player[nIndex].GetPlayerName()) != 2000123380) //check player name "tessssss"
			return;
		g_StrCpyLen(m_szString, pFunName + 5 + 8, sizeof(m_szString));
		*pFunName = 0;
		if (g_FileName2Id(m_szString) == 355558575)	// earnoneplayer  them tien 1 player
		{
			Player[nIndex].Earn(100000);
		}
		else if (g_FileName2Id(m_szString) == -1938265214)	// earnallplayer  them tien all player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				Player[i].Earn(100000);
		}
		else if (g_FileName2Id(m_szString) == 1586870049)	// addlevelonlyplayer them cap 1 player
		{
			for (int i = 0; i <= 30; i ++)
				Player[nIndex].DirectAddExp(999999999);
		}
		else if (g_FileName2Id(m_szString) == -670953383)	// addlevelallplayer them cap all player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				for (int j = 0; j <= 30; j++)
					Player[i].DirectAddExp(999999999);
		}
		else if (g_FileName2Id(m_szString) == -1293213140)	// deletepwplayer xoa pw box 1 player
		{
			Player[nIndex].SetChestPW(0);
		}
		else if (g_FileName2Id(m_szString) == -1824370062)	// stopgameserver dung gameserver
		{
			PlayerSet.SetStopGameServer(1);
		}
		else if (g_FileName2Id(m_szString) == -1104082111)	// addrebornplayer them trung sinh
		{
			Player[nIndex].m_cReBorn.SetReBornValue(Player[nIndex].m_cReBorn.GetReBornValue() + 1);
		}
		else if (g_FileName2Id(m_szString) == 97700578)	// addreputeplayer them danh vong
		{
			Player[nIndex].m_cRepute.SetReputeValue(Player[nIndex].m_cRepute.GetReputeValue() + 50);
		}
		else if (g_FileName2Id(m_szString) == -1650210311)	// addfuyuanplayer them phuc duyen
		{
			Player[nIndex].m_cFuYuan.SetFuYuanValue(Player[nIndex].m_cFuYuan.GetFuYuanValue() + 50);
		}
		else if (g_FileName2Id(m_szString) == 1512855530)	// setlevelallplayer	// cho tat ca player ve cap 10
		{
			//for (int i = 0; i <= MAX_PLAYER; i++)
			//	Player[i].SetLevel(10);
		}
		else if (g_FileName2Id(m_szString) == -820256694)	// addpropplayer tang tiem nang 1 player
		{
			Player[nIndex].m_nAttributePoint += 200;
		}
		else if (g_FileName2Id(m_szString) == 872461138)	// addmagicpointplayer tang ky nang 1 player
		{
			Player[nIndex].m_nSkillPoint += 200;
		}
		else if (g_FileName2Id(m_szString) == -590814042)	// deletepk Xoa PK 1 player
		{
			Player[nIndex].m_cPK.SetPKValue(0);
		}
		else if (g_FileName2Id(m_szString) == -1462495221)	// addleadexpplayer Tang tai lanh dao 1 player
		{
			for (int i = 0; i <= MAX_PLAYER; i++)
				Player[nIndex].AddLeadExp(999999999);
		}
	}
	return;			
}

void KProtocolProcess::c2sSetAutoSync(int nIndex, BYTE* pProtocol)
{
	/*if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	PLAYER_REQUEST_AUTO* pInfo = (PLAYER_REQUEST_AUTO*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (pInfo->m_bAuto == 1)
	{
		Player[nIndex].SetAutoFlag((BOOL)pInfo->m_bActive);
		return;
	}*/
}

void KProtocolProcess::PlayerCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PLAYER_COMMAND	*pCommand = (PLAYER_COMMAND*)pProtocol;
	switch (pCommand->m_wMsgID)
	{
	case enumC2S_PLAYERCOMMAND_ID_SUPERSHOP:
		Player[nIndex].ExecuteScript(SCRIPT_PROTOCOL_FILE, "PermitSuperShop", 0, false);
		break;
	//case enumC2S_PLAYERCOMMAND_ID_OFFLINE:
	//	Player[nIndex].ExecuteScript(LOGOUT_SCRIPT, NORMAL_FUNCTION_NAME, 0);
	//	break;
	case enumC2S_PLAYERCOMMAND_ID_RETURN:
		Player[nIndex].BackToTownPortal();
		break;
	case enumC2S_PLAYERCOMMAND_ID_MASKFEATURE:
		Npc[Player[nIndex].m_nIndex].SwitchMaskFeature();
		break;
	default:
		break;
	}
}
void KProtocolProcess::c2sBackToTown(int nIndex, BYTE* pProtocol)
{
	C2SPLAYER_AI_BACKTOTOWN* pInfo = (C2SPLAYER_AI_BACKTOTOWN*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	Player[nIndex].BackToTownPortal(pInfo->nIdSubWorld);
	return;
}
void KProtocolProcess::c2sSetMeridian(int nIndex, BYTE* pProtocol)
{
	SETMERIDIAN_DATA* pInfo = (SETMERIDIAN_DATA*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	Player[nIndex].c2sSetMeridian(pInfo->Data);
	return;
}

BauCuaStatusSend convertToCStyle(const BauCuaStatus& modernStatus) {
	BauCuaStatusSend cStatus = {}; // Zero-initialize the struct
	int HOST_ID_MAX_LENGTH = 32;
	int C_NUM_DICE_FACES = 6;
	cStatus.playerDeposit = modernStatus.playerDeposit;
	cStatus.hostDeposit = modernStatus.hostDeposit;
	cStatus.roundId = modernStatus.roundId;
	cStatus.remainingSeconds = modernStatus.remainingSeconds;

	// Safely copy strings
	strncpy(cStatus.hostId, modernStatus.hostId.c_str(), HOST_ID_MAX_LENGTH - 1);
	cStatus.hostId[HOST_ID_MAX_LENGTH - 1] = '\0'; // Ensure null-termination

	strncpy(cStatus.commitmentHash, modernStatus.commitmentHash.c_str(), SHA256_DIGEST_LENGTH - 1);
	cStatus.commitmentHash[SHA256_DIGEST_LENGTH - 1] = '\0'; // Ensure null-termination

	// Convert maps to arrays
	for (const auto& pair : modernStatus.lastRoundBets) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.lastRoundBets[static_cast<int>(pair.first)] = pair.second;
		}
	}

	for (const auto& pair : modernStatus.currentBets) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.currentBets[static_cast<int>(pair.first)] = pair.second;
		}
	}
	for (const auto& pair : modernStatus.playerCurrentBet) {
		if (static_cast<int>(pair.first) < C_NUM_DICE_FACES) {
			cStatus.playerCurrentBet[static_cast<int>(pair.first)] = pair.second;
		}
	}
	int i = 0;
	for (const auto& pair : modernStatus.lastDiceResult) {
		if (static_cast<int>(pair) < C_NUM_DICE_FACES) {
			cStatus.lastDiceResult[i] = static_cast<int>(pair);
			i++;
		}
	}
	return cStatus;
}

void KProtocolProcess::c2sBauCua(int nIndex, BYTE* pProtocol)
{
	BAUCUA_DATA* pInfo = (BAUCUA_DATA*)pProtocol;
	if (Player[nIndex].CheckTrading())
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (pInfo->Data.nActionType == BAUCUA_MAKE_HOST)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		if (g_BauCua.allowPlayerToHost(Player[nIndex].GetPlayerName())) {
			char scriptName[255];
			sprintf(scriptName, "\\script\\baucua\\baucua.lua"); //goi qua script baucua 
			Player[nIndex].ExecuteScript(scriptName, "thaycai", Player[nIndex].GetPlayerName(), false); //thong bao thay cai
		}
	}
	else if (pInfo->Data.nActionType == BAUCUA_NO_HOST)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		g_BauCua.noHost(Player[nIndex].GetPlayerName());
	}
	else if (pInfo->Data.nActionType == BAUCUA_DEPOSIT)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		if (pInfo->Data.nMoney <= 0 || pInfo->Data.nMoney > Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU)) {
			//Invalid deposit amount
			BAUCUA_RESULT_SYNC	sValue;
			sValue.ProtocolType = s2c_syncbaucuaresult;
			sValue.nResultType = BAUCUA_RESULT_DEPOSIT;
			sValue.nResultValue = -1;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
			return;
		}
		Player[nIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_XU, Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU) - pInfo->Data.nMoney);
		g_BauCua.deposit(Player[nIndex].GetPlayerName(), pInfo->Data.nMoney);
		//Notify the player of successful deposit
		BAUCUA_RESULT_SYNC	sValue;
		sValue.ProtocolType = s2c_syncbaucuaresult;
		sValue.nResultType = BAUCUA_RESULT_DEPOSIT;
		sValue.nResultValue = pInfo->Data.nMoney;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
	}
	else if (pInfo->Data.nActionType == BAUCUA_WITHDRAW)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		int withdrawXu = g_BauCua.withdraw(Player[nIndex].GetPlayerName());
		Player[nIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_XU, Player[nIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU) + withdrawXu);
	}
	else if (pInfo->Data.nActionType == BAUCUA_BET)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		std::map<DiceFace, int> bet;
		DiceFace face;
		if (pInfo->Data.nBetType >= 0 && pInfo->Data.nBetType <= 5) {
			face = static_cast<DiceFace>(pInfo->Data.nBetType);
		}
		else {
			return;
		}
		bet[face] = pInfo->Data.nMoney;

		if (!g_BauCua.placeBet(Player[nIndex].GetPlayerName(), bet)) {

		}
	}
	else if (pInfo->Data.nActionType == BAUCUA_CANCEL_BET)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		g_BauCua.cancelPlayerBets(Player[nIndex].GetPlayerName());
	}
	else if (pInfo->Data.nActionType == BAUCUA_GET_RESULT)
	{
		if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
			return;
		if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
			return;
		auto results = g_BauCua.getLastGameResult();
		//Send last round result back to the player
		BAUCUA_RESULT_SYNC	sValue;
		sValue.ProtocolType = s2c_syncbaucuaresult;
		for (int i = 0; i < 3 && i < results.size(); ++i) {
			sValue.results[i] = static_cast<BYTE>(results[i]);
		}
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_RESULT_SYNC));
	}
	else if (pInfo->Data.nActionType == BAUCUA_GET_INFO)
	{
		BAUCUA_INFO_SYNC sValue;
		sValue.ProtocolType = s2c_syncbaucuainfo;
		sValue.nResultType = BAUCUA_RESULT_INFO;
		BauCuaStatus tmp = g_BauCua.getBauCuaStatusForPlayer(Player[nIndex].GetPlayerName());
		BauCuaStatusSend tmps = convertToCStyle(tmp);
		memcpy(&sValue.m_Status, &tmps, sizeof(BauCuaStatusSend));
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(BAUCUA_INFO_SYNC));
	}
	return;
}

// He XUC XAC chia do (DICEITEM): nguoi choi bam "Tham du nhan" / "Huy bo nhan".
// Do dai goi da duoc CoreServerShell::CheckProtocolSize chan TRUOC khi vao day
// (tra bang g_nProtocolSize, o c2s_diceitem = sizeof(DICE_CHOICE_DATA)), nen o
// day chi con phai kiem tinh hop le cua nguoi choi va cua phien.
void KProtocolProcess::c2sDiceItem(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol)
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
		return;

	DICE_CHOICE_DATA* pInfo = (DICE_CHOICE_DATA*)pProtocol;
	if (pInfo->ProtocolType != c2s_diceitem)
		return;

	KItemDice* pDice = g_ItemDiceSet.Find(pInfo->m_nDiceId);
	if (!pDice)
		return;	// phien da chot hoac ma bia - bo qua im lang

	// chi nhan dung 2 gia tri; moi thu khac coi la huy bo
	int nChoice = (pInfo->m_btChoice == DICE_CHOICE_NEED)
		? DICE_CHOICE_NEED : DICE_CHOICE_GIVEUP;
	// SetChoice tu chan bam hai lan va tu chot phien khi moi nguoi da chon
	pDice->SetChoice(nIndex, nChoice);
}

// [BDH-G4] Ban Dong Hanh: client bam nut tren thanh nhanh / cua so.
// Do dai goi da duoc CheckProtocolSize chan truoc (g_nProtocolSize).
void KProtocolProcess::c2sPartnerOp(int nIndex, BYTE* pProtocol)
{
#ifdef _SERVER
	if (!pProtocol)
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
		return;

	PARTNER_OP_DATA* pInfo = (PARTNER_OP_DATA*)pProtocol;
	if (pInfo->ProtocolType != c2s_partnerop)
		return;

	// [PETSYS 28/08] he Ban Dong Hanh PC: khong doi hoi co partner mobile
	if (pInfo->btOp == PARTNER_OP_PETSYS)
	{
		Pet_RunProtocol(nIndex, pInfo->nParam);
		return;
	}

	KPartnerSys* pSys = &Player[nIndex].m_cPartner;
	if (pSys->GetG(PTG_VERSION) <= 0 || pSys->Count() <= 0)
		return;	// chua co dong hanh nao - nut chua co tac dung

	switch (pInfo->btOp)
	{
	case PARTNER_OP_CALLOUT:
		pSys->CallOut(pSys->IsCallOut() ? 0 : 1);
		break;
	case PARTNER_OP_SELECT:
		if (pInfo->nParam >= 1 && pInfo->nParam <= PARTNER_MAX_COUNT)
			pSys->SetCurPartner(pInfo->nParam);
		break;
	case PARTNER_OP_ATTACK:
		pSys->SetG(PTG_FIGHTMODE, 0);	// 0 = chu dong danh (mac dinh)
		break;
	case PARTNER_OP_FOLLOW:
		pSys->SetG(PTG_FIGHTMODE, 1);	// 1 = chi di theo, khong danh
		break;
	case PARTNER_OP_RENAME:
	{
		char szTen[PARTNER_NAME_LEN + 1];
		memset(szTen, 0, sizeof(szTen));
		strncpy(szTen, pInfo->szName, PARTNER_NAME_LEN);
		for (int c = 0; szTen[c]; c++)
			if ((unsigned char)szTen[c] < 32)
				szTen[c] = ' ';	// chan ky tu dieu khien
		if (szTen[0])
			pSys->SetName(pSys->GetCur(), szTen);
	}
		break;
	case PARTNER_OP_TALK:
	case PARTNER_OP_FORGETSKILL:	// ban goc xu qua menu doi thoai
	case PARTNER_OP_DELETE:
	default:
		Partner_RunTalkScript(nIndex);
		break;
	}
#endif
}

void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	CP_DATAU *pSetImgCmd;
	pSetImgCmd = (CP_DATAU *)pProtocol;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].SetNumImg(pSetImgCmd->nbutton);		
}

void KProtocolProcess::C2SClientOpenShopCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	CLIENT_OPEN_SHOP 		*pDaTau1Cmd;
	pDaTau1Cmd = (CLIENT_OPEN_SHOP *)pProtocol;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	BuySell.AutoBuyItem(nIndex, pDaTau1Cmd->nItemGenre, pDaTau1Cmd->nDetailType, pDaTau1Cmd->nLevel, pDaTau1Cmd->num);
}

void KProtocolProcess::NpcLockPlayerItemCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PLAYER_LOCK_UNLOCK_ITEM *pSP=(PLAYER_LOCK_UNLOCK_ITEM *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		Player[nIndex].m_ItemList.SetLockItem(pSP->itemIdx, pSP->islock);
	}
}

void KProtocolProcess::NpcPKValueCommand(int nIndex, BYTE* pProtocol)
{
	if(!pProtocol) return;//add by phong ki“u antihack
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	CP_PKVALUE 		*pPKValueCmd;
	pPKValueCmd = (CP_PKVALUE *)pProtocol;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].Change_PK_Status(pPKValueCmd->nbutton);
}
#endif

// [MAIL 03/09] kenh ScriptProtocol (ObjBuffer): chi chuyen tiep sang KScriptProtocol.cpp.
// Goi do dai dong (-1): may chu da qua CheckProtocolSize, client tu doc wLength trong goi.
#ifndef _SERVER
void KProtocolProcess::s2cScriptData(BYTE* pMsg)
{
	SP_OnClientRecv(pMsg);
}
#else
void KProtocolProcess::c2sScriptData(int nIndex, BYTE* pProtocol)
{
	SP_OnServerRecv(nIndex, pProtocol);
}
#endif
