//////////////////////////////////////////////////////////////////////////////
// KGameKV.cpp -- xem KGameKV.h de biet cach dung va ly do thiet ke.
//////////////////////////////////////////////////////////////////////////////

// Tep nay KHONG dung precompiled header (vcxproj dat PrecompiledHeader=NotUsing).
// Ly do: PCH cua Core la KCore.h, ma trinh bien dich BO QUA moi thu truoc dong
// #include "KCore.h" -- lam #ifndef dat truoc no bi nuot, con #endif thanh mo coi.
// Bo PCH cho rieng hai tep nay la cach duy nhat vua sach vua bien dich rieng le duoc.
#include "KGameKV.h"

#ifdef _SERVER

#include "KMySQLDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tran mot gia tri KV. StatData.dat do duoc 202.216 byte; codenew.lua 399 KB.
// 8 MB la rong rai cho moi thu hien co ma van chan duoc tep hong khong lo.
#define KV_MAX_LEN   (8 * 1024 * 1024)

namespace KGameKV
{

//////////////////////////////////////////////////////////////////////////////
static bool _Put(const char* szNs, const char* szKey,
                 const void* pData, int nLen, bool bAsync)
{
    if (!szNs || !szKey || !pData) return false;
    if (nLen < 0 || nLen > KV_MAX_LEN)
    {
        KDBLog("KV: TU CHOI ns=%s k=%s vi do dai %d ngoai [0, %d]",
               szNs, szKey, nLen, KV_MAX_LEN);
        return false;
    }
    if (!g_MySQLDB.IsReady()) return false;

    // Chup ban CU vao game_kv_history TRUOC khi ghi de -- de con quay lui khi
    // mot tinh nang ghi hong. Lam chung mot duong (dong bo hay bat dong bo) voi
    // cau ghi de, nen thu tu luon dung.
    {
        KDBParam h[2];
        h[0] = KDBParam::S(szNs);
        h[1] = KDBParam::B(szKey, (int)strlen(szKey));
        const char* sqlH =
            "INSERT INTO game_kv_history (ns,k,v,saved_at) "
            "SELECT ns,k,v,NOW() FROM game_kv WHERE ns=? AND k=?";
        if (bAsync) g_MySQLDB.Post(sqlH, h, 2);
        else        g_MySQLDB.Exec(sqlH, h, 2);
    }

    KDBParam p[4];
    p[0] = KDBParam::S(szNs);
    p[1] = KDBParam::B(szKey, (int)strlen(szKey));
    p[2] = KDBParam::B(pData, nLen);
    p[3] = KDBParam::I(nLen);
    const char* sql =
        "INSERT INTO game_kv (ns,k,v,v_len) VALUES (?,?,?,?) "
        "ON DUPLICATE KEY UPDATE v=VALUES(v), v_len=VALUES(v_len)";
    return bAsync ? g_MySQLDB.Post(sql, p, 4)
                  : g_MySQLDB.Exec(sql, p, 4);
}

bool Put(const char* szNs, const char* szKey, const void* pData, int nLen, bool bAsync)
{
    return _Put(szNs, szKey, pData, nLen, bAsync);
}

//////////////////////////////////////////////////////////////////////////////
struct _KVNhan
{
    char* pBuf;
    int   nCap;
    int   nLen;
};
static bool _NhanKV(const KDBRow& row, void* pParam)
{
    _KVNhan* t = (_KVNhan*)pParam;
    if (row.nCol < 1) return false;
    int n = row.pLen[0];
    if (n > t->nCap) n = t->nCap;
    if (n > 0) memcpy(t->pBuf, row.pVal[0], n);
    t->nLen = n;
    return false;   // chi lay dong dau
}

int Get(const char* szNs, const char* szKey, void* pBuf, int nCap)
{
    if (!szNs || !szKey || !pBuf || nCap <= 0) return -1;
    if (!g_MySQLDB.IsReady()) return -1;
    _KVNhan t;
    t.pBuf = (char*)pBuf;
    t.nCap = nCap;
    t.nLen = -1;
    KDBParam p[2];
    p[0] = KDBParam::S(szNs);
    p[1] = KDBParam::B(szKey, (int)strlen(szKey));
    if (!g_MySQLDB.Query("SELECT v FROM game_kv WHERE ns=? AND k=?", p, 2, _NhanKV, &t))
        return -1;
    return t.nLen;
}

bool Del(const char* szNs, const char* szKey)
{
    if (!szNs || !szKey || !g_MySQLDB.IsReady()) return false;
    KDBParam p[2];
    p[0] = KDBParam::S(szNs);
    p[1] = KDBParam::B(szKey, (int)strlen(szKey));
    return g_MySQLDB.Exec("DELETE FROM game_kv WHERE ns=? AND k=?", p, 2);
}

//////////////////////////////////////////////////////////////////////////////
bool PutFile(const char* szNs, const char* szKey, const char* szPath, bool bAsync)
{
    if (!szPath) return false;
    FILE* f = fopen(szPath, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0 || n > KV_MAX_LEN)
    {
        fclose(f);
        KDBLog("KV: PutFile bo qua %s vi kich thuoc %ld", szPath, n);
        return false;
    }
    char* p = (char*)malloc(n > 0 ? n : 1);
    if (!p) { fclose(f); return false; }
    size_t nDoc = (n > 0) ? fread(p, 1, n, f) : 0;
    fclose(f);
    bool ok = false;
    if ((long)nDoc == n)
        ok = _Put(szNs, szKey, p, (int)n, bAsync);
    else
        KDBLog("KV: PutFile doc thieu %s (%d/%ld byte) -- KHONG ghi len MySQL",
               szPath, (int)nDoc, n);
    free(p);
    return ok;
}

bool GetFileToDisk(const char* szNs, const char* szKey, const char* szPath,
                   bool bChiKhiThieu)
{
    if (!szPath) return false;
    if (bChiKhiThieu)
    {
        FILE* f = fopen(szPath, "rb");
        if (f) { fclose(f); return false; }   // tep con do -> khong dung toi
    }
    if (!g_MySQLDB.IsReady()) return false;

    char* p = (char*)malloc(KV_MAX_LEN);
    if (!p) return false;
    int n = Get(szNs, szKey, p, KV_MAX_LEN);
    if (n < 0) { free(p); return false; }

    // Ghi qua .tmp roi doi cho -- khong bao gio de lai tep cut do tren dia.
    char szTmp[MAX_PATH];
    _snprintf(szTmp, sizeof(szTmp) - 1, "%s.kvtmp", szPath);
    szTmp[sizeof(szTmp) - 1] = 0;
    FILE* f = fopen(szTmp, "wb");
    if (!f) { free(p); return false; }
    bool ok = (n == 0) || (fwrite(p, 1, n, f) == (size_t)n);
    if (ok) ok = (fflush(f) == 0);
    fclose(f);
    free(p);
    if (!ok) { DeleteFileA(szTmp); return false; }
    if (!MoveFileExA(szTmp, szPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        DeleteFileA(szTmp);
        return false;
    }
    KDBLog("KV: da dung lai %s tu MySQL (ns=%s k=%s, %d byte)", szPath, szNs, szKey, n);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
bool Log(const char* szLoai, const char* szRole, const void* pNoiDung, int nLen)
{
    if (!szLoai || !pNoiDung || nLen <= 0) return false;
    if (!g_MySQLDB.IsReady()) return false;
    if (nLen > 1024) nLen = 1024;      // cot noi_dung la VARBINARY(1024)
    KDBParam p[3];
    p[0] = KDBParam::S(szLoai);
    if (szRole && szRole[0]) p[1] = KDBParam::B(szRole, (int)strlen(szRole));
    else                     p[1] = KDBParam::Null();
    p[2] = KDBParam::B(pNoiDung, nLen);
    // LUON bat dong bo: day la duong nong nhat, khong duoc cham vao vong lap game.
    return g_MySQLDB.Post(
        "INSERT INTO game_log (ngay,loai,role_name,noi_dung,at) "
        "VALUES (CURDATE(),?,?,?,NOW())", p, 3);
}

bool LogStr(const char* szLoai, const char* szRole, const char* szNoiDung)
{
    return szNoiDung ? Log(szLoai, szRole, szNoiDung, (int)strlen(szNoiDung)) : false;
}

} // namespace KGameKV

//////////////////////////////////////////////////////////////////////////////
// Lay mot lenh quan tri chua xu ly. Nam NGOAI namespace vi Lua goi truc tiep.
//
// Cach cu (timerserver.lua:145,202): doc ca tep dulieu/username_kick.txt roi
// GHI DE thanh rong. Neu web quan tri ghi them mot dong dung luc script dang
// xoa thi LENH DO MAT. O day: SELECT ... FOR UPDATE roi UPDATE done_at trong
// MOT giao dich, nen khong the mat lenh.
//////////////////////////////////////////////////////////////////////////////
struct _KAdmNhan { long long id; char* pArg; int nCap; int nLen; };
static bool _NhanLenh(const KDBRow& row, void* pParam)
{
    _KAdmNhan* t = (_KAdmNhan*)pParam;
    if (row.nCol < 2) return false;
    t->id = _atoi64(row.pVal[0]);
    int n = row.pLen[1];
    if (n > t->nCap - 1) n = t->nCap - 1;
    if (n > 0) memcpy(t->pArg, row.pVal[1], n);
    t->pArg[n > 0 ? n : 0] = 0;
    t->nLen = n;
    return false;
}

bool KGameKV_LayLenhQuanTri(const char* szCmd, char* szArg, int nCap)
{
    if (!szCmd || !szArg || nCap <= 1) return false;
    if (!g_MySQLDB.IsReady()) return false;
    if (!g_MySQLDB.Begin()) return false;

    _KAdmNhan t; t.id = 0; t.pArg = szArg; t.nCap = nCap; t.nLen = -1;
    KDBParam p[1];
    p[0] = KDBParam::S(szCmd);
    bool ok = g_MySQLDB.Query(
        "SELECT id, arg FROM admin_command WHERE cmd=? AND done_at IS NULL "
        "ORDER BY id LIMIT 1 FOR UPDATE", p, 1, _NhanLenh, &t);
    if (!ok || t.id == 0) { g_MySQLDB.Rollback(); return false; }

    KDBParam q[1];
    q[0] = KDBParam::I(t.id);
    __int64 aff = 0;
    ok = g_MySQLDB.Exec("UPDATE admin_command SET done_at=NOW() WHERE id=? "
                        "AND done_at IS NULL", q, 1, &aff);
    if (!ok || aff != 1) { g_MySQLDB.Rollback(); return false; }
    if (!g_MySQLDB.Commit()) { g_MySQLDB.Rollback(); return false; }
    return true;
}

#endif // _SERVER
