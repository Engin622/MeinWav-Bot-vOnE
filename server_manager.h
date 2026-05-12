#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct ServerProfile {
    std::string name;
    uintptr_t farmBase;
    uintptr_t farmVidOff;
    uintptr_t selectedBase;
    uintptr_t selectedVidOff;
    uintptr_t netPointer;
    uintptr_t battleCall;
    
    // ElyM2 Specific / New Features
    uintptr_t offSkillRadius;
    uintptr_t offWallHack;
    uintptr_t offWeaponLen;
    uintptr_t offAttackSpeed;
    uintptr_t offMoveSpeed;
};

class ServerManager {
public:
    std::vector<ServerProfile> profiles;
    int selectedIndex = -1;

    void Init() {
        if (!profiles.empty()) return; 

        // ElyM2 - Premium High-Performance Profile
        profiles.push_back({
            "ELYM2 (Premium)", 
            0x011AEE5C, 0x0040E80,  // PlayerBase & Target VID Offset
            0x011AF10C, 0x0040E80,  // Mob Pointer & Target VID
            0x01574164, 0x004AFA90, // NetPointer & SendBattle Call
            0x540, 0x744, 0x678, 0x680, 0x67C // SkillRad, Wall, Weapon, AtkSpd, MovSpd
        });

        // RAZUNING (Server A)
        profiles.push_back({
            "RAZUNING (Server A)", 
            0x002EB0C8, 0x50,
            0x0EDB0C8, 0x0010814,
            0x0EDB098, 0x0C72D20,
            0x0, 0x0, 0x0, 0x0, 0x0 // Placeholder for legacy
        });
    }

    const ServerProfile* GetActive() {
        if (profiles.empty()) Init();
        if (selectedIndex < 0) return nullptr;
        return &profiles[selectedIndex];
    }
};

extern ServerManager g_serverMgr;
