/*
** LuaLib.h - [LUA54 05/09/2026] API Lua 4.0 tren loi Lua 5.4.7 (Lua54Dll.dll).
**
** Engine / Core / S3Client van goi DUNG cac ten API Lua 4.0.1 (lua_gettop, lua_tonumber, lua_call,
** lua_dostring, lua_newtag ...). Tat ca duoc anh xa bang macro sang ham lua4_* xuat tu Lua54Dll.dll
** (Sources\Library\Lua54\lua4compat.c) voi DUNG chu ky va ngu nghia Lua 4.0.1:
**   - ma kieu Lua 4 (TNIL=1, TNUMBER=2 ...), boolean cua 5.4 duoc coi la so 1/0
**   - lua_call co bao ve (tra ma loi), loi di qua _ERRORMESSAGE/_ALERT nhu Lua 4
**   - tag method -> metatable, ref -> luaL_ref, GC -> lua_gc
** 742 ham gan trong ScriptFuns.cpp va lop KLuaScript KHONG can sua.
**
** Tep nay duoc ca Lua54Dll (LUA54DLL_EXPORTS, dinh nghia LUA4COMPAT_NO_MACROS de khong dung macro
** doi ten) lan cac du an tieu thu include. Xem PHANTICH_NANG_LUA54_0509.md.
*/

#ifndef LUA4COMPAT_LUALIB_H
#define LUA4COMPAT_LUALIB_H

#include <stddef.h>

#ifdef LUA_API
#undef LUA_API
#endif
#ifdef LUALIB_API
#undef LUALIB_API
#endif
#ifdef LUA54DLL_EXPORTS
#define LUA_API		extern __declspec(dllexport)
#else
#define LUA_API		extern __declspec(dllimport)
#endif
#define LUALIB_API	LUA_API

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LUA4COMPAT_NO_MACROS
typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State* L);
#endif

/* ------------------------------------------------------------------ hang so Lua 4.0.1 ---- */
#ifndef LUA4COMPAT_NO_MACROS
#define LUA_VERSION		"Lua 5.4.7 (lua4compat)"
#define LUA_COPYRIGHT	"Copyright (C) 1994-2024 Lua.org, PUC-Rio"
#define LUA_ERRORMESSAGE	"_ERRORMESSAGE"
#define LUA_ALERT		"_ALERT"
#define LUA_NOREF		(-2)
#define LUA_REFNIL		(-1)
#define LUA_REFREGISTRY	0
#define LUA_ANYTAG		(-1)
#define LUA_NOTAG		(-2)
#define LUA_MULTRET		(-1)
#define LUA_MINSTACK	20
#define LUA_ERRRUN		1
#define LUA_ERRFILE		2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM		4
#define LUA_ERRERR		5
#define LUA_TNONE		(-1)
#define LUA_TUSERDATA	0
#define LUA_TNIL		1
#define LUA_TNUMBER		2
#define LUA_TSTRING		3
#define LUA_TTABLE		4
#define LUA_TFUNCTION	5
#endif

