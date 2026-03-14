#include "BookmarkListActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "MappedInputManager.h"
#include "components/UITheme.h"
#include "fontIds.h"

BookmarkListActivity::BookmarkListActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                           BookmarkStore& bookmarkStore)
    : Activity("BookmarkList", renderer, mappedInput), bookmarkStore(bookmarkStore) {}

void BookmarkListActivity::onEnter() {
  Activity::onEnter();
  requestUpdate();
}

void BookmarkListActivity::onExit() { Activity::onExit(); }

void BookmarkListActivity::loop() {
  const int count = bookmarkStore.count();

  if (confirmDelete) {
    // In delete confirmation mode
    if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
      bookmarkStore.removeBookmark(selectedIndex);
      bookmarkStore.save();
      confirmDelete = false;
      if (selectedIndex >= bookmarkStore.count() && selectedIndex > 0) {
        selectedIndex--;
      }
      requestUpdate();
      return;
    }
    if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
      confirmDelete = false;
      requestUpdate();
      return;
    }
    return;
  }

  if (count == 0) {
    // No bookmarks — any button goes back
    if (mappedInput.wasReleased(MappedInputManager::Button::Back) ||
        mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
      finish();
      return;
    }
    return;
  }

  // Navigation
  buttonNavigator.onNext([this, count] {
    selectedIndex = ButtonNavigator::nextIndex(selectedIndex, count);
    requestUpdate();
  });

  buttonNavigator.onPrevious([this, count] {
    selectedIndex = ButtonNavigator::previousIndex(selectedIndex, count);
    requestUpdate();
  });

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    // Select bookmark — return its position to navigate there
    const auto& bm = bookmarkStore.getBookmarks()[selectedIndex];
    setResult(MenuResult{selectedIndex, static_cast<uint8_t>(bm.spineIndex), bm.pageInChapter});
    finish();
    return;
  }

  if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    // Long press or second press: toggle delete mode
    ActivityResult result;
    result.isCancelled = true;
    setResult(std::move(result));
    finish();
    return;
  }
}

void BookmarkListActivity::render(RenderLock&&) {
  renderer.clearScreen();
  const auto pageWidth = renderer.getScreenWidth();

  // Title
  renderer.drawCenteredText(UI_12_FONT_ID, 15, tr(STR_BOOKMARKS), true, EpdFontFamily::BOLD);

  const int count = bookmarkStore.count();

  if (count == 0) {
    renderer.drawCenteredText(UI_10_FONT_ID, 80, tr(STR_NO_BOOKMARKS));
    const auto labels = mappedInput.mapLabels(tr(STR_BACK), "", "", "");
    GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
    renderer.displayBuffer();
    return;
  }

  if (confirmDelete) {
    // Show delete confirmation
    GUI.drawPopup(renderer, tr(STR_REMOVE_BOOKMARK));
    const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), "", "");
    GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
    renderer.displayBuffer();
    return;
  }

  // Bookmark list
  constexpr int startY = 50;
  constexpr int lineHeight = 35;
  const int maxVisible = (renderer.getScreenHeight() - startY - 40) / lineHeight;

  // Calculate scroll window
  int scrollStart = 0;
  if (selectedIndex >= maxVisible) {
    scrollStart = selectedIndex - maxVisible + 1;
  }

  for (int i = 0; i < maxVisible && (scrollStart + i) < count; i++) {
    const int idx = scrollStart + i;
    const auto& bm = bookmarkStore.getBookmarks()[idx];
    const int displayY = startY + (i * lineHeight);
    const bool isSelected = (idx == selectedIndex);

    if (isSelected) {
      renderer.fillRect(0, displayY, pageWidth - 1, lineHeight, true);
    }

    // Bookmark info: progress% + chapter
    char label[64];
    snprintf(label, sizeof(label), "%d%% — Ch.%d P.%d",
             static_cast<int>(bm.bookProgress * 100.0f),
             bm.spineIndex + 1,
             bm.pageInChapter + 1);
    renderer.drawText(UI_10_FONT_ID, 20, displayY + 2, label, !isSelected);
  }

  // Scroll indicator
  if (count > maxVisible) {
    char scrollInfo[16];
    snprintf(scrollInfo, sizeof(scrollInfo), "%d/%d", selectedIndex + 1, count);
    const auto w = renderer.getTextWidth(UI_10_FONT_ID, scrollInfo);
    renderer.drawText(UI_10_FONT_ID, pageWidth - w - 10,
                      renderer.getScreenHeight() - 35, scrollInfo);
  }

  // Footer
  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_DIR_UP), tr(STR_DIR_DOWN));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}
