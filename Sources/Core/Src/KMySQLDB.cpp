//////////////////////////////////////////////////////////////////////////////
// KMySQLDB.cpp -- xem KMySQLDB.h de biet kien truc va cac rang buoc an toan.
//////////////////////////////////////////////////////////////////////////////

// KCore.h PHAI dung dau tien: du an Core dung precompiled header qua tep nay.
// May man la KCore.h dat WIN32_LEAN_AND_MEAN ngay dong 8, nen windows.h KHONG
// keo winsock.h vao -- nho vay mysql.h (can winsock2.h) khong bi dung do.
// Tep nay KHONG dung precompiled header (vcxproj dat PrecompiledHeader=NotUsing).
// Ly do: PCH cua Core la KCore.h, ma trinh bien dich BO QUA moi thu truoc dong
// #include "KCore.h" -- lam #ifndef dat truoc no bi nuot, con #endif thanh mo coi.
// Bo PCH cho rieng hai tep nay la cach duy nhat vua sach vua bien dich rieng le duoc.
#include "KMySQLDB.h"

#ifdef _SERVER

#include <mysql.h>
#include <process.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KDB_LOG_FILE     "mysql_core.log"
#define KDB_MAX_QUEUE    20000     // tran hang doi ghi nen
#define KDB_MAX_PARAM    32

KMySQLDB g_MySQLDB;

//////////////////////////////////////////////////////////////////////////////
void KDBLog(const char *fmt, ...)
{
    FILE *f = fopen(KDB_LOG_FILE, "a");
    if (!f) return;
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    char stamp[32];
    strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", lt);
    fprintf(f, "[%s] ", stamp);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fprintf(f, "\n");
    fclose(f);
}

//////////////////////////////////////////////////////////////////////////////
// Mot goi lenh trong hang doi ghi nen. Toan bo du lieu duoc SAO CHEP vao day
// nen ben goi khong phai giu bo dem song.
//////////////////////////////////////////////////////////////////////////////
struct KDBJob
{
    char     *pSql;
    int       nParam;
    KDBParam  aParam[KDB_MAX_PARAM];
    char     *pBlobBuf;      // vung chua toan bo du lieu cua cac tham so BLOB/STR
    KDBJob   *pNext;
};

struct KMySQLDB::Impl
{
    MYSQL *pSync;            // ket noi cua luong game (dong bo)
    MYSQL *pAsync;           // ket noi RIENG cua luong ghi nen
    char   szHost[128], szUser[128], szPass[128], szDb[128];
    unsigned nPort;

    CRITICAL_SECTION csQueue;
    HANDLE hEvent;
    HANDLE hThread;
    volatile bool bStop;
    KDBJob *pHead, *pTail;

    Impl()
        : pSync(0), pAsync(0), nPort(3306), hEvent(0), hThread(0),
          bStop(false), pHead(0), pTail(0)
    {
        szHost[0] = szUser[0] = szPass[0] = szDb[0] = 0;
        InitializeCriticalSection(&csQueue);
    }
    ~Impl() { DeleteCriticalSection(&csQueue); }
};

//////////////////////////////////////////////////////////////////////////////
static void IniStr(const char *ini, const char *sec, const char *key,
                   const char *def, char *out, int cap)
{
    GetPrivateProfileStringA(sec, key, def, out, cap - 1, ini);
    out[cap - 1] = 0;
    int n = (int)strlen(out);
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == '\t' ||
                     out[n - 1] == '\r' || out[n - 1] == '\n'))
        out[--n] = 0;
}

static MYSQL *KDBConnect(KMySQLDB::Impl *im, const char *szAi)
{
    MYSQL *c = mysql_init(NULL);
    if (!c) { KDBLog("%s: mysql_init that bai", szAi); return 0; }
    // Byte tho -- TUYET DOI khong utf8mb4
    mysql_options(c, MYSQL_SET_CHARSET_NAME, "latin1");
    unsigned int to = 5;
    mysql_options(c, MYSQL_OPT_CONNECT_TIMEOUT, &to);
    // Tham so cuoi PHAI = 0
    if (!mysql_real_connect(c, im->szHost, im->szUser, im->szPass, im->szDb,
                            im->nPort, NULL, 0))
    {
        KDBLog("%s: mysql_real_connect LOI: %s", szAi, mysql_error(c));
        mysql_close(c);
        return 0;
    }
    mysql_query(c, "SET SESSION sql_mode='STRICT_ALL_TABLES'");
    mysql_autocommit(c, 1);
    return c;
}

