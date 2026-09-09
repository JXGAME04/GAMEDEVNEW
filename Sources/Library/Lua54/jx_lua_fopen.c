/* [ANDROID 08/09] libLua54.so tren POSIX: moi fopen() trong Lua (luaL_loadfilex / io.open / lua4compat lua4_dofile ...) di qua
   JxPathPosix() cua libJxPosix.so ("\" -> "/", ha chu thuong phan tuong doi hoac duoi thu muc du lieu) vi script duoc goi bang
   duong dan Windows (root\script\...). CMake Android bien dich Lua54 voi -Dfopen=jx_lua_fopen va link JxPosix. Windows khong dung tep nay. */
#if !defined(_WIN32)
#include <stdio.h>
#include <stddef.h>
#undef fopen   /* trong tep nay goi fopen that */
extern char* JxPathPosix(const char* pszIn, char* pszOut, size_t nOut);
FILE* jx_lua_fopen(const char* pszPath, const char* pszMode)
{
	char sz[1024];
	return fopen(JxPathPosix(pszPath, sz, sizeof(sz)), pszMode);
}
#endif
