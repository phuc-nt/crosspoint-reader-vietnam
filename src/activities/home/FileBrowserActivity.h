#pragma once

#include <functional>
#include <string>
#include <vector>

#include "../Activity.h"
#include "RecentBooksStore.h"
#include "util/ButtonNavigator.h"

enum class FileSortMode : uint8_t {
  NAME = 0,        // A→Z natural sort (default)
  NAME_DESC,       // Z→A
  SIZE_ASC,        // Smallest first
  SIZE_DESC,       // Largest first
  COUNT
};

class FileBrowserActivity final : public Activity {
 public:
  struct FileEntry {
    std::string name;
    size_t fileSize = 0;  // 0 for directories
    bool isDir = false;
  };

  explicit FileBrowserActivity(GfxRenderer& renderer, MappedInputManager& mappedInput, std::string initialPath = "/")
      : Activity("FileBrowser", renderer, mappedInput), basepath(initialPath.empty() ? "/" : std::move(initialPath)) {}
  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  // Deletion
  void clearFileMetadata(const std::string& fullPath);

  ButtonNavigator buttonNavigator;

  size_t selectorIndex = 0;

  // Sort
  static FileSortMode currentSortMode;
  void cycleSortMode();

  // Files state
  std::string basepath = "/";
  std::vector<FileEntry> entries;

  // Data loading
  void loadFiles();
  void sortEntries();
  size_t findEntry(const std::string& name) const;

  // Helpers
  const std::string& getEntryName(size_t idx) const { return entries[idx].name; }
};
