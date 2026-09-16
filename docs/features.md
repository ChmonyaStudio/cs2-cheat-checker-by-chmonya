# Features — deep dive

Full breakdown of every scan module.

## 1. Files (`ScanFiles`)

Scans Desktop, Downloads, Documents, AppData (Roaming + Local), C:\Users, C:\ProgramData.

**What it does:**
- Name match against 200+ cheat keywords
- Content scan (binary-safe) for `.exe`, `.dll`, `.sys`
- Signature verification (`WinVerifyTrust`)
- Severity based on signature: signed = 1, unsigned = 3

**Exclusions:** `g_ignoreList` (Windows, System32, Steam, etc.)

**Max files:** 50,000 (configurable via `SCAN_FILES_MAX`)

**Multi-threaded:** Yes (std::thread, hardware_concurrency)

---

## 2. Processes (`ScanProcesses`)

Enumerates all running processes via `CreateToolhelp32Snapshot`.

**What it does:**
- Name + full path match against cheat keywords
- Loads modules (DLLs) of each process
- Matches DLL names against cheat keywords

**Privileges:** PROCESS_QUERY_LIMITED_INFORMATION for path, PROCESS_QUERY_INFORMATION + PROCESS_VM_READ for modules

---

## 3. Network (`ScanNet`)

Uses `GetExtendedTcpTable` with `TCP_TABLE_OWNER_PID_ALL`.

**What it does:**
- Lists all active IPv4 TCP connections
- Resolves owning PID to process path
- Matches process name against cheat keywords

**Not covered:** UDP, IPv6

---

## 4. Task Scheduler (`ScanTaskScheduler`)

Scans `C:\Windows\System32\Tasks` and `C:\Windows\Tasks`.

**What it does:**
- Reads every task XML file
- Matches name + content against cheat keywords

---

## 5. Hidden files (`ScanHidden`)

Recursively scans AppData, ProgramData, C:\Users.

**What it does:**
- Checks `FILE_ATTRIBUTE_HIDDEN` / `FILE_ATTRIBUTE_SYSTEM`
- Matches names against cheat keywords

---

## 6. Signatures (`ScanSignatures`)

Focuses on hot paths: Temp, LocalAppData, RoamingAppData, ProgramData, Public.

**What it does:**
- For `.exe`, `.dll`, `.sys` — content scan + signature check
- Reports unsigned files with cheat content separately
- Special case: `imgui.ini` = flag (common in internal cheats)

---

## 7. Services (`ScanServices`)

Enumerates via `EnumServicesStatusEx`.

**What it does:**
- Matches service name + display name against cheat keywords

---

## 8. Autorun (`ScanAutorun`)

Registry keys:
- `HKLM\...\Run`, `RunOnce`
- `HKCU\...\Run`, `RunOnce`
- `HKLM\Wow6432Node\...\Run`

**What it does:**
- Reads all values
- Matches name + data against cheat keywords

---

## 9. Memory (`ScanMemory`)

Scans memory of a single selected process.

**What it does:**
- VirtualQueryEx through whole process address space
- Reads `MEM_COMMIT` + `PAGE_READWRITE` regions
- Binary-safe Aho-Corasick (`ContainsRaw`) — works with null bytes
- Stops at first match per chunk

**Select:** use dropdown to pick process (auto-selects `cs2.exe` if running)

---

## 10. Drivers (`ScanDrivers`)

Enumerates via `EnumDeviceDrivers`.

**Known vulnerable drivers:**
- `kdmapper.sys`, `kdu.sys`, `iqvw64e.sys`, `capcom.sys`
- `gdrv.sys`, `rtcore64.sys`, `msio64.sys`, `dbk64.sys`
- `ene.sys`, `asusio.sys`, `winio.sys`, `winring0.sys`

---

## 11. Activity (`ScanActivity`)

Forensic registry paths:

### UserAssist (ROT13 encoded)
`HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\UserAssist`

