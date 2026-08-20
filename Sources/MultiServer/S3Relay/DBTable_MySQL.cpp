//////////////////////////////////////////////////////////////////////////////
// DBTable_MySQL.cpp  (S3Relay)
//
// Cai dat ZDBTable va CDBTableReadOnly tren nen MySQL, THAY CHO Berkeley DB.
// Khai bao van la DBTable.h -- TONGDB.CPP va FriendMgr.cpp KHONG phai sua dong nao.
//
// Ca 5 kho (friend / TongTable / MemberTable / MoneyTable / ZhaoMuTable) nam trong
// MOT bang MySQL `relay_kv`, phan biet bang cot `store`. Blob giu NGUYEN VEN tung
// byte nen di tru chung minh duoc bang hash va khong the sai anh xa truong.
//
// ---------------------------------------------------------------------------
// !! TUYET DOI KHONG DUNG STL TRONG TEP NAY !!
// S3Relay.vcxproj dat RuntimeLibrary = MultiThreaded (/MT) NHUNG lai
// IgnoreSpecificDefaultLibraries = libcmt.lib;msvcrt.lib;msvcrtd.lib. Chi can
// include <string>/<vector> la keo libcpmt.lib vao, roi libcpmt doi `operator new`
// tu libcmt.lib da bi bo qua => 1.688 loi LNK2001. Da thu va do that.
// Vi vay toan bo tep dung dem C thuan + malloc/free.
// ---------------------------------------------------------------------------
//
// RANG BUOC PHAI GIU (doi chieu tung dong voi ban Berkeley DB)
//  1. closeCursor() giai phong bang free() -> moi vung cap phat bang malloc().
//  2. next() tra false thi con tro TU HUY (TONGDB.CPP:270/305/408/563 dua vao day).
//  3. Khoa bang `friend` KHONG gom NUL, cac bang Tong CO -> cat NUL cuoi cho ca hai.
//
// LO HONG CUA BAN CU DUOC VA
//  V1. remove(key, size, index) ben cu BO QUA index (DBTable.cpp:251) nen
//      TONGDB.CPP:181 `while(!m_MemberTable->remove(aTongName, aKeySize, 0)){}`
//      LAP VO TAN khi xoa bang hoi (chiem 100% CPU mot loi). Ban nay hien thuc
//      DUNG ngu nghia khoa phu -> vong lap thoat dung cach va xoa sach thanh vien.
//  V2. Xoa la mat vinh vien -> chep sang relay_delete_log truoc, trong 1 giao dich.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBTable.h"
#include <mysql.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define RELAY_INI       ".\\DataBase.ini"
#define RELAY_SECTION   "relaydb"
#define RELAY_LOG       "mysql_relay.log"
#define SCAN_PAGE_ROWS  64
#define RL_MAX_KEY      72
#define RL_MAX_STR      128

