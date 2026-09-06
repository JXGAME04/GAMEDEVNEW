//////////////////////////////////////////////////////////////////////////////
// RelayTaskCentre.h  (S3Relay)  [RELAYHT 06/09]
//
// TRUNG TAM TAC VU THEO LICH -- port theo ban Linux (lop KTaskCentre trong
// s3relay_y, tep ./swordonline/multiserver/S3Relay/TaskCentre.cpp).
//
// Ban Linux doc `relaysetting\task\tasklist.ini`:
//     [List]
//     Count=128
//     [Task_0]
//     TaskFile=announce.lua
//     ExcutedCount=-1
// Moi tep tac vu la mot kich ban Lua trong `relaysetting\task\`, khai bao lich
// ngay trong ham TaskShedule() (dung: ban goc viet thieu chu 'c'):
//     function TaskShedule()
//         TaskName("Chien dich Tuong Duong 13:00");
//         TaskInterval(1440);      -- so PHUT giua hai lan chay
//         TaskTime(12, 50);        -- gio:phut lan chay dau trong ngay
//         TaskCountLimit(0);       -- 0 = khong gioi han so lan
//     end
//     function TaskContent()  ...  end     -- than tac vu, goi khi den gio
//     function GameSvrConnected(dwGameSvrIP) end
//     function GameSvrReady(dwGameSvrIP) end
//
// Ben ta giu NGUYEN ten ham va y nghia tham so de kich ban ban Linux chep sang
// chay duoc (chi phai chuyen cu phap Lua 4 -> 5.4 nhu cay script chinh).
//////////////////////////////////////////////////////////////////////////////
#ifndef RELAYTASKCENTRE_H
#define RELAYTASKCENTRE_H

#include "../../Engine/src/KWin32.h"
#include "../../Engine/src/KLuaScript.h"

#define TC_LIST_FILE	".\\relaysetting\\task\\tasklist.ini"
#define TC_TASK_DIR		"\\relaysetting\\task\\"
#define TC_MAX_TASK		512

BOOL	TaskCentre_Init();			// doc tasklist.ini, nap va chay TaskShedule() tung tac vu
void	TaskCentre_Uninit();
void	TaskCentre_Tick();			// goi moi giay tu WM_TIMER
int		TaskCentre_Reload();		// nap lai danh sach (lenh nap nong)
int		TaskCentre_GetCount();

// Bao cho moi tac vu biet mot GameServer vua noi / vua san sang.
void	TaskCentre_OnGameSvrConnected(DWORD dwIP);
void	TaskCentre_OnGameSvrReady(DWORD dwIP);

extern TLua_Funcs	g_TaskCentreFuns[];
int					g_GetTaskCentreFunNum();

#endif // RELAYTASKCENTRE_H
