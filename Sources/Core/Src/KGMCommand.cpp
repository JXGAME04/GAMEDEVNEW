#include "KCore.h"
#include "KPlayer.h"
#include "LuaFuns.h"
#include "KGMCommand.h"
#include "KSortScript.h"
#define GMCMD_GENRE_PREFIX_LEN 3
#define GM_CTRL_CMD_PREFIX '?'

#ifdef _SERVER
#include "KNpc.h"
// [BAOMAT A1 16/09] Lenh "?gm ..." qua kenh chat truoc day KHONG kiem quyen: nguoi choi nao cung chay duoc Lua tuy y
// tren may chu (ds/dw/RSF/RLS/RLAS -> mat may chu chu khong chi mat do). Nay chi tai khoan co ten trong tep
// \GmTaiKhoan.ini (canh GameServer.exe) moi duoc chay:
//   [GM]
//   TaiKhoan=gm1,gm2      (ten TAI KHOAN dang nhap, khong phai ten nhan vat; cach nhau dau phay; khong phan biet hoa/thuong)
// Khong co tep / danh sach rong = KHONG AI chay duoc (dong cua an toan). Moi lan thu (cho hay chan) deu ghi log may chu.
// Tep doc lai moi lan co lenh ?gm (hiem) nen sua danh sach khong can mo lai may chu.
static BOOL JxGmDuocPhep(int nPlayerIdx, const char* pText, int nLen)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_dwID <= 0)
		return FALSE;
	const char* szTk = Player[nPlayerIdx].GetPlayerAccount();
	const char* szNv = "";
	if (Player[nPlayerIdx].m_nIndex > 0 && Player[nPlayerIdx].m_nIndex < MAX_NPC)
		szNv = Npc[Player[nPlayerIdx].m_nIndex].Name;
	char szDs[1024];
	szDs[0] = 0;
	KIniFile ini;
	if (ini.Load("\\GmTaiKhoan.ini"))
		ini.GetString("GM", "TaiKhoan", "", szDs, sizeof(szDs));
	BOOL bOk = FALSE;
	if (szTk && szTk[0] && szDs[0])
	{
		int nTk = (int)strlen(szTk);
		const char* p = szDs;
		while (*p && !bOk)
		{
			while (*p == ',' || *p == ';' || *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
				p++;
			const char* q = p;
			while (*q && *q != ',' && *q != ';' && *q != ' ' && *q != '\t' && *q != '\r' && *q != '\n')
				q++;
			if (q > p && (int)(q - p) == nTk && _strnicmp(p, szTk, nTk) == 0)
				bOk = TRUE;
			p = q;
		}
	}
	if (nLen < 0) nLen = 0;
	if (nLen > 200) nLen = 200;
	// printf = cua so console GameServer (luon co); g_DebugLog chi toi DebugWin.exe khi dang mo.
	printf("[BAOMAT-GM] %s: tai khoan '%s' nhan vat '%s' lenh '%.*s'\n",
		bOk ? "CHO CHAY" : "CHAN", szTk ? szTk : "", szNv, nLen, pText);
	g_DebugLog((LPSTR)"[BAOMAT-GM] %s: tai khoan '%s' nhan vat '%s' lenh '%.*s'",
		bOk ? "CHO CHAY" : "CHAN", szTk ? szTk : "", szNv, nLen, pText);
	return bOk;
}
#define JX_GM_DEM_TEP	300	// [BAOMAT A1 16/09] moi token co the dai toi nLen (< 300): bo dem cu 200/100/100 tran
#define JX_GM_DEM_HAM	300
#define JX_GM_DEM_THAM	300
#else
#define JX_GM_DEM_TEP	200
#define JX_GM_DEM_HAM	100
#define JX_GM_DEM_THAM	100
#endif

