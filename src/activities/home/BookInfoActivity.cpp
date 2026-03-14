#include "BookInfoActivity.h"

#include <Epub.h>
#include <FsHelpers.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <I18n.h>

#include "MappedInputManager.h"
#include "components/UITheme.h"
#include "FileBrowserMetaCache.h"
#include "fontIds.h"

namespace {
std::string formatFileSize(size_t bytes) {
  if (bytes < 1024) return std::to_string(bytes) + " B";
  if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
  // Show one decimal for MB
  int mb10 = static_cast<int>(bytes * 10 / (1024 * 1024));
  return std::to_string(mb10 / 10) + "." + std::to_string(mb10 % 10) + " MB";
}
}  // namespace

void BookInfoActivity::onEnter() {
  Activity::onEnter();

  // Get file size
  auto file = Storage.open(filePath.c_str());
  if (file) {
    fileSizeStr = formatFileSize(file.fileSize());
    file.close();
  }

  // Try metadata cache first
  const auto* meta = BROWSER_META.get(filePath);
  if (meta) {
    title = meta->title;
    author = meta->author;
  }

  // For epub, load full metadata if needed
  if (FsHelpers::hasEpubExtension(filePath)) {
    isEpub = true;
    Epub epub(filePath, "/.crosspoint");
    if (epub.load(false, true)) {  // skipLoadingCss = true
      if (title.empty()) title = epub.getTitle();
      if (author.empty()) author = epub.getAuthor();
      language = epub.getLanguage();
      chapterCount = epub.getTocItemsCount();

      // Cache for future use
      if (!BROWSER_META.has(filePath)) {
        BROWSER_META.set(filePath, title, author);
        BROWSER_META.save();
      }
    }
  } else {
    // Non-epub: use filename as title
    if (title.empty()) {
      auto lastSlash = filePath.rfind('/');
      auto lastDot = filePath.rfind('.');
      if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
        title = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
      } else if (lastSlash != std::string::npos) {
        title = filePath.substr(lastSlash + 1);
      }
    }
  }

  requestUpdate();
}

void BookInfoActivity::loop() {
  // Any button press → go back
  if (mappedInput.wasReleased(MappedInputManager::Button::Back) ||
      mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    finish();
  }
}

void BookInfoActivity::render(RenderLock&&) {
  renderer.clearScreen();

  const auto pageWidth = renderer.getScreenWidth();
  const auto pageHeight = renderer.getScreenHeight();
  const auto& metrics = UITheme::getInstance().getMetrics();

  // Header
  GUI.drawHeader(renderer, Rect{0, metrics.topPadding, pageWidth, metrics.headerHeight}, tr(STR_BOOK_INFO));

  int y = metrics.topPadding + metrics.headerHeight + metrics.verticalSpacing + 5;
  const int x = metrics.contentSidePadding;
  const int lineHeight = 28;

  // Title
  if (!title.empty()) {
    std::string line = std::string(tr(STR_TITLE_LABEL)) + title;
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
    y += lineHeight;
  }

  // Author
  if (!author.empty()) {
    std::string line = std::string(tr(STR_AUTHOR_LABEL)) + author;
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
    y += lineHeight;
  }

  // Language (epub only)
  if (isEpub && !language.empty()) {
    std::string line = std::string(tr(STR_LANGUAGE_LABEL)) + language;
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
    y += lineHeight;
  }

  // Chapter count (epub only)
  if (isEpub && chapterCount > 0) {
    std::string line = std::string(tr(STR_CHAPTERS_LABEL)) + std::to_string(chapterCount);
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
    y += lineHeight;
  }

  // File size
  if (!fileSizeStr.empty()) {
    std::string line = std::string(tr(STR_FILE_SIZE_LABEL)) + fileSizeStr;
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
    y += lineHeight;
  }

  // File path (may wrap, so use smaller area)
  {
    // Show just filename
    auto lastSlash = filePath.rfind('/');
    std::string filename = (lastSlash != std::string::npos) ? filePath.substr(lastSlash + 1) : filePath;
    std::string line = std::string(tr(STR_FILE_PATH_LABEL)) + filename;
    renderer.drawText(UI_10_FONT_ID, x, y, line.c_str());
  }

  // Button hints
  const auto labels = mappedInput.mapLabels(tr(STR_BACK), "", "", "");
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}
