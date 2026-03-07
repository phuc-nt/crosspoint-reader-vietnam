# Dev Log — CrossPoint Reader Vietnam

> Theo dõi tiến độ phát triển theo từng session/build.

---

## Session 001 — 07/03/2026 (Kickoff & Research)

**Thời gian**: 22:38 - 23:30+  
**Người thực hiện**: PM (Phúc) + AI Agent  
**Trạng thái**: ✅ Hoàn thành

### Công việc đã làm

#### Research
- [x] Tìm hiểu Xteink X4 hardware specs
- [x] Tìm hiểu CrossPoint Reader upstream (features, architecture, codebase)
- [x] Research cộng đồng Việt Nam (Facebook group "Nghiện Xteink" — 2,700+ members)
- [x] Phân tích pain points người dùng Việt
- [x] Research font tiếng Việt phù hợp E-ink (Literata, Be Vietnam Pro, Vollkorn)
- [x] Phân tích font pipeline CrossPoint (fontconvert.py đã có Vietnamese Unicode range!)
- [x] Phân tích I18n system (YAML → Python → C++)
- [x] Check partition table: 6.5MB per OTA slot — dư dả
- [x] Check features đã có sẵn vs cần implement

#### Key Findings
- Font pipeline (`fontconvert.py`) **đã có sẵn** Vietnamese Unicode ranges (U+1EA0-1EF9, U+01A0-01B0)
- Phần lớn features bạn muốn **đã có sẵn** (cover, progress bar, images in EPUB)
- Chỉ cần: (1) font files, (2) vietnamese.yaml, (3) register fonts
- Flash partition 6.5MB → thoải mái thêm fonts

#### Project Setup
- [x] Tạo `pm_docs/VISION.md`
- [x] Tạo `pm_docs/DEV_PLAN.md`
- [x] Tạo `pm_docs/DEV_LOG.md` (file này)
- [x] Tạo `GEMINI.md`
- [x] Tạo `.gemini/skills/`
- [x] Tạo `lib/I18n/translations/vietnamese.yaml`

#### Artifacts
- Research report: `xteink_vietnam_research.md` (artifact)
- Implementation plan: `implementation_plan.md` (artifact)

### Issues / Blockers
- Cần download font TTF files từ Google Fonts (Phase tiếp theo)
- Cần test build trên PlatformIO

### Decisions Made
| Quyết định | Lý do |
|---|---|
| Chọn Literata thay Bookerly | SIL OFL license rõ ràng, Vietnamese tốt hơn |
| Chọn Be Vietnam Pro thay Noto Sans | Thiết kế bởi người Việt, cho tiếng Việt |
| Chọn Vollkorn | Serif cổ điển, chữ dày rõ trên E-ink |
| Bỏ Bokerlam | License không rõ ràng |

---

## Session 002 — (Pending)

**Planned**:
- Download 3 font families
- Generate font bitmap headers
- Register fonts trong source code
- Build verification

---

*Template cho session mới:*

```markdown
## Session XXX — DD/MM/YYYY (Tiêu đề)

**Thời gian**: HH:MM - HH:MM
**Người thực hiện**: ...
**Trạng thái**: 🟡 In Progress / ✅ Hoàn thành / 🔴 Blocked

### Công việc đã làm
- [x] ...
- [ ] ...

### Issues / Blockers
- ...

### Decisions Made
| Quyết định | Lý do |
|---|---|
| ... | ... |

### Build Info
- Firmware size: ...
- Build warnings: ...
- Test results: ...
```
