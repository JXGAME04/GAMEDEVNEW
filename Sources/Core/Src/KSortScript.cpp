#include "KCore.h"
#include "KSortScript.h"
#include "LuaFuns.h"
#include "KFilePath.h"
#include "KDebug.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif
//#include "Shlwapi.h"
//Author: Fong Ki襲
//Date: 2021
KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET];
KScriptBinTree g_ScriptBinTree;
unsigned int   nCurrentScriptNum;
char g_szCurScriptDir[MAX_PATH];

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir);

int	operator<(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() < ScriptRight.GetScriptID();
};
	
int operator==(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() == ScriptRight.GetScriptID();
};

static unsigned long LoadAllScript(char * szFilePath)
{
	printf("====== [Start Loading Script : %d] ======\n", nCurrentScriptNum);
	g_SetFilePath("\\");
	char szRootPath[100];
	char szOldRootPath[MAX_PATH];
//	GetCurrentDirectory(MAX_PATH, szOldRootPath);
	getcwd(szOldRootPath, MAXPATH);
	g_GetFullPath(szRootPath,szFilePath);
	LoadScriptInDirectory(szRootPath, "");
	chdir(szOldRootPath);
//	SetCurrentDirectory(szOldRootPath);
	printf("====== [Total ScriptLoaded : %d] ======\n", nCurrentScriptNum);
	printf("===============================\n");
	return nCurrentScriptNum;
}

// [LUA54 06/09] Thong ke nap script + cache Include cua Lua54Dll (PHANTICH_NANG_LUA54_0509.md muc 13).
// Lay ham qua GetProcAddress de Core van chay voi Lua54Dll cu (chua co ham) - khi do chi in thoi gian.
static void sGhiThongKeNapScript(unsigned long nLoaded, DWORD dwMs)
{
	typedef void (*PFN_INCSTATS)(long long*, long long*, long long*, long long*, long long*);
	long long nStateHit = 0, nGlobalHit = 0, nCompile = 0, nBytesSaved = 0, nBytesCode = 0;
	HMODULE hLua = GetModuleHandleA("Lua54Dll.dll");
	PFN_INCSTATS pfn = hLua ? (PFN_INCSTATS)GetProcAddress(hLua, "lua4_inc_stats") : NULL;
	if (pfn)
		pfn(&nStateHit, &nGlobalHit, &nCompile, &nBytesSaved, &nBytesCode);
	char szMsg[512];
	sprintf_s(szMsg, sizeof(szMsg),
		"[script] LoadAllScript: %lu tep, %lu ms; cache Include: bien dich %lld, dung lai cung state %lld, bytecode chung %lld, bo qua phan tich %lld KB, bytecode giu %lld KB",
		nLoaded, dwMs, nCompile, nStateHit, nGlobalHit, nBytesSaved / 1024, nBytesCode / 1024);
	printf("%s\n", szMsg);
	g_DebugLog((LPSTR)"%s", szMsg);
	// [LUA54 06/09 toi] IncludeOnce (@IncludeOnce) + che do mot state (LUA54_MOT_STATE)
	{
		typedef long long (*PFN_ONCE)(void);
		typedef int (*PFN_MOT)(void);
		PFN_ONCE pfnOnce = hLua ? (PFN_ONCE)GetProcAddress(hLua, "lua4_inc_once_skip") : NULL;
		PFN_MOT pfnMot = hLua ? (PFN_MOT)GetProcAddress(hLua, "lua4_mot_state") : NULL;
		if (pfnOnce || pfnMot)
		{
			sprintf_s(szMsg, sizeof(szMsg), "[script] IncludeOnce bo qua %lld lan; che do mot state: %s",
				pfnOnce ? pfnOnce() : 0LL, (pfnMot && pfnMot()) ? "BAT (LUA54_MOT_STATE=1)" : "tat");
			printf("%s\n", szMsg);
			g_DebugLog((LPSTR)"%s", szMsg);
		}
	}
}