//////////////////////////////////////////////////////////////////////////////
KMySQLDB::KMySQLDB()
    : m_pImpl(0), m_bReady(false), m_nQueue(0), m_nDropped(0), m_nErrors(0)
{
}

KMySQLDB::~KMySQLDB()
{
    Close();
}

bool KMySQLDB::Init(const char *szIniPath, const char *szSection)
{
    if (m_bReady) return true;
    if (!m_pImpl) m_pImpl = new Impl();
    Impl *im = m_pImpl;

    IniStr(szIniPath, szSection, "Server",   "127.0.0.1", im->szHost, sizeof(im->szHost));
    IniStr(szIniPath, szSection, "User",     "root",      im->szUser, sizeof(im->szUser));
    IniStr(szIniPath, szSection, "PassWord", "123456",    im->szPass, sizeof(im->szPass));
    IniStr(szIniPath, szSection, "DataBase", "jx1_game",  im->szDb,   sizeof(im->szDb));
    im->nPort = (unsigned)GetPrivateProfileIntA(szSection, "Port", 3306, szIniPath);

    im->pSync = KDBConnect(im, "ket noi dong bo");
    if (!im->pSync) return false;
    im->pAsync = KDBConnect(im, "ket noi ghi nen");
    if (!im->pAsync)
    {
        mysql_close(im->pSync);
        im->pSync = 0;
        return false;
    }

    im->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    im->bStop = false;
    unsigned id = 0;
    im->hThread = (HANDLE)_beginthreadex(0, 0, _WorkerThunk, this, 0, &id);
    if (!im->hThread)
    {
        KDBLog("khong tao duoc luong ghi nen");
        Close();
        return false;
    }

    m_bReady = true;
    KDBLog("Init OK -- %s:%u/%s (mot ket noi dong bo + mot ket noi ghi nen)",
           im->szHost, im->nPort, im->szDb);
    return true;
}

void KMySQLDB::Close()
{
    if (!m_pImpl) return;
    Impl *im = m_pImpl;
    m_bReady = false;

    if (im->hThread)
    {
        im->bStop = true;
        SetEvent(im->hEvent);
        // Cho luong nen xa het hang doi. 10 giay la du cho vai chuc nghin dong.
        WaitForSingleObject(im->hThread, 10000);
        CloseHandle(im->hThread);
        im->hThread = 0;
    }
    if (im->hEvent) { CloseHandle(im->hEvent); im->hEvent = 0; }

    // Bo not nhung goi chua kip ghi (neu co) -- va ghi nhat ky de biet
    int nBo = 0;
    while (im->pHead)
    {
        KDBJob *j = im->pHead;
        im->pHead = j->pNext;
        free(j->pSql);
        free(j->pBlobBuf);
        free(j);
        nBo++;
    }
    im->pTail = 0;
    if (nBo) KDBLog("Close: con %d goi chua ghi kip -> da bo", nBo);

    if (im->pAsync) { mysql_close(im->pAsync); im->pAsync = 0; }
    if (im->pSync)  { mysql_close(im->pSync);  im->pSync  = 0; }
    delete im;
    m_pImpl = 0;
    KDBLog("Close xong (bo qua %d goi vi hang doi day, %d loi)", m_nDropped, m_nErrors);
}

