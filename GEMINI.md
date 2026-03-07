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

## Project Structure
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
- **3 Vietnamese fonts**: Literata (serif), Be Vietnam Pro (sans-serif), Vollkorn (classic serif)
- **All fonts are SIL OFL licensed** — safe for firmware embedding
- **Upstream sync**: Keep changes minimal and focused on Vietnamese support to ease merging

## Coding Standards
- Use `tr(STR_KEY)` for ALL user-facing text (I18n system)
- Use `LOG_INF/LOG_DBG/LOG_ERR` for logging (not Serial.print)
- Use `Storage` singleton for SD card I/O
- Use `GUI` singleton for UI rendering
- No hardcoded screen dimensions — use `renderer.getScreenWidth()/getScreenHeight()`
- Free all memory in `onExit()` that was allocated in `onEnter()`
- Prefer `static constexpr` for compile-time constants

## PM Documentation
- **Vision**: `pm_docs/VISION.md` — Project background and goals
- **Dev Plan**: `pm_docs/DEV_PLAN.md` — Milestones and task tracking
- **Dev Log**: `pm_docs/DEV_LOG.md` — Session logs by build/release
- Always update Dev Log after completing significant work
- Always check Dev Plan for current priorities before starting work

## Reference
- Upstream CLAUDE.md has detailed technical documentation (800+ lines)
- See `docs/i18n.md` for I18n system documentation
- See `docs/contributing/` for architecture and development workflow
- See `SCOPE.md` for feature scope boundaries
