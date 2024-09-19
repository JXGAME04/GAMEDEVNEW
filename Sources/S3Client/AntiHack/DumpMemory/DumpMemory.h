#ifndef Dump_Dump_H
#define Dump_Dump_H
#include <fstream>
#include <string>

#define MAX_DUMP_SIZE 16      // << Quantidade de Hex contados
#define MAX_PROCESS_DUMP 155  // << Atualizar Quantidade de Hacks

typedef struct ANITHACK_PROCDUMP
{
	unsigned int m_aOffset;
	unsigned char m_aMemDump[MAX_DUMP_SIZE];
} *PANITHACK_PROCDUMP;

extern ANITHACK_PROCDUMP g_ProcessesDumps[MAX_PROCESS_DUMP];

void SystemProcessesScan();
void ProtectionMainDumpMemory();
bool ScanProcessMemory(HANDLE hProcess);
#endif