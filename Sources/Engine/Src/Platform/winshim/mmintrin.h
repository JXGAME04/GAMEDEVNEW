/* [ANDROID 08/09] winshim: thay <mmintrin.h> cua Windows SDK / CRT MSVC khi bien dich Android (JX_POSIX).
   Sinh boi android/gen_winshim.py - KHONG sua tay. Ban Windows khong dung thu muc nay. */
#if defined(__x86_64__) || defined(__i386__)
#include_next <mmintrin.h>
#else
#include "../KPosixCompat.h"
#endif
