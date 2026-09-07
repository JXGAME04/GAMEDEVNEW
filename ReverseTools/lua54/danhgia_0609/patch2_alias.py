import sys
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
def rd(p): return open(p, "rb").read().decode("latin-1")
def wr(p, s): open(p, "wb").write(s.encode("latin-1"))
def once(t, a, w):
    n = t.count(a)
    if n != 1: raise SystemExit("ANCHOR %s: %d" % (w, n))
BSL = chr(92)          # mot dau backslash that
TAB = BSL + "t"        # hai ky tu: backslash + t (nhu trong ma C)
CR = BSL + "r"; NL = BSL + "n"
E = "\r\n"

# ---- DLL: dong "--@ cu=moi" duoc nhan; dong "--" khac va "#" bo qua
C = WT + "/Sources/Library/Lua54/lua4compat.c"
c = rd(C)
old = ("\t\twhile (*p == ' ' || *p == '" + TAB + "') p++;" + E +
       "\t\tif (p[0] == '-' && p[1] == '-') continue;" + E +
       "\t\tif (*p == '#' || *p == '" + CR + "' || *p == '" + NL + "' || *p == 0) continue;" + E)
once(c, old, "dll parser")
new = ("\t\twhile (*p == ' ' || *p == '" + TAB + "') p++;" + E +
       "\t\tif (p[0] == '-' && p[1] == '-' && p[2] == '@') p += 3;\t/* dong bi danh \"--@ cu=moi\" (ca tep la chu thich Lua hop le) */" + E +
       "\t\telse if (p[0] == '-' && p[1] == '-') continue;" + E +
       "\t\tif (*p == '#' || *p == '" + CR + "' || *p == '" + NL + "' || *p == 0) continue;" + E)
c = c.replace(old, new)
wr(C, c); print("DLL parser OK")

# ---- Core: cache g_IsJx2Script theo state
K = WT + "/Sources/Core/Src/KSortScript.cpp"
t = rd(K); h0 = sum(1 for ch in t if ord(ch) > 127)
old = ("\tconst char* szName = g_GetScriptNameByState(L);" + E + "\tif (!szName || !szName[0])" + E + "\t\treturn 0;" + E +
       "\tconst char* szCu = sBiDanh_TenCu(szName);\t// [SAPXEP 06/09] tep da doi cho: xet theo ten cu" + E +
       "\tfor (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)" + E + "\t{" + E +
       "\t\tif (strstr(szName, szJx2[i]) != NULL)" + E + "\t\t\treturn 1;" + E +
       "\t\tif (szCu && strstr(szCu, szJx2[i]) != NULL)" + E + "\t\t\treturn 1;" + E + "\t}" + E + "\treturn 0;" + E)
once(t, old, "jx2 body")
new = ("\t// [SAPXEP 06/09] nho ket qua theo state (tra bi danh la quet tuyen tinh); s_mapJx2 xoa trong sDangKyBiDanh" + E +
       "\tstd::map<Lua_State*, int>::iterator itJ = s_mapJx2.find(L);" + E +
       "\tif (itJ != s_mapJx2.end())" + E + "\t\treturn itJ->second;" + E +
       "\tconst char* szName = g_GetScriptNameByState(L);" + E + "\tif (!szName || !szName[0])" + E + "\t\treturn 0;" + E +
       "\tconst char* szCu = sBiDanh_TenCu(szName);\t// tep da doi cho: xet theo ten cu" + E +
       "\tint nJx2 = 0;" + E +
       "\tfor (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)" + E + "\t{" + E +
       "\t\tif (strstr(szName, szJx2[i]) != NULL || (szCu && strstr(szCu, szJx2[i]) != NULL))" + E +
       "\t\t{" + E + "\t\t\tnJx2 = 1;" + E + "\t\t\tbreak;" + E + "\t\t}" + E + "\t}" + E +
       "\ts_mapJx2[L] = nJx2;" + E + "\treturn nJx2;" + E)
t = t.replace(old, new)
old2 = "static std::vector<KBiDanhScript>\ts_BiDanh;" + E
once(t, old2, "s_BiDanh decl")
t = t.replace(old2, old2 + "#include <map>" + E + "static std::map<Lua_State*, int>\ts_mapJx2;\t\t// [SAPXEP 06/09] cache g_IsJx2Script theo state" + E)
old3 = "\ts_nBiDanhDaDoc = 0;\t\t\t\t\t// ReLoadAllScript doc lai tep" + E
once(t, old3, "sDangKyBiDanh head")
t = t.replace(old3, old3 + "\ts_mapJx2.clear();" + E)
assert sum(1 for ch in t if ord(ch) > 127) == h0
wr(K, t); print("Core jx2 cache OK")
