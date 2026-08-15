#pragma once
// ----------------------------------------------------------------------------------------------

#define TRAYMODE_ICON_ID		100
#define TRAYMODE_ICON_MESSAGE	0x445
// ----------------------------------------------------------------------------------------------
class TrayMode
{
public:
	NOTIFYICONDATA 		Icon;
	void	HideNotify(HINSTANCE hInst);
	void	ShowNotify();
}; extern TrayMode gTrayMode;