/* ------------------------------------------------------------------ API lua4_* (chu ky Lua 4) ---- */
/* state */
LUA_API lua_State*	lua4_open(int stacksize);
LUA_API void		lua4_close(lua_State* L);
/* stack */
LUA_API int			lua4_gettop(lua_State* L);
LUA_API void		lua4_settop(lua_State* L, int index);
LUA_API void		lua4_pushvalue(lua_State* L, int index);
LUA_API void		lua4_remove(lua_State* L, int index);
LUA_API void		lua4_insert(lua_State* L, int index);
LUA_API int			lua4_stackspace(lua_State* L);
/* hoi gia tri */
LUA_API int			lua4_type(lua_State* L, int index);
LUA_API const char*	lua4_typename(lua_State* L, int t);
LUA_API int			lua4_isnumber(lua_State* L, int index);
LUA_API int			lua4_isstring(lua_State* L, int index);
LUA_API int			lua4_iscfunction(lua_State* L, int index);
LUA_API int			lua4_tag(lua_State* L, int index);
LUA_API int			lua4_equal(lua_State* L, int index1, int index2);
LUA_API int			lua4_lessthan(lua_State* L, int index1, int index2);
LUA_API double		lua4_tonumber(lua_State* L, int index);
LUA_API const char*	lua4_tostring(lua_State* L, int index);
LUA_API size_t		lua4_strlen(lua_State* L, int index);
LUA_API lua_CFunction lua4_tocfunction(lua_State* L, int index);
LUA_API void*		lua4_touserdata(lua_State* L, int index);
LUA_API const void*	lua4_topointer(lua_State* L, int index);
/* day len stack */
LUA_API void		lua4_pushnil(lua_State* L);
LUA_API void		lua4_pushnumber(lua_State* L, double n);
LUA_API void		lua4_pushlstring(lua_State* L, const char* s, size_t len);
LUA_API void		lua4_pushstring(lua_State* L, const char* s);
LUA_API void		lua4_pushcclosure(lua_State* L, lua_CFunction fn, int n);
LUA_API void		lua4_pushusertag(lua_State* L, void* u, int tag);
/* lay */
LUA_API void		lua4_getglobal(lua_State* L, const char* name);
LUA_API void		lua4_gettable(lua_State* L, int index);
LUA_API void		lua4_rawget(lua_State* L, int index);
LUA_API void		lua4_rawgeti(lua_State* L, int index, int n);
LUA_API void		lua4_getglobals(lua_State* L);
LUA_API void		lua4_gettagmethod(lua_State* L, int tag, const char* event);
LUA_API int			lua4_getref(lua_State* L, int ref);
LUA_API void		lua4_newtable(lua_State* L);
/* dat */
LUA_API void		lua4_setglobal(lua_State* L, const char* name);
LUA_API void		lua4_settable(lua_State* L, int index);
LUA_API void		lua4_rawset(lua_State* L, int index);
LUA_API void		lua4_rawseti(lua_State* L, int index, int n);
LUA_API void		lua4_setglobals(lua_State* L);
LUA_API void		lua4_settagmethod(lua_State* L, int tag, const char* event);
LUA_API int			lua4_ref(lua_State* L, int lock);
/* goi / nap */
LUA_API int			lua4_call(lua_State* L, int nargs, int nresults);
LUA_API void		lua4_rawcall(lua_State* L, int nargs, int nresults);
LUA_API int			lua4_dofile(lua_State* L, const char* filename);
LUA_API int			lua4_dostring(lua_State* L, const char* str);
LUA_API int			lua4_dobuffer(lua_State* L, const char* buff, size_t size, const char* name);
LUA_API int			lua4_compilebuffer(lua_State* L, const char* buff, size_t size, const char* name);
LUA_API int			lua4_compilefile(lua_State* L, const char* filename);
LUA_API int			lua4_execute(lua_State* L);
LUA_API void		lua4_gettopindex(lua_State* L, int* pindex);
/* GC */
LUA_API int			lua4_getgcthreshold(lua_State* L);
LUA_API int			lua4_getgccount(lua_State* L);
LUA_API void		lua4_setgcthreshold(lua_State* L, int newthreshold);
/* khac */
LUA_API int			lua4_newtag(lua_State* L);
LUA_API int			lua4_copytagmethods(lua_State* L, int tagto, int tagfrom);
LUA_API void		lua4_settag(lua_State* L, int tag);
LUA_API void		lua4_error(lua_State* L, const char* s);
LUA_API void		lua4_unref(lua_State* L, int ref);
LUA_API int			lua4_next(lua_State* L, int index);
LUA_API int			lua4_getn(lua_State* L, int index);
LUA_API void		lua4_concat(lua_State* L, int n);
LUA_API void*		lua4_newuserdata(lua_State* L, size_t size);
/* thu vien */
LUA_API void		lua4_baselibopen(lua_State* L);
LUA_API void		lua4_iolibopen(lua_State* L);
LUA_API void		lua4_strlibopen(lua_State* L);
LUA_API void		lua4_mathlibopen(lua_State* L);
LUA_API void		lua4_dblibopen(lua_State* L);
LUA_API void		lua4_openlibs(lua_State* L);
/* debug out (lua.c cu) */
LUA_API int			lua4_setdebugout(const char* szoutfile, const char* szerrfile);
LUA_API void		lua4_outoutmsg(const char* szoutmsg);
LUA_API void		lua4_outerrmsg(const char* szerrmsg);
/* thong tin */
LUA_API const char*	lua4_version(void);			/* "Lua 5.4.7" */
LUA_API int			lua4_selftest(lua_State* L);	/* 0 = OK, khac 0 = so muc that bai (kiem sau khi build) */
/* [LUA54 06/09 toi] PHUONGAN_LUA54_SCRIPT_0609.md: boolean/so nguyen that (C3), mot state (C8), profiler (C11), IncludeOnce (C6) */
LUA_API void		lua4_pushboolean(lua_State* L, int b);
LUA_API void		lua4_pushinteger(lua_State* L, long long n);
LUA_API int			lua4_toboolean(lua_State* L, int index);
LUA_API int			lua4_mot_state(void);				/* 1 = dang chay che do mot state (LUA54_MOT_STATE=1) */
LUA_API lua_State*	lua4_master(void);					/* state chu (mot state) hoac NULL */
LUA_API lua_State*	lua4_owner(lua_State* L);			/* thread script so huu L (coroutine -> script tao no) */
LUA_API int			lua4_so_state(void);
LUA_API int			lua4_prof_set(int every);			/* 0 = tat; n >= 100 = lay mau moi n lenh; tra gia tri ap dung */
LUA_API long long	lua4_prof_write(const char* path, int top);	/* ghi bang xep hang (ghi noi) roi xoa; tra so mau */
LUA_API long long	lua4_inc_once_skip(void);			/* so lan Include bo qua nho @IncludeOnce */

