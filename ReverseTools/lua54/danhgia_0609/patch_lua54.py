import sys, re
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
C = WT + "/Sources/Library/Lua54/lua4compat.c"
LUA = WT + "/Sources/Library/Lua54/lua4compat.lua"

def rd(p):
    return open(p, "rb").read().decode("latin-1")
def wr(p, s):
    assert all(ord(ch) < 256 for ch in s)
    open(p, "wb").write(s.encode("latin-1"))
def crlf(s):
    return s.replace("\r\n", "\n").replace("\n", "\r\n")
def once(txt, anchor, what):
    n = txt.count(anchor)
    if n != 1:
        raise SystemExit("ANCHOR %s: found %d times" % (what, n))

# ---------------------------------------------------------------- lua4compat.lua
t = rd(LUA)
assert t.count("\r\n") > 100
a = "local pcall, xpcall, error, load, loadfile, setmetatable, getmetatable = pcall, xpcall, error, load, loadfile, setmetatable, getmetatable\r\n"
once(t, a, "lua locals")
t = t.replace(a, a + "local collectgarbage_54 = collectgarbage	-- [LUA54 06/09] giu ban goc: ban shim cu goi lai chinh no -> tran ngan xep (gcinfo/collectgarbage)\r\n")
for old, new in (('collectgarbage("count")', 'collectgarbage_54("count")'),
                 ('collectgarbage("step", n)', 'collectgarbage_54("step", n)'),
                 ('collectgarbage("collect")', 'collectgarbage_54("collect")')):
    once(t, old, old)
    t = t.replace(old, new)
wr(LUA, t)
print("lua4compat.lua: OK")

# ---------------------------------------------------------------- lua4compat.c
c = rd(C)
assert c.count("\r\n") > 500

BLOCK_DECL = crlf(r'''
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
''')

a = "#define L4_TAG_DAU\t\t100\t\t\t/* tag dau tien do newtag cap (0..5 la tag co ban cua Lua 4) */\r\n"
once(c, a, "L4_TAG_DAU")
c = c.replace(a, a + BLOCK_DECL)

a = "\tlua_gc(L, LUA_GCGEN, 0, 0);\t\t\t/* GC the he: khong con dung-toan-bo nhu Lua 4 */\r\n"
once(c, a, "lua4_open gc")
c = c.replace(a, a + "\tl4_inc_init();\t\t\t\t\t\t/* [LUA54 06/09] khoa cache Include (mot lan, luong chinh) */\r\n")

OLD_CALL = crlf('''LUA_API int lua4_call(lua_State* L, int nargs, int nresults)
{
	int base = lua_gettop(L) - nargs;		/* vi tri ham */
	int st;
	if (base < 1) return L4_ERRRUN;
	lua_pushcfunction(L, l4_msgh);
	lua_insert(L, base);
	st = lua_pcall(L, nargs, nresults, base);
	lua_remove(L, base);
	if (st != LUA_OK)
		lua_pop(L, 1);						/* bo thong diep: Lua 4 da bao qua _ERRORMESSAGE */
	return l4_status(st);
}
''')
NEW_CALL = crlf('''LUA_API int lua4_call(lua_State* L, int nargs, int nresults)
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
''')
once(c, OLD_CALL, "lua4_call")
c = c.replace(OLD_CALL, NEW_CALL)

OLD_DOFILE = crlf('''LUA_API int lua4_dofile(lua_State* L, const char* filename)
{
	int st = luaL_loadfilex(L, filename, "t");
	if (st != LUA_OK) return l4_loi_nap(L, st);
	return lua4_call(L, 0, LUA_MULTRET);
}
''')
NEW_DOFILE = crlf('''LUA_API int lua4_dofile(lua_State* L, const char* filename)
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
''')
once(c, OLD_DOFILE, "lua4_dofile")
c = c.replace(OLD_DOFILE, NEW_DOFILE)

OLD_END = crlf('''		lua4_unref(L, r);
		lua_settop(L, 0);
	}
	return loi;
}
''')
NEW_END = crlf('''		lua4_unref(L, r);
		lua_settop(L, 0);
	}
	/* [LUA54 06/09] cache Include: lan 2 CHAY LAI than tep nhung khong bien dich; tep doi -> bien dich lai */
	{
		const char* tep = "l4_selftest_inc.lua";
		long long h0 = s_inc_n_state_hit, c0 = s_inc_n_compile;
		FILE* f = fopen(tep, "wb");
		if (f != NULL)
		{
			fputs("L4_DEM = (L4_DEM or 0) + 1  L4_GT = 1\\n", f);
			fclose(f);
			lua_settop(L, 0);
			if (lua4_dofile(L, tep) != 0 || lua4_dofile(L, tep) != 0) { lua4_outerrmsg("  selftest SAI: dofile qua cache\\n"); loi++; }
			lua_settop(L, 0);
			lua_getglobal(L, "L4_DEM");
			if (lua4_tonumber(L, -1) != 2.0) { lua4_outerrmsg("  selftest SAI: Include lan 2 phai CHAY LAI than tep\\n"); loi++; }
			lua_settop(L, 0);
			if (l4_inc_enabled() && (s_inc_n_state_hit != h0 + 1 || s_inc_n_compile != c0 + 1))
			{ lua4_outerrmsg("  selftest SAI: cache Include khong trung (hit/compile)\\n"); loi++; }
			f = fopen(tep, "wb");
			if (f != NULL) { fputs("L4_DEM = (L4_DEM or 0) + 1  L4_GT = 22\\n", f); fclose(f); }
			lua4_dofile(L, tep);
			lua_settop(L, 0);
			lua_getglobal(L, "L4_GT");
			if (lua4_tonumber(L, -1) != 22.0) { lua4_outerrmsg("  selftest SAI: tep doi ma khong bien dich lai\\n"); loi++; }
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
			if (n < 1 || tong < 0.0) { lua4_outerrmsg("  selftest SAI: dong ho lua4_call\\n"); loi++; }
			lua_settop(L, 0);
		}
	}
	return loi;
}
''')
once(c, OLD_END, "selftest end")
c = c.replace(OLD_END, NEW_END)
wr(C, c)
print("lua4compat.c: OK, CRLF lines:", c.count("\r\n"), "LF-only:", c.count("\n") - c.count("\r\n"))
