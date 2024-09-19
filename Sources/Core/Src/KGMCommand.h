#ifndef KGMCOMMAND_H
#define KGMCOMMAND_H

#include "KCore.h"
enum	EGameMasterCommand
{
		GMDoScriptAction,			
		GMDoWorldScriptAction,		
		GMRunScriptFile,
		GMReloadScriptFile,
		GMReloadAllScriptFile,
};

struct TGameMaster_Command
{
	char				Command[20];
	EGameMasterCommand	 eCommandId;
};

extern int GetNextUnit(const char * szString , const char cDiv, int nLen, char * szResult);
extern BOOL TextMsgProcessGMCmd(int nPlayerIdx, const char * pGMCmd, int nLen);
#ifdef _DEBUG
extern CORE_API BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen);
#else
extern BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen);
#endif

extern BOOL  ProcessGMCommand(int nPlayerIdx, EGameMasterCommand eCommand, const char * pParam, int nLen);

#endif
