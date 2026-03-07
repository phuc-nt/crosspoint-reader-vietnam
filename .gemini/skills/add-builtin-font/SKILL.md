---
name: add-builtin-font
description: How to add a new built-in font to the CrossPoint firmware (download, convert, register)
---

# Adding a New Built-in Font

## When to Use
Use this workflow when adding a new font family (TTF/OTF) to the firmware as a built-in option.

## Prerequisites
- Font files must be **SIL OFL** or compatible open-source license
- Font must support Vietnamese Unicode ranges (U+1EA0-U+1EF9)
- Python environment with `freetype-py` and `fonttools` installed

## Steps

1. **Create source directory** for the new font:
   ```bash
   mkdir -p lib/EpdFont/builtinFonts/source/<FontName>/
   ```

2. **Place TTF/OTF files** (4 styles required):
   ```
   <FontName>-Regular.ttf
   <FontName>-Italic.ttf
   <FontName>-Bold.ttf
   <FontName>-BoldItalic.ttf
   ```

3. **Update `convert-builtin-fonts.sh`** to include the new font:
   ```bash
   # Add font size array
   NEWFONT_FONT_SIZES=(12 14 16 18)
   
   # Add generation loop
   for size in ${NEWFONT_FONT_SIZES[@]}; do
     for style in ${READER_FONT_STYLES[@]}; do
       font_name="newfont_${size}_$(echo $style | tr '[:upper:]' '[:lower:]')"
       font_path="../builtinFonts/source/<FontName>/<FontName>-${style}.ttf"
       output_path="../builtinFonts/${font_name}.h"
       python fontconvert.py $font_name $size $font_path --2bit --compress > $output_path
       echo "Generated $output_path"
     done
   done
   ```

4. **Run font conversion**:
   ```bash
   cd lib/EpdFont/scripts && bash convert-builtin-fonts.sh
   ```
   ⚠️ This takes several minutes. Each font generates ~300-500KB headers.

5. **Update `lib/EpdFont/builtinFonts/all.h`** to include new headers:
   ```cpp
   #include "newfont_12_regular.h"
   // ... all size/style combinations
   ```

6. **Run font ID builder**:
   // turbo
   ```bash
   cd lib/EpdFont/scripts && bash build-font-ids.sh
   ```

7. **Update `src/fontIds.h`** with new font ID constants:
   ```cpp
   constexpr int FONT_NEWFONT_12 = <next_available_id>;
   // ... etc
   ```

8. **Register fonts in `src/main.cpp`** inside `#ifndef OMIT_FONTS`:
   ```cpp
   EpdFont newfont12Regular(newfont_12_regular...);
   EpdFontFamily newfont12Family(newfont12Regular, newfont12Bold, newfont12Italic, newfont12BoldItalic);
   ```

9. **Add font option to settings** (CrossPointSettings.h/cpp, SettingsList)

10. **Build and verify**:
    // turbo
    ```bash
    pio run 2>&1 | tail -5
    ```

## Size Estimation
- Each font style at one size ≈ 30-50KB compressed header
- 4 styles × 4 sizes = 16 files ≈ 500-800KB per font family
- 6.5MB OTA partition → can fit ~8 full font families

## Troubleshooting
- **Missing glyphs**: Check if source TTF has the Unicode ranges
- **Build too large**: Reduce sizes (keep only 14+16), or remove unused fonts
- **Python errors**: Install deps: `pip install freetype-py fonttools`
