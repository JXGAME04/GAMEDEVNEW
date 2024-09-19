#include "KWin32.h"
#include "DetectWindowsTitle.h"
#include "../../S3Config.h"

void Msg_Windows_Title()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL,"WindowsTitle-Scan: Phat hien phan mem bat hop phap!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL,"WindowsTitle-Scan: An illegal choice has been detected!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
}

bool TitleWindows(LPCSTR WindowTitle)
{
	HWND WinTitle = FindWindowA(NULL,WindowTitle);
	if( WinTitle > 0)
	{
		if (GAME_GUARD_LOG)
		{	
			using namespace std;
			ofstream out(TXT_GAME_GUARD_LOG, ios::app);
			out << "\nWindows-Scan:  ", out <<   WindowTitle;
		}
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Windows_Title),NULL,0,0);
		Sleep(3000); 
		ExitProcess(0);
		return false;
	}
	return true;
}

void DetectWindowsTitleScanner()
{
	TitleWindows("!xSpeed 6.0");
	TitleWindows("!xSpeed.net 2");
	TitleWindows("!xSpeed.net 3");
	TitleWindows("!xSpeed.net 6");
	TitleWindows("!xSpeed.net");
	TitleWindows("!xSpeedPro");
	TitleWindows("!xpeed.net 1.41");
	TitleWindows("99QJ MU Bot");
	TitleWindows("AE Bot v1.0 beta");
	TitleWindows("AIO Bots");
	TitleWindows("Add address");
	TitleWindows("ArtMoney PRO v7.27");
	TitleWindows("ArtMoney SE v7.31");
	TitleWindows("ArtMoney SE v7.32");
	TitleWindows("Auto Combo");
	TitleWindows("Auto-Repairer");
	TitleWindows("AutoBuff D-C");
	TitleWindows("AutoBuff");
	TitleWindows("AutoCombo D-C By WANTED");
	TitleWindows("AutoCombo");
	TitleWindows("Auto_Buff v5 Hack Rat");
	TitleWindows("Autoprision");
	TitleWindows("Bot MG-DK-ELF");
	TitleWindows("CapoteCheatfreadcompany");
	TitleWindows("Capotecheat");
	TitleWindows("Capotecheat(deltacholl)");
	TitleWindows("Catastrophe v0.1");
	TitleWindows("Catastrophe v1.2");
	TitleWindows("Catastrophe");
	TitleWindows("Chaos Bot 2.1.0");
	TitleWindows("CharBlaster");
	TitleWindows("CharEditor (www.darkhacker.com.ar)");
	TitleWindows("Cheat Engine 5.0");
	TitleWindows("Cheat Engine 5.1");
	TitleWindows("Cheat Engine 5.1.1");
	TitleWindows("Cheat Engine 5.2");
	TitleWindows("Cheat Engine 5.3");
	TitleWindows("Cheat Engine 5.4");
	TitleWindows("Cheat Engine 5.5");
	TitleWindows("Cheat Engine 5.6");
	TitleWindows("Cheat Engine 5.6.1");
	TitleWindows("Cheat Engine 6.0");
	TitleWindows("Cheat Engine 6.1");
	TitleWindows("Cheat Engine 6.2");
	TitleWindows("Cheat Engine 6.3");
	TitleWindows("Cheat Engine 7.2");
	TitleWindows("Cheat Engine");
	TitleWindows("Cheat Happens v3.95b1/b2");
	TitleWindows("Cheat Happens v3.95b3");
	TitleWindows("Cheat Happens v3.96b2");
	TitleWindows("Cheat Happens v3.9b1");
	TitleWindows("Cheat Happens");
	TitleWindows("Cheat Master");
	TitleWindows("Cheat4Fun v0.9 Beta");
	TitleWindows("Cheat4Fun");
	TitleWindows("CheatHappens");
	TitleWindows("Codehitcz");
	TitleWindows("Created processes");
	TitleWindows("D-C Bypass");
	TitleWindows("D-C DupeHack 1.0");
	TitleWindows("D-C Master Inject v1.0 by WANTED");
	TitleWindows("DC Mu 1.04x _F3R_ Hack");
	TitleWindows("DC-BYPASS By DjCheats  Public Vercion");
	TitleWindows("DK(AE)MultiStrikeByDude");
	TitleWindows("DarkCheats Mu Ar");
	TitleWindows("DarkLord Bot v1.1");
	TitleWindows("DarkyStats (www.darkhacker.com.ar)");
	TitleWindows("Dizzys Auto Buff");
	TitleWindows("Dupe-Full");
	TitleWindows("Easy As MuPie");
	TitleWindows("Esperando Mu Online");
	TitleWindows("FunnyZhyper v5");
	TitleWindows("FunnyZhyper");
	TitleWindows("Game Speed Adjuster");
	TitleWindows("Game Speed Changer");
	TitleWindows("GodMode");
	TitleWindows("Godlike");
	TitleWindows("HahaMu 1.16");
	TitleWindows("Hasty MU 0.3");
	TitleWindows("Hasty MU");
	TitleWindows("HastyMU");
	TitleWindows("HastyMu 1.1.0 NEW");
	TitleWindows("HastyMu v0.1");
	TitleWindows("HastyMu v0.2");
	TitleWindows("HastyMu v0.3");
	TitleWindows("HastyMu");
	TitleWindows("HiDeToolz");
	TitleWindows("HideToolz");
	TitleWindows("Hit Count");
	TitleWindows("Hit Hack");
	TitleWindows("Injector");
	TitleWindows("Janopn Mini Multi Cheat v0.1");
	TitleWindows("Jewel Drop Beta");
	TitleWindows("JoyToKey");
	TitleWindows("Kill");
	TitleWindows("Lipsum v1 and v2");
	TitleWindows("Lipsum_v2");
	TitleWindows("List");
	TitleWindows("Load File");
	TitleWindows("Load");
	TitleWindows("MJB Perfect DL Bot");
	TitleWindows("MLEngine");
	TitleWindows("MU Lite Trainer");
	TitleWindows("MU Utilidades");
	TitleWindows("MU-SS4 Speed Hack 1.2");
	TitleWindows("MUSH");
	TitleWindows("Minimize");
	TitleWindows("ModzMu");
	TitleWindows("MoonLight");
	TitleWindows("Mu Cheater 16");
	TitleWindows("Mu Philiphinas Cheat II");
	TitleWindows("Mu Pie Beta");
	TitleWindows("Mu Pirata MMHack v0.2 by janopn");
	TitleWindows("Mu proxy");
	TitleWindows("MuBot");
	TitleWindows("MuCheat");
	TitleWindows("MuHackRm");
	TitleWindows("MuOnline Speed Hack");
	TitleWindows("MuPie HG v2");
	TitleWindows("MuPie HG v3");
	TitleWindows("MuPie v2 Beta");
	TitleWindows("MuPieHGV2");
	TitleWindows("MuPieHGV3");
	TitleWindows("MuPieX");
	TitleWindows("MuPie_v2Beta");
	TitleWindows("MuProxy");
	TitleWindows("Mugster Bot");
	TitleWindows("Mupie Minimizer");
	TitleWindows("Mush");
	TitleWindows("NoNameMini");
	TitleWindows("Nsauditor 1.9.1");
	TitleWindows("Olly Debugger");
	TitleWindows("Overclock Menu");
	TitleWindows("Perfect AutoPotion");
	TitleWindows("Permit");
	TitleWindows("PeruCheats");
	TitleWindows("Process Explorer 11.33");
	TitleWindows("Process Explorer");
	TitleWindows("ProxCheatsX 2.0 - Acacias");
	TitleWindows("RPE");
	TitleWindows("Razor Code Only");
	TitleWindows("Razor Code");
	TitleWindows("Snd Bot 1.5");
	TitleWindows("Speed Gear 5");
	TitleWindows("Speed Gear 6");
	TitleWindows("Speed Gear v5.00");
	TitleWindows("Speed Gear");
	TitleWindows("Speed Hack 99.62t");
	TitleWindows("Speed Hack Simplifier 1.0-1.2");
	TitleWindows("Speed Hack Simplifier");
	TitleWindows("Speed Hack");
	TitleWindows("Speed Hacker");
	TitleWindows("SpeedGear");
	TitleWindows("SpeedMUVN");
	TitleWindows("SpiffsAutobot");
	TitleWindows("SpotHack 1.1");
	TitleWindows("SpotHack");
	TitleWindows("Stop");
	TitleWindows("Super Bot");
	TitleWindows("T Search");
	TitleWindows("Tablet 2");
	TitleWindows("The following opcodes accessed the selected address");
	TitleWindows("Trade HACK 1.8");
	TitleWindows("Ultimate Cheat");
	TitleWindows("UoPilot  v2.17   WK");
	TitleWindows("UoPilot");
	TitleWindows("VaultBlaster");
	TitleWindows("VaultEditor (www.darkhacker.com.ar)");
	TitleWindows("WPE PRO");
	TitleWindows("WPePro 0.9a");
	TitleWindows("WPePro 1.3");
	TitleWindows("Wall");
	TitleWindows("WildProxy 1.0 Alpha");
	TitleWindows("WildProxy v0.1");
	TitleWindows("WildProxy v0.2");
	TitleWindows("WildProxy v0.3");
	TitleWindows("WildProxy v1.0 Public");
	TitleWindows("WildProxy");
	TitleWindows("Xelerator 1.4");
	TitleWindows("Xelerator 2.0");
	TitleWindows("Xelerator");
	TitleWindows("ZhyperMu Packet Editor");
	TitleWindows("[Dark-Cheats] MultiD-C");
	TitleWindows("eXpLoRer");
	TitleWindows("hacker");	
	TitleWindows("rPE - rEdoX Packet Editor");
	TitleWindows("razorcode");
	TitleWindows("speed");
	TitleWindows("speednet");
	TitleWindows("speednet2");
	TitleWindows("www.55xp.com");
	TitleWindows("xSpeed.net 3.0");
	TitleWindows("Windows Task Manager");
}

void DetectWindowsTitleScan()
{
again:
    DetectWindowsTitleScanner();
    Sleep(30000);
    goto again;
}

void ThreadDetectWindowsTitle()
{
	if (ANTI_DETECT_WINDOWS_TITLE)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(DetectWindowsTitleScan),NULL,0,0);
	}
}
