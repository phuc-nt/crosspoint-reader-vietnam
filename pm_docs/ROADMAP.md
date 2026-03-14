# CrossPoint Reader Vietnam — Roadmap

> **PM**: Phúc Nguyễn | **Lead Dev**: AI Agent
> **Repo**: `phuc-nt/crosspoint-reader-vietnam`

---

## Tổng quan các đợt

| Đợt | Tên | Trạng thái | Ngày |
|-----|-----|-----------|------|
| 01 | Vietnamese Localization & Fonts | ✅ Done | 07-08/03/2026 |
| 02 | Home Screen Progress & UI | ✅ Done | 08/03/2026 |
| 03 | Quick Wins + Loading Indicator | ✅ Done | 14/03/2026 |
| 04 | Core Features (Bookmarks, Stats, Metadata) | ✅ Done | 14/03/2026 |
| 05 | Polish + Remaining | ⬜ Backlog | — |
| 06 | Network + Release | ⬜ Backlog | — |

---

## Quy ước

### Đặt tên file
```
pm_docs/
├── VISION.md           # Tiền đề, mục tiêu, scope (ít thay đổi)
├── ROADMAP.md          # File này — tổng quan các đợt
├── 01-vn-localization.md  # Đợt 1: plan + log
├── 02-home-progress.md    # Đợt 2: plan + log
├── 03-release.md          # Đợt 3: plan + log
└── ...
```

### Cấu trúc mỗi file đợt
```markdown
# Đợt XX — Tên ngắn gọn

## Mục tiêu
## Tasks (checklist)
## Log (entries theo ngày)
## Build info
```

### Nguyên tắc
- **1 file = 1 đợt** — chứa cả plan + log, không tách rời
- **Đánh số** 01, 02, 03... theo thứ tự thời gian
- **Tên file** ngắn, dùng kebab-case, mô tả nội dung chính
- **Tasks** dùng checkbox `- [x]` / `- [ ]` để tracking
- **Log** ghi ngắn gọn kết quả, quyết định, issues
