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
// Mot tin nhan MAT gui toi bot. GameServer dung goi, Core dien.
// PHAI nam NGOAI #ifdef _SERVER: GameServer khong dinh nghia _SERVER.
struct PB_WHISPER
{
	int			nSenderIdx;   // khe Player[] cua NGUOI GUI
	unsigned long	nPackageID;   // packageID cua goi CMD - phai vong lai trong CHAT_FEEDBACK
	const char*	szTarget;     // ten nhan vat duoc nhan (co the la bot, co the khong)
	const char*	szMsg;        // noi dung
	int			nMsgLen;
};

struct PB_DB_RESULT
{
	unsigned long	ulIdentity;
	const void*		pPayload;
	int				nPayloadLen;
};

#ifdef _SERVER

#include <vector>   // PB_WalkState giu lo trinh A* (danh sach id block)

// Tran bot cua dot nay. CO Y de nho va gan cung, KHONG lay tu cau hinh.
//
// VI SAO 20: KPlayerSet::FindFree lay tu DAU danh sach ro (KPlayerSet.cpp:161), ma danh
// sach ro duoc dung tu MAX_PLAYER-1 xuong 1 va chen o DAU (KPlayerSet.cpp:59-62 +
// KLinkArray.cpp:55-58) => bot chiem cac khe THAP 1..N va day nguoi choi that len chi so
// cao. Ma KSOServer::SavePlayerData chan nIndex > m_nMaxPlayer (=470, KSOServer.cpp:3190
// va 3 cho nua) => nguoi that bi day qua 470 se KHONG BAO GIO DUOC LUU.
// Giu tran bot nho la cach vo hieu hoa lo hong do MA KHONG dung mot dong nao cua duong
// nguoi choi that.
// 18/08: chu game yeu cau mo len 1000 de do tai that ([BotPerf] theo doi).
// Cac tran engine du cho: MAX_PLAYER=1500, MAX_NPC=98000, MAX_TEAM=MAX_PLAYER.
#define PB_MAX_BOTS      1000

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

// ---------------------------------------------------------------------------
// Trang thai DI BO cua mot bot (moi bot giu mot ban).
//
// Vi sao can trang thai: A* tra ve MOT LO TRINH nhieu waypoint, con engine chi nhan lenh di
// toi MOT diem. Phai nho dang nham waypoint thu may, va nho khoa (ban do, o dich) de biet
// khi nao can tinh lai duong. Kem hai bo do chong ket, deu lay tu cay tham khao:
//   - DUNG YEN qua lau (thuong do bi NPC chan - JX1 tinh NPC LA TUONG) -> tim lai duong CO
//     tinh NPC lam vat can;
//   - goi nhieu lan ma KHONG tien duoc waypoint nao -> tim lai duong, toi da 3 lan roi thoi.
// ---------------------------------------------------------------------------
struct PB_WalkState
{
	std::vector<int> path;        // danh sach id BLOCK do A* tra ve
	int          idx;             // dang nham toi waypoint thu may
	int          destTileX;       // khoa tinh lai duong: dich quy ve o luoi
	int          destTileY;
	int          mapId;           // khoa tinh lai duong: lo trinh nay thuoc ban do nao
	int          lastSendX;       // toa do lan cuoi da phat lenh (chong phat lai lien tuc)
	int          lastSendY;
	int          lastPosX;        // bo do dung yen
	int          lastPosY;
	unsigned int lastMoveTick;
	int          repathCount;     // so lan da tim lai duong trong chuyen nay (toi da 3)
	int          noAdvanceCalls;  // so lan goi lien tiep ma khong tien duoc waypoint
	int          nKieuDuong;      // FindPathServer tra ve: 1 = duong TRON, 2 = duong CUT
	                              // (dich khong toi duoc, chi dan toi block gan nhat)

	PB_WalkState() { Reset(); }
	void Reset()
	{
		path.clear();
		idx = 0;
		destTileX = destTileY = -1;
		mapId = -1;
		lastSendX = lastSendY = 0;
		lastPosX = lastPosY = 0;
		lastMoveTick = 0;
		repathCount = 0;
		noAdvanceCalls = 0;
		nKieuDuong = 0;
	}
};

// Cho bot di toi mot toa do MPS. Goi MOI NHIP cho toi khi tra ve khac 0.
// Tra: 1 = da toi noi, 0 = dang di, -1 = khong di duoc (chua nap luoi / khong co duong).
int  PB_WalkTo(int nNpcIdx, int nDstMpsX, int nDstMpsY, int nSubIdx,
               PB_WalkState& st, int nArriveMps);

void PB_SetSender(PB_DbSender pfn);            // GameServer cai dat duong gui
int  PB_Spawn(const char* szAccountName);      // xep 1 tai khoan vao hang doi sinh bot
int  PB_SpawnRange(int nFrom, int nTo);        // xep dai tai khoan "nFrom".."nTo"
void PB_OnRoleList(const PB_DB_RESULT* pRes);  // Goddess tra danh sach nhan vat
void PB_OnRoleData(const PB_DB_RESULT* pRes);  // Goddess tra blob TRoleData
int  PB_IsBot(int nPlayerIdx);                 // 1 = khe nay la bot
void PB_TrapLog(int nPlayerIdx, unsigned long dwScriptId, int nMapX, int nMapY); // ghi bot.log khi bot dap trap
int  PB_GetCount();                            // so bot dang song
int  PB_RemoveAll();                           // go het bot (tra so da go)
int  PB_JoinFaction();                         // ra lenh cho bot di vao phai (tra so da nhan lenh)
int  PB_SetFight(int bOn);                     // bat/tat danh quai (tra so bot doi trang thai)
// Tra 1 = da nhan la bot VA da tra loi (GameServer khong chuyen tiep nua); 0 = khong phai bot.
int  PB_WhisperReply(const PB_WHISPER* p);
// Hoc bang ten -> id kenh chat (goi tu KNewProtocolProcess khi relay cap id cho
// client). Bot can id kenh THE GIOI de chat kenh nhu ban goc linux.
void PB_GhiNhoKenh(const char* szTen, unsigned long dwId);
// Ghi bot.log tu file khac (CoreServerShell dung cho [SvPerf]).
void PB_LogNgoai(const char* szFmt, ...);
void PB_Breathe();                             // nhip: rut hang doi + het han cho

// Ham Lua cho lenh GM (dang ky trong ScriptFuns.cpp)
int  LuaPB_AddBot(Lua_State* L);               // (nTu [, nDen]) -> so da xep hang, tran
int  LuaPB_BotCount(Lua_State* L);             // () -> so bot dang song, tran
int  LuaPB_ClearBot(Lua_State* L);             // () -> so bot da go
int  LuaPB_JoinFaction(Lua_State* L);          // () -> so bot da nhan lenh vao phai
int  LuaPB_SetFight(Lua_State* L);             // (bOn) -> so bot da bat/tat danh quai
int  LuaPB_SetChat(Lua_State* L);              // (nRate[,szFile,szType]) -> so cau da nap

#endif // _SERVER
#endif // KPLAYERBOT_H