### BAM (Background Activity Monitor)
`HKLM\SYSTEM\CurrentControlSet\Services\bam\State\UserSettings`

### MUICache
`HKCU\SOFTWARE\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache`

**What it does:**
- ROT13-decodes UserAssist keys
- Matches decoded value + path against cheat keywords

---

## 12. USB (`ScanUSBDevices`)

Registry: `HKLM\SYSTEM\CurrentControlSet\Enum\USBSTOR`

**What it does:**
- Lists all USB storage devices
- Shows friendly name + registry path

**Use case:** detect KMBox / Arduino hardware cheats.

---

## 13. Steam (`ScanSteamAccounts`)

Reads `loginusers.vdf` from Steam install folder.

**What it does:**
- Parses VDF (Valve Data Format)
- Extracts SteamID64, PersonaName, AccountName, Timestamp
- Marks the most recent account
- Also scans `steamapps/common` for suspicious file names

---

## 14. Browser (`ScanBrowserHistory`)

Reads browser history files directly.

**Browsers:**
- Chrome, Edge, Brave, Yandex
- Firefox (places.sqlite), Opera

**Cheat domains (partial list):**
`xone.fun`, `midnight.im`, `aimware.net`, `nixware.cc`, `onetap.com`, `neverlose.cc`, `interium.ooo`, `yougame.biz`, `lolz.market`, `gamesense.pub`, `fatality.win`, `primordial.dev`, `memesense.gg`, `spirthack.me`, `ev0lve.xyz`, `legendware.pw`, `exloader.ru`, `aimware.wtf`

**Method:** raw string search in History file (works even for locked browsers if you have read access).

---

## 15. Browser Strings (`ScanBrowserStrings`)

Reads memory of running browser processes.

**Browsers scanned:**
`chrome.exe`, `msedge.exe`, `firefox.exe`, `opera.exe`, `brave.exe`, `yandex.exe`, `browser.exe`

**Patterns:** 100+ cheat-site URLs.

**Method:**
- `ReadProcessMemory` through each browser
- Extract printable ASCII strings (≥6 chars)
- Match against URL list
- Reports any URL hit as severity 5

---

## 16. CS2 Strings (`ScanCS2Strings`)

Reads memory of `cs2.exe`.

**Patterns (partial):**
`#unload_popup`, `[XONE`, `AimStar`, `Aimmy`, `Whiskey`, `INTERIUM`, `Enigma`, `Luno`, `pastehook`, `NAIM`, `plague`, `#main_window`, `#watermark`, `aimbot.dll`, `wallhack`, `triggerbot`, `silent_aim`, `backtrack`, `antiaim`

**Severity:** 5 (direct evidence).

---

## 17. Registry (`ScanRegistryPaths`)

Extended forensic paths:

- BAM (Background Activity Monitor)
- Compatibility Assistant
- AppCompat Layers
- Windows Defender Exclusions
- UserAssist (ROT13)
- MUICache
- ShellBags
- **RecentDocs** — recently opened files
- **TypedPaths** — manually typed paths in explorer
- **RunMRU** — Win+R command history
- **WordWheelQuery** — explorer search history
- **RDP History** — Terminal Server Client
- **RecentApps** — recently launched apps
- **Startup Approved**
- **Sysmon Config** (if installed)

**What it does:**
- Recursive read (depth 3)
- Matches value names + data against cheat keywords
- Suspicious findings go to global results AND local findings list

---

## Extra features

### VirusTotal integration
Click **VT** in results → opens `virustotal.com/gui/file/<sha256>`.

SHA256 computed only for File / Signature / Module / Driver results.

### HTML report
- Dark orange theme
- Embedded logo + background (base64)
- Stats: total / high / medium / low
- Table with all findings
- VT links per row
- Footer with links to author

### Discord Rich Presence
- Shows current tab + found count
- Proxy runs as separate process (works with admin rights)
- Named pipe: `\\.\pipe\chmonya-cc-rpc`

### Language toggle
- EN / RU
- All UI strings localized
- Saved in `settings.json`
