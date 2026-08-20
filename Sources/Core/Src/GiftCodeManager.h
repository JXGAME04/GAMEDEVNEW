// GiftCodeManager.h

#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

//////////////////////////////////////////////////////////////////////////////
// (20/08) Da chuyen sang MySQL, VAN GIU duong tep lam duong lui.
//
// LO HONG CUA BAN CU: "danh dau da dung" la APPEND mot dong vao tep text
// (SaveUsedCode). Viec do KHONG NGUYEN TU va chi co tac dung trong MOT tien
// trinh. Hai nguoi choi nhan cung mot ma o hai GameServer khac nhau thi CA HAI
// deu duoc -- vi moi tien trinh giu ban sao validCodes rieng trong bo nho.
// Them nua, mat tep giftcode_tuan_used.txt la 5.000 ma quay lai trang thai
// CHUA DUNG (tep do hien KHONG TON TAI tren may dang chay).
//
// Ban MySQL dung MOT cau:
//     UPDATE giftcode SET used_by_role=?, used_at=NOW()
//     WHERE code=? AND batch=? AND used_at IS NULL
// roi kiem affected_rows == 1. Nguyen tu, dung ca khi co nhieu GameServer.
//
// Neu MySQL chua san sang (g_MySQLDB.IsReady() == false) thi TU DONG quay ve
// duong tep cu -- may chu KHONG BAO GIO chet chi vi thieu MySQL.
//////////////////////////////////////////////////////////////////////////////
class GiftCodeManager {
public:
    // szBatch: ten lo trong bang MySQL ('new' hoac 'tuan'). Phai dat truoc khi
    // dung duong MySQL; de rong thi chi con duong tep.
    void SetBatch(const char* szBatch);
    const char* GetBatch() const { return batchName.c_str(); }

    bool LoadGiftCodes(const std::string& filename);
    bool LoadUsedCodes(const std::string& filename);

    // szRole: ten nhan vat nhan ma, de truy vet duoc. Cho phep NULL.
    bool UseCode(const std::string& code, const char* szRole = 0);
    bool IsValidCode(const std::string& code) const;

private:
    std::unordered_map<std::string, bool> validCodes;
    std::unordered_set<std::string> usedCodes;

    std::string usedCodeFilePath; // <- Add this
    std::string batchName;        // (20/08) ten lo trong bang MySQL

    mutable std::mutex codeMutex;
    void SaveUsedCode(const std::string& code);

    // true neu dang dung duong MySQL (da ket noi VA da dat ten lo)
    bool DungMySQL() const;
};

extern CORE_API GiftCodeManager g_GiftCodeFanCungManager;
extern CORE_API GiftCodeManager g_GiftCodeNewManager;