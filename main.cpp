// ================================================================
// CS2 Cheat Checker v3.3
// Author: Chmonya (14yo, не бейте сильно)
// Website: https://chmonya.ct.ws
//
// v3.3:
//   1. Прокачана проверка реестра (RecentDocs, TypedPaths, RunMRU, RDP, RecentApps)
//   2. Мелкие фиксы (счётчики, pipe, ссылки)
//   3. RAII-хэндлы, npos-чеки, binary-safe Aho-Corasick (из v3.2)
// ================================================================
#if defined(_MSC_VER) && _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#include <winhttp.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <winsvc.h>
#include <psapi.h>
#include <mmsystem.h>
#include <iostream>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <set>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <ctime>
#include <shlobj.h>
#include <knownfolders.h>
#include <comdef.h>
#include <wintrust.h>
#include <softpub.h>
#include <wininet.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_TEXTURE_WRAP_S
#define GL_TEXTURE_WRAP_S 0x2802
#endif
#ifndef GL_TEXTURE_WRAP_T
#define GL_TEXTURE_WRAP_T 0x2803
#endif
#ifndef GL_TEXTURE_MIN_FILTER
#define GL_TEXTURE_MIN_FILTER 0x2801
#endif
#ifndef GL_TEXTURE_MAG_FILTER
#define GL_TEXTURE_MAG_FILTER 0x2800
#endif
#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif
#ifndef GL_RGBA
#define GL_RGBA 0x1908
#endif
#ifndef GL_UNSIGNED_BYTE
#define GL_UNSIGNED_BYTE 0x1401
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"
#include <json.hpp>
#include <discord/discord_rpc.h>
#include <discord/discord_register.h>

#pragma comment(lib, "discord-rpc.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "winmm.lib")

using json = nlohmann::json;
namespace fs = std::filesystem;

// ===================== ИКОНКИ =====================
#ifndef ICON_FA_SHIELD_HALVED
#define ICON_FA_SHIELD_HALVED        "\xef\x8f\xad"
#endif
#ifndef ICON_FA_TRIANGLE_EXCLAMATION
#define ICON_FA_TRIANGLE_EXCLAMATION "\xef\x81\xb1"
#endif
#ifndef ICON_FA_CIRCLE_CHECK
#define ICON_FA_CIRCLE_CHECK         "\xef\x81\x98"
#endif
#ifndef ICON_FA_SPINNER
#define ICON_FA_SPINNER              "\xef\x84\x90"
#endif
#ifndef ICON_FA_CHECK
#define ICON_FA_CHECK                "\xef\x80\x8c"
#endif
#ifndef ICON_FA_FILE
#define ICON_FA_FILE                 "\xef\x85\x9b"
#endif
#ifndef ICON_FA_MICROCHIP
#define ICON_FA_MICROCHIP            "\xef\x8b\x9b"
#endif
#ifndef ICON_FA_WIFI
#define ICON_FA_WIFI                 "\xef\x87\xab"
#endif
#ifndef ICON_FA_EYE_SLASH
#define ICON_FA_EYE_SLASH            "\xef\x81\xb0"
#endif
#ifndef ICON_FA_FINGERPRINT
#define ICON_FA_FINGERPRINT          "\xef\x95\xb7"
#endif
#ifndef ICON_FA_GEARS
#define ICON_FA_GEARS                "\xef\x82\x85"
#endif
#ifndef ICON_FA_POWER_OFF
#define ICON_FA_POWER_OFF            "\xef\x80\x91"
#endif
#ifndef ICON_FA_BRAIN
#define ICON_FA_BRAIN                "\xef\x97\x9c"
#endif
#ifndef ICON_FA_FILE_EXPORT
#define ICON_FA_FILE_EXPORT          "\xef\x95\xae"
#endif
#ifndef ICON_FA_PLAY
#define ICON_FA_PLAY                 "\xef\x81\x8b"
#endif
#ifndef ICON_FA_STOP
#define ICON_FA_STOP                 "\xef\x81\x8d"
#endif
#ifndef ICON_FA_TRASH
#define ICON_FA_TRASH                "\xef\x87\xb8"
#endif
#ifndef ICON_FA_SAVE
#define ICON_FA_SAVE                 "\xef\x83\x87"
#endif
#ifndef ICON_FA_FILTER
#define ICON_FA_FILTER               "\xef\x82\xb0"
#endif
#ifndef ICON_FA_USER
#define ICON_FA_USER                 "\xef\x80\x87"
#endif
#ifndef ICON_FA_USER_CIRCLE
#define ICON_FA_USER_CIRCLE          "\xef\x8a\xbd"
#endif
#ifndef ICON_FA_BAN
#define ICON_FA_BAN                  "\xef\x81\x9e"
#endif
#ifndef ICON_FA_SEARCH
#define ICON_FA_SEARCH               "\xef\x80\x82"
#endif
#ifndef ICON_FA_CLOCK
#define ICON_FA_CLOCK                "\xef\x80\x97"
#endif
#ifndef ICON_FA_CIRCLE_INFO
#define ICON_FA_CIRCLE_INFO          "\xef\x81\x9a"
#endif
#ifndef ICON_FA_XMARK
#define ICON_FA_XMARK                "\xef\x80\x8d"
#endif
#ifndef ICON_FA_MINUS
#define ICON_FA_MINUS                "\xef\x81\xa8"
#endif
#ifndef ICON_FA_SQUARE
#define ICON_FA_SQUARE               "\xef\x83\x88"
#endif
#ifndef ICON_FA_DISCORD
#define ICON_FA_DISCORD              "\xef\x8e\x92"
#endif
#ifndef ICON_FA_TELEGRAM
#define ICON_FA_TELEGRAM             "\xef\x8b\x86"
#endif
#ifndef ICON_FA_VK
#define ICON_FA_VK                   "\xef\x86\x89"
#endif
#ifndef ICON_FA_FOLDER_OPEN
#define ICON_FA_FOLDER_OPEN          "\xef\x81\xbc"
#endif
#ifndef ICON_FA_FOLDER
#define ICON_FA_FOLDER               "\xef\x81\xbb"
#endif
#ifndef ICON_FA_HEART
#define ICON_FA_HEART                "\xef\x80\x84"
#endif
#ifndef ICON_FA_GHOST
#define ICON_FA_GHOST                "\xef\x9b\xa2"
#endif
#ifndef ICON_FA_SKULL
#define ICON_FA_SKULL                "\xef\x95\x8c"
#endif
#ifndef ICON_FA_CODE
#define ICON_FA_CODE                 "\xef\x84\xa1"
#endif
#ifndef ICON_FA_BOOK
#define ICON_FA_BOOK                 "\xef\x80\xad"
#endif
#ifndef ICON_FA_CROWN
#define ICON_FA_CROWN                "\xef\x94\xa1"
#endif
#ifndef ICON_FA_FIRE
#define ICON_FA_FIRE                 "\xef\x81\xad"
#endif
#ifndef ICON_FA_LINK
#define ICON_FA_LINK                 "\xef\x83\x81"
#endif
#ifndef ICON_FA_HDD
#define ICON_FA_HDD                  "\xef\x82\xa0"
#endif
#ifndef ICON_FA_HISTORY
#define ICON_FA_HISTORY              "\xef\x87\x9a"
#endif
#ifndef ICON_FA_SUN
#define ICON_FA_SUN                  "\xef\x86\x85"
#endif
#ifndef ICON_FA_MOON
#define ICON_FA_MOON                 "\xef\x86\x86"
#endif
#ifndef ICON_FA_GLOBE
#define ICON_FA_GLOBE                "\xef\x82\xac"
#endif
#ifndef ICON_FA_USB
#define ICON_FA_USB                  "\xef\x8a\x87"
#endif
#ifndef ICON_FA_DATABASE
#define ICON_FA_DATABASE             "\xef\x87\x80"
#endif
#ifndef ICON_FA_STEAM
#define ICON_FA_STEAM                "\xef\x86\xb6"
#endif
#ifndef ICON_FA_ROTATE_LEFT
#define ICON_FA_ROTATE_LEFT          "\xef\x83\xa2"
#endif
#ifndef ICON_FA_TERMINAL
#define ICON_FA_TERMINAL             "\xef\x84\xa0"
#endif
#ifndef ICON_FA_HOUSE
#define ICON_FA_HOUSE                "\xef\x80\x95"
#endif
#ifndef ICON_FA_LAYER_GROUP
#define ICON_FA_LAYER_GROUP          "\xef\x97\xbd"
#endif
#ifndef ICON_FA_MUG_HOT
#define ICON_FA_MUG_HOT              "\xef\x9e\xb6"
#endif
#ifndef ICON_FA_PIZZA_SLICE
#define ICON_FA_PIZZA_SLICE          "\xef\xa0\x98"
#endif
#ifndef ICON_FA_HEART_CRACK
#define ICON_FA_HEART_CRACK          "\xef\x9e\xa9"
#endif
#ifndef ICON_FA_BOMB
#define ICON_FA_BOMB                 "\xef\x87\xa2"
#endif
#ifndef ICON_FA_ROCKET
#define ICON_FA_ROCKET               "\xef\x84\xb5"
#endif
#ifndef ICON_FA_CAT
#define ICON_FA_CAT                  "\xef\x9a\xbe"
#endif
#ifndef ICON_FA_DRAGON
#define ICON_FA_DRAGON               "\xef\x9b\x95"
#endif

#ifndef ICON_MIN_FA
#define ICON_MIN_FA 0xe005
#endif
#ifndef ICON_MAX_FA
#define ICON_MAX_FA 0xf8ff
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ===================== КОНСТАНТЫ =====================
const std::string SETTINGS_FILE = "settings.json";
const std::string FONT_ICONS = "assets/fa-solid-900.ttf";
const std::string FONT_BRANDS = "assets/fa-brands-400.ttf";

const std::string WEBHOOK_URL = "https://discord.com/api/webhooks/YOUR_WEBHOOK";

// Ссылки автора
const char* MY_WEBSITE = "https://chmonya.ct.ws";
const char* MY_GITHUB = "https://github.com/ChmonyaStudio";
const char* MY_TELEGRAM = "https://t.me/p1zdabol4ik";
const char* MY_DISCORD = "https://discord.com/users/debiloidpub";

const char* HASH_DB_URL = "https://raw.githubusercontent.com/ChmonyaStudio/cs2-cheat-checker-by-chmonya/main/hashes.json";

const size_t SCAN_FILES_MAX = 50000;
const char* EASTER_EGG = "CHMONYA_2026_DEV_MODE";

// ===================== FORWARD DECLARATIONS =====================
std::string ToLower(std::string s);
std::string WStringToString(const std::wstring& wstr);
std::string HttpGet(const std::string& url);
std::string ExtractTag(const std::string& xml, const std::string& tag);
bool ContainsKeyword(const std::string& text);
bool ContainsKeywordRaw(const char* data, size_t len);
bool VerifyFileSignature(const std::wstring& filePath);
bool IsWhitelisted(const std::string& path);
std::string GetFolderPath(REFKNOWNFOLDERID rfid);
std::string GetFileSHA256(const std::wstring& path);
bool FileHasKeywordContent(const fs::path& p, size_t maxBytes);
void AddResult(const std::string& name, const std::string& path, const std::string& type,
    const std::string& reason, int severity);
void ClearResults();
void OpenInExplorer(std::string path);
void OpenURL(const std::string& url);
void PlayAlertSound();
void SendToDiscord(const std::string& msg);
void SetAutoStart(bool enable);
bool IsAutoStartEnabled();
bool IsRunningAsAdmin();
bool RelaunchAsAdmin();
void SaveSettings();
void LoadSettings();
void LoadHashDatabase();
void SaveScanToHistory(const std::string& tabName, size_t filesScanned, size_t found, const std::vector<json>& results);
void LoadHistoryList();
void InitFolders();
GLuint LoadTextureFromFile(const char* filename);

void RPC_Init();
void RPC_Shutdown();
void RPC_SetStatus(const std::string& details, const std::string& state);
void RPC_SetState(const std::string& state);
void RPC_OnScanStart(const char* scanName);
void RPC_OnScanFinish();
void RPC_OnTabSwitch(const char* tabName);

void ScanModulesInProcess(DWORD pid, const std::string& procName);
void ScanFiles();
void ScanProcesses();
void ScanNet();
void ScanTaskScheduler();
void ScanHidden();
void ScanSignatures();
void ScanServices();
void ScanAutorun();
void ScanMemory();
void ScanDrivers();
void ScanActivity();
void ScanUSBDevices();
void ScanSteamAccounts();
void ScanBrowserHistory();
void ScanBrowserStrings();
void ScanCS2Strings();
void ScanRegistryPaths();
void PerformFileSearch(const std::string& folder, const std::string& query);
void OpenRecoveryTool();
void RefreshToolsList();
void LaunchTool(const struct ToolEntry& tool);
void OpenToolFolder(const struct ToolEntry& tool);
void RefreshProcessList();
void AutoSelectCS2();
void ScanProcessStringsForProcess(DWORD pid, const std::string& processName,
    const std::vector<std::string>& patterns, size_t minLen, const std::string& category);

std::string FindSteamInstallPath();
void ParseLoginUsersVDF(const std::string& filePath);
std::string ExtractSteamID64FromInput(const std::string& input);
std::string ExtractVanityName(const std::string& input);
std::string ResolveVanityToSteamID64(const std::string& vanityName);
void CheckPlayerProfile(const std::string& rawInput);

void StartScan(int tab);
void StopScan();

void ApplyCSArenaStyle();
bool BigButton(const char* label, const ImVec2& size, bool accent);
void SectionTitle(const char* icon, const char* text, const char* desc);
void RenderProgressBar(float progress, const char* overlay);
void RenderScanControls(int tabId);
void RenderResultsTable(const char* tableId, std::vector<json>& copySource);
void RenderResultsByTypes(const char* tableId, std::initializer_list<const char*> types);

void RenderSplash(float dt);
void RenderTitlebar();
void RenderTopMenu();
void RenderSubMenu();
void RenderContent();
void RenderUI();
void RenderHomeTab();
void RenderSitesTab();
void RenderActivityTab();
void RenderUSBTab();
void RenderRegistryTab();
void RenderFileSearchTab();
void RenderSteamTab();
void RenderRecoveryTab();
void RenderProcessStringsTab();
void RenderToolsTab();
void RenderSystemFoldersTab();
void RenderReportTab();
void RenderProfileTab();
void RenderInfoTab();

void ExportReport();
void ExportHTMLReport();
void ExportCSVReport();
std::string FileToBase64(const std::string& path);

std::vector<struct SystemFolder> GetSystemFolders();

// ===================== СТРУКТУРЫ =====================
struct RegPath {
    std::string name;
    std::string description;
    HKEY        root;
    std::string subkey;
    bool        isFolder;
};

struct CheckSite {
    std::string name;
    std::string url;
    std::string note;
};

struct ToolEntry {
    std::string folderName;
    std::string exeName;
    std::string description;
    std::string category;
};

struct CSColors {
    static constexpr ImVec4 Primary = ImVec4(1.00f, 0.62f, 0.05f, 1.00f);
    static constexpr ImVec4 PrimaryDark = ImVec4(0.85f, 0.48f, 0.00f, 1.00f);
    static constexpr ImVec4 PrimarySoft = ImVec4(1.00f, 0.62f, 0.05f, 0.18f);
    static constexpr ImVec4 Secondary = ImVec4(0.22f, 0.65f, 0.98f, 1.00f);
    static constexpr ImVec4 PanelCard = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
    static constexpr ImVec4 PanelCardHover = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
    static constexpr ImVec4 BtnIdle = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    static constexpr ImVec4 BtnHover = ImVec4(1.00f, 1.00f, 1.00f, 0.12f);
    static constexpr ImVec4 BtnActive = ImVec4(1.00f, 0.62f, 0.05f, 0.30f);
    static constexpr ImVec4 FrameIdle = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
    static constexpr ImVec4 FrameHover = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    static constexpr ImVec4 BorderLight = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    static constexpr ImVec4 BorderSoft = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
    static constexpr ImVec4 Text = ImVec4(0.97f, 0.97f, 0.99f, 1.00f);
    static constexpr ImVec4 TextMuted = ImVec4(0.60f, 0.62f, 0.68f, 1.00f);
    static constexpr ImVec4 Success = ImVec4(0.30f, 0.88f, 0.50f, 1.00f);
    static constexpr ImVec4 Danger = ImVec4(0.98f, 0.38f, 0.38f, 1.00f);
    static constexpr ImVec4 Warning = ImVec4(1.00f, 0.78f, 0.28f, 1.00f);
    static constexpr ImVec4 Info = ImVec4(0.35f, 0.78f, 0.98f, 1.00f);
    static constexpr ImVec4 Bg = ImVec4(0.04f, 0.04f, 0.05f, 1.00f);
};

struct StatusString { const char* key; const char* en; const char* ru; };

struct LangPack {
    const char* Home; const char* Files; const char* Processes; const char* Network;
    const char* Activity; const char* USB; const char* Registry; const char* FileSearch;
    const char* SteamAccounts; const char* Recovery; const char* ProcessStrings; const char* Tools;
    const char* Drivers; const char* Autorun; const char* Services; const char* CheckSites;
    const char* SystemFolders;
    const char* Report; const char* Profile; const char* Info;
    const char* CatMain; const char* CatAnalysis; const char* CatAdvanced; const char* CatReport;
    const char* StartScan; const char* StopScan; const char* ClearResults;
    const char* ExportJSON; const char* ExportHTML; const char* ExportCSV;
    const char* ClearLog; const char* IgnoreList; const char* SaveIgnore; const char* ReloadIgnore;
    const char* Ready; const char* Scanning; const char* Admin; const char* NoAdmin;
    const char* Search; const char* SearchHint; const char* Status; const char* Scanned; const char* Hits;
    const char* TargetProcess; const char* RefreshList; const char* NoProcesses;
    const char* TotalFindings; const char* NoData;
    const char* Type; const char* Name; const char* Reason; const char* Path; const char* OpenFolder;
    const char* ProfileTitle; const char* SteamID; const char* CheckProfile;
    const char* Bans; const char* NoBans; const char* Checking; const char* EnterSteamID;
    const char* ProfileNotFound; const char* VACBan; const char* GameBan; const char* DaysSinceLastBan;
    const char* MemberSince; const char* Country; const char* RealName;
    const char* NoProfileData; const char* Clear;
    const char* ReportSaved; const char* ReportError; const char* NoResults;
    const char* AuthorTitle; const char* AuthorText;
    const char* HowToTitle; const char* HowToText;
    const char* SocialsTitle; const char* MemesTitle; const char* DeveloperTitle;
    const char* SettingsTitle; const char* SoundEnabled; const char* AutoStart;
    const char* HistoryEmpty; const char* HistoryClear; const char* HistoryOpen;
    const char* OpenAll; const char* OpenFolder2; const char* Launch;
    const char* CopyToClipboard; const char* Copied;
    const char* ActivityDesc; const char* USBDesc; const char* RegistryDesc;
    const char* FileSearchDesc; const char* SteamDesc; const char* RecoveryDesc;
    const char* ProcessStringsDesc; const char* ToolsDesc; const char* SitesDesc;
    const char* SystemFoldersDesc;
    const char* HomeWelcome; const char* HomeSubtitle;
    const char* StatTotal; const char* StatScans; const char* StatVersion; const char* StatStatus;
    const char* SearchFileByName; const char* SelectFolder; const char* FolderPath;
    const char* ScanFolder; const char* SteamAccountsFound; const char* NoSteamAccounts;
    const char* RunTool; const char* ToolNotFound; const char* OpenToolFolder;
    const char* AboutTitle; const char* AboutText;
};

struct ScanHistoryEntry {
    std::string timestamp;
    std::string tabName;
    size_t files = 0;
    size_t found = 0;
    std::string file;
};

struct SteamAccount {
    std::string personaName;
    std::string accountName;
    std::string steamID64;
    std::string lastLogin;
    bool        mostRecent = false;
};

struct RegistryFinding {
    std::string pathName;
    std::string valueName;
    std::string value;
    bool        suspicious = false;
};

struct SystemFolder {
    std::string name;
    std::string path;
    std::string icon;
};

struct ProcessStringHit {
    std::string processName;
    DWORD       pid = 0;
    std::string foundString;
    std::string category;
};

struct PlayerProfile {
    std::string steamID, playerName, realName, country, memberSince;
    int vacBans = 0, gameBans = 0, daysSince = 0;
    bool isLoading = false, hasData = false, isPrivate = false, error = false;
    std::string errorMsg;
};

struct MenuTab { const char* icon; const char* label; int tabIndex; };

// ===================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =====================
bool g_isRussian = true;
LangPack* L = nullptr;

// Расширенный список путей реестра (v3.3)
std::vector<RegPath> g_regPaths = {
    // === Anti-forensics / программа запуска ===
    { "BAM (Background Activity Monitor)",
      "Показывает все запущенные программы с датой и временем",
      HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings",
      false },
    { "Compatibility Assistant",
      "Хранит данные о совместимости запущенных программ",
      HKEY_CURRENT_USER,
      "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store",
      false },
    { "AppCompat Layers",
      "Флаги совместимости — запуск от админа, режимы совместимости",
      HKEY_CURRENT_USER,
      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",
      false },
    { "Windows Defender Exclusions",
      "Исключения Windows Defender — папки, которые не сканируются антивирусом",
      HKEY_LOCAL_MACHINE,
      "SOFTWARE\\Microsoft\\Windows Defender\\Exclusions\\Paths",
      false },
    { "UserAssist (ROT13)",
      "Статистика запуска программ из реестра (закодирована ROT13)",
      HKEY_CURRENT_USER,
      "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist",
      true },
    { "MUICache",
      "Кэш MUI — список запускавшихся программ",
      HKEY_CURRENT_USER,
      "SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache",
      false },
    { "ShellBags",
      "История открытых папок в проводнике",
      HKEY_CURRENT_USER,
      "SOFTWARE\\Microsoft\\Windows\\Shell\\BagMRU",
      true },

      // === NEW v3.3 — Расширенные пути ===
      { "RecentDocs",
        "Недавние документы — история открытых файлов",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs",
        true },
      { "TypedPaths",
        "Пути, введённые вручную в проводнике",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TypedPaths",
        false },
      { "RunMRU",
        "История команд из Win+R",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
        false },
      { "WordWheelQuery",
        "История поиска в проводнике",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\WordWheelQuery",
        false },
      { "RDP History (Terminal Server Client)",
        "История подключений RDP",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Terminal Server Client\\Default",
        false },
      { "RecentApps",
        "Недавно запущенные приложения в Windows Search",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search\\RecentApps",
        true },
      { "Startup Approved",
        "Утверждённые записи автозапуска",
        HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartupApproved",
        true },
      { "Sysmon Config (если установлен)",
        "Конфиг Sysmon — может быть модифицирован читом",
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\SysmonDrv\\Parameters",
        false },
};

std::vector<CheckSite> g_checkSites = {
    { "xone.fun",       "https://xone.fun/",         "ЗА ЧТО БАНИМ: xone.fun" },
    { "midnight.im",    "https://midnight.im/",      "ЗА ЧТО БАНИМ: midnight.im" },
    { "aimware.net",    "https://aimware.net/",      "ЗА ЧТО БАНИМ: aimware.net" },
    { "nixware.cc",     "https://nixware.cc/",       "ЗА ЧТО БАНИМ: nixware.cc" },
    { "onetap.com",     "https://onetap.com/",       "ЗА ЧТО БАНИМ: onetap.com" },
    { "neverlose.cc",   "https://neverlose.cc/",     "НЕ БАНИМ: обычное посещение сайтов" },
    { "interium.ooo",   "https://interium.ooo/",     "ЗА ЧТО БАНИМ: interium.ooo" },
    { "yougame.biz",    "https://yougame.biz/",      "ЗА ЧТО БАНИМ: yougame.biz" },
    { "lolz.market",    "https://lolz.market/",      "ЗА ЧТО БАНИМ: lolz.market" },
    { "gamesense.pub",  "https://gamesense.pub/",    "ЗА ЧТО БАНИМ: gamesense.pub (skeet)" },
    { "fatality.win",   "https://fatality.win/",     "ЗА ЧТО БАНИМ: fatality.win" },
    { "primordial.dev", "https://primordial.dev/",   "ЗА ЧТО БАНИМ: primordial.dev" },
    { "memesense.gg",   "https://memesense.gg/",     "ЗА ЧТО БАНИМ: memesense.gg" },
    { "spirthack.me",   "https://spirthack.me/",     "ЗА ЧТО БАНИМ: spirthack.me" },
    { "ev0lve.xyz",     "https://ev0lve.xyz/",       "ЗА ЧТО БАНИМ: ev0lve.xyz" },
    { "legendware.pw",  "https://legendware.pw/",    "ЗА ЧТО БАНИМ: legendware.pw" },
    { "exloader.ru",    "https://exloader.ru/",      "ЗА ЧТО БАНИМ: exloader" },
};

