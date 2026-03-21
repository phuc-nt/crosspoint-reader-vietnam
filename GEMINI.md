# CrossPoint Reader Vietnam — Development Guide

> Fork: `phuc-nt/crosspoint-reader-vietnam` ← Upstream: `crosspoint-reader/crosspoint-reader`

## 🇻🇳 Vietnamese Fork — What's Different

### New Modules (not in upstream)
| Module | Description | Location |
|--------|-------------|----------|
| Vietnamese I18n | Full UI translation + 2 VN-optimized fonts | `lib/I18n/translations/vietnamese.yaml` |
| BookmarkStore | Per-book bookmark persistence (JSON on SD) | `src/BookmarkStore.*` |
| BookmarkListActivity | Browse/navigate/delete bookmarks | `src/activities/reader/BookmarkListActivity.*` |
| ReadingStats | Reading time + pages tracking (singleton) | `src/ReadingStats.*` |
| FileBrowserMetaCache | Epub title/author cache for file browser | `src/FileBrowserMetaCache.*` |
| BookInfoActivity | Book metadata detail screen | `src/activities/home/BookInfoActivity.*` |

### Modified Behavior (vs upstream)
- **Home** — shows reading stats, faster load (skipped Storage.exists check)
- **File Browser** — shows epub title/author, 4 sort modes (name A→Z/Z→A, size asc/desc)
- **Reader** — loading indicator, bookmark add/list/navigate
- **Fonts** — Be Vietnam Pro (default) + Literata, Vollkorn removed
- **Sleep screen** — shows book cover by default

### Vietnamese Unicode Ranges
```
U+01A0-01A1 (Ơ/ơ), U+01AF-01B0 (Ư/ư)
U+1EA0-1EF9 (all precomposed Vietnamese diacritics)
```

---

## Project Structure

```
├── pm_docs/              # PM documentation (vision, roadmap, sprint logs)
├── lib/
│   ├── I18n/             # Internationalization
│   │   └── translations/ # YAML files per language (incl. vietnamese.yaml)
│   ├── Epub/             # EPUB parser + renderer
│   ├── EpdFont/          # Font system (bitmap fonts from TTF/OTF)
│   ├── GfxRenderer/      # E-ink framebuffer rendering
│   └── hal/              # HAL: HalDisplay, HalGPIO, HalStorage
├── src/
│   ├── activities/       # UI screens (Activity lifecycle: onEnter/loop/onExit)
│   │   ├── reader/       # Reader + BookmarkListActivity
│   │   ├── home/         # Home, FileBrowser, BookInfo, RecentBooks, Settings
│   │   └── util/         # ConfirmationActivity, KeyboardEntry, ButtonNavigator
│   ├── components/       # UITheme, BaseTheme, Lyra themes
│   ├── BookmarkStore.*   # Bookmark persistence
│   ├── ReadingStats.*    # Reading stats singleton
│   ├── FileBrowserMetaCache.* # Epub metadata cache
│   ├── fontIds.h         # Font ID constants
│   └── main.cpp          # Entry point, global font loading
├── scripts/
│   └── gen_i18n.py       # I18n code generator
├── open-x4-sdk/          # Low-level hardware SDK (submodule)
└── GEMINI.md             # This file
```

---

## Critical Constraints

- **380KB RAM** — hard ceiling. Justify every allocation.
- **6.5MB flash** per OTA partition. Font data lives here.
- **Single-core RISC-V** (ESP32-C3) — cooperative multitasking only.
- **No exceptions, no RTTI** — error handling via return codes + LOG_ERR.
- **48KB framebuffer** — single buffer mode (no double buffering).

## ⚠️ Rules That Prevent Device Bricking

> These have BRICKED REAL DEVICES. NEVER break them.

1. **Static Global Constructors: NO HEAP**
   - `static MyClass instance;` → constructor runs BEFORE `setup()`
   - ANY `new`/`make_unique`/`std::string`/`std::vector` → crash loop → BRICK
   - FIX: Constructor = no-op. Defer init to explicit `reload()`/`init()` method.

2. **Never Deep Sleep on USB Power Wakeup**
   - `WakeupReason::AfterUSBPower` + `startDeepSleep()` = infinite crash loop
   - FIX: Just `break;` — boot normally.

