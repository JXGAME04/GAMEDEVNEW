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

//===========================================================================
// HOP DONG CHUNG giua Core (x64, co _SERVER) va GameServer (x64, KHONG co _SERVER).
// PHAI nam NGOAI #ifdef _SERVER: GameServer khong dinh nghia _SERVER (GameServer.vcxproj)
// nen neu de trong guard thi no khong thay gi ca.
// Chi la typedef + POD, khong keo theo phu thuoc nao.
//===========================================================================

// nWhat truyen cho PB_DbSender
#define PB_ASK_ROLELIST  0
#define PB_ASK_ROLEDATA  1

// Core dua yeu cau ra ngoai qua con tro ham nay; GameServer cai dat no.
// Tra 1 = da gui, 0 = that bai.
typedef int (*PB_DbSender)(int nWhat, unsigned long ulIdentity, const char* szName);

// Ket qua Goddess tra ve, GameServer chuyen nguoc vao Core.
// POD thuan, CHI di trong cung mot tien trinh (CoreServer.dll nam trong GameServer.exe,
// lien ket tinh - KSOServer.cpp:629) va hai ben cung x64 + cung header => an toan.
// pPayload tro thang vao pDataBuffer cua TProcessData (GameServer da boc phan dau),
// nen Core KHONG BAO GIO cham vao TProcessData - do la ca y do, xem chu thich duoi.
struct PB_DB_RESULT
{
	unsigned long	ulIdentity;
	const void*		pPayload;
	int				nPayloadLen;
};

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

// ---------------------------------------------------------------------------
// Gia tri m_nLixian danh RIENG cho bot. KHONG duoc dung 1 hay 2.
//
// Ngu nghia san co (ScriptFuns.cpp:8785): 0 = khong uy thac, 1 = dang uy thac,
// 2 = ket thuc uy thac. Bot can cai gi do KHAC 0 de song, nhung ca 1 lan 2 deu co hai:
//
//   dat 1 -> KSOServer.cpp:3450 (trong PlayerLogoutGateway, quet MOI khe MOI khung) rot
//            vao nhanh "else if (GetCharacterLixian(nIndex) == 1)": ban 3 goi tagLeaveGame2
//            THEO TEN TAI KHOAN toi Transfer/Chat/Tong roi goi SetCharacterLixianCompleted
//            -> engine TU LAT co thanh 2 ngay o khung dau.
//   dat 2 -> CoreServerShell.cpp:1257 (SetCharacterLixianEnd) tim theo TEN TAI KHOAN va
//            tra ve o khop DAU TIEN. Bot mang 2 se bi bat truoc nguoi that trung ten,
//            khien nguoi do KET UY THAC VINH VIEN.
//
// Gia tri 3 lot qua dung hai cong CAN bao ve:
//   KPlayer.cpp:970  if (m_nLixian) -> khong danh dau bot la dang thoat
//   KPlayer.cpp:998  if (m_nLixian) -> mien nhiem IsLoginTimeOut (bot khong co ket noi nen
//                    neu khong co cai nay se bi giet sau dung 10 giay)
// va truot ca ba cong gay hai o tren. Da ra soat TOAN BO 9 cho doc co nay.
// ---------------------------------------------------------------------------
#define PB_LIXIAN_BOT    3

void PB_SetSender(PB_DbSender pfn);            // GameServer cai dat duong gui
int  PB_Spawn(const char* szAccountName);      // xep 1 tai khoan vao hang doi sinh bot
int  PB_SpawnRange(int nFrom, int nTo);        // xep dai tai khoan "nFrom".."nTo"
void PB_OnRoleList(const PB_DB_RESULT* pRes);  // Goddess tra danh sach nhan vat
void PB_OnRoleData(const PB_DB_RESULT* pRes);  // Goddess tra blob TRoleData
int  PB_IsBot(int nPlayerIdx);                 // 1 = khe nay la bot
int  PB_GetCount();                            // so bot dang song
int  PB_RemoveAll();                           // go het bot (tra so da go)
void PB_Breathe();                             // nhip: rut hang doi + het han cho

// Ham Lua cho lenh GM (dang ky trong ScriptFuns.cpp)
int  LuaPB_AddBot(Lua_State* L);               // (nTu [, nDen]) -> so da xep hang, tran
int  LuaPB_BotCount(Lua_State* L);             // () -> so bot dang song, tran
int  LuaPB_ClearBot(Lua_State* L);             // () -> so bot da go

#endif // _SERVER
#endif // KPLAYERBOT_H
