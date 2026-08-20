//////////////////////////////////////////////////////////////////////////////
// test_kmysqldb.cpp -- Kiem thu tang KMySQLDB (ban x64 cho CoreServer).
//
// Kiem dung nhung dieu de vo nhat khi dua "toan bo len MySQL":
//   1. Byte nguy hiem: gia tri chua 0x00, 0x27 (nhay don), 0x5C (gach cheo nguoc)
//      va byte TCVN3 >= 0x80 phai di ve NGUYEN VEN. Day la cho ma sprintf noi
//      chuoi SQL se hong hoac mo cua SQL injection.
//   2. Duong ghi BAT DONG BO: day 5.000 dong vao hang doi phai tra ve gan nhu
//      tuc thi (khong duoc chan vong lap game), va Close() phai XA HET.
//   3. Hang doi DAY thi bo goi va DEM lai, KHONG duoc chan.
//   4. Doc lai bang Query() va so tung byte.
//////////////////////////////////////////////////////////////////////////////

#define _SERVER
#include "KMySQLDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_pass = 0, g_fail = 0;
static void OK(bool c, const char *fmt, ...)
{
    char b[512];
    va_list ap; va_start(ap, fmt); _vsnprintf(b, sizeof(b) - 1, fmt, ap); va_end(ap);
    b[sizeof(b) - 1] = 0;
    if (c) { g_pass++; printf("  [DAT   ] %s\n", b); }
    else   { g_fail++; printf("  [HONG  ] %s\n", b); }
}

// --- bo nhan ket qua ---
struct ThuNhan
{
    char  buf[4096];
    int   len;
    int   nRow;
};
static bool NhanMotDong(const KDBRow &row, void *p)
{
    ThuNhan *t = (ThuNhan *)p;
    t->nRow++;
    if (row.nCol >= 1 && row.pLen[0] > 0 && row.pLen[0] <= (int)sizeof(t->buf))
    {
        memcpy(t->buf, row.pVal[0], row.pLen[0]);
        t->len = row.pLen[0];
    }
    return true;
}
struct DemSo { __int64 n; };
static bool NhanSo(const KDBRow &row, void *p)
{
    ((DemSo *)p)->n = row.nCol ? _atoi64(row.pVal[0]) : 0;
    return true;
}

