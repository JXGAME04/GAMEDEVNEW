/*
** lua4compat.c - [LUA54 05/09/2026] API Lua 4.0.1 tren loi Lua 5.4.7.
**
** Xuat cac ham lua4_* voi DUNG chu ky va ngu nghia cua lua_* trong Lua 4.0.1 (lapi.c / ldo.c /
** lua.c / LuaExtend.c cua Sources\Library\LuaLib), de Engine, Core (742 ham gan) va S3Client
** khong phai sua. Engine\Include\LuaLib.h anh xa ten cu -> lua4_* bang macro.
**
** Diem khac biet phai xu ly (xem PHANTICH_NANG_LUA54_0509.md muc 3):
**   - ma kieu Lua 4: TUSERDATA=0 TNIL=1 TNUMBER=2 TSTRING=3 TTABLE=4 TFUNCTION=5 TNONE=-1
**   - boolean (5.4 tra ve tu phep so sanh) duoc coi la SO 1/0 doi voi C (Lua 4 khong co boolean)
**   - lua_call CO BAO VE: pcall + ham nhan loi ghep traceback roi goi _ERRORMESSAGE (-> _ALERT
**     do engine dang ky ghi ScriptError.log) - y het lua_error/message() cua Lua 4
**   - ma loi ve ma Lua 4: ERRRUN=1 ERRFILE=2 ERRSYNTAX=3 ERRMEM=4 ERRERR=5
**   - tag -> metatable trong registry ("lua4.tags"); pushusertag -> full userdata boc con tro
**   - ref -> luaL_ref; GC threshold -> lua_gc (state mo o che do GC THE HE)
**   - lua_baselibopen: luaL_openlibs + nap lua4compat.lua (nhung) + ham nong bang C
**     (getn / tinsert / tremove / format 64 bit / random / randomseed)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define LUA4COMPAT_NO_MACROS
#include "../../Engine/Include/LuaLib.h"
#include "lua4compat_lua.h"		/* static const char lua4compat_lua[] - sinh boi sinh_shim_h.py */

/* ---- hang so Lua 4.0.1 (khong dung macro cua LuaLib.h vi trung ten voi lua.h 5.4) ---- */
#define L4_TNONE		(-1)
#define L4_TUSERDATA	0
#define L4_TNIL			1
#define L4_TNUMBER		2
#define L4_TSTRING		3
#define L4_TTABLE		4
#define L4_TFUNCTION	5
#define L4_ERRRUN		1
#define L4_ERRFILE		2
#define L4_ERRSYNTAX	3
#define L4_ERRMEM		4
#define L4_ERRERR		5
#define L4_REFNIL		(-1)
#define L4_REFREGISTRY	0

#define L4_REG_TAGS		"lua4.tags"
#define L4_REG_TAGSEQ	"lua4.tagseq"
#define L4_REG_LIBS		"lua4.libs"
#define L4_REG_MRANDOM	"lua4.mrandom"
#define L4_REG_MRANDOMSEED	"lua4.mrandomseed"
#define L4_TAG_DAU		100			/* tag dau tien do newtag cap (0..5 la tag co ban cua Lua 4) */

/* ======================================================================== [LUA54 06/09] cache Include + dong ho Lua ==== */
/* PHANTICH_NANG_LUA54_0509.md muc 12-13: 3.220 state, Include khong khu trung -> 338 MB phai phan tich cu phap
** luc boot (33 lan cay nguon 10 MB), ~5 s. Hai lop cache, KHONG doi ngu nghia (than tep VAN CHAY LAI moi lan Include):
**   2a  moi state giu closure da bien dich theo duong dan (registry "lua4.inc.fn"/"lua4.inc.st")
**   2b  bytecode dung chung ca tien trinh (bang bam: duong dan chuan hoa -> code + mtime + size); state khac
**       nap nhi phan (~14 lan nhanh hon phan tich nguon). Tep doi tren dia (mtime hoac size khac) -> bien dich lai,
**       nen sua nong / ReLoadAllScript van an. Tat: bien moi truong LUA54_KHONG_CACHE=1 hoac lua4_inc_set(0).
** Dong ho: lua4_perf_set(1) -> lua4_call do thoi gian o do sau 0 (moi loi goi tu C++ vao Lua, ke ca Include/dofile);
** Core doc + xoa bo dem moi tick qua lua4_perf_read (KPerfTick, giai doan LUA_CALL). Khong bat = khong doc dong ho. */
#include <sys/types.h>
#include <sys/stat.h>

#define L4_REG_INC_FN	"lua4.inc.fn"
#define L4_REG_INC_ST	"lua4.inc.st"
#define L4_INC_BUCKETS	4096

typedef struct L4IncEntry
{
	char* key;
	long long mtime;
	long long size;
	char* code;
	size_t len;
	struct L4IncEntry* next;
} L4IncEntry;

typedef struct L4Buf { char* buf; size_t len; size_t cap; int oom; } L4Buf;

static L4IncEntry* s_inc_bucket[L4_INC_BUCKETS];
static int s_inc_on = -1;					/* -1 = chua doc bien moi truong */
static long long s_inc_n_state_hit = 0, s_inc_n_global_hit = 0, s_inc_n_compile = 0;
static long long s_inc_bytes_saved = 0, s_inc_bytes_code = 0;
static int s_perf_on = 0;
static int s_perf_depth = 0;
static double s_perf_total = 0.0, s_perf_max = 0.0;
static int s_perf_n = 0;

static int l4_loi_nap(lua_State* L, int st);	/* dinh nghia o duoi */

#ifdef _WIN32
static CRITICAL_SECTION s_inc_cs;
static int s_inc_cs_init = 0;
static void l4_inc_init(void)
{
	if (!s_inc_cs_init) { InitializeCriticalSection(&s_inc_cs); s_inc_cs_init = 1; }
}
static void l4_inc_lock(void)	{ if (!s_inc_cs_init) l4_inc_init(); EnterCriticalSection(&s_inc_cs); }
static void l4_inc_unlock(void)	{ LeaveCriticalSection(&s_inc_cs); }
static double s_perf_freq = 0.0;
static double l4_now_ms(void)
{
	LARGE_INTEGER c;
	if (s_perf_freq <= 0.0)
	{
		LARGE_INTEGER f;
		if (!QueryPerformanceFrequency(&f) || f.QuadPart <= 0) return (double)GetTickCount();
		s_perf_freq = (double)f.QuadPart / 1000.0;
	}
	QueryPerformanceCounter(&c);
	return (double)c.QuadPart / s_perf_freq;
}
#else
static void l4_inc_init(void) {}
static void l4_inc_lock(void) {}
static void l4_inc_unlock(void) {}
static double l4_now_ms(void) { return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC; }
#define _stat64 stat
#define _S_IFDIR S_IFDIR
#define _strdup strdup
#endif

static int l4_inc_enabled(void)
{
	if (s_inc_on < 0) s_inc_on = (getenv("LUA54_KHONG_CACHE") != NULL) ? 0 : 1;
	return s_inc_on;
}

static unsigned l4_fnv(const char* s)
{
	unsigned h = 2166136261u;
	while (*s) { h ^= (unsigned char)*s++; h *= 16777619u; }
	return h;
}

