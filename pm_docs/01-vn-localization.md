# Đợt 01 — Vietnamese Localization & Fonts

> **Trạng thái**: ✅ Done
> **Ngày**: 07-08/03/2026

---

## Mục tiêu

Flash xong là có giao diện tiếng Việt + 3 font đọc sách tiếng Việt đẹp.

---

## Tasks

### Fonts
- [x] Download Literata (4 styles) — SIL OFL, Google Fonts
- [x] Download Be Vietnam Pro (4 styles) — SIL OFL, thiết kế cho tiếng Việt
- [x] Download Vollkorn (4 styles) — SIL OFL, serif cổ điển
- [x] Generate bitmap headers (sizes 12, 14, 16, 18 × 4 styles × 3 fonts = 48 files)
- [x] Register fonts trong `main.cpp`
- [x] Update `fontIds.h`, `all.h`, `build-font-ids.sh`
- [x] Loại bỏ Bookerly, Noto Sans, OpenDyslexic (tiết kiệm ~3.5 MB)
- [x] Set Literata làm font mặc định

### UI Font Fix
- [x] Phát hiện Ubuntu UI font không có Vietnamese glyphs → UI lỗi
- [x] Thay Ubuntu UI bằng Be Vietnam Pro UI (sizes 8, 10, 12)
- [x] Thay NotoSans 8 smallFont bằng Be Vietnam Pro 8

### Localization
- [x] Tạo `vietnamese.yaml` (342 string keys)
- [x] Set default language = Vietnamese

### Defaults
- [x] Bật book progress bar mặc định
- [x] Bật LYRA_3_COVERS theme (hiện cover sách)

### Docs & Tooling
- [x] Tạo `GEMINI.md`
- [x] Tạo `.gemini/skills/` (3 skill files)
- [x] Tạo `pm_docs/` (VISION, plan, log)

---

## Log

### 07/03 22:38 — Kickoff & Research
- Research Xteink X4 specs, CrossPoint architecture, VN community
- Phát hiện `fontconvert.py` đã có Vietnamese Unicode ranges
- Phần lớn features (cover, progress, images) đã có sẵn trong upstream

### 07/03 23:00 — Font Integration
- Download 3 font families từ Google Fonts
- Generate 48 bitmap headers cho reader fonts
- Loại bỏ 3 font cũ (Bookerly, Noto Sans, OpenDyslexic)
- Build OK: Flash 87.6%, RAM 29.0%

### 07/03 23:45 — UI Font Fix
- Flash lên máy → phát hiện UI tiếng Việt bị lỗi (Ubuntu font thiếu VN glyphs)
- Thay Ubuntu UI → Be Vietnam Pro UI (sizes 8, 10, 12)
- Set default language = Vietnamese
- Build OK: Flash 85.5%, RAM 29.0% (nhẹ hơn!)

### 08/03 00:10 — Defaults & Push
- Bật progress bar + cover theme mặc định
- Flash thành công, test OK
- Git push 3 commits lên `master`

---

## Build Info

| Metric | Bắt đầu | Kết thúc |
|--------|---------|---------|
| Flash | N/A (first build) | 85.5% (5.6 MB / 6.5 MB) |
| RAM | — | 29.0% (95 KB / 320 KB) |
| Headroom | — | ~950 KB |

## Quyết định

| Quyết định | Lý do |
|---|---|
| Literata thay Bookerly | SIL OFL license, Vietnamese tốt hơn |
| Be Vietnam Pro thay Noto Sans | Thiết kế bởi người Việt, cho tiếng Việt |
| Be Vietnam Pro cho UI font | Thay Ubuntu (thiếu VN glyphs) |
| Bỏ Bokerlam | License không rõ ràng, chỉ có 1 style |