3. **Changing Defaults in CrossPointSettings.h Does NOT Work**
   - SD card saved settings override ALL code defaults
   - FIX: Force-override in `loadFromFile()`, set `resave = true`.

4. **Before Flashing**
   - Check port: `ls /dev/cu.usb*`
   - Monitor after flash: `pio device monitor`
   - Crash loop → may appear on different USB port
   - Decode: `addr2line -e .pio/build/default/firmware.elf -f -p <MEPC>`

---

## Key Workflows

### Build & Flash
```bash
pio run                    # Build
pio run -t upload          # Build + flash
pio run -e gh_release      # Production build
pio device monitor         # Serial monitor
```

### I18n Translations
```bash
# 1. Edit YAML
vim lib/I18n/translations/vietnamese.yaml

# 2. Regenerate code
python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/

# 3. Build
pio run
```

### Adding Fonts
1. Place TTF/OTF in `lib/EpdFont/builtinFonts/source/<FontName>/`
2. Update `lib/EpdFont/scripts/convert-builtin-fonts.sh`
3. Run: `cd lib/EpdFont/scripts && bash convert-builtin-fonts.sh`
4. Update `all.h`, run `build-font-ids.sh`, update `fontIds.h`
5. Register in `src/main.cpp` (inside `#ifndef OMIT_FONTS`)

---

## Coding Standards

### Must-Use Patterns
- `tr(STR_KEY)` for ALL user-facing text (I18n)
- `LOG_INF/LOG_DBG/LOG_ERR` for logging (never Serial.print)
- `Storage` singleton for SD I/O
- `GUI` singleton for UI rendering
- `READING_STATS`, `BROWSER_META`, `BOOKMARK_STORE` singletons
- `renderer.getScreenWidth()/getScreenHeight()` — no hardcoded dimensions

### Memory Rules
- Free in `onExit()` what you allocated in `onEnter()`
- `static constexpr` for compile-time constants
- `std::unique_ptr` for ownership (avoid `shared_ptr`)
- `reserve()` before `push_back()` loops
- Stack locals < 256 bytes; larger → heap with `malloc`/`free`
- Data on SD: `/.crosspoint/` directory

### Singleton Pattern
```cpp
static MyClass instance;                    // In .cpp
static MyClass& getInstance() { return instance; } // In .h
#define MACRO MyClass::getInstance()        // Convenience macro
```

---

## Singletons Reference

| Macro | Class | Purpose |
|-------|-------|---------|
| `SETTINGS` | `CrossPointSettings` | User settings |
| `APP_STATE` | `CrossPointState` | Runtime state |
| `GUI` | `UITheme` | Current theme / UI rendering |
| `Storage` | `HalStorage` | SD card I/O |
| `I18N` | `I18n` | Internationalization |
| `RECENT_BOOKS` | `RecentBooksStore` | Recent books list |
| `READING_STATS` | `ReadingStats` | Time + pages tracking |
| `BROWSER_META` | `FileBrowserMetaCache` | Epub metadata cache |

---

## ESP32-C3 Pitfalls (Quick Reference)

| Issue | Rule |
|-------|------|
| `string_view` + C API | Convert to `std::string(view).c_str()` — not null-terminated |
| ISR handlers | Must be `IRAM_ATTR`; data accessed from ISR → `DRAM_ATTR` |
| RISC-V alignment | Never cast `uint8_t*` to wider type — use `memcpy` |
| `std::function` | Adds 2-4KB per signature + heap alloc — prefer function pointers |
| Watchdog | Add `vTaskDelay(1)` in tight loops (>5s block = crash) |

---

## PM Documentation

- **Vision**: `pm_docs/VISION.md` — project goals, scope
- **Roadmap**: `pm_docs/ROADMAP.md` — sprint overview
- **Per-sprint**: `pm_docs/01-*.md`, `02-*.md`... — plan + log per sprint

## Upstream Reference

For detailed technical docs not covered here:
- `docs/contributing/` — architecture patterns, development workflow
- `docs/file-formats.md` — binary cache format specs
- `docs/i18n.md` — I18n system documentation
- `SCOPE.md` — feature scope boundaries
- `GOVERNANCE.md` — community principles

---

*Philosophy: We are building a dedicated e-reader for Vietnamese users — not a Swiss Army knife. Keep changes focused on Vietnamese support and reading UX.*