/* chuan hoa khoa: '\' -> '/', ASCII thuong; byte cao (ten Han GBK) giu nguyen */
static void l4_inc_norm(const char* in, char* out, size_t cap)
{
	size_t i;
	for (i = 0; in[i] && i + 1 < cap; i++)
	{
		char ch = in[i];
		if (ch == '\\') ch = '/';
		else if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + 32);
		out[i] = ch;
	}
	out[i] = 0;
}

static L4IncEntry* l4_inc_find(const char* key, unsigned h)
{
	L4IncEntry* e;
	for (e = s_inc_bucket[h % L4_INC_BUCKETS]; e; e = e->next)
		if (strcmp(e->key, key) == 0) return e;
	return NULL;
}

static int l4_writer(lua_State* L, const void* p, size_t sz, void* ud)
{
	L4Buf* b = (L4Buf*)ud;
	(void)L;
	if (b->len + sz > b->cap)
	{
		size_t nc = b->cap ? b->cap * 2 : 8192;
		char* nb;
		while (nc < b->len + sz) nc *= 2;
		nb = (char*)realloc(b->buf, nc);
		if (nb == NULL) { b->oom = 1; return 1; }
		b->buf = nb; b->cap = nc;
	}
	memcpy(b->buf + b->len, p, sz);
	b->len += sz;
	return 0;
}

/* closure o dinh stack -> ghi vao cache cua state (FN[key] = closure, ST[key] = stamp); closure van o dinh */
static void l4_inc_remember(lua_State* L, const char* key, const char* stamp)
{
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_INC_FN);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_INC_FN);
	}
	lua_pushvalue(L, -2);
	lua_setfield(L, -2, key);
	lua_pop(L, 1);
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_INC_ST);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_INC_ST);
	}
	lua_pushstring(L, stamp);
	lua_setfield(L, -2, key);
	lua_pop(L, 1);
}

/* Tra: 1 = closure da o dinh stack (tu cache hoac vua bien dich); 0 = khong dung cache (goi duong cu);
**      < 0 = -(ma loi Lua 4), loi da duoc bao qua _ERRORMESSAGE nhu duong cu */
static int l4_inc_load(lua_State* L, const char* filename)
{
	char key[1024];
	char stamp[64];
	struct _stat64 st;
	long long mt, sz;
	unsigned h;
	L4IncEntry* e;
	int r;
	if (strlen(filename) >= sizeof(key)) return 0;
	if (_stat64(filename, &st) != 0 || (st.st_mode & _S_IFDIR)) return 0;
	mt = (long long)st.st_mtime;
	sz = (long long)st.st_size;
	l4_inc_norm(filename, key, sizeof(key));
	snprintf(stamp, sizeof(stamp), "%lld:%lld", mt, sz);
	/* 2a: closure cua chinh state nay */
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_INC_ST);
	if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, key);
		if (lua_isstring(L, -1) && strcmp(lua_tostring(L, -1), stamp) == 0)
		{
			lua_pop(L, 2);
			lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_INC_FN);
			lua_getfield(L, -1, key);
			if (lua_isfunction(L, -1))
			{
				lua_remove(L, -2);
				s_inc_n_state_hit++;
				s_inc_bytes_saved += sz;
				return 1;
			}
			lua_pop(L, 2);
		}
		else
			lua_pop(L, 2);
	}
	else
		lua_pop(L, 1);
	/* 2b: bytecode dung chung */
	h = l4_fnv(key);
	l4_inc_lock();
	e = l4_inc_find(key, h);
	if (e != NULL && e->code != NULL && e->mtime == mt && e->size == sz)
	{
		r = luaL_loadbufferx(L, e->code, e->len, key, "b");
		l4_inc_unlock();
		if (r == LUA_OK)
		{
			s_inc_n_global_hit++;
			s_inc_bytes_saved += sz;
			l4_inc_remember(L, key, stamp);
			return 1;
		}
		lua_pop(L, 1);							/* bytecode hong (khong mong doi) -> bien dich lai tu nguon */
	}
	else
		l4_inc_unlock();
	r = luaL_loadfilex(L, filename, "t");
	if (r != LUA_OK) return -l4_loi_nap(L, r);
	s_inc_n_compile++;
	{
		L4Buf b;
		memset(&b, 0, sizeof(b));
		if (lua_dump(L, l4_writer, &b, 0) == 0 && !b.oom && b.buf != NULL)
		{
			l4_inc_lock();
			e = l4_inc_find(key, h);
			if (e == NULL)
			{
				e = (L4IncEntry*)calloc(1, sizeof(L4IncEntry));
				if (e != NULL)
				{
					e->key = _strdup(key);
					e->next = s_inc_bucket[h % L4_INC_BUCKETS];
					s_inc_bucket[h % L4_INC_BUCKETS] = e;
				}
			}
			if (e != NULL && e->key != NULL)
			{
				if (e->code) { s_inc_bytes_code -= (long long)e->len; free(e->code); }
				e->code = b.buf; e->len = b.len; e->mtime = mt; e->size = sz;
				s_inc_bytes_code += (long long)b.len;
				b.buf = NULL;
			}
			l4_inc_unlock();
		}
		free(b.buf);
	}
	l4_inc_remember(L, key, stamp);
	return 1;
}

LUA_API void lua4_inc_set(int on)		{ s_inc_on = on ? 1 : 0; }
LUA_API void lua4_inc_stats(long long* state_hit, long long* global_hit, long long* compiled, long long* bytes_saved, long long* bytes_code)
{
	if (state_hit) *state_hit = s_inc_n_state_hit;
	if (global_hit) *global_hit = s_inc_n_global_hit;
	if (compiled) *compiled = s_inc_n_compile;
	if (bytes_saved) *bytes_saved = s_inc_bytes_saved;
	if (bytes_code) *bytes_code = s_inc_bytes_code;
}
LUA_API void lua4_perf_set(int on)		{ s_perf_on = on ? 1 : 0; }
LUA_API int lua4_perf_read(double* total_ms, double* max_ms, int* n)
{
	if (total_ms) *total_ms = s_perf_total;
	if (max_ms) *max_ms = s_perf_max;
	if (n) *n = s_perf_n;
	s_perf_total = 0.0; s_perf_max = 0.0; s_perf_n = 0;
	return 1;
}

/* ======================================================================== ghi ra ngoai ==== */

static FILE* s_out = NULL;
static FILE* s_err = NULL;

LUA_API int lua4_setdebugout(const char* szoutfile, const char* szerrfile)
{
#ifdef _DEBUG
	if (szoutfile) s_out = freopen(szoutfile, "a", stdout);
	if (szerrfile) s_err = freopen(szerrfile, "a", stderr);
	if (s_out == NULL || s_err == NULL) return 0;
#else
	(void)szoutfile; (void)szerrfile;
#endif
	return 1;
}

LUA_API void lua4_outerrmsg(const char* szerrmsg)
{
	if (szerrmsg) { fputs(szerrmsg, stderr); fflush(stderr); }
}

LUA_API void lua4_outoutmsg(const char* szoutmsg)
{
	if (szoutmsg) { fputs(szoutmsg, stdout); }
}

LUA_API const char* lua4_version(void)
{
	return LUA_RELEASE;
}

/* ======================================================================== bao loi kieu Lua 4 ==== */

