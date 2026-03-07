---
name: build-and-release
description: How to build firmware, verify size constraints, and create a release
---

# Build & Release Workflow

## When to Use
Use this workflow when preparing a firmware build for testing or release.

## Build Steps

1. **Check Dev Plan** for current status:
   ```bash
   cat pm_docs/DEV_PLAN.md | head -50
   ```

2. **Regenerate I18n** (if translations changed):
   // turbo
   ```bash
   python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/
   ```

3. **Clean build**:
   ```bash
   pio run -t clean
   ```

4. **Build firmware**:
   ```bash
   pio run 2>&1 | tee /tmp/build_output.txt
   ```

5. **Check firmware size** (must be < 6.5MB = 6,684,672 bytes):
   // turbo
   ```bash
   # Check binary size
   ls -la .pio/build/default/firmware.bin 2>/dev/null || echo "Build not found"
   
   # Check from build output
   grep -i "flash\|size\|memory" /tmp/build_output.txt
   ```

6. **Verify no warnings**:
   // turbo
   ```bash
   grep -i "warning" /tmp/build_output.txt | grep -v "deprecat" | head -20
   ```

7. **Update Dev Log** with build results:
   - Firmware size
   - Build warnings (if any)
   - Test results

## Release Build (Production)

```bash
# Production build (no serial logging)
pio run -e gh_release

# Release candidate (minimal logging)
pio run -e gh_release_rc
```

## Flash to Device

```bash
# USB flash
pio run -t upload

# Monitor serial output
python3 scripts/debugging_monitor.py
```

## Checklist Before Release
- [ ] All translations complete in vietnamese.yaml
- [ ] All 3 Vietnamese fonts working
- [ ] Build succeeds with 0 errors
- [ ] Firmware size < 6.5MB
- [ ] Device testing complete
- [ ] Dev Log updated
- [ ] README updated
- [ ] GitHub Release created with firmware.bin