static TGameMaster_Command GM_Command[]=
{
	{"DoSct",			GMDoScriptAction},				//DoSct Say("abc");
	{"ds",				GMDoScriptAction},				//DS Say("abc");

	{"dw",				GMDoWorldScriptAction},			//dw AddNews("abc");

	{"RunSctFile",		GMRunScriptFile},				//RunSctFile /Scripts/Abc.lua main 10
	{"RSF",				GMRunScriptFile},				//RSF /Scripts/Abc.lua main 10

	{"ReLoadSct",		GMReloadScriptFile},			//ReloadSct  /Scripts/Abc.lua
	{"RLS",				GMReloadScriptFile},
	
	{"ReLoadAllSct",	GMReloadAllScriptFile},			//ReloadAllSct
	{"RLAS",			GMReloadAllScriptFile},
	
};
#ifdef _DEBUG
CORE_API BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen)
#else
BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen)
#endif
{
	try
	{
		
		if (!pText)	return FALSE;
		bool	bHandled = false;
		if (nLen >= GMCMD_GENRE_PREFIX_LEN + 1 && pText[0] == GM_CTRL_CMD_PREFIX)
		{
			if ((*(unsigned int*)pText) == 0x206D673F || (*(unsigned int*)pText) == 0x204D473F)	// 0x2067642F = "/gm " "/GM "
			{
#ifdef _SERVER
				if (!JxGmDuocPhep(nPlayerIdx, pText, nLen))	// [BAOMAT A1 16/09] khong phai GM: nuot cau chat, khong chay
					return TRUE;
#endif
				bHandled = (bool)(0 != TextMsgProcessGMCmd(nPlayerIdx, pText + GMCMD_GENRE_PREFIX_LEN + 1,
					nLen - GMCMD_GENRE_PREFIX_LEN - 1));
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		return FALSE;
	}
	catch(...)
	{
		printf("Catch TextGMFilter 执行GM指令,发生异常!\n");//edit by phong kieu catch TextGMFilter
		return FALSE;
	}

}

BOOL TextMsgProcessGMCmd(int nPlayerIdx, const char * pGMCmd, int nLen)
{
	if (nLen <= 0 || !pGMCmd) return FALSE;
	char szCmd[20];
	const char* pStart = strstr(pGMCmd, " ");
	int nTempLen = nLen;
	
#ifdef _SERVER
	// [BAOMAT A1 16/09] ten lenh dai hon bo dem 20 byte -> truoc day tran ngan xep; strstr cung co the chay qua nLen
	// (cau chat khong ket thuc bang NUL) nen chi nhan dau cach nam trong nLen.
	if (NULL != pStart && (int)(pStart - pGMCmd) < nLen)
		nTempLen = (int)(pStart - pGMCmd);
	if (nTempLen <= 0 || nTempLen >= (int)sizeof(szCmd))
		return FALSE;
	memcpy(szCmd, pGMCmd, nTempLen);
	szCmd[nTempLen] = 0;
#else
	if (NULL == pStart)
	{
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nLen] = 0;
	}
	else
	{	nTempLen = pStart - pGMCmd;
		memcpy(szCmd, pGMCmd, nTempLen);
		szCmd[nTempLen] = 0;
	}
#endif
	for(int i  = 0; i < sizeof(GM_Command) / sizeof(TGameMaster_Command); i ++)
	{
		if (strcmp(GM_Command[i].Command, szCmd) == 0)
			return ProcessGMCommand(nPlayerIdx, GM_Command[i].eCommandId, pGMCmd + nTempLen + 1, nLen - nTempLen - 1);
	}
		
	return FALSE;
}

BOOL  ProcessGMCommand(int nPlayerIdx, EGameMasterCommand eCommand, const char * pParam, int nLen)
{
	
	switch(eCommand)
	{
	
	case GMDoWorldScriptAction:
		{
			if (nLen <=0 || nLen >= 300)
				return FALSE;
			char szScriptAction[300];
			memcpy(szScriptAction, pParam, nLen);
			szScriptAction[nLen] = 0;
			BOOL bResult = FALSE;
			KLuaScript WorldScript;
			WorldScript.Init();
			WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());
			
			if (WorldScript.LoadBuffer((PBYTE)szScriptAction, nLen))
			{
				bResult = WorldScript.ExecuteCode();	
			}
			return bResult;

		}break;

	case GMDoScriptAction:
		{
			if (nLen <= 0 || nLen >= 300) 
				return FALSE;

			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)
				return FALSE;
			char szScriptAction[300];
			memcpy(szScriptAction, pParam, nLen);
			szScriptAction[nLen] = 0;
			Player[nPlayerIdx].DoScript(szScriptAction);
			return TRUE;

		}break;

	case GMRunScriptFile:
		{
			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)
				return FALSE;
#ifdef _SERVER
			if (nLen <= 0 || nLen >= 300)	// [BAOMAT A1 16/09] nhu ds/dw: tham so phai < 300 de vua bo dem
				return FALSE;
#endif
			char szScriptFile[JX_GM_DEM_TEP];
			char szScriptFun[JX_GM_DEM_HAM];
			char szScriptParam[JX_GM_DEM_THAM];
			int nBufLen = GetNextUnit(pParam, ' ', nLen, szScriptFile);
			if (szScriptFile[0] == 0) return FALSE;

			int nBufLen1 = GetNextUnit(pParam + nBufLen, ' ', nLen - nBufLen, szScriptFun);
			if (szScriptFun[0] == 0) return FALSE;
			
			GetNextUnit(pParam + nBufLen + nBufLen1, ' ', nLen - nBufLen - nBufLen1, szScriptParam);
			return Player[nPlayerIdx].ExecuteScript(szScriptFile, szScriptFun, szScriptParam);
			
		}break;
	case GMReloadScriptFile:
		{
			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)	return FALSE;
#ifdef _SERVER
			if (nLen <= 0 || nLen >= 300)	// [BAOMAT A1 16/09]
				return FALSE;
#endif
			char szScriptFile[JX_GM_DEM_TEP];
			GetNextUnit(pParam, ' ', nLen, szScriptFile);
			if (szScriptFile[0] == 0) return FALSE;
			ReLoadScript(szScriptFile);
			return TRUE;

		}break;
	case GMReloadAllScriptFile:
		{
			return ReLoadAllScript();
		}break;
	}

	return FALSE;
}

int GetNextUnit(const char * szString , const char cDiv, int nLen, char * szResult)
{
	szResult[0] = 0;
	if (nLen <= 0 || szString == NULL)
		return FALSE;
	
	char * pChar = (char *)szString;
	int i = 0;
	int j = 0;
	BOOL bFind = FALSE;

	while (*(pChar + i) == cDiv && i < nLen) i++;
	
	while(i  < nLen && *(pChar + i) != '\0')
	{
		if (*(pChar + i) == cDiv) 
		{
			szResult[j] = 0;
			return i;
		}
		else
			szResult[j++] = *(pChar + i);
		i ++;
	}

	szResult[j] = 0;
	return i;	
}