/* Thong diep o dinh stack (giu nguyen): goi _ERRORMESSAGE(msg) neu la ham, khong thi _ALERT, khong thi stderr */
static void l4_report(lua_State* L)
{
	int top = lua_gettop(L);
	if (top < 1) return;
	lua_getglobal(L, "_ERRORMESSAGE");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		lua_getglobal(L, "_ALERT");
	}
	if (lua_isfunction(L, -1))
	{
		lua_pushvalue(L, top);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			lua_pop(L, 1);
	}
	else
	{
		lua_pop(L, 1);
		lua4_outerrmsg(lua_tostring(L, top) ? lua_tostring(L, top) : "(loi khong phai chuoi)");
		lua4_outerrmsg("\n");
	}
	lua_settop(L, top);
}

/* Ham nhan loi cho pcall: msg -> msg + traceback, bao ngay (nhu lua_error -> message() cua Lua 4) */
static int l4_msgh(lua_State* L)
{
	const char* msg = lua_tostring(L, 1);
	if (msg == NULL)
	{
		if (luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING)
			msg = lua_tostring(L, -1);
		else
			msg = lua_pushfstring(L, "(loi kieu %s)", luaL_typename(L, 1));
	}
	luaL_traceback(L, L, msg, 1);
	l4_report(L);
	return 1;
}

static int l4_panic(lua_State* L)
{
	const char* msg = lua_tostring(L, -1);
	lua4_outerrmsg("PANIC (loi ngoai vung bao ve): ");
	lua4_outerrmsg(msg ? msg : "?");
	lua4_outerrmsg("\n");
	return 0;
}

static int l4_status(int st)
{
	switch (st)
	{
	case LUA_OK:		return 0;
	case LUA_ERRRUN:	return L4_ERRRUN;
	case LUA_ERRSYNTAX:	return L4_ERRSYNTAX;
	case LUA_ERRMEM:	return L4_ERRMEM;
	case LUA_ERRERR:	return L4_ERRERR;
	case LUA_ERRFILE:	return L4_ERRFILE;
	default:			return L4_ERRRUN;
	}
}

/* ======================================================================== state ==== */

LUA_API lua_State* lua4_open(int stacksize)
{
	lua_State* L = luaL_newstate();
	(void)stacksize;					/* 5.4 tu tang stack */
	if (L == NULL) return NULL;
	lua_atpanic(L, l4_panic);
	lua_gc(L, LUA_GCGEN, 0, 0);			/* GC the he: khong con dung-toan-bo nhu Lua 4 */
	l4_inc_init();						/* [LUA54 06/09] khoa cache Include (mot lan, luong chinh) */
	return L;
}

LUA_API void lua4_close(lua_State* L)
{
	if (L) lua_close(L);
}

/* ======================================================================== stack ==== */

LUA_API int  lua4_gettop(lua_State* L)					{ return lua_gettop(L); }
LUA_API void lua4_settop(lua_State* L, int index)		{ lua_settop(L, index); }
LUA_API void lua4_pushvalue(lua_State* L, int index)	{ lua_pushvalue(L, index); }
LUA_API void lua4_remove(lua_State* L, int index)		{ lua_remove(L, index); }
LUA_API void lua4_insert(lua_State* L, int index)		{ lua_insert(L, index); }
LUA_API int  lua4_stackspace(lua_State* L)				{ return LUAI_MAXSTACK - lua_gettop(L); }
LUA_API void lua4_gettopindex(lua_State* L, int* pindex) { if (pindex) *pindex = lua_gettop(L); }

/* ======================================================================== kieu / gia tri ==== */

static int l4_type4(lua_State* L, int index)
{
	switch (lua_type(L, index))
	{
	case LUA_TNONE:			return L4_TNONE;
	case LUA_TNIL:			return L4_TNIL;
	case LUA_TBOOLEAN:		return L4_TNUMBER;
	case LUA_TNUMBER:		return L4_TNUMBER;
	case LUA_TSTRING:		return L4_TSTRING;
	case LUA_TTABLE:		return L4_TTABLE;
	case LUA_TFUNCTION:		return L4_TFUNCTION;
	default:				return L4_TUSERDATA;	/* light/full userdata, thread */
	}
}

LUA_API int lua4_type(lua_State* L, int index)
{
	return l4_type4(L, index);
}

LUA_API const char* lua4_typename(lua_State* L, int t)
{
	(void)L;
	switch (t)
	{
	case L4_TNONE:		return "no value";
	case L4_TUSERDATA:	return "userdata";
	case L4_TNIL:		return "nil";
	case L4_TNUMBER:	return "number";
	case L4_TSTRING:	return "string";
	case L4_TTABLE:		return "table";
	case L4_TFUNCTION:	return "function";
	default:			return "?";
	}
}

LUA_API int lua4_isnumber(lua_State* L, int index)
{
	return lua_isnumber(L, index) || lua_isboolean(L, index);
}

LUA_API int lua4_isstring(lua_State* L, int index)
{
	return lua_isstring(L, index) || lua_isboolean(L, index);
}

LUA_API int lua4_iscfunction(lua_State* L, int index)
{
	return lua_iscfunction(L, index);
}

LUA_API int lua4_equal(lua_State* L, int index1, int index2)
{
	return lua_rawequal(L, index1, index2);
}

LUA_API int lua4_lessthan(lua_State* L, int index1, int index2)
{
	return lua_compare(L, index1, index2, LUA_OPLT);
}

LUA_API double lua4_tonumber(lua_State* L, int index)
{
	int isnum = 0;
	double d;
	if (lua_isboolean(L, index))
		return lua_toboolean(L, index) ? 1.0 : 0.0;
	d = (double)lua_tonumberx(L, index, &isnum);
	return isnum ? d : 0.0;
}

LUA_API const char* lua4_tostring(lua_State* L, int index)
{
	if (lua_isboolean(L, index))
		return lua_toboolean(L, index) ? "1" : "0";
	return lua_tolstring(L, index, NULL);		/* so -> chuoi tai cho, nhu Lua 4 */
}

LUA_API size_t lua4_strlen(lua_State* L, int index)
{
	size_t len = 0;
	if (lua_isboolean(L, index)) return 1;
	if (lua_tolstring(L, index, &len) == NULL) return 0;
	return len;
}

LUA_API lua_CFunction lua4_tocfunction(lua_State* L, int index)
{
	return lua_tocfunction(L, index);
}

static int l4_la_hop(lua_State* L, int index)		/* full userdata do lua4_pushusertag boc? */
{
	int r = 0;
	if (lua_type(L, index) != LUA_TUSERDATA) return 0;
	if (lua_getmetatable(L, index))
	{
		lua_getfield(L, -1, "__lua4box");
		r = lua_toboolean(L, -1);
		lua_pop(L, 2);
	}
	return r;
}

LUA_API void* lua4_touserdata(lua_State* L, int index)
{
	void* p = lua_touserdata(L, index);
	if (p != NULL && l4_la_hop(L, index))
		return *(void**)p;
	return p;
}

LUA_API const void* lua4_topointer(lua_State* L, int index)
{
	return lua_topointer(L, index);
}

/* ======================================================================== tag -> metatable ==== */

/* de bang tags cua registry len stack (tao neu chua co) */
static void l4_push_tags(lua_State* L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_TAGS);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_TAGS);
	}
}

