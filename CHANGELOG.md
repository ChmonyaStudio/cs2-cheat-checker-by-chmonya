# Changelog

All notable changes to this project are documented here.

## [Unreleased]

### Planned
- Custom scan profiles (quick / full / deep)
- Hash DB for known cheat builds
- Background scanning during CS2 match

---

## [3.3.0] — 2026-09-16

### Added
- 📊 Extended registry scanning:
  - RecentDocs — recently opened files
  - TypedPaths — manually typed paths in explorer
  - RunMRU — Win+R command history
  - WordWheelQuery — explorer search history
  - RDP History (Terminal Server Client)
  - RecentApps — recently launched apps
  - Startup Approved
  - Sysmon config (if installed)
- Suspicious registry findings now also go to the main results list
- New personal site link: chmonya.ct.ws
- Rebranded to ChmonyaStudios (removed all third-party references)

### Fixed
- `g_scannedCount` now correctly increments in recursive registry reading
- `OpenInExplorer` no longer tries to open "String: ..." entries
- RPC pipe name changed to `chmonya-cc-rpc` (was conflicting with old)
- Webhook URL placeholder guard (no more 404 spam)

### Changed
- HTML report footer rebranded
- Splash + Titlebar now show "CHMONYA"
- App version bump 3.2 → 3.3

---

## [3.2.0] — 2026-09-15

### Added
- RAII handle wrapper (`ScopedHandle`) — no more HANDLE leaks
- Binary-safe Aho-Corasick (`ContainsRaw`) — no UB on null bytes
- `npos` guards in Steam ID parser — no crash on malformed input
- 100+ cheat signatures in keyword DB

### Fixed
- Crash on binary data in memory scan (fixed by binary-safe AC)
- Random crashes in Steam ID parser

---

## [3.1.0] — 2026-09-10

### Added
- 17 scan modules
- Discord Rich Presence via named pipe proxy
- HTML report with branding and stats
- VirusTotal integration
- RU / EN interface
- Sound alerts on findings

---

## [3.0.0] — 2026-09-01

### Added
- Initial public release
- Core scanning engine
- Basic UI with ImGui
- JSON / CSV export
