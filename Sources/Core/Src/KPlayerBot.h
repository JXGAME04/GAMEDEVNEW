//---------------------------------------------------------------------------
// KPlayerBot.h - bot la KPlayer THAT (khac han bot NPC cua KSimCity).
//
// PHAN BIET HAI HE BOT TRONG CAY NAY - dung lan:
//   KSimCity.*  : bot = KNpc kind_player, KHONG co KPlayer. Tien to SC_ / m_btSimCityBot.
//                 Dung lam DAN THANH THI: dong, re, chi di lai va noi chuyen.
//   KPlayerBot.*: bot = KPlayer THAT nap tu roledb qua Goddess. Tien to PB_.
//                 Co m_ItemList => co trang bi that, chi so that, sat thuong that;
//                 xem tin tuc / PM / vao phai / luyen cap deu di duong chinh thong.
//
// NGUYEN TAC KIEN TRUC QUAN TRONG NHAT: CORE KHONG BAO GIO TU DUNG BYTE LEN DAY.
//   Cay nay co HAI tep KProtocol.h dung chung guard KPROTOCOL_H, va ban trong
//   Sources\Core\Src tung khai nDataLen la size_t (=> 15 byte tren x64) trong khi
//   Goddess/Bishop (Win32) doc 11 byte. Neu de Core dong goi TProcessData roi ban sang
//   Goddess thi Goddess doc nham ulIdentity, ten tai khoan lech 4 byte, va
//   GetRoleListOfAccount that bai MA KHONG CO MOT DONG BAO LOI NAO.
//   => Core chi dua (viec gi, ulIdentity, ten tai khoan) qua mot con tro ham;
//      GameServer moi la noi dung goi tin, bang chinh khoi ma da chay 20 nam
//      (KSOServer.cpp, khuon o :3193-3211). Nhu vay bay KHONG THE xay ra,
//      khong phu thuoc sizeof la bao nhieu.
//---------------------------------------------------------------------------
#ifndef KPLAYERBOT_H
#define KPLAYERBOT_H

#ifdef _SERVER

// Tran bot cua dot nay. CO Y de nho va gan cung, KHONG lay tu cau hinh.
//
// VI SAO 20: KPlayerSet::FindFree lay tu DAU danh sach ro (KPlayerSet.cpp:161), ma danh
// sach ro duoc dung tu MAX_PLAYER-1 xuong 1 va chen o DAU (KPlayerSet.cpp:59-62 +
// KLinkArray.cpp:55-58) => bot chiem cac khe THAP 1..N va day nguoi choi that len chi so
// cao. Ma KSOServer::SavePlayerData chan nIndex > m_nMaxPlayer (=470, KSOServer.cpp:3190
// va 3 cho nua) => nguoi that bi day qua 470 se KHONG BAO GIO DUOC LUU.
// Giu tran bot nho la cach vo hieu hoa lo hong do MA KHONG dung mot dong nao cua duong
// nguoi choi that.
#define PB_MAX_BOTS      20

// So yeu cau dang bay toi Goddess. Moi bot can 2 luot hoi (danh sach nhan vat -> du lieu).
#define PB_MAX_PENDING   32

// So tick cho toi da cho mot luot hoi truoc khi bo (GAME_FPS = 18 => 540 tick ~ 30 giay).
#define PB_PENDING_TIMEOUT  540

// So bot rut khoi hang doi moi nhip. Giu nho de khong nghen hang gui cua GameServer.
#define PB_DRAIN_PER_TICK   2

// nWhat truyen cho PB_DbSender
#define PB_ASK_ROLELIST  0
#define PB_ASK_ROLEDATA  1

// Core dua yeu cau ra ngoai qua con tro ham nay; GameServer cai dat no.
// Tra 1 = da gui, 0 = that bai.
typedef int (*PB_DbSender)(int nWhat, unsigned long ulIdentity, const char* szName);

// Ket qua Goddess tra ve, GameServer chuyen nguoc vao Core.
// POD thuan, CHI di trong cung mot tien trinh (CoreServer.dll nam trong GameServer.exe,
// lien ket tinh - KSOServer.cpp:629) va hai ben cung x64 + cung header => an toan.
struct PB_DB_RESULT
{
	unsigned long	ulIdentity;
	const void*		pPayload;
	int				nPayloadLen;
};

void PB_SetSender(PB_DbSender pfn);            // GameServer cai dat duong gui
int  PB_Spawn(const char* szAccountName);      // xep 1 tai khoan vao hang doi sinh bot
int  PB_SpawnRange(int nFrom, int nTo);        // xep dai tai khoan "nFrom".."nTo"
void PB_OnRoleList(const PB_DB_RESULT* pRes);  // Goddess tra danh sach nhan vat
void PB_OnRoleData(const PB_DB_RESULT* pRes);  // Goddess tra blob TRoleData
int  PB_IsBot(int nPlayerIdx);                 // 1 = khe nay la bot
int  PB_GetCount();                            // so bot dang song
int  PB_RemoveAll();                           // go het bot (tra so da go)
void PB_Breathe();                             // nhip: rut hang doi + het han cho

#endif // _SERVER
#endif // KPLAYERBOT_H
