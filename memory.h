// memory.h
// __try / __except bloklari C++ objesiyle (std::string vs.) ayni fonksiyonda
// bulunamaz. Bu yuzden saf C-style fonksiyonlara ayirdik.
#pragma once
#include <Windows.h>
#include <cstring>

// Ham bellek okuma - sadece POD tipler (int, float, ptr...)
// C++ objesi YOK, __try guvenli
template<typename T>
inline T MemRead(uintptr_t addr) {
    T val{};
    __try {
        val = *reinterpret_cast<const T*>(addr);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // Gecersiz adres - sifir dondur
    }
    return val;
}

template<typename T>
inline bool MemWrite(uintptr_t addr, const T& value) {
    __try {
        *reinterpret_cast<T*>(addr) = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

// String okuma - char buffer kullan, std::string YOK bu fonksiyonda
// Sonucu dis fonksiyon std::string'e cevirir
inline bool MemReadBuf(uintptr_t addr, char* outBuf, size_t maxLen) {
    __try {
        const char* src = reinterpret_cast<const char*>(addr);
        size_t i = 0;
        for (; i < maxLen - 1; i++) {
            outBuf[i] = src[i];
            if (src[i] == '\0') break;
        }
        outBuf[i] = '\0';
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        outBuf[0] = '\0';
        return false;
    }
}

// Adresin okunabilir olup olmadigini kontrol et
inline bool IsValidPtr(uintptr_t addr) {
    if (addr < 0x10000 || addr > 0x7FFFFFFF) return false;
    __try {
        volatile char c = *reinterpret_cast<const char*>(addr);
        (void)c;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}
