import sys
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
C = WT + "/Sources/Library/Lua54/lua4compat.c"
LUA = WT + "/Sources/Library/Lua54/lua4compat.lua"
def rd(p): return open(p, "rb").read().decode("latin-1")
def wr(p, s):
    assert all(ord(ch) < 256 for ch in s)
    open(p, "wb").write(s.encode("latin-1"))
def crlf(s): return s.replace("\r\n", "\n").replace("\n", "\r\n")
def once(txt, a, what):
    n = txt.count(a)
    if n != 1: raise SystemExit("ANCHOR %s: %d" % (what, n))

c = rd(C)
# 1) khoi bi danh: dat ngay sau ham l4_inc_remember (truoc l4_inc_load)
ANCHOR = crlf('''/* Tra: 1 = closure da o dinh stack (tu cache hoac vua bien dich); 0 = khong dung cache (goi duong cu);
**      < 0 = -(ma loi Lua 4), loi da duoc bao qua _ERRORMESSAGE nhu duong cu */
static int l4_inc_load(lua_State* L, const char* filename)
{
''')
once(c, ANCHOR, "l4_inc_load head")
ALIAS = crlf(r'''/* ======================================================================== [SAPXEP 06/09] bi danh duong dan ==== */
/* SAPXEP_SCRIPT_0609.md R2: khi sap xep lai cay script, duong dan CU van song nho tep
** script\_duongdan_cu.txt (moi dong "cu=moi", duong dan tuong doi goc may chu, dau '\' hay '/', khong phan biet
** hoa thuong; dong bat dau '--' hoac '#' la chu thich; tep bat dau '--' de KSortScript cu (nap .txt nhu Lua) khong hong).
** Ap dung o lua4_dofile (= Include cua C++) va ham Lua L4_DuongDanMoi(p) (shim dofile) KHI tep goc khong ton tai.
** Core (KSortScript) doc cung tep de dang ky ID bam cu -> cung script (trap/settings goi theo ID). */
#define L4_ALIAS_BUCKETS	8192
typedef struct L4Alias { char* cu; char* moi; struct L4Alias* next; } L4Alias;
static L4Alias* s_alias_bucket[L4_ALIAS_BUCKETS];
static int s_alias_da_doc = 0;
static int s_alias_n = 0;

/* chuan hoa khoa bi danh: '\'->'/', ASCII thuong, bo '/' dau, bo ".\" dau */
static void l4_alias_norm(const char* in, char* out, size_t cap)
{
	size_t i = 0, o = 0;
	while (in[i] == '\\' || in[i] == '/' || (in[i] == '.' && (in[i + 1] == '\\' || in[i + 1] == '/'))) i += (in[i] == '.') ? 2 : 1;
	for (; in[i] && o + 1 < cap; i++)
	{
		char ch = in[i];
		if (ch == '\\') ch = '/';
		else if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + 32);
		out[o++] = ch;
	}
	while (o > 0 && (out[o - 1] == ' ' || out[o - 1] == '\r' || out[o - 1] == '\n' || out[o - 1] == '\t')) o--;
	out[o] = 0;
}

static void l4_alias_doc(void)
{
	FILE* f;
	char dong[2048];
	if (s_alias_da_doc) return;
	s_alias_da_doc = 1;
	f = fopen("script\\_duongdan_cu.txt", "rb");
	if (f == NULL) return;
	while (fgets(dong, sizeof(dong), f))
	{
		char* p = dong;
		char* eq;
		char cu[1024], moi[1024];
		unsigned h;
		L4Alias* a;
		while (*p == ' ' || *p == '\t') p++;
		if (p[0] == '-' && p[1] == '-') continue;
		if (*p == '#' || *p == '\r' || *p == '\n' || *p == 0) continue;
		eq = strchr(p, '=');
		if (eq == NULL) continue;
		*eq = 0;
		l4_alias_norm(p, cu, sizeof(cu));
		l4_alias_norm(eq + 1, moi, sizeof(moi));
		if (cu[0] == 0 || moi[0] == 0 || strcmp(cu, moi) == 0) continue;
		h = l4_fnv(cu) % L4_ALIAS_BUCKETS;
		a = (L4Alias*)calloc(1, sizeof(L4Alias));
		if (a == NULL) break;
		a->cu = _strdup(cu); a->moi = _strdup(moi);
		a->next = s_alias_bucket[h];
		s_alias_bucket[h] = a;
		s_alias_n++;
	}
	fclose(f);
}

/* rel (da chuan hoa) -> duong dan moi (da chuan hoa) hoac NULL */
static const char* l4_alias_tra(const char* rel)
{
	L4Alias* a;
	l4_alias_doc();
	if (s_alias_n == 0) return NULL;
	for (a = s_alias_bucket[l4_fnv(rel) % L4_ALIAS_BUCKETS]; a; a = a->next)
		if (strcmp(a->cu, rel) == 0) return a->moi;
	return NULL;
}

/* full (tuyet doi hoac tuong doi) khong ton tai -> thu bi danh; tra 1 va ghi duong dan moi vao out */
static int l4_alias_doi(const char* full, char* out, size_t cap)
{
	char norm[1024];
	const char* rel;
	const char* moi;
	size_t i, o;
	if (strlen(full) >= sizeof(norm)) return 0;
	for (i = 0; full[i]; i++) { char ch = full[i]; if (ch == '\\') ch = '/'; else if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + 32); norm[i] = ch; }
	norm[i] = 0;
	rel = strstr(norm, "/scriptjx2/");
	if (rel == NULL) rel = strstr(norm, "/script/");
	if (rel != NULL) rel++;
	else if (strncmp(norm, "scriptjx2/", 10) == 0 || strncmp(norm, "script/", 7) == 0) rel = norm;
	else return 0;
	moi = l4_alias_tra(rel);
	if (moi == NULL) return 0;
	o = (size_t)(rel - norm);					/* phan dau (goc) giu nguyen byte goc */
	if (o + strlen(moi) + 1 > cap) return 0;
	memcpy(out, full, o);
	for (i = 0; moi[i]; i++) out[o + i] = (moi[i] == '/') ? '\\' : moi[i];
	out[o + i] = 0;
	return 1;
}

/* Lua: L4_DuongDanMoi(p) -> duong dan moi hoac nil (shim dofile dung khi loadfile khong mo duoc) */
static int l4_b_duongdanmoi(lua_State* L)
{
	char out[1024];
	const char* p = luaL_checkstring(L, 1);
	if (l4_alias_doi(p, out, sizeof(out))) lua_pushstring(L, out); else lua_pushnil(L);
	return 1;
}
LUA_API int lua4_alias_count(void) { l4_alias_doc(); return s_alias_n; }
LUA_API int lua4_alias_doi(const char* full, char* out, int cap) { return l4_alias_doi(full, out, (size_t)cap); }

''')
c = c.replace(ANCHOR, ALIAS + ANCHOR)

