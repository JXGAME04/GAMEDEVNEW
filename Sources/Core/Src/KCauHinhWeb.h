//////////////////////////////////////////////////////////////////////////////
// KCauHinhWeb.h - [CFGW 04/09] CAU HINH GAME chinh tu WEB ADMIN qua MySQL (bang gcfg).
//
// Chu game 04/09: "config toan bo len web de chinh cho de" - exp, exp ky nang, ti le
// rot do, tien (van), thoi gian hoat dong.
//
// Phan cong voi phien web (webver5-eb): GAME tao bang gcfg / gcfg_config / gcfg_log
// (tep nay) - WEB chi SELECT/UPDATE noi dung, KHONG CREATE/ALTER. Web sua v roi tang
// gcfg_config['cfg_version']; may chu do moi 30 giay -> nap lai -> ap ngay (khoa C++)
// hoac tra qua G_CFG/HD_CFG (khoa script). May chu ghi nguoc gcfg_log + v_ap.
//
// Xem: D:\GAMEDEVNEW\BANGIAO_CAUHINH_WEB_0409.md (hop dong game <-> web)
//////////////////////////////////////////////////////////////////////////////

#ifndef KCAUHINHWEB_H
#define KCAUHINHWEB_H

typedef struct lua_State Lua_State;

#ifdef _SERVER

// ---- loi C++ ----
bool CauHinhWeb_EnsureTables();							// tao 3 bang + khai khoa C++; false = MySQL chua san sang
int  CauHinhWeb_TuNapLai(int bEp);						// -1 chua san sang | 0 khong doi | 1 da nap lai
void CauHinhWeb_Log(int nLevel, const char* k, const char* szMsg);	// ghi nguoc gcfg_log (0 tin, 1 canh bao, 2 loi)
const char* CauHinhWeb_Get(const char* k);				// gia tri ban web (NULL = chua co)

// ---- ham Lua (dang ky trong ScriptFuns.cpp, nhanh may chu) ----
int LuaCFGW_Get(Lua_State* L);		// (k) -> so | chuoi | nil
int LuaCFGW_Khai(Lua_State* L);		// (k, v, nhom, kieu, min, max, mota, nguon, ap_dung, don_vi) -> 1 | 0
int LuaCFGW_Tick(Lua_State* L);		// () -> 1 vua nap lai | 0 khong doi | -1 chua san sang
int LuaCFGW_Reload(Lua_State* L);	// () ep nap lai -> so loi | -1
int LuaCFGW_Info(Lua_State* L);		// () -> bang {ready, ver, so_khoa, so_loi, nap_cuoi, so_khai}

#endif // _SERVER
#endif // KCAUHINHWEB_H
