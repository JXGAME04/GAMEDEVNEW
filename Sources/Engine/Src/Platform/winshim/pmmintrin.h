/* [ANDROID 08/09] winshim: thay <pmmintrin.h> cua Windows SDK / CRT MSVC khi bien dich Android (JX_POSIX).
   Sinh boi android/gen_winshim.py - KHONG sua tay. Ban Windows khong dung thu muc nay. */
#if defined(__x86_64__) || defined(__i386__)
#include_next <pmmintrin.h>
#else
#include "../KPosixCompat.h"
#endif