// ============================================================================
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
#include <map>
static std::map<Lua_State*, int>	s_mapJx2;		// [SAPXEP 06/09] cache g_IsJx2Script theo state
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
	s_mapJx2.clear();
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

unsigned long g_IniScriptEngine()
{
	g_szCurScriptDir[0] = 0;
	nCurrentScriptNum = 0;
	g_ScriptBinTree.ClearList();
	DWORD dwT0 = GetTickCount();		// [LUA54 06/09] do thoi gian nap toan bo script
	unsigned long nLoaded = LoadAllScript("\\script");
#ifdef _SERVER
	// JX2 port: script bang hoi nam NGOAI \script (o scriptjx2\) de khong
	// bi chay lung tung luc boot chung. Nhung NPC lanh dia + bao tri tac
	// phuong goi chung theo ScriptID (g_GetScript KHONG tu nap) nen phai
	// co mat trong cay. Chi nap 2 thu muc can cho duong bam NPC/MAINTAIN_R.
	// nap TRON tong_vn (de quy ca npc\ + workshop\ + map\): tong.lua can
	// cho bao tri ngay/tuan, tong_mix cho cac duong _G, npc/workshop cho
	// duong bam NPC. KHONG nap rieng thu muc con nua de khoi trung ID.
	nLoaded = LoadAllScript("\\scriptjx2\\tong_vn");
#endif
	sDangKyBiDanh();				// [SAPXEP 06/09] ID cu -> script moi
	sGhiThongKeNapScript(nLoaded, GetTickCount() - dwT0);
	return nLoaded;
}

