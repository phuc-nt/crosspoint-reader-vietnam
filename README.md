# CrossPoint Reader Vietnam 🇻🇳

**Firmware tiếng Việt cho máy đọc sách Xteink X4** — fork từ [crosspoint-reader](https://github.com/crosspoint-reader/crosspoint-reader).

> Bản customize cho người dùng Việt Nam: giao diện tiếng Việt, font hỗ trợ đầy đủ dấu, và nhiều tính năng UX bổ sung.

## Tính năng mới (so với upstream)

### 🇻🇳 Vietnamese Support
- **Giao diện hoàn toàn tiếng Việt** — tất cả menu, thông báo, nút bấm
- **2 font tối ưu cho tiếng Việt**: Be Vietnam Pro (sans-serif, mặc định) + Literata (serif)
- Hỗ trợ đầy đủ Unicode tiếng Việt (Ơ/ơ, Ư/ư, tất cả dấu tổ hợp)

### 📖 Trải nghiệm đọc
- **Loading indicator** — hiện "Đang tải..." khi mở sách lớn
- **Bookmarks** — đánh dấu trang + giao diện quản lý bookmark đầy đủ
- **Reading Stats** — theo dõi thời gian đọc + số trang đã đọc, hiện trên Home
- **Sleep screen** — hiện bìa sách đang đọc khi ngủ

### 📂 File Browser
- **Hiện tên sách + tác giả** thay vì filename (metadata cache)
- **Sắp xếp** — 4 chế độ: A→Z, Z→A, Nhỏ→Lớn, Lớn→Nhỏ (giữ Back ở thư mục gốc)
- **Book Info** — giữ nút Confirm trên epub → xem thông tin chi tiết

### ⚡ Performance
- **Home load nhanh hơn** — bỏ kiểm tra file tồn tại cho danh sách gần đây

## Cài đặt

### Từ source (dành cho developer)

```bash
# Clone repo
git clone --recursive https://github.com/phuc-nt/crosspoint-reader-vietnam
cd crosspoint-reader-vietnam

# Build
pio run

# Flash (cắm USB-C vào Xteink X4)
pio run --target upload
```

**Yêu cầu**: PlatformIO Core, Python 3.8+, USB-C cable, Xteink X4

### Cập nhật OTA

Chưa hỗ trợ OTA riêng cho bản Vietnam. Flash qua USB.

## Sử dụng

| Thao tác | Chức năng |
|----------|-----------|
| Bấm nhanh Confirm | Mở file/thư mục |
| Giữ Confirm (epub) | Xem thông tin sách |
| Giữ Confirm (non-epub) | Xóa file |
| Giữ Back (ở thư mục gốc) | Đổi kiểu sắp xếp |
| Giữ Back (trong thư mục) | Về thư mục gốc |
| Up/Down | Di chuyển lên/xuống |

Xem [USER_GUIDE.md](./USER_GUIDE.md) của upstream cho hướng dẫn chi tiết.

## Dữ liệu trên SD Card

```
.crosspoint/
├── epub_<hash>/        # Cache cho mỗi epub
│   ├── progress.bin    # Vị trí đọc
│   ├── book.bin        # Metadata
│   └── sections/       # Cache chapter
├── bookmarks/          # Bookmark cho từng sách
├── reading_stats.json  # Thống kê đọc
└── browser_meta.json   # Cache metadata file browser
```

## Upstream

Fork từ [crosspoint-reader/crosspoint-reader](https://github.com/crosspoint-reader/crosspoint-reader).
Xem repo gốc cho: kiến trúc chi tiết, contributing guide, governance, scope.

## License

Giữ nguyên license từ upstream. Font: SIL Open Font License.

---

*CrossPoint Reader không liên kết với Xteink hay bất kỳ nhà sản xuất phần cứng X4 nào.*
