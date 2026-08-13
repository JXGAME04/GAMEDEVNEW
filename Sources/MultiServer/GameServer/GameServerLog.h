#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <iostream>

class GameServerLog {
public:
    static GameServerLog& Instance() {
        static GameServerLog instance;
        return instance;
    }

    void Write(const std::string& msg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_ofs.is_open()) {
            m_ofs.open("GameServer.log", std::ios::app);
        }
        if (m_ofs.is_open()) {
            m_ofs << msg << std::endl;
        }
    }

    // Optional: also log to console
    void WriteAndConsole(const std::string& msg) {
        Write(msg);
        std::cout << msg << std::endl;
    }

private:
    GameServerLog() = default;
    ~GameServerLog() { if (m_ofs.is_open()) m_ofs.close(); }
    GameServerLog(const GameServerLog&) = delete;
    GameServerLog& operator=(const GameServerLog&) = delete;

    std::ofstream m_ofs;
    std::mutex m_mutex;
};