const KScript * g_GetScript(DWORD dwScriptId)
{
	KSortScriptNode ScriptNode;
	ScriptNode.SetScriptID(dwScriptId);
	if (g_ScriptBinTree.Find(ScriptNode))
	{
		return ScriptNode.GetScript();
	}
	return NULL;
}
const KScript * g_GetScript(const char * szRelativeScriptFile)
{
	DWORD dwScriptId = g_FileName2Id((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}

// [JX2COMPAT 22/08] g_ScriptSet[i].m_szScriptName = duong dan tuong doi chu thuong
// ("\script\missions\bw\bwhead.lua", LoadScriptToSortList). Quet tuyen tinh lan dau,
// nho lai theo state (script chi nap luc boot; ReLoadScript giu nguyen state).
#include <map>
// [LUA54 06/09 toi] L co the la COROUTINE (SayWait) hay thread cua che do mot state: hoi Lua54Dll thread script so huu
static Lua_State* sLuaOwner(Lua_State* L)
{
	typedef Lua_State* (*PFN_L4OWNER)(Lua_State*);
	static PFN_L4OWNER s_pfnOwner = NULL;
	static int s_nThu = 0;
	if (!s_nThu)
	{
		s_nThu = 1;
		HMODULE h = GetModuleHandleA("Lua54Dll.dll");
		if (h) s_pfnOwner = (PFN_L4OWNER)GetProcAddress(h, "lua4_owner");
	}
	return s_pfnOwner ? s_pfnOwner(L) : NULL;
}

const char * g_GetScriptNameByState(Lua_State* L)
{
	static std::map<Lua_State*, int> s_mapState;
	if (!L)
		return NULL;
	std::map<Lua_State*, int>::iterator it = s_mapState.find(L);
	if (it != s_mapState.end() && it->second >= 0 && it->second < (int)nCurrentScriptNum)
	{
		if (g_ScriptSet[it->second].m_LuaState == L)
			return g_ScriptSet[it->second].m_szScriptName;
		Lua_State* L2 = sLuaOwner(L);
		if (L2 && L2 != L && g_ScriptSet[it->second].m_LuaState == L2)
			return g_ScriptSet[it->second].m_szScriptName;
	}
	for (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)
	{
		if (g_ScriptSet[i].m_LuaState == L)
		{
			s_mapState[L] = (int)i;
			return g_ScriptSet[i].m_szScriptName;
		}
	}
	{
		Lua_State* L2 = sLuaOwner(L);
		if (L2 && L2 != L)
		{
			for (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)
			{
				if (g_ScriptSet[i].m_LuaState == L2)
				{
					s_mapState[L] = (int)i;
					return g_ScriptSet[i].m_szScriptName;
				}
			}
		}
	}
	return NULL;
}

// Cac cay script chep nguyen ban tu Linux (JX2): SetPunish/GetTeamMember/AddSkillState/
// GetGameTime co ngu nghia KHAC JX1 - ham Lua re nhanh theo co nay. Include() dung
// state cua tep goi nen NPC JX1 goi luong JX2 (dichquan.lua -> posthouse.lua) phai liet ke.
int g_IsJx2Script(Lua_State* L)
{
	static const char* szJx2[] = {
		"\\script\\missions\\citywar_", "\\script\\missions\\leaguematch\\", "\\script\\leaguematch\\",
		"\\script\\missions\\tong\\", "\\script\\task\\tollgate\\", "\\script\\item\\messenger\\",
		"\\script\\item\\xinshirenwu\\", "\\script\\missions\\tongwar\\", "\\script\\event\\tongwar\\",
		"\\script\\missions\\bw\\", "\\script\\missions\\bairenleitai\\", "\\script\\missions\\tongcastle\\",
		"\\script\\missions\\arena\\", "\\script\\activitysys\\", "\\script\\tong\\", "\\scriptjx2\\",
		"\\script\\global\\npcchucnang\\dichquan.lua",	// Dich Quan 7 thanh: Include posthouse.lua (Tin Su)
		"\\script\\missions\\basemission\\",	// [PORT5 23/08 phan bien F7] lib JX2 (CallNpc bNoRevive)
		// [3HD 25/08] 3 hoat dong ban Linux (fengling / challengeoftime / boss lib
		// + trigger o settings). Include() chay trong state TEP GOI nen chi can
		// liet ke cac STATE GOC; bao dong Include cua chung tu duoc phu theo.
		"\\script\\missions\\fengling_ferry\\",
		"\\script\\missions\\challengeoftime\\",
		"\\script\\missions\\boss\\",
		"\\script\\vng_feature\\",
		"\\settings\\trigger_",
		// [BDH 27/08] script Ban Dong Hanh port tu Linux: GetGameTime tra GIAY nhu goc
		"\\script\\partner\\",
		"\\script\\task\\partner\\",
		// [VIEMDE 29/08] cay Viem De Bao Tang la script Linux chep nguyen van:
		// GetTeamMember phai theo quy uoc Linux (vi tri 1 = doi truong) - cay nay
		// viet "for i = 1, GetTeamSize() do GetTeamMember(i)" o yandibaozang_main
		// :90/:134/:174 (kiem, tru ve, lap danh sach vao tran); va AddSkillState
		// phai LUON ap skill that (include.lua:37-40 buff 461/458/459).
		"\\script\\missions\\yandibaozang\\",
	};
	// [SAPXEP 06/09] nho ket qua theo state (tra bi danh la quet tuyen tinh); s_mapJx2 xoa trong sDangKyBiDanh
	std::map<Lua_State*, int>::iterator itJ = s_mapJx2.find(L);
	if (itJ != s_mapJx2.end())
		return itJ->second;
	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	const char* szCu = sBiDanh_TenCu(szName);	// tep da doi cho: xet theo ten cu
	int nJx2 = 0;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL || (szCu && strstr(szCu, szJx2[i]) != NULL))
		{
			nJx2 = 1;
			break;
		}
	}
	s_mapJx2[L] = nJx2;
	return nJx2;
}

extern int LuaIncludeFile(Lua_State * L);