int main()
{
    printf("==========================================================\n");
    printf("KIEM THU KMySQLDB (x64) -- tang MySQL cua CoreServer\n");
    printf("==========================================================\n");

    if (!g_MySQLDB.Init(".\\DataBase.ini", "gamedb"))
    {
        printf("!! Init() THAT BAI -- xem mysql_core.log\n");
        return 2;
    }
    printf("Init() OK\n\n");

    // ---------------------------------------------------------------- 1
    printf("-- 1. Byte nguy hiem phai di ve NGUYEN VEN --\n");
    // Chuoi co: NUL o giua, nhay don, gach cheo nguoc, va byte TCVN3
    char hiem[] = { 'A', 0x00, '\'', '\\', (char)0xD9, (char)0xAD, '"', ';', 0x1A, 'Z' };
    const int nHiem = sizeof(hiem);
    {
        KDBParam p[3];
        p[0] = KDBParam::S("kiemthu");
        p[1] = KDBParam::B("byte_hiem", 9);
        p[2] = KDBParam::B(hiem, nHiem);
        __int64 aff = 0;
        bool w = g_MySQLDB.Exec(
            "INSERT INTO game_kv (ns,k,v,v_len) VALUES (?,?,?,LENGTH(?)) "
            "ON DUPLICATE KEY UPDATE v=VALUES(v), v_len=VALUES(v_len)",
            0, 0, &aff);
        // cau tren thieu tham so thu 4 -> phai THAT BAI, do la mot phep kiem luon
        OK(!w, "cau lenh sai so tham so bi TU CHOI (khong am tham nuot)");

        KDBParam q[4];
        q[0] = KDBParam::S("kiemthu");
        q[1] = KDBParam::B("byte_hiem", 9);
        q[2] = KDBParam::B(hiem, nHiem);
        q[3] = KDBParam::B(hiem, nHiem);
        w = g_MySQLDB.Exec(
            "INSERT INTO game_kv (ns,k,v,v_len) VALUES (?,?,?,LENGTH(?)) "
            "ON DUPLICATE KEY UPDATE v=VALUES(v), v_len=VALUES(v_len)",
            q, 4, &aff);
        OK(w, "ghi duoc gia tri chua NUL / nhay don / gach cheo / byte TCVN3");

        ThuNhan t; memset(&t, 0, sizeof(t));
        KDBParam r[2];
        r[0] = KDBParam::S("kiemthu");
        r[1] = KDBParam::B("byte_hiem", 9);
        g_MySQLDB.Query("SELECT v FROM game_kv WHERE ns=? AND k=?", r, 2, NhanMotDong, &t);
        OK(t.nRow == 1, "doc lai duoc dung 1 dong (%d)", t.nRow);
        OK(t.len == nHiem && memcmp(t.buf, hiem, nHiem) == 0,
           "gia tri doc lai KHOP TUNG BYTE (%d/%d byte)", t.len, nHiem);
    }

    // ---------------------------------------------------------------- 2
    printf("\n-- 2. Ghi BAT DONG BO khong duoc chan vong lap game --\n");
    {
        g_MySQLDB.Exec("DELETE FROM game_log WHERE loai='kiemthu'", 0, 0);
        const int N = 5000;
        DWORD t0 = GetTickCount();
        int nGui = 0;
        for (int i = 0; i < N; i++)
        {
            char nd[64];
            _snprintf(nd, sizeof(nd) - 1, "dong nhat ky so %d", i);
            KDBParam p[4];
            p[0] = KDBParam::S("kiemthu");
            p[1] = KDBParam::B("NguoiChoi", 9);
            p[2] = KDBParam::B(nd, (int)strlen(nd));
            p[3] = KDBParam::I(0);
            if (g_MySQLDB.Post(
                    "INSERT INTO game_log (ngay,loai,role_name,noi_dung,at) "
                    "VALUES (CURDATE(),?,?,?,NOW())", p, 3))
                nGui++;
        }
        DWORD dt = GetTickCount() - t0;
        printf("     day %d dong vao hang doi het %lu ms\n", nGui, dt);
        OK(nGui == N, "day du %d dong vao hang doi (%d)", N, nGui);
        OK(dt < 1000, "day %d dong ton %lu ms -- KHONG chan vong lap game", N, dt);

        // cho luong nen xa
        for (int i = 0; i < 200 && g_MySQLDB.QueueSize() > 0; i++) Sleep(50);
        printf("     hang doi con lai: %d\n", g_MySQLDB.QueueSize());

        DemSo d; d.n = -1;
        g_MySQLDB.Query("SELECT COUNT(*) FROM game_log WHERE loai='kiemthu'", 0, 0, NhanSo, &d);
        OK(d.n == N, "luong ghi nen da ghi du %d dong (%lld)", N, d.n);
        OK(g_MySQLDB.DroppedCount() == 0, "khong goi nao bi bo (%d)",
           g_MySQLDB.DroppedCount());
        OK(g_MySQLDB.ErrorCount() == 1, "so loi dung bang 1 (cau sai tham so o buoc 1) = %d",
           g_MySQLDB.ErrorCount());
    }

    // ---------------------------------------------------------------- 3
    printf("\n-- 3. Ten nhan vat TCVN3 lam khoa --\n");
    {
        // "Nh?c V?n" kieu TCVN3: co byte >= 0x80
        char ten[] = { 'N', 'h', (char)0xB9, 'c', (char)0xD9, 'V', (char)0xAB, 'n' };
        KDBParam p[3];
        p[0] = KDBParam::B(ten, sizeof(ten));
        p[1] = KDBParam::I(12345);
        p[2] = KDBParam::I(0);
        bool w = g_MySQLDB.Exec(
            "INSERT INTO baucua_balance (role_name,balance,locked) VALUES (?,?,?) "
            "ON DUPLICATE KEY UPDATE balance=VALUES(balance)", p, 3);
        OK(w, "ghi duoc so du cho ten TCVN3");

        DemSo d; d.n = -1;
        KDBParam r[1]; r[0] = KDBParam::B(ten, sizeof(ten));
        g_MySQLDB.Query("SELECT balance FROM baucua_balance WHERE role_name=?", r, 1, NhanSo, &d);
        OK(d.n == 12345, "doc lai dung so du theo khoa TCVN3 (%lld)", d.n);

        // chu thuong PHAI KHONG khop (VARBINARY phan biet hoa/thuong)
        char ten2[sizeof(ten)];
        memcpy(ten2, ten, sizeof(ten)); ten2[0] = 'n';
        DemSo d2; d2.n = -1;
        KDBParam r2[1]; r2[0] = KDBParam::B(ten2, sizeof(ten2));
        g_MySQLDB.Query("SELECT balance FROM baucua_balance WHERE role_name=?", r2, 1, NhanSo, &d2);
        OK(d2.n == -1, "doi chu dau thanh thuong thi KHONG khop (phan biet HOA/thuong)");
    }

    // ---------------------------------------------------------------- 4
    printf("\n-- 4. Giao dich: Rollback phai huy sach --\n");
    {
        g_MySQLDB.Exec("DELETE FROM baucua_ledger WHERE viec='kiemthu'", 0, 0);
        g_MySQLDB.Begin();
        KDBParam p[4];
        p[0] = KDBParam::B("Ai Do", 5);
        p[1] = KDBParam::S("kiemthu");
        p[2] = KDBParam::I(-100);
        p[3] = KDBParam::I(900);
        g_MySQLDB.Exec("INSERT INTO baucua_ledger (role_name,viec,so_tien,so_du_sau,at) "
                       "VALUES (?,?,?,?,NOW())", p, 4);
        g_MySQLDB.Rollback();
        DemSo d; d.n = -1;
        g_MySQLDB.Query("SELECT COUNT(*) FROM baucua_ledger WHERE viec='kiemthu'", 0, 0, NhanSo, &d);
        OK(d.n == 0, "sau Rollback khong con dong nao (%lld)", d.n);

        g_MySQLDB.Begin();
        g_MySQLDB.Exec("INSERT INTO baucua_ledger (role_name,viec,so_tien,so_du_sau,at) "
                       "VALUES (?,?,?,?,NOW())", p, 4);
        g_MySQLDB.Commit();
        d.n = -1;
        g_MySQLDB.Query("SELECT COUNT(*) FROM baucua_ledger WHERE viec='kiemthu'", 0, 0, NhanSo, &d);
        OK(d.n == 1, "sau Commit co dung 1 dong (%lld)", d.n);
    }

    // ---------------------------------------------------------------- don
    printf("\n-- don dau vet kiem thu --\n");
    g_MySQLDB.Exec("DELETE FROM game_log WHERE loai='kiemthu'", 0, 0);
    g_MySQLDB.Exec("DELETE FROM game_kv WHERE ns='kiemthu'", 0, 0);
    g_MySQLDB.Exec("DELETE FROM baucua_ledger WHERE viec='kiemthu'", 0, 0);
    g_MySQLDB.Exec("DELETE FROM baucua_balance WHERE balance=12345", 0, 0);
    printf("     xong\n");

    g_MySQLDB.Close();

    printf("\n==========================================================\n");
    printf("TONG KET: DAT=%d  HONG=%d\n", g_pass, g_fail);
    printf("==========================================================\n");
    return g_fail ? 1 : 0;
}