std::vector<ToolEntry> g_tools = {
    { "ExecutedProgramsList", "ExecutedProgramsList.exe", "История запуска программ",     "activity" },
    { "LastActivityView",     "LastActivityView.exe",     "Последняя активность",         "activity" },
    { "UserAssistView",       "UserAssistView.exe",       "Статистика запуска из реестра","activity" },
    { "JumpListView",         "JumpListsView.exe",        "Последние папки (JumpLists)",  "activity" },
    { "MUICacheView",         "MUICacheView.exe",         "Кэш MUI — список программ",    "activity" },
    { "ShellBag Analyzer",    "ShellBag Analyzer.exe",    "Анализ ShellBag",              "activity" },
    { "SystemInformer",       "SystemInformer.exe",       "Мониторинг системы",           "system" },
    { "DevManView",           "DevManView.exe",           "Диспетчер устройств",          "system" },
    { "Everything",           "Everything.exe",           "Мгновенный поиск файлов",      "system" },
    { "LoadedDllsView x64",   "LoadedDllsView.exe",       "Загруженные DLL",              "system" },
    { "USBDeview",            "USBDeview.exe",            "USB-устройства",               "usb" },
    { "BrowserDownloadsView", "BrowserDownloadsView.exe", "История загрузок браузера",    "browser" },
};

std::vector<std::string> FONT_CANDIDATES = {
    "assets/font.ttf",
    "assets/Inter-Regular.ttf",
    "assets/Roboto-Medium.ttf",
    "C:\\Windows\\Fonts\\segoeui.ttf",
    "C:\\Windows\\Fonts\\tahoma.ttf",
    "C:\\Windows\\Fonts\\arial.ttf",
};

std::vector<std::string> g_ignoreList = {
    "windows","system32","microsoft","gigabyte","nvidia","amd","windowsapps",
    "winsxs","steam","epic games","valve","counter strike"
};

std::mutex g_ignoreMutex;
std::mutex g_resultMutex;
std::vector<json> g_results;
std::atomic<uint64_t> g_resultsVersion{ 0 };
std::atomic<bool> g_scanRunning{ false }, g_cancelScan{ false }, g_appQuit{ false };
std::atomic<size_t> g_scannedCount{ 0 }, g_matchesCount{ 0 };
std::atomic<int> g_activeTab{ 0 };
char g_searchQuery[256] = { 0 };
std::vector<std::string> g_processList;
int g_selectedProcess = 0;
std::atomic<bool> g_showSplash{ true };
float g_splashTime = 0.f;
float g_splashAlpha = 0.f;
GLuint g_backgroundTexture = 0;
GLuint g_logoTexture = 0;
bool g_hasBackground = false;
bool g_hasLogo = false;
bool g_cs2AutoSelected = false;

HWND g_hwnd = nullptr;
GLFWwindow* g_window = nullptr;
bool g_windowMaximized = false;

std::atomic<bool> g_autoStart{ false };
std::atomic<bool> g_soundEnabled{ true };
std::map<std::string, std::string> g_knownHashes;

std::vector<ScanHistoryEntry> g_scanHistory;
std::mutex g_historyMutex;

std::vector<SteamAccount> g_steamAccounts;
std::mutex g_steamMutex;

std::vector<RegistryFinding> g_registryFindings;
std::mutex g_registryMutex;

std::vector<ProcessStringHit> g_processStringHits;
std::mutex g_processStringMutex;

PlayerProfile g_playerProfile;

std::string g_statusKey = "ready";

// ===================== RAII WRAPPER ДЛЯ HANDLE =====================
struct ScopedHandle {
    HANDLE h = INVALID_HANDLE_VALUE;
    ScopedHandle() = default;
    explicit ScopedHandle(HANDLE handle) : h(handle) {}
    ~ScopedHandle() { if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    ScopedHandle(ScopedHandle&& other) noexcept : h(other.h) { other.h = INVALID_HANDLE_VALUE; }
    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
            h = other.h;
            other.h = INVALID_HANDLE_VALUE;
        }
        return *this;
    }
    operator HANDLE() const { return h; }
    bool valid() const { return h && h != INVALID_HANDLE_VALUE; }
    HANDLE get() const { return h; }
};

// ===================== БАЗОВЫЕ УТИЛИТЫ =====================
bool IsRunningAsAdmin() {
    BOOL isAdmin = FALSE; PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin == TRUE;
}

bool RelaunchAsAdmin() {
    char path[MAX_PATH]{};
    if (!GetModuleFileNameA(nullptr, path, MAX_PATH)) return false;
    SHELLEXECUTEINFOA sei{ sizeof(sei) };
    sei.lpVerb = "runas"; sei.lpFile = path; sei.nShow = SW_SHOWNORMAL;
    return ShellExecuteExA(&sei) == TRUE;
}

std::string ToLower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) return "";
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

void OpenURL(const std::string& url) {
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void PlayAlertSound() {
    if (!g_soundEnabled.load()) return;
    MessageBeep(MB_ICONEXCLAMATION);
}

void SaveSettings() {
    json j;
    j["ignore_list"] = g_ignoreList;
    j["russian"] = g_isRussian;
    j["sound_enabled"] = g_soundEnabled.load();
    fs::create_directories("settings");
    std::ofstream ofs("settings/" + SETTINGS_FILE);
    if (ofs) ofs << j.dump(4);
}

void LoadSettings() {
    std::ifstream ifs("settings/" + SETTINGS_FILE);
    if (!ifs) return;
    try {
        json j; ifs >> j;
        if (j.contains("ignore_list") && j["ignore_list"].is_array()) {
            g_ignoreList.clear();
            for (auto& v : j["ignore_list"]) g_ignoreList.push_back(v.get<std::string>());
        }
        if (j.contains("russian"))       g_isRussian = j["russian"].get<bool>();
        if (j.contains("sound_enabled")) g_soundEnabled = j["sound_enabled"].get<bool>();
    }
    catch (...) {}
}

bool IsWhitelisted(const std::string& path) {
    auto lower = ToLower(path);
    std::lock_guard<std::mutex> lk(g_ignoreMutex);
    for (const auto& w : g_ignoreList)
        if (lower.find(ToLower(w)) != std::string::npos) return true;
    return false;
}

std::string GetFolderPath(REFKNOWNFOLDERID rfid) {
    PWSTR pszPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(rfid, 0, NULL, &pszPath);
    if (SUCCEEDED(hr)) {
        std::wstring wpath(pszPath);
        CoTaskMemFree(pszPath);
        return WStringToString(wpath);
    }
    return "";
}

bool VerifyFileSignature(const std::wstring& filePath) {
    WINTRUST_FILE_INFO FileData; memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = filePath.c_str();
    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA WinTrustData; memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.pFile = &FileData;
    LONG lStatus = WinVerifyTrust(nullptr, &WVTPolicyGUID, &WinTrustData);
    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &WVTPolicyGUID, &WinTrustData);
    return lStatus == ERROR_SUCCESS;
}

std::string GetFileSHA256(const std::wstring& path) {
    std::string result;
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return result;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        CloseHandle(hFile); return result;
    }
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0); CloseHandle(hFile); return result;
    }

    BYTE buf[8192];
    DWORD read = 0;
    while (ReadFile(hFile, buf, sizeof(buf), &read, nullptr) && read > 0)
        CryptHashData(hHash, buf, read, 0);

    BYTE hash[32]; DWORD hashLen = 32;
    CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0);

    char hex[3];
    for (DWORD i = 0; i < hashLen; ++i) {
        sprintf_s(hex, "%02x", hash[i]);
        result += hex;
    }
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
    return result;
}

std::string HttpGet(const std::string& url) {
    std::string result;
    HINTERNET hInt = InternetOpenA("CS2-Cheat-Checker-by-Chmonya/3.3", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInt) return result;
    DWORD timeout = 8000;
    InternetSetOptionA(hInt, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInt, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInt, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    HINTERNET hConn = InternetOpenUrlA(hInt, url.c_str(), NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (hConn) {
        InternetSetOptionA(hConn, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
        char buf[4096]; DWORD read = 0;
        while (InternetReadFile(hConn, buf, sizeof(buf) - 1, &read) && read > 0) {
            buf[read] = 0; result.append(buf, read);
        }
        InternetCloseHandle(hConn);
    }
    InternetCloseHandle(hInt);
    return result;
}

std::string ExtractTag(const std::string& xml, const std::string& tag) {
    std::string xmlLow = ToLower(xml);
    std::string tagLow = ToLower(tag);
    std::string openLow = "<" + tagLow + ">";
    std::string closeLow = "</" + tagLow + ">";

    size_t s = xmlLow.find(openLow);
    if (s == std::string::npos) return "";
    s += openLow.size();
    size_t e = xmlLow.find(closeLow, s);
    if (e == std::string::npos) return "";

    std::string v = xml.substr(s, e - s);

    {
        std::string vLow = ToLower(v);
        size_t cdataStart = vLow.find("<![cdata[");
        if (cdataStart != std::string::npos) {
            size_t cdataEnd = vLow.find("]]>", cdataStart + 9);
            if (cdataEnd != std::string::npos) {
                v = v.substr(cdataStart + 9, cdataEnd - cdataStart - 9);
            }
        }
    }

    auto rep = [&](const std::string& f, const std::string& t) {
        size_t p = 0;
        while ((p = v.find(f, p)) != std::string::npos) {
            v.replace(p, f.size(), t);
            p += t.size();
        }
        };
    rep("&amp;", "&");
    rep("&lt;", "<");
    rep("&gt;", ">");
    rep("&quot;", "\"");
    rep("&apos;", "'");
    rep("&#39;", "'");
    rep("&#34;", "\"");

    while (!v.empty() && (v.front() == ' ' || v.front() == '\n' || v.front() == '\r' || v.front() == '\t'))
        v.erase(v.begin());
    while (!v.empty() && (v.back() == ' ' || v.back() == '\n' || v.back() == '\r' || v.back() == '\t'))
        v.pop_back();

    return v;
}

void OpenInExplorer(std::string path) {
    if (path.empty()) return;
    // Убираем суффиксы (PID, [in proc], String:)
    auto pipePos = path.find(" | PID ");
    if (pipePos != std::string::npos) path = path.substr(0, pipePos);
    auto inPos = path.find(" [in ");
    if (inPos != std::string::npos) path = path.substr(0, inPos);
    auto strPos = path.find("String: ");
    if (strPos == 0) return; // String не открываем в проводнике

    std::error_code ec;
    if (fs::exists(path, ec)) {
        std::string arg = "/select,\"" + path + "\"";
        ShellExecuteA(nullptr, "open", "explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
        return;
    }
    fs::path parent = fs::path(path).parent_path();
    if (fs::exists(parent, ec)) {
        ShellExecuteA(nullptr, "open", "explorer.exe", parent.string().c_str(), nullptr, SW_SHOWNORMAL);
    }
}

void SetAutoStart(bool enable) {
    HKEY hKey;
    const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) return;
    if (enable) {
        char exePath[MAX_PATH]{};
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        std::string value = "\"" + std::string(exePath) + "\"";
        RegSetValueExA(hKey, "CS2 Cheat Checker by Chmonya", 0, REG_SZ,
            (const BYTE*)value.c_str(), (DWORD)value.size() + 1);
    }
    else {
        RegDeleteValueA(hKey, "CS2 Cheat Checker by Chmonya");
    }
    RegCloseKey(hKey);
}

bool IsAutoStartEnabled() {
    HKEY hKey;
    const char* regPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return false;
    char buf[MAX_PATH * 2]; DWORD size = sizeof(buf);
    bool result = RegQueryValueExA(hKey, "CS2 Cheat Checker by Chmonya", 0, nullptr, (LPBYTE)buf, &size) == ERROR_SUCCESS;
    RegCloseKey(hKey);
    return result;
}

void SendToDiscord(const std::string& msg) {
    if (WEBHOOK_URL.find("REPLACE") != std::string::npos) return;
    json j; j["content"] = msg;
    std::string payload = j.dump();
    HINTERNET hInt = InternetOpenA("CS2 Cheat Checker by Chmonya/3.3", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInt) {
        HINTERNET hConn = InternetOpenUrlA(hInt, WEBHOOK_URL.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConn) {
            const char* headers = "Content-Type: application/json\r\n";
            HttpSendRequestA(hConn, headers, (DWORD)strlen(headers), (LPVOID)payload.c_str(), (DWORD)payload.length());
            InternetCloseHandle(hConn);
        }
        InternetCloseHandle(hInt);
    }
}

void LoadHashDatabase() {
    std::string jsonStr = HttpGet(std::string(HASH_DB_URL));
    if (jsonStr.empty()) return;
    try {
        auto j = json::parse(jsonStr);
        std::lock_guard<std::mutex> lk(g_resultMutex);
        g_knownHashes.clear();
        for (auto& [k, v] : j.items())
            g_knownHashes[k] = v.get<std::string>();
    }
    catch (...) {}
}

// ===================== ADD RESULT =====================
void AddResult(const std::string& name, const std::string& path, const std::string& type,
    const std::string& reason, int severity) {
    std::lock_guard<std::mutex> lock(g_resultMutex);
    for (const auto& r : g_results)
        if (r.value("path", " ") == path && r.value("type", " ") == type) return;
    json res;
    res["name"] = name;
    res["path"] = path;
    res["type"] = type;
    res["reason"] = reason.empty() ? type : reason;
    res["time"] = (long long)std::time(nullptr);
    res["severity"] = severity;

    if (type == "File" || type == "Signature" || type == "Module" || type == "Driver") {
        std::string cleanPath = path;
        auto pipePos = cleanPath.find(" | ");
        if (pipePos != std::string::npos) cleanPath = cleanPath.substr(0, pipePos);
        auto inPos = cleanPath.find(" [in ");
        if (inPos != std::string::npos) cleanPath = cleanPath.substr(0, inPos);

        if (fs::exists(cleanPath)) {
            std::wstring wp(cleanPath.begin(), cleanPath.end());
            std::string hash = GetFileSHA256(wp);
            if (!hash.empty()) {
                res["sha256"] = hash;
                auto it = g_knownHashes.find(hash);
                if (it != g_knownHashes.end()) {
                    res["known"] = it->second;
                    res["severity"] = 5;
                }
            }
        }
    }

    g_results.push_back(std::move(res));
    g_matchesCount++;
    g_resultsVersion.fetch_add(1, std::memory_order_relaxed);
    PlayAlertSound();

    if (g_scanRunning.load()) {
        static std::atomic<int64_t> lastRPCUpdate{ 0 };
        int64_t now = (int64_t)time(nullptr);
        int64_t last = lastRPCUpdate.load();
        if (now - last >= 2 && lastRPCUpdate.compare_exchange_strong(last, now)) {
            size_t found = g_matchesCount.load();
            RPC_SetState("Найдено: " + std::to_string(found));
        }
    }
}

void ClearResults() {
    std::lock_guard<std::mutex> lk(g_resultMutex);
    g_results.clear();
    g_resultsVersion.fetch_add(1, std::memory_order_relaxed);
}

// ===================== FILE CONTENT SCAN (binary-safe) =====================
bool FileHasKeywordContent(const fs::path& p, size_t maxBytes = 512 * 1024) {
    try {
        std::ifstream ifs(p, std::ios::binary);
        if (!ifs) return false;
        std::vector<char> buf(maxBytes);
        ifs.read(buf.data(), (std::streamsize)buf.size());
        auto n = ifs.gcount();
        if (n <= 0) return false;
        return ContainsKeywordRaw(buf.data(), (size_t)n);
    }
    catch (...) { return false; }
}

// ===================== AHO-CORASICK (binary-safe) =====================
class AhoCorasick {
public:
    struct Node {
        std::map<char, int> next;
        int fail = 0;
        bool terminal = false;
    };

    void Build(const std::vector<std::string>& patterns) {
        nodes.clear();
        nodes.emplace_back();
        for (const auto& p : patterns) {
            std::string low = p;
            for (auto& c : low) c = (char)std::tolower((unsigned char)c);
            int cur = 0;
            for (char ch : low) {
                auto it = nodes[cur].next.find(ch);
                if (it == nodes[cur].next.end()) {
                    int nid = (int)nodes.size();
                    nodes[cur].next[ch] = nid;
                    nodes.emplace_back();
                    cur = nid;
                }
                else cur = it->second;
            }
            nodes[cur].terminal = true;
        }

        std::queue<int> q;
        for (auto& kv : nodes[0].next) { nodes[kv.second].fail = 0; q.push(kv.second); }
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto& kv : nodes[u].next) {
                char ch = kv.first;
                int v = kv.second;
                int f = nodes[u].fail;
                while (f != 0 && nodes[f].next.find(ch) == nodes[f].next.end()) f = nodes[f].fail;
                auto it = nodes[f].next.find(ch);
                nodes[v].fail = (it != nodes[f].next.end() && it->second != v) ? it->second : 0;
                if (nodes[nodes[v].fail].terminal) nodes[v].terminal = true;
                q.push(v);
            }
        }
    }

    bool Contains(const std::string& text) const {
        if (nodes.empty()) return false;
        int cur = 0;
        for (unsigned char ch : text) {
            char c = (char)std::tolower(ch);
            while (cur != 0 && nodes[cur].next.find(c) == nodes[cur].next.end()) cur = nodes[cur].fail;
            auto it = nodes[cur].next.find(c);
            if (it != nodes[cur].next.end()) cur = it->second;
            if (nodes[cur].terminal) return true;
        }
        return false;
    }

    bool ContainsRaw(const char* data, size_t len) const {
        if (nodes.empty() || !data || len == 0) return false;
        int cur = 0;
        for (size_t i = 0; i < len; ++i) {
            char c = (char)std::tolower((unsigned char)data[i]);
            while (cur != 0 && nodes[cur].next.find(c) == nodes[cur].next.end()) cur = nodes[cur].fail;
            auto it = nodes[cur].next.find(c);
            if (it != nodes[cur].next.end()) cur = it->second;
            if (nodes[cur].terminal) return true;
        }
        return false;
    }

private:
    std::vector<Node> nodes;
};
AhoCorasick g_keywordAC;

bool ContainsKeyword(const std::string& text) { return g_keywordAC.Contains(text); }
bool ContainsKeywordRaw(const char* data, size_t len) { return g_keywordAC.ContainsRaw(data, len); }

// ===================== ОЧЕРЕДЬ ЗАДАЧ =====================
class OptimizedTaskQueue {
public:
    void push(const std::string& p) {
        { std::lock_guard<std::mutex> lk(m); q.push(p); active_tasks++; }
        cv.notify_one();
    }
    bool pop(std::string& out) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this] { return !q.empty() || g_cancelScan || (q.empty() && active_tasks == 0); });
        if (g_cancelScan) return false;
        if (q.empty() && active_tasks == 0) return false;
        out = q.front(); q.pop();
        return true;
    }
    void task_done() {
        std::lock_guard<std::mutex> lk(m);
        if (active_tasks > 0) active_tasks--;
        if (q.empty() && active_tasks == 0) cv.notify_all();
    }
    void clear() { std::lock_guard<std::mutex> lk(m); while (!q.empty()) q.pop(); active_tasks = 0; }
    void notify_all() { { std::lock_guard<std::mutex> lk(m); g_cancelScan = true; } cv.notify_all(); }
private:
    std::queue<std::string> q; std::mutex m; std::condition_variable cv; int active_tasks = 0;
} g_taskQueue;

// ===================== DISCORD RPC =====================
static const char* DISCORD_CLIENT_ID = "1548371598594277396";
static int64_t     g_rpcStartTime = 0;
static std::mutex  g_rpcMutex;
static bool        g_rpcReady = false;
static std::string g_rpcDetails = "В меню";
static std::string g_rpcState = "Готов";

// v3.3: pipe name обновлён (было csarena-rpc)
static const wchar_t* RPC_PIPE_NAME = L"\\\\.\\pipe\\chmonya-cc-rpc";
static const char* RPC_PIPE_NAME_A = "\\\\.\\pipe\\chmonya-cc-rpc";

static HANDLE     g_rpcPipe = INVALID_HANDLE_VALUE;
static std::mutex g_rpcPipeMutex;
static bool       g_rpcProxyLaunched = false;

static DWORD FindExplorerPid() {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe{ sizeof(pe) };
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"explorer.exe") == 0) { pid = pe.th32ProcessID; break; }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return pid;
}

