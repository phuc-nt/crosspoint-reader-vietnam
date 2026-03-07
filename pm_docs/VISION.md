# CrossPoint Reader Vietnam — Vision & Context

> **PM**: Phúc Nguyễn  
> **Ngày khởi động**: 07/03/2026  
> **Trạng thái**: 🟢 Active Development

---

## Tiền đề

### Máy Xteink X4
Xteink X4 là e-reader siêu nhỏ gọn (~74g, 4.9mm) từ Trung Quốc, giá $50-70 USD, chạy ESP32-C3. Firmware gốc closed-source, chỉ hỗ trợ Trung/Anh/Nhật.

### CrossPoint Reader (upstream)
Firmware open-source (MIT License) thay thế firmware gốc. Được cộng đồng quốc tế phát triển, tập trung vào trải nghiệm đọc EPUB trên phần cứng hạn chế. Repo gốc: `crosspoint-reader/crosspoint-reader`.

### Cộng đồng Việt Nam
- **2,700+ thành viên** trong Facebook group "Nghiện Xteink - Hội người chơi máy đọc sách mini" (tạo 17/12/2025, tăng trưởng bùng nổ)
- CrossPoint được công nhận là *"firmware tốt và phổ biến nhất"*
- Đã có bản mod CrossPoint thêm font Việt (không chính thức, mất OTA)
- Combo phổ biến: **CrossPoint + font Bokerlam**
- Shopee có shop flash sẵn firmware mod khi bán máy

### Pain points hiện tại
1. Font builtin không có đủ Vietnamese glyphs → chữ Việt hiển thị lỗi (ô vuông)
2. UI chỉ có tiếng Anh/Trung
3. Bản mod không chính thức → mất OTA, không update được
4. Người dùng không biết kỹ thuật phải phụ thuộc shop flash sẵn

---

## Vision

> **Tạo bản CrossPoint Reader chính thức cho người Việt — flash xong là dùng được ngay, đọc EPUB tiếng Việt đẹp, giao diện tiếng Việt.**

### Mục tiêu cụ thể
1. **3 font tiếng Việt builtin** — Literata, Be Vietnam Pro, Vollkorn (tất cả SIL OFL)
2. **Giao diện UI tiếng Việt** — dịch toàn bộ menu/settings
3. **OTA update hoạt động** — không như bản mod hiện tại
4. **Maintain dài hạn** — sync với upstream CrossPoint, nhận feature mới

### Giá trị mang lại
- **2,700+ người dùng** có firmware chính thức, ổn định
- **Cộng đồng Việt Nam** có thể contribute ngược lại upstream
- **Giảm phụ thuộc** vào shop flash sẵn
- **Nền tảng** cho các tính năng Việt hóa sâu hơn (hyphenation, dictionary...)

---

## Phạm vi (Scope)

### In-Scope (v1.0)
- Dịch giao diện sang tiếng Việt (I18n)
- Thêm 3 font family tiếng Việt (Literata, Be Vietnam Pro, Vollkorn)
- Font UI hỗ trợ tiếng Việt
- Build & release firmware flashable
- Tài liệu hướng dẫn tiếng Việt

### Out-of-Scope (future)
- Từ điển tra cứu tiếng Việt
- Hyphenation engine tiếng Việt
- Ứng dụng companion riêng
- Cải tiến UI/UX vượt phạm vi upstream
- Hỗ trợ format mới (PDF, MOBI...)

---

## Đối thủ / Alternatives

| Firmware | Điểm mạnh | Điểm yếu |
|---|---|---|
| **Stock Xteink** | Ổn định, từ nhà sản xuất | Closed source, không hỗ trợ Việt |
| **CrossPoint gốc** | Open source, feature-rich | Không hỗ trợ tiếng Việt |
| **Bản mod MrSlim** | Có font Việt, UI Việt | Không chính thức, mất OTA |
| **Firmware 1.0.0** | UI đẹp, khen nhiều | Ít thông tin, độ tin cậy chưa rõ |
| **→ CrossPoint VN (chúng ta)** | Font Việt + UI Việt + OTA + Open source | Fork cần maintain |

---

## Tech Stack & Constraints
- **MCU**: ESP32-C3 (RISC-V, ~380KB RAM, 16MB flash)
- **Build**: PlatformIO + C++20
- **Flash partition**: 6.5MB per OTA slot (dư dả cho font)
- **Font system**: Bitmap glyphs compiled vào firmware, DEFLATE compressed
- **I18n**: YAML → Python script → C++ generated code