/* de metatable cua tag len stack (tao neu chua co) */
static void l4_push_tagmt(lua_State* L, int tag)
{
	l4_push_tags(L);
	lua_rawgeti(L, -1, (lua_Integer)tag);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushinteger(L, (lua_Integer)tag);
		lua_setfield(L, -2, "__lua4tag");
		lua_pushvalue(L, -1);
		lua_rawseti(L, -3, (lua_Integer)tag);
	}
	lua_remove(L, -2);		/* bo bang tags */
}

static const char* l4_event(const char* ev)
{
	if (ev == NULL) return NULL;
	if (!strcmp(ev, "gettable") || !strcmp(ev, "index")) return "__index";
	if (!strcmp(ev, "settable")) return "__newindex";
	if (!strcmp(ev, "function")) return "__call";
	if (!strcmp(ev, "gc")) return "__gc";
	if (!strcmp(ev, "add")) return "__add";
	if (!strcmp(ev, "sub")) return "__sub";
	if (!strcmp(ev, "mul")) return "__mul";
	if (!strcmp(ev, "div")) return "__div";
	if (!strcmp(ev, "pow")) return "__pow";
	if (!strcmp(ev, "unm")) return "__unm";
	if (!strcmp(ev, "lt")) return "__lt";
	if (!strcmp(ev, "le")) return "__le";
	if (!strcmp(ev, "concat")) return "__concat";
	return ev;			/* su kien la: giu ten */
}

LUA_API int lua4_newtag(lua_State* L)
{
	int tag;
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_TAGSEQ);
	tag = lua_isinteger(L, -1) ? (int)lua_tointeger(L, -1) : L4_TAG_DAU;
	lua_pop(L, 1);
	tag++;
	lua_pushinteger(L, tag);
	lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_TAGSEQ);
	l4_push_tagmt(L, tag);
	lua_pop(L, 1);
	return tag;
}

