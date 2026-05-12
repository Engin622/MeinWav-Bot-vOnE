#include "pch.h"
#include "ui_manager.h"
#include "scanner.h"
#include "target.h"
#include "imgui/imgui_internal.h"
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

#ifdef va_copy
#undef va_copy
#endif

UIManager g_ui;
extern TargetSystem g_targetSys;

struct Translation {
    std::string LangName;
    std::string Dashboard, Combat, Radar, Info, Settings, MasterSwitch, Wallhack, ProDamage, Speed, AutoFarm, Save, Lang, MobLock;
    std::string DescProDamage, DescAutoFarm, DescMobLock, DescWallhack;
    std::string SkillRadius, WeaponLen, SkillRadiusDesc, WeaponLenDesc;
};

std::map<int, Translation> dict = {
    {0, {"T\xC3\x9CRK\xC3\x87\x45", "ANA SAYFA", "SAVA\xC5\x9E", "RADAR", "B\xC4\xB0LG\xC4\xB0", "AYARLAR", "ANA \xC5\x9E\x41LTER", "WALLHACK", "HIZLI HASAR", "HIZ AYARLARI", "OTOMAT\xC4\xB0K AV", "AYARLARI KAYDET", "D\xC4\xB0L SE\xC3\x87\xC4\xB0M\xC4\xB0", "MOB K\xC4\xB0L\xC4\xB0TLE", "Otomatik y\xC3\xbcksek h\xC4\xb1zda sald\xC4\xb1r\xC4\xb1.", "Otomatik Metin ve Boss keser.", "Moblar\xC4\xb1 " "\xC3\xbc" "zerine " "\xC3\xa7" "eker.", "Engellerden ge\xC3\xa7" "menizi sa\xC4\x9f" "lar.", "BECER\xC4\xB0 MENZ\xC4\xB0L\xC4\xB0", "S\xC4\xB0L\x41H UZUNLU\xC4\x9EU", "Yeteneklerin vuru\xC5\x9f alan\xC4\xb1n\xC4\xb1 art\xC4\xb1r\xC4\xb1r.", "Silah\xC4\xb1n\xC4\xb1z\xC4\xb1n ula\xC5\x9f" " " "abilece\xC4\x9f" "i mesafeyi art\xC4\xb1r\xC4\xb1r."}},
    {1, {"ENGLISH", "DASHBOARD", "COMBAT", "RADAR", "INFO", "SETTINGS", "MASTER SWITCH", "WALLHACK", "PRO DAMAGE", "SPEED MODS", "AUTO FARM", "SAVE CONFIG", "LANGUAGE", "MOB LOCK", "Automatic high-speed attacks.", "Auto Metin and Boss farming.", "Pulls mobs to your position.", "Allows walking through walls.", "SKILL RADIUS", "WEAPON LENGTH", "Increases the hit area of your skills.", "Increases the reach of your weapon."}},
    {2, {"DEUTSCH", "ÜBERSICHT", "KAMPF", "RADAR", "INFO", "EINSTELLUNGEN", "HAUPTSCHALTER", "WALLHACK", "PRO SCHADEN", "GESCHWINDIGKEIT", "AUTO FARM", "SPEICHERN", "SPRACHE", "MOB-SPERRE", "Automatische Angriffe.", "Auto Metin und Boss Farm.", "Zieht Monster zu dir.", "Durch Wände gehen.", "SKILL-RADIUS", "WAFFENLÄNGE", "Erhöht den Trefferbereich.", "Erhöht die Reichweite."}},
    {3, {"FRANÇAIS", "TABLEAU DE BORD", "COMBAT", "RADAR", "INFO", "REGLAGES", "INTERRUPTEUR", "WALLHACK", "DEGATS PRO", "VITESSE", "AUTO FARM", "SAUVEGARDER", "LANGUE", "VERROU MOB", "Attaques automatiques.", "Ferme auto Metin et Boss.", "Attire les monstres.", "Marcher à travers les murs.", "RAYON DE COMPÉTENCE", "LONGUEUR D'ARME", "Augmente la zone d'impact.", "Augmente la portée."}},
    {4, {"ESPAÑOL", "TABLERO", "COMBATE", "RADAR", "INFO", "AJUSTES", "INTERRUPTOR", "WALLHACK", "DAÑO PRO", "VELOCIDAD", "AUTO FARM", "GUARDAR", "IDIOMA", "BLOQUEO MOB", "Ataques automáticos.", "Granja auto Metin y Boss.", "Atrae monstruos.", "Caminar por las paredes.", "RADIO DE HABILIDAD", "LONGITUD DE ARMA", "Aumenta el área de impacto.", "Aumenta el alcance."}},
    {5, {"ROMÂNĂ", "TABLOU BORD", "LUPTA", "RADAR", "INFO", "SETARI", "COMUTATOR", "WALLHACK", "DAUNE PRO", "VITEZA", "AUTO FARM", "SALVARE", "LIMBA", "BLOCARE MOB", "Atacuri automate.", "Auto Metin și Boss.", "Atrage monștri.", "Trece prin pereți.", "RAZA ABILITĂȚII", "LUNGIMEA ARMEI", "Crește zona de impact.", "Crește raza."}},
    {6, {"POLSKI", "PANEL", "WALKA", "RADAR", "INFO", "USTAWIENIA", "GŁÓWNY", "WALLHACK", "PRO DAMAGE", "PRĘDKOŚĆ", "AUTO FARM", "ZAPISZ", "JĘZYK", "BLOKADA MOB", "Automatyczne ataki.", "Auto Metin i Boss.", "Przyciąga potwory.", "Chodzenie przez ściany.", "PROMIEŃ SKILLI", "DŁUGOŚĆ BRONI", "Zwiększa obszar ataku.", "Zwiększa zasięg."}},
    {7, {"ITALIANO", "CRUSCOTTO", "COMBATTIMENTO", "RADAR", "INFO", "IMPOSTAZIONI", "INTERRUTTORE", "WALLHACK", "DANNO PRO", "VELOCITÀ", "AUTO FARM", "SALVA", "LINGUA", "BLOCCO MOB", "Attacchi automatici.", "Farma auto Metin e Boss.", "Attira i mostri.", "Cammina attraverso i muri.", "RAGGIO ABILITÀ", "LUNGHEZZA ARMA", "Aumenta l'area di colpo.", "Aumenta la portata."}},
    {8, {"РУССКИЙ", "ГЛАВНАЯ", "БОЙ", "РАДАР", "ИНФО", "НАСТРОЙКИ", "ГЛАВНЫЙ", "WALLHACK", "PRO УРОН", "СКОРОСТЬ", "АВТО-ФЕРМА", "СОХРАНИТЬ", "ЯЗЫК", "МОБ ЛОК", "Автоматические атаки.", "Авто фарм Метинов и Боссов.", "Притягивает мобов.", "Хождение сквозь стены.", "РАДИУС НАВЫКА", "ДЛИНА ОРУЖИЯ", "Увеличивает зону поражения.", "Увеличивает охват."}},
    {9, {"한국어", "대시보드", "전투", "레이더", "정보", "설정", "메인 스위치", "월핵", "프로 데미지", "속도 설정", "자동 팜", "저장", "언어", "몹 잠금", "자동 고속 공격.", "자동 메틴 및 보스 파밍.", "몹을 당신의 위치로 끌어당깁니다.", "벽 통과 기능.", "스킬 범위", "무기 길이", "스킬 영역 증가.", "무기 범위 증가."}},
    {10, {"AZƏRBAYCAN", "ANA SƏHİFƏ", "DÖYÜŞ", "RADAR", "MƏLUMAT", "AYARLAR", "ANA ŞALTER", "WALLHACK", "PRO HASAR", "SÜRƏT", "AUTO FARM", "YADDA SAXLA", "DİL SEÇİMİ", "MOB KİLİDİ", "Avtomatik yüksək sürətli hücum.", "Avtomatik Metin və Boss kəsir.", "Mobları üzərinə çəkir.", "Maneələrdən keçməyə imkan verir.", "BACARIQ RADIUSU", "SİLAH UZUNLUĞU", "Vuruş sahəsini artırır.", "Silahın məsafəsini artırır."}},
    {11, {"ΕΛΛΗΝΙΚΑ", "ΠΙΝΑΚΑΣ", "ΜΑΧΗ", "ΡΑΝΤΑΡ", "ΠΛΗΡΟΦΟΡΙΕΣ", "ΡΥΘΜΙΣΕΙΣ", "ΔΙΑΚΟΠΤΗΣ", "WALLHACK", "PRO ΖΗΜΙΑ", "ΤΑΧΥΤΗΤΑ", "AUTO FARM", "ΑΠΟΘΗΚΕΥΣΗ", "ΓΛΩΣΣΑ", "ΚΛΕΙΔΩΜΑ", "Αυτόματες επιθέσεις.", "Αυτόματο φαρμάρισμα.", "Έλκει τα τέρατα.", "Περνά μέσα από τοίχους.", "ΑΚΤΙΝΑ ΔΕΞΙΟΤΗΤΩΝ", "ΜΗΚΟΣ ΟΠΛΟΥ", "Αυξάνει την περιοχή.", "Αυξάνει την εμβέλεια."}},
    {12, {"العربية", "لوحة التحكم", "قتال", "رادار", "معلومات", "إعدادات", "مفتاح", "WALLHACK", "ضرر برو", "سرعة", "مزارع آلي", "حفظ", "لغة", "قفل موب", "هجمات تلقائية سريعة.", "زراعة ميتين وزعيم آلي.", "يسحب الوحوش إليك.", "المشي عبر الجدران.", "نطاق المهارة", "طول السلاح", "يزيد مساحة المهارة.", "يزيد مدى السلاح."}},
    {13, {"فارسی", "داشبورد", "مبارزه", "رادار", "اطلاعات", "تنظیمات", "سوئیچ", "WALLHACK", "آسیب برو", "سرعت", "فارم خودکار", "ذخیره", "زبان", "قفل موب", "حملات سریع خودکار.", "فارم خودکار متین و باس.", "هیولاها را جذب می کند.", "راه رفتن از دیوار.", "شعاع مهارت", "طول سلاح", "منطقه مهارت را افزایش می دهد.", "برد سلاح را افزایش می دهد."}},
    {14, {"हिन्दी", "डैशबोर्ड", "लड़ाई", "रडार", "जानकारी", "सेटिंग्स", "स्विच", "WALLHACK", "प्रो क्षति", "गति", "ऑटो फार्म", "सहेजें", "भाषा", "मोब लॉक", "स्वचालित हमले।", "मेटिन और बॉस फार्म।", "भीड़ को खींचता है।", "दीवारों से पार। ", "कौशल त्रिज्या", "हथियार की लंबाई", "हिट क्षेत्र बढ़ाता है।", "पहुंच बढ़ाता है।"}},
    {15, {"KURDÎ", "PANELE", "ŞER", "RADAR", "ZANYARÎ", "MÎSENG", "SÛÎÇ", "WALLHACK", "PRO HASAR", "LEZ", "AUTO FARM", "TOMAR BIKE", "ZIMAN", "MOB LOCK", "Êrîşên otomatîk.", "Auto Metin û Boss.", "Moban dikişîne.", "Di dîwar de derbas bi dîwar.", "RADYUSA SKILL", "DIRÊJIYA ÇEK", "Qada lêdanê zêde dike.", "Rêjeya çekê zêde dike."}},
    {16, {"ČEŠTINA", "PANELE", "BOJ", "RADAR", "INFO", "NASTAVENÍ", "SPÍNAČ", "WALLHACK", "PRO POŠKOZENÍ", "RYCHLOST", "AUTO FARM", "ULOŽIT", "JAZYK", "ZÁMEK MOB", "Automatické útoky.", "Auto Metin a Boss farm.", "Přitahuje monstra.", "Procházení zdmi.", "RADIUS DOVEDNOSTÍ", "DÉLKA ZBRANĚ", "Zvyšuje oblast zásahu.", "Zvyšuje dosah."}},
    {17, {"ZAZAKI", "PANEL", "CENG", "RADAR", "MALUMAT", "AYARİ", "SÜİÇO SERİ", "WALLHACK", "PRO HASAR", "LEZ", "AUTO FARM", "QEYD BIKE", "ZIWAN", "MOB LOCK", "Êrîşo otomatîk.", "Auto Metin û Boss.", "Moban dikişêno.", "Dêwar de vêreno.", "RADYUSÊ SKILL", "DİRÊJEYA ÇEK", "Caye pırodayışi keno zêde.", "Mesafeyê çek keno zêde."}}
};

