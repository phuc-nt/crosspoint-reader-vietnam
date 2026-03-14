#include "FileBrowserActivity.h"

#include <Epub.h>
#include <FsHelpers.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <I18n.h>

#include <algorithm>

#include "../util/ConfirmationActivity.h"
#include "MappedInputManager.h"
#include "components/UITheme.h"
#include "FileBrowserMetaCache.h"
#include "fontIds.h"

namespace {
constexpr unsigned long GO_HOME_MS = 1000;
constexpr unsigned long SORT_TOGGLE_MS = 500;  // Long press on UP to cycle sort
}  // namespace

// Static member — sort mode persists across directories
FileSortMode FileBrowserActivity::currentSortMode = FileSortMode::NAME;

// ── Natural sort comparator (A→Z, directories first) ──
static bool naturalSortAsc(const FileBrowserActivity::FileEntry& a, const FileBrowserActivity::FileEntry& b) {
  if (a.isDir != b.isDir) return a.isDir;

  const char* s1 = a.name.c_str();
  const char* s2 = b.name.c_str();

  while (*s1 && *s2) {
    if (isdigit(*s1) && isdigit(*s2)) {
      while (*s1 == '0') s1++;
      while (*s2 == '0') s2++;
      int len1 = 0, len2 = 0;
      while (isdigit(s1[len1])) len1++;
      while (isdigit(s2[len2])) len2++;
      if (len1 != len2) return len1 < len2;
      for (int i = 0; i < len1; i++) {
        if (s1[i] != s2[i]) return s1[i] < s2[i];
      }
      s1 += len1;
      s2 += len2;
    } else {
      char c1 = tolower(*s1);
      char c2 = tolower(*s2);
      if (c1 != c2) return c1 < c2;
      s1++;
      s2++;
    }
  }
  return *s1 == '\0' && *s2 != '\0';
}

void FileBrowserActivity::sortEntries() {
  switch (currentSortMode) {
    case FileSortMode::NAME:
      std::sort(entries.begin(), entries.end(), naturalSortAsc);
      break;
    case FileSortMode::NAME_DESC:
      std::sort(entries.begin(), entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDir != b.isDir) return a.isDir;
        return naturalSortAsc(b, a);  // reverse
      });
      break;
    case FileSortMode::SIZE_ASC:
      std::sort(entries.begin(), entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDir != b.isDir) return a.isDir;
        if (a.isDir && b.isDir) return naturalSortAsc(a, b);
        return a.fileSize < b.fileSize;
      });
      break;
    case FileSortMode::SIZE_DESC:
      std::sort(entries.begin(), entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDir != b.isDir) return a.isDir;
        if (a.isDir && b.isDir) return naturalSortAsc(a, b);
        return a.fileSize > b.fileSize;
      });
      break;
    default:
      break;
  }
}

void FileBrowserActivity::cycleSortMode() {
  int mode = static_cast<int>(currentSortMode);
  mode = (mode + 1) % static_cast<int>(FileSortMode::COUNT);
  currentSortMode = static_cast<FileSortMode>(mode);
  sortEntries();
  selectorIndex = 0;
  requestUpdate();
}

void FileBrowserActivity::loadFiles() {
  entries.clear();

  auto root = Storage.open(basepath.c_str());
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    return;
  }

  root.rewindDirectory();

  char name[500];
  for (auto file = root.openNextFile(); file; file = root.openNextFile()) {
    file.getName(name, sizeof(name));
    if (name[0] == '.' || strcmp(name, "System Volume Information") == 0) {
      file.close();
      continue;
    }

    if (file.isDirectory()) {
      entries.push_back({std::string(name) + "/", 0, true});
    } else {
      std::string_view filename{name};
      if (FsHelpers::hasEpubExtension(filename) || FsHelpers::hasXtcExtension(filename) ||
          FsHelpers::hasTxtExtension(filename) || FsHelpers::hasMarkdownExtension(filename) ||
          FsHelpers::hasBmpExtension(filename)) {
        entries.push_back({std::string(filename), file.fileSize(), false});
      }
    }
    file.close();
  }
  root.close();
  sortEntries();
}

