//////////////////////////////////////////////////////////////////////////////
// DBTable_MySQL.cpp
//
// Cai dat lop ZDBTable tren nen MySQL, THAY CHO Berkeley DB.
// Khai bao van la DBTable.h -- moi tep goi (IDBRoleServer.cpp, DBBackup.cpp,
// ClientNode.cpp) KHONG phai sua mot dong nao.
//
// Muon quay lai Berkeley DB: trong Goddess.vcxproj doi lai DBTable.cpp
// (ban goc con nguyen o DBTable_BDB_goc.cpp.bak) va bo DBTable_MySQL.cpp.
//
// ---------------------------------------------------------------------------
// NHUNG RANG BUOC BAT BUOC PHAI GIU (da doi chieu tung dong voi ban Berkeley DB)
//
//  1. closeCursor() giai phong bang free() -> moi vung key/data PHAI cap phat
//     bang malloc(), TUYET DOI khong dung new[].
//  2. next() tra false thi con tro TU HUY (ben goi khong closeCursor nua):
//     IDBRoleServer.cpp:156/224/285/638, DBBackup.cpp:410-420 deu dua vao quy uoc nay.
//  3. DBBackup.cpp:885 SUA THANG vung nho cursor->data roi ghi lai -> vung do phai
//     la bo nho rieng cua cursor, khong duoc la dem dung chung cua thu vien.
//  4. first() trong ban Berkeley DB goi _search(false, NULL, 0, -1) va vo tinh doc
//     index_db[-1] -- vi bo cuc lop, o nho do chinh la primary_db. Nghia la first()
//     duyet BANG CHINH. Ban MySQL nay duyet bang chinh -- dung hanh vi thuc te.
//  5. search_key()/next_key() KHONG duoc goi o bat cu dau trong Goddess
//     (da grep toan bo *.cpp) -> cai dat tra ve NULL/false va ghi nhat ky.
//
// ---------------------------------------------------------------------------
// NHUNG LO HONG CUA BAN CU DUOC VA NGAY TRONG TANG NAY
//
//  V1. XOA LA MAT VINH VIEN. ZDBTable::remove() cu goi thang db->del.
//      Ban nay: INSERT vao role_delete_log ROI moi DELETE, trong MOT transaction.
//  V2. dwDataLen la int CO DAU va IDBRoleServer.cpp:478 chi chan tren.
//      Gia tri 0..3 hoac AM lot qua roi lam `dwDataLen - 4` tran nguoc.
//      Ban nay chan ca hai dau ngay tai add() truoc khi cham DB.
//  V3. Ghi that bai truoc day im lang. Ban nay ghi vao bang role_save_fail
//      (giu nguyen goi bi tu choi) va tra false de tang tren biet.
//  V4. Mat ket noi giua chung: tu noi lai + chuan bi lai cau lenh + thu lai 1 lan.
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBTable.h"
#include <mysql.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

#include "S3DBInterface.h"
// CRC32 nguyen ban zlib, dung CHINH xac ham ma IDBRoleServer.cpp:6 dang dung
// (..\Common\ nam trong AdditionalIncludeDirectories).
#include "CRC32.h"

//////////////////////////////////////////////////////////////////////////////
// Cau hinh doc tu DataBase.ini, section [roledb]
//////////////////////////////////////////////////////////////////////////////
#define ROLEDB_INI          ".\\DataBase.ini"
#define ROLEDB_SECTION      "roledb"
#define ROLEDB_LOG          "mysql_roledb.log"

// Vung vat pham phai chia het cho kich thuoc mot vat pham.
// Da kiem chung tren 2.822 ban ghi that: sizeof(TDBItemData) == 233.
#define ITEM_REC_SIZE       ((int)sizeof(TDBItemData))
// Tran cung, giong het IDBRoleServer.cpp:478
#define MAX_ROLE_DATALEN    (64 * 1024 * 5)
// Chan duoi: header TRoleData truoc pBuffer. Do duoc = 745.
#define MIN_ROLE_DATALEN    ((int)(sizeof(TRoleData) - 1))
// So dong lay moi trang khi duyet toan bang (first/next)
#define SCAN_PAGE_ROWS      64

//////////////////////////////////////////////////////////////////////////////
// CHONG LOI DU LIEU O TANG UNG DUNG
//
// InnoDB da lo phan hong do DIA/MAT DIEN (doublewrite + checksum crc32 +
// flush_log_at_trx_commit=1). Nhung loai loi that su lam mat do trong game la:
// GAME ghi de mot blob THIEU VAT PHAM, va DB ngoan ngoan luu dung cai sai do.
// Ba co che duoi day nham vao dung loai loi do.
//
//  (1) role_history : chup anh ban ghi CU TRUOC KHI ghi de -> quay lui duoc.
//      Sao chep NGAY TRONG MAY CHU (INSERT ... SELECT) nen blob khong phai
//      di qua duong mang lan nao.
//  (2) role_anomaly : so cot dan xuat cu <-> moi. Vat pham tut manh, tien nhay
//      vot, cap tut -> ghi chuong bao NGAY LUC XAY RA, kem con tro toi ban chup.
//  (3) Kiem CRC khi DOC, nhung CHI bao dong khi cot crc_ok=1 ma byte lai khong
//      khop -- tuc hong SAU khi ghi. Khong bao dong voi 82% ban ghi cu von da
//      co CRC sai san (do CDBBackup::SaveStatInfo ghi de ma khong tinh lai CRC),
//      nho vay chuong khong keu oan.
//
// Nguong doc tu DataBase.ini section [roledb], deu tat duoc bang cach dat 0.
//////////////////////////////////////////////////////////////////////////////
static int     g_nLichSuPhut        = 30;         // gian cach chup anh dinh ky (phut), 0 = tat
static int     g_nNguongMatVatPham  = 5;          // tut bao nhieu vat pham thi bao dong, 0 = tat
static __int64 g_nNguongTien        = 100000000;  // tien nhay bao nhieu thi bao dong, 0 = tat
static int     g_nKiemCrcKhiDoc     = 1;          // 1 = kiem CRC moi lan doc

struct MyTableImpl;
static void GhiBaoDong(MyTableImpl *im, const std::string &k, const char *kind,
                       const char *chi_tiet, unsigned __int64 old_ver, __int64 history_id);

