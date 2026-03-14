#include "BookmarkStore.h"

#include <HalStorage.h>
#include <Logging.h>

#include <algorithm>

#include "FsHelpers.h"

// Simple JSON serialization to avoid pulling in a full JSON library
// Format: {"bookmarks":[{"s":0,"p":1,"bp":0.15,"ch":"Chapter 1","ts":1234567890},...]}

static std::string escapeJson(const std::string& str) {
  std::string escaped;
  escaped.reserve(str.size());
  for (char c : str) {
    if (c == '"')
      escaped += "\\\"";
    else if (c == '\\')
      escaped += "\\\\";
    else
      escaped += c;
  }
  return escaped;
}

bool BookmarkStore::loadForBook(const std::string& bookPath) {
  bookmarks.clear();

  // Generate file path from book path hash
  // Use a simple hash of the book path for the filename
  uint32_t hash = 0;
  for (char c : bookPath) {
    hash = hash * 31 + static_cast<uint8_t>(c);
  }
  char hashStr[12];
  snprintf(hashStr, sizeof(hashStr), "%08x", hash);
  filePath = std::string("/.crosspoint/bookmarks/") + hashStr + ".json";

  if (!Storage.exists(filePath.c_str())) {
    return true;  // No bookmarks yet, that's OK
  }

  String json = Storage.readFile(filePath.c_str());
  if (json.isEmpty()) {
    return true;
  }

  // Simple JSON parser for our known format
  const char* p = json.c_str();

  // Find "bookmarks":[
  p = strstr(p, "\"bookmarks\":[");
  if (!p) return true;
  p = strchr(p, '[') + 1;

  while (*p) {
    // Skip whitespace
    while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t' || *p == ',') p++;
    if (*p == ']') break;
    if (*p != '{') break;
    p++;  // past {

    Bookmark bm;
    // Parse fields
    while (*p && *p != '}') {
      while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t' || *p == ',') p++;
      if (*p == '}') break;
      if (*p != '"') { p++; continue; }
      p++;  // past opening "

      // Read key
      const char* keyStart = p;
      while (*p && *p != '"') p++;
      std::string key(keyStart, p - keyStart);
      p++;  // past closing "
      while (*p == ':' || *p == ' ') p++;

      if (key == "s") {
        bm.spineIndex = atoi(p);
        while (*p && *p != ',' && *p != '}') p++;
      } else if (key == "p") {
        bm.pageInChapter = atoi(p);
        while (*p && *p != ',' && *p != '}') p++;
      } else if (key == "bp") {
        bm.bookProgress = atof(p);
        while (*p && *p != ',' && *p != '}') p++;
      } else if (key == "ts") {
        bm.timestamp = strtoul(p, nullptr, 10);
        while (*p && *p != ',' && *p != '}') p++;
      } else if (key == "ch") {
        if (*p == '"') {
          p++;
          std::string val;
          while (*p && *p != '"') {
            if (*p == '\\' && *(p + 1) == '"') {
              val += '"';
              p += 2;
            } else if (*p == '\\' && *(p + 1) == '\\') {
              val += '\\';
              p += 2;
            } else {
              val += *p++;
            }
          }
          if (*p == '"') p++;
          bm.chapterTitle = val;
        }
      }
    }
    if (*p == '}') p++;
    bookmarks.push_back(bm);
  }

  LOG_DBG("BMS", "Loaded %d bookmarks from %s", static_cast<int>(bookmarks.size()), filePath.c_str());
  return true;
}

bool BookmarkStore::save() const {
  if (filePath.empty()) return false;

  Storage.mkdir("/.crosspoint");
  Storage.mkdir("/.crosspoint/bookmarks");

  std::string json = "{\"bookmarks\":[";
  for (size_t i = 0; i < bookmarks.size(); i++) {
    const auto& bm = bookmarks[i];
    if (i > 0) json += ",";
    char buf[128];
    snprintf(buf, sizeof(buf), "{\"s\":%d,\"p\":%d,\"bp\":%.4f,\"ts\":%u,\"ch\":\"",
             bm.spineIndex, bm.pageInChapter, bm.bookProgress, bm.timestamp);
    json += buf;
    json += escapeJson(bm.chapterTitle);
    json += "\"}";
  }
  json += "]}";

  return Storage.writeFile(filePath.c_str(), json.c_str());
}

void BookmarkStore::addBookmark(int spineIndex, int pageInChapter, float bookProgress,
                                const std::string& chapterTitle) {
  // Don't add duplicate
  if (hasBookmarkAt(spineIndex, pageInChapter)) return;

  Bookmark bm;
  bm.spineIndex = spineIndex;
  bm.pageInChapter = pageInChapter;
  bm.bookProgress = bookProgress;
  bm.chapterTitle = chapterTitle;
  bm.timestamp = static_cast<uint32_t>(millis() / 1000);  // Approximate, no RTC
  bookmarks.push_back(bm);

  // Sort by book progress
  std::sort(bookmarks.begin(), bookmarks.end(),
            [](const Bookmark& a, const Bookmark& b) { return a.bookProgress < b.bookProgress; });
}

void BookmarkStore::removeBookmark(int index) {
  if (index >= 0 && index < static_cast<int>(bookmarks.size())) {
    bookmarks.erase(bookmarks.begin() + index);
  }
}

bool BookmarkStore::hasBookmarkAt(int spineIndex, int pageInChapter) const {
  for (const auto& bm : bookmarks) {
    if (bm.spineIndex == spineIndex && bm.pageInChapter == pageInChapter) return true;
  }
  return false;
}

void BookmarkStore::clear() { bookmarks.clear(); }