static bool RPC_ProxyConnect(int timeoutMs) {
    if (g_rpcPipe != INVALID_HANDLE_VALUE) return true;
    if (!WaitNamedPipeA(RPC_PIPE_NAME_A, timeoutMs)) return false;
    HANDLE h = CreateFileA(RPC_PIPE_NAME_A, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    std::lock_guard<std::mutex> lk(g_rpcPipeMutex);
    g_rpcPipe = h;
    g_rpcReady = true;
    return true;
}

static void RPC_ProxySend(const std::string& details, const std::string& state) {
    std::string msg = details + "|" + state;
    std::lock_guard<std::mutex> lk(g_rpcPipeMutex);
    if (g_rpcPipe == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    if (!WriteFile(g_rpcPipe, msg.c_str(), (DWORD)msg.size(), &written, nullptr)) {
        CloseHandle(g_rpcPipe);
        g_rpcPipe = INVALID_HANDLE_VALUE;
        g_rpcReady = false;
    }
}

void RPC_SetStatus(const std::string& details, const std::string& state) {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lk(g_rpcMutex);
        if (g_rpcDetails != details || g_rpcState != state) {
            g_rpcDetails = details;
            g_rpcState = state;
            changed = true;
        }
    }
    if (changed) RPC_ProxySend(details, state);
}

void RPC_SetState(const std::string& state) {
    bool changed = false;
    std::string det;
    {
        std::lock_guard<std::mutex> lk(g_rpcMutex);
        if (g_rpcState != state) { g_rpcState = state; changed = true; }
        det = g_rpcDetails;
    }
    if (changed) RPC_ProxySend(det, state);
}

static bool LaunchRPCProxyAsUser() {
    DWORD explorerPid = FindExplorerPid();
    if (!explorerPid) return false;
    HANDLE hExplorer = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, explorerPid);
    if (!hExplorer) return false;
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(hExplorer, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &hToken)) {
        CloseHandle(hExplorer); return false;
    }
    CloseHandle(hExplorer);
    HANDLE hDup = nullptr;
    if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenPrimary, &hDup)) {
        CloseHandle(hToken); return false;
    }
    CloseHandle(hToken);
    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    wchar_t cmdLine[MAX_PATH + 32];
    swprintf_s(cmdLine, L"\"%s\" --rpc-proxy", exePath);
    STARTUPINFOW si{ sizeof(si) };
    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessWithTokenW(hDup, 0, exePath, cmdLine,
        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    if (!ok) {
        ok = CreateProcessAsUserW(hDup, exePath, cmdLine, nullptr, nullptr, FALSE,
            CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    }
    CloseHandle(hDup);
    if (!ok) return false;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

void RPC_Init() {
    g_rpcStartTime = (int64_t)time(nullptr);
    if (IsRunningAsAdmin() && !g_rpcProxyLaunched) {
        LaunchRPCProxyAsUser();
        g_rpcProxyLaunched = true;
    }
    for (int i = 0; i < 15; ++i) {
        if (RPC_ProxyConnect(1000)) break;
        Sleep(300);
    }
    if (g_rpcPipe != INVALID_HANDLE_VALUE) {
        RPC_ProxySend(g_rpcDetails, g_rpcState);
    }
}

void RPC_Shutdown() {
    std::lock_guard<std::mutex> lk(g_rpcPipeMutex);
    if (g_rpcPipe != INVALID_HANDLE_VALUE) {
        const char* quitMsg = "__QUIT__|";
        DWORD written = 0;
        WriteFile(g_rpcPipe, quitMsg, (DWORD)strlen(quitMsg), &written, nullptr);
        CloseHandle(g_rpcPipe);
        g_rpcPipe = INVALID_HANDLE_VALUE;
    }
    g_rpcReady = false;
}

void RPC_OnScanStart(const char* scanName) {
    RPC_SetStatus(std::string(scanName) + ": Сканирую...", "Найдено: 0");
}
void RPC_OnScanFinish() {
    size_t found = g_matchesCount.load();
    RPC_SetStatus("В меню", "Готов  |  Найдено: " + std::to_string(found));
}
void RPC_OnTabSwitch(const char* tabName) {
    RPC_SetStatus(std::string(tabName), "Найдено: " + std::to_string(g_matchesCount.load()));
}

// ===================== RPC PROXY =====================
static void RunRPCProxy() {
    CreateDirectoryA("logs", nullptr);
    static FILE* logFile = nullptr;
    if (fopen_s(&logFile, "logs/rpc-proxy.log", "w") == 0 && logFile) {
        freopen_s(&logFile, "logs/rpc-proxy.log", "w", stdout);
    }
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("[PROXY] CS2 Cheat Checker by Chmonya RPC proxy starting...\n");

    DiscordEventHandlers h;
    memset(&h, 0, sizeof(h));
    h.ready = [](const DiscordUser* u) {
        printf("[PROXY] *** CONNECTED as %s#%s ***\n", u->username, u->discriminator);
        };
    h.disconnected = [](int c, const char* m) {
        printf("[PROXY] disconnected: code=%d msg=%s\n", c, m ? m : "(null)");
        };
    h.errored = [](int c, const char* m) {
        printf("[PROXY] error: code=%d msg=%s\n", c, m ? m : "(null)");
        };

    Discord_Initialize(DISCORD_CLIENT_ID, &h, 1, nullptr);

    for (int i = 0; i < 40; ++i) {
        Discord_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    {
        DiscordRichPresence p;
        memset(&p, 0, sizeof(p));
        p.details = "В меню";
        p.state = "Готов";
        p.startTimestamp = (int64_t)time(nullptr);
        p.largeImageKey = "logo";
        p.largeImageText = "CS2 Cheat Checker by Chmonya";
        p.smallImageKey = "shield";
        p.smallImageText = "by Chmonya";
        Discord_UpdatePresence(&p);
    }

    for (int i = 0; i < 20; ++i) {
        Discord_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);

    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    HANDLE hPipe = CreateNamedPipeW(
        RPC_PIPE_NAME, PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 4096, 4096, 0, &sa);

    if (hPipe == INVALID_HANDLE_VALUE) return;
    printf("[PROXY] Pipe ready\n");

    std::string lastDet = "В меню";
    std::string lastSt = "Готов";

    while (true) {
        Discord_RunCallbacks();
        BOOL connected = ConnectNamedPipe(hPipe, nullptr);
        DWORD err = GetLastError();
        if (!connected && err != ERROR_PIPE_CONNECTED) { Sleep(100); continue; }

        char buf[1024];
        while (true) {
            Discord_RunCallbacks();
            DWORD read = 0;
            BOOL ok = ReadFile(hPipe, buf, sizeof(buf) - 1, &read, nullptr);
            if (!ok || read == 0) {
                DWORD e = GetLastError();
                if (e == ERROR_BROKEN_PIPE || e == ERROR_PIPE_NOT_CONNECTED) break;
                Sleep(20); continue;
            }
            buf[read] = 0;
            std::string msg(buf);
            if (msg.rfind("__QUIT__", 0) == 0) {
                Discord_ClearPresence();
                Discord_Shutdown();
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);
                return;
            }
            auto sep = msg.find('|');
            if (sep == std::string::npos) continue;
            lastDet = msg.substr(0, sep);
            lastSt = msg.substr(sep + 1);

            DiscordRichPresence p;
            memset(&p, 0, sizeof(p));
            p.details = lastDet.c_str();
            p.state = lastSt.c_str();
            p.startTimestamp = (int64_t)time(nullptr);
            p.largeImageKey = "logo";
            p.largeImageText = "CS2 Cheat Checker by Chmonya";
            p.smallImageKey = "shield";
            p.smallImageText = "by Chmonya";
            Discord_UpdatePresence(&p);
        }
        DisconnectNamedPipe(hPipe);
    }
}

// ===================== KEYWORDS =====================
const std::vector<std::string> KEYWORDS = {
    "interium","xone","midnight","mutiny","vredux","espdx","neverlose","legendware",
    "nixware","phoenixhack","onebyteradar","ezinjector","reborn","keter","annihilation",
    "sapphire","osiris","multihack","extrimhack","shark","exloader","imgui.ini","csghost",
    "luno","primordial","angelrose","rawetrip","interception","ezglobal","apasia","demotion",
    "nullhooks","elysion","aimfuck","anyx.gg","crux.exe","injector","loader.exe","loader",
    "cheats","kaban","fatality","gamesense","onetap","skeet","spirthack","weave","pandora",
    "rifk","memesense","ev0lve","plaguecheat","dumb.gg","enigma","amalgam","external","internal",
    "midnight.im","midnight.im loader","aimware","catalyst","pandora-hvh","pandora hvh",
    "primordial.dev","primordialcheat","neverlose.cc","getneverlose","nl-loader",
    "fatality.win","fatality.win loader","fatality cli","gamesense.pub","gs-loader",
    "onetap.su","onetap loader","skeet.cc","mesa","mesa-v2","penguware","pengu loader",
    "interium.gg","interium.pro","interium loader","xone.wtf","xone-loader",
    "memesense.gg","memesense loader","meme sense","ms-loader","msense",
    "spirthack.me","spirthack loader","ev0lve.xyz","ev0lve-loader","ev0lve api",
    "rifk7","rifk.gg","rifk-loader","aimware.wtf","aw-v5","aw-loader",
    "novoline","novoline.wtf","novoline hook","nvl","nvl-loader",
    "pandora.wtf","pandora-loader","pandora crack",
    "midnight beta","midnight crack","midnight cheat",
    "phantom","phantom.wtf","phantom-loader",
    "nemesis","nemesis.wtf","nemesis-loader",
    "oxygene","oxygene.wtf","oxy-loader",
    "atlas","atlas.wtf","atlas-loader",
    "plague","plague.wtf","plague-cheat",
    "baimless","baimless.wtf",
    "weave.su","weave-loader","weave crack",
    "gamesense crack","onetap crack","skeet crack",
    "wallhack","triggerbot","bhop","skinchanger","autoexec.cfg","glow esp",
    "chams","norecoil","nospread","antiaim","silent aim","silentaim","psilent",
    "backtrack","lagcomp","lag compensation","fakelag","fake lag","fake duck",
    "fakeduck","desync","desynced","resolver","anti-aim","anti aim","aa-desync",
    "hitchance","hit chance","no spread","no recoil","no-spread","no-recoil",
    "aim step","aimstep","edge jump","edgejump","jumpbug","jump bug",
    "auto peek","autopeek","auto wall","autowall","auto fire","autofire",
    "rapid fire","rapidfire","double tap","doubletap","hide shots","hideshots",
    "on shot","onshot aa","defensive aa","defensive-aa",
    "injector.exe","inject.exe","injector64","injector-x64","injector x86",
    "loader.exe","load.exe","premium loader","cheat loader","hack loader",
    "manual mapper","manualmap","manual-mapper","kernel driver","kernel injector",
    "kdmapper","kdmapper.exe","kdu.exe","kdu-signed","vuln driver","vuln-driver",
    "drvmap","drvmap.exe","drv loader","drv-loader",
    "efi injector","efiguard","efi-guard",
    "hypervisor bypass","hvci bypass","pg bypass","patchguard bypass",
    "signed driver","unsigned driver","byovd","byovd loader",
    "cheat.dll","cheat64.dll","cheat-x64.dll","hack.dll","hack64.dll",
    "cheat.sys","hack.sys","injector.sys","loader.sys","driver.sys",
    "aimbot.dll","aimbot.sys","esp.dll","wallhack.dll","triggerbot.dll",
    "skin.dll","skinchanger.dll","glow.dll","chams.dll","backtrack.dll",
    "hvh.dll","hvh-cheat.dll","rage.dll","ragecheat.dll","legit.dll",
    "internal.dll","external.dll","private.dll","public.dll",
    "config.cfg","userconfig.cfg","hvh.cfg","legit.cfg","rage.cfg",
    "cheat.cfg","hack.cfg","aimbot.cfg","legit_config","rage_config",
    "aimbot.json","hvh.json","cheat.json",
    "cheat_config","legit config","rage config",
    "lua_api","api.lua","cheat.lua","hack.lua",
    "hitsound.wav","killsound.wav","cheat_sound",".cfg",".lua",
    "faceit bypass","esportal bypass","esea bypass","popflash bypass",
    "vanguard bypass","eac bypass","battleye bypass","be bypass",
    "trust factor bypass","trustfactor bypass","trust factor fix",
    "hwid spoofer","hwid-spoofer","hwidspoofer","hwid changer",
    "mac spoofer","disk spoofer","smbios spoofer",
    "cleaner.exe","tracer cleaner","trace cleaner","footprint cleaner",
    "anti-screen","antiscreen","anti screenshot","screenshot blocker",
    "stream proof","streamproof","stream-proof","obs bypass","obsbypass",
    "createRemoteThread","WriteProcessMemory","ReadProcessMemory",
    "NtCreateThreadEx","NtWriteVirtualMemory","NtReadVirtualMemory",
    "NtMapViewOfSection","NtUnmapViewOfSection","NtQueueApcThread",
    "SetWindowsHookEx","GetAsyncKeyState","BlockInput",
    "ManualMap","VirtualAllocEx","VirtualProtectEx",
    "SetThreadContext","SuspendThread","ResumeThread",
    "RtlCreateUserThread","QueueUserAPC",
    "syscall stub","syscall-stub","hells gate","hellsgate","halosgate",
    "freshycall","tartarus","sweetscape",
    "getprocaddress hash","gpa hash","api hashing","apihash",
    "string obfuscation","xor key","xorkey",
    "kmbox","kmbox net","kmbox b","kmbox net pro","kmbox.net",
    "arduino aimbot","arduino-aimbot","raspberry aimbot","pi aimbot",
    "makcu","makcu.net","makcu fc","makcu fusion",
    "ferrum","ferrum hardware","ferrum-hw",
    "aimbot board","aimbot-board","arduino.ino",
    "neverlose crack","onetap crack","gamesense crack","fatality crack",
    "primordial crack","memesense crack","interium crack","novoline crack",
    "midnight crack","spirthack crack","ev0lve crack","nl crack","ot crack",
    "gs crack","ms crack","fatality free","primordial free","onetap free",
    "neverlose free","gamesense free",
    "hvh config","hvh loader","hvh-cheat",
    "aa.cfg","aa-desync","aa-jitter","jitter.cfg",
    "defensive aa","aggressive aa","static aa",
    "hitchance.cfg","hc.cfg","mindmg.cfg","min dmg","mindamage",
    "autopeek.cfg","fakeyaw","fake yaw",
    "hvh clan","hvh clan tag","hvh team",
    "dt exploit","doubletap exploit","dt exploit cs2",
    "sub tick","subtick exploit","subtick abuse",
    "exploit.cfg","exploits.cfg","abuse.cfg",
    "c4 timer","c4timer","bomb timer","bombtimer",
    "radar hack","radarhack","radar.cfg","minimap hack",
    "penetration crosshair","pen crosshair","penxhair",
    "nade helper","nadehelper","grenade helper","lineup helper",
    "auto accept","autoaccept","auto-accept"
};

// ===================== СКАН ПРОЦЕССОВ — RAII =====================
void ScanModulesInProcess(DWORD pid, const std::string& procName) {
    ScopedHandle h(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
    if (!h.valid()) return;

    HMODULE modules[1024];
    DWORD needed = 0;
    if (EnumProcessModules(h.get(), modules, sizeof(modules), &needed)) {
        DWORD count = needed / sizeof(HMODULE);
        for (DWORD i = 0; i < count; ++i) {
            char modName[MAX_PATH]{};
            if (GetModuleFileNameExA(h.get(), modules[i], modName, MAX_PATH)) {
                std::string modPath = modName;
                std::string modFile = fs::path(modPath).filename().string();
                if (ContainsKeyword(modFile)) {
                    AddResult(modFile, modPath + " [in " + procName + "]",
                        "Module", "Suspicious DLL in process", 5);
                }
            }
        }
    }
}

// ===================== STEAM =====================
std::string FindSteamInstallPath() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char steamPath[MAX_PATH]{};
        DWORD size = sizeof(steamPath);
        if (RegQueryValueExA(hKey, "SteamPath", nullptr, nullptr, (LPBYTE)steamPath, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return std::string(steamPath);
        }
        RegCloseKey(hKey);
    }
    std::vector<std::string> candidates = {
        "C:\\Program Files (x86)\\Steam",
        "C:\\Program Files\\Steam",
        "D:\\Steam",
        "E:\\Steam",
    };
    for (auto& p : candidates) {
        if (fs::exists(p + "\\config\\loginusers.vdf")) return p;
    }
    return "";
}

void ParseLoginUsersVDF(const std::string& filePath) {
    std::lock_guard<std::mutex> lk(g_steamMutex);
    g_steamAccounts.clear();

    std::ifstream ifs(filePath);
    if (!ifs) return;
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    size_t pos = 0;
    while (true) {
        pos = content.find("\"76561", pos);
        if (pos == std::string::npos) break;
        size_t steamIdStart = pos + 1;
        size_t steamIdEnd = content.find('"', steamIdStart);
        if (steamIdEnd == std::string::npos) break;
        std::string steamId = content.substr(steamIdStart, steamIdEnd - steamIdStart);
        size_t blockStart = content.find('{', steamIdEnd);
        size_t blockEnd = content.find('}', blockStart);
        if (blockStart == std::string::npos || blockEnd == std::string::npos) break;
        std::string block = content.substr(blockStart, blockEnd - blockStart);

        auto extractVDF = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\"";
            size_t kp = block.find(search);
            if (kp == std::string::npos) return "";
            size_t q1 = block.find('"', kp + search.size());
            if (q1 == std::string::npos) return "";
            size_t q2 = block.find('"', q1 + 1);
            if (q2 == std::string::npos) return "";
            size_t q3 = block.find('"', q2 + 1);
            if (q3 == std::string::npos) return "";
            size_t q4 = block.find('"', q3 + 1);
            if (q4 == std::string::npos) return "";
            return block.substr(q3 + 1, q4 - q3 - 1);
            };

        SteamAccount acc;
        acc.steamID64 = steamId;
        acc.personaName = extractVDF("PersonaName");
        acc.accountName = extractVDF("AccountName");
        acc.mostRecent = block.find("\"MostRecent\" \"1\"") != std::string::npos;

        std::string tsStr = extractVDF("Timestamp");
        if (!tsStr.empty()) {
            try {
                time_t ts = std::stoll(tsStr);
                struct tm t;
                localtime_s(&t, &ts);
                char buf[64];
                strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &t);
                acc.lastLogin = buf;
            }
            catch (...) {}
        }

        if (!acc.steamID64.empty()) g_steamAccounts.push_back(acc);
        pos = blockEnd + 1;
    }
}

// ===================== STEAM ID PARSER =====================
static bool IsValidSteamID64(const std::string& s) {
    if (s.size() != 17) return false;
    if (!std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c) != 0; })) return false;
    try {
        unsigned long long v = std::stoull(s);
        return v >= 76561197960265728ULL && v <= 76561202255233023ULL;
    }
    catch (...) { return false; }
}

std::string ExtractSteamID64FromInput(const std::string& input) {
    if (IsValidSteamID64(input)) return input;

    auto pos = input.find("/profiles/");
    if (pos != std::string::npos) {
        std::string rest = input.substr(pos + 10), id;
        for (char ch : rest) {
            if (std::isdigit((unsigned char)ch)) id += ch;
            else break;
        }
        if (IsValidSteamID64(id)) return id;
    }

    if (input.rfind("STEAM_", 0) == 0) {
        try {
            size_t c1 = input.find(':');
            if (c1 == std::string::npos) return "";
            size_t c2 = input.find(':', c1 + 1);
            if (c2 == std::string::npos) return "";
            if (c2 <= c1 + 1) return "";
            std::string yStr = input.substr(c1 + 1, c2 - c1 - 1);
            std::string zStr = input.substr(c2 + 1);
            if (yStr.empty() || zStr.empty()) return "";
            if (!std::all_of(yStr.begin(), yStr.end(), [](unsigned char c) { return std::isdigit(c) != 0; })) return "";
            if (!std::all_of(zStr.begin(), zStr.end(), [](unsigned char c) { return std::isdigit(c) != 0; })) return "";

            uint64_t yv = std::stoull(yStr);
            uint64_t zv = std::stoull(zStr);
            std::string id = std::to_string(76561197960265728ULL + zv * 2ULL + yv);
            if (IsValidSteamID64(id)) return id;
        }
        catch (...) {}
    }

    if (input.rfind("[U:1:", 0) == 0) {
        try {
            size_t colon = input.find(':', 4);
            if (colon == std::string::npos) return "";
            size_t bracket = input.find(']', colon + 1);
            if (bracket == std::string::npos) return "";
            if (bracket <= colon + 1) return "";
            std::string num = input.substr(colon + 1, bracket - colon - 1);
            if (num.empty()) return "";
            if (!std::all_of(num.begin(), num.end(), [](unsigned char c) { return std::isdigit(c) != 0; })) return "";

            uint64_t accountId = std::stoull(num);
            std::string id = std::to_string(76561197960265728ULL + accountId);
            if (IsValidSteamID64(id)) return id;
        }
        catch (...) {}
    }
    return "";
}

std::string ExtractVanityName(const std::string& input) {
    auto pos = input.find("/id/");
    if (pos != std::string::npos) {
        std::string rest = input.substr(pos + 4);
        size_t cut = rest.find_first_of("/?");
        if (cut != std::string::npos) rest = rest.substr(0, cut);
        return rest;
    }
    if (input.find("/profiles/") == std::string::npos &&
        input.rfind("STEAM_", 0) != 0 &&
        input.rfind("[U:1:", 0) != 0 &&
        !(input.size() == 17 && std::all_of(input.begin(), input.end(),
            [](unsigned char c) { return std::isdigit(c) != 0; }))) {
        return input;
    }
    return "";
}

std::string ResolveVanityToSteamID64(const std::string& vanityName) {
    if (vanityName.empty()) return "";
    std::string xml = HttpGet("https://steamcommunity.com/id/" + vanityName + "/?xml=1");
    if (xml.empty() || xml.find("<error>") != std::string::npos) return "";
    return ExtractTag(xml, "steamID64");
}

void CheckPlayerProfile(const std::string& rawInput) {
    g_playerProfile = PlayerProfile{};
    g_playerProfile.isLoading = true;
    g_playerProfile.steamID = rawInput;

    std::thread([rawInput]() {
        std::string sid = ExtractSteamID64FromInput(rawInput);
        if (sid.empty()) {
            std::string vanity = ExtractVanityName(rawInput);
            if (!vanity.empty()) sid = ResolveVanityToSteamID64(vanity);
        }
        if (sid.empty()) {
            g_playerProfile.isLoading = false;
            g_playerProfile.error = true;
            g_playerProfile.errorMsg = L ? L->ProfileNotFound : "Profile not found";
            return;
        }
        std::string xml = HttpGet("https://steamcommunity.com/profiles/" + sid + "/?xml=1");
        if (xml.empty() || xml.find("<error>") != std::string::npos) {
            g_playerProfile.isLoading = false;
            g_playerProfile.error = true;
            g_playerProfile.errorMsg = L ? L->ProfileNotFound : "Profile not found";
            return;
        }
        g_playerProfile.playerName = ExtractTag(xml, "steamID");
        g_playerProfile.realName = ExtractTag(xml, "realname");
        g_playerProfile.country = ExtractTag(xml, "location");
        g_playerProfile.memberSince = ExtractTag(xml, "memberSince");
        g_playerProfile.steamID = sid;
        try {
            std::string v = ExtractTag(xml, "vacBanned");
            g_playerProfile.vacBans = (!v.empty() && v != "0") ? 1 : 0;
            std::string ng = ExtractTag(xml, "numberOfGameBans");
            if (!ng.empty()) g_playerProfile.gameBans = std::stoi(ng);
            std::string ds = ExtractTag(xml, "daysSinceLastBan");
            if (!ds.empty()) g_playerProfile.daysSince = std::stoi(ds);
            if (g_playerProfile.playerName.empty()) g_playerProfile.isPrivate = true;
        }
        catch (...) {}
        g_playerProfile.isLoading = false;
        g_playerProfile.hasData = true;
        }).detach();
}

