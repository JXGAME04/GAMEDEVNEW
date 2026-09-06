import sys
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
KS = WT + "/Sources/Core/Src/KSortScript.cpp"
def rd(p): return open(p, "rb").read().decode("latin-1")
def wr(p, s): open(p, "wb").write(s.encode("latin-1"))
def eol(t): return "\r\n" if t.count("\r\n") > t.count("\n") // 2 else "\n"
def fix(s, e): return s.replace("\r\n", "\n").replace("\n", e)
def once(t, a, w):
    n = t.count(a)
    if n != 1: raise SystemExit("ANCHOR %s: %d" % (w, n))
def hi(t): return sum(1 for ch in t if ord(ch) > 127)

t = rd(KS); e = eol(t); h0 = hi(t)

# 1) khoi bi danh truoc g_IniScriptEngine
a = fix("unsigned long g_IniScriptEngine()\n{\n", e)
once(t, a, "g_IniScriptEngine head")
BLOCK = fix(r'''// ============================================================================
// [SAPXEP 06/09] BI DANH DUONG DAN SCRIPT (SAPXEP_SCRIPT_0609.md R2)
// Tep script\_duongdan_cu.txt: moi dong "--@ <cu>=<moi>" (duong dan tuong doi goc may chu,
// khong phan biet hoa thuong, '\' hay '/'). Dong khac bat dau "--" la chu thich; ca tep la
// chu thich Lua hop le nen engine cu nap nham cung vo hai.
//  - sDangKyBiDanh(): sau LoadAllScript, moi <cu> co <moi> da nap -> them node
//    ID(g_FileName2Id("\<cu>")) tro cung script index. Trap trong Region_S.dat, cot script
//    trong settings, g_GetScript(ten cu) deu bam theo duong dan cu -> van tim duoc.
//  - sBiDanh_TenCu(): ten moi -> ten cu, de g_IsJx2Script (ngu nghia ham JX2 theo tien to
//    duong dan) van dung sau khi doi cho tep.
//  Lua54Dll doc cung tep cho Include/dofile khi tep goc khong ton tai (lua4_dofile).
// ============================================================================
#include <vector>
#include <string>
struct KBiDanhScript { std::string szCu; std::string szMoi; };
static std::vector<KBiDanhScript>	s_BiDanh;
static int							s_nBiDanhDaDoc = 0;

static void sBiDanh_ChuanHoa(std::string& s)
{
	// -> "\thu\muc\tep.lua" chu thuong, bo khoang trang dau/cuoi, bo ".\" va "/" dau
	while (!s.empty() && (s[0] == ' ' || s[0] == '\t')) s.erase(0, 1);
	while (!s.empty() && (s[s.size() - 1] == ' ' || s[s.size() - 1] == '\t' || s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n')) s.erase(s.size() - 1);
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '/') s[i] = '\\';
		else if (s[i] >= 'A' && s[i] <= 'Z') s[i] = (char)(s[i] + 32);
	}
	while (s.size() >= 2 && s[0] == '.' && s[1] == '\\') s.erase(0, 2);
	while (!s.empty() && s[0] == '\\') s.erase(0, 1);
	if (!s.empty()) s.insert(0, "\\");
}

static void sDocBiDanh()
{
	if (s_nBiDanhDaDoc) return;
	s_nBiDanhDaDoc = 1;
	s_BiDanh.clear();
	FILE* f = fopen("script\\_duongdan_cu.txt", "rb");
	if (!f) return;
	char szDong[2048];
	while (fgets(szDong, sizeof(szDong), f))
	{
		if (strncmp(szDong, "--@", 3) != 0) continue;
		char* p = szDong + 3;
		char* eq = strchr(p, '=');
		if (!eq) continue;
		*eq = 0;
		KBiDanhScript bd;
		bd.szCu = p; bd.szMoi = eq + 1;
		sBiDanh_ChuanHoa(bd.szCu); sBiDanh_ChuanHoa(bd.szMoi);
		if (bd.szCu.size() <= 1 || bd.szMoi.size() <= 1 || bd.szCu == bd.szMoi) continue;
		s_BiDanh.push_back(bd);
	}
	fclose(f);
}

// ten moi (dang "\script\...", chu thuong) -> ten cu, hoac NULL
static const char* sBiDanh_TenCu(const char* szMoi)
{
	if (!szMoi || !szMoi[0]) return NULL;
	sDocBiDanh();
	for (size_t i = 0; i < s_BiDanh.size(); i++)
		if (_stricmp(s_BiDanh[i].szMoi.c_str(), szMoi) == 0)
			return s_BiDanh[i].szCu.c_str();
	return NULL;
}

static void sDangKyBiDanh()
{
	s_nBiDanhDaDoc = 0;					// ReLoadAllScript doc lai tep
	sDocBiDanh();
	int nDangKy = 0, nThieu = 0, nTrung = 0;
	for (size_t i = 0; i < s_BiDanh.size(); i++)
	{
		KSortScriptNode nodeMoi, nodeCu;
		nodeMoi.SetScriptID(g_FileName2Id((LPSTR)s_BiDanh[i].szMoi.c_str()));
		if (!g_ScriptBinTree.Find(nodeMoi)) { nThieu++; continue; }
		nodeCu.SetScriptID(g_FileName2Id((LPSTR)s_BiDanh[i].szCu.c_str()));
		if (g_ScriptBinTree.Find(nodeCu)) { nTrung++; continue; }		// ten cu van con tep that -> khong de
		nodeCu.SetScriptIndex(nodeMoi.GetScriptIndex());
		g_ScriptBinTree.Insert(nodeCu);
		nDangKy++;
	}
	if (!s_BiDanh.empty())
	{
		char szMsg[256];
		sprintf_s(szMsg, sizeof(szMsg), "[script] Bi danh duong dan: %d dong, dang ky ID cu %d, ten moi chua nap %d, ten cu con ton tai %d",
			(int)s_BiDanh.size(), nDangKy, nThieu, nTrung);
		printf("%s\n", szMsg);
		g_DebugLog((LPSTR)"%s", szMsg);
	}
}

''', e)
t = t.replace(a, BLOCK + a)

# 2) goi sDangKyBiDanh sau LoadAllScript
a = fix('#endif\n\tsGhiThongKeNapScript(nLoaded, GetTickCount() - dwT0);\n', e)
once(t, a, "sGhiThongKeNapScript call")
t = t.replace(a, fix('#endif\n\tsDangKyBiDanh();				// [SAPXEP 06/09] ID cu -> script moi\n\tsGhiThongKeNapScript(nLoaded, GetTickCount() - dwT0);\n', e))

# 3) g_IsJx2Script: kiem ca ten cu
a = fix('''	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL)
			return 1;
	}
	return 0;
''', e)
once(t, a, "g_IsJx2Script body")
t = t.replace(a, fix('''	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	const char* szCu = sBiDanh_TenCu(szName);	// [SAPXEP 06/09] tep da doi cho: xet theo ten cu
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL)
			return 1;
		if (szCu && strstr(szCu, szJx2[i]) != NULL)
			return 1;
	}
	return 0;
''', e))

# 4) LoadScriptInDirectory: bo qua ten bat dau '_' (WIN32)
a = fix('''		if(strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0)	{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
''', e)
once(t, a, "findfirst skip")
t = t.replace(a, a + fix('''		if (FindData.name[0] == '_')	// [SAPXEP 06/09] bo qua thu muc/tep bat dau '_' (_duongdan_cu.txt, _luutru, ...)
		{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
''', e))
assert hi(t) == h0, "high bytes changed"
wr(KS, t)
print("KSortScript.cpp alias OK; high bytes", h0)
