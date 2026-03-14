#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * Stores bookmarks for a single book.
 * Bookmarks are saved as JSON to SD card at /.crosspoint/bookmarks/<md5>.json
 */
class BookmarkStore {
 public:
  struct Bookmark {
    int spineIndex = 0;         // Chapter index in EPUB spine
    int pageInChapter = 0;      // Page within the chapter
    float bookProgress = 0.0f;  // Overall book progress (0.0-1.0)
    std::string chapterTitle;   // Chapter title at bookmark
    uint32_t timestamp = 0;     // Unix timestamp when created
  };

  /// Load bookmarks for a specific book path, returns true on success
  bool loadForBook(const std::string& bookPath);

  /// Save bookmarks to SD card
  bool save() const;

  /// Add a new bookmark at current position
  void addBookmark(int spineIndex, int pageInChapter, float bookProgress, const std::string& chapterTitle);

  /// Remove bookmark at given index
  void removeBookmark(int index);

  /// Check if a bookmark exists at the given position
  bool hasBookmarkAt(int spineIndex, int pageInChapter) const;

  /// Get all bookmarks (sorted by book progress)
  const std::vector<Bookmark>& getBookmarks() const { return bookmarks; }

  /// Get number of bookmarks
  int count() const { return static_cast<int>(bookmarks.size()); }

  /// Clear all bookmarks for current book
  void clear();

 private:
  std::string filePath;  // Path to JSON file on SD
  std::vector<Bookmark> bookmarks;
};