//////////////////////////////////////////////////////////////////////////////
// Thuc thi mot cau lenh co tham so tren mot ket noi cho truoc
//////////////////////////////////////////////////////////////////////////////
static bool KDBRun(MYSQL *conn, const char *szSql, const KDBParam *pParam, int nParam,
                   __int64 *pnAffected, __int64 *pnInsertId,
                   KDBRowFunc pfnRow, void *pUserParam, volatile long *pnErrors)
{
    if (!conn || !szSql) return false;
    if (nParam < 0 || nParam > KDB_MAX_PARAM) return false;

    MYSQL_STMT *st = mysql_stmt_init(conn);
    if (!st) return false;
    if (mysql_stmt_prepare(st, szSql, (unsigned long)strlen(szSql)))
    {
        KDBLog("prepare LOI: %s | SQL=%s", mysql_stmt_error(st), szSql);
        mysql_stmt_close(st);
        if (pnErrors) InterlockedIncrement(pnErrors);
        return false;
    }

    MYSQL_BIND bind[KDB_MAX_PARAM];
    unsigned long lens[KDB_MAX_PARAM];
    my_bool nulls[KDB_MAX_PARAM];
    memset(bind, 0, sizeof(bind));
    for (int i = 0; i < nParam; i++)
    {
        const KDBParam &p = pParam[i];
        lens[i] = (unsigned long)(p.len > 0 ? p.len : 0);
        nulls[i] = 0;
        switch (p.type)
        {
        case KDBP_I64:
            bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
            bind[i].buffer = (void *)&p.i64;
            break;
        case KDBP_DOUBLE:
            bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
            bind[i].buffer = (void *)&p.dbl;
            break;
        case KDBP_BLOB:
            bind[i].buffer_type = MYSQL_TYPE_BLOB;
            bind[i].buffer = (void *)p.buf;
            bind[i].buffer_length = lens[i];
            bind[i].length = &lens[i];
            break;
        case KDBP_STR:
            bind[i].buffer_type = MYSQL_TYPE_STRING;
            bind[i].buffer = (void *)p.buf;
            bind[i].buffer_length = lens[i];
            bind[i].length = &lens[i];
            break;
        default:
            nulls[i] = 1;
            bind[i].buffer_type = MYSQL_TYPE_NULL;
            break;
        }
        bind[i].is_null = &nulls[i];
    }
    if (nParam && mysql_stmt_bind_param(st, bind))
    {
        KDBLog("bind LOI: %s | SQL=%s", mysql_stmt_error(st), szSql);
        mysql_stmt_close(st);
        if (pnErrors) InterlockedIncrement(pnErrors);
        return false;
    }
    if (mysql_stmt_execute(st))
    {
        KDBLog("execute LOI %u: %s | SQL=%s", mysql_stmt_errno(st),
               mysql_stmt_error(st), szSql);
        mysql_stmt_close(st);
        if (pnErrors) InterlockedIncrement(pnErrors);
        return false;
    }
    if (pnAffected) *pnAffected = (__int64)mysql_stmt_affected_rows(st);
    if (pnInsertId) *pnInsertId = (__int64)mysql_stmt_insert_id(st);

    // ---- doc ket qua neu co ----
    if (pfnRow)
    {
        MYSQL_RES *meta = mysql_stmt_result_metadata(st);
        if (meta)
        {
            int nCol = (int)mysql_num_fields(meta);
            if (nCol > 0 && nCol <= 64)
            {
                MYSQL_BIND rb[64];
                unsigned long rlen[64];
                my_bool rnull[64];
                memset(rb, 0, sizeof(rb));
                for (int i = 0; i < nCol; i++)
                {
                    rb[i].buffer_type = MYSQL_TYPE_BLOB;
                    rb[i].buffer = 0;
                    rb[i].buffer_length = 0;
                    rb[i].length = &rlen[i];
                    rb[i].is_null = &rnull[i];
                }
                if (!mysql_stmt_bind_result(st, rb) && !mysql_stmt_store_result(st))
                {
                    char *pCell[64];
                    const char *pVal[64];
                    int nLen[64];
                    memset(pCell, 0, sizeof(pCell));
                    int rc;
                    while ((rc = mysql_stmt_fetch(st)) == 0 || rc == MYSQL_DATA_TRUNCATED)
                    {
                        bool bTiep = true;
                        for (int i = 0; i < nCol; i++)
                        {
                            int n = rnull[i] ? 0 : (int)rlen[i];
                            pCell[i] = (char *)malloc(n + 1);
                            if (!pCell[i]) { bTiep = false; break; }
                            if (n)
                            {
                                MYSQL_BIND b;
                                memset(&b, 0, sizeof(b));
                                unsigned long got = 0;
                                b.buffer_type = MYSQL_TYPE_BLOB;
                                b.buffer = pCell[i];
                                b.buffer_length = n;
                                b.length = &got;
                                mysql_stmt_fetch_column(st, &b, i, 0);
                            }
                            pCell[i][n] = 0;
                            pVal[i] = pCell[i];
                            nLen[i] = n;
                        }
                        if (bTiep)
                        {
                            KDBRow row;
                            row.nCol = nCol;
                            row.pVal = pVal;
                            row.pLen = nLen;
                            bTiep = pfnRow(row, pUserParam);
                        }
                        for (int i = 0; i < nCol; i++)
                        {
                            if (pCell[i]) { free(pCell[i]); pCell[i] = 0; }
                        }
                        if (!bTiep) break;
                    }
                }
            }
            mysql_free_result(meta);
        }
    }
    mysql_stmt_close(st);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// [CL 04/09 V3] Ket noi DONG BO (pSync) truoc day KHONG BAO GIO duoc ping hay noi
// lai: mysql_ping chi xuat hien DUNG MOT LAN trong tep nay, o luong ghi nen (pAsync).
// wait_timeout cua may chu MySQL nay la 28.800 giay (8 gio), nen chi can pSync nam
// im qua nguong do la MOI Exec/Query hong VINH VIEN cho toi khi khoi dong lai - va
// hong CAM, chi tang m_nErrors. Hien chua no chi vi mailpoll.lua goi mot Query dong
// bo moi 30 giay, VO TINH giu ket noi song; bo hoac gian vong quet do la bom no.
// Mot lan ping ton ~0,05 ms, khong dang ke so voi 0,08 ms (doc) / 2,5 ms (ghi).
static void KDBEnsureSync(KMySQLDB::Impl *im)
{
    if (!im) return;
    if (im->pSync && mysql_ping(im->pSync) != 0)
    {
        KDBLog("ket noi dong bo: mat ket noi -- dang noi lai");
        mysql_close(im->pSync);
        im->pSync = KDBConnect(im, "ket noi dong bo");
    }
}

//////////////////////////////////////////////////////////////////////////////
bool KMySQLDB::Exec(const char *szSql, const KDBParam *pParam, int nParam,
                    __int64 *pnAffected, __int64 *pnInsertId)
{
    if (!m_bReady || !m_pImpl) return false;
    KDBEnsureSync(m_pImpl);
    return KDBRun(m_pImpl->pSync, szSql, pParam, nParam,
                  pnAffected, pnInsertId, 0, 0, &m_nErrors);
}

bool KMySQLDB::Query(const char *szSql, const KDBParam *pParam, int nParam,
                     KDBRowFunc pfnRow, void *pUserParam)
{
    if (!m_bReady || !m_pImpl) return false;
    KDBEnsureSync(m_pImpl);
    return KDBRun(m_pImpl->pSync, szSql, pParam, nParam,
                  0, 0, pfnRow, pUserParam, &m_nErrors);
}

bool KMySQLDB::Begin()
{
    if (!m_bReady || !m_pImpl) return false;
    return mysql_query(m_pImpl->pSync, "START TRANSACTION") == 0;
}
bool KMySQLDB::Commit()
{
    if (!m_bReady || !m_pImpl) return false;
    return mysql_query(m_pImpl->pSync, "COMMIT") == 0;
}
bool KMySQLDB::Rollback()
{
    if (!m_bReady || !m_pImpl) return false;
    return mysql_query(m_pImpl->pSync, "ROLLBACK") == 0;
}

//////////////////////////////////////////////////////////////////////////////
// Post -- day vao hang doi. Day la duong DUY NHAT duoc goi tu vong lap game.
//////////////////////////////////////////////////////////////////////////////
bool KMySQLDB::Post(const char *szSql, const KDBParam *pParam, int nParam)
{
    if (!m_bReady || !m_pImpl || !szSql) return false;
    if (nParam < 0 || nParam > KDB_MAX_PARAM) return false;
    if (m_nQueue >= KDB_MAX_QUEUE)
    {
        // Hang doi DAY: bo goi va dem lai. KHONG BAO GIO chan vong lap game --
        // mat mot dong nhat ky con hon giat ca may chu.
        InterlockedIncrement(&m_nDropped);
        return false;
    }

    // Tinh tong byte can sao chep cho cac tham so BLOB/STR
    int nBlob = 0;
    for (int i = 0; i < nParam; i++)
        if (pParam[i].type == KDBP_BLOB || pParam[i].type == KDBP_STR)
            nBlob += (pParam[i].len > 0 ? pParam[i].len : 0);

    KDBJob *j = (KDBJob *)malloc(sizeof(KDBJob));
    if (!j) return false;
    memset(j, 0, sizeof(KDBJob));
    size_t nSql = strlen(szSql) + 1;
    j->pSql = (char *)malloc(nSql);
    if (!j->pSql) { free(j); return false; }
    memcpy(j->pSql, szSql, nSql);
    j->nParam = nParam;
    if (nBlob)
    {
        j->pBlobBuf = (char *)malloc(nBlob);
        if (!j->pBlobBuf) { free(j->pSql); free(j); return false; }
    }
    char *pDst = j->pBlobBuf;
    for (int i = 0; i < nParam; i++)
    {
        j->aParam[i] = pParam[i];
        if (pParam[i].type == KDBP_BLOB || pParam[i].type == KDBP_STR)
        {
            int n = pParam[i].len > 0 ? pParam[i].len : 0;
            if (n) memcpy(pDst, pParam[i].buf, n);
            j->aParam[i].buf = pDst;      // tro vao ban SAO, khong giu con tro cua ben goi
            j->aParam[i].len = n;
            pDst += n;
        }
    }

    EnterCriticalSection(&m_pImpl->csQueue);
    if (m_pImpl->pTail) m_pImpl->pTail->pNext = j;
    else                m_pImpl->pHead = j;
    m_pImpl->pTail = j;
    LeaveCriticalSection(&m_pImpl->csQueue);

    InterlockedIncrement(&m_nQueue);
    SetEvent(m_pImpl->hEvent);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
unsigned __stdcall KMySQLDB::_WorkerThunk(void *p)
{
    ((KMySQLDB *)p)->_Worker();
    return 0;
}

void KMySQLDB::_Worker()
{
    Impl *im = m_pImpl;
    for (;;)
    {
        WaitForSingleObject(im->hEvent, 200);
        for (;;)
        {
            EnterCriticalSection(&im->csQueue);
            KDBJob *j = im->pHead;
            if (j)
            {
                im->pHead = j->pNext;
                if (!im->pHead) im->pTail = 0;
            }
            LeaveCriticalSection(&im->csQueue);
            if (!j) break;

            if (im->pAsync && mysql_ping(im->pAsync) != 0)
            {
                KDBLog("luong ghi nen: mat ket noi -- dang noi lai");
                mysql_close(im->pAsync);
                im->pAsync = KDBConnect(im, "ket noi ghi nen");
            }
            if (im->pAsync)
                KDBRun(im->pAsync, j->pSql, j->aParam, j->nParam, 0, 0, 0, 0, &m_nErrors);
            else
                InterlockedIncrement(&m_nErrors);

            free(j->pSql);
            free(j->pBlobBuf);
            free(j);
            InterlockedDecrement(&m_nQueue);
        }
        if (im->bStop && m_nQueue <= 0) break;
    }
}

//////////////////////////////////////////////////////////////////////////////
void KMySQLDB::Tick()
{
    // Chi canh bao khi hang doi phinh -- de chu game biet TRUOC khi mat nhat ky.
    static DWORD s_dwLast = 0;
    if (m_nQueue < KDB_MAX_QUEUE / 2) return;
    DWORD now = GetTickCount();
    if (now - s_dwLast < 30000) return;
    s_dwLast = now;
    KDBLog("CANH BAO: hang doi ghi nen dang o %d/%d (da bo %d goi)",
           (int)m_nQueue, KDB_MAX_QUEUE, (int)m_nDropped);
}

#endif // _SERVER
