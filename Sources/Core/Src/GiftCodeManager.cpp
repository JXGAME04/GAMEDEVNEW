#include <KCore.h>
#include "GiftCodeManager.h"
#include "KMySQLDB.h"
#include <fstream>
#include <iostream>

GiftCodeManager g_GiftCodeFanCungManager;
GiftCodeManager g_GiftCodeNewManager;

void GiftCodeManager::SetBatch(const char* szBatch) {
    batchName = szBatch ? szBatch : "";
}

bool GiftCodeManager::DungMySQL() const {
#ifdef _SERVER
    return g_MySQLDB.IsReady() && !batchName.empty();
#else
    return false;
#endif
}

bool GiftCodeManager::LoadGiftCodes(const std::string& filename) {
    // Van nap vao bo nho de con duong lui khi MySQL chet giua chung.
    std::ifstream infile(filename);
    std::string code;
    while (std::getline(infile, code)) {
        if (!code.empty())
            validCodes[code] = true;
    }
    return true;
}

bool GiftCodeManager::LoadUsedCodes(const std::string& filename) {
    std::ifstream infile(filename);
    usedCodeFilePath = filename; // Save it for reuse
    std::string code;
    while (std::getline(infile, code)) {
        if (!code.empty()) {
            usedCodes.insert(code);
            validCodes.erase(code);
        }
    }
    return true;
}

#ifdef _SERVER
// Bo nhan ket qua cho cau SELECT dem
struct KGiftDem { __int64 n; };
static bool _NhanDem(const KDBRow& row, void* p) {
    ((KGiftDem*)p)->n = (row.nCol > 0 && row.pLen[0] > 0) ? _atoi64(row.pVal[0]) : 0;
    return true;
}
#endif

bool GiftCodeManager::IsValidCode(const std::string& code) const {
    std::lock_guard<std::mutex> lock(codeMutex);

#ifdef _SERVER
    if (DungMySQL()) {
        // Nguon su that la MySQL. Ban sao trong bo nho co the cu (vd GameServer
        // khac vua dung ma nay) nen KHONG duoc tin.
        KGiftDem d; d.n = 0;
        KDBParam p[2];
        p[0] = KDBParam::B(code.data(), (int)code.size());
        p[1] = KDBParam::S(batchName.c_str());
        if (g_MySQLDB.Query(
                "SELECT COUNT(*) FROM giftcode WHERE code=? AND batch=? AND used_at IS NULL",
                p, 2, _NhanDem, &d))
            return d.n > 0;
        // Truy van hong -> quay ve ban trong bo nho, va ghi nhat ky de biet.
        KDBLog("giftcode: IsValidCode truy van hong, tam dung ban trong bo nho (lo=%s)",
               batchName.c_str());
    }
#endif
    return validCodes.find(code) != validCodes.end();
}

bool GiftCodeManager::UseCode(const std::string& code, const char* szRole) {
    std::lock_guard<std::mutex> lock(codeMutex);

#ifdef _SERVER
    if (DungMySQL()) {
        // MOT cau lenh, nguyen tu. Dieu kien `used_at IS NULL` chinh la cho
        // chan hai nguoi cung nhan mot ma -- ke ca o hai tien trinh khac nhau.
        KDBParam p[3];
        int n = 0;
        if (szRole && szRole[0]) p[n++] = KDBParam::B(szRole, (int)strlen(szRole));
        else                     p[n++] = KDBParam::Null();
        p[n++] = KDBParam::B(code.data(), (int)code.size());
        p[n++] = KDBParam::S(batchName.c_str());

        __int64 aff = 0;
        if (g_MySQLDB.Exec(
                "UPDATE giftcode SET used_by_role=?, used_at=NOW() "
                "WHERE code=? AND batch=? AND used_at IS NULL", p, n, &aff))
        {
            if (aff == 1) {
                // Dong bo ban trong bo nho de duong lui khong cap ma nay lan nua
                validCodes.erase(code);
                usedCodes.insert(code);
                return true;
            }
            return false;   // ma khong ton tai, sai lo, hoac DA CO NGUOI DUNG
        }
        // Cau lenh hong (mat ket noi...): KHONG duoc am tham cap ma. Bao that bai
        // va ghi nhat ky -- nguoi choi thu lai la duoc, con hon phat trung.
        KDBLog("giftcode: UseCode truy van hong -> TU CHOI cap ma (lo=%s)",
               batchName.c_str());
        return false;
    }
#endif

    // ---- duong lui: ban tep cu ----
    auto it = validCodes.find(code);
    if (it == validCodes.end()) {
        return false; // Invalid or already used
    }

    validCodes.erase(it);
    usedCodes.insert(code);
    SaveUsedCode(code);
    return true;
}

void GiftCodeManager::SaveUsedCode(const std::string& code) {
    if (usedCodeFilePath.empty()) {
        std::cerr << "Error: usedCodeFilePath is not set.\n";
        return;
    }

    std::ofstream outfile(usedCodeFilePath, std::ios::app);
    if (outfile.is_open()) {
        outfile << code << "\n";
    }
    else {
        std::cerr << "Failed to open used code file for writing: " << usedCodeFilePath << "\n";
    }
}