void UIManager::SaveConfig() {
    std::ofstream f("elite_config.ini");
    f << m_selectedLang << "\n" << m_selectedTheme << "\n" << m_scanner->m_atkSpeedVal << "\n" << m_scanner->m_moveSpeedVal << "\n"
      << g_targetSys.m_filterMetin << "\n" << g_targetSys.m_filterBoss << "\n" << g_targetSys.m_filterMob << "\n"
      << g_targetSys.m_prioMetin << "\n" << g_targetSys.m_prioBoss << "\n" << g_targetSys.m_prioMob << "\n"
      << g_targetSys.m_mobLockEnabled << "\n"
      << g_targetSys.m_mobLockFilterMetin << "\n" << g_targetSys.m_mobLockFilterBoss << "\n" << g_targetSys.m_mobLockFilterMob << "\n"
      << g_targetSys.m_mobLockPrioMetin << "\n" << g_targetSys.m_mobLockPrioBoss << "\n" << g_targetSys.m_mobLockPrioMob << "\n"
      << m_scanner->m_skillRadiusVal << "\n" << m_scanner->m_weaponLenVal;
    f.close();
}

void UIManager::LoadConfig() {
    std::ifstream f("elite_config.ini");
    if (f.is_open()) {
        f >> m_selectedLang >> m_selectedTheme >> m_scanner->m_atkSpeedVal >> m_scanner->m_moveSpeedVal
          >> g_targetSys.m_filterMetin >> g_targetSys.m_filterBoss >> g_targetSys.m_filterMob
          >> g_targetSys.m_prioMetin >> g_targetSys.m_prioBoss >> g_targetSys.m_prioMob
          >> g_targetSys.m_mobLockEnabled
          >> g_targetSys.m_mobLockFilterMetin >> g_targetSys.m_mobLockFilterBoss >> g_targetSys.m_mobLockFilterMob
          >> g_targetSys.m_mobLockPrioMetin >> g_targetSys.m_mobLockPrioBoss >> g_targetSys.m_mobLockPrioMob
          >> m_scanner->m_skillRadiusVal >> m_scanner->m_weaponLenVal;
        f.close();
    }
}