//////////////////////////////////////////////////////////////////////////////
// Nhat ky
//////////////////////////////////////////////////////////////////////////////
static void RlLog(const char *fmt, ...)
{
    FILE *f = fopen(RELAY_LOG, "a");
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
// Bo cuc khoa phu cua tung kho -- lay TU DU LIEU THAT, khong doan.
//   MemberTable : TMemberStruct.szTong   o offset 32 (szName[32] dung truoc)
//   ZhaoMuTable : TZhaoMuStruct.szTong   o offset 0
//   MoneyTable  : TMoneyStruct.m_dwMoney o offset 32 (szTong[32] dung truoc)
// Kich thuoc ban ghi da doi chieu voi du lieu that tren may test:
//   TongTable 6860, MemberTable 404 -- khop chinh xac.
//////////////////////////////////////////////////////////////////////////////
struct StoreCfg
{
    const char *ten;
    int         idx0_off;   // offset szTong trong blob, -1 = khong co khoa phu
    int         idx0_max;
    int         money_off;  // offset truong tien, -1 = khong co
    int         rec_size;   // kich thuoc ban ghi bat buoc, 0 = bien thien
    int         key_co_nul; // 1 = khoa cua kho nay gom byte NUL cuoi
};

static const StoreCfg g_stores[] = {
    { "friend",      -1,  0, -1,    0, 0 },   // ban ghi tu dong goi, do dai bien thien
    { "TongTable",   -1,  0, -1, 6860, 1 },
    { "MemberTable", 32, 32, -1,  404, 1 },
    { "MoneyTable",  -1,  0, 32,   36, 1 },
    { "ZhaoMuTable",  0, 32, -1,   76, 1 },
};

static const StoreCfg *TimStore(const char *name)
{
    int i;
    for (i = 0; i < (int)(sizeof(g_stores) / sizeof(g_stores[0])); i++)
        if (_stricmp(g_stores[i].ten, name) == 0)
            return &g_stores[i];
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Trang thai
//////////////////////////////////////////////////////////////////////////////
enum RlCursorMode { RC_ONE = 0, RC_SEC = 1, RC_SCAN = 2 };

struct RlRow
{
    char  key[RL_MAX_KEY];
    int   key_len;
    char *val;          // malloc
    int   val_len;
};

struct RlCursorImpl
{
    int    mode;
    RlRow  rows[SCAN_PAGE_ROWS];
    int    nrows;
    int    pos;
    char   seek[RL_MAX_KEY];
    int    seek_len;
};

struct RlTableImpl
{
    MYSQL          *conn;
    char            host[RL_MAX_STR], user[RL_MAX_STR], pass[RL_MAX_STR], db[RL_MAX_STR];
    unsigned        port;
    const StoreCfg *cfg;
    int             opened;
};

static void RlFreeRows(RlCursorImpl *ci)
{
    int i;
    for (i = 0; i < ci->nrows; i++)
    {
        if (ci->rows[i].val) { free(ci->rows[i].val); ci->rows[i].val = NULL; }
        ci->rows[i].val_len = 0;
    }
    ci->nrows = 0;
}

static void RlFreeCursorImpl(RlCursorImpl *ci)
{
    if (!ci) return;
    RlFreeRows(ci);
    free(ci);
}

//////////////////////////////////////////////////////////////////////////////
// Tien ich
//////////////////////////////////////////////////////////////////////////////
static void IniStr(const char *key, const char *def, char *out, int cap)
{
    GetPrivateProfileStringA(RELAY_SECTION, key, def, out, cap - 1, RELAY_INI);
    out[cap - 1] = 0;
    // cat khoang trang hai dau
    int n = (int)strlen(out);
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == '\t' ||
                     out[n - 1] == '\r' || out[n - 1] == '\n'))
        out[--n] = 0;
    int i = 0;
    while (out[i] == ' ' || out[i] == '\t') i++;
    if (i) memmove(out, out + i, strlen(out + i) + 1);
}
static int IniInt(const char *key, int def)
{
    return (int)GetPrivateProfileIntA(RELAY_SECTION, key, def, RELAY_INI);
}

static char *DupMalloc(const void *p, int n)
{
    char *q = (char *)malloc(n > 0 ? n : 1);
    if (!q) return NULL;
    if (n > 0) memcpy(q, p, n);
    return q;
}

// Cat MOI byte NUL cuoi. An toan cho ca hai quy uoc vi ten khong bao gio ket
// thuc bang byte NUL that.
static int CatNul(const char *p, int n, char *out, int cap)
{
    if (n > cap - 1) n = cap - 1;
    if (n < 0) n = 0;
    if (n) memcpy(out, p, n);
    while (n > 0 && out[n - 1] == 0) n--;
    out[n] = 0;
    return n;
}

//////////////////////////////////////////////////////////////////////////////
// Ket noi
//////////////////////////////////////////////////////////////////////////////
static int RlConnect(RlTableImpl *im)
{
    if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
    im->conn = mysql_init(NULL);
    if (!im->conn) { RlLog("mysql_init that bai"); return 0; }
    mysql_options(im->conn, MYSQL_SET_CHARSET_NAME, "latin1");
    unsigned int to = 5;
    mysql_options(im->conn, MYSQL_OPT_CONNECT_TIMEOUT, &to);
    // Tham so cuoi PHAI la 0: them CLIENT_MULTI_STATEMENTS la mo cua cho
    // "'; DROP TABLE ...; --".
    if (!mysql_real_connect(im->conn, im->host, im->user, im->pass, im->db,
                            im->port, NULL, 0))
    {
        RlLog("mysql_real_connect LOI: %s", mysql_error(im->conn));
        mysql_close(im->conn);
        im->conn = NULL;
        return 0;
    }
    mysql_query(im->conn, "SET SESSION sql_mode='STRICT_ALL_TABLES'");
    mysql_autocommit(im->conn, 1);
    return 1;
}

