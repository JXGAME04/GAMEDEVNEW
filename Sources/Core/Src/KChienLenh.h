//////////////////////////////////////////////////////////////////////////////
// KChienLenh.h - [CL 04/09] He CHIEN LENH (battle pass) tren MySQL.
//
// Chu chot 04/09: bang thuong CHU TU NHAP TU WEB, nen moi cau hinh nam o MySQL
// (7 bang st_*) chu khong o .lua hay .txt. May chu chi DOC va KIEM.
//
// Phan cong voi phien web (webver5-eb):
//   GAME tao bang (tep nay) - WEB chi doc/ghi noi dung. Web KHONG CREATE gi.
//
// Vi sao co bang st_cfg_log: sai trong Lua thi co ScriptError.log keu, con sai
// DU LIEU trong MySQL thi IM LANG. May chu ghi nguoc ly do tu choi vao bang do,
// trang admin hien ra - do la NOI DUY NHAT chu biet vi sao cau hinh khong an.
//
// Xem: D:\GAMEDEVNEW\BANGIAO_CHIENLENH_SCHEMA_ADMIN_0409.md (hop dong game <-> web)
//////////////////////////////////////////////////////////////////////////////

#ifndef KCHIENLENH_H
#define KCHIENLENH_H

typedef struct lua_State Lua_State;

#ifdef _SERVER

#define CL_MAX_AWARD		64		// tran so moc thuong doc vao (that su 24)
#define CL_MAX_MISSION		20		// 20 nhiem vu, id 1..20 gan CUNG voi cho moc
#define CL_LOG_TIN			0
#define CL_LOG_CANHBAO		1
#define CL_LOG_LOI			2

// ---- loi C++ (dung cho ca cac tep C++ khac trong dot sau) ----
bool ChienLenh_EnsureTables();				// tao 7 bang; false = MySQL chua san sang
int  ChienLenh_Reload();					// nap lai cau hinh; tra SO LOI (0 = nhan ban moi)
void ChienLenh_Log(int nLevel, const char* szMsg);	// ghi nguoc vao st_cfg_log
bool ChienLenh_DangMo();					// mua dang chay (trong khoang mo/dong)?
int  ChienLenh_CapTuDiem(int nScore);		// diem -> cap (diem / level_score)
int  ChienLenh_SoMoc();						// so moc thuong da nap

// ---- ham Lua (script\chienlenh\*.lua dung) ----
int LuaCL_Ready(Lua_State* L);		// () -> 1 neu bang da co va cau hinh da nap
int LuaCL_Reload(Lua_State* L);		// () -> so loi (0 = nhan ban moi)
int LuaCL_Info(Lua_State* L);		// () -> bang {season, start, close, level_score, awards, missions, cfg_ver, loi}
int LuaCL_Mission(Lua_State* L);	// (nId) -> bang {kind, score, target, enabled} | nil
int LuaCL_Award(Lua_State* L);		// (nIdx) -> bang {need_score, branch, award, title} | nil

// ---- dot 1b: trang thai nguoi choi, cong diem, trao thuong ----
int LuaCL_Load(Lua_State* L);		// () nap trang thai (playerlogin.lua)
int LuaCL_Save(Lua_State* L);		// () xa trang thai
int LuaCL_Quen(Lua_State* L);		// () xa lan cuoi roi bo khe (dang xuat)
int LuaCL_Cong(Lua_State* L);		// (nId, nSo) cong tien do; 1 = vua xong
int LuaCL_Xong(Lua_State* L);		// (nId) danh dau xong (nhiem vu khong dem)
int LuaCL_LinhNhiemVu(Lua_State* L);	// (nId) -> so diem duoc cong
int LuaCL_Nhan(Lua_State* L);		// (nMoc, nBranch) -> 1 | 2 | 0
int LuaCL_MuaVip(Lua_State* L);		// () -> 1 | 2 (da co, DUNG tieu the) | 0
int LuaCL_TrangThai(Lua_State* L);	// () -> bang day du cho giao dien
int LuaCL_Tick(Lua_State* L);		// () nhip moi phut (timerserver RunTime)

#endif // _SERVER

#endif // KCHIENLENH_H
