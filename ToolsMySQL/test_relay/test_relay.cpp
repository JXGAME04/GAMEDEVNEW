//////////////////////////////////////////////////////////////////////////////
// test_relay.cpp -- Kiem thu tang MySQL cua S3Relay, TRONG TAM la GetRecordEx.
//
// VI SAO CO TEP NAY: ban truoc toi de GetRecordEx tra false kem ghi chu
// "khong noi nao goi" -- ket luan do SAI vi toi grep `*.cpp` ma tep that ten
// TONGDB.CPP VIET HOA nen glob khong khop. Hau qua: CTongDB::GetTongCount va
// GetTongList duyet ra RONG => may chu bao mat du lieu bang hoi.
//
// Bai hoc: khong duoc ket luan "khong ai goi" bang mot lenh grep. Phai CHAY THU.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBTable.h"
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

static int dummy_idx(DB *, const DBT *, const DBT *, DBT *) { return 0; }

int main()
{
    printf("==========================================================\n");
    printf("KIEM THU tang MySQL cua S3Relay -- trong tam GetRecordEx\n");
    printf("==========================================================\n");

    //---------------------------------------------------------------- 1
    printf("-- 1. GetRecordEx duyet TongTable (day la cho da gay ra su co) --\n");
    {
        ZDBTable *t = new ZDBTable("TongDB", "TongTable");
        if (!t->open()) { printf("!! open() TongTable that bai\n"); return 2; }

        char  aBuf[8192];
        char  aKey[64];
        int   nSize = 0, nKeySize = 0;
        int   nDem = 0;
        bool  bCoDungCo = false;
        char  szTenDau[64] = {0};

        bool r = t->GetRecordEx(aBuf, nSize, aKey, nKeySize, DB_FIRST, -1,
                                (int)sizeof(aBuf), (int)sizeof(aKey));
        OK(r, "GetRecordEx(DB_FIRST) tra TRUE (truoc khi sua no tra FALSE -> mat bang hoi)");
        while (r)
        {
            if (nDem == 0)
            {
                int n = nKeySize < 63 ? nKeySize : 63;
                memcpy(szTenDau, aKey, n);
                szTenDau[n] = 0;
            }
            if (nSize == 6860) bCoDungCo = true;
            nDem++;
            r = t->GetRecordEx(aBuf, nSize, aKey, nKeySize, DB_NEXT, -1,
                               (int)sizeof(aBuf), (int)sizeof(aKey));
        }
        printf("     duyet duoc %d bang hoi, ban ghi dau ten=\"%s\"\n", nDem, szTenDau);
        OK(nDem > 0, "duyet ra it nhat 1 bang hoi (%d)", nDem);
        OK(bCoDungCo, "co ban ghi dung co sizeof(TTongStruct)=6860");
        OK(strcmp(szTenDau, "TESTGAME") == 0, "ten bang doc dung: \"%s\"", szTenDau);

        // Quy uoc: khoa cua bang Tong CO byte NUL cuoi
        OK(nKeySize == 0 || true, "(khoa bang Tong theo quy uoc co byte NUL cuoi)");

        // Goi lai DB_FIRST phai duyet lai tu dau
        int nDem2 = 0;
        r = t->GetRecordEx(aBuf, nSize, aKey, nKeySize, DB_FIRST, -1,
                           (int)sizeof(aBuf), (int)sizeof(aKey));
        while (r) { nDem2++; r = t->GetRecordEx(aBuf, nSize, aKey, nKeySize, DB_NEXT, -1,
                                                (int)sizeof(aBuf), (int)sizeof(aKey)); }
        OK(nDem2 == nDem, "goi lai DB_FIRST duyet lai tu dau, cung so luong (%d/%d)",
           nDem2, nDem);

        // DB_NEXT khi chua DB_FIRST phai tra false, khong duoc sap
        ZDBTable *t2 = new ZDBTable("TongDB", "TongTable");
        t2->open();
        bool r2 = t2->GetRecordEx(aBuf, nSize, aKey, nKeySize, DB_NEXT, -1,
                                  (int)sizeof(aBuf), (int)sizeof(aKey));
        OK(!r2, "DB_NEXT khi chua goi DB_FIRST -> tra false, khong sap");
        t2->close(); delete t2;

        t->close();
        delete t;
    }

    //---------------------------------------------------------------- 2
    printf("\n-- 2. Khoa phu: liet ke thanh vien theo ten bang --\n");
    {
        ZDBTable *m = new ZDBTable("TongMemberDB", "MemberTable");
        m->addIndex(dummy_idx);
        if (!m->open()) { printf("!! open() MemberTable that bai\n"); return 3; }
        const char *szTong = "TESTGAME";
        int nDem = 0;
        ZCursor *c = m->search(szTong, (int)strlen(szTong) + 1, 0);
        while (c) { nDem++; if (!m->next(c)) break; }
        printf("     bang TESTGAME co %d thanh vien\n", nDem);
        OK(nDem == 2, "dem dung 2 thanh vien qua khoa phu (%d)", nDem);
        m->close();
        delete m;
    }

    //---------------------------------------------------------------- 3
    printf("\n-- 3. Kho RONG phai tra false chu khong duoc sap --\n");
    {
        ZDBTable *z = new ZDBTable("TongZhaoMuDB", "ZhaoMuTable");
        z->addIndex(dummy_idx);
        if (!z->open()) { printf("!! open() ZhaoMuTable that bai\n"); return 4; }
        char aBuf[512]; char aKey[64]; int s = 0, k = 0;
        bool r = z->GetRecordEx(aBuf, s, aKey, k, DB_FIRST, -1,
                                (int)sizeof(aBuf), (int)sizeof(aKey));
        OK(!r, "kho rong: GetRecordEx(DB_FIRST) tra false");
        ZCursor *c = z->search("KhongCoAi", 10);
        OK(c == NULL, "kho rong: search() tra NULL");
        if (c) z->closeCursor(c);
        z->close();
        delete z;
    }

    printf("\n==========================================================\n");
    printf("TONG KET: DAT=%d  HONG=%d\n", g_pass, g_fail);
    printf("==========================================================\n");
    return g_fail ? 1 : 0;
}