// ===================== REGISTRY =====================
void ReadRegistryKeyRecursive(HKEY hKey, const std::string& basePath, int depth,
    std::vector<RegistryFinding>& out, const std::string& regPathName, HKEY rootKey) {
    if (depth > 3) return;

    char subName[512];
    DWORD subIdx = 0;
    DWORD subNameLen = sizeof(subName);

    char valName[512];
    DWORD valIdx = 0;
    DWORD valNameLen = sizeof(valName);
    while (RegEnumValueA(hKey, valIdx++, valName, &valNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        BYTE data[2048]{};
        DWORD dataSize = sizeof(data);
        DWORD type = 0;
        if (RegQueryValueExA(hKey, valName, nullptr, &type, data, &dataSize) == ERROR_SUCCESS) {
            std::string valStr;
            if (type == REG_SZ || type == REG_EXPAND_SZ)
                valStr = std::string((char*)data, strnlen((char*)data, dataSize));

            RegistryFinding f;
            f.pathName = regPathName;
            f.valueName = valName;
            f.value = valStr;
            f.suspicious = ContainsKeyword(valName) || ContainsKeyword(valStr);
            out.push_back(f);
            g_scannedCount++;   // v3.3 FIX: считаем и в рекурсии
        }
        valNameLen = sizeof(valName);
    }

    while (RegEnumKeyExA(hKey, subIdx++, subName, &subNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        HKEY hSub = nullptr;
        std::string full = basePath + "\\" + subName;
        if (RegOpenKeyExA(rootKey, full.c_str(), 0, KEY_READ, &hSub) == ERROR_SUCCESS) {
            ReadRegistryKeyRecursive(hSub, full, depth + 1, out, regPathName, rootKey);
            RegCloseKey(hSub);
        }
        subNameLen = sizeof(subName);
    }
}

// v3.3: прокачанная версия — обрабатывает больше типов значений и добавляет в результаты
void ScanRegistryPaths() {
    g_statusKey = "scan_registry"; g_cancelScan = false; g_scannedCount = 0;

    std::lock_guard<std::mutex> lk(g_registryMutex);
    g_registryFindings.clear();

    for (auto& rp : g_regPaths) {
        if (g_cancelScan) break;
        HKEY hKey = nullptr;
        if (RegOpenKeyExA(rp.root, rp.subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) continue;

        if (rp.isFolder) {
            ReadRegistryKeyRecursive(hKey, rp.subkey, 0, g_registryFindings, rp.name, rp.root);
        }
        else {
            char valName[512];
            DWORD valIdx = 0;
            DWORD valNameLen = sizeof(valName);
            while (RegEnumValueA(hKey, valIdx++, valName, &valNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                BYTE data[2048]{};
                DWORD dataSize = sizeof(data);
                DWORD type = 0;
                if (RegQueryValueExA(hKey, valName, nullptr, &type, data, &dataSize) == ERROR_SUCCESS) {
                    std::string valStr;
                    if (type == REG_SZ || type == REG_EXPAND_SZ)
                        valStr = std::string((char*)data, strnlen((char*)data, dataSize));

                    RegistryFinding f;
                    f.pathName = rp.name;
                    f.valueName = valName;
                    f.value = valStr;
                    f.suspicious = ContainsKeyword(valName) || ContainsKeyword(valStr);
                    g_registryFindings.push_back(f);
                    g_scannedCount++;
                }
                valNameLen = sizeof(valName);
            }
        }
        RegCloseKey(hKey);
    }

    // v3.3: все suspicious findings попадают в общий список результатов
    for (auto& f : g_registryFindings) {
        if (f.suspicious) {
            AddResult(
                f.pathName + " → " + f.valueName,
                "HK: " + f.pathName,
                "Registry",
                "Suspicious registry entry",
                4);
        }
    }

    g_statusKey = "ready"; g_scanRunning = false;
}

// ===================== TOOLS / PROCESS LIST / TEXTURES / FOLDERS =====================
void RefreshToolsList() {
    for (auto& t : g_tools) {
        std::string exePath = "Tools\\" + t.folderName + "\\" + t.exeName;
        t.category = fs::exists(exePath) ? t.category : "missing_" + t.category;
    }
}

void LaunchTool(const ToolEntry& tool) {
    std::string exePath = "Tools\\" + tool.folderName + "\\" + tool.exeName;
    if (!fs::exists(exePath)) {
        MessageBoxA(nullptr, L ? L->ToolNotFound : "Tool not found", "Oops", MB_ICONWARNING);
        return;
    }
    ShellExecuteA(nullptr, "open", exePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void OpenToolFolder(const ToolEntry& tool) {
    std::string folder = "Tools\\" + tool.folderName;
    if (fs::exists(folder)) {
        ShellExecuteA(nullptr, "open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    else {
        ShellExecuteA(nullptr, "open", "Tools", nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void AutoSelectCS2() {
    for (int i = 0; i < (int)g_processList.size(); ++i) {
        std::string low = ToLower(g_processList[i]);
        if (low.find("cs2.exe") != std::string::npos) {
            g_selectedProcess = i; g_cs2AutoSelected = true; return;
        }
    }
    for (int i = 0; i < (int)g_processList.size(); ++i) {
        std::string low = ToLower(g_processList[i]);
        if (low.find("csgo.exe") != std::string::npos) {
            g_selectedProcess = i; g_cs2AutoSelected = true; return;
        }
    }
}

void RefreshProcessList() {
    g_processList.clear();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe{ sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            std::string name = WStringToString(pe.szExeFile);
            g_processList.push_back(name + " (PID: " + std::to_string(pe.th32ProcessID) + ")");
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    if (!g_cs2AutoSelected) AutoSelectCS2();
}

void InitFolders() {
    fs::create_directories("assets");
    fs::create_directories("settings");
    fs::create_directories("logs");
    fs::create_directories("Tools");
}

GLuint LoadTextureFromFile(const char* filename) {
    if (!fs::exists(filename)) return 0;
    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(filename, &w, &h, &channels, 4);
    if (!data) return 0;
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return texture;
}

// ===================== СТАТУСЫ / ЯЗЫКИ =====================
const StatusString STATUS_TABLE[] = {
    { "ready",           "Ready",                     "Готов" },
    { "scan_files",      "Scanning files...",         "Сканирую файлы..." },
    { "scan_processes",  "Scanning processes...",     "Сканирую процессы..." },
    { "scan_net",        "Analyzing network...",      "Анализирую сеть..." },
    { "scan_tasks",      "Checking tasks...",         "Проверяю задачи..." },
    { "scan_hidden",     "Searching hidden...",       "Ищу скрытое..." },
    { "scan_signatures", "Signature analysis...",     "Анализ сигнатур..." },
    { "scan_services",   "Checking services...",      "Проверяю службы..." },
    { "scan_autorun",    "Scanning autorun...",       "Сканирую автозапуск..." },
    { "scan_memory",     "Scanning memory...",        "Сканирую память..." },
    { "scan_activity",   "Analyzing activity...",     "Анализирую активность..." },
    { "scan_steam",      "Reading Steam...",          "Читаю Steam..." },
    { "scan_browser",    "Analyzing browser...",      "Анализирую браузер..." },
    { "scan_registry",   "Reading registry...",       "Читаю реестр..." },
};

std::string GetStatusText() {
    for (const auto& s : STATUS_TABLE) {
        if (g_statusKey == s.key) return g_isRussian ? s.ru : s.en;
    }
    return g_isRussian ? "Готов" : "Ready";
}

// ===================== ЯЗЫКОВЫЕ ПАКИ =====================
LangPack L_EN = {
    "Home","Files","Processes","Network",
    "Activity","USB","Registry","File Search",
    "Steam Accounts","Recovery","Process Strings","Tools",
    "Drivers","Autorun","Services","Check Sites",
    "System Folders",
    "Report","Profile","Info",
    "MAIN","ANALYSIS","ADVANCED","REPORT",
    "Start Scan","Stop Scan","Clear Results",
    "Export JSON","Export HTML","Export CSV",
    "Clear Log","IGNORE LIST","Save","Reload",
    "Ready","Scanning...","Admin","NO ADMIN",
    "Search:","Type to filter...","Status:","Scanned:","Hits:",
    "Target Process:","Refresh List","No processes","Total findings:","No data. Run a scan first.",
    "Type","Name","Reason","Path","Open folder",
    "Player Profile Check","Steam ID / Link:","Check Profile",
    "Bans","No bans found","Checking...","Enter Steam ID, profile link or vanity name",
    "Profile not found","VAC Ban","Game Ban","Days since last ban",
    "Member since","Country","Real name","No profile data.","Clear",
    "Report saved","Error writing file","No results to export",
    "About the author","Made by Chmonya. If you got it from somewhere else - be careful, might be a scam.",
    "How to use",
    "1. Pick a tab on top.\n2. Hit 'Start Scan'.\n3. Wait for results.\n4. Double-click a row to open the file.\n5. Export via Report tab.\n\nTip: run as admin so nothing gets skipped.",
    "Links","Memes","Developer",
    "SETTINGS","Sound on findings","Auto-start with Windows",
    "History is empty","Clear history","Open",
    "Open all","Open folder","Launch",
    "Copy to clipboard","Copied!",
    "Suspicious programs from UserAssist/BAM/MUICache","Connected USB devices and storage",
    "Registry keys with traces of launched programs","Search any file by name or mask",
    "Steam accounts that logged in on this PC","Recover deleted files to find hidden traces",
    "Scan process memory for cheat strings","Tools from /Tools folder",
    "Open popular cheat-sites in browser to check history","System folders and quick access",
    "Welcome to CS2 Cheat Checker by Chmonya","Ultimate cheat checker for your PC",
    "Total findings","Scans","Version","Status",
    "Search file name","Select folder","Folder path","Scan folder","Steam accounts found","No Steam accounts",
    "Run tool","Tool not found","Open tools folder",
    "About","Made in Russia with love and beer"
};

LangPack L_RU = {
    "Главная","Файлы","Процессы","Сеть",
    "Анализ активности","USB и устройства","Реестр","Поиск файлов",
    "Steam аккаунты","Восстановление","Строки процессов","Утилиты",
    "Драйверы","Автозапуск","Службы","Проверка сайтов",
    "Системные папки",
    "Отчёт","Профиль","Инфо",
    "ГЛАВНОЕ","АНАЛИЗ","ПРОДВИНУТОЕ","ОТЧЁТ",
    "Начать","Остановить","Очистить",
    "Экспорт JSON","Экспорт HTML","Экспорт CSV",
    "Очистить лог","СПИСОК ИГНОРА","Сохранить","Обновить",
    "Готов","Сканирование...","Админ","НЕТ АДМИНА",
    "Поиск:","Введите для фильтра...","Статус:","Просканировано:","Найдено:",
    "Целевой процесс:","Обновить","Нет процессов","Всего найдено:","Нет данных.",
    "Тип","Имя","Причина","Путь","Открыть папку",
    "Проверка профиля","Steam ID / Ссылка:","Проверить",
    "Баны","Банов не найдено","Проверка...","Введите Steam ID, ссылку или vanity-имя",
    "Профиль не найден","VAC Бан","Игровой бан","Дней с последнего бана",
    "Дата регистрации","Страна","Реальное имя","Нет данных.","Очистить",
    "Отчёт сохранён","Ошибка записи","Нечего экспортировать",
    "Об авторе","Эту тулу сделал Chmonya. Если скачал её с левого сайта — будь осторожен, может быть скам.",
    "Как пользоваться",
    "1. Выбери вкладку сверху.\n2. Нажми 'Начать'.\n3. Дождись результатов.\n4. Двойной клик по строке — открыть файл.\n5. Экспорт на вкладке Отчёт.\n\nСовет: запускай от админа.",
    "Ссылки","Мемы","Разработчик",
    "НАСТРОЙКИ","Звук при находке","Автозапуск с Windows",
    "История пуста","Очистить историю","Открыть",
    "Открыть все","Открыть папку","Запустить",
    "Скопировать в буфер","Скопировано!",
    "Подозрительные программы из UserAssist/BAM/MUICache","Подключённые USB-устройства и накопители",
    "Ключи реестра со следами запущенных программ","Поиск любого файла по имени или маске",
    "Steam-аккаунты, которые заходили на этот ПК","Восстановление удалённых файлов для поиска скрытых следов",
    "Поиск чит-строк в памяти процессов","Утилиты из папки /Tools",
    "Открытие популярных чит-сайтов для проверки в истории браузера","Системные папки и быстрый доступ",
    "Добро пожаловать в CS2 Cheat Checker by Chmonya","Лучший чекер читов для твоего ПК",
    "Всего найдено","Сканов","Версия","Статус",
    "Имя файла","Выбрать папку","Путь к папке","Сканировать папку","Найдено Steam аккаунтов","Steam аккаунты не найдены",
    "Запустить утилиту","Утилита не найдена","Открыть папку Tools",
    "О программе","Сделано в России с любовью и пивом"
};

struct LPackInit {
    LPackInit() { L = &L_RU; }
} g_lpackInit;

// ===================== СКАНЕРЫ =====================

// ---- ФАЙЛЫ ----
void ScanFiles() {
    g_statusKey = "scan_files"; g_cancelScan = false; g_scannedCount = 0;
    g_taskQueue.clear();
    const size_t MAX_DEPTH = 5, MAX_FILES = SCAN_FILES_MAX;

    std::string desktop = GetFolderPath(FOLDERID_Desktop);
    if (!desktop.empty()) g_taskQueue.push(desktop);
    std::string docs = GetFolderPath(FOLDERID_Documents);
    if (!docs.empty()) {
        fs::path docPath(docs);
        g_taskQueue.push((docPath / ".." / "Downloads").lexically_normal().string());
        g_taskQueue.push(docs);
    }
    std::string appdata = GetFolderPath(FOLDERID_RoamingAppData);
    if (!appdata.empty()) g_taskQueue.push(appdata);
    std::string localappdata = GetFolderPath(FOLDERID_LocalAppData);
    if (!localappdata.empty()) g_taskQueue.push(localappdata);
    g_taskQueue.push("C:\\Users");
    g_taskQueue.push("C:\\ProgramData");

    size_t threads = std::max<size_t>(2, (size_t)std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    std::atomic<size_t> scannedCount{ 0 };
    std::atomic<bool> stopFlag{ false };

    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([&]() {
            std::string path;
            while (!g_cancelScan && !stopFlag && g_taskQueue.pop(path)) {
                try {
                    for (auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
                        if (g_cancelScan || stopFlag) break;
                        if (scannedCount.load() >= MAX_FILES) { stopFlag = true; break; }
                        std::string pstr = WStringToString(entry.path().wstring());
                        if (IsWhitelisted(pstr)) continue;
                        if (entry.is_directory()) {
                            size_t depth = std::count(pstr.begin(), pstr.end(), '\\');
                            if (depth < MAX_DEPTH) g_taskQueue.push(pstr);
                            continue;
                        }
                        scannedCount++; g_scannedCount++;
                        std::string fname = WStringToString(entry.path().filename().wstring());
                        if (ContainsKeyword(fname)) {
                            AddResult(fname, pstr, "File", "Keyword in filename", 2);
                            continue;
                        }
                        auto ext = ToLower(WStringToString(entry.path().extension().wstring()));
                        if (ext == ".exe" || ext == ".dll" || ext == ".sys") {
                            bool hasValidSignature = VerifyFileSignature(entry.path().wstring());
                            if (FileHasKeywordContent(entry.path())) {
                                int severity = hasValidSignature ? 1 : 3;
                                AddResult(fname, pstr, "File",
                                    hasValidSignature ? "Keyword content (signed)" : "Keyword content (unsigned)",
                                    severity);
                            }
                        }
                    }
                }
                catch (...) {}
                g_taskQueue.task_done();
            }
            });
    }
    for (auto& t : workers) if (t.joinable()) t.join();
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ПРОЦЕССЫ ----
void ScanProcesses() {
    g_statusKey = "scan_processes"; g_cancelScan = false; g_scannedCount = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) { g_statusKey = "ready"; g_scanRunning = false; return; }

    PROCESSENTRY32W pe{ sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (g_cancelScan) break;
            g_scannedCount++;
            std::string name = WStringToString(pe.szExeFile);
            std::string fullPath = name;
            HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (hp) {
                char buf[MAX_PATH * 2]{}; DWORD sz = sizeof(buf);
                if (QueryFullProcessImageNameA(hp, 0, buf, &sz)) fullPath = buf;
                CloseHandle(hp);
            }
            if (ContainsKeyword(name) || ContainsKeyword(fullPath)) {
                AddResult(name, fullPath + " | PID " + std::to_string(pe.th32ProcessID),
                    "Process", "Keyword in process", 4);
            }
            ScanModulesInProcess(pe.th32ProcessID, name);
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- СЕТЬ ----
void ScanNet() {
    g_statusKey = "scan_net"; g_cancelScan = false; g_scannedCount = 0;
    DWORD size = 0;
    GetExtendedTcpTable(nullptr, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    std::vector<BYTE> buf(size);
    if (GetExtendedTcpTable(buf.data(), &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
        auto* table = (PMIB_TCPTABLE_OWNER_PID)buf.data();
        for (DWORD i = 0; i < table->dwNumEntries; ++i) {
            if (g_cancelScan) break;
            g_scannedCount++;
            DWORD pid = table->table[i].dwOwningPid;
            std::string pname = "pid:" + std::to_string(pid);
            HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            char img[MAX_PATH]{};
            if (hp) {
                DWORD sz = MAX_PATH;
                if (QueryFullProcessImageNameA(hp, 0, img, &sz)) pname = img;
                CloseHandle(hp);
            }
            if (ContainsKeyword(pname)) {
                AddResult(fs::path(pname).filename().string(), pname, "Network",
                    "Suspicious network", 4);
            }
        }
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ЗАДАЧИ ----
void ScanTaskScheduler() {
    g_statusKey = "scan_tasks"; g_cancelScan = false; g_scannedCount = 0;
    std::vector<std::string> dirs = { "C:\\Windows\\System32\\Tasks", "C:\\Windows\\Tasks" };
    for (auto& dir : dirs) {
        try {
            if (!fs::exists(dir)) continue;
            for (auto& e : fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied)) {
                if (g_cancelScan) break;
                if (!e.is_regular_file()) continue;
                g_scannedCount++;
                auto name = WStringToString(e.path().filename().wstring());
                if (ContainsKeyword(name) || FileHasKeywordContent(e.path(), 256 * 1024)) {
                    AddResult(name, WStringToString(e.path().wstring()), "Task",
                        "Suspicious task", 3);
                }
            }
        }
        catch (...) {}
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- СКРЫТЫЕ ----
void ScanHidden() {
    g_statusKey = "scan_hidden"; g_cancelScan = false; g_scannedCount = 0;
    std::vector<std::string> roots;
    std::string a = GetFolderPath(FOLDERID_RoamingAppData); if (!a.empty()) roots.push_back(a);
    std::string l = GetFolderPath(FOLDERID_LocalAppData);   if (!l.empty()) roots.push_back(l);
    roots.push_back("C:\\ProgramData");
    roots.push_back("C:\\Users");

    for (auto& root : roots) {
        try {
            for (auto& e : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
                if (g_cancelScan) break;
                std::string pstr = WStringToString(e.path().wstring());
                if (IsWhitelisted(pstr)) continue;
                g_scannedCount++;
                DWORD attr = GetFileAttributesA(pstr.c_str());
                if (attr == INVALID_FILE_ATTRIBUTES) continue;
                if ((attr & FILE_ATTRIBUTE_HIDDEN) || (attr & FILE_ATTRIBUTE_SYSTEM)) {
                    auto name = WStringToString(e.path().filename().wstring());
                    if (ContainsKeyword(name)) {
                        AddResult(name, pstr, "Hidden", "Hidden keyword", 3);
                    }
                }
            }
        }
        catch (...) {}
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- СИГНАТУРЫ ----
void ScanSignatures() {
    g_statusKey = "scan_signatures"; g_cancelScan = false; g_scannedCount = 0;
    std::vector<std::string> hotPaths;
    std::string l = GetFolderPath(FOLDERID_LocalAppData);
    if (!l.empty()) { hotPaths.push_back(l + "\\Temp"); hotPaths.push_back(l); }
    std::string a = GetFolderPath(FOLDERID_RoamingAppData);
    if (!a.empty()) hotPaths.push_back(a);
    hotPaths.push_back("C:\\ProgramData");
    hotPaths.push_back("C:\\Users\\Public");

    for (auto& root : hotPaths) {
        try {
            if (!fs::exists(root)) continue;
            for (auto& e : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
                if (g_cancelScan) break;
                std::string pstr = WStringToString(e.path().wstring());
                if (!e.is_regular_file() || IsWhitelisted(pstr)) continue;
                g_scannedCount++;
                auto name = WStringToString(e.path().filename().wstring());
                auto ext = ToLower(WStringToString(e.path().extension().wstring()));
                bool hit = ContainsKeyword(name);
                if (!hit && (ext == ".exe" || ext == ".dll" || ext == ".sys")) {
                    bool hasSig = VerifyFileSignature(e.path().wstring());
                    hit = FileHasKeywordContent(e.path(), 1024 * 1024);
                    if (hit && !hasSig) {
                        AddResult(name, pstr, "Signature", "Unsigned keyword", 4);
                        continue;
                    }
                }
                if (!hit && ToLower(name) == "imgui.ini") hit = true;
                if (hit) AddResult(name, pstr, "Signature", "Signature hit", 2);
            }
        }
        catch (...) {}
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- СЛУЖБЫ ----
void ScanServices() {
    g_statusKey = "scan_services"; g_cancelScan = false; g_scannedCount = 0;
    SC_HANDLE scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) { g_statusKey = "ready"; g_scanRunning = false; return; }

    DWORD bn = 0, count = 0, resume = 0;
    EnumServicesStatusExA(scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
        nullptr, 0, &bn, &count, &resume, nullptr);
    std::vector<BYTE> buf(bn + 16);

    if (EnumServicesStatusExA(scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
        buf.data(), (DWORD)buf.size(), &bn, &count, &resume, nullptr)) {
        auto* svc = (LPENUM_SERVICE_STATUS_PROCESSA)buf.data();
        for (DWORD i = 0; i < count; ++i) {
            if (g_cancelScan) break;
            g_scannedCount++;
            std::string name = svc[i].lpServiceName ? svc[i].lpServiceName : "";
            std::string disp = svc[i].lpDisplayName ? svc[i].lpDisplayName : "";
            if (ContainsKeyword(name) || ContainsKeyword(disp)) {
                AddResult(disp.empty() ? name : disp, name, "Service",
                    "Suspicious service", 4);
            }
        }
    }
    CloseServiceHandle(scm);
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- АВТОЗАПУСК ----
void ScanAutorun() {
    g_statusKey = "scan_autorun"; g_cancelScan = false; g_scannedCount = 0;
    struct KT { HKEY root; const char* sub; };
    KT targets[] = {
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce" },
        { HKEY_CURRENT_USER,  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" },
        { HKEY_CURRENT_USER,  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce" },
        { HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run" },
    };
    for (auto& t : targets) {
        HKEY hKey;
        if (RegOpenKeyExA(t.root, t.sub, 0, KEY_READ, &hKey) != ERROR_SUCCESS) continue;
        char vn[256];
        DWORD idx = 0, ns = sizeof(vn);
        while (RegEnumValueA(hKey, idx++, vn, &ns, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            g_scannedCount++;
            char data[2048]{}; DWORD ds = sizeof(data);
            if (RegQueryValueExA(hKey, vn, NULL, NULL, (LPBYTE)data, &ds) == ERROR_SUCCESS) {
                std::string dstr = data, vname = vn;
                if (ContainsKeyword(dstr) || ContainsKeyword(vname)) {
                    AddResult("Autorun: " + vname, dstr, "Registry", t.sub, 4);
                }
            }
            ns = sizeof(vn);
        }
        RegCloseKey(hKey);
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ПАМЯТЬ (binary-safe) ----
void ScanMemory() {
    g_statusKey = "scan_memory"; g_cancelScan = false; g_scannedCount = 0;
    if (g_processList.empty() || g_selectedProcess < 0 || g_selectedProcess >= (int)g_processList.size()) {
        g_statusKey = "ready"; g_scanRunning = false; return;
    }
    std::string selected = g_processList[g_selectedProcess];
    size_t start = selected.find("PID: "), end = selected.find(")", start);
    if (start == std::string::npos || end == std::string::npos) {
        g_statusKey = "ready"; g_scanRunning = false; return;
    }
    DWORD pid = std::stoi(selected.substr(start + 5, end - start - 5));
    ScopedHandle hp(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
    if (!hp.valid()) { g_statusKey = "ready"; g_scanRunning = false; return; }

    SYSTEM_INFO si; GetSystemInfo(&si);
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t addr = (uintptr_t)si.lpMinimumApplicationAddress;
    uintptr_t maxa = (uintptr_t)si.lpMaximumApplicationAddress;
    const size_t CH = 1024 * 1024;
    std::vector<char> buffer;

    while (addr < maxa) {
        if (g_cancelScan) break;
        if (VirtualQueryEx(hp.get(), (LPCVOID)addr, &mbi, sizeof(mbi)) == 0) { addr += 0x1000; continue; }
        if (mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_READWRITE)) {
            uintptr_t rs = (uintptr_t)mbi.BaseAddress, re = rs + mbi.RegionSize;
            for (uintptr_t cur = rs; cur < re && !g_cancelScan; cur += CH) {
                size_t toRead = (re - cur < CH) ? (size_t)(re - cur) : CH;
                buffer.resize(toRead);
                SIZE_T br = 0;
                if (ReadProcessMemory(hp.get(), (LPCVOID)cur, buffer.data(), toRead, &br) && br > 0) {
                    if (ContainsKeywordRaw(buffer.data(), (size_t)br)) {
                        AddResult("PID " + std::to_string(pid),
                            "Memory at 0x" + std::to_string(cur),
                            "Memory", "Keyword in memory", 4);
                        break;
                    }
                }
            }
        }
        addr += mbi.RegionSize;
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ДРАЙВЕРЫ ----
void ScanDrivers() {
    g_statusKey = "scan_services"; g_cancelScan = false; g_scannedCount = 0;

    static const std::vector<std::string> knownVulnDrivers = {
        "kdmapper.sys", "kdu.sys", "iqvw64e.sys", "capcom.sys",
        "gdrv.sys", "rtcore64.sys", "msio64.sys", "dbk64.sys",
        "ene.sys", "asusio.sys", "winio.sys", "winring0.sys"
    };

    LPVOID drivers[1024];
    DWORD cbNeeded = 0;
    if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        DWORD count = cbNeeded / sizeof(LPVOID);
        for (DWORD i = 0; i < count; ++i) {
            if (g_cancelScan) break;
            g_scannedCount++;
            char driverName[MAX_PATH]{};
            if (GetDeviceDriverBaseNameA(drivers[i], driverName, MAX_PATH)) {
                std::string name = driverName;
                std::string low = ToLower(name);
                for (auto& vuln : knownVulnDrivers) {
                    if (low == vuln) {
                        AddResult(name, "Kernel driver", "Driver", "Known vulnerable driver", 5);
                        break;
                    }
                }
                if (ContainsKeyword(name)) {
                    AddResult(name, "Kernel driver", "Driver", "Suspicious driver name", 4);
                }
            }
        }
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- АКТИВНОСТЬ ----
void ScanActivity() {
    g_statusKey = "scan_activity"; g_cancelScan = false; g_scannedCount = 0;

    // UserAssist (ROT13)
    {
        std::string basePath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist";
        HKEY hBase;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, basePath.c_str(), 0, KEY_READ, &hBase) == ERROR_SUCCESS) {
            char subGuid[512];
            DWORD gi = 0, gLen = sizeof(subGuid);
            while (RegEnumKeyExA(hBase, gi++, subGuid, &gLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                std::string guidPath = basePath + "\\" + subGuid + "\\Count";
                HKEY hCount;
                if (RegOpenKeyExA(HKEY_CURRENT_USER, guidPath.c_str(), 0, KEY_READ, &hCount) == ERROR_SUCCESS) {
                    char vn[512]; DWORD vi = 0, vLen = sizeof(vn);
                    while (RegEnumValueA(hCount, vi++, vn, &vLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                        std::string decoded = vn;
                        for (auto& c : decoded) {
                            if (c >= 'a' && c <= 'z') c = (c - 'a' + 13) % 26 + 'a';
                            else if (c >= 'A' && c <= 'Z') c = (c - 'A' + 13) % 26 + 'A';
                        }
                        if (ContainsKeyword(decoded)) {
                            AddResult(decoded, "UserAssist\\" + std::string(subGuid),
                                "Activity", "Suspicious program in UserAssist", 4);
                        }
                        g_scannedCount++;
                        vLen = sizeof(vn);
                    }
                    RegCloseKey(hCount);
                }
                gLen = sizeof(subGuid);
            }
            RegCloseKey(hBase);
        }
    }

    // BAM
    {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char sub[512]; DWORD si = 0, sLen = sizeof(sub);
            while (RegEnumKeyExA(hKey, si++, sub, &sLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                HKEY hUser;
                std::string userPath = std::string("SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings\\") + sub;
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, userPath.c_str(), 0, KEY_READ, &hUser) == ERROR_SUCCESS) {
                    char vn[512]; DWORD vi = 0, vLen = sizeof(vn);
                    while (RegEnumValueA(hUser, vi++, vn, &vLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                        std::string path = vn;
                        std::string file = fs::path(path).filename().string();
                        if (ContainsKeyword(file) || ContainsKeyword(path)) {
                            AddResult(file, path, "Activity", "Suspicious in BAM", 4);
                        }
                        g_scannedCount++;
                        vLen = sizeof(vn);
                    }
                    RegCloseKey(hUser);
                }
                sLen = sizeof(sub);
            }
            RegCloseKey(hKey);
        }
    }

    // MUICache
    {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER,
            "SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char vn[512]; DWORD vi = 0, vLen = sizeof(vn);
            while (RegEnumValueA(hKey, vi++, vn, &vLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                std::string path = vn;
                std::string file = fs::path(path).filename().string();
                if (ContainsKeyword(file) || ContainsKeyword(path)) {
                    AddResult(file, path, "Activity", "Suspicious in MUICache", 3);
                }
                g_scannedCount++;
                vLen = sizeof(vn);
            }
            RegCloseKey(hKey);
        }
    }

    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- USB ----
void ScanUSBDevices() {
    g_statusKey = "ready"; g_cancelScan = false; g_scannedCount = 0;

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Enum\\USBSTOR",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {

        char sub[512]; DWORD si = 0, sLen = sizeof(sub);
        while (RegEnumKeyExA(hKey, si++, sub, &sLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
            if (g_cancelScan) break;
            HKEY hDev;
            std::string devPath = std::string("SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\") + sub;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, devPath.c_str(), 0, KEY_READ, &hDev) == ERROR_SUCCESS) {
                char inst[512]; DWORD ii = 0, iLen = sizeof(inst);
                while (RegEnumKeyExA(hDev, ii++, inst, &iLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                    HKEY hInst;
                    std::string instPath = devPath + "\\" + inst;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, instPath.c_str(), 0, KEY_READ, &hInst) == ERROR_SUCCESS) {
                        char friendlyName[512]{};
                        DWORD sz = sizeof(friendlyName);
                        RegQueryValueExA(hInst, "FriendlyName", nullptr, nullptr, (LPBYTE)friendlyName, &sz);
                        std::string fname = friendlyName[0] ? friendlyName : sub;

                        AddResult(fname, instPath, "USB", "USB device", 1);
                        g_scannedCount++;
                        RegCloseKey(hInst);
                    }
                    iLen = sizeof(inst);
                }
                RegCloseKey(hDev);
            }
            sLen = sizeof(sub);
        }
        RegCloseKey(hKey);
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- STEAM ----
void ScanSteamAccounts() {
    g_statusKey = "scan_steam"; g_cancelScan = false; g_scannedCount = 0;

    std::string steamPath = FindSteamInstallPath();
    if (steamPath.empty()) {
        g_statusKey = "ready"; g_scanRunning = false;
        return;
    }

    std::string vdfPath = steamPath + "\\config\\loginusers.vdf";
    if (!fs::exists(vdfPath)) {
        g_statusKey = "ready"; g_scanRunning = false;
        return;
    }

    ParseLoginUsersVDF(vdfPath);
    g_scannedCount = g_steamAccounts.size();

    try {
        for (auto& e : fs::recursive_directory_iterator(steamPath + "\\steamapps\\common",
            fs::directory_options::skip_permission_denied)) {
            if (g_cancelScan) break;
            if (!e.is_regular_file()) continue;
            g_scannedCount++;
            auto fname = WStringToString(e.path().filename().wstring());
            if (ContainsKeyword(fname)) {
                AddResult(fname, WStringToString(e.path().wstring()), "Steam",
                    "Suspicious file in Steam folder", 4);
            }
        }
    }
    catch (...) {}

    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- БРАУЗЕР ----
void ScanBrowserHistory() {
    g_statusKey = "scan_browser"; g_cancelScan = false; g_scannedCount = 0;

    std::vector<std::string> browserPaths;
    std::string localAppData = GetFolderPath(FOLDERID_LocalAppData);
    std::string roamingAppData = GetFolderPath(FOLDERID_RoamingAppData);

    if (!localAppData.empty()) {
        browserPaths.push_back(localAppData + "\\Google\\Chrome\\User Data\\Default\\History");
        browserPaths.push_back(localAppData + "\\Microsoft\\Edge\\User Data\\Default\\History");
        browserPaths.push_back(localAppData + "\\BraveSoftware\\Brave-Browser\\User Data\\Default\\History");
        browserPaths.push_back(localAppData + "\\Yandex\\YandexBrowser\\User Data\\Default\\History");
    }
    if (!roamingAppData.empty()) {
        browserPaths.push_back(roamingAppData + "\\Mozilla\\Firefox\\Profiles");
        browserPaths.push_back(roamingAppData + "\\Opera Software\\Opera Stable\\History");
    }

    std::vector<std::string> cheatDomains = {
        "xone.fun", "midnight.im", "aimware.net", "nixware.cc",
        "onetap.com", "neverlose.cc", "interium.ooo", "yougame.biz",
        "lolz.market", "gamesense.pub", "fatality.win", "primordial.dev",
        "memesense.gg", "spirthack.me", "ev0lve.xyz", "legendware.pw",
        "exloader.ru", "aimware.wtf"
    };

    for (auto& path : browserPaths) {
        if (g_cancelScan) break;
        try {
            if (fs::is_regular_file(path)) {
                std::ifstream ifs(path, std::ios::binary);
                if (!ifs) continue;
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                g_scannedCount++;
                std::string lowContent = ToLower(content);
                for (auto& domain : cheatDomains) {
                    if (lowContent.find(domain) != std::string::npos) {
                        AddResult(domain, path, "Browser", "Visited cheat site: " + domain, 4);
                    }
                }
            }
            else if (fs::is_directory(path)) {
                for (auto& e : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
                    if (g_cancelScan) break;
                    if (!e.is_regular_file()) continue;
                    std::string fname = e.path().filename().string();
                    if (fname != "places.sqlite" && fname != "History") continue;

                    std::ifstream ifs(e.path(), std::ios::binary);
                    if (!ifs) continue;
                    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                    g_scannedCount++;
                    std::string lowContent = ToLower(content);
                    for (auto& domain : cheatDomains) {
                        if (lowContent.find(domain) != std::string::npos) {
                            AddResult(domain, WStringToString(e.path().wstring()), "Browser",
                                "Visited cheat site: " + domain, 4);
                        }
                    }
                }
            }
        }
        catch (...) {}
    }
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- STRINGS EXTRACTOR ----
std::vector<std::string> ExtractStrings(const char* data, size_t len, size_t minLen) {
    std::vector<std::string> out;
    std::string current;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)data[i];
        if (c >= 32 && c < 127) {
            current += (char)c;
        }
        else {
            if (current.size() >= minLen) out.push_back(current);
            current.clear();
        }
    }
    if (current.size() >= minLen) out.push_back(current);
    return out;
}

// ---- PROCESS STRINGS (RAII + сырой буфер) ----
void ScanProcessStringsForProcess(DWORD pid, const std::string& processName,
    const std::vector<std::string>& patterns, size_t minLen, const std::string& category)
{
    ScopedHandle hProcess(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
    if (!hProcess.valid()) return;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t addr = (uintptr_t)si.lpMinimumApplicationAddress;
    uintptr_t maxAddr = (uintptr_t)si.lpMaximumApplicationAddress;

    std::vector<char> buffer;
    int matches = 0;

    while (addr < maxAddr && !g_cancelScan && matches < 50) {
        if (VirtualQueryEx(hProcess.get(), (LPCVOID)addr, &mbi, sizeof(mbi)) == 0) {
            addr += 0x1000;
            continue;
        }
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE ||
                mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE))
        {
            size_t regionSize = mbi.RegionSize;
            if (regionSize > 4 * 1024 * 1024) regionSize = 4 * 1024 * 1024;
            buffer.resize(regionSize);
            SIZE_T br = 0;
            if (ReadProcessMemory(hProcess.get(), mbi.BaseAddress, buffer.data(), regionSize, &br) && br > 0) {
                auto strings = ExtractStrings(buffer.data(), (size_t)br, minLen);
                for (auto& s : strings) {
                    std::string low = ToLower(s);
                    for (auto& p : patterns) {
                        if (low.find(ToLower(p)) != std::string::npos) {
                            std::lock_guard<std::mutex> lk(g_processStringMutex);
                            ProcessStringHit hit;
                            hit.processName = processName;
                            hit.pid = pid;
                            hit.foundString = s.substr(0, 200);
                            hit.category = category;
                            g_processStringHits.push_back(hit);
                            AddResult(processName, "String: " + hit.foundString,
                                category, "Match: " + p, 5);
                            matches++;
                            break;
                        }
                    }
                    if (matches >= 50) break;
                }
            }
        }
        addr += mbi.RegionSize;
    }
}

void ScanBrowserStrings() {
    g_statusKey = "scan_browser"; g_cancelScan = false; g_scannedCount = 0;

    std::vector<std::string> patterns = {
        "doomxftc.com", "axios-macro.com", "midnight.im", "xone.fun", "blast.hk",
        "yougame.biz", "jstkll", "wh-satano", "cheats.gg", "interium", "r2cheats",
        "ezcheats", "exloader", "cs-elect.ru", "extrimhack", "neverlose.cc",
        "gamesense", "legendware", "nixware", "phoenix-hack", "rf-cheats",
        "anyx.gg", "hackvshack.net", "ezvhack", "unknowncheats", "cheater.ninja",
        "insanitycheats.com", "cheater.fun", "10cheats.ru", "undetek.com",
        "cheater.world", "zelenka.guru", "cs2-cheat", "procheats",
        "hells-hack.com", "clickhack.ru", "procheat.pro", "cheatermag.com",
        "420cheats.com", "wm-satano.ru", "up-game.pro", "lolz.guru", "lolzteam",
        "yougame", "rutor", "s1mple", "zeuscheats", "beltcheat", "n1kso",
        "cash-cheats", "1mpulse.wtf", "darkaim.ru", "invision.gg", "privated-cheats",
        "csgohacks.ru", "hackfinder.pw", "cs2cheats.ru", "cheatmarket",
        "primecheats.ru", "systemshop.pw", "loader-cheats", "darkaim.pw",
        "invision.pw", "shibari-cheats", "2xcheats.pw", "otcheats.pw",
        "nomorecheats.pw", "cr1mcheats.pw", "pr0mix.pw", "hvhzone.pw",
        "catapult.pw", "tokyo-cheats.pw", "gamblock.pw", "5xxcheats",
        "lovacheats.com", "lua.cat", "hvh-cheats.com", "aimbot.pw",
        "lux-cheats.pw", "cs2cheatmarket.pw", "get-cheats.pw", "inject.pw",
        "cheatlogic.pw", "b1gcheats.pw", "pandora.wtf", "cheatup.pw",
        "ezfrags.pw", "1llusion.pw", "aimware.pw", "germod.pw",
        "neverlose.pw", "getneverlose.pw", "nlcheats.pw", "skeet.pw",
        "gamesense.pw", "onetap.pw", "fatality.pw", "primordial.pw",
        "nemesis.pw", "catalyst.pw", "penguware.pw", "mesa.pw",
        "novoline.pw", "pandora.pw", "weave.pw", "spirthack.pw",
        "ev0lve.pw", "atlantiss.pw", "vape.pw", "cheat.biz", "cheat.market"
    };

    std::vector<std::string> browserExes = {
        "chrome.exe", "msedge.exe", "firefox.exe", "opera.exe",
        "brave.exe", "yandex.exe", "browser.exe"
    };

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) { g_statusKey = "ready"; g_scanRunning = false; return; }

    PROCESSENTRY32W pe{ sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (g_cancelScan) break;
            std::string name = WStringToString(pe.szExeFile);
            std::string low = ToLower(name);
            bool isBrowser = false;
            for (auto& b : browserExes) {
                if (low.find(b) != std::string::npos) { isBrowser = true; break; }
            }
            if (!isBrowser) continue;

            ScanProcessStringsForProcess(pe.th32ProcessID, name, patterns, 6, "Browser String");
            g_scannedCount++;
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    g_statusKey = "ready"; g_scanRunning = false;
}

void ScanCS2Strings() {
    g_statusKey = "scan_memory"; g_cancelScan = false; g_scannedCount = 0;

    std::vector<std::string> patterns = {
        "#unload_popup", "[XONE", "AimStar", "Aimmy", "Whiskey",
        "Mod.t.me", "Spectator List", "INTERIUM", "Enigma", "Luno",
        "pastehook", "NAIM", "plague", "#temp_button_add", "#Removals",
        "ZRK 1.4", "tim_apple", "extrimhack", "SimpleInternal",
        "compkiller", "Cryptic", "Osiris", "Ekknod", "#main_window",
        "#watermark", "#Tooltip_hidden", "aimbot.dll", "wallhack",
        "triggerbot", "cheat.dll", "skinchanger", "autoexec",
        "silent_aim", "backtrack", "desync", "fakelag",
        "hitchance", "resolver", "antiaim", "fake_duck"
    };

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) { g_statusKey = "ready"; g_scanRunning = false; return; }

    PROCESSENTRY32W pe{ sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (g_cancelScan) break;
            std::string name = WStringToString(pe.szExeFile);
            std::string low = ToLower(name);
            if (low.find("cs2.exe") != std::string::npos ||
                low.find("csgo.exe") != std::string::npos) {
                ScanProcessStringsForProcess(pe.th32ProcessID, name, patterns, 4, "CS2 String");
                g_scannedCount++;
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ПОИСК ФАЙЛОВ ----
void PerformFileSearch(const std::string& folder, const std::string& query) {
    if (folder.empty() || query.empty()) return;
    try {
        std::string lowQuery = ToLower(query);
        int count = 0;
        int maxResults = 500;

        for (auto& e : fs::recursive_directory_iterator(folder,
            fs::directory_options::skip_permission_denied)) {
            if (g_cancelScan || count >= maxResults) break;
            if (!e.is_regular_file()) continue;

            std::string fname = WStringToString(e.path().filename().wstring());
            std::string lowName = ToLower(fname);

            if (lowName.find(lowQuery) != std::string::npos) {
                AddResult(fname, WStringToString(e.path().wstring()),
                    "FileSearch", "Match query: " + query, 2);
                count++;
                g_scannedCount++;
            }
        }
    }
    catch (...) {}
    g_statusKey = "ready"; g_scanRunning = false;
}

// ---- ВОССТАНОВЛЕНИЕ ----
void OpenRecoveryTool() {
    std::vector<std::string> candidates = {
        "Tools\\Hetman Partition Recovery\\Hetman Partition Recovery.exe",
        "Tools\\Hetman Partition Recovery\\PartitionRecovery.exe",
        "Tools\\Recovery\\Hetman Partition Recovery.exe"
    };
    for (auto& p : candidates) {
        if (fs::exists(p)) {
            ShellExecuteA(nullptr, "open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
    }
    ShellExecuteA(nullptr, "open", "Tools", nullptr, nullptr, SW_SHOWNORMAL);
}

// ---- SYSTEM FOLDERS ----
std::vector<SystemFolder> GetSystemFolders() {
    std::vector<SystemFolder> folders;
    auto add = [&](REFKNOWNFOLDERID id, const std::string& name) {
        std::string p = GetFolderPath(id);
        if (!p.empty()) folders.push_back({ name, p, "" });
        };

    add(FOLDERID_Desktop, "Desktop");
    add(FOLDERID_Downloads, "Downloads");
    add(FOLDERID_Documents, "Documents");
    add(FOLDERID_Pictures, "Pictures");
    add(FOLDERID_RoamingAppData, "AppData (Roaming)");
    add(FOLDERID_LocalAppData, "AppData (Local)");
    add(FOLDERID_ProgramData, "ProgramData");
    add(FOLDERID_Windows, "Windows");
    add(FOLDERID_ProgramFiles, "Program Files");
    add(FOLDERID_ProgramFilesX86, "Program Files (x86)");

    folders.push_back({ "Temp", "C:\\Windows\\Temp", "" });
    folders.push_back({ "Prefetch", "C:\\Windows\\Prefetch", "" });
    folders.push_back({ "Recent", GetFolderPath(FOLDERID_Recent), "" });
    folders.push_back({ "Startup", GetFolderPath(FOLDERID_Startup), "" });
    return folders;
}

// ---- ИСТОРИЯ ----
void SaveScanToHistory(const std::string& tabName, size_t filesScanned, size_t found, const std::vector<json>& results) {
    try {
        fs::create_directories("logs");
        time_t now = time(nullptr);
        struct tm t;
        localtime_s(&t, &now);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &t);
        std::string ts(buf);
        std::string filename = "logs/scan_" + ts + "_" + tabName + ".json";

        json report;
        report["timestamp"] = now;
        report["tab"] = tabName;
        report["files_scanned"] = filesScanned;
        report["found"] = found;
        report["results"] = results;

        std::ofstream ofs(filename);
        if (ofs) ofs << report.dump(2);

        ScanHistoryEntry entry;
        entry.timestamp = ts;
        entry.tabName = tabName;
        entry.files = filesScanned;
        entry.found = found;
        entry.file = filename;

        std::lock_guard<std::mutex> lk(g_historyMutex);
        g_scanHistory.insert(g_scanHistory.begin(), entry);
        if (g_scanHistory.size() > 50) g_scanHistory.pop_back();
    }
    catch (...) {}
}

void LoadHistoryList() {
    g_scanHistory.clear();
    try {
        if (!fs::exists("logs")) return;
        std::vector<fs::path> files;
        for (auto& e : fs::directory_iterator("logs")) {
            if (e.is_regular_file()) {
                std::string fname = e.path().filename().string();
                if (fname.rfind("scan_", 0) == 0 && e.path().extension() == ".json")
                    files.push_back(e.path());
            }
        }
        std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
            return fs::last_write_time(a) > fs::last_write_time(b);
            });

        std::lock_guard<std::mutex> lk(g_historyMutex);
        for (auto& p : files) {
            if (g_scanHistory.size() >= 50) break;
            try {
                std::ifstream ifs(p);
                json j; ifs >> j;
                ScanHistoryEntry e;
                e.file = p.string();
                e.tabName = j.value("tab", "?");
                e.files = j.value("files_scanned", (size_t)0);
                e.found = j.value("found", (size_t)0);
                e.timestamp = p.stem().string();
                g_scanHistory.push_back(e);
            }
            catch (...) {}
        }
    }
    catch (...) {}
}

// ===================== МЕНЕДЖЕР СКАНОВ =====================
void StartScan(int tab) {
    if (g_scanRunning) return;
    g_scanRunning = true;
    g_cancelScan = false;

    static const char* scanNamesRU[] = {
        "Файлы","Процессы","Сеть","Задачи","Скрытое",
        "Сигнатуры","Службы","Автозапуск","Память","Драйверы",
        "Активность","USB","Steam","Браузер","Строки CS2",
        "Реестр"
    };
    static const char* scanNamesEN[] = {
        "Files","Processes","Network","Tasks","Hidden",
        "Signatures","Services","Autorun","Memory","Drivers",
        "Activity","USB","Steam","Browser","CS2 Strings",
        "Registry"
    };
    std::string scanName;
    if (tab >= 0 && tab < 16)
        scanName = g_isRussian ? scanNamesRU[tab] : scanNamesEN[tab];
    else
        scanName = g_isRussian ? "Неизвестно" : "Unknown";

    RPC_OnScanStart(scanName.c_str());

    size_t startFound = g_matchesCount.load();
    size_t startScanned = g_scannedCount.load();

    std::thread([tab, scanName, startFound, startScanned]() {
        switch (tab) {
        case 0:  ScanFiles(); break;
        case 1:  ScanProcesses(); break;
        case 2:  ScanNet(); break;
        case 3:  ScanTaskScheduler(); break;
        case 4:  ScanHidden(); break;
        case 5:  ScanSignatures(); break;
        case 6:  ScanServices(); break;
        case 7:  ScanAutorun(); break;
        case 8:  ScanMemory(); break;
        case 9:  ScanDrivers(); break;
        case 10: ScanActivity(); break;
        case 11: ScanUSBDevices(); break;
        case 12: ScanSteamAccounts(); break;
        case 13: ScanBrowserHistory(); break;
        case 14: ScanCS2Strings(); break;
        case 15: ScanRegistryPaths(); break;
        default: g_scanRunning = false; break;
        }
        RPC_OnScanFinish();

        size_t newFound = g_matchesCount.load() - startFound;
        size_t newScanned = g_scannedCount.load() - startScanned;
        std::vector<json> copy;
        { std::lock_guard<std::mutex> lk(g_resultMutex); copy = g_results; }
        SaveScanToHistory(scanName, newScanned, newFound, copy);
        LoadHistoryList();
        }).detach();
}

void StopScan() {
    g_cancelScan = true;
    g_taskQueue.notify_all();
}

// ===================== BASE64 =====================
std::string FileToBase64(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return "";

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    if (data.empty()) return "";

    static const char* b64chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string out;
    out.reserve(((data.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 2 < data.size()) {
        unsigned int v = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
        out += b64chars[(v >> 18) & 0x3F];
        out += b64chars[(v >> 12) & 0x3F];
        out += b64chars[(v >> 6) & 0x3F];
        out += b64chars[v & 0x3F];
        i += 3;
    }
    if (i < data.size()) {
        unsigned int v = data[i] << 16;
        if (i + 1 < data.size()) v |= data[i + 1] << 8;
        out += b64chars[(v >> 18) & 0x3F];
        out += b64chars[(v >> 12) & 0x3F];
        out += (i + 1 < data.size()) ? b64chars[(v >> 6) & 0x3F] : '=';
        out += '=';
    }
    return out;
}

// ===================== ЭКСПОРТ =====================
void ExportReport() {
    std::lock_guard<std::mutex> lk(g_resultMutex);
    if (g_results.empty()) {
        MessageBoxA(nullptr, L ? L->NoResults : "No results", "Info", MB_ICONINFORMATION);
        return;
    }
    fs::create_directories("logs");
    std::string filename = "logs/Chmonya_CC_Report_" + std::to_string(time(nullptr)) + ".json";
    std::ofstream ofs(filename);
    if (ofs) {
        json report;
        report["timestamp"] = std::time(nullptr);
        report["version"] = "3.3";
        report["total_results"] = g_results.size();
        report["results"] = g_results;
        ofs << report.dump(4);
        std::string msgText = std::string(L ? L->ReportSaved : "Report saved") + ": " + filename +
            " (" + std::to_string(g_results.size()) + ")";
        MessageBoxA(nullptr, msgText.c_str(), "Success", MB_ICONINFORMATION);
        std::thread([msgText]() { SendToDiscord(msgText); }).detach();
    }
    else {
        MessageBoxA(nullptr, L ? L->ReportError : "Error", "Error", MB_ICONERROR);
    }
}

void ExportHTMLReport() {
    std::vector<json> copy;
    { std::lock_guard<std::mutex> lk(g_resultMutex); copy = g_results; }
    if (copy.empty()) {
        MessageBoxA(nullptr, L ? L->NoResults : "No results", "Info", MB_ICONINFORMATION);
        return;
    }

    std::string logoB64 = FileToBase64("assets/logo.png");
    std::string bgB64 = FileToBase64("assets/background.png");

    fs::create_directories("logs");
    std::string filename = "logs/Chmonya_CC_Report_" + std::to_string(time(nullptr)) + ".html";
    std::ofstream ofs(filename);

    int countHigh = 0, countMed = 0, countLow = 0;
    for (auto& r : copy) {
        int sev = r.value("severity", 1);
        if (sev >= 3) countHigh++;
        else if (sev == 2) countMed++;
        else countLow++;
    }

    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    char dateBuf[64];
    strftime(dateBuf, sizeof(dateBuf), "%d.%m.%Y %H:%M", &t);

    ofs << "<!DOCTYPE html><html lang='ru'><head>"
        << "<meta charset='utf-8'>"
        << "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        << "<title>CS2 Cheat Checker by Chmonya — Report</title>"
        << "<style>"
        << ":root{"
        << "--span:#e39a51;--span-low:rgba(227,154,81,.05);--span-10:rgba(227,154,81,.1);"
        << "--span-middle:rgba(227,154,81,.25);--span-half:rgba(227,154,81,.5);"
        << "--text-custom:#ffc48a;--text-secondary:#b9a088;--text-default:#ffe3c7;"
        << "--bg:#1b0e01;--green:#64ce82;--orange:#f6b949;--red:#ff4940;--red-10:rgba(255,73,64,.1);"
        << "--transparent-5-w:rgba(255,255,255,.05);"
        << "}"
        << "*{margin:0;padding:0;box-sizing:border-box}"
        << "html,body{min-height:100%}"
        << "body{font-family:'Segoe UI',system-ui,-apple-system,sans-serif;"
        << "background-color:var(--bg);color:var(--text-default);background-size:cover;"
        << "background-attachment:fixed;background-position:center top;background-repeat:no-repeat;";

    if (!bgB64.empty()) {
        ofs << "background-image:url('data:image/png;base64," << bgB64 << "'),"
            << "radial-gradient(circle at 50% 0%,rgba(227,154,81,.08) 0%,transparent 60%);";
    }
    else {
        ofs << "background-image:radial-gradient(circle at 50% 0%,rgba(227,154,81,.08) 0%,transparent 60%);";
    }

    ofs << "background-blend-mode:overlay,normal;}"
        << ".navbar{position:sticky;top:0;z-index:100;background:rgba(17,17,17,.88);backdrop-filter:blur(12px);border-bottom:1px solid var(--transparent-5-w);padding:14px 32px;display:flex;align-items:center;justify-content:space-between;gap:24px;}"
        << ".nav-logo{font-size:22px;font-weight:800;letter-spacing:1px;color:var(--text-custom);text-transform:uppercase;display:flex;align-items:center;gap:12px;}"
        << ".nav-logo-img{height:40px;width:40px;object-fit:contain;}"
        << ".nav-online{display:flex;align-items:center;gap:8px;font-size:13px;color:var(--text-secondary);}"
        << ".nav-dot{width:8px;height:8px;border-radius:50%;background:var(--green);animation:pulse 1.6s ease-in-out infinite;}"
        << "@keyframes pulse{0%,100%{opacity:1}50%{opacity:.5}}"
        << ".container{max-width:1400px;margin:0 auto;padding:32px 24px;}"
        << ".page-title{font-size:32px;font-weight:800;color:var(--text-custom);margin-bottom:8px;}"
        << ".page-subtitle{color:var(--text-secondary);font-size:14px;margin-bottom:24px;}"
        << ".stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:16px;margin-bottom:32px;}"
        << ".stat{background:rgba(38,25,12,.85);border:1px solid var(--transparent-5-w);border-radius:12px;padding:20px;position:relative;}"
        << ".stat::before{content:'';position:absolute;left:0;top:0;bottom:0;width:3px;background:var(--span);}"
        << ".stat-label{font-size:12px;color:var(--text-secondary);text-transform:uppercase;letter-spacing:1px;margin-bottom:8px;}"
        << ".stat-value{font-size:28px;font-weight:800;color:var(--text-custom);}"
        << ".stat--high::before{background:var(--red);} .stat--high .stat-value{color:var(--red);}"
        << ".stat--med::before{background:var(--orange);} .stat--med .stat-value{color:var(--orange);}"
        << ".stat--low::before{background:var(--green);} .stat--low .stat-value{color:var(--green);}"
        << ".card{background:rgba(38,25,12,.88);border:1px solid var(--transparent-5-w);border-radius:12px;overflow:hidden;}"
        << ".card-header{padding:20px 24px;border-bottom:1px solid var(--transparent-5-w);}"
        << ".card-title{font-size:18px;font-weight:700;color:var(--text-custom);}"
        << "table{width:100%;border-collapse:collapse;}"
        << "thead th{background:rgba(0,0,0,.3);padding:14px 16px;text-align:left;font-size:12px;font-weight:700;color:var(--text-secondary);text-transform:uppercase;letter-spacing:.8px;border-bottom:1px solid var(--transparent-5-w);}"
        << "tbody td{padding:14px 16px;font-size:13px;border-bottom:1px solid rgba(255,255,255,.03);vertical-align:top;}"
        << "tbody tr:hover{background:var(--span-low);}"
        << ".type-badge{display:inline-block;padding:3px 9px;border-radius:6px;font-size:11px;font-weight:700;text-transform:uppercase;}"
        << ".type-file{background:rgba(102,170,204,.15);color:#66AACC;}"
        << ".type-process{background:var(--red-10);color:var(--red);}"
        << ".type-signature{background:rgba(246,185,73,.15);color:var(--orange);}"
        << ".type-driver{background:var(--red-10);color:var(--red);}"
        << ".type-registry{background:rgba(246,185,73,.15);color:var(--orange);}"
        << ".type-activity{background:rgba(246,185,73,.15);color:var(--orange);}"
        << ".type-browser,.type-browser-string,.type-cs2-string{background:var(--red-10);color:var(--red);}"
        << ".type-steam,.type-usb{background:rgba(102,170,204,.15);color:#66AACC;}"
        << ".type-default{background:var(--span-10);color:var(--span);}"
        << ".col-name{font-weight:600;}"
        << ".col-reason,.col-path{color:var(--text-secondary);}"
        << ".col-path{font-family:'Consolas','Monaco',monospace;font-size:12px;word-break:break-all;}"
        << ".vt-link{display:inline-block;padding:3px 8px;border-radius:6px;background:rgba(62,200,250,.15);color:#3ec8fa;text-decoration:none;font-size:11px;font-weight:700;}"
        << ".footer{margin-top:48px;padding:32px 24px;border-top:1px solid var(--transparent-5-w);text-align:center;color:var(--text-secondary);font-size:13px;}"
        << ".footer-logo{font-size:20px;font-weight:800;color:var(--text-custom);letter-spacing:1px;margin-bottom:12px;}"
        << ".footer-links{display:flex;justify-content:center;gap:20px;flex-wrap:wrap;margin-top:16px;}"
        << ".footer-links a{color:var(--span);text-decoration:none;}"
        << ".empty{padding:60px 24px;text-align:center;color:var(--text-secondary);}"
        << "</style></head><body>";

    ofs << "<div class='navbar'><div class='nav-logo'>";
    if (!logoB64.empty()) {
        ofs << "<img src='data:image/png;base64," << logoB64 << "' class='nav-logo-img' alt='Chmonya'>";
    }
    ofs << "CHMONYA</div>"
        << "<div class='nav-online'><span class='nav-dot'></span>Cheat Checker Report</div></div>";

    ofs << "<div class='container'>";
    ofs << "<h1 class='page-title'>Отчёт проверки</h1>";
    ofs << "<div class='page-subtitle'>Сгенерировано: " << dateBuf
        << " | Всего найдено: " << copy.size() << "</div>";

    ofs << "<div class='stats'>";
    ofs << "<div class='stat'><div class='stat-label'>Всего находок</div><div class='stat-value'>"
        << copy.size() << "</div></div>";
    ofs << "<div class='stat stat--high'><div class='stat-label'>Высокий риск</div><div class='stat-value'>"
        << countHigh << "</div></div>";
    ofs << "<div class='stat stat--med'><div class='stat-label'>Средний риск</div><div class='stat-value'>"
        << countMed << "</div></div>";
    ofs << "<div class='stat stat--low'><div class='stat-label'>Низкий риск</div><div class='stat-value'>"
        << countLow << "</div></div>";
    ofs << "</div>";

    ofs << "<div class='card'><div class='card-header'><div class='card-title'>Результаты сканирования</div></div>";

    if (copy.empty()) {
        ofs << "<div class='empty'>Ничего не найдено</div>";
    }
    else {
        ofs << "<table><thead><tr>"
            << "<th style='width:110px'>Тип</th>"
            << "<th style='width:220px'>Имя</th>"
            << "<th style='width:200px'>Причина</th>"
            << "<th>Путь</th>"
            << "<th style='width:80px'>Хеш</th>"
            << "</tr></thead><tbody>";

        for (auto& r : copy) {
            std::string type = r.value("type", "");
            std::string name = r.value("name", "");
            std::string reason = r.value("reason", "");
            std::string path = r.value("path", "");
            std::string hash = r.value("sha256", "");

            std::string typeClass = "type-default";
            if (type == "File")            typeClass = "type-file";
            else if (type == "Process")         typeClass = "type-process";
            else if (type == "Signature")       typeClass = "type-signature";
            else if (type == "Driver")          typeClass = "type-driver";
            else if (type == "Registry" || type == "Autorun") typeClass = "type-registry";
            else if (type == "Activity")        typeClass = "type-activity";
            else if (type == "Browser")         typeClass = "type-browser";
            else if (type == "Browser String")  typeClass = "type-browser-string";
            else if (type == "CS2 String")      typeClass = "type-cs2-string";
            else if (type == "Steam")           typeClass = "type-steam";
            else if (type == "USB")             typeClass = "type-usb";

            ofs << "<tr>";
            ofs << "<td><span class='type-badge " << typeClass << "'>" << type << "</span></td>";
            ofs << "<td class='col-name'>" << name << "</td>";
            ofs << "<td class='col-reason'>" << reason << "</td>";
            ofs << "<td class='col-path'>" << path << "</td>";
            ofs << "<td>";
            if (!hash.empty()) {
                ofs << "<a class='vt-link' href='https://www.virustotal.com/gui/file/"
                    << hash << "' target='_blank' title='" << hash << "'>VT</a>";
            }
            ofs << "</td></tr>";
        }

        ofs << "</tbody></table>";
    }

    ofs << "</div></div>";

    // v3.3: footer обновлён — убраны CS-Arena ссылки, добавлен сайт Chmonya
    ofs << "<div class='footer'>"
        << "<div class='footer-logo'>CHMONYA</div>"
        << "<div>CS2 Cheat Checker by Chmonya</div>"
        << "<div class='footer-links'>"
        << "<a href='https://chmonya.ct.ws' target='_blank'>Website</a>"
        << "<a href='https://github.com/ChmonyaStudio' target='_blank'>GitHub</a>"
        << "<a href='https://t.me/p1zdabol4ik' target='_blank'>Telegram</a>"
        << "</div></div>";

    ofs << "</body></html>";
    ofs.close();

    std::string msg = "HTML report saved: " + filename;
    MessageBoxA(nullptr, msg.c_str(), "Success", MB_ICONINFORMATION);
    ShellExecuteA(nullptr, "open", filename.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void ExportCSVReport() {
    std::vector<json> copy;
    { std::lock_guard<std::mutex> lk(g_resultMutex); copy = g_results; }
    if (copy.empty()) {
        MessageBoxA(nullptr, L ? L->NoResults : "No results", "Info", MB_ICONINFORMATION);
        return;
    }
    fs::create_directories("logs");
    std::string filename = "logs/Chmonya_CC_Report_" + std::to_string(std::time(nullptr)) + ".csv";
    std::ofstream ofs(filename);
    ofs << "\xEF\xBB\xBF";
    ofs << "Type,Name,Reason,Path,SHA256,Severity,Time\n";
    auto escape = [](const std::string& s) {
        std::string out = "\"";
        for (char c : s) { if (c == '"') out += "\"\""; else out += c; }
        out += "\"";
        return out;
        };
    for (auto& r : copy) {
        ofs << escape(r.value("type", "")) << ","
            << escape(r.value("name", "")) << ","
            << escape(r.value("reason", "")) << ","
            << escape(r.value("path", "")) << ","
            << escape(r.value("sha256", "")) << ","
            << r.value("severity", 1) << ","
            << r.value("time", (long long)0) << "\n";
    }
    ofs.close();
    std::string msg = "CSV report saved: " + filename;
    MessageBoxA(nullptr, msg.c_str(), "Success", MB_ICONINFORMATION);
    std::string arg = "/select,\"" + fs::path(filename).string() + "\"";
    ShellExecuteA(nullptr, "open", "explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
}

// ===================== СТИЛЬ IMGUI =====================
void ApplyCSArenaStyle() {
    ImGuiStyle& s = ImGui::GetStyle();
    ImVec4* c = s.Colors;

    s.WindowRounding = 8.0f;
    s.ChildRounding = 8.0f;
    s.FrameRounding = 6.0f;
    s.PopupRounding = 6.0f;
    s.ScrollbarRounding = 6.0f;
    s.GrabRounding = 4.0f;
    s.TabRounding = 6.0f;
    s.WindowPadding = ImVec2(0, 0);
    s.FramePadding = ImVec2(12, 8);
    s.ItemSpacing = ImVec2(10, 8);
    s.ItemInnerSpacing = ImVec2(6, 6);
    s.ScrollbarSize = 8.0f;
    s.GrabMinSize = 10.0f;
    s.WindowBorderSize = 0.0f;
    s.ChildBorderSize = 1.0f;
    s.FrameBorderSize = 1.0f;
    s.AntiAliasedLines = true;
    s.AntiAliasedFill = true;

    c[ImGuiCol_Text] = CSColors::Text;
    c[ImGuiCol_TextDisabled] = CSColors::TextMuted;
    c[ImGuiCol_TextSelectedBg] = CSColors::PrimarySoft;
    c[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.98f);
    c[ImGuiCol_Border] = CSColors::BorderLight;
    c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg] = CSColors::FrameIdle;
    c[ImGuiCol_FrameBgHovered] = CSColors::FrameHover;
    c[ImGuiCol_FrameBgActive] = ImVec4(1, 1, 1, 0.12f);
    c[ImGuiCol_TitleBg] = CSColors::Bg;
    c[ImGuiCol_TitleBgActive] = CSColors::Bg;
    c[ImGuiCol_MenuBarBg] = ImVec4(0.06f, 0.06f, 0.08f, 1.0f);
    c[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ScrollbarGrab] = ImVec4(1, 1, 1, 0.15f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1, 1, 1, 0.25f);
    c[ImGuiCol_ScrollbarGrabActive] = CSColors::Primary;
    c[ImGuiCol_CheckMark] = CSColors::Primary;
    c[ImGuiCol_SliderGrab] = CSColors::Primary;
    c[ImGuiCol_SliderGrabActive] = CSColors::PrimaryDark;
    c[ImGuiCol_Button] = CSColors::BtnIdle;
    c[ImGuiCol_ButtonHovered] = CSColors::BtnHover;
    c[ImGuiCol_ButtonActive] = CSColors::BtnActive;
    c[ImGuiCol_Header] = ImVec4(1, 1, 1, 0.04f);
    c[ImGuiCol_HeaderHovered] = ImVec4(1, 1, 1, 0.08f);
    c[ImGuiCol_HeaderActive] = CSColors::PrimarySoft;
    c[ImGuiCol_Separator] = CSColors::BorderSoft;
    c[ImGuiCol_SeparatorHovered] = CSColors::Primary;
    c[ImGuiCol_SeparatorActive] = CSColors::PrimaryDark;
    c[ImGuiCol_Tab] = ImVec4(1, 1, 1, 0.03f);
    c[ImGuiCol_TabHovered] = ImVec4(1, 1, 1, 0.08f);
    c[ImGuiCol_TabActive] = CSColors::PrimarySoft;
    c[ImGuiCol_PlotLines] = CSColors::Primary;
    c[ImGuiCol_PlotHistogram] = CSColors::Primary;
    c[ImGuiCol_PlotHistogramHovered] = CSColors::PrimaryDark;
    c[ImGuiCol_DragDropTarget] = CSColors::Primary;
    c[ImGuiCol_NavHighlight] = CSColors::Primary;
}

// ===================== UI ХЕЛПЕРЫ =====================
void DrawPanelBg(ImDrawList* dl, const ImVec2& pMin, const ImVec2& pMax, float rounding, ImU32 bg) {
    if (!dl) return;
    dl->AddRectFilled(pMin, pMax, bg, rounding);
    dl->AddRect(pMin, pMax, ImGui::GetColorU32(CSColors::BorderLight), rounding, 0, 1.0f);
}

bool BigButton(const char* label, const ImVec2& size = ImVec2(0, 0), bool accent = false) {
    if (accent) {
        ImGui::PushStyleColor(ImGuiCol_Button, CSColors::Primary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, CSColors::PrimaryDark);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, CSColors::PrimaryDark);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.05f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.10f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.15f));
        ImGui::PushStyleColor(ImGuiCol_Text, CSColors::Text);
    }
    bool r = ImGui::Button(label, size);
    ImGui::PopStyleColor(4);
    return r;
}

void SectionTitle(const char* icon, const char* text, const char* desc = nullptr) {
    ImGui::PushStyleColor(ImGuiCol_Text, CSColors::Primary);
    ImGui::SetWindowFontScale(1.15f);
    ImGui::Text("%s  %s", icon, text);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    if (desc && *desc) {
        ImGui::PushStyleColor(ImGuiCol_Text, CSColors::TextMuted);
        ImGui::TextUnformatted(desc);
        ImGui::PopStyleColor();
    }
    ImGui::Spacing();
}

void RenderProgressBar(float progress, const char* overlay) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float height = 10.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(255, 255, 255, 15), 5.0f);
    float fill = progress < 0.0f ? 0.0f : (progress > 1.0f ? 1.0f : progress);
    ImU32 acc = ImGui::GetColorU32(CSColors::Primary);
    if (progress < 0.0f && g_scanRunning) {
        float t = (float)fmod(ImGui::GetTime() * 0.8, 1.0);
        float w = width * 0.25f;
        float x0 = pos.x + t * (width + w) - w;
        float x1 = x0 + w;
        if (x0 < pos.x) x0 = pos.x;
        if (x1 > pos.x + width) x1 = pos.x + width;
        if (x1 > x0) dl->AddRectFilled(ImVec2(x0, pos.y + 1), ImVec2(x1, pos.y + height - 1), acc, 4.0f);
    }
    else if (fill > 0.0f) {
        dl->AddRectFilled(ImVec2(pos.x + 1, pos.y + 1),
            ImVec2(pos.x + (width - 2) * fill + 1, pos.y + height - 1), acc, 4.0f);
    }
    if (overlay && *overlay) {
        ImVec2 ts = ImGui::CalcTextSize(overlay);
        dl->AddText(ImVec2(pos.x + (width - ts.x) * 0.5f, pos.y + (height - ts.y) * 0.5f),
            IM_COL32(255, 255, 255, 240), overlay);
    }
    ImGui::Dummy(ImVec2(width, height + 4));
}

void RenderScanControls(int tabId) {
    if (!g_scanRunning) {
        if (BigButton((std::string(ICON_FA_PLAY) + "  " + L->StartScan).c_str(), ImVec2(200, 42), true)) {
            StartScan(tabId);
        }
    }
    else {
        if (BigButton((std::string(ICON_FA_STOP) + "  " + L->StopScan).c_str(), ImVec2(200, 42), true)) {
            StopScan();
        }
    }
    ImGui::SameLine();
    ImGui::TextColored(CSColors::TextMuted, "%s %zu  |  %s %zu",
        L->Scanned, g_scannedCount.load(), L->Hits, g_matchesCount.load());
    if (g_scanRunning) {
        ImGui::SameLine();
        ImGui::TextColored(CSColors::Info, ICON_FA_SPINNER "  %s", GetStatusText().c_str());
    }
    ImGui::Spacing();
    RenderProgressBar(-1.0f, g_scanRunning ? L->Scanning : nullptr);
    ImGui::Spacing();
}

// ===================== SPLASH =====================
void RenderSplash(float dt) {
    if (!g_showSplash.load()) return;
    g_splashTime += dt;
    if (g_splashTime < 0.6f)      g_splashAlpha = g_splashTime / 0.6f;
    else if (g_splashTime < 1.8f) g_splashAlpha = 1.0f;
    else {
        g_splashAlpha = 1.0f - ((g_splashTime - 1.8f) / 0.4f);
        if (g_splashAlpha <= 0.0f) { g_showSplash = false; return; }
    }
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 disp = ImGui::GetIO().DisplaySize;
    ImVec2 center(disp.x * 0.5f, disp.y * 0.5f);
    int a = (int)(255 * g_splashAlpha);

    dl->AddRectFilledMultiColor(ImVec2(0, 0), disp,
        IM_COL32(4, 4, 6, a), IM_COL32(8, 8, 12, a),
        IM_COL32(8, 8, 12, a), IM_COL32(4, 4, 6, a));

    float pulse = 1.0f + sinf(g_splashTime * 2.5f) * 0.08f;
    float radius = 70.0f * pulse;
    dl->AddCircleFilled(center, radius + 26, IM_COL32(255, 158, 13, (int)(40 * g_splashAlpha)));
    dl->AddCircleFilled(center, radius, IM_COL32(14, 14, 20, a));
    dl->AddCircle(center, radius, IM_COL32(255, 158, 13, a), 0, 3.0f);

    if (g_hasLogo && g_logoTexture) {
        float ls = radius * 1.2f;
        dl->AddImage((ImTextureID)(intptr_t)g_logoTexture,
            ImVec2(center.x - ls * 0.5f, center.y - ls * 0.5f),
            ImVec2(center.x + ls * 0.5f, center.y + ls * 0.5f),
            ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, a));
    }

    const char* title = "CHMONYA";
    ImVec2 ts = ImGui::CalcTextSize(title);
    dl->AddText(ImVec2(center.x - ts.x * 0.5f, center.y + radius + 20),
        IM_COL32(255, 158, 13, a), title);
    const char* sub = "CHEAT CHECKER v3.3";
    ImVec2 ss2 = ImGui::CalcTextSize(sub);
    dl->AddText(ImVec2(center.x - ss2.x * 0.5f, center.y + radius + 45),
        IM_COL32(160, 160, 170, (int)(220 * g_splashAlpha)), sub);
    const char* meme = g_isRussian ? "погнали читеров искать..." : "let's hunt some cheaters...";
    ImVec2 ms = ImGui::CalcTextSize(meme);
    dl->AddText(ImVec2(center.x - ms.x * 0.5f, center.y + radius + 70),
        IM_COL32(255, 158, 13, (int)(180 * g_splashAlpha)), meme);

    float bw = 260.0f, bh = 3.0f;
    ImVec2 bStart(center.x - bw * 0.5f, center.y + radius + 100);
    ImVec2 bEnd(center.x + bw * 0.5f, center.y + radius + 100 + bh);
    dl->AddRectFilled(bStart, bEnd, IM_COL32(30, 30, 38, (int)(200 * g_splashAlpha)), 2.0f);
    float progress = (g_splashTime < 1.8f) ? (g_splashTime / 1.8f) : 1.0f;
    dl->AddRectFilled(bStart, ImVec2(bStart.x + bw * progress, bEnd.y), IM_COL32(255, 158, 13, a), 2.0f);
}

// ===================== ТАБЛИЦА РЕЗУЛЬТАТОВ =====================
void RenderResultsTable(const char* tableId, std::vector<json>& copySource) {
    if (!ImGui::BeginTable(tableId, 5,
        ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Sortable))
        return;

    ImGui::TableSetupColumn(L->Type, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 110);
    ImGui::TableSetupColumn(L->Name, ImGuiTableColumnFlags_WidthFixed, 220);
    ImGui::TableSetupColumn(L->Reason, ImGuiTableColumnFlags_WidthFixed, 200);
    ImGui::TableSetupColumn(L->Path, ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn(L->OpenFolder, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort, 130);
    ImGui::TableHeadersRow();

    static std::vector<json> filtered;
    static std::string lastSearch;
    static uint64_t lastVersion = (uint64_t)-1;
    std::string sl = ToLower(std::string(g_searchQuery));
    uint64_t curVersion = g_resultsVersion.load(std::memory_order_relaxed);
    ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs();
    bool specsDirty = specs && specs->SpecsDirty;
    bool needRebuild = (curVersion != lastVersion) || (sl != lastSearch) || specsDirty;

    if (needRebuild) {
        filtered.clear();
        for (const auto& r : copySource) {
            if (sl.empty()) filtered.push_back(r);
            else {
                std::string nL = ToLower(r.value("name", ""));
                std::string pL = ToLower(r.value("path", ""));
                if (nL.find(sl) != std::string::npos || pL.find(sl) != std::string::npos)
                    filtered.push_back(r);
            }
        }
        if (specs && specs->SpecsCount > 0) {
            const auto& spec = specs->Specs[0];
            const char* keys[] = { "type", "name", "reason", "path" };
            const char* key = (spec.ColumnIndex >= 0 && spec.ColumnIndex < 4) ? keys[spec.ColumnIndex] : "type";
            bool asc = (spec.SortDirection == ImGuiSortDirection_Ascending);
            std::sort(filtered.begin(), filtered.end(), [&](const json& a, const json& b) {
                std::string va = a.value(key, ""), vb = b.value(key, "");
                return asc ? (va < vb) : (va > vb);
                });
        }
        lastVersion = curVersion;
        lastSearch = sl;
        if (specs) specs->SpecsDirty = false;
    }

    ImGuiListClipper clipper;
    clipper.Begin((int)filtered.size());
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const auto& res = filtered[i];
            ImGui::TableNextRow();
            std::string type = res.value("type", "");
            ImVec4 col = CSColors::Text;
            if (type == "Registry" || type == "Autorun") col = CSColors::Warning;
            else if (type == "Process" || type == "Network" || type == "Memory") col = CSColors::Danger;
            else if (type == "Signature") col = CSColors::Warning;
            else if (type == "Hidden") col = ImVec4(0.85f, 0.45f, 1.f, 1);
            else if (type == "Service" || type == "Task") col = CSColors::Secondary;
            else if (type == "Driver" || type == "Module") col = CSColors::Danger;
            else if (type == "Activity") col = CSColors::Warning;
            else if (type == "Browser" || type == "Browser String" || type == "CS2 String") col = CSColors::Danger;
            else if (type == "Steam") col = CSColors::Secondary;
            else if (type == "USB") col = CSColors::Info;
            else if (type == "FileSearch") col = CSColors::Info;
            else if (type == "File") col = CSColors::Info;

            ImGui::TableNextColumn();
            ImGui::TextColored(col, "%s", type.c_str());

            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", res.value("name", "").c_str());
            if (res.contains("known")) {
                ImGui::SameLine();
                ImGui::TextColored(CSColors::Danger, " [%s]", res.value("known", "").c_str());
            }

            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", res.value("reason", "").c_str());

            ImGui::TableNextColumn();
            std::string path = res.value("path", "");
            ImGui::PushID(i);
            ImGui::Selectable(path.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                OpenInExplorer(path);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", path.c_str());

            ImGui::TableNextColumn();
            if (ImGui::SmallButton(ICON_FA_FOLDER_OPEN)) OpenInExplorer(path);
            std::string hash = res.value("sha256", "");
            if (!hash.empty()) {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.78f, 0.98f, 0.20f));
                if (ImGui::SmallButton("VT")) OpenURL("https://www.virustotal.com/gui/file/" + hash);
                ImGui::PopStyleColor();
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("VirusTotal: %s", hash.c_str());
            }
            ImGui::PopID();
        }
    }
    ImGui::EndTable();
}

void RenderResultsByTypes(const char* tableId, std::initializer_list<const char*> types) {
    std::vector<json> copy;
    {
        std::lock_guard<std::mutex> lock(g_resultMutex);
        for (auto& r : g_results) {
            std::string t = r.value("type", "");
            for (auto& filter : types) {
                if (t == filter) { copy.push_back(r); break; }
            }
        }
    }
    if (copy.empty()) {
        ImGui::TextColored(CSColors::TextMuted, "%s", L->NoData);
        return;
    }
    RenderResultsTable(tableId, copy);
}

// ===================== ВКЛАДКИ =====================

// ---- HOME ----
void RenderHomeTab() {
    SectionTitle(ICON_FA_HOUSE, L->HomeWelcome, L->HomeSubtitle);
    ImGui::Spacing();

    float w = (ImGui::GetContentRegionAvail().x - 36) / 4.0f;
    ImGui::BeginGroup();
    {
        ImGui::BeginChild("##s1", ImVec2(w, 90), true);
        ImGui::SetCursorPos(ImVec2(16, 16));
        ImGui::TextColored(CSColors::Primary, ICON_FA_SKULL);
        ImGui::SetCursorPosX(16);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::Text("%zu", g_matchesCount.load());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", L->StatTotal);
        ImGui::EndChild();
    }
    ImGui::SameLine(0, 12);
    {
        ImGui::BeginChild("##s2", ImVec2(w, 90), true);
        ImGui::SetCursorPos(ImVec2(16, 16));
        ImGui::TextColored(CSColors::Success, ICON_FA_CHECK);
        ImGui::SetCursorPosX(16);
        ImGui::SetWindowFontScale(1.4f);
        std::lock_guard<std::mutex> lk(g_historyMutex);
        ImGui::Text("%zu", g_scanHistory.size());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", L->StatScans);
        ImGui::EndChild();
    }
    ImGui::SameLine(0, 12);
    {
        ImGui::BeginChild("##s3", ImVec2(w, 90), true);
        ImGui::SetCursorPos(ImVec2(16, 16));
        ImGui::TextColored(CSColors::Info, ICON_FA_MICROCHIP);
        ImGui::SetCursorPosX(16);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::Text("v3.3");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", L->StatVersion);
        ImGui::EndChild();
    }
    ImGui::SameLine(0, 12);
    {
        ImGui::BeginChild("##s4", ImVec2(w, 90), true);
        ImGui::SetCursorPos(ImVec2(16, 16));
        if (g_scanRunning) ImGui::TextColored(CSColors::Info, ICON_FA_SPINNER);
        else ImGui::TextColored(CSColors::Success, ICON_FA_CHECK);
        ImGui::SetCursorPosX(16);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::Text("%s", g_scanRunning ? L->Scanning : L->Ready);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", L->StatStatus);
        ImGui::EndChild();
    }
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Spacing();

    SectionTitle(ICON_FA_PLAY, "Быстрые действия");
    if (BigButton((std::string(ICON_FA_MICROCHIP) + "  Скан процессов").c_str(), ImVec2(280, 44), true))
        StartScan(1);
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_FILE) + "  Скан файлов").c_str(), ImVec2(280, 44)))
        StartScan(0);
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_HISTORY) + "  Скан активности").c_str(), ImVec2(280, 44)))
        StartScan(10);

    ImGui::Spacing();
    ImGui::Spacing();

    SectionTitle(ICON_FA_HISTORY, "Последние сканы");
    std::lock_guard<std::mutex> lk(g_historyMutex);
    if (g_scanHistory.empty()) {
        ImGui::TextColored(CSColors::TextMuted, "%s", L->HistoryEmpty);
    }
    else {
        for (int i = 0; i < (int)g_scanHistory.size() && i < 8; ++i) {
            auto& h = g_scanHistory[i];
            ImGui::PushID(i);
            ImGui::BeginChild("##h", ImVec2(0, 44), true);
            ImGui::SetCursorPos(ImVec2(16, 12));
            ImGui::TextColored(CSColors::Primary, "%s", h.timestamp.c_str());
            ImGui::SameLine(0, 20);
            ImGui::TextColored(CSColors::TextMuted, "%s", h.tabName.c_str());
            ImGui::SameLine(0, 20);
            ImGui::Text("%zu files", h.files);
            ImGui::SameLine(0, 20);
            if (h.found > 0) ImGui::TextColored(CSColors::Danger, "%zu findings", h.found);
            else ImGui::TextColored(CSColors::Success, "0");
            ImGui::EndChild();
            ImGui::PopID();
        }
    }
}

// ---- SITES ----
void RenderSitesTab() {
    SectionTitle(ICON_FA_GLOBE, L->CheckSites, L->SitesDesc);

    if (BigButton((std::string(ICON_FA_PLAY) + "  " + L->OpenAll).c_str(), ImVec2(240, 42), true)) {
        std::thread([]() {
            for (auto& s : g_checkSites) {
                OpenURL(s.url);
                Sleep(400);
            }
            }).detach();
    }
    ImGui::SameLine();
    ImGui::TextColored(CSColors::TextMuted, "Откроется %zu сайтов в браузере", g_checkSites.size());
    ImGui::Spacing();

    float w = (ImGui::GetContentRegionAvail().x - 12) / 2.0f;
    int col = 0;
    for (auto& s : g_checkSites) {
        ImGui::PushID(s.name.c_str());
        if (col == 0) ImGui::BeginGroup();
        ImGui::BeginChild("##site", ImVec2(w, 84), true);
        ImGui::SetCursorPos(ImVec2(14, 12));
        ImGui::TextColored(CSColors::Primary, ICON_FA_GLOBE "  %s", s.name.c_str());
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(CSColors::TextMuted, "%s", s.url.c_str());
        ImGui::SetCursorPosX(14);
        ImGui::TextColored(CSColors::Warning, "%s", s.note.c_str());
        ImGui::SetCursorPos(ImVec2(w - 90, 44));
        if (BigButton("Открыть", ImVec2(80, 30))) OpenURL(s.url);
        ImGui::EndChild();
        if (col == 0) { ImGui::SameLine(0, 12); col = 1; }
        else { ImGui::EndGroup(); col = 0; }
        ImGui::PopID();
    }
    if (col == 1) ImGui::EndGroup();
}

// ---- ACTIVITY ----
void RenderActivityTab() {
    SectionTitle(ICON_FA_HISTORY, L->Activity, L->ActivityDesc);
    RenderScanControls(10);
    RenderResultsByTypes("ActTable", { "Activity" });
}

// ---- USB ----
void RenderUSBTab() {
    SectionTitle(ICON_FA_USB, L->USB, L->USBDesc);
    RenderScanControls(11);
    RenderResultsByTypes("UsbTable", { "USB" });
}

// ---- REGISTRY (v3.3: показаны новые пути) ----
void RenderRegistryTab() {
    SectionTitle(ICON_FA_DATABASE, L->Registry, L->RegistryDesc);
    RenderScanControls(15);

    if (BigButton((std::string(ICON_FA_FOLDER_OPEN) + "  Открыть Regedit").c_str(), ImVec2(220, 36))) {
        ShellExecuteA(nullptr, "open", "regedit.exe", nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();

    ImGui::TextColored(CSColors::TextMuted, "Отслеживаемые пути (%zu):", g_regPaths.size());
    ImGui::Spacing();

    for (auto& rp : g_regPaths) {
        ImGui::PushID(rp.name.c_str());
        ImGui::BeginChild("##rp", ImVec2(0, 68), true);
        ImGui::SetCursorPos(ImVec2(16, 10));
        ImGui::TextColored(CSColors::Primary, "%s", rp.name.c_str());
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", rp.subkey.c_str());
        ImGui::SetCursorPosX(16);
        ImGui::TextColored(CSColors::TextMuted, "%s", rp.description.c_str());
        ImGui::EndChild();
        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::TextColored(CSColors::TextMuted, "Найдено записей: %zu", g_registryFindings.size());

    std::lock_guard<std::mutex> lk(g_registryMutex);
    if (!g_registryFindings.empty()) {
        ImGui::BeginChild("##regv", ImVec2(0, 300), true);
        if (ImGui::BeginTable("regtab", 3,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Путь", ImGuiTableColumnFlags_WidthFixed, 200);
            ImGui::TableSetupColumn("Значение", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Найдено", ImGuiTableColumnFlags_WidthFixed, 300);
            ImGui::TableHeadersRow();
            int shown = 0;
            for (auto& f : g_registryFindings) {
                if (shown++ > 500) break;
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(f.suspicious ? CSColors::Danger : CSColors::TextMuted, "%s", f.pathName.c_str());
                ImGui::TableNextColumn();
                ImGui::TextWrapped("%s", f.valueName.c_str());
                ImGui::TableNextColumn();
                ImGui::TextWrapped("%s", f.value.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

// ---- FILE SEARCH ----
void RenderFileSearchTab() {
    SectionTitle(ICON_FA_SEARCH, L->FileSearch, L->FileSearchDesc);

    static char searchBuf[256] = { 0 };
    static char pathBuf[MAX_PATH * 2] = "C:\\";

    ImGui::TextColored(CSColors::TextMuted, "%s:", L->SearchFileByName);
    ImGui::SetNextItemWidth(400);
    ImGui::InputText("##q", searchBuf, sizeof(searchBuf));

    ImGui::SameLine();
    ImGui::TextColored(CSColors::TextMuted, "%s:", L->FolderPath);
    ImGui::SetNextItemWidth(400);
    ImGui::InputText("##p", pathBuf, sizeof(pathBuf));

    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_FOLDER_OPEN) + " Выбрать").c_str(), ImVec2(140, 32))) {
        ShellExecuteA(nullptr, "open", "explorer.exe", pathBuf, nullptr, SW_SHOWNORMAL);
    }

    ImGui::Spacing();

    if (!g_scanRunning) {
        if (BigButton((std::string(ICON_FA_PLAY) + " " + L->StartScan).c_str(), ImVec2(180, 36), true)) {
            std::string folder = pathBuf;
            std::string query = searchBuf;
            if (!folder.empty() && !query.empty()) {
                g_scanRunning = true;
                g_cancelScan = false;
                std::thread([folder, query]() { PerformFileSearch(folder, query); }).detach();
            }
        }
    }
    else {
        if (BigButton((std::string(ICON_FA_STOP) + " " + L->StopScan).c_str(), ImVec2(180, 36), true)) {
            StopScan();
        }
    }

    ImGui::Spacing();
    RenderResultsByTypes("SearchTable", { "FileSearch" });
}

// ---- STEAM ----
void RenderSteamTab() {
    SectionTitle(ICON_FA_STEAM, L->SteamAccounts, L->SteamDesc);

    if (!g_scanRunning) {
        if (BigButton((std::string(ICON_FA_PLAY) + "  " + L->StartScan).c_str(), ImVec2(200, 40), true))
            StartScan(12);
    }
    else {
        if (BigButton((std::string(ICON_FA_STOP) + "  " + L->StopScan).c_str(), ImVec2(200, 40), true))
            StopScan();
    }
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_FOLDER_OPEN) + "  Папка Steam").c_str(), ImVec2(220, 40))) {
        std::string sp = FindSteamInstallPath();
        if (!sp.empty()) ShellExecuteA(nullptr, "open", sp.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();

    ImGui::BeginChild("##src", ImVec2(0, 50), true);
    ImGui::SetCursorPos(ImVec2(16, 8));
    ImGui::TextColored(CSColors::Primary, ICON_FA_DATABASE "  Источник:");
    ImGui::SameLine();
    std::string steamPath = FindSteamInstallPath();
    std::string vdf = steamPath.empty() ? "(Steam не найден)" : (steamPath + "\\config\\loginusers.vdf");
    ImGui::TextColored(CSColors::TextMuted, "%s", vdf.c_str());
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::TextColored(CSColors::TextMuted, "%s: %zu", L->SteamAccountsFound, g_steamAccounts.size());
    ImGui::Spacing();

    std::lock_guard<std::mutex> lk(g_steamMutex);
    if (g_steamAccounts.empty()) {
        ImGui::TextColored(CSColors::TextMuted, "%s", L->NoSteamAccounts);
        return;
    }

    for (auto& acc : g_steamAccounts) {
        ImGui::PushID(acc.steamID64.c_str());
        ImGui::BeginChild("##acc", ImVec2(0, 140), true);
        ImGui::SetCursorPos(ImVec2(18, 14));
        ImGui::TextColored(CSColors::Primary, ICON_FA_USER "  %s", acc.personaName.c_str());
        if (acc.mostRecent) {
            ImGui::SameLine();
            ImGui::TextColored(CSColors::Success, "  [АКТИВНЫЙ]");
        }
        ImGui::SetCursorPosX(18);
        ImGui::TextColored(CSColors::TextMuted, "Логин: %s", acc.accountName.c_str());
        ImGui::SetCursorPosX(18);
        ImGui::TextColored(CSColors::TextMuted, "Steam ID64: %s", acc.steamID64.c_str());
        ImGui::SetCursorPosX(18);
        ImGui::TextColored(CSColors::TextMuted, "Последний вход: %s", acc.lastLogin.c_str());

        ImGui::SetCursorPos(ImVec2(18, 105));
        if (BigButton((std::string(ICON_FA_LINK) + " Профиль").c_str(), ImVec2(160, 30))) {
            OpenURL("https://steamcommunity.com/profiles/" + acc.steamID64);
        }
        ImGui::SameLine();
        if (BigButton((std::string(ICON_FA_SEARCH) + " Проверить").c_str(), ImVec2(180, 30))) {
            CheckPlayerProfile(acc.steamID64);
        }
        ImGui::EndChild();
        ImGui::PopID();
    }
}

// ---- RECOVERY ----
void RenderRecoveryTab() {
    SectionTitle(ICON_FA_ROTATE_LEFT, L->Recovery, L->RecoveryDesc);

    ImGui::TextWrapped("%s", "Восстановление удалённых файлов для обнаружения скрытых следов. Утилита Hetman Partition Recovery из папки Tools.");

    ImGui::Spacing();
    ImGui::BeginChild("##rec", ImVec2(0, 120), true);
    ImGui::SetCursorPos(ImVec2(20, 18));
    ImGui::TextColored(CSColors::Primary, ICON_FA_HISTORY "  Hetman Partition Recovery");
    ImGui::SetCursorPosX(20);
    ImGui::TextColored(CSColors::TextMuted, "Восстановление удалённых файлов и разделов");
    ImGui::SetCursorPos(ImVec2(20, 70));
    if (BigButton((std::string(ICON_FA_PLAY) + "  " + L->RunTool).c_str(), ImVec2(220, 36), true)) {
        OpenRecoveryTool();
    }
    ImGui::EndChild();
}

// ---- PROCESS STRINGS ----
void RenderProcessStringsTab() {
    SectionTitle(ICON_FA_TERMINAL, L->ProcessStrings,
        "Поиск подозрительных строк в памяти процессов (аналог System Informer → Strings)");

    ImGui::TextWrapped("Автоматизируем ручную проверку через System Informer → Strings. Читаем строки из памяти процесса и фильтруем по паттернам.");
    ImGui::Spacing();
    ImGui::TextColored(CSColors::Warning, "Браузеры: ищем чит-сайты. Если видно /download, /cloud — баним.");
    ImGui::TextColored(CSColors::Danger, "CS2: ищем чит-строки в памяти игры (#unload_popup, XONE, AimStar — прямое доказательство).");
    ImGui::Spacing();

    ImGui::BeginChild("##browser", ImVec2(0, 130), true);
    ImGui::SetCursorPos(ImVec2(18, 14));
    ImGui::TextColored(CSColors::Primary, ICON_FA_GLOBE "  Сканирование браузера");
    ImGui::SetCursorPosX(18);
    ImGui::TextColored(CSColors::TextMuted, "Поиск чит-сайтов в памяти браузера (Chrome, Firefox, Edge, Opera, Yandex)");
    ImGui::SetCursorPosX(18);
    ImGui::TextColored(CSColors::Warning, "ЗА ЧТО БАНИМ: midnight.im, xone.fun, neverlose.cc");
    ImGui::SetCursorPos(ImVec2(18, 92));
    if (!g_scanRunning) {
        if (BigButton((std::string(ICON_FA_PLAY) + "  Сканировать браузер").c_str(), ImVec2(260, 32), true)) {
            g_scanRunning = true; g_cancelScan = false;
            std::thread([]() {
                RPC_OnScanStart("Строки браузера");
                ScanBrowserStrings();
                RPC_OnScanFinish();
                }).detach();
        }
    }
    else {
        if (BigButton((std::string(ICON_FA_STOP) + "  Остановить").c_str(), ImVec2(260, 32), true)) StopScan();
    }
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild("##cs2", ImVec2(0, 130), true);
    ImGui::SetCursorPos(ImVec2(18, 14));
    ImGui::TextColored(CSColors::Primary, ICON_FA_MICROCHIP "  Сканирование Counter-Strike");
    ImGui::SetCursorPosX(18);
    ImGui::TextColored(CSColors::TextMuted, "Поиск чит-строк в памяти CS2");
    ImGui::SetCursorPosX(18);
    ImGui::TextColored(CSColors::Success, "Пример: #unload_popup, XONE, AimStar, Aimmy, Whiskey");
    ImGui::SetCursorPos(ImVec2(18, 92));
    if (!g_scanRunning) {
        if (BigButton((std::string(ICON_FA_PLAY) + "  Сканировать CS2").c_str(), ImVec2(260, 32), true)) {
            g_scanRunning = true; g_cancelScan = false;
            std::thread([]() {
                RPC_OnScanStart("Строки CS2");
                ScanCS2Strings();
                RPC_OnScanFinish();
                }).detach();
        }
    }
    else {
        if (BigButton((std::string(ICON_FA_STOP) + "  Остановить").c_str(), ImVec2(260, 32), true)) StopScan();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::TextColored(CSColors::TextMuted, "Найдено совпадений: %zu", g_processStringHits.size());
    ImGui::Spacing();

    RenderResultsByTypes("StringsTable", { "Browser String", "CS2 String" });
}

// ---- TOOLS ----
void RenderToolsTab() {
    SectionTitle(ICON_FA_GEARS, L->Tools, "Утилиты для ручной проверки системы");
    ImGui::TextColored(CSColors::TextMuted, "Папка Tools должна лежать рядом с .exe. Закинь туда утилиты — они появятся здесь.");
    ImGui::Spacing();

    auto drawCategory = [&](const char* cat, const char* title) {
        ImGui::TextColored(CSColors::Primary, ICON_FA_LAYER_GROUP "  %s", title);
        ImGui::Spacing();
        int col = 0;
        float w = (ImGui::GetContentRegionAvail().x - 12) / 2.0f;

        for (auto& t : g_tools) {
            if (t.category.find(cat) == std::string::npos) continue;

            ImGui::PushID(t.folderName.c_str());
            if (col == 0) ImGui::BeginGroup();
            ImGui::BeginChild("##tool", ImVec2(w, 92), true);
            ImGui::SetCursorPos(ImVec2(14, 12));
            ImGui::TextColored(CSColors::Text, ICON_FA_FILE "  %s", t.folderName.c_str());
            ImGui::SetCursorPosX(14);
            ImGui::TextColored(CSColors::TextMuted, "%s", t.description.c_str());
            ImGui::SetCursorPos(ImVec2(14, 56));

            std::string exePath = "Tools\\" + t.folderName + "\\" + t.exeName;
            bool exists = fs::exists(exePath);
            if (exists) {
                if (BigButton((std::string(ICON_FA_PLAY) + "  " + L->RunTool).c_str(), ImVec2(140, 28), true))
                    LaunchTool(t);
            }
            else {
                ImGui::TextColored(CSColors::Danger, "%s", L->ToolNotFound);
            }
            ImGui::SameLine();
            if (BigButton((std::string(ICON_FA_FOLDER_OPEN) + "  Папка").c_str(), ImVec2(100, 28)))
                OpenToolFolder(t);
            ImGui::EndChild();

            if (col == 0) { ImGui::SameLine(0, 12); col = 1; }
            else { ImGui::EndGroup(); col = 0; }
            ImGui::PopID();
        }
        if (col == 1) ImGui::EndGroup();
        ImGui::Spacing();
        ImGui::Spacing();
        };

    drawCategory("activity", "Анализ активности");
    drawCategory("system", "Системные утилиты");
    drawCategory("usb", "USB и устройства");
    drawCategory("browser", "Браузер");
}

// ---- SYSTEM FOLDERS ----
void RenderSystemFoldersTab() {
    SectionTitle(ICON_FA_FOLDER, L->SystemFolders, L->SystemFoldersDesc);
    auto folders = GetSystemFolders();

    float w = (ImGui::GetContentRegionAvail().x - 24) / 3.0f;
    int col = 0;
    for (auto& f : folders) {
        if (f.path.empty()) continue;
        ImGui::PushID(f.path.c_str());
        if (col == 0) ImGui::BeginGroup();
        ImGui::BeginChild("##f", ImVec2(w, 70), true);
        ImGui::SetCursorPos(ImVec2(14, 12));
        ImGui::TextColored(CSColors::Primary, ICON_FA_FOLDER_OPEN "  %s", f.name.c_str());
        ImGui::SetCursorPosX(14);
        ImGui::TextWrapped("%s", f.path.c_str());
        ImGui::SetCursorPos(ImVec2(w - 90, 22));
        if (BigButton("Открыть", ImVec2(80, 26)))
            ShellExecuteA(nullptr, "open", f.path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        ImGui::EndChild();
        if (col == 0) { ImGui::SameLine(0, 12); col = 1; }
        else if (col == 1) { ImGui::SameLine(0, 12); col = 2; }
        else { ImGui::EndGroup(); col = 0; }
        ImGui::PopID();
    }
    if (col != 0) ImGui::EndGroup();
}

// ---- REPORT ----
void RenderReportTab() {
    SectionTitle(ICON_FA_FILE_EXPORT, L->Report);
    ImGui::TextColored(CSColors::TextMuted, "%s: %zu", L->TotalFindings, g_matchesCount.load());
    ImGui::Spacing();

    if (BigButton((std::string(ICON_FA_FILE_EXPORT) + "   " + L->ExportJSON).c_str(), ImVec2(200, 38), true)) ExportReport();
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_CODE) + "   " + L->ExportHTML).c_str(), ImVec2(200, 38))) ExportHTMLReport();
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_FILE) + "   " + L->ExportCSV).c_str(), ImVec2(200, 38))) ExportCSVReport();
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_TRASH) + "   " + L->ClearResults).c_str(), ImVec2(200, 38))) {
        ClearResults(); g_matchesCount = 0; g_scannedCount = 0;
    }
    ImGui::Spacing();

    std::vector<json> copy;
    { std::lock_guard<std::mutex> lock(g_resultMutex); copy = g_results; }
    if (copy.empty()) ImGui::TextColored(CSColors::TextMuted, "%s", L->NoData);
    else RenderResultsTable("ReportTable", copy);
}

// ---- PROFILE ----
void RenderProfileTab() {
    SectionTitle(ICON_FA_USER, L->ProfileTitle);
    ImGui::TextColored(CSColors::TextMuted, "%s", L->SteamID);
    ImGui::SetNextItemWidth(400);
    static char steamInput[256] = { 0 };
    ImGui::InputTextWithHint("##steamid", L->EnterSteamID, steamInput, IM_ARRAYSIZE(steamInput));
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_SEARCH) + "   " + L->CheckProfile).c_str(), ImVec2(180, 32), true)) {
        if (strlen(steamInput) > 0) CheckPlayerProfile(steamInput);
    }
    ImGui::SameLine();
    if (BigButton((std::string(ICON_FA_XMARK) + "   " + L->Clear).c_str(), ImVec2(120, 32))) {
        g_playerProfile = PlayerProfile{};
        steamInput[0] = 0;
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (g_playerProfile.isLoading) {
        ImGui::TextColored(CSColors::Info, ICON_FA_SPINNER "   %s", L->Checking);
        return;
    }
    if (g_playerProfile.error) {
        ImGui::TextColored(CSColors::Danger, ICON_FA_TRIANGLE_EXCLAMATION "   %s", g_playerProfile.errorMsg.c_str());
        return;
    }
    if (!g_playerProfile.hasData) {
        ImGui::TextDisabled("%s", L->NoProfileData);
        return;
    }

    ImGui::BeginChild("##card", ImVec2(0, 140), true);
    ImGui::SetCursorPos(ImVec2(20, 16));
    ImGui::TextColored(CSColors::Primary, ICON_FA_USER_CIRCLE "  %s",
        g_playerProfile.playerName.empty() ? "(private)" : g_playerProfile.playerName.c_str());
    ImGui::SetCursorPosX(20);
    ImGui::TextColored(CSColors::TextMuted, "SteamID64: %s", g_playerProfile.steamID.c_str());
    if (!g_playerProfile.realName.empty()) {
        ImGui::SetCursorPosX(20);
        ImGui::TextColored(CSColors::TextMuted, "%s: %s", L->RealName, g_playerProfile.realName.c_str());
    }
    if (!g_playerProfile.country.empty()) {
        ImGui::SetCursorPosX(20);
        ImGui::TextColored(CSColors::TextMuted, "%s: %s", L->Country, g_playerProfile.country.c_str());
    }
    if (!g_playerProfile.memberSince.empty()) {
        ImGui::SetCursorPosX(20);
        ImGui::TextColored(CSColors::TextMuted, "%s: %s", L->MemberSince, g_playerProfile.memberSince.c_str());
    }
    ImGui::EndChild();

    ImGui::Spacing();

    bool hasBans = (g_playerProfile.vacBans > 0 || g_playerProfile.gameBans > 0);
    ImVec4 banBg = hasBans
        ? ImVec4(CSColors::Danger.x, CSColors::Danger.y, CSColors::Danger.z, 0.14f)
        : ImVec4(CSColors::Success.x, CSColors::Success.y, CSColors::Success.z, 0.10f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, banBg);
    ImGui::BeginChild("##ban", ImVec2(0, hasBans ? 120 : 70), true);
    ImGui::SetCursorPos(ImVec2(20, 14));
    if (hasBans) {
        ImGui::TextColored(CSColors::Danger, ICON_FA_BAN "  %s", L->Bans);
        if (g_playerProfile.vacBans > 0) {
            ImGui::SetCursorPosX(20);
            ImGui::Text("%s: %d", L->VACBan, g_playerProfile.vacBans);
        }
        if (g_playerProfile.gameBans > 0) {
            ImGui::SetCursorPosX(20);
            ImGui::Text("%s: %d", L->GameBan, g_playerProfile.gameBans);
        }
        if (g_playerProfile.daysSince > 0) {
            ImGui::SetCursorPosX(20);
            ImGui::TextColored(CSColors::TextMuted, "%s: %d", L->DaysSinceLastBan, g_playerProfile.daysSince);
        }
    }
    else {
        ImGui::TextColored(CSColors::Success, ICON_FA_CIRCLE_CHECK "  %s", L->NoBans);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ---- INFO (v3.3: обновлено, добавлен сайт chmonya.ct.ws) ----
void RenderInfoTab() {
    SectionTitle(ICON_FA_CROWN, L->AuthorTitle);
    ImGui::BeginChild("##a", ImVec2(0, 100), true);
    ImGui::SetCursorPos(ImVec2(18, 14));
    ImGui::TextColored(CSColors::Primary, ICON_FA_CODE "  Chmonya");
    ImGui::SetCursorPosX(18);
    ImGui::TextWrapped("%s", L->AuthorText);
    ImGui::EndChild();
    ImGui::Spacing();

    // Раздел "Разработчик" — личные ссылки
    SectionTitle(ICON_FA_LINK, L->DeveloperTitle);
    if (BigButton((std::string(ICON_FA_LINK) + "  chmonya.ct.ws").c_str(), ImVec2(220, 40), true)) OpenURL(MY_WEBSITE);
    ImGui::SameLine();
    if (BigButton(ICON_FA_CODE "   GitHub", ImVec2(200, 40))) OpenURL(MY_GITHUB);
    ImGui::SameLine();
    if (BigButton(ICON_FA_TELEGRAM "   Telegram", ImVec2(180, 40))) OpenURL(MY_TELEGRAM);
    ImGui::SameLine();
    if (BigButton(ICON_FA_DISCORD "   Discord", ImVec2(180, 40))) OpenURL(MY_DISCORD);
    ImGui::Spacing();

    SectionTitle(ICON_FA_BOOK, L->HowToTitle);
    ImGui::BeginChild("##h", ImVec2(0, 180), true);
    ImGui::SetCursorPos(ImVec2(18, 14));
    ImGui::TextWrapped("%s", L->HowToText);
    ImGui::EndChild();

    ImGui::Spacing();
    SectionTitle(ICON_FA_SKULL, L->MemesTitle);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(CSColors::Primary.x, CSColors::Primary.y, CSColors::Primary.z, 0.08f));
    ImGui::BeginChild("##m", ImVec2(0, 140), true);
    ImGui::SetCursorPos(ImVec2(20, 14));
    if (g_isRussian) {
        ImGui::TextColored(CSColors::Primary, ICON_FA_GHOST "  Если чит не найден — не значит что его нет :)");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_FIRE "  Скан памяти в 3 часа ночи — отдельный вид спорта");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_CODE "  Писал этот чекер пока не понял что уже 4 утра");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_MUG_HOT "  Кофе кончился на 3000 строк, а проект всё растёт");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_PIZZA_SLICE "  Кто скушал мою пиццу? Признавайтесь");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_HEART "  Спасибо что пользуешься чекером!");
    }
    else {
        ImGui::TextColored(CSColors::Primary, ICON_FA_GHOST "  If no cheat was found - it doesn't mean there is none :)");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_FIRE "  Memory scan at 3 AM - a separate sport");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_CODE "  Built this checker before realizing it was 4 AM");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_MUG_HOT "  Coffee ran out at line 3000, project keeps growing");
        ImGui::TextColored(CSColors::TextMuted, ICON_FA_HEART "  Thanks for using the checker!");
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ===================== ВЕРХНЕЕ МЕНЮ =====================
int g_activeCategory = 0;

MenuTab g_mainTabs[] = {
    { ICON_FA_HOUSE,    "Главная",    0 },
    { ICON_FA_FILE,     "Файлы",      1 },
    { ICON_FA_MICROCHIP,"Процессы",   2 },
    { ICON_FA_WIFI,     "Сеть",       3 },
};

MenuTab g_analysisTabs[] = {
    { ICON_FA_HISTORY,    "Активность",     4 },
    { ICON_FA_USB,        "USB",            5 },
    { ICON_FA_DATABASE,   "Реестр",         6 },
    { ICON_FA_SEARCH,     "Поиск",          7 },
    { ICON_FA_STEAM,      "Steam",          8 },
    { ICON_FA_ROTATE_LEFT,"Восстановление", 9 },
    { ICON_FA_GLOBE,      "Сайты",         10 },
    { ICON_FA_FOLDER,     "Папки",         11 },
};

MenuTab g_advancedTabs[] = {
    { ICON_FA_TERMINAL,  "Строки",     12 },
    { ICON_FA_GEARS,     "Утилиты",    13 },
    { ICON_FA_HDD,       "Драйверы",   14 },
    { ICON_FA_POWER_OFF, "Автозапуск", 15 },
    { ICON_FA_GEARS,     "Службы",     16 },
};

MenuTab g_reportTabs[] = {
    { ICON_FA_FILE_EXPORT,"Отчёт",     17 },
    { ICON_FA_USER,       "Профиль",   18 },
    { ICON_FA_CIRCLE_INFO,"Инфо",      19 },
};

bool MenuItemButton(const MenuTab& t, int activeTab, const ImVec2& size) {
    bool selected = (activeTab == t.tabIndex);
    ImVec4 bg = selected ? CSColors::Primary : ImVec4(0, 0, 0, 0);
    ImVec4 bgH = selected ? CSColors::PrimaryDark : ImVec4(1, 1, 1, 0.06f);
    ImVec4 txt = selected ? ImVec4(0, 0, 0, 1) : CSColors::Text;

    ImGui::PushStyleColor(ImGuiCol_Button, bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgH);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgH);
    ImGui::PushStyleColor(ImGuiCol_Text, txt);

    std::string label = std::string(t.icon) + "  " + t.label;
    bool r = ImGui::Button(label.c_str(), size);
    ImGui::PopStyleColor(4);
    return r;
}

void RenderTopMenu() {
    ImGuiIO& io = ImGui::GetIO();
    float menuH = 60.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 32), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, menuH), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("##topmenu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + ws.y), IM_COL32(10, 10, 12, 255));
    dl->AddLine(ImVec2(wp.x, wp.y + ws.y - 1), ImVec2(wp.x + ws.x, wp.y + ws.y - 1),
        ImGui::GetColorU32(CSColors::BorderSoft), 1.0f);

    ImGui::SetCursorPos(ImVec2(20, 12));
    if (g_hasLogo && g_logoTexture) {
        ImGui::Image((ImTextureID)(intptr_t)g_logoTexture, ImVec2(36, 36));
        ImGui::SameLine(0, 10);
        ImGui::SetCursorPosY(22);
    }
    ImGui::PushStyleColor(ImGuiCol_Text, CSColors::Primary);
    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("CHMONYA");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);
    ImGui::SetCursorPosY(24);
    ImGui::TextColored(CSColors::TextMuted, "CHECKER v3.3");

    ImGui::SameLine(0, 40);
    ImGui::SetCursorPosY(14);

    auto drawCat = [&](const char* label, int catIdx) {
        bool active = (g_activeCategory == catIdx);
        ImVec4 bg = active ? ImVec4(CSColors::Primary.x, CSColors::Primary.y, CSColors::Primary.z, 0.15f) : ImVec4(0, 0, 0, 0);
        ImVec4 bgH = active ? ImVec4(CSColors::Primary.x, CSColors::Primary.y, CSColors::Primary.z, 0.25f) : ImVec4(1, 1, 1, 0.06f);
        ImVec4 txt = active ? CSColors::Primary : CSColors::Text;
        ImGui::PushStyleColor(ImGuiCol_Button, bg);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgH);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgH);
        ImGui::PushStyleColor(ImGuiCol_Text, txt);
        if (ImGui::Button(label, ImVec2(150, 32))) g_activeCategory = catIdx;
        ImGui::PopStyleColor(4);
        ImGui::SameLine(0, 6);
        };

    drawCat("ГЛАВНОЕ", 0);
    drawCat("АНАЛИЗ", 1);
    drawCat("ПРОДВИНУТОЕ", 2);
    drawCat("ОТЧЁТ", 3);

    float rightX = io.DisplaySize.x - 260;
    ImGui::SetCursorPos(ImVec2(rightX, 16));
    if (!IsRunningAsAdmin())
        ImGui::TextColored(CSColors::Warning, ICON_FA_TRIANGLE_EXCLAMATION "  %s", L->NoAdmin);
    else
        ImGui::TextColored(CSColors::Success, ICON_FA_CIRCLE_CHECK "  %s", L->Admin);

    ImGui::SameLine(0, 16);
    ImGui::PushStyleColor(ImGuiCol_Button, !g_isRussian ? CSColors::Primary : ImVec4(1, 1, 1, 0.06f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, !g_isRussian ? CSColors::PrimaryDark : ImVec4(1, 1, 1, 0.12f));
    ImGui::PushStyleColor(ImGuiCol_Text, !g_isRussian ? ImVec4(0, 0, 0, 1) : CSColors::Text);
    if (ImGui::Button("EN", ImVec2(38, 26))) {
        if (g_isRussian) { g_isRussian = false; L = &L_EN; SaveSettings(); }
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine(0, 4);
    ImGui::PushStyleColor(ImGuiCol_Button, g_isRussian ? CSColors::Primary : ImVec4(1, 1, 1, 0.06f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, g_isRussian ? CSColors::PrimaryDark : ImVec4(1, 1, 1, 0.12f));
    ImGui::PushStyleColor(ImGuiCol_Text, g_isRussian ? ImVec4(0, 0, 0, 1) : CSColors::Text);
    if (ImGui::Button("RU", ImVec2(38, 26))) {
        if (!g_isRussian) { g_isRussian = true; L = &L_RU; SaveSettings(); }
    }
    ImGui::PopStyleColor(3);

    ImGui::End();
    ImGui::PopStyleVar(3);
}

void RenderSubMenu() {
    ImGuiIO& io = ImGui::GetIO();
    float subH = 48.0f;
    float y = 32 + 60;

    ImGui::SetNextWindowPos(ImVec2(0, y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, subH), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("##submenu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + ws.y), IM_COL32(14, 14, 18, 255));
    dl->AddLine(ImVec2(wp.x, wp.y + ws.y - 1), ImVec2(wp.x + ws.x, wp.y + ws.y - 1),
        ImGui::GetColorU32(CSColors::BorderSoft), 1.0f);

    static int active_tab = 0;

    auto drawRow = [&](MenuTab* arr, int count) {
        for (int i = 0; i < count; ++i) {
            if (MenuItemButton(arr[i], active_tab, ImVec2(0, 32))) active_tab = arr[i].tabIndex;
            if (i < count - 1) ImGui::SameLine(0, 8);
        }
        };

    ImGui::SetCursorPosY(8);
    switch (g_activeCategory) {
    case 0: drawRow(g_mainTabs, IM_ARRAYSIZE(g_mainTabs));     break;
    case 1: drawRow(g_analysisTabs, IM_ARRAYSIZE(g_analysisTabs)); break;
    case 2: drawRow(g_advancedTabs, IM_ARRAYSIZE(g_advancedTabs)); break;
    case 3: drawRow(g_reportTabs, IM_ARRAYSIZE(g_reportTabs));   break;
    }

    static int lastRPCtab = -1;
    if (lastRPCtab != active_tab && !g_scanRunning) {
        static const char* tabNames[] = {
            "Главная","Файлы","Процессы","Сеть","Активность","USB","Реестр","Поиск",
            "Steam","Восстановление","Сайты","Папки","Строки","Утилиты","Драйверы",
            "Автозапуск","Службы","Отчёт","Профиль","Инфо"
        };
        if (active_tab >= 0 && active_tab < 20) RPC_OnTabSwitch(tabNames[active_tab]);
        lastRPCtab = active_tab;
    }

    g_activeTab = active_tab;
    ImGui::End();
    ImGui::PopStyleVar(3);
}

// ===================== TITLEBAR =====================
void RenderTitlebar() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 32), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("##titlebar", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 ws = ImGui::GetWindowSize();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + ws.y), IM_COL32(6, 6, 8, 255));
    dl->AddLine(ImVec2(wp.x, wp.y + ws.y - 1), ImVec2(wp.x + ws.x, wp.y + ws.y - 1),
        ImGui::GetColorU32(CSColors::BorderSoft), 1.0f);

    ImGui::SetCursorPos(ImVec2(14, 7));
    ImGui::TextColored(CSColors::TextMuted, ICON_FA_SHIELD_HALVED "  CS2 Cheat Checker by Chmonya v3.3");

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::InvisibleButton("##drag", ImVec2(ws.x - 138, ws.y));
    if (ImGui::IsItemActive() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ReleaseCapture();
        SendMessageA(g_hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }

    ImGui::SetCursorPos(ImVec2(ws.x - 138, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.10f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.16f));
    if (ImGui::Button(ICON_FA_MINUS, ImVec2(46, 32))) ShowWindow(g_hwnd, SW_MINIMIZE);
    ImGui::SameLine(0, 0);
    if (ImGui::Button(ICON_FA_SQUARE, ImVec2(46, 32))) {
        if (g_windowMaximized) { glfwRestoreWindow(g_window);  g_windowMaximized = false; }
        else { glfwMaximizeWindow(g_window); g_windowMaximized = true; }
    }
    ImGui::SameLine(0, 0);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.25f, 0.25f, 0.85f));
    if (ImGui::Button(ICON_FA_XMARK, ImVec2(46, 32))) glfwSetWindowShouldClose(g_window, GLFW_TRUE);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();

    ImGui::End();
    ImGui::PopStyleVar(3);
}

// ===================== CONTENT ROUTER =====================
void RenderContent() {
    ImGuiIO& io = ImGui::GetIO();
    float top = 32 + 60 + 48;
    ImGui::SetNextWindowPos(ImVec2(0, top), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - top), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 16));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, CSColors::Bg);
    ImGui::Begin("##content", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

    int tab = g_activeTab.load();

    bool showSearch =
        (tab == 1 || tab == 2 || tab == 3 || tab == 6 ||
            tab == 12 || tab == 14 || tab == 15 || tab == 16 || tab == 17);
    if (showSearch) {
        ImGui::TextColored(CSColors::TextMuted, "%s", L->Search);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputTextWithHint("##search", L->SearchHint, g_searchQuery, sizeof(g_searchQuery));
        ImGui::SameLine();
        if (BigButton("Очистить", ImVec2(120, 26))) g_searchQuery[0] = 0;
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    switch (tab) {
    case 0: RenderHomeTab(); break;

    case 1:
        SectionTitle(ICON_FA_FILE, L->Files, "Скан файловой системы");
        RenderScanControls(0);
        RenderResultsByTypes("FilesTable", { "File", "Signature", "Hidden" });
        break;

    case 2:
        SectionTitle(ICON_FA_MICROCHIP, L->Processes, "Скан запущенных процессов");
        RenderScanControls(1);
        RenderResultsByTypes("ProcTable", { "Process", "Module", "Memory" });
        break;

    case 3:
        SectionTitle(ICON_FA_WIFI, L->Network, "Анализ сетевых соединений");
        RenderScanControls(2);
        RenderResultsByTypes("NetTable", { "Network" });
        break;

    case 4:  RenderActivityTab();       break;
    case 5:  RenderUSBTab();            break;
    case 6:  RenderRegistryTab();       break;
    case 7:  RenderFileSearchTab();     break;
    case 8:  RenderSteamTab();          break;
    case 9:  RenderRecoveryTab();       break;
    case 10: RenderSitesTab();          break;
    case 11: RenderSystemFoldersTab();  break;
    case 12: RenderProcessStringsTab(); break;
    case 13: RenderToolsTab();          break;

    case 14:
        SectionTitle(ICON_FA_HDD, L->Drivers, "Скан драйверов ядра");
        RenderScanControls(9);
        RenderResultsByTypes("DrvTable", { "Driver" });
        break;

    case 15:
        SectionTitle(ICON_FA_POWER_OFF, L->Autorun, "Проверка ключей автозапуска");
        RenderScanControls(7);
        RenderResultsByTypes("AutoTable", { "Registry" });
        break;

    case 16:
        SectionTitle(ICON_FA_GEARS, L->Services, "Проверка системных служб");
        RenderScanControls(6);
        RenderResultsByTypes("SrvTable", { "Service" });
        break;

    case 17: RenderReportTab();  break;
    case 18: RenderProfileTab(); break;
    case 19: RenderInfoTab();    break;

    default:
        ImGui::TextColored(CSColors::TextMuted, "Вкладка в разработке...");
        break;
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// ===================== ГЛАВНЫЙ UI =====================
void RenderUI() {
    ImGuiIO& io = ImGui::GetIO();

    ImDrawList* bgDL = ImGui::GetBackgroundDrawList();
    if (g_hasBackground && g_backgroundTexture) {
        bgDL->AddImage((ImTextureID)(intptr_t)g_backgroundTexture,
            ImVec2(0, 0), io.DisplaySize, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
        bgDL->AddRectFilled(ImVec2(0, 0), io.DisplaySize, IM_COL32(4, 4, 6, 160));
    }
    else {
        bgDL->AddRectFilledMultiColor(ImVec2(0, 0), io.DisplaySize,
            IM_COL32(6, 6, 8, 255), IM_COL32(10, 10, 14, 255),
            IM_COL32(10, 10, 14, 255), IM_COL32(6, 6, 8, 255));
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_E)) ExportReport();
    if (ImGui::IsKeyPressed(ImGuiKey_Escape) && g_scanRunning) StopScan();

    RenderTitlebar();
    RenderTopMenu();
    RenderSubMenu();
    RenderContent();
}

// ===================== FONT PICKER =====================
std::string PickBestFont(const std::string& exeDir) {
    std::vector<std::string> candidates;
    for (auto& p : FONT_CANDIDATES) {
        if (p.size() > 2 && p[1] == ':') candidates.push_back(p);
        else {
            candidates.push_back(exeDir + "\\" + p);
            candidates.push_back(p);
        }
    }
    for (auto& p : candidates) {
        std::error_code ec;
        if (fs::exists(p, ec)) return p;
    }
    return "C:\\Windows\\Fonts\\segoeui.ttf";
}

// ===================== MAIN =====================
int main(int argc, char** argv) {
    // --- RPC proxy mode (отдельный процесс) ---
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--rpc-proxy") == 0) {
            RunRPCProxy();
            return 0;
        }
    }

    InitFolders();

    // Авто-перезапуск от админа
    if (!IsRunningAsAdmin()) {
        if (RelaunchAsAdmin()) return 0;
    }

    if (!glfwInit()) {
        MessageBoxA(nullptr, "GLFW init failed.", "Error", MB_ICONERROR);
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1600, 900,
        "CS2 Cheat Checker by Chmonya v3.3", nullptr, nullptr);
    if (!window) {
        MessageBoxA(nullptr, "Failed to create window.", "Error", MB_ICONERROR);
        glfwTerminate();
        return 1;
    }

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = fs::path(exePath).parent_path().string();

    HWND hwnd = glfwGetWin32Window(window);
    g_hwnd = hwnd;
    g_window = window;

    // Иконка окна
    if (hwnd) {
        std::string icoPath = exeDir + "\\assets\\favicon.ico";
        HICON hIconBig = (HICON)LoadImageA(NULL, icoPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        HICON hIconSmall = (HICON)LoadImageA(NULL, icoPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIconBig) {
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
            SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hIconBig);
        }
        if (hIconSmall) {
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
            SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hIconSmall);
        }
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Загрузка текстур
    {
        std::string bgPath = exeDir + "\\assets\\background.png";
        std::string logoPath = exeDir + "\\assets\\logo.png";
        if (fs::exists(bgPath)) { g_backgroundTexture = LoadTextureFromFile(bgPath.c_str());   g_hasBackground = g_backgroundTexture != 0; }
        if (fs::exists(logoPath)) { g_logoTexture = LoadTextureFromFile(logoPath.c_str()); g_hasLogo = g_logoTexture != 0; }
    }

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = "settings/imgui.ini";

    ApplyCSArenaStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Шрифты
    std::string chosenFont = PickBestFont(exeDir);

    static const ImWchar glyphRanges[] = {
        0x0020, 0x007F, 0x00A0, 0x00FF, 0x0100, 0x017F,
        0x0400, 0x04FF, 0x0500, 0x052F, 0x2000, 0x206F,
        0x20A0, 0x20BF, 0x2190, 0x21FF, 0x2500, 0x257F,
        0x25A0, 0x25FF, 0,
    };

    ImFontConfig fontCfg;
    fontCfg.OversampleH = 3;
    fontCfg.OversampleV = 1;
    fontCfg.PixelSnapH = true;
    fontCfg.RasterizerMultiply = 1.10f;

    ImFont* mainFont = io.Fonts->AddFontFromFileTTF(chosenFont.c_str(), 16.0f, &fontCfg, glyphRanges);
    if (!mainFont) mainFont = io.Fonts->AddFontDefault();

    static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig iconCfg;
    iconCfg.MergeMode = true;
    iconCfg.PixelSnapH = true;
    iconCfg.GlyphMinAdvanceX = 18.0f;
    iconCfg.GlyphOffset = ImVec2(0.0f, 1.0f);
    if (fs::exists(FONT_ICONS)) io.Fonts->AddFontFromFileTTF(FONT_ICONS.c_str(), 16.0f, &iconCfg, iconRanges);

    ImFontConfig brandCfg = iconCfg;
    if (fs::exists(FONT_BRANDS)) io.Fonts->AddFontFromFileTTF(FONT_BRANDS.c_str(), 16.0f, &brandCfg, iconRanges);

    io.FontDefault = mainFont;
    io.Fonts->Build();

    // Инициализация
    LoadSettings();
    RefreshProcessList();
    RPC_Init();
    RefreshToolsList();

    g_autoStart = IsAutoStartEnabled();
    LoadHistoryList();

    // Ахо-Корасик строится ОДИН РАЗ при старте
    g_keywordAC.Build(KEYWORDS);

    // Hash DB — в фоне
    std::thread([]() { LoadHashDatabase(); }).detach();

    auto last = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window) && !g_appQuit) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        if (g_showSplash.load()) RenderSplash(dt);
        else                     RenderUI();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.02f, 0.02f, 0.03f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Корректное завершение
    g_appQuit = true;
    g_cancelScan = true;
    g_taskQueue.notify_all();

    if (g_backgroundTexture) glDeleteTextures(1, &g_backgroundTexture);
    if (g_logoTexture)       glDeleteTextures(1, &g_logoTexture);

    RPC_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}