void UIManager::Init(Scanner* scanner) {
    m_scanner = scanner;
    InitThemes();
    LoadConfig();
    ApplyBotStyle();
}

void UIManager::InitThemes() {
    m_themes.clear();
    m_themes.push_back({"ELITE ABYSSAL", ImVec4(0.00f, 0.86f, 0.91f, 1.00f), ImVec4(0.04f, 0.04f, 0.06f, 1.00f), ImVec4(0.06f, 0.06f, 0.08f, 1.00f), ImVec4(0.08f, 0.08f, 0.12f, 1.00f), ImVec4(0.00f, 0.50f, 0.55f, 0.60f)});
    m_themes.push_back({"BLOOD MOON", ImVec4(1.00f, 0.20f, 0.20f, 1.00f), ImVec4(0.05f, 0.01f, 0.01f, 1.00f), ImVec4(0.08f, 0.02f, 0.02f, 1.00f), ImVec4(0.12f, 0.03f, 0.03f, 1.00f), ImVec4(0.60f, 0.00f, 0.00f, 0.60f)});
}

void UIManager::ApplyBotStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    auto& t = m_themes[m_selectedTheme];
    ImVec4* colors = style.Colors;
    style.WindowRounding = 16.0f; style.FrameRounding = 8.0f;
    colors[ImGuiCol_WindowBg] = ImVec4(t.WindowBG.x, t.WindowBG.y, t.WindowBG.z, 0.98f);
    colors[ImGuiCol_ChildBg] = t.ChildBG;
    colors[ImGuiCol_Border] = ImVec4(t.Accent.x, t.Accent.y, t.Accent.z, 0.20f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.10f, 1.00f);
    colors[ImGuiCol_Button] = t.Button;
    colors[ImGuiCol_ButtonHovered] = t.ButtonHover;
    colors[ImGuiCol_ButtonActive] = t.Accent;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.98f, 1.00f);
}