//////////////////////////////////////////////////////////////////////////////
// Nhat ky
//////////////////////////////////////////////////////////////////////////////
static void DbLog(const char *fmt, ...)
{
    FILE *f = fopen(ROLEDB_LOG, "a");
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
// Trang thai rieng cua mot con tro
//////////////////////////////////////////////////////////////////////////////
enum CursorMode
{
    CM_ONE = 0,     // tra cuu mot ban ghi theo khoa chinh
    CM_ACCOUNT = 1, // liet ke nhan vat cua mot tai khoan (chi muc phu roledb.0)
    CM_SCAN = 2     // duyet toan bang (first/next)
};

struct MyRow
{
    std::string key;    // role_name + byte NUL (giu dung quy uoc Berkeley DB)
    std::string blob;
};

struct MyCursorImpl
{
    int                 mode;
    std::vector<MyRow>  rows;
    size_t              pos;
    std::string         seek;       // khoa cuoi cua trang truoc (CM_SCAN)
    ZDBTable           *owner;
};

//////////////////////////////////////////////////////////////////////////////
// Trang thai rieng cua bang
//////////////////////////////////////////////////////////////////////////////
struct MyTableImpl
{
    MYSQL      *conn;
    std::string host, user, pass, db, table;
    unsigned    port;
    bool        opened;

    MyTableImpl() : conn(NULL), port(3306), opened(false) {}
};

//////////////////////////////////////////////////////////////////////////////
// Tien ich
//////////////////////////////////////////////////////////////////////////////
static std::string IniStr(const char *key, const char *def)
{
    char buf[512] = {0};
    GetPrivateProfileStringA(ROLEDB_SECTION, key, def, buf, sizeof(buf) - 1, ROLEDB_INI);
    // cat khoang trang hai dau
    std::string s(buf);
    while (!s.empty() && (s[0] == ' ' || s[0] == '\t')) s.erase(0, 1);
    while (!s.empty() && (s[s.size() - 1] == ' ' || s[s.size() - 1] == '\t' ||
                          s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n'))
        s.erase(s.size() - 1);
    return s;
}

static int IniInt(const char *key, int def)
{
    return (int)GetPrivateProfileIntA(ROLEDB_SECTION, key, def, ROLEDB_INI);
}

// Cap phat bang malloc va chep -- BAT BUOC vi closeCursor dung free()
static char *DupMalloc(const void *p, size_t n)
{
    char *q = (char *)malloc(n ? n : 1);
    if (!q) return NULL;
    if (n) memcpy(q, p, n);
    return q;
}

//////////////////////////////////////////////////////////////////////////////
// Giai ma cac truong dan xuat tu blob.
// Dung THANG struct trong Lib\S3DBInterface.h (581 dong, #pragma pack(push,1))
// nen khong the lech offset -- day la ban DUY NHAT duoc bien dich.
//////////////////////////////////////////////////////////////////////////////
struct RoleDerived
{
    const char *acc_name;
    int   data_len;
    int   level, series, lead_level, pk_value, kill_num;
    int   revival_id, enter_game_id, world_stat, sect_stat;
    __int64 money, save_money;
    double fight_exp;
    unsigned int  tong_id, crc32_stored;
    unsigned char sex, first_sect, sect, use_revive;
    short n_item, n_task, n_fight_skill, n_state_skill;
    bool  has_crc;
    bool  crc_ok;
};

// Nhan dien ban ghi CO hay KHONG co 4 byte CRC cuoi.
//   Dang A -- da tung duoc GameServer luu: KSOServer.cpp lam dwDataLen += 4 roi
//             noi CRC32 -> vung vat pham = [dwItemOffset, dwDataLen-4)
//   Dang B -- nhan vat MOI TAO: PlayerCreator.cpp:354 dat
//             dwDataLen = (BYTE*)pItemData - pData, KHONG co CRC
//             -> vung vat pham = [dwItemOffset, dwDataLen)
// Do that tren kho production: 1816 dang A, 3 dang B. Neu ep cung "luon co CRC"
// thi 3 nguoi choi that (CayTien1 / THIEULAMCUIBAP / TinheBanDam) bi loai IM LANG.
// Khong the nham: neu ca (X-4) va X chia het 233 thi 4 chia het 233 -- vo ly.
static bool DetectHasCrc(const TRoleData *p, bool *pHasCrc)
{
    int oi = p->dwItemOffset;
    int dl = p->dwDataLen;
    if (oi <= 0 || oi > dl) return false;
    int a = dl - 4 - oi;
    int b = dl - oi;
    if (a >= 0 && (a % ITEM_REC_SIZE) == 0) { *pHasCrc = true;  return true; }
    if (b >= 0 && (b % ITEM_REC_SIZE) == 0) { *pHasCrc = false; return true; }
    return false;
}

static bool ParseRole(const char *data, int size, RoleDerived *d)
{
    if (!data || size < MIN_ROLE_DATALEN) return false;
    const TRoleData *p = (const TRoleData *)data;
    if (p->dwDataLen != size) return false;

    bool hasCrc = true;
    if (!DetectHasCrc(p, &hasCrc)) return false;

    memset(d, 0, sizeof(*d));
    d->acc_name      = p->BaseInfo.caccname;
    d->data_len      = p->dwDataLen;
    d->sex           = p->BaseInfo.bSex ? 1 : 0;
    d->first_sect    = p->BaseInfo.nFirstSect;
    d->sect          = p->BaseInfo.nSect;
    d->use_revive    = (unsigned char)p->BaseInfo.cUseRevive;
    d->series        = p->BaseInfo.ifiveprop;
    d->level         = p->BaseInfo.ifightlevel;
    d->fight_exp     = p->BaseInfo.fightexp;
    d->lead_level    = p->BaseInfo.ileadlevel;
    d->money         = (__int64)p->BaseInfo.imoney;
    d->save_money    = (__int64)p->BaseInfo.isavemoney;
    d->pk_value      = p->BaseInfo.ipkvalue;
    d->kill_num      = p->BaseInfo.nKillPeopleNumber;
    d->tong_id       = p->BaseInfo.dwTongID;
    d->revival_id    = p->BaseInfo.irevivalid;
    d->enter_game_id = p->BaseInfo.ientergameid;
    d->world_stat    = p->BaseInfo.nWorldStat;
    d->sect_stat     = p->BaseInfo.nSectStat;
    d->n_item        = p->nItemCount;
    d->n_task        = p->nTaskCount;
    d->n_fight_skill = p->nFightSkillCount;
    d->n_state_skill = p->nStateSkillCount;
    d->has_crc       = hasCrc;

    if (hasCrc)
    {
        d->crc32_stored = *(const unsigned int *)(data + p->dwDataLen - 4);
        unsigned c = CRC32(0, data, (unsigned)(p->dwDataLen - 4));
        d->crc_ok = (c == d->crc32_stored);
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// Lop bo tro: chay mot cau lenh co tham so nhi phan
//////////////////////////////////////////////////////////////////////////////
class Stmt
{
public:
    Stmt(MYSQL *c) : m_conn(c), m_st(NULL) {}
    ~Stmt() { Close(); }

    bool Prepare(const char *sql)
    {
        Close();
        m_st = mysql_stmt_init(m_conn);
        if (!m_st) return false;
        if (mysql_stmt_prepare(m_st, sql, (unsigned long)strlen(sql)))
        {
            DbLog("prepare LOI: %s | SQL=%s", mysql_stmt_error(m_st), sql);
            Close();
            return false;
        }
        return true;
    }
    void Close()
    {
        if (m_st) { mysql_stmt_close(m_st); m_st = NULL; }
    }
    MYSQL_STMT *Get() { return m_st; }

private:
    MYSQL      *m_conn;
    MYSQL_STMT *m_st;
};

static void BindBlob(MYSQL_BIND &b, const void *p, unsigned long *len)
{
    memset(&b, 0, sizeof(b));
    b.buffer_type = MYSQL_TYPE_BLOB;
    b.buffer = (void *)p;
    b.buffer_length = *len;
    b.length = len;
}
static void BindI64(MYSQL_BIND &b, __int64 *v)
{
    memset(&b, 0, sizeof(b));
    b.buffer_type = MYSQL_TYPE_LONGLONG;
    b.buffer = v;
}
static void BindDbl(MYSQL_BIND &b, double *v)
{
    memset(&b, 0, sizeof(b));
    b.buffer_type = MYSQL_TYPE_DOUBLE;
    b.buffer = v;
}

//////////////////////////////////////////////////////////////////////////////
// Doc mot cot BLOB do dai bat ky ra std::string
//////////////////////////////////////////////////////////////////////////////
static bool FetchBlobColumn(MYSQL_STMT *st, unsigned idx, MYSQL_BIND *bind,
                            unsigned long len, std::string &out)
{
    out.resize(len);
    if (len == 0) return true;
    MYSQL_BIND b;
    memset(&b, 0, sizeof(b));
    b.buffer_type = MYSQL_TYPE_BLOB;
    b.buffer = &out[0];
    b.buffer_length = len;
    unsigned long got = 0;
    b.length = &got;
    if (mysql_stmt_fetch_column(st, &b, idx, 0))
    {
        DbLog("fetch_column LOI: %s", mysql_stmt_error(st));
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// ZDBTable -- ham dung / huy
//////////////////////////////////////////////////////////////////////////////
ZDBTable::ZDBTable(const char *path, const char *name)
{
    bStop = false;
    index_number = 0;
    dbenv = NULL;
    primary_db = NULL;
    memset(env_path, 0, sizeof(env_path));
    memset(table_name, 0, sizeof(table_name));
    strncpy(table_name, name ? name : "roledb", MAX_TABLE_NAME - 1);
    strncpy(env_path, path ? path : "database", MAX_TABLE_NAME - 1);

    MyTableImpl *im = new MyTableImpl();
    im->host  = IniStr("Server",   "127.0.0.1");
    im->port  = (unsigned)IniInt("Port", 3306);
    im->user  = IniStr("User",     "root");
    im->pass  = IniStr("PassWord", "123456");
    im->db    = IniStr("DataBase", "jx1_role");

    // Nguong chong loi du lieu -- dat 0 la tat tung co che
    g_nLichSuPhut       = IniInt("LichSuPhut", 30);
    g_nNguongMatVatPham = IniInt("NguongMatVatPham", 5);
    g_nNguongTien       = (__int64)IniInt("NguongTien", 100000000);
    g_nKiemCrcKhiDoc    = IniInt("KiemCrcKhiDoc", 1);

    // Anh xa (thu muc, ten bang Berkeley DB) -> ten bang MySQL.
    // Kho chinh la ("database","roledb") -> bang `role`.
    // Hai kho gop server ("database1"/"database2") CHUA duoc ho tro o dot 1;
    // ten bang sinh ra se khong ton tai -> open() tra false, MergeDB bao loi
    // ro rang thay vi chay bay.
    if (_stricmp(env_path, "database") == 0)
        im->table = "role";
    else
    {
        im->table = "role_";
        im->table += env_path;
    }
    m_pImpl = im;
    DbLog("ZDBTable(\"%s\",\"%s\") -> MySQL %s:%u/%s bang `%s`",
          env_path, table_name, im->host.c_str(), im->port, im->db.c_str(),
          im->table.c_str());
}

ZDBTable::~ZDBTable()
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (im)
    {
        if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
        delete im;
        m_pImpl = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Ket noi
//////////////////////////////////////////////////////////////////////////////
static bool DoConnect(MyTableImpl *im)
{
    if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
    im->conn = mysql_init(NULL);
    if (!im->conn) { DbLog("mysql_init that bai"); return false; }

    // TUYET DOI khong dat utf8mb4: ten nhan vat va blob deu la byte tho.
    mysql_options(im->conn, MYSQL_SET_CHARSET_NAME, "latin1");
    unsigned int to = 5;
    mysql_options(im->conn, MYSQL_OPT_CONNECT_TIMEOUT, &to);
    // KHONG bat MYSQL_OPT_RECONNECT: noi lai ngam se mat prepared statement
    // va mat ca transaction dang do. Tu noi lai co kiem soat o EnsureConn().

    // Tham so cuoi PHAI la 0. Chi can them CLIENT_MULTI_STATEMENTS la mo toang
    // cua cho "'; DROP TABLE ...; --".
    if (!mysql_real_connect(im->conn, im->host.c_str(), im->user.c_str(),
                            im->pass.c_str(), im->db.c_str(), im->port, NULL, 0))
    {
        DbLog("mysql_real_connect LOI: %s", mysql_error(im->conn));
        mysql_close(im->conn);
        im->conn = NULL;
        return false;
    }
    // Bao loi thay vi cat cut am tham.
    mysql_query(im->conn, "SET SESSION sql_mode='STRICT_ALL_TABLES'");
    mysql_autocommit(im->conn, 1);
    return true;
}

static bool EnsureConn(MyTableImpl *im)
{
    if (im->conn && mysql_ping(im->conn) == 0) return true;
    DbLog("mat ket noi MySQL -- dang noi lai");
    return DoConnect(im);
}

//////////////////////////////////////////////////////////////////////////////
// open / close / commit
//////////////////////////////////////////////////////////////////////////////
bool ZDBTable::open()
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im) return false;
    bStop = false;
    if (!DoConnect(im)) return false;

    // Bang phai co san. KHONG tu tao bang o day: tao bang lang le se giau
    // di truong hop tro nham database va cho ra danh sach nhan vat TRONG.
    char sql[512];
    _snprintf(sql, sizeof(sql) - 1,
              "SELECT COUNT(*) FROM information_schema.TABLES "
              "WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s'",
              im->db.c_str(), im->table.c_str());
    if (mysql_query(im->conn, sql))
    {
        DbLog("kiem bang LOI: %s", mysql_error(im->conn));
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
        DbLog("KHONG tim thay bang `%s`.`%s` -- open() tra false",
              im->db.c_str(), im->table.c_str());
        return false;
    }
    im->opened = true;
    DbLog("open() OK -- bang `%s`.`%s`", im->db.c_str(), im->table.c_str());
    return true;
}

void ZDBTable::close()
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im) return;
    if (im->conn) { mysql_close(im->conn); im->conn = NULL; }
    im->opened = false;
    DbLog("close()");
}

bool ZDBTable::commit()
{
    // MySQL o che do autocommit; moi add()/remove() da tu chot.
    return true;
}

int ZDBTable::addIndex(GetIndexFunc func, bool isUnique)
{
    // Chi muc phu cua Berkeley DB (roledb.0 sinh tu get_account) duoc thay bang
    // chi muc SQL idx_acc tren cot acc_name. Ham get_account KHONG con duoc goi.
    // Goddess chi dang ky DUY NHAT mot chi muc, luon la so 0 -- tuong ung acc_name.
    if (index_number + 1 >= MAX_INDEX) return index_number;
    get_index_funcs[index_number] = func;
    is_index_unique[index_number] = isUnique;
    return index_number++;
}

// deadlock() giu than inline trong DBTable.h (bao boi #ifndef ROLEDB_MYSQL).

void ZDBTable::removeLog()
{
    // Khong con tep log Berkeley DB. Binlog do MySQL tu xoay (expire_logs_days).
}

//////////////////////////////////////////////////////////////////////////////
// Con tro
//////////////////////////////////////////////////////////////////////////////
static ZCursor *MakeCursor(MyCursorImpl *ci)
{
    // Nap ban ghi tai vi tri hien tai vao cac truong ma ben goi doc truc tiep.
    if (ci->pos >= ci->rows.size()) return NULL;
    const MyRow &r = ci->rows[ci->pos];

    ZCursor *c = new ZCursor;
    memset(c, 0, sizeof(ZCursor));
    c->pImpl = ci;
    c->index = (int)ci->pos;
    c->dbcp = NULL;
    c->data = DupMalloc(r.blob.data(), r.blob.size());
    c->size = (int)r.blob.size();
    if (ci->mode == CM_SCAN)
    {
        c->bTravel = true;
        c->key = DupMalloc(r.key.data(), r.key.size());
        c->key_size = (int)r.key.size();
    }
    else
    {
        c->bTravel = false;
        c->key = NULL;
        c->key_size = 0;
    }
    return c;
}

void ZDBTable::closeCursor(ZCursor *cursor)
{
    if (!cursor) return;
    if (cursor->bTravel) free(cursor->key);
    free(cursor->data);
    delete (MyCursorImpl *)cursor->pImpl;
    delete cursor;
}

// Nap mot trang khi duyet toan bang (keyset pagination, khong dung OFFSET)
static bool LoadScanPage(MyTableImpl *im, MyCursorImpl *ci)
{
    ci->rows.clear();
    ci->pos = 0;
    if (!EnsureConn(im)) return false;

    char sql[512];
    _snprintf(sql, sizeof(sql) - 1,
              "SELECT role_name, role_blob FROM `%s` WHERE role_name > ? "
              "ORDER BY role_name LIMIT %d", im->table.c_str(), SCAN_PAGE_ROWS);

    Stmt st(im->conn);
    if (!st.Prepare(sql)) return false;

    MYSQL_BIND p;
    unsigned long plen = (unsigned long)ci->seek.size();
    BindBlob(p, ci->seek.data(), &plen);
    if (mysql_stmt_bind_param(st.Get(), &p) || mysql_stmt_execute(st.Get()))
    {
        DbLog("LoadScanPage LOI: %s", mysql_stmt_error(st.Get()));
        return false;
    }

    MYSQL_BIND res[2];
    unsigned long l0 = 0, l1 = 0;
    my_bool n0 = 0, n1 = 0;
    memset(res, 0, sizeof(res));
    res[0].buffer_type = MYSQL_TYPE_BLOB; res[0].buffer = NULL;
    res[0].buffer_length = 0; res[0].length = &l0; res[0].is_null = &n0;
    res[1].buffer_type = MYSQL_TYPE_BLOB; res[1].buffer = NULL;
    res[1].buffer_length = 0; res[1].length = &l1; res[1].is_null = &n1;
    if (mysql_stmt_bind_result(st.Get(), res) || mysql_stmt_store_result(st.Get()))
    {
        DbLog("LoadScanPage bind/store LOI: %s", mysql_stmt_error(st.Get()));
        return false;
    }

    int rc;
    while ((rc = mysql_stmt_fetch(st.Get())) == 0 || rc == MYSQL_DATA_TRUNCATED)
    {
        MyRow row;
        if (!FetchBlobColumn(st.Get(), 0, &res[0], l0, row.key))  return false;
        if (!FetchBlobColumn(st.Get(), 1, &res[1], l1, row.blob)) return false;
        // Berkeley DB luu khoa KEM byte NUL (strlen+1). MySQL luu ten KHONG NUL.
        // Ben goi (DBBackup.cpp:414) mong doi dung quy uoc cu -> noi lai NUL.
        row.key.push_back('\0');
        ci->seek = row.key.substr(0, row.key.size() - 1);
        ci->rows.push_back(row);
    }
    if (rc != MYSQL_NO_DATA)
    {
        DbLog("LoadScanPage fetch LOI: %s", mysql_stmt_error(st.Get()));
        return false;
    }
    return true;
}

ZCursor *ZDBTable::_search(bool bKey, const char *key_ptr, int key_size, int index)
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im || !im->opened) return NULL;
    if (bKey)
    {
        // search_key/next_key khong duoc goi o bat cu dau trong Goddess.
        DbLog("search_key() duoc goi -- ban MySQL chua cai dat, tra NULL");
        return NULL;
    }
    if (!EnsureConn(im)) return NULL;

    MyCursorImpl *ci = new MyCursorImpl();
    ci->owner = this;
    ci->pos = 0;

    if (!key_ptr || !key_size)
    {
        // first(): duyet toan bang
        ci->mode = CM_SCAN;
        ci->seek = "";
        if (!LoadScanPage(im, ci) || ci->rows.empty()) { delete ci; return NULL; }
        ZCursor *c = MakeCursor(ci);
        if (!c) { delete ci; return NULL; }
        return c;
    }

    // Khoa tu ben goi CO byte NUL cuoi (strlen+1). Cot MySQL luu KHONG NUL.
    std::string k(key_ptr, key_size);
    while (!k.empty() && k[k.size() - 1] == '\0') k.erase(k.size() - 1);

    char sql[512];
    if (index < 0)
    {
        ci->mode = CM_ONE;
        // Lay them crc_ok: chi bao dong khi ban ghi TUNG co CRC dung ma gio lai sai,
        // tuc hong SAU khi ghi. Cac ban ghi cu von da sai CRC san thi bo qua.
        _snprintf(sql, sizeof(sql) - 1,
                  "SELECT role_name, role_blob, crc_ok FROM `%s` WHERE role_name=?",
                  im->table.c_str());
    }
    else
    {
        // index 0 == chi muc theo tai khoan (thay cho roledb.0 / get_account)
        ci->mode = CM_ACCOUNT;
        _snprintf(sql, sizeof(sql) - 1,
                  "SELECT role_name, role_blob FROM `%s` WHERE acc_name=? "
                  "ORDER BY created_at, role_name", im->table.c_str());
    }

    Stmt st(im->conn);
    if (!st.Prepare(sql)) { delete ci; return NULL; }

    MYSQL_BIND p;
    unsigned long plen = (unsigned long)k.size();
    BindBlob(p, k.data(), &plen);
    if (mysql_stmt_bind_param(st.Get(), &p) || mysql_stmt_execute(st.Get()))
    {
        DbLog("_search LOI: %s", mysql_stmt_error(st.Get()));
        delete ci;
        return NULL;
    }

    bool co_cot_crc = (ci->mode == CM_ONE);
    MYSQL_BIND res[3];
    unsigned long l0 = 0, l1 = 0;
    my_bool n0 = 0, n1 = 0, n2 = 0;
    __int64 v_crcok = 0;
    memset(res, 0, sizeof(res));
    res[0].buffer_type = MYSQL_TYPE_BLOB; res[0].length = &l0; res[0].is_null = &n0;
    res[1].buffer_type = MYSQL_TYPE_BLOB; res[1].length = &l1; res[1].is_null = &n1;
    res[2].buffer_type = MYSQL_TYPE_LONGLONG; res[2].buffer = &v_crcok; res[2].is_null = &n2;
    if (mysql_stmt_bind_result(st.Get(), res) || mysql_stmt_store_result(st.Get()))
    {
        DbLog("_search bind/store LOI: %s", mysql_stmt_error(st.Get()));
        delete ci;
        return NULL;
    }

    int rc;
    while ((rc = mysql_stmt_fetch(st.Get())) == 0 || rc == MYSQL_DATA_TRUNCATED)
    {
        MyRow row;
        if (!FetchBlobColumn(st.Get(), 0, &res[0], l0, row.key))  { delete ci; return NULL; }
        if (!FetchBlobColumn(st.Get(), 1, &res[1], l1, row.blob)) { delete ci; return NULL; }

        // ---- CHONG LOI DU LIEU (3): kiem CRC ngay luc doc ----
        // Chi bao dong khi cot crc_ok = 1 (ban ghi TUNG dung) ma byte doc ra lai sai
        // => hong SAU khi ghi. Nho vay khong keu oan voi ban ghi cu von sai CRC san.
        if (co_cot_crc && g_nKiemCrcKhiDoc && v_crcok == 1)
        {
            RoleDerived dd;
            if (!ParseRole(row.blob.data(), (int)row.blob.size(), &dd))
            {
                char ct[256];
                _snprintf(ct, sizeof(ct) - 1, "blob %d byte doc ra KHONG giai ma duoc",
                          (int)row.blob.size());
                GhiBaoDong(im, k, "blob_hong_khi_doc", ct, 0, 0);
            }
            else if (dd.has_crc && !dd.crc_ok)
            {
                char ct[256];
                _snprintf(ct, sizeof(ct) - 1,
                          "CRC luu %08X nhung tinh lai ra khac -- ban ghi TUNG dung CRC",
                          dd.crc32_stored);
                GhiBaoDong(im, k, "crc_sai_khi_doc", ct, 0, 0);
            }
        }

        row.key.push_back('\0');
        ci->rows.push_back(row);
        if (ci->mode == CM_ONE) break;
    }

    if (ci->rows.empty()) { delete ci; return NULL; }
    ZCursor *c = MakeCursor(ci);
    if (!c) { delete ci; return NULL; }
    return c;
}

bool ZDBTable::_next(bool bKey, ZCursor *cursor)
{
    if (!cursor || !cursor->pImpl) return false;
    MyCursorImpl *ci = (MyCursorImpl *)cursor->pImpl;
    MyTableImpl  *im = (MyTableImpl *)m_pImpl;

    // Giai phong vung cua ban ghi hien tai -- dung thu tu nhu ban Berkeley DB
    free(cursor->data);
    cursor->data = NULL;
    if (cursor->bTravel) { free(cursor->key); cursor->key = NULL; }

    ci->pos++;
    if (ci->pos >= ci->rows.size())
    {
        if (ci->mode == CM_SCAN && im && LoadScanPage(im, ci) && !ci->rows.empty())
        {
            // sang trang moi, pos da ve 0
        }
        else
        {
            // Het du lieu: con tro TU HUY, dung quy uoc cua ban Berkeley DB.
            delete ci;
            cursor->pImpl = NULL;
            delete cursor;
            return false;
        }
    }

    const MyRow &r = ci->rows[ci->pos];
    cursor->index = (int)ci->pos;
    cursor->data = DupMalloc(r.blob.data(), r.blob.size());
    cursor->size = (int)r.blob.size();
    if (cursor->bTravel)
    {
        cursor->key = DupMalloc(r.key.data(), r.key.size());
        cursor->key_size = (int)r.key.size();
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// add -- ghi mot ban ghi (tao moi hoac ghi de)
//////////////////////////////////////////////////////////////////////////////
static void GhiThatBai(MyTableImpl *im, const std::string &name, const char *why,
                       const char *data, int size)
{
    if (!im || !im->conn) return;
    char sql[256];
    _snprintf(sql, sizeof(sql) - 1,
              "INSERT INTO role_save_fail (role_name,reason,data_len,role_blob,at) "
              "VALUES (?,?,?,?,NOW())");
    Stmt st(im->conn);
    if (!st.Prepare(sql)) return;
    MYSQL_BIND p[4];
    unsigned long l0 = (unsigned long)name.size();
    unsigned long l1 = (unsigned long)strlen(why);
    unsigned long l3 = (unsigned long)(size > 0 ? size : 0);
    __int64 dl = size;
    memset(p, 0, sizeof(p));
    BindBlob(p[0], name.data(), &l0);
    p[1].buffer_type = MYSQL_TYPE_STRING; p[1].buffer = (void *)why;
    p[1].buffer_length = l1; p[1].length = &l1;
    BindI64(p[2], &dl);
    BindBlob(p[3], data ? data : "", &l3);
    if (!mysql_stmt_bind_param(st.Get(), p))
        mysql_stmt_execute(st.Get());
}

//////////////////////////////////////////////////////////////////////////////
// Trang thai ban ghi CU (chi cac cot re, KHONG keo blob qua mang)
//////////////////////////////////////////////////////////////////////////////
struct BanGhiCu
{
    bool          co;
    unsigned __int64 ver;
    __int64       money, save_money;
    int           level, n_item;
    int           crc_ok;
    bool          can_chup_dinh_ky;
};

static bool DocBanGhiCu(MyTableImpl *im, const std::string &k, BanGhiCu *o)
{
    memset(o, 0, sizeof(*o));
    char sql[512];
    _snprintf(sql, sizeof(sql) - 1,
              "SELECT ver,money,save_money,level,n_item,crc_ok,"
              "(hist_at IS NULL OR hist_at < NOW() - INTERVAL %d MINUTE) "
              "FROM `%s` WHERE role_name=?",
              g_nLichSuPhut > 0 ? g_nLichSuPhut : 100000, im->table.c_str());
    Stmt st(im->conn);
    if (!st.Prepare(sql)) return false;
    MYSQL_BIND p;
    unsigned long l = (unsigned long)k.size();
    BindBlob(p, k.data(), &l);
    if (mysql_stmt_bind_param(st.Get(), &p) || mysql_stmt_execute(st.Get())) return false;

    MYSQL_BIND r[7];
    __int64 v[7] = {0, 0, 0, 0, 0, 0, 0};
    my_bool isnull[7] = {0, 0, 0, 0, 0, 0, 0};
    memset(r, 0, sizeof(r));
    for (int i = 0; i < 7; i++)
    {
        r[i].buffer_type = MYSQL_TYPE_LONGLONG;
        r[i].buffer = &v[i];
        r[i].is_null = &isnull[i];
    }
    if (mysql_stmt_bind_result(st.Get(), r) || mysql_stmt_store_result(st.Get())) return false;
    if (mysql_stmt_fetch(st.Get()) != 0) return true;      // chua co ban ghi -> o->co = false

    o->co = true;
    o->ver              = (unsigned __int64)v[0];
    o->money            = v[1];
    o->save_money       = v[2];
    o->level            = (int)v[3];
    o->n_item           = (int)v[4];
    o->crc_ok           = (int)v[5];
    o->can_chup_dinh_ky = (v[6] != 0);
    return true;
}

// Chup anh ban ghi CU vao role_history. Blob duoc sao chep NGAY TRONG MAY CHU,
// khong di qua duong mang. Tra ve id cua ban chup, hoac 0 neu that bai.
static __int64 ChupAnh(MyTableImpl *im, const std::string &k, const char *ly_do)
{
    char sql[1024];
    _snprintf(sql, sizeof(sql) - 1,
        "INSERT INTO role_history "
        "(role_name,ver,data_len,role_blob,crc32,level,money,save_money,n_item,reason,saved_at) "
        "SELECT role_name,ver,data_len,role_blob,crc32,level,money,save_money,n_item,?,NOW() "
        "FROM `%s` WHERE role_name=?", im->table.c_str());
    Stmt st(im->conn);
    if (!st.Prepare(sql)) return 0;
    MYSQL_BIND p[2];
    memset(p, 0, sizeof(p));
    unsigned long l0 = (unsigned long)strlen(ly_do);
    unsigned long l1 = (unsigned long)k.size();
    p[0].buffer_type = MYSQL_TYPE_STRING;
    p[0].buffer = (void *)ly_do;
    p[0].buffer_length = l0;
    p[0].length = &l0;
    BindBlob(p[1], k.data(), &l1);
    if (mysql_stmt_bind_param(st.Get(), p) || mysql_stmt_execute(st.Get()))
    {
        DbLog("ChupAnh(\"%s\") LOI: %s", k.c_str(), mysql_stmt_error(st.Get()));
        return 0;
    }
    return (__int64)mysql_stmt_insert_id(st.Get());
}

static void GhiBaoDong(MyTableImpl *im, const std::string &k, const char *kind,
                       const char *chi_tiet, unsigned __int64 old_ver, __int64 history_id)
{
    char sql[512];
    _snprintf(sql, sizeof(sql) - 1,
        "INSERT INTO role_anomaly (role_name,kind,chi_tiet,old_ver,history_id,at) "
        "VALUES (?,?,?,?,%s,NOW())", history_id ? "?" : "NULL");
    Stmt st(im->conn);
    if (!st.Prepare(sql)) return;
    MYSQL_BIND p[5];
    memset(p, 0, sizeof(p));
    unsigned long l0 = (unsigned long)k.size();
    unsigned long l1 = (unsigned long)strlen(kind);
    unsigned long l2 = (unsigned long)strlen(chi_tiet);
    __int64 v3 = (__int64)old_ver;
    __int64 v4 = history_id;
    BindBlob(p[0], k.data(), &l0);
    p[1].buffer_type = MYSQL_TYPE_STRING; p[1].buffer = (void *)kind;
    p[1].buffer_length = l1; p[1].length = &l1;
    p[2].buffer_type = MYSQL_TYPE_STRING; p[2].buffer = (void *)chi_tiet;
    p[2].buffer_length = l2; p[2].length = &l2;
    BindI64(p[3], &v3);
    if (history_id) BindI64(p[4], &v4);
    if (!mysql_stmt_bind_param(st.Get(), p))
        mysql_stmt_execute(st.Get());
    DbLog("BAO DONG [%s] \"%s\": %s (history_id=%I64d)", kind, k.c_str(), chi_tiet, history_id);
}

bool ZDBTable::add(const char *key_ptr, int key_size, const char *data_ptr, int data_size)
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im || !im->opened || bStop) return false;

    std::string k(key_ptr ? key_ptr : "", key_size > 0 ? key_size : 0);
    while (!k.empty() && k[k.size() - 1] == '\0') k.erase(k.size() - 1);
    if (k.empty() || k.size() > 32)
    {
        DbLog("add() TU CHOI: khoa dai %d byte", (int)k.size());
        return false;
    }

    // --- V2: chan CA HAI DAU. Ban cu (IDBRoleServer.cpp:478) chi chan tren,
    // nen dwDataLen = 0..3 hoac AM lot qua roi lam `dwDataLen - 4` tran nguoc.
    if (data_size < MIN_ROLE_DATALEN || data_size >= MAX_ROLE_DATALEN)
    {
        DbLog("add() TU CHOI \"%s\": data_size=%d ngoai [%d, %d)",
              k.c_str(), data_size, MIN_ROLE_DATALEN, MAX_ROLE_DATALEN);
        if (EnsureConn(im))
            GhiThatBai(im, k, "data_size ngoai khoang hop le", data_ptr, data_size);
        return false;
    }

    RoleDerived d;
    if (!ParseRole(data_ptr, data_size, &d))
    {
        DbLog("add() TU CHOI \"%s\": blob khong giai ma duoc", k.c_str());
        if (EnsureConn(im))
            GhiThatBai(im, k, "blob khong giai ma duoc", data_ptr, data_size);
        return false;
    }

    if (!EnsureConn(im)) return false;

    // ---- CHONG LOI DU LIEU: doc trang thai CU (chi cot re, khong keo blob) ----
    BanGhiCu cu;
    DocBanGhiCu(im, k, &cu);

    const char *kind = NULL;
    char chi_tiet[256] = {0};
    if (cu.co)
    {
        int tut = cu.n_item - (int)d.n_item;
        __int64 tien_cu  = cu.money + cu.save_money;
        __int64 tien_moi = d.money + d.save_money;
        __int64 nhay = tien_moi - tien_cu;
        if (nhay < 0) nhay = -nhay;

        if (g_nNguongMatVatPham > 0 && tut >= g_nNguongMatVatPham)
        {
            kind = "mat_vat_pham";
            _snprintf(chi_tiet, sizeof(chi_tiet) - 1,
                      "so vat pham %d -> %d (tut %d)", cu.n_item, (int)d.n_item, tut);
        }
        else if (g_nNguongTien > 0 && nhay >= g_nNguongTien)
        {
            kind = "tien_nhay";
            _snprintf(chi_tiet, sizeof(chi_tiet) - 1,
                      "tien %I64d -> %I64d (lech %I64d)", tien_cu, tien_moi, tien_moi - tien_cu);
        }
        else if (d.level < cu.level)
        {
            kind = "cap_tut";
            _snprintf(chi_tiet, sizeof(chi_tiet) - 1, "cap %d -> %d", cu.level, d.level);
        }
    }

    // Chup anh khi CO bao dong, hoac khi den ky chup dinh ky.
    bool can_chup = cu.co && (kind != NULL || (g_nLichSuPhut > 0 && cu.can_chup_dinh_ky));

    // Ca ba viec (chup anh + bao dong + ghi de) nam trong MOT giao dich:
    // khong the co canh "da chup ma chua ghi" hay "da ghi ma mat ban chup".
    bool co_giao_dich = can_chup;
    if (co_giao_dich && mysql_query(im->conn, "START TRANSACTION"))
    {
        DbLog("add() START TRANSACTION LOI: %s", mysql_error(im->conn));
        co_giao_dich = false;
        can_chup = false;
    }
    __int64 hist_id = 0;
    if (can_chup)
    {
        hist_id = ChupAnh(im, k, kind ? "bat thuong" : "dinh ky");
        if (kind)
            GhiBaoDong(im, k, kind, chi_tiet, cu.ver, hist_id);
    }

    char sql[2048];
    _snprintf(sql, sizeof(sql) - 1,
        "INSERT INTO `%s` (role_name,acc_name,data_len,role_blob,crc32,crc_ok,has_crc,"
        "fmt_ver,sex,first_sect,sect,series,level,fight_exp,lead_level,money,save_money,"
        "pk_value,kill_num,tong_id,use_revive,revival_id,enter_game_id,world_stat,"
        "sect_stat,n_item,n_task,n_fight_skill,n_state_skill,ver,created_at) "
        "VALUES (?,?,?,?,?,?,?,1,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,0,NOW()) "
        "ON DUPLICATE KEY UPDATE "
        "acc_name=VALUES(acc_name),data_len=VALUES(data_len),role_blob=VALUES(role_blob),"
        "crc32=VALUES(crc32),crc_ok=VALUES(crc_ok),has_crc=VALUES(has_crc),"
        "sex=VALUES(sex),first_sect=VALUES(first_sect),sect=VALUES(sect),"
        "series=VALUES(series),level=VALUES(level),fight_exp=VALUES(fight_exp),"
        "lead_level=VALUES(lead_level),money=VALUES(money),save_money=VALUES(save_money),"
        "pk_value=VALUES(pk_value),kill_num=VALUES(kill_num),tong_id=VALUES(tong_id),"
        "use_revive=VALUES(use_revive),revival_id=VALUES(revival_id),"
        "enter_game_id=VALUES(enter_game_id),world_stat=VALUES(world_stat),"
        "sect_stat=VALUES(sect_stat),n_item=VALUES(n_item),n_task=VALUES(n_task),"
        "n_fight_skill=VALUES(n_fight_skill),n_state_skill=VALUES(n_state_skill),"
        "ver=ver+1%s", im->table.c_str(), can_chup ? ",hist_at=NOW()" : "");

    Stmt st(im->conn);
    if (!st.Prepare(sql)) return false;

    std::string acc(d.acc_name);
    if (acc.size() > 32) acc.resize(32);

    MYSQL_BIND p[29];
    memset(p, 0, sizeof(p));
    unsigned long lname = (unsigned long)k.size();
    unsigned long lacc  = (unsigned long)acc.size();
    unsigned long lblob = (unsigned long)data_size;
    __int64 v_datalen = d.data_len, v_crc = (__int64)(unsigned int)d.crc32_stored;
    __int64 v_crcok = d.crc_ok ? 1 : 0, v_hascrc = d.has_crc ? 1 : 0;
    __int64 v_sex = d.sex, v_fs = d.first_sect, v_sect = d.sect, v_series = d.series;
    __int64 v_level = d.level, v_lead = d.lead_level;
    __int64 v_money = d.money, v_save = d.save_money;
    __int64 v_pk = d.pk_value, v_kill = d.kill_num, v_tong = (__int64)d.tong_id;
    __int64 v_ur = d.use_revive, v_rev = d.revival_id, v_eg = d.enter_game_id;
    __int64 v_ws = d.world_stat, v_ss = d.sect_stat;
    __int64 v_ni = d.n_item, v_nt = d.n_task, v_nf = d.n_fight_skill, v_ns = d.n_state_skill;
    double  v_exp = d.fight_exp;

    int i = 0;
    BindBlob(p[i++], k.data(), &lname);
    BindBlob(p[i++], acc.data(), &lacc);
    BindI64 (p[i++], &v_datalen);
    BindBlob(p[i++], data_ptr, &lblob);
    BindI64 (p[i++], &v_crc);
    BindI64 (p[i++], &v_crcok);
    BindI64 (p[i++], &v_hascrc);
    BindI64 (p[i++], &v_sex);
    BindI64 (p[i++], &v_fs);
    BindI64 (p[i++], &v_sect);
    BindI64 (p[i++], &v_series);
    BindI64 (p[i++], &v_level);
    BindDbl (p[i++], &v_exp);
    BindI64 (p[i++], &v_lead);
    BindI64 (p[i++], &v_money);
    BindI64 (p[i++], &v_save);
    BindI64 (p[i++], &v_pk);
    BindI64 (p[i++], &v_kill);
    BindI64 (p[i++], &v_tong);
    BindI64 (p[i++], &v_ur);
    BindI64 (p[i++], &v_rev);
    BindI64 (p[i++], &v_eg);
    BindI64 (p[i++], &v_ws);
    BindI64 (p[i++], &v_ss);
    BindI64 (p[i++], &v_ni);
    BindI64 (p[i++], &v_nt);
    BindI64 (p[i++], &v_nf);
    BindI64 (p[i++], &v_ns);

    if (mysql_stmt_bind_param(st.Get(), p))
    {
        DbLog("add() bind LOI: %s", mysql_stmt_error(st.Get()));
        return false;
    }
    if (mysql_stmt_execute(st.Get()))
    {
        unsigned err = mysql_stmt_errno(st.Get());
        DbLog("add(\"%s\") LOI %u: %s", k.c_str(), err, mysql_stmt_error(st.Get()));
        // --- V4: mat ket noi -> noi lai va thu DUNG MOT lan nua
        if (err == 2006 || err == 2013)
        {
            st.Close();
            if (DoConnect(im) && st.Prepare(sql) &&
                !mysql_stmt_bind_param(st.Get(), p) && !mysql_stmt_execute(st.Get()))
            {
                DbLog("add(\"%s\") thu lai sau khi noi lai: THANH CONG", k.c_str());
                return true;
            }
        }
        // --- V3: that bai KHONG duoc im lang
        if (co_giao_dich) mysql_query(im->conn, "ROLLBACK");
        if (EnsureConn(im))
            GhiThatBai(im, k, "mysql_stmt_execute that bai", data_ptr, data_size);
        return false;
    }
    if (co_giao_dich && mysql_query(im->conn, "COMMIT"))
    {
        DbLog("add(\"%s\") COMMIT LOI: %s", k.c_str(), mysql_error(im->conn));
        mysql_query(im->conn, "ROLLBACK");
        GhiThatBai(im, k, "COMMIT that bai", data_ptr, data_size);
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// quarantine -- cat giu mot goi ghi BI TU CHOI de con cuu ho.
// Goi tu CClientNode::_SaveRoleInfo qua QuarantineRoleInfo() khi goi luu bi tu
// choi vi ket noi khong giu khoa nhan vat. Truoc day truong hop nay bi VUT BO
// IM LANG -- nguy hiem nhat la luot luu CUOI luc dang xuat / doi may chu.
//////////////////////////////////////////////////////////////////////////////
bool ZDBTable::quarantine(const char *key_ptr, int key_size,
                          const char *data_ptr, int data_size, const char *why)
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im || !im->opened) return false;
    if (!EnsureConn(im)) return false;

    std::string k(key_ptr ? key_ptr : "", key_size > 0 ? key_size : 0);
    while (!k.empty() && k[k.size() - 1] == '\0') k.erase(k.size() - 1);
    if (k.empty() || k.size() > 32) return false;

    // Kep kich thuoc goi giu lai: goi hong co the mang do dai phi ly.
    int giu = data_size;
    if (giu < 0) giu = 0;
    if (giu > MAX_ROLE_DATALEN) giu = MAX_ROLE_DATALEN;

    GhiThatBai(im, k, why ? why : "khong ro ly do", data_ptr, giu);
    DbLog("quarantine(\"%s\") ly do=%s data_size=%d (giu %d byte)",
          k.c_str(), why ? why : "?", data_size, giu);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// remove -- V1: KHONG xoa trang. Chep sang role_delete_log truoc, trong 1 giao dich.
//////////////////////////////////////////////////////////////////////////////
bool ZDBTable::remove(const char *key_ptr, int key_size, int index)
{
    MyTableImpl *im = (MyTableImpl *)m_pImpl;
    if (!im || !im->opened) return false;
    if (!EnsureConn(im)) return false;

    std::string k(key_ptr ? key_ptr : "", key_size > 0 ? key_size : 0);
    while (!k.empty() && k[k.size() - 1] == '\0') k.erase(k.size() - 1);
    if (k.empty()) return false;

    if (mysql_query(im->conn, "START TRANSACTION"))
    {
        DbLog("remove() START TRANSACTION LOI: %s", mysql_error(im->conn));
        return false;
    }

    bool ok = true;
    char sql[1024];

    _snprintf(sql, sizeof(sql) - 1,
        "INSERT INTO role_delete_log (role_name,acc_name,data_len,role_blob,deleted_at) "
        "SELECT role_name,acc_name,data_len,role_blob,NOW() FROM `%s` WHERE role_name=?",
        im->table.c_str());
    {
        Stmt st(im->conn);
        MYSQL_BIND p;
        unsigned long l = (unsigned long)k.size();
        BindBlob(p, k.data(), &l);
        if (!st.Prepare(sql) || mysql_stmt_bind_param(st.Get(), &p) ||
            mysql_stmt_execute(st.Get()))
        {
            DbLog("remove() luu nhat ky xoa LOI: %s",
                  st.Get() ? mysql_stmt_error(st.Get()) : mysql_error(im->conn));
            ok = false;
        }
    }

    if (ok)
    {
        _snprintf(sql, sizeof(sql) - 1, "DELETE FROM `%s` WHERE role_name=?",
                  im->table.c_str());
        Stmt st(im->conn);
        MYSQL_BIND p;
        unsigned long l = (unsigned long)k.size();
        BindBlob(p, k.data(), &l);
        if (!st.Prepare(sql) || mysql_stmt_bind_param(st.Get(), &p) ||
            mysql_stmt_execute(st.Get()))
        {
            DbLog("remove() DELETE LOI: %s",
                  st.Get() ? mysql_stmt_error(st.Get()) : mysql_error(im->conn));
            ok = false;
        }
    }

    if (ok)
    {
        if (mysql_query(im->conn, "COMMIT"))
        {
            DbLog("remove() COMMIT LOI: %s", mysql_error(im->conn));
            ok = false;
        }
        else
            DbLog("remove(\"%s\") xong -- ban goc da luu vao role_delete_log", k.c_str());
    }
    if (!ok)
        mysql_query(im->conn, "ROLLBACK");
    return ok;
}
