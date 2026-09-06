//////////////////////////////////////////////////////////////////////////////
// RelayShareData.h  (S3Relay)  [RELAYHT 06/09]
//
// SHAREDATA -- KHO DU LIEU DUNG CHUNG CHO CA CUM, port theo ban Linux.
//
// Ban Linux (s3relay_y): lop KShareDatabase / KEnumShareData / KQuery_ShareData,
// bang MySQL ShareData(ShareKey, Param1, Param2, Data) khoa duy nhat
// (ShareKey(32), Param1, Param2); luc khoi dong doc HET vao bo nho
// ("SELECT ShareKey, Param1, Param2, Data FROM ShareData"), sau do moi thao tac
// deu tren bo nho va ghi xuong MySQL. Day la xuong song de moi hoat dong toan
// may chu chia du lieu giua cac GameServer.
//
// Ben ta:
//   - bang `relay_sharedata` (cung kho MySQL voi relay_offline_msg / relay_kv,
//     doc DataBase.ini [relaydb]); tu tao luc khoi dong
//   - nap het vao bo nho luc khoi dong; doc = bo nho, ghi = bo nho + MySQL
//   - loi MySQL -> TU TAT tinh nang, relay van chay (giong OfflineMsg)
//
// Ham Lua (dat ten GIONG ban Linux de kich ban chep sang dung luon):
//   OB_SaveShareData(handle, szKey, nP1, nP2)     -- luu nguyen bo dem
//   OB_LoadShareData(handle, szKey, nP1, nP2)     -- nap vao bo dem
//   OB_DeleteShareData(szKey, nP1, nP2)
//   SaveStringToSDB / SaveStringToSDBOw / GetStringFromSDB
//   SaveIntegerToSDB / SaveIntegerToSDBOw / GetIntegerFromSDB
//   ClearRecordOnShareDB(szKey, nP1, nP2, bP1Valid, bP2Valid)
//   GetRecordCount(szKey)
//   GetFirstRecordFromSDB(szKey) -> nP1, nP2
//////////////////////////////////////////////////////////////////////////////
#ifndef RELAYSHAREDATA_H
#define RELAYSHAREDATA_H

#include "../../Engine/src/KWin32.h"
#include "../../Engine/src/KLuaScript.h"

#define SD_KEY_MAX		64			// do dai toi da ShareKey
#define SD_DATA_MAX		4096		// = RS_OB_BUF_SIZE

BOOL	ShareData_Init();
void	ShareData_Uninit();
BOOL	ShareData_Enabled();

// Dung cho module khac (TaskCentre...) doc/ghi truc tiep.
BOOL	ShareData_Set(const char* szKey, DWORD dwP1, DWORD dwP2, const void* pData, int nLen);
int		ShareData_Get(const char* szKey, DWORD dwP1, DWORD dwP2, void* pOut, int nCap);	// -1 = khong co
BOOL	ShareData_Del(const char* szKey, DWORD dwP1, DWORD dwP2);

extern TLua_Funcs	g_ShareDataFuns[];
int					g_GetShareDataFunNum();

#endif // RELAYSHAREDATA_H