static int RlEnsure(RlTableImpl *im)
{
    if (im->conn && mysql_ping(im->conn) == 0) return 1;
    RlLog("mat ket noi MySQL -- dang noi lai");
    return RlConnect(im);
}

//////////////////////////////////////////////////////////////////////////////
// Nap ket qua truy van vao cursor. Tra ve so dong doc duoc, -1 neu loi.
//////////////////////////////////////////////////////////////////////////////
static int RlLoad(RlTableImpl *im, RlCursorImpl *ci, const char *sql,
                  const char *param, int param_len, int motDong)
{
    RlFreeRows(ci);
    ci->pos = 0;

    MYSQL_STMT *st = mysql_stmt_init(im->conn);
    if (!st) return -1;
    if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)))
    {
        RlLog("prepare LOI: %s | SQL=%s", mysql_stmt_error(st), sql);
        mysql_stmt_close(st);
        return -1;
    }

    MYSQL_BIND p[2];
    memset(p, 0, sizeof(p));
    unsigned long lstore = (unsigned long)strlen(im->cfg->ten);
    unsigned long lparam = (unsigned long)param_len;
    p[0].buffer_type = MYSQL_TYPE_STRING;
    p[0].buffer = (void *)im->cfg->ten;
    p[0].buffer_length = lstore;
    p[0].length = &lstore;
    p[1].buffer_type = MYSQL_TYPE_BLOB;
    p[1].buffer = (void *)param;
    p[1].buffer_length = lparam;
    p[1].length = &lparam;

    if (mysql_stmt_bind_param(st, p) || mysql_stmt_execute(st))
    {
        RlLog("truy van LOI: %s", mysql_stmt_error(st));
        mysql_stmt_close(st);
        return -1;
    }

    MYSQL_BIND res[2];
    unsigned long l0 = 0, l1 = 0;
    my_bool n0 = 0, n1 = 0;
    memset(res, 0, sizeof(res));
    res[0].buffer_type = MYSQL_TYPE_BLOB; res[0].length = &l0; res[0].is_null = &n0;
    res[1].buffer_type = MYSQL_TYPE_BLOB; res[1].length = &l1; res[1].is_null = &n1;
    if (mysql_stmt_bind_result(st, res) || mysql_stmt_store_result(st))
    {
        RlLog("bind/store LOI: %s", mysql_stmt_error(st));
        mysql_stmt_close(st);
        return -1;
    }

    int rc;
    while (ci->nrows < SCAN_PAGE_ROWS &&
           ((rc = mysql_stmt_fetch(st)) == 0 || rc == MYSQL_DATA_TRUNCATED))
    {
        RlRow *row = &ci->rows[ci->nrows];
        memset(row, 0, sizeof(*row));

        // --- khoa ---
        int klen = (int)l0;
        if (klen > RL_MAX_KEY - 2) klen = RL_MAX_KEY - 2;
        if (klen > 0)
        {
            MYSQL_BIND b;
            memset(&b, 0, sizeof(b));
            unsigned long got = 0;
            b.buffer_type = MYSQL_TYPE_BLOB;
            b.buffer = row->key;
            b.buffer_length = klen;
            b.length = &got;
            if (mysql_stmt_fetch_column(st, &b, 0, 0))
            {
                RlLog("fetch khoa LOI: %s", mysql_stmt_error(st));
                mysql_stmt_close(st);
                return -1;
            }
        }
        row->key_len = klen;
        // Tra ve khoa dung quy uoc cua kho: cac bang Tong mong doi CO byte NUL.
        if (im->cfg->key_co_nul)
        {
            row->key[row->key_len] = 0;
            row->key_len++;
        }

        // --- gia tri ---
        row->val_len = (int)l1;
        row->val = (char *)malloc(row->val_len > 0 ? row->val_len : 1);
        if (!row->val) { mysql_stmt_close(st); return -1; }
        if (row->val_len > 0)
        {
            MYSQL_BIND b;
            memset(&b, 0, sizeof(b));
            unsigned long got = 0;
            b.buffer_type = MYSQL_TYPE_BLOB;
            b.buffer = row->val;
            b.buffer_length = row->val_len;
            b.length = &got;
            if (mysql_stmt_fetch_column(st, &b, 1, 0))
            {
                RlLog("fetch gia tri LOI: %s", mysql_stmt_error(st));
                free(row->val);
                row->val = NULL;
                mysql_stmt_close(st);
                return -1;
            }
        }

        // moc phan trang = khoa THO (chua noi NUL)
        ci->seek_len = klen;
        if (klen > 0) memcpy(ci->seek, row->key, klen);
        ci->seek[klen] = 0;

        ci->nrows++;
        if (motDong) break;
    }
    mysql_stmt_close(st);
    return ci->nrows;
}

