# Đợt 02 — Home Screen Progress & UI

> **Trạng thái**: 🟡 Planning
> **Ngày bắt đầu**: 08/03/2026

---

## Mục tiêu

Home screen hiện danh sách sách đang đọc + tiến trình đọc (% + progress bar), giống bản MrSlim. Bỏ Lyra theme, chỉ giữ Classic + custom UI.

---

## Tham khảo

Layout MrSlim:
- **Trái**: Cover sách đang đọc + "ĐỌC TIẾP"
- **Phải**: Panel "TIẾN TRÌNH" — 4 sách kèm tên + progress bar + %
- **Dưới**: Menu (Tủ sách, Tệp, Truyền tệp, Cài đặt)

---

## Tasks

### Data Model
- [ ] Thêm `uint8_t progressPercent` vào `RecentBook` struct
- [ ] Update serialize/deserialize trong `RecentBooksStore.cpp`
- [ ] Backward-compatible: default = 0 cho sách cũ

### Progress Tracking
- [ ] Cập nhật progress khi thoát reader (`EpubReaderActivity::onExit`)
- [ ] Gọi `RECENT_BOOKS.updateProgress(path, percent)`

### Home Screen UI (Classic Theme)
- [ ] Sửa `BaseTheme::drawRecentBookCover()`:
  - Trái: Cover sách chính + "ĐỌC TIẾP" label
  - Phải: Danh sách 4 sách recent + progress bar + %
- [ ] Tăng `homeRecentBooksCount` = 5 (1 chính + 4 phụ)
- [ ] Cập nhật navigation (selectorIndex) cho layout mới

### Bỏ Lyra Theme
- [ ] Xóa `lyra/LyraTheme.cpp`, `lyra/LyraTheme.h`
- [ ] Xóa `lyra/Lyra3CoversTheme.cpp`, `lyra/Lyra3CoversTheme.h`
- [ ] Cập nhật `UITheme.cpp` — bỏ case LYRA, LYRA_3_COVERS
- [ ] Cập nhật `CrossPointSettings.h` — bỏ LYRA enum values
- [ ] Cập nhật `SettingsList.h` — bỏ theme dropdown

### Build & Test
- [ ] Build thành công
- [ ] Flash lên device
- [ ] Test hiển thị progress per book
- [ ] Test navigation trên home screen

---

## Log

_(Chưa có — sẽ cập nhật khi bắt đầu implement)_

---

## Risks

| Risk | Mitigation |
|---|---|
| RAM khi load Epub metadata | Cache progress khi thoát reader, không load trên home |
| Backward compatibility (state files cũ) | Default progressPercent = 0 |
| Layout phức tạp trên màn hình nhỏ | Tham khảo layout MrSlim đã proven |