LUA_API int lua4_copytagmethods(lua_State* L, int tagto, int tagfrom)
{
	l4_push_tagmt(L, tagto);		/* [to] */
	l4_push_tagmt(L, tagfrom);		/* [to from] */
	lua_pushnil(L);
	while (lua_next(L, -2))			/* [to from k v] */
	{
		if (lua_type(L, -2) == LUA_TSTRING && strcmp(lua_tostring(L, -2), "__lua4tag") != 0)
		{
			lua_pushvalue(L, -2);	/* [to from k v k] */
			lua_pushvalue(L, -2);	/* [to from k v k v] */
			lua_settable(L, -6);	/* to[k] = v */
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 2);
	return tagto;
}

LUA_API void lua4_settag(lua_State* L, int tag)
{
	if (lua_type(L, -1) != LUA_TTABLE && lua_type(L, -1) != LUA_TUSERDATA)
		return;
	l4_push_tagmt(L, tag);
	lua_setmetatable(L, -2);
}

LUA_API int lua4_tag(lua_State* L, int index)
{
	int tag = -1;
	if (lua_getmetatable(L, index))
	{
		lua_getfield(L, -1, "__lua4tag");
		if (lua_isinteger(L, -1)) tag = (int)lua_tointeger(L, -1);
		lua_pop(L, 2);
	}
	if (tag >= 0) return tag;
	return l4_type4(L, index);		/* Lua 4: tag co ban = ma kieu */
}

LUA_API void lua4_settagmethod(lua_State* L, int tag, const char* event)
{
	const char* key = l4_event(event);
	if (key == NULL) { lua_pop(L, 1); lua_pushnil(L); return; }
	l4_push_tagmt(L, tag);			/* [new mt] */
	lua_getfield(L, -1, key);		/* [new mt old] */
	lua_pushvalue(L, -3);			/* [new mt old new] */
	lua_setfield(L, -3, key);		/* [new mt old] */
	lua_remove(L, -2);				/* [new old] */
	lua_remove(L, -2);				/* [old] */
}

LUA_API void lua4_gettagmethod(lua_State* L, int tag, const char* event)
{
	const char* key = l4_event(event);
	if (key == NULL) { lua_pushnil(L); return; }
	l4_push_tagmt(L, tag);
	lua_getfield(L, -1, key);
	lua_remove(L, -2);
}

/* ======================================================================== push ==== */

LUA_API void lua4_pushnil(lua_State* L) { lua_pushnil(L); }

LUA_API void lua4_pushnumber(lua_State* L, double n)
{
	/* so tron trong tam 2^53 -> day so nguyen (dung kieu voi 5.4, in khong co '.0') */
	if (n == floor(n) && fabs(n) < 9007199254740992.0)
		lua_pushinteger(L, (lua_Integer)n);
	else
		lua_pushnumber(L, (lua_Number)n);
}

LUA_API void lua4_pushlstring(lua_State* L, const char* s, size_t len)	{ lua_pushlstring(L, s, len); }
LUA_API void lua4_pushstring(lua_State* L, const char* s)				{ lua_pushstring(L, s); }
LUA_API void lua4_pushcclosure(lua_State* L, lua_CFunction fn, int n)	{ lua_pushcclosure(L, fn, n); }

LUA_API void lua4_pushusertag(lua_State* L, void* u, int tag)
{
	if (tag <= 0)
	{
		lua_pushlightuserdata(L, u);
		return;
	}
	{
		void** box = (void**)lua_newuserdatauv(L, sizeof(void*), 0);
		*box = u;
		l4_push_tagmt(L, tag);
		lua_pushboolean(L, 1);
		lua_setfield(L, -2, "__lua4box");
		lua_setmetatable(L, -2);
	}
}

/* ======================================================================== get / set ==== */

LUA_API void lua4_getglobal(lua_State* L, const char* name)	{ lua_getglobal(L, name); }
LUA_API void lua4_gettable(lua_State* L, int index)			{ lua_gettable(L, index); }
LUA_API void lua4_rawget(lua_State* L, int index)			{ lua_rawget(L, index); }
LUA_API void lua4_rawgeti(lua_State* L, int index, int n)	{ lua_rawgeti(L, index, (lua_Integer)n); }
LUA_API void lua4_getglobals(lua_State* L)					{ lua_pushglobaltable(L); }
LUA_API void lua4_newtable(lua_State* L)					{ lua_newtable(L); }
LUA_API void lua4_setglobal(lua_State* L, const char* name)	{ lua_setglobal(L, name); }
LUA_API void lua4_settable(lua_State* L, int index)			{ lua_settable(L, index); }
LUA_API void lua4_rawset(lua_State* L, int index)			{ lua_rawset(L, index); }
LUA_API void lua4_rawseti(lua_State* L, int index, int n)	{ lua_rawseti(L, index, (lua_Integer)n); }

LUA_API void lua4_setglobals(lua_State* L)
{
	if (!lua_istable(L, -1)) { lua_pop(L, 1); return; }
	lua_rawseti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
}

LUA_API int lua4_getref(lua_State* L, int ref)
{
	if (ref == L4_REFNIL) { lua_pushnil(L); return 1; }
	if (ref == L4_REFREGISTRY) { lua_pushvalue(L, LUA_REGISTRYINDEX); return 1; }
	if (ref < 0) return 0;
	lua_rawgeti(L, LUA_REGISTRYINDEX, (lua_Integer)ref);
	if (lua_isnil(L, -1)) { lua_pop(L, 1); return 0; }
	return 1;
}

LUA_API int lua4_ref(lua_State* L, int lock)
{
	(void)lock;			/* Lua 4: lock=0 co the bi GC; o day luon giu */
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

LUA_API void lua4_unref(lua_State* L, int ref)
{
	if (ref > 0) luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

LUA_API int lua4_next(lua_State* L, int index)	{ return lua_next(L, index); }
LUA_API void lua4_concat(lua_State* L, int n)	{ lua_concat(L, n); }
LUA_API void* lua4_newuserdata(lua_State* L, size_t size) { return lua_newuserdatauv(L, size, 0); }

/* Lua 4 lua_getn: t.n neu la so; khong thi chi so so lon nhat co gia tri */
LUA_API int lua4_getn(lua_State* L, int index)
{
	lua_Number max = 0;
	index = lua_absindex(L, index);
	lua_getfield(L, index, "n");
	if (lua_type(L, -1) == LUA_TNUMBER)
	{
		int n = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		return n;
	}
	lua_pop(L, 1);
	lua_pushnil(L);
	while (lua_next(L, index))
	{
		if (lua_type(L, -2) == LUA_TNUMBER)
		{
			lua_Number k = lua_tonumber(L, -2);
			if (k > max) max = k;
		}
		lua_pop(L, 1);
	}
	return (int)max;
}

/* ======================================================================== goi / nap ==== */

LUA_API int lua4_call(lua_State* L, int nargs, int nresults)
{
	int base = lua_gettop(L) - nargs;		/* vi tri ham */
	int st;
	int do_perf = 0;
	double t0 = 0.0;
	if (base < 1) return L4_ERRRUN;
	if (s_perf_on && s_perf_depth == 0)		/* [LUA54 06/09] do o do sau 0: Lua -> C -> Lua khong tinh trung */
	{
		do_perf = 1;
		t0 = l4_now_ms();
	}
	s_perf_depth++;
	lua_pushcfunction(L, l4_msgh);
	lua_insert(L, base);
	st = lua_pcall(L, nargs, nresults, base);
	lua_remove(L, base);
	s_perf_depth--;
	if (do_perf)
	{
		double dt = l4_now_ms() - t0;
		if (dt < 0.0) dt = 0.0;
		s_perf_total += dt;
		s_perf_n++;
		if (dt > s_perf_max) s_perf_max = dt;
	}
	if (st != LUA_OK)
		lua_pop(L, 1);						/* bo thong diep: Lua 4 da bao qua _ERRORMESSAGE */
	return l4_status(st);
}

LUA_API void lua4_rawcall(lua_State* L, int nargs, int nresults)
{
	lua_call(L, nargs, nresults);
}

LUA_API void lua4_error(lua_State* L, const char* s)
{
	if (s) lua_pushstring(L, s); else lua_pushliteral(L, "");
	lua_error(L);
}

/* nap that bai: thong diep o dinh -> bao roi bo; tra ma Lua 4 */
static int l4_loi_nap(lua_State* L, int st)
{
	if (st == LUA_ERRFILE)
	{
		lua_pop(L, 1);						/* Lua 4: khong mo duoc tep -> im lang, ERRFILE */
		return L4_ERRFILE;
	}
	l4_report(L);
	lua_pop(L, 1);
	return l4_status(st);
}

LUA_API int lua4_dostring(lua_State* L, const char* str)
{
	int st;
	if (str == NULL) return L4_ERRRUN;
	st = luaL_loadbufferx(L, str, strlen(str), str, "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return lua4_call(L, 0, LUA_MULTRET);
}

LUA_API int lua4_dofile(lua_State* L, const char* filename)
{
	int st;
	if (filename != NULL && l4_inc_enabled())	/* [LUA54 06/09] cache Include 2a + 2b, xem khoi tren */
	{
		int r = l4_inc_load(L, filename);
		if (r > 0) return lua4_call(L, 0, LUA_MULTRET);
		if (r < 0) return -r;
	}
	st = luaL_loadfilex(L, filename, "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return lua4_call(L, 0, LUA_MULTRET);
}

LUA_API int lua4_dobuffer(lua_State* L, const char* buff, size_t size, const char* name)
{
	int st = luaL_loadbufferx(L, buff, size, name ? name : "?", "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return lua4_call(L, 0, LUA_MULTRET);
}

LUA_API int lua4_compilebuffer(lua_State* L, const char* buff, size_t size, const char* name)
{
	int st = luaL_loadbufferx(L, buff, size, name ? name : "?", "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return 0;								/* ham da bien dich nam o dinh stack */
}

LUA_API int lua4_compilefile(lua_State* L, const char* filename)
{
	int st = luaL_loadfilex(L, filename, "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return 0;
}

LUA_API int lua4_execute(lua_State* L)
{
	if (!lua_isfunction(L, -1)) return L4_ERRRUN;
	return lua4_call(L, 0, LUA_MULTRET);
}

/* ======================================================================== GC ==== */

LUA_API int lua4_getgccount(lua_State* L)		{ return lua_gc(L, LUA_GCCOUNT); }
LUA_API int lua4_getgcthreshold(lua_State* L)	{ return lua_gc(L, LUA_GCCOUNT) * 2; }
LUA_API void lua4_setgcthreshold(lua_State* L, int newthreshold)
{
	/* Lua 4: threshold <= dang dung -> don TOAN BO ngay (KScriptList goi moi khung!).
	   5.4: mot buoc tang dan, re. */
	(void)newthreshold;
	lua_gc(L, LUA_GCSTEP, 0);
}

/* ======================================================================== ham nong bang C ==== */

/* so -> so nguyen 64 bit theo kieu Lua 4 ((int) cat phan thap phan); boolean -> 1/0 */
static lua_Integer l4_tointeger_floor(lua_State* L, int arg)
{
	lua_Number d;
	lua_Integer n;
	if (lua_isinteger(L, arg)) return lua_tointeger(L, arg);
	if (lua_isboolean(L, arg)) return lua_toboolean(L, arg) ? 1 : 0;
	d = luaL_checknumber(L, arg);
	d = (d < 0) ? ceil(d) : floor(d);		/* (int) trong C cat ve 0 */
	if (lua_numbertointeger(d, &n)) return n;
	return (d < 0) ? LLONG_MIN : LLONG_MAX;
}

static int l4_b_getn(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushinteger(L, lua4_getn(L, 1));
	return 1;
}

static int l4_b_tinsert(lua_State* L)		/* luaB_tinsert 4.0.1 */
{
	int v = lua_gettop(L);
	int n, pos;
	luaL_checktype(L, 1, LUA_TTABLE);
	n = lua4_getn(L, 1);
	if (v == 2)
		pos = n + 1;
	else
		pos = (int)l4_tointeger_floor(L, 2);
	lua_pushstring(L, "n");
	lua_pushinteger(L, n + 1);
	lua_rawset(L, 1);						/* t.n = n+1 */
	for (; n >= pos; n--)
	{
		lua_rawgeti(L, 1, n);
		lua_rawseti(L, 1, n + 1);			/* t[n+1] = t[n] */
	}
	lua_pushvalue(L, v);
	lua_rawseti(L, 1, pos);					/* t[pos] = v */
	return 0;
}

static int l4_b_tremove(lua_State* L)		/* luaB_tremove 4.0.1 */
{
	int pos, n;
	luaL_checktype(L, 1, LUA_TTABLE);
	n = lua4_getn(L, 1);
	pos = lua_isnoneornil(L, 2) ? n : (int)l4_tointeger_floor(L, 2);
	if (n <= 0) return 0;
	lua_rawgeti(L, 1, pos);					/* result = t[pos] */
	for (; pos < n; pos++)
	{
		lua_rawgeti(L, 1, pos + 1);
		lua_rawseti(L, 1, pos);
	}
	lua_pushstring(L, "n");
	lua_pushinteger(L, n - 1);
	lua_rawset(L, 1);						/* t.n = n-1 */
	lua_pushnil(L);
	lua_rawseti(L, 1, n);					/* t[n] = nil */
	return 1;
}

#define L4_MAX_ITEM		512
#define L4_MAX_FORMAT	32

static void l4_addquoted(lua_State* L, luaL_Buffer* b, int arg)	/* addquoted 4.0.1 */
{
	size_t l;
	const char* s = luaL_checklstring(L, arg, &l);
	luaL_addchar(b, '"');
	while (l--)
	{
		switch (*s)
		{
		case '"': case '\\': case '\n':
			luaL_addchar(b, '\\');
			luaL_addchar(b, *s);
			break;
		case '\0':
			luaL_addlstring(b, "\\000", 4);
			break;
		default:
			luaL_addchar(b, *s);
		}
		s++;
	}
	luaL_addchar(b, '"');
}

static int l4_b_format(lua_State* L)		/* str_format 4.0.1, %d/%x 64 bit */
{
	int arg = 1;
	size_t sfl;
	const char* strfrmt = luaL_checklstring(L, arg, &sfl);
	const char* strfrmt_end = strfrmt + sfl;
	luaL_Buffer b;
	luaL_buffinit(L, &b);
	while (strfrmt < strfrmt_end)
	{
		if (*strfrmt != '%')
			luaL_addchar(&b, *strfrmt++);
		else if (*++strfrmt == '%')
			luaL_addchar(&b, *strfrmt++);
		else
		{
			char form[L4_MAX_FORMAT];
			char buff[L4_MAX_ITEM];
			const char* p = strfrmt;
			size_t nspec;
			char conv;
			while (*p && strchr("-+ #0123456789.", *p) && (p - strfrmt) < 16) p++;
			nspec = (size_t)(p - strfrmt);
			conv = *p;
			form[0] = '%';
			memcpy(form + 1, strfrmt, nspec);
			form[1 + nspec] = '\0';
			strfrmt = p + 1;
			arg++;
			switch (conv)
			{
			case 'c':
				strcat(form, "c");
				snprintf(buff, sizeof(buff), form, (int)l4_tointeger_floor(L, arg));
				luaL_addstring(&b, buff);
				break;
			case 'd': case 'i':
				strcat(form, "lld");
				snprintf(buff, sizeof(buff), form, (long long)l4_tointeger_floor(L, arg));
				luaL_addstring(&b, buff);
				break;
			case 'o': case 'u': case 'x': case 'X':
				strcat(form, "ll");
				form[strlen(form) + 1] = '\0';
				form[strlen(form)] = conv;
				snprintf(buff, sizeof(buff), form, (unsigned long long)l4_tointeger_floor(L, arg));
				luaL_addstring(&b, buff);
				break;
			case 'e': case 'E': case 'f': case 'g': case 'G':
				form[strlen(form) + 1] = '\0';
				form[strlen(form)] = conv;
				if (lua_isboolean(L, arg))
					snprintf(buff, sizeof(buff), form, (double)lua_toboolean(L, arg));
				else
					snprintf(buff, sizeof(buff), form, (double)luaL_checknumber(L, arg));
				luaL_addstring(&b, buff);
				break;
			case 'q':
				l4_addquoted(L, &b, arg);
				break;
			case 's':
			{
				size_t l;
				const char* s;
				if (lua_isboolean(L, arg))
				{
					lua_pushstring(L, lua_toboolean(L, arg) ? "1" : "0");
					lua_replace(L, arg);
				}
				s = luaL_tolstring(L, arg, &l);		/* day ban chuoi len dinh */
				if (nspec == 0 || (strchr(form, '.') == NULL && l >= 100))
				{
					luaL_addvalue(&b);				/* ca chuoi, khong qua sprintf */
				}
				else
				{
					form[strlen(form) + 1] = '\0';
					form[strlen(form)] = 's';
					snprintf(buff, sizeof(buff), form, s);
					lua_pop(L, 1);
					luaL_addstring(&b, buff);
				}
				break;
			}
			default:
				return luaL_error(L, "invalid option '%%%c' to `format'", conv);
			}
		}
	}
	luaL_pushresult(&b);
	return 1;
}

static void l4_push_reg_fn(lua_State* L, const char* key)
{
	lua_getfield(L, LUA_REGISTRYINDEX, key);
}

static int l4_b_random(lua_State* L)		/* math_random 4.0.1 tren xoshiro cua 5.4 */
{
	int n = lua_gettop(L);
	l4_push_reg_fn(L, L4_REG_MRANDOM);
	if (n == 0)
	{
		lua_call(L, 0, 1);
		return 1;
	}
	if (n == 1)
	{
		lua_Integer u = l4_tointeger_floor(L, 1);
		if (u < 1) u = 1;
		lua_pushinteger(L, u);
		lua_call(L, 1, 1);
		return 1;
	}
	{
		lua_Integer lo = l4_tointeger_floor(L, 1);
		lua_Integer hi = l4_tointeger_floor(L, 2);
		if (hi < lo) { lua_Integer t = lo; lo = hi; hi = t; }
		lua_pushinteger(L, lo);
		lua_pushinteger(L, hi);
		lua_call(L, 2, 1);
		return 1;
	}
}

static int l4_b_randomseed(lua_State* L)
{
	l4_push_reg_fn(L, L4_REG_MRANDOMSEED);
	if (lua_gettop(L) >= 2)
		lua_pushinteger(L, l4_tointeger_floor(L, 1));
	lua_call(L, lua_gettop(L) >= 2 ? 1 : 0, 0);
	return 0;
}

/* ======================================================================== thu vien ==== */

static int l4_libs_da_mo(lua_State* L)
{
	int r;
	lua_getfield(L, LUA_REGISTRYINDEX, L4_REG_LIBS);
	r = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return r;
}

/* Chan canh "DLL 5.4 + cay script Lua 4": thu muc script\ (tuong doi thu muc chay) phai co dau
   LUA54_DA_CHUYEN.txt do chuyen_lua4_54.py ghi. Khong co script\ (bo thu) hoac dat bien moi truong
   LUA54_BO_KIEM=1 thi bo qua. Chi kiem MOT lan moi tien trinh. */
static void l4_kiem_cay_script(void)
{
	static int da_kiem = 0;
	FILE* f;
	if (da_kiem) return;
	da_kiem = 1;
	if (getenv("LUA54_BO_KIEM") != NULL) return;
	f = fopen("script\\timerserver.lua", "rb");
	if (f == NULL) f = fopen("script\\protocol.lua", "rb");
	if (f == NULL) return;					/* khong phai thu muc chay cua game */
	fclose(f);
	f = fopen("script\\LUA54_DA_CHUYEN.txt", "rb");
	if (f != NULL) { fclose(f); return; }
	{
		const char* msg = "[LUA54] Cay script\\ CHUA duoc chuyen sang Lua 5.4 (thieu script\\LUA54_DA_CHUYEN.txt) "
			"nhung binary dang dung Lua54Dll.dll. Chay ChayGameServer.bat / ChoiGame.bat co buoc doi cay script "
			"(hoac LuiLua4 de ve ban cu). Dung tien trinh de tranh loi hang loat.\n";
		FILE* log = fopen("ScriptError.log", "a");
		if (log) { fputs(msg, log); fclose(log); }
		lua4_outerrmsg(msg);
#ifdef _WIN32
		MessageBoxA(NULL, msg, "Lua54Dll: cay script chua chuyen", MB_OK | MB_ICONERROR);
#endif
		exit(3);
	}
}

LUA_API void lua4_baselibopen(lua_State* L)
{
	if (l4_libs_da_mo(L)) return;
	l4_kiem_cay_script();
	luaL_openlibs(L);
	/* giu math.random / math.randomseed that cua 5.4 cho ham nong */
	lua_getglobal(L, "math");
	lua_getfield(L, -1, "random");
	lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_MRANDOM);
	lua_getfield(L, -1, "randomseed");
	lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_MRANDOMSEED);
	lua_pop(L, 1);
	/* lop tuong thich viet bang Lua (nhung trong DLL) */
	if (luaL_loadbufferx(L, lua4compat_lua, sizeof(lua4compat_lua) - 1, "=lua4compat.lua", "t") != LUA_OK
		|| lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		lua4_outerrmsg("[lua4compat] khong nap duoc lua4compat.lua: ");
		lua4_outerrmsg(lua_tostring(L, -1) ? lua_tostring(L, -1) : "?");
		lua4_outerrmsg("\n");
		lua_pop(L, 1);
	}
	/* ham nong: ban C thay ban Lua cua shim */
	lua_register(L, "getn", l4_b_getn);
	lua_register(L, "tinsert", l4_b_tinsert);
	lua_register(L, "tremove", l4_b_tremove);
	lua_register(L, "format", l4_b_format);
	lua_register(L, "random", l4_b_random);
	lua_register(L, "randomseed", l4_b_randomseed);
	lua_pushboolean(L, 1);
	lua_setfield(L, LUA_REGISTRYINDEX, L4_REG_LIBS);
}

LUA_API void lua4_iolibopen(lua_State* L)	{ lua4_baselibopen(L); }
LUA_API void lua4_strlibopen(lua_State* L)	{ lua4_baselibopen(L); }
LUA_API void lua4_mathlibopen(lua_State* L)	{ lua4_baselibopen(L); }
LUA_API void lua4_dblibopen(lua_State* L)	{ lua4_baselibopen(L); }
LUA_API void lua4_openlibs(lua_State* L)	{ lua4_baselibopen(L); }

/* ======================================================================== LuaExtend.c cu ==== */

LUA_API int Lua_SetTable_DoubleFromName(lua_State* L, int nIndex, const char* szMemberName, double Number)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushstring(L, szMemberName);
	lua4_pushnumber(L, Number);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_SetTable_IntFromName(lua_State* L, int nIndex, const char* szMemberName, int Number)
{
	return Lua_SetTable_DoubleFromName(L, nIndex, szMemberName, (double)Number);
}

LUA_API int Lua_SetTable_StringFromName(lua_State* L, int nIndex, const char* szMemberName, char* szString)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushstring(L, szMemberName);
	lua_pushstring(L, szString);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_SetTable_StringFromId(lua_State* L, int nIndex, int Id, const char* szString)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushinteger(L, Id);
	lua_pushstring(L, szString);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_SetTable_DoubleFromId(lua_State* L, int nIndex, int Id, double nNumber)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushinteger(L, Id);
	lua4_pushnumber(L, nNumber);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_SetTable_IntFromId(lua_State* L, int nIndex, int Id, int nNumber)
{
	return Lua_SetTable_DoubleFromId(L, nIndex, Id, (double)nNumber);
}

LUA_API int Lua_SetTable_CFunFromName(lua_State* L, int nIndex, const char* szMemberName, lua_CFunction CFun)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushstring(L, szMemberName);
	lua_pushcfunction(L, CFun);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_SetTable_CFunFromId(lua_State* L, int nIndex, int nId, lua_CFunction CFun)
{
	if (!lua_istable(L, nIndex)) return 0;
	nIndex = lua_absindex(L, nIndex);
	lua_pushinteger(L, nId);
	lua_pushcfunction(L, CFun);
	lua_settable(L, nIndex);
	return 1;
}

LUA_API int Lua_GetValuesFromStack(lua_State* L, char* cFormat, ...)
{
	va_list vlist;
	int i = 0, nTopIndex, nIndex, nValueNum;
	if (!L || !cFormat) return 0;
	nTopIndex = lua_gettop(L);
	nValueNum = (int)strlen(cFormat);
	if (nTopIndex == 0 || nValueNum == 0) return 0;
	nIndex = (nTopIndex < nValueNum) ? 1 : nTopIndex - nValueNum + 1;
	va_start(vlist, cFormat);
	while (cFormat[i] != '\0')
	{
		switch (cFormat[i])
		{
		case 'n':
		{
			double* pNumber = va_arg(vlist, double*);
			if (pNumber == NULL || !lua4_isnumber(L, nIndex)) { va_end(vlist); return 0; }
			*pNumber = lua4_tonumber(L, nIndex++);
			break;
		}
		case 'd':
		{
			int* pInt = va_arg(vlist, int*);
			if (pInt == NULL || !lua4_isnumber(L, nIndex)) { va_end(vlist); return 0; }
			*pInt = (int)lua4_tonumber(L, nIndex++);
			break;
		}
		case 's':
		{
			const char** pStr = va_arg(vlist, const char**);
			if (pStr == NULL || !lua4_isstring(L, nIndex)) { va_end(vlist); return 0; }
			*pStr = lua4_tostring(L, nIndex++);
			break;
		}
		default:
			break;
		}
		i++;
	}
	va_end(vlist);
	return 1;
}

/* ======================================================================== tu kiem ==== */

static int l4_kiem(lua_State* L, const char* ten, const char* code, const char* mong)
{
	const char* r;
	int ok;
	lua_settop(L, 0);
	if (lua4_dostring(L, code) != 0) { lua4_outerrmsg("  selftest LOI chay: "); lua4_outerrmsg(ten); lua4_outerrmsg("\n"); return 1; }
	lua_getglobal(L, "KQ");
	r = lua4_tostring(L, -1);
	ok = (r != NULL && strcmp(r, mong) == 0);
	if (!ok)
	{
		lua4_outerrmsg("  selftest SAI: "); lua4_outerrmsg(ten); lua4_outerrmsg(" -> "); lua4_outerrmsg(r ? r : "(nil)");
		lua4_outerrmsg(" (mong "); lua4_outerrmsg(mong); lua4_outerrmsg(")\n");
	}
	lua_settop(L, 0);
	return ok ? 0 : 1;
}

LUA_API int lua4_selftest(lua_State* L)
{
	int loi = 0;
	lua4_baselibopen(L);
	loi += l4_kiem(L, "cong", "KQ = 1 + 2", "3");
	loi += l4_kiem(L, "chia in 5", "KQ = tostring(10/2)", "5");
	loi += l4_kiem(L, "chia in 3.5", "KQ = tostring(7/2)", "3.5");
	loi += l4_kiem(L, "1/3 %.16g", "KQ = tostring(1/3)", "0.3333333333333333");
	loi += l4_kiem(L, "format %d 3e9", "KQ = format('%d', 3000000000)", "3000000000");
	loi += l4_kiem(L, "format %d so thuc", "KQ = format('%d|%5.2f|%s|%03d', 7/2, 1/3, 'x', 5)", "3| 0.33|x|005");
	loi += l4_kiem(L, "getn", "KQ = getn({1,2,3}) .. ',' .. getn({n=5})", "3,5");
	loi += l4_kiem(L, "tinsert/tremove", "local t = {} tinsert(t,'a') tinsert(t,'b') tinsert(t,1,'z') local r = tremove(t) KQ = t.n .. t[1] .. t[2] .. r", "2zab");
	loi += l4_kiem(L, "upvalue that", "local d = 0 local f = function() d = d + 1 end f() f() KQ = d", "2");
	loi += l4_kiem(L, "de quy 5000", "local function s(k) if k == 0 then return 0 end return 1 + s(k-1) end KQ = s(5000)", "5000");
	loi += l4_kiem(L, "random 64 bit", "local t, n = {}, 0 for i = 1, 100000 do local r = random(1, 100000) if not t[r] then t[r] = 1 n = n + 1 end end KQ = (n > 50000) and 'ok' or n", "ok");
	loi += l4_kiem(L, "random(n)", "local ok = 1 for i = 1, 1000 do local r = random(6) if r < 1 or r > 6 then ok = 0 end end KQ = ok", "1");
	loi += l4_kiem(L, "tag method index", "local tg = newtag() settagmethod(tg, 'gettable', function(t, k) return rawget(t, k) or 'mac dinh' end) local o = settag({a = 1}, tg) KQ = o.a .. ',' .. o.b .. ',' .. tag(o)", "1,mac dinh,101");
	loi += l4_kiem(L, "call x", "local r = call(function() error('boom') end, {}, 'x', function(m) LOI_CALL = 1 end) KQ = tostring(r) .. tostring(LOI_CALL)", "nil1");
	loi += l4_kiem(L, "call thuong", "KQ = call(function(a, b) return a + b end, {2, 3})", "5");
	loi += l4_kiem(L, "dostring", "KQ = dostring('return 6 * 7')", "42");
	loi += l4_kiem(L, "strfind/strsub", "local a, b = strfind('hello world', 'wor') KQ = a .. ',' .. b .. ',' .. strsub('abcdef', 2, 4)", "7,9,bcd");
	loi += l4_kiem(L, "date", "KQ = strlen(date('%Y')) == 4 and 'ok' or 'sai'", "ok");
	loi += l4_kiem(L, "gsub", "local s, n = gsub('a-b-c', '-', '+') KQ = s .. n", "a+b+c2");
	/* loi runtime: _ERRORMESSAGE phai duoc goi, lua4_call tra 1 */
	{
		lua_settop(L, 0);
		lua4_dostring(L, "DA_BAO = 0 _ERRORMESSAGE = function(m) DA_BAO = 1 end");
		if (lua4_dostring(L, "error('loi thu')") != L4_ERRRUN) { lua4_outerrmsg("  selftest SAI: ma loi ERRRUN\n"); loi++; }
		lua_getglobal(L, "DA_BAO");
		if (lua4_tonumber(L, -1) != 1.0) { lua4_outerrmsg("  selftest SAI: _ERRORMESSAGE khong duoc goi\n"); loi++; }
		if (lua_gettop(L) != 1) { lua4_outerrmsg("  selftest SAI: stack sau loi\n"); loi++; }
		lua_settop(L, 0);
		lua4_dostring(L, "_ERRORMESSAGE = function(m) local a = _ALERT if type(a) == 'function' then a('error: ' .. tostring(m)) end end");
	}
	/* boolean -> so voi C */
	{
		lua_settop(L, 0);
		lua4_dostring(L, "KQB = (1 == 1)");
		lua_getglobal(L, "KQB");
		if (!lua4_isnumber(L, -1) || lua4_tonumber(L, -1) != 1.0) { lua4_outerrmsg("  selftest SAI: boolean -> 1\n"); loi++; }
		if (lua4_type(L, -1) != L4_TNUMBER) { lua4_outerrmsg("  selftest SAI: type boolean\n"); loi++; }
		lua_settop(L, 0);
	}
	/* pushusertag / touserdata */
	{
		int tg = lua4_newtag(L);
		int x = 7;
		lua4_pushusertag(L, &x, tg);
		if (lua4_touserdata(L, -1) != &x || lua4_tag(L, -1) != tg || lua4_type(L, -1) != L4_TUSERDATA)
		{ lua4_outerrmsg("  selftest SAI: usertag\n"); loi++; }
		lua_settop(L, 0);
	}
	/* ref */
	{
		int r;
		lua_pushstring(L, "gia tri ref");
		r = lua4_ref(L, 1);
		if (!lua4_getref(L, r) || strcmp(lua4_tostring(L, -1), "gia tri ref") != 0) { lua4_outerrmsg("  selftest SAI: ref\n"); loi++; }
		lua4_unref(L, r);
		lua_settop(L, 0);
	}
	/* [LUA54 06/09] cache Include: lan 2 CHAY LAI than tep nhung khong bien dich; tep doi -> bien dich lai */
	{
		const char* tep = "l4_selftest_inc.lua";
		long long h0 = s_inc_n_state_hit, c0 = s_inc_n_compile;
		FILE* f = fopen(tep, "wb");
		if (f != NULL)
		{
			fputs("L4_DEM = (L4_DEM or 0) + 1  L4_GT = 1\n", f);
			fclose(f);
			lua_settop(L, 0);
			if (lua4_dofile(L, tep) != 0 || lua4_dofile(L, tep) != 0) { lua4_outerrmsg("  selftest SAI: dofile qua cache\n"); loi++; }
			lua_settop(L, 0);
			lua_getglobal(L, "L4_DEM");
			if (lua4_tonumber(L, -1) != 2.0) { lua4_outerrmsg("  selftest SAI: Include lan 2 phai CHAY LAI than tep\n"); loi++; }
			lua_settop(L, 0);
			if (l4_inc_enabled() && (s_inc_n_state_hit != h0 + 1 || s_inc_n_compile != c0 + 1))
			{ lua4_outerrmsg("  selftest SAI: cache Include khong trung (hit/compile)\n"); loi++; }
			f = fopen(tep, "wb");
			if (f != NULL) { fputs("L4_DEM = (L4_DEM or 0) + 1  L4_GT = 22\n", f); fclose(f); }
			lua4_dofile(L, tep);
			lua_settop(L, 0);
			lua_getglobal(L, "L4_GT");
			if (lua4_tonumber(L, -1) != 22.0) { lua4_outerrmsg("  selftest SAI: tep doi ma khong bien dich lai\n"); loi++; }
			lua_settop(L, 0);
			remove(tep);
		}
		/* dong ho Lua */
		{
			double tong = 0.0, mx = 0.0; int n = 0;
			lua4_perf_set(1);
			lua4_dostring(L, "local s = 0 for i = 1, 100000 do s = s + i end");
			lua4_perf_read(&tong, &mx, &n);
			lua4_perf_set(0);
			if (n < 1 || tong < 0.0) { lua4_outerrmsg("  selftest SAI: dong ho lua4_call\n"); loi++; }
			lua_settop(L, 0);
		}
	}
	return loi;
}