bool UIManager::ToggleSwitch(const char* label, bool* v) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float height = 24.0f, width = 48.0f;
    ImGui::InvisibleButton(label, ImVec2(width, height));
    if (ImGui::IsItemClicked(0)) *v = !(*v);
    if (ImGui::IsItemClicked(1)) ImGui::OpenPopup(label);
    
    float& anim = m_animState[label];
    float target = *v ? 1.0f : 0.0f;
    anim += (target - anim) * 0.15f;

    ImU32 bg_col = ImGui::ColorConvertFloat4ToU32(ImLerp(ImVec4(0.15f, 0.15f, 0.18f, 1.0f), m_themes[m_selectedTheme].Accent, anim));
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), bg_col, 12.0f);
    draw_list->AddCircleFilled(ImVec2(p.x + 12.0f + anim * 24.0f, p.y + 12.0f), 9.0f, IM_COL32_WHITE);
    
    ImGui::SameLine(); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    ImGui::Text("%s", label);
    return *v;
}

void UIManager::RenderRadar() {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 40.0f);
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float size = 320.0f, center = size / 2.0f;
    ImVec2 cp = ImVec2(p.x + center, p.y + center);
    dl->AddCircleFilled(cp, center, IM_COL32(5, 5, 10, 255));
    dl->AddCircle(cp, center, ImGui::GetColorU32(m_themes[m_selectedTheme].Accent), 64, 2.0f);
    if (m_scanner) {
        auto myPos = m_scanner->GetPlayerPos();
        for (const auto& e : m_scanner->m_list) {
            float dx = (e.x - myPos.x) / 18.0f, dy = (e.y - myPos.y) / 18.0f;
            if (sqrtf(dx*dx + dy*dy) < center) {
                ImU32 col = IM_COL32_WHITE;
                if (e.type == 0) col = IM_COL32(255, 0, 0, 255);
                else if (e.type == 1) col = IM_COL32(200, 0, 255, 255);
                else if (e.type == 2) col = IM_COL32(0, 255, 0, 255);
                else if (e.type == 6) col = IM_COL32(255, 255, 0, 255);
                dl->AddCircleFilled(ImVec2(cp.x + dx, cp.y - dy), 5.0f, col);
            }
        }
    }
    dl->AddCircleFilled(cp, 7.0f, ImGui::GetColorU32(m_themes[m_selectedTheme].Accent));
    ImGui::Dummy(ImVec2(size, size));
}