# 2) lua4_dofile: thu bi danh khi tep khong ton tai
OLD = crlf('''LUA_API int lua4_dofile(lua_State* L, const char* filename)
{
	int st;
	if (filename != NULL && l4_inc_enabled())	/* [LUA54 06/09] cache Include 2a + 2b, xem khoi tren */
	{
		int r = l4_inc_load(L, filename);
		if (r > 0) return lua4_call(L, 0, LUA_MULTRET);
		if (r < 0) return -r;
	}
	st = luaL_loadfilex(L, filename, "t");
''')
NEW = crlf('''LUA_API int lua4_dofile(lua_State* L, const char* filename)
{
	int st;
	char moi[1024];
	if (filename != NULL)					/* [SAPXEP 06/09] tep goc khong co -> bi danh duong dan cu */
	{
		struct _stat64 stt;
		if (_stat64(filename, &stt) != 0 && l4_alias_doi(filename, moi, sizeof(moi)))
			filename = moi;
	}
	if (filename != NULL && l4_inc_enabled())	/* [LUA54 06/09] cache Include 2a + 2b, xem khoi tren */
	{
		int r = l4_inc_load(L, filename);
		if (r > 0) return lua4_call(L, 0, LUA_MULTRET);
		if (r < 0) return -r;
	}
	st = luaL_loadfilex(L, filename, "t");
''')
once(c, OLD, "lua4_dofile")
c = c.replace(OLD, NEW)

# 3) dang ky L4_DuongDanMoi trong baselibopen
OLD = crlf('''	lua_register(L, "randomseed", l4_b_randomseed);
''')
once(c, OLD, "register randomseed")
c = c.replace(OLD, OLD + crlf('''	lua_register(L, "L4_DuongDanMoi", l4_b_duongdanmoi);	/* [SAPXEP 06/09] bi danh duong dan cho shim dofile */
'''))
wr(C, c)
print("lua4compat.c alias OK; CRLF", c.count("\r\n"), "LF-only", c.count("\n") - c.count("\r\n"))

# 4) shim dofile: neu loadfile khong mo duoc -> thu L4_DuongDanMoi
t = rd(LUA)
OLD = "_G.dofile = function(path)\r\n\tlocal f, e = loadfile(path, \"t\")\r\n\treturn chayChunk(f, e)\r\nend\r\n"
once(t, OLD, "shim dofile")
NEW = ("_G.dofile = function(path)\r\n\tlocal f, e = loadfile(path, \"t\")\r\n"
       "\tif f == nil and type(path) == \"string\" and type(_G.L4_DuongDanMoi) == \"function\" then\r\n"
       "\t\tlocal moi = _G.L4_DuongDanMoi(path)\t-- [SAPXEP 06/09] duong dan cu -> moi (script\\_duongdan_cu.txt)\r\n"
       "\t\tif moi ~= nil then f, e = loadfile(moi, \"t\") end\r\n"
       "\tend\r\n\treturn chayChunk(f, e)\r\nend\r\n")
t = t.replace(OLD, NEW)
wr(LUA, t)
print("lua4compat.lua dofile alias OK")
