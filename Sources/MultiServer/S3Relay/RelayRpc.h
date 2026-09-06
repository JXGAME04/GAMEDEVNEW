//////////////////////////////////////////////////////////////////////////////
// RelayRpc.h  (S3Relay)  [RELAYHT 06/09]
//
// GOI KICH BAN QUA LAI GameServer <-> S3Relay  (RemoteExecute kieu ban Linux)
//
// Ban Linux: ca hai ben deu co ham Lua
//     RemoteExecute(szScript, szFunc, hParam, szCallBackFunc, nCallBackParam, dwGameSvrId)
// Ben nhan chay  szFunc(hParam, hResult, dwGameSvrId)  trong state cua szScript,
// roi tra hResult ve; ben goi chay  szCallBackFunc(nCallBackParam, hResult).
// Xem D:\ServerLinux\server1\script\lib\remoteexc.lua.
//
// Duong truyen: goi nam trong RELAY_DATA, byte dau than goi la s2s_script, roi
// RELAY_SCRIPT_CALL (Headers\KRelayProtocol.h).
//   GameServer -> relay : RELAY_DATA.nToIP = 0 ("da toi noi") -- relay bat o
//                         CHostConnect::Proc1_Relay_Data
//   relay -> GameServer : gui thang tren dung ket noi cua may chu do
//
// LUONG: goi toi tren LUONG MANG nhung Lua KHONG an toan da luong, nen goi chi
// duoc XEP HANG o day; RelayRpc_Tick() (WM_TIMER 100 ms, luong chinh) moi chay.
//////////////////////////////////////////////////////////////////////////////
#ifndef RELAYRPC_H
#define RELAYRPC_H

#include "../../Engine/src/KWin32.h"
#include "../../Engine/src/KLuaScript.h"

#define RPC_MAX_QUEUE		256			// tran hang doi (bo goi moi khi day)
#define RPC_MAX_PENDING		512			// tran so lenh dang cho ket qua

BOOL	RelayRpc_Init();
void	RelayRpc_Uninit();
void	RelayRpc_Tick();				// goi moi 100 ms tu luong chinh

// Goi tu LUONG MANG: than goi (sau byte s2s_script) va ket noi GameServer gui toi.
void	RelayRpc_OnPacket(const void* pData, size_t nSize, unsigned long ulConnId);

int		RelayRpc_QueueSize();

extern TLua_Funcs	g_RelayRpcFuns[];
int					g_GetRelayRpcFunNum();

#endif // RELAYRPC_H
