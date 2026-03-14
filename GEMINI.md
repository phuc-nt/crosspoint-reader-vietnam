# CrossPoint Reader Vietnam — Development Guide

## Project Identity
- **Name**: CrossPoint Reader Vietnam
- **Type**: ESP32-C3 embedded firmware (e-reader)
- **Language**: C++20 (PlatformIO + Arduino-ESP32)
- **Upstream**: Fork of `crosspoint-reader/crosspoint-reader`
- **Goal**: Vietnamese localization of CrossPoint firmware for Xteink X4

## Agent Role
You are a Senior Embedded Systems Engineer specializing in ESP32/Arduino firmware development.
Your primary mission is maintaining the Vietnamese fork of CrossPoint Reader.

## Critical Constraints
- **380KB RAM** is the hard ceiling. Every allocation must be justified.
- **6.5MB flash** per OTA partition. Font data lives here.
- **Single-core RISC-V** — no true parallelism, only cooperative multitasking.
- **No exceptions, no RTTI** — error handling via return codes + LOG_ERR.

## ⚠️ CRITICAL: Rules That Prevent Device Bricking

> **These rules exist because violating them has BRICKED REAL DEVICES, requiring physical USB recovery. NEVER break these rules.**

### 1. Static Global Constructors: NO HEAP ALLOCATION
- `static MyClass instance;` → constructor runs **before `setup()`** during C++ global init
- After deep sleep wakeup, **heap is NOT ready** at this point
- **ANY** heap allocation (`new`, `make_unique`, `std::string`, `std::vector`) in a static constructor → **Store Access Fault → crash loop → BRICKED DEVICE**
- **FIX**: Constructor must be a no-op. Defer all init to an explicit method called from `setup()`
- **Example**: `UITheme::UITheme()` must be empty; `UITheme::reload()` does the actual init

### 2. Never Deep Sleep on USB Power Wakeup
- `WakeupReason::AfterUSBPower` + `startDeepSleep()` = **infinite crash loop**
- Device becomes unresponsive while USB cable is connected
- **FIX**: Just `break;` — let the device boot normally

### 3. Changing Defaults in CrossPointSettings.h Does NOT Work
- Settings saved on SD card **override ALL defaults** in code
- Changing `uint8_t foo = NEW_VALUE` has **zero effect** on existing devices
- **FIX**: Force-override in `CrossPointSettings::loadFromFile()` after loading, then set `resave = true`

### 4. Before Flashing: Always Verify
- Always check USB port: `ls /dev/cu.usb*`
- Always monitor serial after flash: `pio device monitor`
- If device is in crash loop, it may appear on a **different USB port** than expected
- Use `addr2line` to decode crash addresses from serial output

```
├── pm_docs/              # PM documentation (vision, dev plan, dev log)
├── lib/
│   ├── I18n/             # Internationalization
│   │   └── translations/ # YAML files per language (including vietnamese.yaml)
│   ├── EpdFont/          # Font system
│   │   ├── builtinFonts/ # Generated bitmap font headers (.h)
│   │   │   └── source/   # Source TTF/OTF files
│   │   └── scripts/      # Font conversion tools
│   └── hal/              # Hardware Abstraction Layer
├── src/
│   ├── activities/       # UI screens (Activity lifecycle pattern)
│   │   ├── reader/       # Reader activities + BookmarkListActivity
│   │   └── home/         # Home, FileBrowser, Settings activities
│   ├── BookmarkStore.*   # Per-book bookmark persistence (JSON on SD)
│   ├── ReadingStats.*    # Reading time/pages tracking (singleton)
│   ├── FileBrowserMetaCache.* # Epub title/author cache for file browser
│   ├── fontIds.h         # Font ID constants
│   └── main.cpp          # Entry point, global font loading
├── scripts/
│   └── gen_i18n.py       # I18n code generator
├── open-x4-sdk/          # Low-level hardware SDK (submodule)
└── GEMINI.md             # This file
```

## Key Workflows

### Adding/Modifying Vietnamese Translations
1. Edit `lib/I18n/translations/vietnamese.yaml`
2. Run: `python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/`
3. Build: `pio run`

### Adding/Modifying Fonts
1. Place TTF/OTF source in `lib/EpdFont/builtinFonts/source/<FontName>/`
2. Update `lib/EpdFont/scripts/convert-builtin-fonts.sh`
3. Run conversion: `cd lib/EpdFont/scripts && bash convert-builtin-fonts.sh`
4. Update `lib/EpdFont/builtinFonts/all.h` with new includes
5. Run: `cd lib/EpdFont/scripts && bash build-font-ids.sh`
6. Update `src/fontIds.h` with new font IDs
7. Register fonts in `src/main.cpp` (inside `#ifndef OMIT_FONTS`)
8. Build: `pio run`

### Building Firmware
```bash
pio run                    # Build default environment
pio run -e gh_release      # Build production
pio run -t upload          # Build + flash to device
```

## Vietnamese-Specific Notes
- **Vietnamese Unicode ranges** already included in `fontconvert.py`:
  - U+01A0-01A1 (Ơ/ơ), U+01AF-01B0 (Ư/ư)
  - U+1EA0-1EF9 (all precomposed Vietnamese diacritics)
- **2 Vietnamese fonts**: Literata (serif), Be Vietnam Pro (sans-serif, default)
- **Vollkorn removed** — only Classic theme, no Lyra themes
- **All fonts are SIL OFL licensed** — safe for firmware embedding
- **Upstream sync**: Keep changes minimal and focused on Vietnamese support to ease merging

## Coding Standards
- Use `tr(STR_KEY)` for ALL user-facing text (I18n system)
- Use `LOG_INF/LOG_DBG/LOG_ERR` for logging (not Serial.print)
- Use `Storage` singleton for SD card I/O
- Use `GUI` singleton for UI rendering
- Use `READING_STATS` for reading statistics, `BROWSER_META` for file browser metadata cache
- No hardcoded screen dimensions — use `renderer.getScreenWidth()/getScreenHeight()`
- Free all memory in `onExit()` that was allocated in `onEnter()`
- Prefer `static constexpr` for compile-time constants
- Singleton pattern: `static MyClass instance;` + `getInstance()` + `#define MACRO getInstance()`
- **Data files on SD**: Store in `/.crosspoint/` directory (bookmarks, stats, metadata cache)

## PM Documentation
- **Vision**: `pm_docs/VISION.md` — Tiền đề, mục tiêu, scope (ít thay đổi)
- **Roadmap**: `pm_docs/ROADMAP.md` — Tổng quan các đợt
- **Per-sprint**: `pm_docs/01-*.md`, `02-*.md`... — Mỗi đợt 1 file (plan + log)
- Luôn check ROADMAP trước khi bắt đầu đợt mới
- Cập nhật file đợt sau mỗi session

## Reference
- Upstream CLAUDE.md has detailed technical documentation (800+ lines)
- See `docs/i18n.md` for I18n system documentation
- See `docs/contributing/` for architecture and development workflow
- See `SCOPE.md` for feature scope boundaries