static BOOL LoadScriptToSortListA(char * szRelativeFile)
{
	if (!szRelativeFile || !szRelativeFile[0]) return FALSE;
	KSortScriptNode ScriptNode ;
	ScriptNode.SetScriptID(g_FileName2Id(szRelativeFile));
	int t  =strlen(szRelativeFile);
	if (t >= 90)
		t ++;

#ifdef _DEBUG
	//strcpy(ScriptNode.m_szScriptName, szRelativeFile);
#endif
	if (nCurrentScriptNum < MAX_SCRIPT_IN_SET)
	{
		g_ScriptSet[nCurrentScriptNum].Init();
		g_ScriptSet[nCurrentScriptNum].RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		// JX2 port: script trong scriptjx2\ boc toan than trong khoi
		// "if MODEL_GAMESERVER == 1 ..." (tong.lua, tong_mix.lua...) -
		// khong dat co truoc khi chay than file thi nap xong chi con vo.
		if (strstr(szRelativeFile, "scriptjx2") != NULL)
		{
			Lua_PushNumber(g_ScriptSet[nCurrentScriptNum].m_LuaState, 1);
			lua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, "MODEL_GAMESERVER");
		}
		// [MAIL 03/09] bo dieu phoi ScriptProtocol (script_protocol\protocol_def_gs.lua /
		// protocol_def_c.lua) mo dau bang 'if MODEL_GAME* ~= 1 then return end' - dat co theo phia.
		if (strstr(szRelativeFile, "script_protocol") != NULL)
		{
			Lua_PushNumber(g_ScriptSet[nCurrentScriptNum].m_LuaState, 1);
#ifdef _SERVER
			lua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, "MODEL_GAMESERVER");
#else
			lua_setglobal(g_ScriptSet[nCurrentScriptNum].m_LuaState, "MODEL_GAMECLIENT");
#endif
		}
		g_StrCpyLen(g_ScriptSet[nCurrentScriptNum].m_szScriptName, szRelativeFile, 100);
		if (g_ScriptSet[nCurrentScriptNum].Load(szRelativeFile))
		{
		
		}
		else
		{
			g_DebugLog("[脚本]加载脚本%s，出错，该脚本无法加载！！请检查！！", szRelativeFile);
			// FIX 14/08: BO HAN slot nay. Load() tra FALSE ca khi than file
			// loi luc chay (KLuaScript::Load -> ExecuteCode); truoc day
			// nCurrentScriptNum KHONG tang nen file KE TIEP nap vao dung
			// Lua state da chay do dang: bien toan cuc + head-guard cua file
			// hong con nguyen -> Include cua file sau thanh no-op IM LANG.
			nCurrentScriptNum++;
			return FALSE;
		}
	}
	else
	{
		g_DebugLog("[脚本]严重错误!脚本数量超限制%d！请立即解决！！", MAX_SCRIPT_IN_SET);
		return FALSE;
	}
	
	ScriptNode.SetScriptIndex(nCurrentScriptNum++);
	g_ScriptBinTree.Insert(ScriptNode);
	//printf("---Total ScriptLoaded:%d --- %s ---\n", nCurrentScriptNum, szRelativeFile);
	return TRUE;
}

