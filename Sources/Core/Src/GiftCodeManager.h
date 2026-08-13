// GiftCodeManager.h

#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

class GiftCodeManager {
public:
    bool LoadGiftCodes(const std::string& filename);
    bool LoadUsedCodes(const std::string& filename);
    bool UseCode(const std::string& code);
    bool IsValidCode(const std::string& code) const;

private:
    std::unordered_map<std::string, bool> validCodes;
    std::unordered_set<std::string> usedCodes;

    std::string usedCodeFilePath; // <- Add this

    mutable std::mutex codeMutex;
    void SaveUsedCode(const std::string& code);
};

extern CORE_API GiftCodeManager g_GiftCodeFanCungManager;
extern CORE_API GiftCodeManager g_GiftCodeNewManager;