void UIManager::HelpMarker(const char* desc, const char* title) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.10f, 0.50f, 1.00f, 1.00f), "(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        if (title) ImGui::TextColored(ImVec4(0.10f, 0.50f, 1.00f, 1.00f), "%s", title);
        ImGui::TextColored(ImVec4(1.00f, 1.00f, 0.00f, 1.00f), "%s", desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void UIManager::Render() {
    if (!m_showMenu) return;
    
    if (m_showServerSelector) {
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::Begin("SELECT SERVER PROFILE", &m_showServerSelector, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::SetCursorPos(ImVec2(100, 40));
        ImGui::TextColored(m_themes[m_selectedTheme].Accent, "ELITE SERVER SELECTOR");
        ImGui::Separator(); ImGui::Spacing();
        if (g_serverMgr.profiles.empty()) g_serverMgr.Init();
        for (int i = 0; i < (int)g_serverMgr.profiles.size(); i++) {
            if (ImGui::Button(g_serverMgr.profiles[i].name.c_str(), ImVec2(-1, 50))) {
                g_serverMgr.selectedIndex = i;
                m_showServerSelector = false;
            }
            ImGui::Spacing();
        }
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40);
        ImGui::TextDisabled("Select a profile to load specific offsets.");
        ImGui::End();
        return;
    }

    auto& lang = dict[m_selectedLang];
    ImGui::SetNextWindowSize(ImVec2(800, 550), ImGuiCond_FirstUseEver);
    ImGui::Begin("MEINWAV_ELITE_PRO", &m_showMenu, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
    
    ImVec2 p = ImGui::GetWindowPos(); float sw = 180.0f; auto acc = m_themes[m_selectedTheme].Accent;
    ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sw, p.y + 550), IM_COL32(10, 10, 15, 255), 16.0f, ImDrawFlags_RoundCornersLeft);
    
    ImGui::SetCursorPos(ImVec2(25, 45)); 
    ImGui::TextColored(ImVec4(0.10f, 0.50f, 1.00f, 1.00f), "MEINWAV"); ImGui::SameLine(); 
    ImGui::TextColored(ImVec4(1.00f, 1.00f, 0.00f, 1.00f), " ELITE");
    
    ImGui::SetCursorPos(ImVec2(25, 75));
    ImGui::TextDisabled("[%s]", lang.LangName.c_str());

    const char* tabs[] = { lang.Dashboard.c_str(), lang.Combat.c_str(), lang.Radar.c_str(), lang.Info.c_str(), lang.Settings.c_str() };
    for (int i = 0; i < 5; i++) {
        ImGui::SetCursorPosX(15); 
        ImGui::SetCursorPosY(140.0f + (i * 60));
        bool is_active = (m_currentTab == i);
        ImVec2 label_pos = ImGui::GetCursorScreenPos();
        if (is_active) {
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                ImVec2(p.x, label_pos.y - 10), ImVec2(p.x + sw - 10, label_pos.y + 35),
                ImGui::ColorConvertFloat4ToU32(ImVec4(acc.x, acc.y, acc.z, 0.15f)), 
                IM_COL32(0,0,0,0), IM_COL32(0,0,0,0),
                ImGui::ColorConvertFloat4ToU32(ImVec4(acc.x, acc.y, acc.z, 0.15f))
            );
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, label_pos.y - 10), ImVec2(p.x + 4, label_pos.y + 35), ImGui::ColorConvertFloat4ToU32(acc), 2.0f);
        }
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(acc.x, acc.y, acc.z, 0.05f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0,0,0,0));
        if (ImGui::Selectable(tabs[i], is_active, 0, ImVec2(sw - 30, 40))) m_currentTab = i;
        ImGui::PopStyleColor(3);
    }

    ImGui::SetCursorPos(ImVec2(sw + 25, 25)); ImGui::BeginGroup();
    ImGui::TextColored(acc, "Elite vOnE"); ImGui::Separator(); ImGui::Spacing();

    if (m_currentTab == 0) {
        ImGui::BeginChild("TableArea", ImVec2(0, 320), true);
        if (ImGui::BeginTable("T1", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
            ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("TYPE", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("VNUM", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("DIST", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("LV", ImGuiTableColumnFlags_WidthFixed, 35);
            ImGui::TableSetupColumn("VID", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableHeadersRow();
            for (auto& e : m_scanner->m_list) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                bool isSelected = (g_targetSys.m_lockedVid == e.vid);
                if (ImGui::Selectable(e.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    g_targetSys.m_lockedVid = e.vid;
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", (e.type == 0 ? "MOB" : e.type == 1 ? "NPC" : e.type == 2 ? "METIN" : "PLR"));
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", e.vnum);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", e.dist);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d", e.level);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%X", e.vid);
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        if (ImGui::Button(g_targetSys.m_autoFarmEnabled ? "FARM: ON" : lang.AutoFarm.c_str(), ImVec2(-1, 50))) g_targetSys.m_autoFarmEnabled = !g_targetSys.m_autoFarmEnabled;
    }
    else if (m_currentTab == 1) {
        ToggleSwitch(lang.MasterSwitch.c_str(), &m_scanner->m_hacksEnabled);
        ImGui::Spacing();
        ToggleSwitch(lang.ProDamage.c_str(), &g_targetSys.m_autoAttack);
        HelpMarker(lang.DescProDamage.c_str(), lang.ProDamage.c_str());
        ImGui::Spacing();
        ToggleSwitch(lang.AutoFarm.c_str(), &g_targetSys.m_autoFarmEnabled);
        HelpMarker(lang.DescAutoFarm.c_str(), lang.AutoFarm.c_str());
        if (ImGui::BeginPopup(lang.AutoFarm.c_str())) {
            ImGui::TextColored(ImVec4(0.10f, 0.50f, 1.00f, 1.00f), m_selectedLang == 0 ? "OTOMAT\xC4\xB0K AV AYARLARI" : "AUTO FARM SETTINGS");
            ImGui::Separator();
            ImGui::Checkbox(m_selectedLang == 0 ? "MET\xC4\xB0N" : "METIN", &g_targetSys.m_filterMetin);
            ImGui::Checkbox("BOSS", &g_targetSys.m_filterBoss);
            ImGui::Checkbox("MOB", &g_targetSys.m_filterMob);
            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ToggleSwitch(lang.MobLock.c_str(), &g_targetSys.m_mobLockEnabled);
        HelpMarker(lang.DescMobLock.c_str(), lang.MobLock.c_str());
        if (ImGui::BeginPopup(lang.MobLock.c_str())) {
            ImGui::TextColored(ImVec4(0.10f, 0.50f, 1.00f, 1.00f), m_selectedLang == 0 ? "MOB K\xC4\xB0L\xC4\xB0TLEME AYARLARI" : "MOB LOCK SETTINGS");
            ImGui::Separator();
            ImGui::Checkbox(m_selectedLang == 0 ? "MET\xC4\xB0N" : "METIN", &g_targetSys.m_mobLockFilterMetin);
            ImGui::Checkbox("BOSS", &g_targetSys.m_mobLockFilterBoss);
            ImGui::Checkbox("MOB", &g_targetSys.m_mobLockFilterMob);
            ImGui::EndPopup();
        }
        ImGui::Spacing();
        if (g_targetSys.m_autoFarmEnabled || g_targetSys.m_mobLockEnabled) {
            ImGui::Indent();
            ImGui::PushItemWidth(100);
            ImGui::Checkbox(m_selectedLang == 0 ? "MET\xC4\xB0N" : "METIN", &g_targetSys.m_filterMetin); ImGui::SameLine(180); ImGui::SliderInt("Prio##1", &g_targetSys.m_prioMetin, 1, 3);
            ImGui::Checkbox("BOSS ", &g_targetSys.m_filterBoss); ImGui::SameLine(180); ImGui::SliderInt("Prio##2", &g_targetSys.m_prioBoss, 1, 3);
            ImGui::Checkbox("MOB  ", &g_targetSys.m_filterMob);  ImGui::SameLine(180); ImGui::SliderInt("Prio##3", &g_targetSys.m_prioMob, 1, 3);
            ImGui::PopItemWidth();
            ImGui::Unindent();
        }
        ImGui::Spacing();
        ToggleSwitch(lang.Wallhack.c_str(), &m_scanner->m_wallhackEnabled);
        HelpMarker(lang.DescWallhack.c_str(), lang.Wallhack.c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("%s", lang.Speed.c_str());
        ImGui::SliderFloat("ATK", &m_scanner->m_atkSpeedVal, 1.0f, 10.0f);
        ImGui::SliderFloat("MOVE", &m_scanner->m_moveSpeedVal, 1.0f, 10.0f);
        ImGui::Spacing();
        ImGui::Text("%s", lang.SkillRadius.c_str());
        ImGui::SliderFloat("RAD##SR", &m_scanner->m_skillRadiusVal, 0.0f, 1000.0f);
        HelpMarker(lang.SkillRadiusDesc.c_str(), lang.SkillRadius.c_str());
        ImGui::Spacing();
        ImGui::Text("%s", lang.WeaponLen.c_str());
        ImGui::SliderFloat("LEN##WL", &m_scanner->m_weaponLenVal, 0.0f, 1000.0f);
        HelpMarker(lang.WeaponLenDesc.c_str(), lang.WeaponLen.c_str());
    }
    else if (m_currentTab == 2) { RenderRadar(); }
    else if (m_currentTab == 3) {
        ImGui::Text("Player: %s", m_scanner->GetPlayerName().c_str());
        ImGui::Text("Level: %d", m_scanner->GetPlayerLevel());
        auto pos = m_scanner->GetPlayerPos();
        ImGui::Text("Pos: %.1f, %.1f", pos.x, pos.y);
    }
    else if (m_currentTab == 4) {
        ImGui::Text("%s", lang.Lang.c_str());
        ImGui::InputText("##Search", m_langSearch, 64);
        ImGui::SameLine(); ImGui::TextDisabled("(Search)");
        if (ImGui::BeginChild("LangList", ImVec2(0, 180), true)) {
            for (auto const& [id, t] : dict) {
                std::string searchStr = m_langSearch;
                std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);
                std::string langName = t.LangName;
                std::transform(langName.begin(), langName.end(), langName.begin(), ::tolower);
                if (strlen(m_langSearch) > 0 && langName.find(searchStr) == std::string::npos) continue;
                if (ImGui::Selectable(t.LangName.c_str(), m_selectedLang == id)) {
                    m_selectedLang = id;
                }
            }
            ImGui::EndChild();
        }
        ImGui::Spacing();
        if (ImGui::Button(lang.Save.c_str(), ImVec2(-1, 40))) SaveConfig();
    }
    ImGui::EndGroup(); ImGui::End();
}

void UIManager::LoadLanguage(int index) {}