//////////////////////////////////////////////////////////////////////////////
// Con tro
//////////////////////////////////////////////////////////////////////////////
static ZCursor *RlMakeCursor(RlCursorImpl *ci)
{
    if (ci->pos >= ci->nrows) return NULL;
    RlRow *r = &ci->rows[ci->pos];
    ZCursor *c = new ZCursor;
    memset(c, 0, sizeof(ZCursor));
    c->pImpl = ci;
    c->index = ci->pos;
    c->dbcp = NULL;
    c->data = DupMalloc(r->val, r->val_len);
    c->size = r->val_len;
    if (ci->mode == RC_SCAN)
    {
        c->bTravel = true;
        c->key = DupMalloc(r->key, r->key_len);
        c->key_size = r->key_len;
    }
    return c;
}

void ZDBTable::closeCursor(ZCursor *cursor)
{
    if (!cursor) return;
    if (cursor->bTravel) free(cursor->key);
    free(cursor->data);
    RlFreeCursorImpl((RlCursorImpl *)cursor->pImpl);
    delete cursor;
}

//////////////////////////////////////////////////////////////////////////////
// ZDBTable
//////////////////////////////////////////////////////////////////////////////
ZDBTable::ZDBTable(const char *path, const char *name)
{
    bStop = false;
    index_number = 0;
    dbenv = NULL;
    primary_db = NULL;
    dbcp = NULL;
    memset(env_path, 0, sizeof(env_path));
    memset(table_name, 0, sizeof(table_name));
    strncpy(table_name, name ? name : "", MAX_TABLE_NAME - 1);
    strncpy(env_path, path ? path : "", MAX_TABLE_NAME - 1);

    RlTableImpl *im = (RlTableImpl *)malloc(sizeof(RlTableImpl));
    memset(im, 0, sizeof(RlTableImpl));
    IniStr("Server", "127.0.0.1", im->host, RL_MAX_STR);
    IniStr("User", "root", im->user, RL_MAX_STR);
    IniStr("PassWord", "123456", im->pass, RL_MAX_STR);
    IniStr("DataBase", "jx1_role", im->db, RL_MAX_STR);
    im->port = (unsigned)IniInt("Port", 3306);
    im->cfg = TimStore(table_name);
    m_pImpl = im;
    RlLog("ZDBTable(\"%s\",\"%s\") -> MySQL %s:%u/%s store=%s",
          env_path, table_name, im->host, im->port, im->db,
          im->cfg ? im->cfg->ten : "(KHONG NHAN RA)");
}

ZDBTable::~ZDBTable()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (im)
    {
        if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
        free(im);
        m_pImpl = NULL;
    }
}

int ZDBTable::addIndex(GetIndexFunc func, bool isUnique)
{
    // Khoa phu Berkeley DB (get_TongName / get_ZhaoMuTongName) duoc thay bang cot
    // `idx0` + chi muc SQL. Ham truyen vao KHONG con duoc goi.
    if (index_number + 1 >= MAX_INDEX) return index_number;
    get_index_funcs[index_number] = func;
    is_index_unique[index_number] = isUnique;
    return index_number++;
}

