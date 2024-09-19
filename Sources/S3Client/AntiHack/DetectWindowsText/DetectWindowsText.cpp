#include "KWin32.h"
#include "DetectWindowsText.h"
#include "../../S3Config.h"

void Msg_WindowsTextScanner()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL,"WindowsText-Scan\nAn illegal choice has been detected!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL,"WindowsText-Scan\nPhat hien co dau hieu hack.!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
}

void DetectWindowsTextScanner()
{
	POINT p;
	HWND DebugerFound = 0;
	for ( int qy = 0 ; qy < 100 ; qy++)
	{
		for ( int qx = 0 ; qx < 100 ; qx++)
		{
			p.x = qx * 20;
			p.y = qy * 20;	
			DebugerFound = WindowFromPoint(p);
			char t[255];
			GetWindowTextA( DebugerFound , t , 225); 
			if (
				strstr(t,"Acelera")		||
				strstr(t,"Aclererador")	||
				strstr(t,"AutoBuff")	||
				strstr(t,"Auto_Buff")	||
				strstr(t,"BETA")		||
				strstr(t,"BYPASS")	    ||
				strstr(t,"Beta")		||
				strstr(t,"ByPass")		||
				strstr(t,"CAPOTE")	    ||
				strstr(t,"CAPOTECHEAT")	||
				strstr(t,"CATASTROPHE")	||
				strstr(t,"CHEAT")	    ||
				strstr(t,"CHEATER")	    ||
				strstr(t,"CHEATS")	    ||
				strstr(t,"CHECK")	    ||
				strstr(t,"Check")	    ||
				strstr(t,"CMD")			||
				strstr(t,"CPU")         ||	
				strstr(t,"CR4CK3R")     ||
				strstr(t,"Call")		||
				strstr(t,"Camtasia")	||
				strstr(t,"Capote")	    ||
				strstr(t,"CapoteCheat")	||
				strstr(t,"Capotecheat")	||
				strstr(t,"Catastrophe")	||
				strstr(t,"Cheat")	    ||
				strstr(t,"Cheater")	    ||
				strstr(t,"Cheats")      ||
				strstr(t,"Check")		||
				strstr(t,"Cmd")			||
				strstr(t,"Command")	    ||
				strstr(t,"Configuration") ||
				strstr(t,"Console")	    ||
				strstr(t,"Cr4ck3r")     ||
				strstr(t,"DARK")	    ||
				strstr(t,"DEMO")	    ||
				strstr(t,"DUPAR")		||
				strstr(t,"DUPE")		||
				strstr(t,"DUPER")		||
				strstr(t,"Dark TeaM")   ||
				strstr(t,"Dark")	    ||
				strstr(t,"DarkTeaM")    ||
				strstr(t,"Debug")		||
				strstr(t,"Demo")	    ||
				strstr(t,"Dupar")		||
				strstr(t,"Dupe")		||
				strstr(t,"Duper")		||
				strstr(t,"EDITOR")		||
				strstr(t,"ENGINE")	    ||
				strstr(t,"EXPLORER")	||
				strstr(t,"Editor do Registro")  ||
				strstr(t,"Editor")		||
				strstr(t,"Engine")      ||
				strstr(t,"Explorer")	||
				strstr(t,"FIND")		||
				strstr(t,"FREEZE")		||
				strstr(t,"Find")		||
				strstr(t,"Fraps")       ||
				strstr(t,"Freeze")		||
				strstr(t,"Game Speed Adjuster") ||
				strstr(t,"Gerenciador") ||
				strstr(t,"Gold Dupe")	||
				strstr(t,"HACK")		||
				strstr(t,"HACKER")		||
				strstr(t,"HIDE")		||
				strstr(t,"HIDETOOLZ")	||
				strstr(t,"Hack")		||
				strstr(t,"Hacker")		||
				strstr(t,"Happens")		||
				strstr(t,"HastyMU")		||
				strstr(t,"HastyMu")	    ||
				strstr(t,"Hide")		||
				strstr(t,"HideToolz")	||
				strstr(t,"Hit")		    ||
				strstr(t,"INJECT")	    ||
				strstr(t,"INJECTOR")	||
				strstr(t,"IP-Tools")	||
				strstr(t,"Inject")	    ||	
				strstr(t,"Injector")	||
				strstr(t,"KILL")	    ||
				strstr(t,"Kill")		||
				strstr(t,"Line")	    ||
				strstr(t,"Lipsum")		||
				strstr(t,"MANAGER")		||
				strstr(t,"MSconfig")	||
				strstr(t,"Manager")		||
				strstr(t,"Msconfig")	||
				strstr(t,"MuCheat")		||
				strstr(t,"MuPie")		||
				strstr(t,"Mucheat")		||
				strstr(t,"Olly")		||
				strstr(t,"Ollydbg")		||
				strstr(t,"Option")		||
				strstr(t,"PROCESS")     ||
				strstr(t,"PROJECT")     ||
				strstr(t,"Packager")	||
				strstr(t,"Pacotes")	    ||
				strstr(t,"Perfect")		||
				strstr(t,"Pirata")		||
				strstr(t,"Process")     ||
				strstr(t,"Project")     ||
				strstr(t,"Rat")		    ||
				strstr(t,"Record")	    ||
				strstr(t,"Recorder")	||
				strstr(t,"Regedit")		||
				strstr(t,"Registry Editor")	||
				strstr(t,"SPEED")		||
				strstr(t,"SUSPEND")		||
				strstr(t,"ScreenRecorder") ||
				strstr(t,"Simplifier")	||
				strstr(t,"Speed Gear")	||
				strstr(t,"Speed Hack")	||
				strstr(t,"Speed")		||
				strstr(t,"SpotHack")	||
				strstr(t,"Suspend")		||
				strstr(t,"Sysinternals") ||
				strstr(t,"TEAM")        ||
				strstr(t,"TOOL")        ||
				strstr(t,"TOOLZ")		||
				strstr(t,"TOOLZ")       ||
				strstr(t,"TeaM")        ||
				strstr(t,"Team")        ||
				strstr(t,"Timer1")		||
				strstr(t,"Tool")        ||
				strstr(t,"Toolz")		||
				strstr(t,"Toolz")       ||
				strstr(t,"Trade Hacker") ||
				strstr(t,"Trap")	    ||
				strstr(t,"ViCtor_5")	||
				strstr(t,"Victor_5")	||
				strstr(t,"WH")			||
				strstr(t,"WILDPROXY")	||
				strstr(t,"WINDOWS")		||
				strstr(t,"WPE PRO")		||
				strstr(t,"WebCheat")	||
				strstr(t,"WildProxy")	||
				strstr(t,"WinHex")		||
				strstr(t,"Windows")		||
				strstr(t,"Wpe Pro")		||
				strstr(t,"WpePro")		||
				strstr(t,"WriteALong")	||
				strstr(t,"XELERATOR")	||
				strstr(t,"XSpeed")		||
				strstr(t,"XTRAP")	    ||
				strstr(t,"XTrap")	    ||
				strstr(t,"Xelerator")	||
				strstr(t,"bypass")	    ||
				strstr(t,"capote")	    ||
				strstr(t,"capotecheat")	||
				strstr(t,"catastrophe")	||
				strstr(t,"cheat")       ||
				strstr(t,"cheater")	    ||
				strstr(t,"cheats")	    ||
				strstr(t,"cmd")			||
				strstr(t,"combo")		||
				strstr(t,"command")	    ||
				strstr(t,"console")	    ||
				strstr(t,"cr4ck3r")     ||
				strstr(t,"dark")	    ||
				strstr(t,"debug")       ||
				strstr(t,"demo")	    ||
				strstr(t,"dupar")		||
				strstr(t,"dupe")		||
				strstr(t,"duper")		||
				strstr(t,"editor")		||
				strstr(t,"engine")      ||
				strstr(t,"explorer")	||
				strstr(t,"find")		||
				strstr(t,"freeze")		||
				strstr(t,"gold dupe")	||
				strstr(t,"hack")		||
				strstr(t,"hacker")		||
				strstr(t,"hastyMU")		||
				strstr(t,"hide")		||
				strstr(t,"hidetoolz")	||
				strstr(t,"inject")	    ||
				strstr(t,"injector")	||
				strstr(t,"kill")	    ||
				strstr(t,"line")	    ||
				strstr(t,"manager")		||
				strstr(t,"mucheat")		||
				strstr(t,"olly")		||
				strstr(t,"ollydbg")		||
				strstr(t,"packager")	||
				strstr(t,"pacotes")	    ||
				strstr(t,"process")     ||
				strstr(t,"project")		||
				strstr(t,"regedit")		||
				strstr(t,"speed")	    ||
				strstr(t,"spothack")	||
				strstr(t,"suspend")		||
				strstr(t,"tarefas")		||
				strstr(t,"team")        ||
				strstr(t,"tool")        ||
				strstr(t,"toolz")		||
				strstr(t,"trade hacker") ||
				strstr(t,"trap")	    ||
				strstr(t,"webcheat")	||
				strstr(t,"wildproxy")	||
				strstr(t,"wpe pro")		||
				strstr(t,"wpe-pro.spt")	||
				strstr(t,"wpepro")		||
				strstr(t,"xSpeed")	    ||
				strstr(t,"xSpeed.net")	||
				strstr(t,"xTrap")	    ||
				strstr(t,"xelerator")	||	
				strstr(t,"xspeed")		||
				strstr(t,"xtrap")	    ||
				strstr(t,"ByDark")		||
				strstr(t,"Bot")		    ||
				strstr(t,"Bots")		||
				strstr(t,"Cheat4Fun")   ||
				strstr(t,"FunnyZhyper")	||
				strstr(t,"Skill")		||
				strstr(t,"Blaster")		||
				strstr(t,"Master")		||
				strstr(t,"Ziomal")		||
				strstr(t,"Sniff")		||
				strstr(t,"AutoSkill")	||
				strstr(t,"Agility")		||
				strstr(t,"HitHack")		||
				strstr(t,"Spiffs")		||
				strstr(t,"Macro")		||
				strstr(t,"Permit")		||
				strstr(t,"Comercio")	||
				strstr(t,"Trade")	    ||
				strstr(t,"Packet")	    ||
				strstr(t,"Wall")	    ||
				strstr(t,"Perfect")	    ||
				strstr(t,"Ultimate")	||
				strstr(t,"!xSpeedPro")	||
				strstr(t,"MuEnergyX")	||
				strstr(t,"Razor")	    ||
				strstr(t,"Hammer")	    ||
				strstr(t,"AutoIt")	    ||
				strstr(t,"Autoit")	    ||
				strstr(t,"iCheat")	    ||
				strstr(t,"AutoKeyboard") ||
				strstr(t,"Speeder")  	||
				strstr(t,"smoll")    	||
				strstr(t,"smoll")    	||
				strstr(t,"Smoll")    	||
				strstr(t,"SmoLL")    	||
				strstr(t,"List1")    	||
				strstr(t,"SpeederXP")
			)
			
			{
				unsigned char * hack = (unsigned char*) GetProcAddress(GetModuleHandleA("kernel32.dll") , "OpenProcess");
				if (*(hack+6) == 0xEA)
				{
					if (GAME_GUARD_LOG)
					{
						using namespace std;
						ofstream out(TXT_GAME_GUARD_LOG, ios::app);
						out << "\nH-Scan: ", out <<   (t);
					}
					CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_WindowsTextScanner),NULL,0,0);
					Sleep(3000); 
					ExitProcess(0);
				}
				else
				{
					if (GAME_GUARD_LOG)
					{
						using namespace std;
						ofstream out(TXT_GAME_GUARD_LOG, ios::app);
						out << "\nH-Scan: ", out <<   (t);
					}
					CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_WindowsTextScanner),NULL,0,0);
					Sleep(3000); 
					ExitProcess(0);
				}
			}
		}
	}
}

void DetectWindowsTextScan()
{
again:
    DetectWindowsTextScanner();
    Sleep(10000);
    goto again;
}

void ThreadDetectWindowsText()
{
	if (ANTI_DETECT_WINDOWS_TEXT)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(DetectWindowsTextScan),NULL,0,0);
	}
}
