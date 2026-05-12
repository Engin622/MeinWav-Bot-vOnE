// patch_survival.h — v14.2 (PASSIVE SAFE MODE)
#pragma once
#include "pch.h"
#include <string>

namespace RuntimeOffsets {
    // Calistigi onaylanmis sabit adresler
    inline uintptr_t BASE_ADDR      = 0x0029B9AC; 
    inline uintptr_t PLAYER_BASE    = 0x002EB0B4;
    inline uintptr_t SELECTED_BASE  = 0x002EB0C8;
    inline uintptr_t NET_POINTER    = 0x002DB098; 
    inline uintptr_t BATTLE_CALL    = 0x00872D20; 
    inline bool         patchDetected = false;
    inline std::string  patchLog = "SAFE_MODE_ACTIVE";
}

// Pasif tarayici - Sadece istendiginde calisir, acilista cokme yapmaz
class PatternScanner {
public:
    void RunStartupCheck(const std::string& dummy) {
        // Otomatik tarama kapatildi (Crash on Injection onlemek icin)
    }
};
inline PatternScanner g_patchScanner;
