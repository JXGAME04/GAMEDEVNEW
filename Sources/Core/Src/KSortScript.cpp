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

unsigned long g_IniScriptEngine()
{
	g_szCurScriptDir[0] = 0;
	nCurrentScriptNum = 0;
	g_ScriptBinTree.ClearList();
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
const char * g_GetScriptNameByState(Lua_State* L)
{
	static std::map<Lua_State*, int> s_mapState;
	if (!L)
		return NULL;
	std::map<Lua_State*, int>::iterator it = s_mapState.find(L);
	if (it != s_mapState.end() && it->second >= 0 && it->second < (int)nCurrentScriptNum
		&& g_ScriptSet[it->second].m_LuaState == L)
		return g_ScriptSet[it->second].m_szScriptName;
	for (unsigned int i = 0; i < nCurrentScriptNum && i < MAX_SCRIPT_IN_SET; i++)
	{
		if (g_ScriptSet[i].m_LuaState == L)
		{
			s_mapState[L] = (int)i;
			return g_ScriptSet[i].m_szScriptName;
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
	};
	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL)
			return 1;
	}
	return 0;
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