void FileBrowserActivity::onEnter() {
  Activity::onEnter();

  BROWSER_META.load();
  loadFiles();
  selectorIndex = 0;

  requestUpdate();
}

void FileBrowserActivity::onExit() {
  Activity::onExit();
  entries.clear();
}

void FileBrowserActivity::clearFileMetadata(const std::string& fullPath) {
  if (FsHelpers::hasEpubExtension(fullPath)) {
    Epub(fullPath, "/.crosspoint").clearCache();
    LOG_DBG("FileBrowser", "Cleared metadata cache for: %s", fullPath.c_str());
  }
}

void FileBrowserActivity::loop() {
  // Long press BACK (1s+) goes to root folder
  if (mappedInput.isPressed(MappedInputManager::Button::Back) && mappedInput.getHeldTime() >= GO_HOME_MS &&
      basepath != "/") {
    basepath = "/";
    loadFiles();
    selectorIndex = 0;
    return;
  }

  const int pageItems = UITheme::getInstance().getNumberOfItemsPerPage(renderer, true, false, true, false);

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    if (entries.empty()) return;

    const auto& entry = entries[selectorIndex];
    bool isDirectory = entry.isDir;

    if (mappedInput.getHeldTime() >= GO_HOME_MS && !isDirectory) {
      // --- LONG PRESS ACTION: DELETE FILE ---
      std::string cleanBasePath = basepath;
      if (cleanBasePath.back() != '/') cleanBasePath += "/";
      const std::string fullPath = cleanBasePath + entry.name;

      auto handler = [this, fullPath](const ActivityResult& res) {
        if (!res.isCancelled) {
          LOG_DBG("FileBrowser", "Attempting to delete: %s", fullPath.c_str());
          clearFileMetadata(fullPath);
          if (Storage.remove(fullPath.c_str())) {
            LOG_DBG("FileBrowser", "Deleted successfully");
            loadFiles();
            if (entries.empty()) {
              selectorIndex = 0;
            } else if (selectorIndex >= entries.size()) {
              selectorIndex = entries.size() - 1;
            }

            requestUpdate(true);
          } else {
            LOG_ERR("FileBrowser", "Failed to delete file: %s", fullPath.c_str());
          }
        } else {
          LOG_DBG("FileBrowser", "Delete cancelled by user");
        }
      };

      std::string heading = tr(STR_DELETE) + std::string("? ");

      startActivityForResult(std::make_unique<ConfirmationActivity>(renderer, mappedInput, heading, entry.name), handler);
      return;
    } else {
      // --- SHORT PRESS ACTION: OPEN/NAVIGATE ---
      if (basepath.back() != '/') basepath += "/";

      if (isDirectory) {
        // Strip trailing '/'
        const auto& dirName = entry.name;
        basepath += dirName.substr(0, dirName.length() - 1);
        loadFiles();
        selectorIndex = 0;
        requestUpdate();
      } else {
        onSelectBook(basepath + entry.name);
      }
    }
    return;
  }

  if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    if (mappedInput.getHeldTime() < GO_HOME_MS) {
      if (basepath != "/") {
        const std::string oldPath = basepath;

        basepath.replace(basepath.find_last_of('/'), std::string::npos, "");
        if (basepath.empty()) basepath = "/";
        loadFiles();

        const auto pos = oldPath.find_last_of('/');
        const std::string dirName = oldPath.substr(pos + 1) + "/";
        selectorIndex = findEntry(dirName);

        requestUpdate();
      } else {
        onGoHome();
      }
    }
  }

  int listSize = static_cast<int>(entries.size());

  // Long press UP → cycle sort mode
  buttonNavigator.onPreviousRelease([this, listSize] {
    if (mappedInput.getHeldTime() >= SORT_TOGGLE_MS) {
      cycleSortMode();
    } else {
      selectorIndex = ButtonNavigator::previousIndex(static_cast<int>(selectorIndex), listSize);
      requestUpdate();
    }
  });

  buttonNavigator.onNextRelease([this, listSize] {
    selectorIndex = ButtonNavigator::nextIndex(static_cast<int>(selectorIndex), listSize);
    requestUpdate();
  });

  buttonNavigator.onNextContinuous([this, listSize, pageItems] {
    selectorIndex = ButtonNavigator::nextPageIndex(static_cast<int>(selectorIndex), listSize, pageItems);
    requestUpdate();
  });

  buttonNavigator.onPreviousContinuous([this, listSize, pageItems] {
    selectorIndex = ButtonNavigator::previousPageIndex(static_cast<int>(selectorIndex), listSize, pageItems);
    requestUpdate();
  });
}