/* LuaExtend.c cu (ten giu nguyen, C linkage) */
LUA_API int Lua_SetTable_StringFromId(lua_State* L, int nIndex, int Id, const char* szString);
LUA_API int Lua_SetTable_DoubleFromId(lua_State* L, int nIndex, int Id, double nNumber);
LUA_API int Lua_SetTable_IntFromId(lua_State* L, int nIndex, int Id, int nNumber);
LUA_API int Lua_SetTable_CFunFromName(lua_State* L, int nIndex, const char* szMemberName, lua_CFunction CFun);
LUA_API int Lua_SetTable_CFunFromId(lua_State* L, int nIndex, int nId, lua_CFunction CFun);
LUA_API int Lua_SetTable_StringFromName(lua_State* L, int nIndex, const char* szMemberName, char* szString);
LUA_API int Lua_SetTable_IntFromName(lua_State* L, int nIndex, const char* szMemberName, int Number);
LUA_API int Lua_SetTable_DoubleFromName(lua_State* L, int nIndex, const char* szMemberName, double Number);
LUA_API int Lua_GetValuesFromStack(lua_State* L, char* cFormat, ...);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ ten Lua 4 -> lua4_* ---- */
#ifndef LUA4COMPAT_NO_MACROS

#define lua_open			lua4_open
#define lua_close			lua4_close
#define lua_gettop			lua4_gettop
#define lua_settop			lua4_settop
#define lua_pushvalue		lua4_pushvalue
#define lua_remove			lua4_remove
#define lua_insert			lua4_insert
#define lua_stackspace		lua4_stackspace
#define lua_type			lua4_type
#define lua_typename		lua4_typename
#define lua_isnumber		lua4_isnumber
#define lua_isstring		lua4_isstring
#define lua_iscfunction		lua4_iscfunction
#define lua_tag				lua4_tag
#define lua_equal			lua4_equal
#define lua_lessthan		lua4_lessthan
#define lua_tonumber		lua4_tonumber
#define lua_tostring		lua4_tostring
#define lua_strlen			lua4_strlen
#define lua_tocfunction		lua4_tocfunction
#define lua_touserdata		lua4_touserdata
#define lua_topointer		lua4_topointer
#define lua_pushnil			lua4_pushnil
#define lua_pushnumber		lua4_pushnumber
#define lua_pushlstring		lua4_pushlstring
#define lua_pushstring		lua4_pushstring
#define lua_pushcclosure	lua4_pushcclosure
#define lua_pushusertag		lua4_pushusertag
#define lua_getglobal		lua4_getglobal
#define lua_gettable		lua4_gettable
#define lua_rawget			lua4_rawget
#define lua_rawgeti			lua4_rawgeti
#define lua_getglobals		lua4_getglobals
#define lua_gettagmethod	lua4_gettagmethod
#define lua_getref			lua4_getref
#define lua_newtable		lua4_newtable
#define lua_setglobal		lua4_setglobal
#define lua_settable		lua4_settable
#define lua_rawset			lua4_rawset
#define lua_rawseti			lua4_rawseti
#define lua_setglobals		lua4_setglobals
#define lua_settagmethod	lua4_settagmethod
#define lua_ref				lua4_ref
#define lua_call			lua4_call
#define lua_rawcall			lua4_rawcall
#define lua_dofile			lua4_dofile
#define lua_dostring		lua4_dostring
#define lua_dobuffer		lua4_dobuffer
#define lua_compilebuffer	lua4_compilebuffer
#define lua_compilefile		lua4_compilefile
#define lua_execute			lua4_execute
#define lua_gettopindex		lua4_gettopindex
#define lua_getgcthreshold	lua4_getgcthreshold
#define lua_getgccount		lua4_getgccount
#define lua_setgcthreshold	lua4_setgcthreshold
#define lua_newtag			lua4_newtag
#define lua_copytagmethods	lua4_copytagmethods
#define lua_settag			lua4_settag
#define lua_error			lua4_error
#define lua_unref			lua4_unref
#define lua_next			lua4_next
#define lua_getn			lua4_getn
#define lua_concat			lua4_concat
#define lua_newuserdata		lua4_newuserdata
#define lua_baselibopen		lua4_baselibopen
#define lua_iolibopen		lua4_iolibopen
#define lua_strlibopen		lua4_strlibopen
#define lua_mathlibopen		lua4_mathlibopen
#define lua_dblibopen		lua4_dblibopen
#define luaL_openlibs		lua4_openlibs
#define lua_setdebugout		lua4_setdebugout
#define lua_outoutmsg		lua4_outoutmsg
#define lua_outerrmsg		lua4_outerrmsg