bool ZDBTable::open()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im) return false;
    bStop = false;
    if (!im->cfg)
    {
        RlLog("open() THAT BAI: khong nhan ra kho \"%s\"", table_name);
        return false;
    }
    if (!RlConnect(im)) return false;

    char sql[512];
    _snprintf(sql, sizeof(sql) - 1,
              "SELECT COUNT(*) FROM information_schema.TABLES "
              "WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='relay_kv'", im->db);
    if (mysql_query(im->conn, sql))
    {
        RlLog("kiem bang LOI: %s", mysql_error(im->conn));
        return false;
    }
    MYSQL_RES *r = mysql_store_result(im->conn);
    long n = 0;
    if (r)
    {
        MYSQL_ROW row = mysql_fetch_row(r);
        if (row && row[0]) n = atol(row[0]);
        mysql_free_result(r);
    }
    if (n <= 0)
    {
        RlLog("KHONG tim thay bang `%s`.`relay_kv` -- open() tra false", im->db);
        return false;
    }
    im->opened = 1;
    RlLog("open() OK -- store=%s", im->cfg->ten);
    return true;
}

void ZDBTable::close()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im) return;
    if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
    im->opened = 0;
    RlLog("close() store=%s", im->cfg ? im->cfg->ten : "?");
}

bool ZDBTable::commit()
{
    // Ban Berkeley DB o day lam sync + txn_checkpoint TOAN DB sau MOI ban ghi --
    // rat cham. MySQL da chot ngay tai autocommit nen khong con gi de lam.
    return true;
}

void ZDBTable::removeLog()
{
    // Khong con tep log Berkeley DB.
}

ZCursor *ZDBTable::_search(bool bKey, const char *key_ptr, int key_size, int index)
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im || !im->opened || !im->cfg) return NULL;
    if (bKey)
    {
        RlLog("search_key() khong noi nao goi o S3Relay -- tra NULL");
        return NULL;
    }
    if (!RlEnsure(im)) return NULL;

    RlCursorImpl *ci = (RlCursorImpl *)malloc(sizeof(RlCursorImpl));
    memset(ci, 0, sizeof(RlCursorImpl));

    char sql[512];
    int got;
    if (!key_ptr || !key_size)
    {
        ci->mode = RC_SCAN;
        _snprintf(sql, sizeof(sql) - 1,
                  "SELECT k, v FROM relay_kv WHERE store=? AND k > ? ORDER BY k LIMIT %d",
                  SCAN_PAGE_ROWS);
        got = RlLoad(im, ci, sql, "", 0, 0);
    }
    else
    {
        char k[RL_MAX_KEY];
        int klen = CatNul(key_ptr, key_size, k, RL_MAX_KEY);
        if (index < 0)
        {
            ci->mode = RC_ONE;
            strcpy(sql, "SELECT k, v FROM relay_kv WHERE store=? AND k=?");
            got = RlLoad(im, ci, sql, k, klen, 1);
        }
        else
        {
            ci->mode = RC_SEC;
            strcpy(sql, "SELECT k, v FROM relay_kv WHERE store=? AND idx0=? ORDER BY k");
            got = RlLoad(im, ci, sql, k, klen, 0);
        }
    }
    if (got <= 0) { RlFreeCursorImpl(ci); return NULL; }

    ZCursor *c = RlMakeCursor(ci);
    if (!c) { RlFreeCursorImpl(ci); return NULL; }
    return c;
}

