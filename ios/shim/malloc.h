/* [IOS 11/09] Apple khong co <malloc.h> (Linux/Android/Windows deu co). Mot so tep nguon include thang
   ten do, vi du Sources/FilterText/Regexp.cpp:72 "#include \"malloc.h\"".
   Thu muc ios/shim CHI nam trong duong include cua ban iOS -> Android va Windows khong bao gio thay tep nay. */
#pragma once
#include <stdlib.h>
