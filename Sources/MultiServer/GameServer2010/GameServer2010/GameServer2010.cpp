// GameServer.cpp : Defines the entry point for the console application.
//

/*#ifdef _STANDALONE
#ifndef __linux
//#include <winsock2.h>
#endif
#endif*/
#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include "StdAfx.h"
#include "KSOServer.h"

//extern KSwordOnLineSever g_SOServer;
/*#ifdef __linux
#include <unistd.h>
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
void init_daemon(void) { 
	int pid; 
	int i; 
	if(pid=fork()) exit(0);			//是父进程，结束父进程 
	else if(pid< 0) exit(1);		//fork失败，退出 
//是第一子进程，后台继续执行 
	setsid();//第一子进程成为新的会话组长和进程组长 
//并与控制终端分离 
	if(pid = fork()) exit(0);//是第一子进程，结束第一子进程 
	else if(pid< 0) 
	exit(1);//fork失败，退出 
//是第二子进程，继续 
//第二子进程不再是会话组长 

	for(i=0; i< NOFILE; ++i) close(i); 
	umask(0);//重设文件创建掩模 
	return; 
} 

#endif*/

void ClearConsoleToColors(int ForgC, int BackC);        //function declaration

enum Colors{
	black,          //  0 text color - multiply by 16, for background colors
	dark_blue,      //  1
	dark_green,     //  2
	dark_cyan,      //  3
	dark_red,       //  4
	dark_magenta,   //  5
	dark_yellow,    //  6
	light_gray,     //  7
	dark_gray,      //  8
	light_blue,     //  9
	light_green,    // 10
	light_cyan,     // 11
	light_red,      // 12
	light_magenta,  // 13
	light_yellow,   // 14
	white           // 15
};

int g_nPort = 0;
static int countdownCheckLic = 0;


int main(int argc, char* argv[])
{	

	ClearConsoleToColors(4,0);         //function call to change console background color

	BOOL bRunning = TRUE;
	if (argc == 2)
	{
		g_nPort = atoi(argv[1]);
	}
/*#ifdef __linux
	init_daemon();
#endif*/

	if (!g_SOServer.Init())
		return 0;

	while(bRunning)
	{
		bRunning = g_SOServer.Breathe();
	}

	g_SOServer.Release();
	return 1;
}


void ClearConsoleToColors(int ForgC, int BackC) //edit by phong kieu change GS console corlor
{
     WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
     //This is used to get the handle to current output buffer.
     
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     //This is used to reset the carat/cursor to the top left.
     
     COORD coord = {0, 0};
                      //This is a return value indicating how many characterss were written
                        //   It is not used but we need to capture this since it will be
                          //   written anyway (passing NULL causes an access violation).
                          
     DWORD count;
 
                                   //This is a structure containing all the console info
                          // It is used here to find the size of the console.
                          
     CONSOLE_SCREEN_BUFFER_INFO csbi;
                     //Now the current color will be set by this handle
                     
     SetConsoleTextAttribute(hStdOut, wColor);
     
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
                              //This fills the buffer with a given character (in this case 32=space).
          FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
 
          FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
                              //This will set our cursor position for the next print statement
          SetConsoleCursorPosition(hStdOut, coord);
     }
     return;
}

#ifdef _STANDALONE
extern "C"
{
void lua_outerrmsg(const char * szerrmsg)
{
	fprintf(stderr, szerrmsg);
}
};
#endif