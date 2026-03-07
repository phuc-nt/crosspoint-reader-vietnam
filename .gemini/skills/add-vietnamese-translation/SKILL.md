---
name: add-vietnamese-translation
description: How to add or update Vietnamese translation strings in the I18n system
---

# Adding/Updating Vietnamese Translations

## When to Use
Use this workflow when you need to add new translatable strings or update existing Vietnamese translations.

## Steps

1. **Check English reference file** for all available string keys:
   ```bash
   cat lib/I18n/translations/english.yaml
   ```

2. **Edit the Vietnamese translation file**:
   ```bash
   # Edit vietnamese.yaml
   # Format: STR_KEY: "Translated text"
   # Missing keys will fall back to English automatically
   ```
   File: `lib/I18n/translations/vietnamese.yaml`

3. **Run the I18n generator** to produce C++ code:
   // turbo
   ```bash
   python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/
   ```

4. **Verify no generation errors** in output

5. **Build to verify** compilation:
   // turbo
   ```bash
   pio run 2>&1 | tail -20
   ```

6. **Update Dev Log** if significant changes:
   - Note which strings were added/changed in `pm_docs/DEV_LOG.md`

## Rules
- Always use UTF-8 encoding
- Keep translations concise (E-ink space constraints, 480px wide screen)
- String values must be quoted: `STR_KEY: "value"`
- Use `\\n` for newlines inside strings
- Run generator after EVERY edit to vietnamese.yaml