/* macro tien ich cua lua.h 4.0.1 */
#define lua_pop(L,n)			lua_settop(L, -(n)-1)
#define lua_register(L,n,f)		(lua_pushcfunction(L, f), lua_setglobal(L, n))
#define lua_pushuserdata(L,u)	lua_pushusertag(L, u, 0)
#define lua_pushcfunction(L,f)	lua_pushcclosure(L, f, 0)
#define lua_clonetag(L,t)		lua_copytagmethods(L, lua_newtag(L), (t))
#define lua_isfunction(L,n)		(lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)		(lua_type(L,n) == LUA_TTABLE)
#define lua_isuserdata(L,n)		(lua_type(L,n) == LUA_TUSERDATA)
#define lua_isnil(L,n)			(lua_type(L,n) == LUA_TNIL)
#define lua_isnull(L,n)			(lua_type(L,n) == LUA_TNONE)
#define lua_getregistry(L)		lua_getref(L, LUA_REFREGISTRY)

/* lop Lua_* cua engine (Engine\Include\lua.h cu) */
#define Lua_CFunction					lua_CFunction
#define Lua_State						lua_State
#define Lua_Create(nSize)				lua_open(nSize)
#define Lua_Release(L)					lua_close(L)
#define Lua_GetTopIndex(L)				lua_gettop(L)
#define Lua_SetTopIndex(L,nIndex)		lua_settop(L,nIndex)
#define Lua_PushValue(L,nIndex)			lua_pushvalue(L,nIndex)
#define Lua_RemoveValue(L,nIndex)		lua_remove(L,nIndex)
#define Lua_InsertValue(L,nIndex)		lua_insert(L,nIndex)
#define Lua_GetStackSpace(L)			lua_stackspace(L)
#define Lua_GetValueType(L,nIndex)		lua_type(L,nIndex)
#define Lua_GetTypeName(L,Tag)			lua_typename(L,Tag)
#define Lua_IsNumber(L,nIndex)			lua_isnumber(L,nIndex)
#define Lua_IsString(L,nIndex)			lua_isstring(L,nIndex)
#define Lua_IsCFunction(L,nIndex)		lua_iscfunction(L,nIndex)
#define Lua_IsTable(L,nIndex)			lua_istable(L,nIndex)
#define Lua_GetValueTag(L,nIndex)		lua_tag(L,nIndex)
#define Lua_IsEqual(L,index1,index2)	lua_equal(L,index1,index2)
#define Lua_IsLessThan(L,index1,index2)	lua_lessthan(L,index1,index2)
#define Lua_ValueToNumber(L,nIndex)		lua_tonumber(L,nIndex)
#define Lua_ValueToString(L,nIndex)		lua_tostring(L,nIndex)
#define Lua_GetStrLen(L,nIndex)			lua_strlen(L,nIndex)
#define Lua_ValueToCFunction(L,nIndex)	lua_tocfunction(L,nIndex)
#define Lua_ValueToUserData(L,nIndex)	lua_touserdata(L,nIndex)
#define Lua_ValueToPoint(L,nIndex)		lua_topointer(L,nIndex)
#define Lua_PushNil(L)					lua_pushnil(L)
#define Lua_PushNumber(L,Number)		lua_pushnumber(L,Number)
#define Lua_PushLString(L,LString,Len)	lua_pushlstring(L,LString,Len)
#define Lua_PushString(L,String)		lua_pushstring(L,String)
#define Lua_PushBoolean(L,b)			lua4_pushboolean(L,b)		/* [LUA54 06/09 toi] */
#define Lua_PushInteger(L,n)			lua4_pushinteger(L,n)
#define Lua_ToBoolean(L,nIndex)			lua4_toboolean(L,nIndex)
#define Lua_PushCClosure(L,Fun,N)		lua_pushcclosure(L,Fun,N)
#define Lua_PushUserTag(L,PVoid,Tag)	lua_pushusertag(L,PVoid,Tag)
#define Lua_GetGlobal(L,Valuename)		lua_getglobal(L,Valuename)
#define Lua_GetTable(L,nIndex)			lua_gettable(L,nIndex)
#define Lua_RawGet(L,nIndex)			lua_rawget(L,nIndex)
#define Lua_RawGetI(L,nIndex,n)			lua_rawgeti(L,nIndex,n)
#define Lua_GetGlobals(L)				lua_getglobals(L)
#define Lua_GetTagMethod(L,iTag,cEvent)	lua_gettagmethod(L,iTag,cEvent)
#define Lua_GetRef(L,iRef)				lua_getref(L,iRef)
#define Lua_NewTable(L)					lua_newtable(L)
#define Lua_SetGlobal(L,cName)			lua_setglobal(L,cName)
#define Lua_SetTable(L,nIndex)			lua_settable(L,nIndex)
#define Lua_RawSet(L,nIndex)			lua_rawset(L,nIndex)
#define Lua_RawSetI(L,nIndex,nNum)		lua_rawseti(L,nIndex,nNum)
#define Lua_SetGlobals(L)				lua_setglobals(L)
#define Lua_SetTagMethod(L,iTag,cEvent)	lua_settagmethod(L,iTag,cEvent)
#define Lua_SetRef(L,nLock)				lua_ref(L,nLock)
#define Lua_Call(L,nArgs,nResults)		lua_call(L,nArgs,nResults)
#define Lua_RawCall(L,nArgs,nResults)	lua_rawcall(L,nArgs,nResults)
#define Lua_ExecuteFile(L,cFilename)	lua_dofile(L,cFilename)
#define Lua_ExecuteString(L,cString)	lua_dostring(L,cString)
#define Lua_ExecuteBuffer(L,pBuff,iSize,cname)	lua_dobuffer(L,pBuff,iSize,cname)
#define Lua_GetGCThreshold(L)			lua_getgcthreshold(L)
#define Lua_GetGCCount(L)				lua_getgccount(L)
#define Lua_SetGCThreshold(L,nThreshold)	lua_setgcthreshold(L,nThreshold)
#define Lua_NewTag(L)					lua_newtag(L)
#define Lua_CopyTagMethods(L,nTagTo,nTagFrom)	lua_copytagmethods(L,nTagTo,nTagFrom)
#define Lua_SetTag(L,nTag)				lua_settag(L,nTag)
#define Lua_Error(L,cError)				lua_error(L,cError)
#define Lua_UnRef(L,nRef)				lua_unref(L,nRef)
#define Lua_Next(L,nIndex)				lua_next(L,nIndex)
#define Lua_GetN(L,nIndex)				lua_getn(L,nIndex)
#define Lua_Concat(L,nNum)				lua_concat(L,nNum)
#define Lua_NewUserData(L,nSize)		lua_newuserdata(L,nSize)
#define Lua_OpenBaseLib(L)				lua_baselibopen(L)
#define Lua_OpenIOLib(L)				lua_iolibopen(L)
#define Lua_OpenStrLib(L)				lua_strlibopen(L)
#define Lua_OpenMathLib(L)				lua_mathlibopen(L)
#define Lua_OpenDBLib(L)				lua_dblibopen(L)
#define Lua_OpenAllLib(L)				luaL_openlibs(L)
#define Lua_Pop(L,nIndex)				lua_pop(L,nIndex)
#define Lua_Register(L,cfname,pFun)		lua_register(L,cfname,pFun)
#define Lua_PushUserData(L,UseData)		lua_pushuserdata(L,UseData)
#define Lua_PushCFunction(L,pFun)		lua_pushcfunction(L,pFun)
#define Lua_CloneTag(L,nTag)			lua_clonetag(L,nTag)
#define Lua_GetRegistry(L)				lua_getregistry(L)
#define Lua_CompileBuffer(L,pBUFF,nBuffSize,cBname)	lua_compilebuffer(L,pBUFF,nBuffSize,cBname)
#define Lua_CompileFile(L,cFilename)	lua_compilefile(L,cFilename)
#define Lua_Execute(L)					lua_execute(L)
/* KLuaScript::SafeCallBegin co hai dang: (L, int*) va (L) - macro nhan ca hai */
#ifdef __cplusplus
static inline int lua4_safebegin(lua_State* L, int* pIndex) { int t = lua4_gettop(L); if (pIndex) *pIndex = t; return t; }
static inline int lua4_safebegin(lua_State* L) { return lua4_gettop(L); }
#define Lua_SafeBegin(...)				lua4_safebegin(__VA_ARGS__)
#else
#define Lua_SafeBegin(L,pIndex)			lua_gettopindex(L,pIndex)
#endif
#define Lua_SafeEnd(L,nIndex)			Lua_SetTopIndex(L,nIndex)

#endif /* LUA4COMPAT_NO_MACROS */

#endif /* LUA4COMPAT_LUALIB_H */
