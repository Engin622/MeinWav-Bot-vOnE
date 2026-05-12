#pragma once
#include "pch.h"
#include <string>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "patch_survival.h"
#include "server_manager.h"

struct PlayerPos { float x, y, z; };

struct Entity {
    uint32_t vid;
    int type;
    float x, y, z;
    std::string name;
    float dist;
    uint32_t level;
    uint32_t vnum; // Added VNUM support
    uintptr_t address;
};

// --- GLOBAL SAFE HELPER (C2712 FIX) ---
static bool SafeReadString(uintptr_t addr, char* buffer, size_t size) {
    __try { memcpy(buffer, (void*)addr, size); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

namespace Offsets {
    constexpr uintptr_t PLAYER_OFF1      = 0xC;   
    constexpr uintptr_t PLAYER_NAME_OFF  = 0x10;  
    constexpr uintptr_t PLAYER_LEVEL_OFF = 0x3C;  
    constexpr uintptr_t PLAYER_X         = 0x570; 
    constexpr uintptr_t PLAYER_Y         = 0x574; 
    constexpr uintptr_t PLAYER_Z         = 0x578; 
    constexpr uintptr_t WALLHACK_OFF     = 0x664; 
    constexpr uintptr_t ATK_SPEED_OFF    = 0x5A4; 
    constexpr uintptr_t WEAPON_SIZE_OFF  = 0x59C; 
    constexpr uintptr_t MOVE_SPEED_OFF   = 0x5A0; 
    constexpr uintptr_t INST_VID         = 0x065C; 
    constexpr uintptr_t INST_TYPE        = 0x0504; 
    constexpr uintptr_t INST_POS_X       = 0x0570; 
    constexpr uintptr_t INST_POS_Y       = 0x0574; 
    constexpr uintptr_t INST_POS_Z       = 0x0578; 
    constexpr uintptr_t INST_NAME_OFF    = 0x10;   // Corrected name offset
    constexpr uintptr_t INST_LEVEL_OFF   = 0x003C; 
    constexpr uintptr_t INST_VNUM_OFF    = 0x0508; 
    constexpr uintptr_t INST_DEAD_OFF    = 0x628; 
    constexpr uintptr_t FARM_BASE_ADDR   = 0x002EB0C8; // Updated from user's detail
    constexpr uintptr_t FARM_VID_OFF     = 0x50;       // Updated from user's detail
    constexpr uint32_t  MAX_ENTITIES     = 1024;
    constexpr uintptr_t SELECTED_VID_OFF = 0x10814;
}

class Scanner {
public:
    template<typename T>
    static T SafeRead(uintptr_t addr) {
        __try { return *reinterpret_cast<T*>(addr); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return T(); }
    }

    template<typename T>
    static void SafeWrite(uintptr_t addr, T val) {
        __try {
            DWORD old; 
            if (VirtualProtect((LPVOID)addr, sizeof(T), PAGE_EXECUTE_READWRITE, &old)) {
                *reinterpret_cast<T*>(addr) = val;
                VirtualProtect((LPVOID)addr, sizeof(T), old, &old);
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    template<typename T>
    static void FastWrite(uintptr_t addr, T val) {
        __try {
            *reinterpret_cast<T*>(addr) = val;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    uintptr_t m_base = 0;
    std::vector<Entity> m_list;
    uint32_t m_selectedVID = 0;
    bool m_wallhackEnabled = false;
    bool m_hacksEnabled = false;
    float m_atkSpeedVal = 100.0f; // Default start for ElyM2
    float m_moveSpeedVal = 100.0f;
    float m_weaponLenVal = 100.0f;
    float m_skillRadiusVal = 100.0f;
    float m_origAtk = -1.0f, m_origMove = -1.0f, m_origWeapon = -1.0f, m_origSkill = -1.0f;

    bool Init() {
        m_base = (uintptr_t)GetModuleHandleA(nullptr);
        return true;
    }

    bool IsValidPtr(uintptr_t p) const { return (p > 0x10000 && p < 0x7FFFFFFE); }

    float GetDistance(float x1, float y1, float x2, float y2) const {
        float dx = x1 - x2; float dy = y1 - y2;
        return sqrtf(dx*dx + dy*dy) / 100.0f;
    }

    uintptr_t GetPlayerBase() const {
        auto active = g_serverMgr.GetActive();
        if (!active) return 0;
        uintptr_t p1 = SafeRead<uintptr_t>(m_base + active->farmBase);
        if (!IsValidPtr(p1)) return 0;
        return SafeRead<uintptr_t>(p1 + 0xC); // Standard CPythonPlayer -> InstanceBase
    }

    PlayerPos GetPlayerPos() const {
        uintptr_t p2 = GetPlayerBase();
        if (!p2) return { 0, 0, 0 };
        return { SafeRead<float>(p2 + 0x64C), SafeRead<float>(p2 + 0x650), SafeRead<float>(p2 + 0x654) };
    }

    std::string GetPlayerName() const {
        uintptr_t p2 = GetPlayerBase();
        if (!p2) return "N/A";
        char name[32] = {0};
        if (SafeReadString(p2 + 0x10, name, 31)) return std::string(name);
        return "N/A";
    }

    int GetPlayerLevel() const {
        uintptr_t p2 = GetPlayerBase();
        if (!p2) return 0;
        return (int)SafeRead<uint32_t>(p2 + 0x40);
    }

    void Update() {
        m_list.clear();
        auto active = g_serverMgr.GetActive();
        if (!active) return;

        uintptr_t pList = SafeRead<uintptr_t>(m_base + active->selectedBase);
        if (!IsValidPtr(pList)) return;
        PlayerPos myPos = GetPlayerPos();
        std::unordered_set<uintptr_t> seen;

        for (uint32_t i = 0; i < 1024; i++) {
            uintptr_t inst = SafeRead<uintptr_t>(pList + i * 4);
            if (!IsValidPtr(inst) || seen.count(inst)) continue;
            seen.insert(inst);
            
            Entity e;
            e.vid = SafeRead<uint32_t>(inst + active->farmVidOff);
            if (e.vid == 0) continue;
            
            // isDead check (0x628)
            if (SafeRead<uint8_t>(inst + 0x628) == 1) continue;

            e.type = SafeRead<int>(inst + 0x5DC);
            e.x = SafeRead<float>(inst + 0x64C);
            e.y = SafeRead<float>(inst + 0x650);
            e.z = SafeRead<float>(inst + 0x654);
            e.dist = GetDistance(e.x, e.y, myPos.x, myPos.y);
            e.level = SafeRead<uint32_t>(inst + 0x40);
            e.vnum = SafeRead<uint32_t>(inst + 0x5E0);
            e.address = inst;
            
            char nameBuf[64] = {0};
            SafeReadString(inst + 0x10, nameBuf, 64);
            e.name = nameBuf;

            if (e.dist < 250.0f) m_list.push_back(e);
        }
        std::sort(m_list.begin(), m_list.end(), [](const Entity& a, const Entity& b) { return a.dist < b.dist; });
        
        UpdateHacks();
    }

    void UpdateHacks() {
        auto active = g_serverMgr.GetActive();
        if (!active || active->offSkillRadius == 0) return;

        uintptr_t p2 = GetPlayerBase();
        if (!p2) return;

        if (m_hacksEnabled) {
            if (m_origAtk == -1.0f) m_origAtk = SafeRead<float>(p2 + active->offAttackSpeed);
            if (m_origMove == -1.0f) m_origMove = SafeRead<float>(p2 + active->offMoveSpeed);
            if (m_origWeapon == -1.0f) m_origWeapon = SafeRead<float>(p2 + active->offWeaponLen);
            if (m_origSkill == -1.0f) m_origSkill = SafeRead<float>(p2 + active->offSkillRadius);

            FastWrite<float>(p2 + active->offAttackSpeed, m_atkSpeedVal);
            FastWrite<float>(p2 + active->offMoveSpeed, m_moveSpeedVal);
            FastWrite<float>(p2 + active->offWeaponLen, m_weaponLenVal);
            FastWrite<float>(p2 + active->offSkillRadius, m_skillRadiusVal);
            FastWrite<uint8_t>(p2 + active->offWallHack, m_wallhackEnabled ? 1 : 0);
        } else if (m_origAtk != -1.0f) {
            FastWrite<float>(p2 + active->offAttackSpeed, m_origAtk);
            FastWrite<float>(p2 + active->offMoveSpeed, m_origMove);
            FastWrite<float>(p2 + active->offWeaponLen, m_origWeapon);
            FastWrite<float>(p2 + active->offSkillRadius, m_origSkill);
            FastWrite<uint8_t>(p2 + active->offWallHack, 0);
            m_origAtk = -1.0f;
        }
    }
};