bool ZDBTable::_next(bool bKey, ZCursor *cursor)
{
    if (!cursor || !cursor->pImpl) return false;
    RlCursorImpl *ci = (RlCursorImpl *)cursor->pImpl;
    RlTableImpl *im = (RlTableImpl *)m_pImpl;

    free(cursor->data);
    cursor->data = NULL;
    if (cursor->bTravel) { free(cursor->key); cursor->key = NULL; }

    ci->pos++;
    if (ci->pos >= ci->nrows)
    {
        int conTrang = 0;
        if (ci->mode == RC_SCAN && im && RlEnsure(im))
        {
            char sql[512];
            char moc[RL_MAX_KEY];
            int mlen = ci->seek_len;
            if (mlen > 0) memcpy(moc, ci->seek, mlen);
            moc[mlen] = 0;
            _snprintf(sql, sizeof(sql) - 1,
                      "SELECT k, v FROM relay_kv WHERE store=? AND k > ? ORDER BY k LIMIT %d",
                      SCAN_PAGE_ROWS);
            if (RlLoad(im, ci, sql, moc, mlen, 0) > 0)
                conTrang = 1;
        }
        if (!conTrang)
        {
            // Het du lieu: con tro TU HUY, dung quy uoc cua ban Berkeley DB.
            RlFreeCursorImpl(ci);
            cursor->pImpl = NULL;
            delete cursor;
            return false;
        }
    }

    RlRow *r = &ci->rows[ci->pos];
    cursor->index = ci->pos;
    cursor->data = DupMalloc(r->val, r->val_len);
    cursor->size = r->val_len;
    if (cursor->bTravel)
    {
        cursor->key = DupMalloc(r->key, r->key_len);
        cursor->key_size = r->key_len;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// add
//////////////////////////////////////////////////////////////////////////////
bool ZDBTable::add(const char *key_ptr, int key_size, const char *data_ptr, int data_size)
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im || !im->opened || !im->cfg || bStop) return false;
    if (!data_ptr || data_size <= 0 || data_size > 16 * 1024 * 1024) return false;

    char k[RL_MAX_KEY];
    int klen = CatNul(key_ptr, key_size, k, RL_MAX_KEY);
    if (klen <= 0 || klen > 64)
    {
        RlLog("add() TU CHOI: khoa dai %d byte (store=%s)", klen, im->cfg->ten);
        return false;
    }
    // Kho co ban ghi co dinh thi phai dung co. Ban cu am tham bo qua ban ghi lech
    // co (TONGDB.CPP so 6860 vs 6732) -- o day tu choi ro rang va co nhat ky.
    if (im->cfg->rec_size && data_size != im->cfg->rec_size)
    {
        RlLog("add(\"%s\") TU CHOI: store=%s can %d byte nhung nhan %d",
              k, im->cfg->ten, im->cfg->rec_size, data_size);
        return false;
    }
    if (!RlEnsure(im)) return false;

    // Rut khoa phu va tien tu blob
    char idx0[RL_MAX_KEY];
    int idx0_len = 0;
    my_bool idx_null = 1;
    if (im->cfg->idx0_off >= 0 && data_size >= im->cfg->idx0_off + im->cfg->idx0_max)
    {
        const char *p = data_ptr + im->cfg->idx0_off;
        while (idx0_len < im->cfg->idx0_max && p[idx0_len]) idx0_len++;
        memcpy(idx0, p, idx0_len);
        idx0[idx0_len] = 0;
        idx_null = 0;
    }
    else
        idx0[0] = 0;

    __int64 money = 0;
    my_bool money_null = 1;
    if (im->cfg->money_off >= 0 && data_size >= im->cfg->money_off + 4)
    {
        unsigned int m = 0;
        memcpy(&m, data_ptr + im->cfg->money_off, 4);
        money = (__int64)m;   // DWORD KHONG DAU -> BIGINT CO DAU, khong the tran am
        money_null = 0;
    }

    const char *sql =
        "INSERT INTO relay_kv (store,k,v,v_len,idx0,money) VALUES (?,?,?,?,?,?) "
        "ON DUPLICATE KEY UPDATE v=VALUES(v), v_len=VALUES(v_len), "
        "idx0=VALUES(idx0), money=VALUES(money)";

    MYSQL_STMT *st = mysql_stmt_init(im->conn);
    if (!st) return false;
    if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)))
    {
        RlLog("add prepare LOI: %s", mysql_stmt_error(st));
        mysql_stmt_close(st);
        return false;
    }

    MYSQL_BIND p[6];
    memset(p, 0, sizeof(p));
    unsigned long lstore = (unsigned long)strlen(im->cfg->ten);
    unsigned long lk = (unsigned long)klen;
    unsigned long lv = (unsigned long)data_size;
    unsigned long li = (unsigned long)idx0_len;
    __int64 vlen = data_size;
    p[0].buffer_type = MYSQL_TYPE_STRING; p[0].buffer = (void *)im->cfg->ten;
    p[0].buffer_length = lstore; p[0].length = &lstore;
    p[1].buffer_type = MYSQL_TYPE_BLOB;   p[1].buffer = k;
    p[1].buffer_length = lk; p[1].length = &lk;
    p[2].buffer_type = MYSQL_TYPE_BLOB;   p[2].buffer = (void *)data_ptr;
    p[2].buffer_length = lv; p[2].length = &lv;
    p[3].buffer_type = MYSQL_TYPE_LONGLONG; p[3].buffer = &vlen;
    p[4].buffer_type = MYSQL_TYPE_BLOB;   p[4].buffer = idx0;
    p[4].buffer_length = li; p[4].length = &li; p[4].is_null = &idx_null;
    p[5].buffer_type = MYSQL_TYPE_LONGLONG; p[5].buffer = &money;
    p[5].is_null = &money_null;

    bool ok = true;
    if (mysql_stmt_bind_param(st, p) || mysql_stmt_execute(st))
    {
        unsigned err = mysql_stmt_errno(st);
        RlLog("add(\"%s\", store=%s) LOI %u: %s", k, im->cfg->ten, err, mysql_stmt_error(st));
        ok = false;
        if (err == 2006 || err == 2013)
        {
            mysql_stmt_close(st);
            st = NULL;
            if (RlConnect(im))
            {
                st = mysql_stmt_init(im->conn);
                if (st && !mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)) &&
                    !mysql_stmt_bind_param(st, p) && !mysql_stmt_execute(st))
                {
                    RlLog("add(\"%s\") thu lai sau khi noi lai: THANH CONG", k);
                    ok = true;
                }
            }
        }
    }
    if (st) mysql_stmt_close(st);
    return ok;
}

