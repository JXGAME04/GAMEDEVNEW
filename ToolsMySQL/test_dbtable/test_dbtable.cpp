//////////////////////////////////////////////////////////////////////////////
// test_dbtable.cpp -- Bo kiem thu doc lap cho ZDBTable ban MySQL.
//
// Goi THANG cac ham ma Goddess dung (open/search/next/add/remove/first), roi
// doi chieu voi tep tham chieu do cong cu Python xuat ra tu Berkeley DB.
// Muc dich: chung minh tang MySQL cho ra DUNG NHUNG BYTE ma Berkeley DB cho ra,
// khong phai "chay duoc la xong".
//
// Bien dich: xem build_test.bat
//////////////////////////////////////////////////////////////////////////////

// PHAI include stdafx.h TRUOC: no dat WIN32_LEAN_AND_MEAN, neu khong thi
// windows.h keo winsock.h vao va dung do voi winsock2.h ma mysql.h can,
// dong thoi lam db.h khong bien dich duoc.
#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

#include "DBTable.h"
#include "S3DBInterface.h"
#include "CRC32.h"

static int g_pass = 0, g_fail = 0;

static void OK(bool cond, const char *fmt, ...)
{
    char buf[1024];
    va_list ap; va_start(ap, fmt); _vsnprintf(buf, sizeof(buf) - 1, fmt, ap); va_end(ap);
    buf[sizeof(buf) - 1] = 0;
    if (cond) { g_pass++; printf("  [DAT   ] %s\n", buf); }
    else      { g_fail++; printf("  [HONG  ] %s\n", buf); }
}

// Ham sinh khoa phu -- Goddess truyen vao addIndex. Ban MySQL khong goi den no.
static int dummy_get_account(DB *, const DBT *, const DBT *, DBT *) { return 0; }

//////////////////////////////////////////////////////////////////////////////
// Doc tep tham chieu do Python xuat: [4B ksize][key][4B dsize][data] lap lai
//////////////////////////////////////////////////////////////////////////////
typedef std::map<std::string, std::string> RefMap;

static bool LoadRef(const char *path, RefMap &out)
{
    FILE *f = fopen(path, "rb");
    if (!f) { printf("!! khong mo duoc tep tham chieu %s\n", path); return false; }
    for (;;)
    {
        unsigned int ks = 0, ds = 0;
        if (fread(&ks, 4, 1, f) != 1) break;
        if (ks == 0 || ks > 64) { printf("!! ksize la: %u\n", ks); fclose(f); return false; }
        std::string k; k.resize(ks);
        if (fread(&k[0], 1, ks, f) != ks) break;
        if (fread(&ds, 4, 1, f) != 1) break;
        std::string d; d.resize(ds);
        if (ds && fread(&d[0], 1, ds, f) != ds) break;
        out[k] = d;
    }
    fclose(f);
    return true;
}

