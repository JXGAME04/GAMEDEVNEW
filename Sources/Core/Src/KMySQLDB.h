//////////////////////////////////////////////////////////////////////////////
// KMySQLDB.h -- Tang truy cap MySQL cho CoreServer (x64).
//
// Dung cho toan bo du lieu tinh nang trong game dang nam o tep phang: giftcode,
// tien Xu song bac, Cong Thanh Chien / xep hang / lien minh, bang xep hang,
// cac tep .lua may chu tu ghi de, va NHAT KY nguoi choi.
//
// ---------------------------------------------------------------------------
// DIEU QUAN TRONG NHAT: KHONG DUOC CHAN VONG LAP GAME
//
// CoreServer chay vong lap the gioi 18 khung/giay trong MOT luong. Neu goi MySQL
// dong bo ngay trong vong lap do, mot cau lenh cham la GIAT CA MAY CHU.
// Vi vay lop nay co HAI duong:
//
//   Post()  -- BAT DONG BO. Day cau lenh vao hang doi CO TRAN, mot luong nen
//              rieng (co ket noi rieng) se ghi. Vong lap game chi ton vai tram
//              nano giay. DUNG CHO: nhat ky, dem, cap nhat trang thai khong
//              can doc lai ngay.
//              Hang doi DAY thi BO GOI va DEM lai -- KHONG BAO GIO chan vong lap
//              game. Mat mot dong nhat ky con hon giat ca may chu.
//
//   Exec()/Query() -- DONG BO. Chi dung khi (a) luc khoi dong/tat may, hoac
//              (b) thao tac TIEN bat buoc phai biet ket qua ngay.
//              Khoi luong nhung cho nay rat nho (vai chuc luot/ngay).
//
// ---------------------------------------------------------------------------
// AN TOAN
//  * Moi tham so deu di qua prepared statement co bind nhi phan. TUYET DOI khong
//    noi chuoi SQL bang sprintf: ten nhan vat la byte TCVN3 tho, co the chua
//    0x27 (nhay don) va 0x5C (gach cheo nguoc) o vi tri bat ky.
//  * Charset ket noi la latin1 (byte tho). TUYET DOI khong utf8mb4.
//  * Tham so cuoi cua mysql_real_connect PHAI la 0 -- them CLIENT_MULTI_STATEMENTS
//    la mo cua cho "'; DROP TABLE ...; --".
//  * Mat ket noi: tu noi lai co kiem soat, khong dung MYSQL_OPT_RECONNECT (no lam
//    mat prepared statement va mat ca giao dich dang do trong im lang).
//////////////////////////////////////////////////////////////////////////////

#ifndef KMYSQLDB_H
#define KMYSQLDB_H

#ifdef _SERVER

#include <windows.h>
#include <string.h>   // strlen dung trong KDBParam::S()

// Kieu tham so truyen vao cau lenh
enum KDBParamType
{
    KDBP_NULL = 0,
    KDBP_I64,        // so nguyen (dung cho ca int/BOOL/DWORD)
    KDBP_DOUBLE,
    KDBP_BLOB,       // byte tho: ten nhan vat, blob nhi phan
    KDBP_STR         // chuoi (van truyen theo do dai, khong dua vao NUL)
};

struct KDBParam
{
    KDBParamType type;
    __int64      i64;
    double       dbl;
    const char  *buf;
    int          len;

    static KDBParam Null()                 { KDBParam p; p.type = KDBP_NULL; p.i64 = 0; p.dbl = 0; p.buf = 0; p.len = 0; return p; }
    static KDBParam I(__int64 v)           { KDBParam p = Null(); p.type = KDBP_I64;    p.i64 = v; return p; }
    static KDBParam D(double v)            { KDBParam p = Null(); p.type = KDBP_DOUBLE; p.dbl = v; return p; }
    static KDBParam B(const void *b, int n){ KDBParam p = Null(); p.type = KDBP_BLOB;   p.buf = (const char *)b; p.len = n; return p; }
    static KDBParam S(const char *s)       { KDBParam p = Null(); p.type = KDBP_STR;    p.buf = s; p.len = s ? (int)strlen(s) : 0; return p; }
};

// Mot dong ket qua tra ve tu Query(). Con tro chi song trong pham vi ham gia lai.
struct KDBRow
{
    int          nCol;
    const char **pVal;      // du lieu tho (co the chua byte 0)
    const int   *pLen;
};

// Ham goi lai cho tung dong. Tra false de dung som.
typedef bool (*KDBRowFunc)(const KDBRow &row, void *pParam);

class KMySQLDB
{
public:
    KMySQLDB();
    ~KMySQLDB();

    // Doc DataBase.ini section [gamedb] roi ket noi. Tra FALSE neu that bai --
    // ben goi PHAI xu ly, khong duoc coi nhu da ket noi.
    bool Init(const char *szIniPath = ".\\DataBase.ini",
              const char *szSection = "gamedb");
    void Close();
    bool IsReady() const { return m_bReady; }

    // ---- DONG BO: chi dung luc khoi dong/tat may, hoac thao tac TIEN ----
    bool Exec(const char *szSql, const KDBParam *pParam = 0, int nParam = 0,
              __int64 *pnAffected = 0, __int64 *pnInsertId = 0);
    bool Query(const char *szSql, const KDBParam *pParam, int nParam,
               KDBRowFunc pfnRow, void *pUserParam);

    // Giao dich dong bo (dung cho TIEN)
    bool Begin();
    bool Commit();
    bool Rollback();

    // ---- BAT DONG BO: KHONG BAO GIO chan vong lap game ----
    // Sao chep cau lenh + tham so vao hang doi roi tra ve ngay.
    // Tra false neu hang doi DAY (goi bi bo) hoac chua san sang.
    bool Post(const char *szSql, const KDBParam *pParam = 0, int nParam = 0);

    // So lieu de theo doi
    int  QueueSize() const   { return m_nQueue; }
    int  DroppedCount() const{ return m_nDropped; }
    int  ErrorCount() const  { return m_nErrors; }

    // Goi moi vong game (re): chi de ghi nhat ky canh bao khi hang doi phinh.
    void Tick();

    // Khai bao truoc, dinh nghia trong .cpp. De o public vi mot vai ham tro giup
    // trong .cpp (KDBConnect) can dung toi kieu nay.
    struct Impl;

private:
    Impl *m_pImpl;
    bool  m_bReady;
    volatile long m_nQueue;
    volatile long m_nDropped;
    volatile long m_nErrors;

    static unsigned __stdcall _WorkerThunk(void *p);
    void  _Worker();

    KMySQLDB(const KMySQLDB &);
    KMySQLDB &operator=(const KMySQLDB &);
};

// The hien dung chung cho toan CoreServer.
extern KMySQLDB g_MySQLDB;

// Ghi nhat ky rieng cua tang nay (mysql_core.log). Dung khi chua/khong ket noi duoc.
void KDBLog(const char *fmt, ...);

#endif // _SERVER
#endif // KMYSQLDB_H
