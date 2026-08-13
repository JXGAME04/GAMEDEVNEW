#include <KCore.h>
#include "GiftCodeManager.h"
#include <fstream>
#include <iostream>

GiftCodeManager g_GiftCodeFanCungManager;
GiftCodeManager g_GiftCodeNewManager;

bool GiftCodeManager::LoadGiftCodes(const std::string& filename) {
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

bool GiftCodeManager::IsValidCode(const std::string& code) const {
    std::lock_guard<std::mutex> lock(codeMutex);
    return validCodes.find(code) != validCodes.end();
}

bool GiftCodeManager::UseCode(const std::string& code) {
    std::lock_guard<std::mutex> lock(codeMutex);

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