static BOOL LoadScriptToSortList(char * szFileName)
{
	if (!szFileName || !szFileName[0]) return FALSE;
	if (nCurrentScriptNum >= MAX_SCRIPT_IN_SET)
	{
		printf("[LoadScript Error] nCurrentScriptNum > MAX_SCRIPT_IN_SET %d --- \n!",MAX_SCRIPT_IN_SET);
		return FALSE;
	}

	int nFileNameLen = strlen(szFileName);
	
	char szRootPath[MAX_PATH];
	g_GetRootPath(szRootPath);
//	char szRelativePath[MAX_PATH];
	char *szRelativePath;
	char szCurrentDirectory[MAX_PATH];

//	GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	getcwd(szCurrentDirectory, MAX_PATH);
	szRelativePath = szCurrentDirectory + strlen(szRootPath);
//	PathRelativePathTo(szRelativePath,szRootPath, FILE_ATTRIBUTE_DIRECTORY,szCurrentDirectory , FILE_ATTRIBUTE_NORMAL );
	char szRelativeFile[MAX_PATH];
	if (szRelativePath[0] == '.' && szRelativePath[1] == '\\')
		sprintf(szRelativeFile, "%s\\%s", szRelativePath + 1, szFileName);
	else
		sprintf(szRelativeFile, "%s\\%s", szRelativePath, szFileName);
	g_StrLower(szRelativeFile);
	g_DebugLog("[Script]Loading Script %s %d", szRelativeFile, g_FileName2Id(szRelativeFile));
//
	return LoadScriptToSortListA(szRelativeFile);
//	return FALSE;
}

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir)
{
	int				nFlag;
	char			szRealDir[MAX_PATH];
#ifdef WIN32
	sprintf(szRealDir, "%s\\%s", lpszRootDir, lpszSubDir);
#else
	sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
        char *ptr = szRealDir;
        while(*ptr) { if(*ptr == '\\') *ptr = '/';  ptr++;  }
#endif

#ifdef WIN32
	if(chdir(szRealDir)) return;
	_finddata_t FindData;
	intptr_t dir = _findfirst("*.*", &FindData);
	while(dir != -1) {
		if(strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0)	{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
		if (FindData.name[0] == '_')	// [SAPXEP 06/09] bo qua thu muc/tep bat dau '_' (_duongdan_cu.txt, _luutru, ...)
		{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
		if(FindData.attrib == _A_SUBDIR)
		{
			LoadScriptInDirectory(szRealDir, FindData.name);
		}
		else
		{	
			nFlag = 0;
			for (int i = 0; i < (int)strlen(FindData.name);  i++)
			{
				if (FindData.name[i] == '.')
					break;
				if (FindData.name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir, FindData.name);
			}
			else
			{
				char szExt[50];
				if (strlen(FindData.name) >= 4) 
				{
					strcpy(szExt, FindData.name + strlen(FindData.name) - 4);
					_strupr(szExt);
					if ( (!strcmp(szExt, ".LUA")) || (!strcmp(szExt, ".TXT")))
						if (!LoadScriptToSortList(FindData.name))
							g_DebugLog("加载%s文件出错", FindData.name);
				}
			}
		}
		if(_findnext(dir, &FindData)) break;
	} 
	_findclose(dir);
	chdir(lpszRootDir);
#else
     DIR *dp;
     int i;
     struct dirent *ep;
     if(chdir(szRealDir)) return;
     dp = opendir(".");
     if(dp) {
          while(ep = readdir(dp)) {
            if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) continue;
            
            if(ep->d_type ==4) {
                LoadScriptInDirectory(szRealDir, ep->d_name);
            }
            else {
			nFlag = 0;
			for (i = 0; i < (int)strlen(ep->d_name);  i++)
			{
				if (ep->d_name[i] == '.')
					break;
				if (ep->d_name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir,ep->d_name);
			}
			else
			{
				char szExt[50];
				if (strlen(ep->d_name) >= 4)
				{
					strcpy(szExt, ep->d_name + strlen(ep->d_name) - 4);
                                        g_StrUpper(szExt);
///					_strupr(szExt);
					if ( (!strcmp(szExt, ".LUA")) || (!strcmp(szExt, ".TXT")))
						if (!LoadScriptToSortList(ep->d_name))
							g_DebugLog("加载%s文件出错", ep->d_name);
				}
			}
		}
	          }
          closedir(dp);
     }
	chdir(lpszRootDir);
#endif
}

void UnLoadScript(DWORD dwScriptID)
{
	KSortScriptNode ScriptNode;
	ScriptNode.SetScriptID(dwScriptID);
	g_ScriptBinTree.Delete(ScriptNode);
}

BOOL ReLoadScript(const char * szRelativePathScript)
{
	if (!szRelativePathScript || !szRelativePathScript[0])
		return FALSE;
	char script[MAX_PATH];
	strcpy(script, szRelativePathScript);
//	_strlwr(script);
        g_StrLower(script);
	UnLoadScript(g_FileName2Id(script));
	return LoadScriptToSortListA(script);
}

unsigned long  ReLoadAllScript()
{
	g_ScriptBinTree.ClearList();
	return g_IniScriptEngine();
}


