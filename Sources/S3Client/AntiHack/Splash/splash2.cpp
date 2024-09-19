#include "KWin32.h"
#include "../../S3Config.h"
#include "Splash.h"

void SplashShow() 
{
    CSplash splash1(TEXT(".\\GameGuard/logo.bmp"), RGB(128, 128, 128));
    splash1.ShowSplash();
    Sleep(3000);

    //  Close the splash screen
    splash1.CloseSplash();
}



