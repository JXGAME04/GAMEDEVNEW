/* Kiem cu phap script Lua 4.0.1 bang CHINH thu vien Lua cua engine JX1.
 * Dung: syncheck.exe <file.lua> ...
 * Nap file bang lua_dofile voi cac ham engine gia lap (no-op) de chay het top-level.
 */
#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"

/* moi ham engine chua biet -> tra ve nil, khong lam gi */
static int stub_fn(lua_State *L) { (void)L; return 0; }

static const char *g_stubs[] = {
    "Include", "Msg2Player", "SayEx", "Say", "Talk", "AddItem", "GetTask", "SetTask",
    "SetTaskTemp", "GetTaskTemp", "NewWorld", "DynamicExecute", "GetGlbValue",
    "PB_SetTongKim", "ReLoadScript", "GetLevel", "GetName", "GetTaskDailyCount",
    "OutputMsg", "print", "openfile", "closefile", "read", "write", "SubWorldID2Idx",
    "AddNpc", "SetNpcScript", "GetLocalDate", "GetCurServerTime", "TabFile_Load",
    "TabFile_GetCell", "basemission_CallNpc", "AddOwnExp", "CallPlayerFunction",
    "PlayerFunLib", "WriteLog", "RemoteExc", "GetWorldPos", "GetMapName",
    NULL
};

int main(int argc, char **argv)
{
    int i, bad = 0;
    if (argc < 2) { printf("dung: syncheck <file.lua> ...\n"); return 2; }
    for (i = 1; i < argc; i++) {
        lua_State *L = lua_open(0);
        int k;
        lua_baselibopen(L);
        lua_strlibopen(L);
        lua_mathlibopen(L);
        lua_iolibopen(L);
        for (k = 0; g_stubs[k]; k++)
            lua_register(L, g_stubs[k], stub_fn);
        {
            /* LUA_ERRRUN(1) = loi CHAY (do moi truong gia lap thieu ham engine) -> BO QUA.
               LUA_ERRFILE(2) = khong mo duoc tep. LUA_ERRSYNTAX(3) = LOI CU PHAP THAT. */
            int r = lua_dofile(L, argv[i]);
            if (r == LUA_ERRSYNTAX) { printf("LOI CU PHAP  %s\n", argv[i]); bad++; }
            else if (r == LUA_ERRFILE) { printf("KHONG MO DUOC  %s\n", argv[i]); bad++; }
            else printf("cu phap OK   %s%s\n", argv[i], r ? "   (co loi chay - binh thuong khi gia lap)" : "");
        }
        lua_close(L);
    }
    return bad ? 1 : 0;
}