int main(int argc, char **argv)
{
    const char *refpath = (argc > 1) ? argv[1] : "ref_roledb.bin";
    printf("==========================================================\n");
    printf("KIEM THU ZDBTable ban MySQL\n");
    printf("tep tham chieu (xuat tu Berkeley DB): %s\n", refpath);
    printf("==========================================================\n");

    RefMap ref;
    if (!LoadRef(refpath, ref)) return 2;
    printf("tham chieu co %d ban ghi\n\n", (int)ref.size());

    ZDBTable *t = new ZDBTable("database", "roledb");
    t->addIndex(dummy_get_account);
    if (!t->open())
    {
        printf("!! open() THAT BAI -- xem mysql_roledb.log\n");
        return 3;
    }
    printf("open() OK\n\n");

    //---------------------------------------------------------------- 1
    printf("-- 1. search() tung ban ghi va so TUNG BYTE voi Berkeley DB --\n");
    int nfound = 0, nbyte_ok = 0, nmiss = 0, nbyte_bad = 0;
    for (RefMap::iterator it = ref.begin(); it != ref.end(); ++it)
    {
        ZCursor *c = t->search(it->first.data(), (int)it->first.size());
        if (!c) { nmiss++; if (nmiss <= 3) printf("     THIEU: %s\n", it->first.c_str()); continue; }
        nfound++;
        if ((int)it->second.size() == c->size &&
            memcmp(c->data, it->second.data(), c->size) == 0)
            nbyte_ok++;
        else
        {
            nbyte_bad++;
            if (nbyte_bad <= 3)
                printf("     LECH: %s  ref=%d db=%d\n", it->first.c_str(),
                       (int)it->second.size(), c->size);
        }
        t->closeCursor(c);
    }
    OK(nmiss == 0, "khong ban ghi nao bi thieu (thieu=%d)", nmiss);
    OK(nbyte_bad == 0, "moi ban ghi khop tung byte (lech=%d)", nbyte_bad);
    OK(nbyte_ok == (int)ref.size(), "so ban ghi khop = %d / %d", nbyte_ok, (int)ref.size());

    //---------------------------------------------------------------- 2
    printf("\n-- 2. first()/next() duyet toan bang --\n");
    int nscan = 0, nscan_ok = 0, nkey_nul = 0;
    std::map<std::string, int> seen;
    ZCursor *c = t->first();
    while (c)
    {
        nscan++;
        std::string k(c->key, c->key_size);
        seen[k]++;
        if (c->key_size > 0 && c->key[c->key_size - 1] == '\0') nkey_nul++;
        RefMap::iterator f = ref.find(k);
        if (f != ref.end() && (int)f->second.size() == c->size &&
            memcmp(c->data, f->second.data(), c->size) == 0)
            nscan_ok++;
        if (!t->next(c)) break;      // next() tra false -> cursor TU HUY
    }
    OK(nscan == (int)ref.size(), "duyet duoc %d ban ghi, tham chieu %d",
       nscan, (int)ref.size());
    OK(nscan_ok == nscan, "moi ban ghi duyet duoc deu khop byte (%d/%d)", nscan_ok, nscan);
    OK(nkey_nul == nscan, "moi khoa deu ket thuc bang NUL nhu Berkeley DB (%d/%d)",
       nkey_nul, nscan);
    int dup = 0;
    for (std::map<std::string, int>::iterator i = seen.begin(); i != seen.end(); ++i)
        if (i->second > 1) dup++;
    OK(dup == 0, "khong khoa nao bi tra ve hai lan (%d)", dup);

    //---------------------------------------------------------------- 3
    printf("\n-- 3. search(acc, index 0) -- liet ke nhan vat theo tai khoan --\n");
    // dem so nhan vat moi tai khoan tu tham chieu
    std::map<std::string, int> byacc;
    for (RefMap::iterator it = ref.begin(); it != ref.end(); ++it)
    {
        const TRoleData *p = (const TRoleData *)it->second.data();
        byacc[std::string(p->BaseInfo.caccname)]++;
    }
    int acc_checked = 0, acc_ok = 0;
    for (std::map<std::string, int>::iterator a = byacc.begin();
         a != byacc.end() && acc_checked < 60; ++a, ++acc_checked)
    {
        std::string key = a->first; key.push_back('\0');
        int n = 0;
        ZCursor *ac = t->search(key.data(), (int)key.size(), 0);
        while (ac) { n++; if (!t->next(ac)) break; }
        if (n == a->second) acc_ok++;
        else printf("     LECH tai khoan %s: db=%d ref=%d\n", a->first.c_str(), n, a->second);
    }
    OK(acc_ok == acc_checked, "dem nhan vat theo tai khoan dung %d/%d",
       acc_ok, acc_checked);

    //---------------------------------------------------------------- 4
    printf("\n-- 4. tra cuu ten KHONG ton tai --\n");
    const char *nope = "___khong_co_ai_ten_the_nay___";
    ZCursor *nc = t->search(nope, (int)strlen(nope) + 1);
    OK(nc == NULL, "search() ten khong ton tai tra NULL");
    if (nc) t->closeCursor(nc);

    //---------------------------------------------------------------- 5
    printf("\n-- 5. PHAN BIET HOA/thuong (day la cho mat 27 nhan vat neu dung _ci) --\n");
    // tim mot cap chi khac hoa/thuong trong tham chieu
    int cs_checked = 0, cs_ok = 0;
    for (RefMap::iterator it = ref.begin(); it != ref.end() && cs_checked < 8; ++it)
    {
        std::string lower = it->first;
        bool haveUpper = false;
        for (size_t i = 0; i + 1 < lower.size(); i++)
            if (lower[i] >= 'A' && lower[i] <= 'Z') { lower[i] += 'a' - 'A'; haveUpper = true; }
        if (!haveUpper) continue;
        if (ref.find(lower) != ref.end()) continue;   // ca hai deu ton tai, bo qua
        cs_checked++;
        ZCursor *lc = t->search(lower.data(), (int)lower.size());
        if (lc == NULL) cs_ok++;
        else { printf("     SAI: tra cuu \"%s\" lai ra ket qua!\n", lower.c_str());
               t->closeCursor(lc); }
    }
    OK(cs_checked == 0 || cs_ok == cs_checked,
       "tra cuu ban chu thuong KHONG khop ban chu hoa (%d/%d)", cs_ok, cs_checked);

    //---------------------------------------------------------------- 6
    printf("\n-- 6. add() ghi de roi doc lai --\n");
    if (!ref.empty())
    {
        RefMap::iterator it = ref.begin();
        std::string k = it->first;
        std::string blob = it->second;
        // doi mot truong vo hai (ipkvalue) roi ghi lai
        TRoleData *p = (TRoleData *)&blob[0];
        int old_pk = p->BaseInfo.ipkvalue;
        p->BaseInfo.ipkvalue = old_pk + 7;
        bool added = t->add(k.data(), (int)k.size(), blob.data(), (int)blob.size());
        OK(added, "add() ghi de tra ve true");
        ZCursor *rc = t->search(k.data(), (int)k.size());
        OK(rc != NULL, "doc lai duoc sau khi ghi de");
        if (rc)
        {
            OK(rc->size == (int)blob.size() &&
               memcmp(rc->data, blob.data(), rc->size) == 0,
               "noi dung doc lai khop TUNG BYTE voi thu vua ghi");
            t->closeCursor(rc);
        }
        // tra lai nguyen trang
        p->BaseInfo.ipkvalue = old_pk;
        t->add(k.data(), (int)k.size(), blob.data(), (int)blob.size());
        ZCursor *r2 = t->search(k.data(), (int)k.size());
        OK(r2 && r2->size == (int)it->second.size() &&
           memcmp(r2->data, it->second.data(), r2->size) == 0,
           "da tra ban ghi ve nguyen trang");
        if (r2) t->closeCursor(r2);
    }

    //---------------------------------------------------------------- 7
    printf("\n-- 7. add() phai TU CHOI kich thuoc phi ly (lo hong dwDataLen am) --\n");
    {
        char rac[64]; memset(rac, 0, sizeof(rac));
        const char *kk = "___ban_ghi_rac___";
        OK(!t->add(kk, (int)strlen(kk) + 1, rac, 0),   "tu choi data_size = 0");
        OK(!t->add(kk, (int)strlen(kk) + 1, rac, 3),   "tu choi data_size = 3");
        OK(!t->add(kk, (int)strlen(kk) + 1, rac, -8),  "tu choi data_size AM");
        OK(!t->add(kk, (int)strlen(kk) + 1, rac, 64),  "tu choi blob ngan hon header");
        ZCursor *gc = t->search(kk, (int)strlen(kk) + 1);
        OK(gc == NULL, "khong co ban ghi rac nao lot vao bang");
        if (gc) t->closeCursor(gc);
    }

    //---------------------------------------------------------------- 8
    printf("\n-- 8. remove() phai luu ban goc vao role_delete_log truoc khi xoa --\n");
    if (!ref.empty())
    {
        RefMap::iterator it = ref.begin();
        std::string k = it->first;
        bool rm = t->remove(k.data(), (int)k.size());
        OK(rm, "remove() tra ve true");
        ZCursor *gc = t->search(k.data(), (int)k.size());
        OK(gc == NULL, "ban ghi da bien khoi bang role");
        if (gc) t->closeCursor(gc);
        // nap lai de tra nguyen trang
        bool back = t->add(k.data(), (int)k.size(), it->second.data(), (int)it->second.size());
        OK(back, "nap lai duoc ban ghi vua xoa");
        ZCursor *r3 = t->search(k.data(), (int)k.size());
        OK(r3 && r3->size == (int)it->second.size() &&
           memcmp(r3->data, it->second.data(), r3->size) == 0,
           "ban ghi nap lai khop tung byte voi ban goc");
        if (r3) t->closeCursor(r3);
        printf("     (kiem role_delete_log bang SQL o buoc sau)\n");
    }

    //---------------------------------------------------------------- 9
    printf("\n-- 9. CHONG LOI DU LIEU: dung lai kich ban MAT VAT PHAM --\n");
    if (!ref.empty())
    {
        // Tim mot nhan vat co du vat pham de cat bot
        RefMap::iterator src = ref.end();
        for (RefMap::iterator it = ref.begin(); it != ref.end(); ++it)
        {
            const TRoleData *p = (const TRoleData *)it->second.data();
            if (p->nItemCount >= 20) { src = it; break; }
        }
        if (src == ref.end())
            printf("     (khong tim duoc nhan vat >=20 vat pham, bo qua)\n");
        else
        {
            std::string k = src->first;
            const std::string &goc = src->second;
            const TRoleData *pg = (const TRoleData *)goc.data();
            int soCat = 10;

            // Dung blob "bi mat 10 vat pham": vat pham la khoi CUOI truoc CRC,
            // nen chi can cat bot tu duoi len roi noi lai CRC.
            int newLen = pg->dwDataLen - soCat * (int)sizeof(TDBItemData);
            std::string xau(goc.data(), newLen - 4);
            TRoleData *px = (TRoleData *)&xau[0];
            int soCu  = pg->nItemCount;
            int soMoi = soCu - soCat;
            px->nItemCount = (short)soMoi;
            px->dwDataLen  = newLen;
            unsigned c = CRC32(0, xau.data(), (unsigned)xau.size());
            // CANH BAO: append() co the CAP PHAT LAI vung dem -> px thanh con tro TREO.
            // Phai lay so lieu ra bien cuc bo TRUOC khi append.
            xau.append((const char *)&c, 4);
            px = NULL;

            printf("     nhan vat %s: %d vat pham -> %d\n", k.c_str(), soCu, soMoi);
            bool w = t->add(k.data(), (int)k.size(), xau.data(), (int)xau.size());
            OK(w, "add() van ghi (Goddess khong tu y chan -- nhung PHAI de lai dau vet)");

            ZCursor *vc = t->search(k.data(), (int)k.size());
            OK(vc && vc->size == (int)xau.size(), "doc lai ra ban ghi MOI (da mat do)");
            if (vc) t->closeCursor(vc);

            // Nap lai ban goc de tra nguyen trang
            t->add(k.data(), (int)k.size(), goc.data(), (int)goc.size());
            ZCursor *bc = t->search(k.data(), (int)k.size());
            OK(bc && bc->size == (int)goc.size() &&
               memcmp(bc->data, goc.data(), bc->size) == 0,
               "da tra ban ghi ve nguyen trang tu ban goc");
            if (bc) t->closeCursor(bc);
            printf("     -> kiem role_anomaly + role_history bang SQL o buoc sau\n");
        }
    }

    t->close();
    delete t;

    printf("\n==========================================================\n");
    printf("TONG KET: DAT=%d  HONG=%d\n", g_pass, g_fail);
    printf("==========================================================\n");
    return g_fail ? 1 : 0;
}
