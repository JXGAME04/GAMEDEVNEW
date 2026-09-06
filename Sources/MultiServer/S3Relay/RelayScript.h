//////////////////////////////////////////////////////////////////////////////
// RelayScript.h  (S3Relay)  [RELAYHT 06/09]
//
// HA TANG KICH BAN CHO S3RELAY -- port theo ban Linux (s3relay_y).
//
// Ban Linux: relay co MOT BO KICH BAN LUA rieng o thu muc \script cua relay,
// cong them TaskCentre chay theo lich (relaysetting\task\tasklist.ini) va
// ShareData luu MySQL. Toan bo hoat dong toan may chu (cong thanh, Tong Kim,
// lien dau, van tieu, xo so...) deu la kich banl Lua dat tren relay.
// Xem BANGIAO_MO_S3RELAY_LINUX_0609.md.
//
// Tep nay lam 3 viec:
//   1. BO KICH BAN: moi tep .lua = mot KLuaScript (mot Lua_State) rieng, nap
//      luoi, cache theo duong dan da ha chu thuong -- giong g_GetScript ben Core.
//   2. OBJBUFFER (OB_*): bo dem byte de dong goi bang Lua, dinh dang GIONG HET
//      Core\Src\KJx2SharedStore.cpp de hai ben doc duoc cua nhau.
//   3. BANG HAM LUA cua relay (g_RelayScriptFuns).
//
// Duong dan kich ban viet kieu Linux: "\\script\\lib\\common.lua", goc = thu muc
// lam viec cua S3Relay.exe.
//////////////////////////////////////////////////////////////////////////////
#ifndef RELAYSCRIPT_H
#define RELAYSCRIPT_H

#include "../../Engine/src/KWin32.h"
#include "../../Engine/src/KLuaScript.h"

#define RS_MAX_SCRIPT		512			// tran so tep .lua nap cung luc
#define RS_OB_BUF_SIZE		4096		// = JX2OB_BUF_SIZE ben Core (PHAI bang nhau)

//---------------------------------------------------------------------------
// Bo kich ban
//---------------------------------------------------------------------------
BOOL		RelayScript_Init();				// tao bo kich ban + nap \script\lib\*.lua neu co
void		RelayScript_Uninit();
BOOL		RelayScript_IsReady();

// [RELAYHT 06/09 VA2] Ten tep kich ban dang chay tren mot Lua_State (rong neu khong biet).
// Dung de chay callback trong state cua TEP DANG GOI, giong remoteexc.lua ban Linux.
const char* RelayScript_FileOfState(Lua_State* L);

// Lay (va nap neu chua co) mot tep kich ban. szFile kieu "\\script\\x.lua".
KLuaScript*	RelayScript_Get(const char* szFile, BOOL bLoadIfNeed);

// Nap lai tat ca kich ban dang giu (dung cho lenh nap nong).
int			RelayScript_ReloadAll();

// Goi ham szFunc trong tep szFile, khong tham so, bo qua ket qua. 1 = OK.
BOOL		RelayScript_CallVoid(const char* szFile, const char* szFunc);

//---------------------------------------------------------------------------
// ObjBuffer -- dung chung cho ShareData va (dot sau) RemoteExecute
//---------------------------------------------------------------------------
int			RelayOB_Alloc();							// tra ve handle > 0, 0 = loi
void		RelayOB_Free(int nHandle);
BOOL		RelayOB_Peek(int nHandle, const BYTE** ppData, int* pnLen);	// doc nguyen khoi
BOOL		RelayOB_Assign(int nHandle, const void* pData, int nLen);	// ghi de nguyen khoi

//---------------------------------------------------------------------------
// Bang ham Lua cua relay
//---------------------------------------------------------------------------
extern TLua_Funcs	g_RelayScriptFuns[];
int					g_GetRelayScriptFunNum();

// Dang ky them bang ham cua module khac (ShareData, TaskCentre) vao MOI script
// moi nap. Goi TRUOC RelayScript_Init().
BOOL		RelayScript_AddFuncTable(TLua_Funcs* pFuns, int nCount);

#endif // RELAYSCRIPT_H