//////////////////////////////////////////////////////////////////////////////
// remove -- V1: hien thuc DUNG ngu nghia khoa phu, va khong xoa trang
//////////////////////////////////////////////////////////////////////////////
static int RlExec1(RlTableImpl *im, const char *sql, const char *k, int klen,
                   my_ulonglong *pAff)
{
    MYSQL_STMT *st = mysql_stmt_init(im->conn);
    if (!st) return 0;
    if (mysql_stmt_prepare(st, sql, (unsigned long)strlen(sql)))
    {
        RlLog("prepare LOI: %s", mysql_stmt_error(st));
        mysql_stmt_close(st);
        return 0;
    }
    MYSQL_BIND p[2];
    memset(p, 0, sizeof(p));
    unsigned long l0 = (unsigned long)strlen(im->cfg->ten);
    unsigned long l1 = (unsigned long)klen;
    p[0].buffer_type = MYSQL_TYPE_STRING; p[0].buffer = (void *)im->cfg->ten;
    p[0].buffer_length = l0; p[0].length = &l0;
    p[1].buffer_type = MYSQL_TYPE_BLOB;   p[1].buffer = (void *)k;
    p[1].buffer_length = l1; p[1].length = &l1;
    int ok = 1;
    if (mysql_stmt_bind_param(st, p) || mysql_stmt_execute(st))
    {
        RlLog("thuc thi LOI: %s", mysql_stmt_error(st));
        ok = 0;
    }
    else if (pAff)
        *pAff = mysql_stmt_affected_rows(st);
    mysql_stmt_close(st);
    return ok;
}

bool ZDBTable::remove(const char *key_ptr, int key_size, int index)
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im || !im->opened || !im->cfg) return false;
    if (!RlEnsure(im)) return false;

    char k[RL_MAX_KEY];
    int klen = CatNul(key_ptr, key_size, k, RL_MAX_KEY);
    if (klen <= 0) return false;

    // index >= 0 : xoa MOT ban ghi khop KHOA PHU (idx0). Ban Berkeley DB bo qua
    // tham so nay nen TONGDB.CPP:181 lap vo tan; xem ghi chu dau tep.
    const char *cot = (index >= 0) ? "idx0" : "k";

    if (mysql_query(im->conn, "START TRANSACTION"))
    {
        RlLog("remove() START TRANSACTION LOI: %s", mysql_error(im->conn));
        return false;
    }

    char sql[512];
    int ok;
    my_ulonglong xoa = 0;

    _snprintf(sql, sizeof(sql) - 1,
        "INSERT INTO relay_delete_log (store,k,v,v_len,deleted_at) "
        "SELECT store,k,v,v_len,NOW() FROM relay_kv WHERE store=? AND %s=? LIMIT 1", cot);
    ok = RlExec1(im, sql, k, klen, NULL);

    if (ok)
    {
        _snprintf(sql, sizeof(sql) - 1,
                  "DELETE FROM relay_kv WHERE store=? AND %s=? LIMIT 1", cot);
        ok = RlExec1(im, sql, k, klen, &xoa);
    }

    if (ok && xoa > 0)
        mysql_query(im->conn, "COMMIT");
    else
        mysql_query(im->conn, "ROLLBACK");

    // Berkeley DB tra false khi khong co gi de xoa -- giu dung, vi TONGDB.CPP:181
    // dua vao gia tri nay de thoat vong lap.
    return (ok && xoa > 0);
}

