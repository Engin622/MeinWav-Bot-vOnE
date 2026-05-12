#pragma once
#include <map>
#include <string>
#include <vector>

#ifdef va_copy
#undef va_copy
#endif

#include "imgui/imgui.h"

class Scanner;

struct Theme {
    std::string Name;
    ImVec4 Accent;
    ImVec4 WindowBG;
    ImVec4 ChildBG;
    ImVec4 FrameBG;
    ImVec4 Button;
    ImVec4 ButtonHover;
};

class UIManager {
public:
    void Init(Scanner* scanner);
    void Render();
    void ApplyBotStyle();
    void InitThemes();
    
    void SaveConfig();
    void LoadConfig();
    
    bool ToggleSwitch(const char* label, bool* v);
    void RenderRadar();
    void LoadLanguage(int index);
    void HelpMarker(const char* desc, const char* title);

    bool m_showMenu = true;
    bool m_showServerSelector = true; // Splash Screen for server selection
    char m_langSearch[64] = "";       // Language search buffer
    int m_currentTab = 0;
    int m_selectedTheme = 0;
    int m_selectedLang = 0;
    
    Scanner* m_scanner = nullptr;
    std::vector<Theme> m_themes;
    std::map<std::string, float> m_animState;
};

extern UIManager g_ui;
