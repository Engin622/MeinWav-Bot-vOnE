#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <ctime>

// --- AYAR VE YAPILANDIRMA (CONFIG) SISTEMI ---
struct BotSettings {
    int breakIntervalMin = 15; // Kac dakikada bir mola verilecek
    int breakDurationMin = 1;  // Mola kac dakika surecek
    bool enableRoute = false;  // Rota takibi acik mi
};

class BotConfig {
public:
    static BotSettings Settings;
    
    static void Load(const std::string& path = ".\\config.ini") {
        Settings.breakIntervalMin = GetPrivateProfileIntA("FARM", "BreakInterval", 15, path.c_str());
        Settings.breakDurationMin = GetPrivateProfileIntA("FARM", "BreakDuration", 1, path.c_str());
        Settings.enableRoute = GetPrivateProfileIntA("ROUTE", "EnableRoute", 0, path.c_str());
    }
    
    static void Save(const std::string& path = ".\\config.ini") {
        WritePrivateProfileStringA("FARM", "BreakInterval", std::to_string(Settings.breakIntervalMin).c_str(), path.c_str());
        WritePrivateProfileStringA("FARM", "BreakDuration", std::to_string(Settings.breakDurationMin).c_str(), path.c_str());
        WritePrivateProfileStringA("ROUTE", "EnableRoute", std::to_string(Settings.enableRoute).c_str(), path.c_str());
    }
};

inline BotSettings BotConfig::Settings;


// --- ISTATISTIK VE LOGLAMA SISTEMI ---
class FarmLogger {
public:
    static uint32_t KilledMetins;
    static uint32_t KilledSlots;

    static void LogKill(const std::string& targetName, bool isMetin) {
        if (isMetin) KilledMetins++;
        else KilledSlots++;

        time_t t = time(nullptr);
        struct tm tm_info;
        localtime_s(&tm_info, &t);
        
        char dateFileName[128];
        strftime(dateFileName, sizeof(dateFileName), "FarmLog_%Y-%m-%d.txt", &tm_info);
        
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "[%H:%M:%S] ", &tm_info);

        std::ofstream file(dateFileName, std::ios::app);
        if(file.is_open()) {
            file << timeStr << "KESILDI: " << targetName << " (" << (isMetin ? "METIN" : "SLOT") << ")\n";
            file.close();
        }
    }
};

inline uint32_t FarmLogger::KilledMetins = 0;
inline uint32_t FarmLogger::KilledSlots = 0;
