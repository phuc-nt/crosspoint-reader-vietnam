# Dev Plan — CrossPoint Reader Vietnam

> **PM**: Phúc Nguyễn | **Lead Dev**: AI Agent  
> **Khởi động**: 07/03/2026  
> **Target Release**: v1.0.0-vn

---

## Milestone Overview

```
Phase 1: Foundation (Vietnamese Language + Fonts)     ← HIỆN TẠI
Phase 2: Build & Verify
Phase 3: Polish & Testing
Phase 4: Release & Community
```

---

## Phase 1: Foundation 🔨

### Task 1.1: Project Setup
- [x] Fork CrossPoint Reader upstream
- [x] Tạo pm_docs/ (vision, dev plan, dev log)
- [x] Tạo GEMINI.md cho AI agents
- [x] Tạo skill files cho workflows
- [ ] Setup CI/CD cho fork

### Task 1.2: Vietnamese UI Translation
- [x] Tạo `lib/I18n/translations/vietnamese.yaml`
- [x] Chạy `gen_i18n.py` để generate code
- [ ] Verify build thành công

### Task 1.3: Vietnamese Fonts
- [x] Download Literata (Regular, Italic, Bold, BoldItalic)
- [x] Download Be Vietnam Pro (Regular, Italic, Bold, BoldItalic)
- [x] Download Vollkorn (Regular, Italic, Bold, BoldItalic)
- [x] Đặt vào `lib/EpdFont/builtinFonts/source/`
- [x] Cập nhật `convert-builtin-fonts.sh`
- [ ] Generate font bitmap headers
- [ ] Update `all.h` includes
- [x] Update `fontIds.h`
- [x] Register fonts trong `main.cpp`
- [x] Update settings UI để hiển thị font mới

### Task 1.4: UI Font Vietnamese Support
- [ ] Verify Ubuntu font có Vietnamese glyphs
- [ ] Hoặc thay bằng Be Vietnam Pro cho UI font

---

## Phase 2: Build & Verify 🔧

### Task 2.1: Build Verification
- [ ] `pio run` thành công
- [ ] Check firmware size (phải < 6.5MB)
- [ ] Check build warnings
- [ ] Check I18n generation output

### Task 2.2: Optimization (nếu cần)
- [ ] Giảm font sizes nếu firmware quá lớn
- [ ] Loại bỏ font không cần (OpenDyslexic?)
- [ ] Tối ưu compression

---

## Phase 3: Polish & Testing 🧪

### Task 3.1: Device Testing
- [ ] Flash firmware lên Xteink X4
- [ ] Test hiển thị EPUB tiếng Việt
- [ ] Test switching giữa 3 fonts
- [ ] Test UI tiếng Việt (menu, settings, button labels)
- [ ] Test ảnh trong EPUB
- [ ] Test cover hiển thị trên home screen
- [ ] Test reading progress bar
- [ ] Test OTA update

### Task 3.2: Bug Fixes
- [ ] Fix rendering issues (nếu có)
- [ ] Fix line breaking với tiếng Việt (nếu có)
- [ ] Fix spacing/kerning (nếu có)

---

## Phase 4: Release & Community 🚀

### Task 4.1: Documentation
- [ ] README tiếng Việt
- [ ] Hướng dẫn flash
- [ ] Screenshots/photos trên device

### Task 4.2: Release
- [ ] GitHub Release v1.0.0-vn
- [ ] Build firmware.bin
- [ ] Web flasher (nếu được)

### Task 4.3: Community
- [ ] Post lên group "Nghiện Xteink"
- [ ] Hướng dẫn cài đặt cho người dùng

---

## Font Selection Rationale

| Font | Loại | Lý do chọn | License |
|---|---|---|---|
| **Literata** | Serif | Font đọc sách Google Play Books, Vietnamese diacritics rất rõ | SIL OFL |
| **Be Vietnam Pro** | Sans-serif | Thiết kế BỞI người Việt, CHO tiếng Việt | SIL OFL |
| **Vollkorn** | Serif Classic | Chữ dày dặn, phù hợp E-ink độ phân giải thấp | SIL OFL |

### Fonts bị loại
| Font | Lý do |
|---|---|
| ~~Bokerlam~~ | Không tìm được license rõ ràng, nguồn gốc không rõ |
| ~~Bookerly~~ | License Amazon, không rõ ràng cho nhúng firmware |
| ~~OpenDyslexic~~ | Không có Vietnamese glyphs, ít người dùng |
