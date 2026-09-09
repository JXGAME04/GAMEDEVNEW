/* [ANDROID 08/09] winshim: thay <d3d9types.h> cua Windows SDK / CRT MSVC khi bien dich Android (JX_POSIX).
   Sinh boi android/gen_winshim.py - KHONG sua tay. Ban Windows khong dung thu muc nay. */
#include "../KPosixCompat.h"
#ifdef JX_D3D9MINI
#include "d3d9mini.h"
#endif
