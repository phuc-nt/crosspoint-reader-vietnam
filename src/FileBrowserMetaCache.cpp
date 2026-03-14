#include "FileBrowserMetaCache.h"

FileBrowserMetaCache FileBrowserMetaCache::instance;

#include <HalStorage.h>
#include <Logging.h>

#include <cstring>

bool FileBrowserMetaCache::load() {
  cache.clear();
  if (!Storage.exists(CACHE_FILE)) return true;

  String json = Storage.readFile(CACHE_FILE);
  if (json.isEmpty()) return true;

  // Simple JSON array parser: [{"p":"path","t":"title","a":"author"},...]
  const char* p = json.c_str();
  while (*p) {
    const char* pathKey = strstr(p, "\"p\":\"");
    if (!pathKey) break;
    pathKey += 4;
    const char* pathEnd = strchr(pathKey, '"');
    if (!pathEnd) break;

    const char* titleKey = strstr(pathEnd, "\"t\":\"");
    if (!titleKey) break;
    titleKey += 4;
    const char* titleEnd = strchr(titleKey, '"');
    if (!titleEnd) break;

    const char* authorKey = strstr(titleEnd, "\"a\":\"");
    if (!authorKey) break;
    authorKey += 4;
    const char* authorEnd = strchr(authorKey, '"');
    if (!authorEnd) break;

    std::string path(pathKey, pathEnd - pathKey);
    BookMeta meta;
    meta.title = std::string(titleKey, titleEnd - titleKey);
    meta.author = std::string(authorKey, authorEnd - authorKey);

    cache[path] = std::move(meta);
    p = authorEnd + 1;
  }

  LOG_DBG("BMETA", "Loaded %d cached metadata entries", cache.size());
  return true;
}

bool FileBrowserMetaCache::save() const {
  if (!dirty) return true;

  Storage.mkdir("/.crosspoint");

  // Build JSON array
  std::string json = "[";
  bool first = true;
  for (const auto& [path, meta] : cache) {
    if (!first) json += ",";
    first = false;
    json += "{\"p\":\"" + path + "\",\"t\":\"" + meta.title + "\",\"a\":\"" + meta.author + "\"}";
  }
  json += "]";

  return Storage.writeFile(CACHE_FILE, json.c_str());
}

const FileBrowserMetaCache::BookMeta* FileBrowserMetaCache::get(const std::string& path) const {
  auto it = cache.find(path);
  if (it != cache.end()) return &it->second;
  return nullptr;
}

void FileBrowserMetaCache::set(const std::string& path, const std::string& title, const std::string& author) {
  cache[path] = {title, author};
  dirty = true;
}

bool FileBrowserMetaCache::has(const std::string& path) const {
  return cache.find(path) != cache.end();
}
