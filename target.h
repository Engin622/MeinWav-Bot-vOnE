// target.h (GITHUB ORIGINAL REBORN)
#pragma once
#include "scanner.h"
#include "server_manager.h"

/**
 * @brief TargetSystem - GitHub Orijinal SǬrǬmǬ
 */
class TargetSystem {
public:
    uint32_t m_lockedVid = 0;      // Combat / Manual
    uint32_t m_farmTargetVid = 0;  // Auto-Farm (0x50 Logic)
    bool     m_autoAttack = false; 
    bool     m_autoFarmEnabled = false;
    bool     m_filterMetin = true;
    bool     m_filterBoss = true;
    bool     m_filterMob = true;
    int      m_prioMetin = 1;
    int      m_prioBoss = 2;
    int      m_prioMob = 3;
    uint32_t m_lastAtkTick = 0;   
    
    // --- SEPARATE FILTERS FOR MOB LOCK ---
    bool     m_mobLockEnabled = false; 
    bool     m_mobLockFilterMetin = false;
    bool     m_mobLockFilterBoss = false;
    bool     m_mobLockFilterMob = true;
    int      m_mobLockPrioMetin = 1;
    int      m_mobLockPrioBoss = 2;
    int      m_mobLockPrioMob = 3;

    const float MULTI_ATTACK_RANGE = 1200.0f; 

    void SendAttack(uint32_t targetVid, Scanner& scanner) {
        if (targetVid == 0) return;
        const auto* profile = g_serverMgr.GetActive();
        if (!profile) return;

        __try {
            // Write Target VID to selected base for visual lock
            uintptr_t basePtr = scanner.SafeRead<uintptr_t>(scanner.m_base + profile->selectedBase);
            if (scanner.IsValidPtr(basePtr)) {
                scanner.FastWrite<uint32_t>(basePtr + profile->selectedVidOff, targetVid);
            }

            uintptr_t NetPointer = scanner.SafeRead<uintptr_t>(scanner.m_base + profile->netPointer); 
            uintptr_t BattleCall = profile->battleCall;

            if (NetPointer && BattleCall) {
                __asm {
                    mov ecx, NetPointer    
                    push targetVid          
                    push 0                 
                    call BattleCall        
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    // Boss VNUM Database from USER
    bool IsBoss(uint32_t vnum) {
        static const std::unordered_set<uint32_t> bossVnums = {
            691, 791, 5163, 2091, 2092, 2093, 2094, 2191, 2192, 2206, 2207, 1901, 1902,
            1191, 1192, 1091, 1092, 1093, 1094, 1095, 2306, 2491, 2492, 2490, 2591, 2598,
            2493, 2597, 3690, 3590, 3390, 3090, 3290, 3190, 3191, 3595, 3890, 3790, 3705,
            3491, 3791, 3801, 5161
        };
        return bossVnums.count(vnum) > 0;
    }

    void Update(Scanner& scanner) {
        const auto* profile = g_serverMgr.GetActive();
        if (!profile) return;

        // --- 1. AUTO FARM LOGIC ---
        if (m_autoFarmEnabled) {
            bool farmTargetStillAlive = false;
            if (m_farmTargetVid != 0) {
                for (const auto& e : scanner.m_list) {
                    if (e.vid == m_farmTargetVid) { farmTargetStillAlive = true; break; }
                }
            }

            if (!farmTargetStillAlive) {
                m_farmTargetVid = 0;
                struct TempTarget { uint32_t vid; int prio; float dist; };
                std::vector<TempTarget> candidates;
                for (const auto& e : scanner.m_list) {
                    int p = -1;
                    if (e.type == 2 && m_filterMetin) p = m_prioMetin;
                    else if (IsBoss(e.vnum) && m_filterBoss) p = m_prioBoss;
                    else if (e.type == 0 && m_filterMob) p = m_prioMob;
                    if (p != -1) candidates.push_back({e.vid, p, e.dist});
                }
                if (!candidates.empty()) {
                    std::sort(candidates.begin(), candidates.end(), [](const TempTarget& a, const TempTarget& b) {
                        if (a.prio != b.prio) return a.prio < b.prio;
                        return a.dist < b.dist;
                    });
                    m_farmTargetVid = candidates[0].vid;
                }
            }

            if (m_farmTargetVid != 0) {
                uintptr_t p1 = scanner.SafeRead<uintptr_t>(scanner.m_base + profile->farmBase);
                if (scanner.IsValidPtr(p1)) {
                    // Standard CPythonPlayer + 0x50/0x0040E80 Logic
                    scanner.FastWrite<uint32_t>(p1 + profile->farmVidOff, m_farmTargetVid);
                }
            }
        }

        // --- 2. PRO DAMAGE LOGIC ---
        if (m_autoAttack) {
            uint32_t now = GetTickCount();
            if (now - m_lastAtkTick >= 150) { // ElyM2 Optimized timing
                m_lastAtkTick = now;
                for (const auto& e : scanner.m_list) {
                    bool matches = false;
                    if (e.type == 2 && m_filterMetin) matches = true;
                    else if (IsBoss(e.vnum) && m_filterBoss) matches = true;
                    else if (e.type == 0 && m_filterMob) matches = true;

                    if (matches && e.dist < MULTI_ATTACK_RANGE) {
                        SendAttack(e.vid, scanner);
                    }
                }
            }
        }

        // --- 3. MOB LOCK LOGIC (Magnet) ---
        if (m_mobLockEnabled) {
            PlayerPos myPos = scanner.GetPlayerPos();
            if (myPos.x != 0 && myPos.y != 0) {
                for (const auto& e : scanner.m_list) {
                    if (e.address == 0) continue;

                    bool matches = false;
                    if (e.type == 2 && m_mobLockFilterMetin) matches = true;
                    else if (IsBoss(e.vnum) && m_mobLockFilterBoss) matches = true;
                    else if (e.type == 0 && m_mobLockFilterMob) matches = true;

                    if (matches && e.dist < 2500.0f) {
                        scanner.FastWrite<float>(e.address + 0x64C, myPos.x);
                        scanner.FastWrite<float>(e.address + 0x650, myPos.y);
                        scanner.FastWrite<float>(e.address + 0x654, myPos.z);
                        scanner.FastWrite<uint32_t>(e.address + 0x628, 0); // Force Alive visual
                    }
                }
            }
        }
    }

    void Clear() {
        m_lockedVid = 0;
        m_autoAttack = false;
    }
};

extern TargetSystem g_targetSys;
