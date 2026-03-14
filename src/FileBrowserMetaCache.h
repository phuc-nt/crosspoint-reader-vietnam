#pragma once

#include <string>
#include <unordered_map>

/**
 * Lightweight cache for file browser metadata (title, author).
 * Stores a single JSON file at /.crosspoint/browser_meta.json
 * Maps file paths to their metadata.
 * 
 * Only caches entries as they are discovered (lazy loading).
 */
class FileBrowserMetaCache {
 public:
  struct BookMeta {
    std::string title;
    std::string author;
  };

  static FileBrowserMetaCache& getInstance() { return instance; }

  /// Load cache from SD
  bool load();
  
  /// Save cache to SD
  bool save() const;

  /// Get metadata for a path, or nullptr if not cached
  const BookMeta* get(const std::string& path) const;

  /// Set metadata for a path
  void set(const std::string& path, const std::string& title, const std::string& author);

  /// Check if metadata exists for a path
  bool has(const std::string& path) const;

 private:
  static FileBrowserMetaCache instance;
  std::unordered_map<std::string, BookMeta> cache;
  bool dirty = false;

  static constexpr const char* CACHE_FILE = "/.crosspoint/browser_meta.json";
};

#define BROWSER_META FileBrowserMetaCache::getInstance()
