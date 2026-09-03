// KScriptProtocol.h - [MAIL 03/09] kenh ScriptProtocol (ObjBuffer) hai chieu client <-> may chu.
// Truoc day SendScriptData chi la stub (KJx2WarInfra.cpp LuaHD3_SendScriptData: ghi log, tra 0)
// nen moi giao thuc "emSCRIPT_PROTOCOL_*" trong script\protocol.lua deu cam. Nay:
//   Lua may chu : SendScriptData(id, h)                -> nguoi choi dang chay script (PlayerIndex)
//                 SendScriptDataToPlayer(idx, id, h)   -> nguoi choi bat ky dang online
//   Lua client  : SendScriptDataToServer(id, h)
//   Ca hai      : EnsureScript(path) -> 1 neu script da nap (nap them neu chua) - dung cho Require()
// Goi mang (khung do dai dong -1 cua g_nProtocolSize):
//   [BYTE ProtocolType][WORD wLength][WORD wProtocolId][WORD wDataLen][wDataLen byte ObjBuffer]
//   wLength = so byte SAU ProtocolType (ke ca 2 byte cua chinh no) - xem CoreServerShell::CheckProtocolSize
//   va S3Client\NetConnect\NetConnectAgent.cpp (nhanh GetProtocolSize <= 0).
//   wProtocolId = chi so 1-based trong ScriptProtocol.KE_SCRIPT_PROTOCOL (script\protocol.lua) -
//   danh sach PHAI giong het nhau o hai dau.
// Ben nhan dung lai ObjBuffer tu byte tho roi goi ScriptProtocol:ProtocolProcess(id, h) trong state
// cua bo dieu phoi: may chu \script\script_protocol\protocol_def_gs.lua, client protocol_def_c.lua.
#ifndef KSCRIPTPROTOCOL_H
#define KSCRIPTPROTOCOL_H

#include "KWin32.h"

typedef struct lua_State Lua_State;

#pragma pack(push, enter_scriptprotocol)
#pragma pack(1)
typedef struct
{
	BYTE	ProtocolType;	// s2c_scriptdata / c2s_scriptdata
	WORD	wLength;		// so byte sau ProtocolType (khung -1)
	WORD	wProtocolId;	// chi so giao thuc script (protocol.lua)
	WORD	wDataLen;		// so byte ObjBuffer theo sau
} SCRIPT_DATA_HEAD;
#pragma pack(pop, enter_scriptprotocol)

#define SCRIPT_DATA_MAXLEN	4096	// = JX2OB_BUF_SIZE (KJx2SharedStore.cpp)

// Dung goi vao pOut tu ObjBuffer hOB. Tra tong so byte goi, 0 neu that bai.
int  SP_BuildPacket(BYTE btType, int nProtocolId, int hOB, BYTE* pOut, int nOutSize);

#ifdef _SERVER
void SP_OnServerRecv(int nPlayerIdx, BYTE* pMsg);	// KProtocolProcess::c2sScriptData
int  SP_SendToPlayer(int nPlayerIdx, int nProtocolId, int hOB);
int  LuaSendScriptData(Lua_State* L);
int  LuaSendScriptDataToPlayer(Lua_State* L);
#else
void SP_OnClientRecv(BYTE* pMsg);					// KProtocolProcess::s2cScriptData
int  LuaSendScriptDataToServer(Lua_State* L);
int  SP_RunClientLua(const char* szScript, const char* szCall);	// [MAIL 03/09 D2] chay 1 cau Lua trong state cua script (nap neu chua)
#endif
int  LuaEnsureScript(Lua_State* L);

#endif // KSCRIPTPROTOCOL_H
