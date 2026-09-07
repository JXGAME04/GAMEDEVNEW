"""patch_alias_cwd.py - Lua54Dll: tep bi danh doc theo GOC suy ra tu duong dan Include (khong phu thuoc cwd, vi engine chdir
   vao tung thu muc script luc boot); khong ghi nho ket qua 'khong co tep' vinh vien."""
import sys, ast
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
C = WT + "/Sources/Library/Lua54/lua4compat.c"
def rd(p): return open(p, "rb").read().decode("latin-1")
def wr(p, s): open(p, "wb").write(s.encode("latin-1"))
def once(t, a, w):
    n = t.count(a)
    if n != 1: raise SystemExit("ANCHOR %s: %d" % (w, n))
E = "\r\n"; B = chr(92)
c = rd(C)

# 1) l4_alias_doc(): nhan duong dan tep bi danh; chi danh dau 'da doc' khi MO DUOC tep
old = ("static void l4_alias_doc(void)" + E + "{" + E + "\tFILE* f;" + E + "\tchar dong[2048];" + E +
       "\tif (s_alias_da_doc) return;" + E + "\ts_alias_da_doc = 1;" + E +
       "\tf = fopen(\"script" + B + B + "_duongdan_cu.txt\", \"rb\");" + E + "\tif (f == NULL) return;" + E)
once(c, old, "l4_alias_doc head")
new = ("static void l4_alias_doc(const char* tep)" + E + "{" + E + "\tFILE* f;" + E + "\tchar dong[2048];" + E +
       "\tif (s_alias_da_doc) return;" + E +
       "\tf = fopen(tep ? tep : \"script" + B + B + "_duongdan_cu.txt\", \"rb\");" + E +
       "\tif (f == NULL) return;\t\t\t\t\t/* chua thay -> lan sau thu lai (engine chdir vao tung thu muc script luc boot) */" + E +
       "\ts_alias_da_doc = 1;" + E)
c = c.replace(old, new)

# 2) l4_alias_tra(): nhan them goc (prefix tuyet doi toi truoc "script/") de dung tep bi danh <goc>script\_duongdan_cu.txt
old = ("static const char* l4_alias_tra(const char* rel)" + E + "{" + E + "\tL4Alias* a;" + E + "\tl4_alias_doc();" + E)
once(c, old, "l4_alias_tra head")
new = ("static const char* l4_alias_tra(const char* rel, const char* goc_full, size_t goc_len)" + E + "{" + E + "\tL4Alias* a;" + E +
       "\tif (!s_alias_da_doc)" + E + "\t{" + E +
       "\t\tchar tep[1200];" + E +
       "\t\tif (goc_full != NULL && goc_len + 32 < sizeof(tep))" + E + "\t\t{" + E +
       "\t\t\tmemcpy(tep, goc_full, goc_len);\t\t/* giu byte goc cua duong dan (ten Han) */" + E +
       "\t\t\tstrcpy(tep + goc_len, \"script" + B + B + "_duongdan_cu.txt\");" + E +
       "\t\t\tl4_alias_doc(tep);" + E + "\t\t}" + E +
       "\t\tif (!s_alias_da_doc) l4_alias_doc(NULL);\t/* tuong doi cwd (shim dofile, goc may chu) */" + E + "\t}" + E)
c = c.replace(old, new)

# 3) l4_alias_doi(): truyen goc (phan truoc "script/") cho l4_alias_tra
old = ("\tmoi = l4_alias_tra(rel);" + E + "\tif (moi == NULL) return 0;" + E + "\to = (size_t)(rel - norm);\t\t\t\t\t/* phan dau (goc) giu nguyen byte goc */" + E)
once(c, old, "l4_alias_doi tra")
new = ("\to = (size_t)(rel - norm);\t\t\t\t\t/* phan dau (goc) giu nguyen byte goc */" + E +
       "\tmoi = l4_alias_tra(rel, full, o);" + E + "\tif (moi == NULL) return 0;" + E)
c = c.replace(old, new)

# 4) lua4_alias_count(): doc theo cwd nhu cu
old = "LUA_API int lua4_alias_count(void) { l4_alias_doc(); return s_alias_n; }" + E
once(c, old, "alias_count")
c = c.replace(old, "LUA_API int lua4_alias_count(void) { if (!s_alias_da_doc) l4_alias_doc(NULL); return s_alias_n; }" + E)
wr(C, c)
print("lua4compat.c alias-cwd OK; CRLF lines", c.count("\r\n"), "LF-only", c.count("\n") - c.count("\r\n"))