//////////////////////////////////////////////////////////////////////////////
// Cac ham khong noi nao goi -- giu de lien ket duoc
//////////////////////////////////////////////////////////////////////////////
ZCursor *ZDBTable::GetRecord(int cpMode, int index)
{
    RlLog("GetRecord() chua cai dat o ban MySQL (khong noi nao goi)");
    return NULL;
}
ZCursor *ZDBTable::GetRecord_key(int cpMode, int index)
{
    RlLog("GetRecord_key() chua cai dat o ban MySQL (khong noi nao goi)");
    return NULL;
}
bool ZDBTable::GetRecordEx(char *aBuffer, int &size, char *aKeyBuffer, int &keysize,
                           int cpMode, int index, int nBufCap, int nKeyCap)
{
    RlLog("GetRecordEx() chua cai dat o ban MySQL (khong noi nao goi)");
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// CDBTableReadOnly -- FriendMgr mo THEM mot the hien chi-doc tren cung kho
//////////////////////////////////////////////////////////////////////////////
CDBTableReadOnly::CDBTableReadOnly(const char *path, const char *name)
{
    dbenv = NULL;
    primary_db = NULL;
    dbcp = NULL;
    memset(env_path, 0, sizeof(env_path));
    memset(table_name, 0, sizeof(table_name));
    strncpy(table_name, name ? name : "", MAX_TABLE_NAME - 1);
    strncpy(env_path, path ? path : "", MAX_TABLE_NAME - 1);

    RlTableImpl *im = (RlTableImpl *)malloc(sizeof(RlTableImpl));
    memset(im, 0, sizeof(RlTableImpl));
    IniStr("Server", "127.0.0.1", im->host, RL_MAX_STR);
    IniStr("User", "root", im->user, RL_MAX_STR);
    IniStr("PassWord", "123456", im->pass, RL_MAX_STR);
    IniStr("DataBase", "jx1_role", im->db, RL_MAX_STR);
    im->port = (unsigned)IniInt("Port", 3306);
    im->cfg = TimStore(table_name);
    m_pImpl = im;
}

CDBTableReadOnly::~CDBTableReadOnly()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (im)
    {
        if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
        free(im);
        m_pImpl = NULL;
    }
}

bool CDBTableReadOnly::open()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im || !im->cfg) return false;
    if (!RlConnect(im)) return false;
    im->opened = 1;
    RlLog("CDBTableReadOnly::open() OK -- store=%s", im->cfg->ten);
    return true;
}

void CDBTableReadOnly::close()
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im) return;
    if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
    im->opened = 0;
}

void CDBTableReadOnly::closeCursor(ZCursor *cursor)
{
    if (!cursor) return;
    if (cursor->bTravel) free(cursor->key);
    free(cursor->data);
    RlFreeCursorImpl((RlCursorImpl *)cursor->pImpl);
    delete cursor;
}

ZCursor *CDBTableReadOnly::_search(bool bKey, const char *key_ptr, int key_size, int index)
{
    RlTableImpl *im = (RlTableImpl *)m_pImpl;
    if (!im || !im->opened || !im->cfg || bKey) return NULL;
    if (!key_ptr || !key_size) return NULL;
    if (!RlEnsure(im)) return NULL;

    RlCursorImpl *ci = (RlCursorImpl *)malloc(sizeof(RlCursorImpl));
    memset(ci, 0, sizeof(RlCursorImpl));
    ci->mode = RC_ONE;

    char k[RL_MAX_KEY];
    int klen = CatNul(key_ptr, key_size, k, RL_MAX_KEY);
    if (RlLoad(im, ci, "SELECT k, v FROM relay_kv WHERE store=? AND k=?", k, klen, 1) <= 0)
    { RlFreeCursorImpl(ci); return NULL; }

    ZCursor *c = RlMakeCursor(ci);
    if (!c) { RlFreeCursorImpl(ci); return NULL; }
    return c;
}