std::string getFileName(const std::string& filename) {
  if (filename.back() == '/') {
    return filename.substr(0, filename.length() - 1);
  }
  const auto pos = filename.rfind('.');
  return filename.substr(0, pos);
}

std::string getDisplayName(const std::string& basepath, const std::string& filename) {
  if (filename.back() == '/') {
    return filename.substr(0, filename.length() - 1);
  }

  if (FsHelpers::hasEpubExtension(filename)) {
    std::string cleanBase = basepath;
    if (cleanBase.back() != '/') cleanBase += "/";
    const std::string fullPath = cleanBase + filename;
    const auto* meta = BROWSER_META.get(fullPath);
    if (meta && !meta->title.empty()) {
      if (!meta->author.empty()) {
        return meta->title + " - " + meta->author;
      }
      return meta->title;
    }
  }

  const auto pos = filename.rfind('.');
  return filename.substr(0, pos);
}

static const char* getSortLabel(FileSortMode mode) {
  switch (mode) {
    case FileSortMode::NAME: return tr(STR_SORT_NAME_ASC);
    case FileSortMode::NAME_DESC: return tr(STR_SORT_NAME_DESC);
    case FileSortMode::SIZE_ASC: return tr(STR_SORT_SIZE_ASC);
    case FileSortMode::SIZE_DESC: return tr(STR_SORT_SIZE_DESC);
    default: return "";
  }
}

void FileBrowserActivity::render(RenderLock&&) {
  renderer.clearScreen();

  const auto pageWidth = renderer.getScreenWidth();
  const auto pageHeight = renderer.getScreenHeight();
  const auto& metrics = UITheme::getInstance().getMetrics();

  // Header: folder name + sort indicator
  std::string folderName = (basepath == "/") ? tr(STR_SD_CARD) : basepath.substr(basepath.rfind('/') + 1);
  std::string headerText = folderName + " [" + getSortLabel(currentSortMode) + "]";
  GUI.drawHeader(renderer, Rect{0, metrics.topPadding, pageWidth, metrics.headerHeight}, headerText.c_str());

  const int contentTop = metrics.topPadding + metrics.headerHeight + metrics.verticalSpacing;
  const int contentHeight = pageHeight - contentTop - metrics.buttonHintsHeight - metrics.verticalSpacing;
  if (entries.empty()) {
    renderer.drawText(UI_10_FONT_ID, metrics.contentSidePadding, contentTop + 20, tr(STR_NO_FILES_FOUND));
  } else {
    GUI.drawList(
        renderer, Rect{0, contentTop, pageWidth, contentHeight}, entries.size(), selectorIndex,
        [this](int index) { return getDisplayName(basepath, entries[index].name); }, nullptr,
        [this](int index) { return UITheme::getFileIcon(entries[index].name); });
  }

  // Button hints
  const auto labels =
      mappedInput.mapLabels(basepath == "/" ? tr(STR_HOME) : tr(STR_BACK), entries.empty() ? "" : tr(STR_OPEN),
                            entries.empty() ? "" : tr(STR_DIR_UP), entries.empty() ? "" : tr(STR_DIR_DOWN));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}

size_t FileBrowserActivity::findEntry(const std::string& name) const {
  for (size_t i = 0; i < entries.size(); i++)
    if (entries[i].name == name) return i;
  return 0;
}