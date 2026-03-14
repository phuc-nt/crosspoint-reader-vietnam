#include "ReadingStats.h"

ReadingStats ReadingStats::instance;

#include <HalStorage.h>
#include <Logging.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

bool ReadingStats::load() {
  if (!Storage.exists(STATS_FILE)) {
    return true;  // No stats yet
  }

  String json = Storage.readFile(STATS_FILE);
  if (json.isEmpty()) return true;

  const char* p = json.c_str();

  // Parse "time":N
  const char* timeKey = strstr(p, "\"time\":");
  if (timeKey) {
    totalReadingTimeSeconds = strtoul(timeKey + 7, nullptr, 10);
  }

  // Parse "books":N
  const char* booksKey = strstr(p, "\"books\":");
  if (booksKey) {
    booksFinished = static_cast<uint16_t>(atoi(booksKey + 8));
  }

  // Parse "pages":N
  const char* pagesKey = strstr(p, "\"pages\":");
  if (pagesKey) {
    totalPagesTurned = strtoul(pagesKey + 8, nullptr, 10);
  }

  LOG_DBG("RST", "Loaded stats: %us reading, %u books, %u pages",
          totalReadingTimeSeconds, booksFinished, totalPagesTurned);
  return true;
}

bool ReadingStats::save() const {
  Storage.mkdir("/.crosspoint");

  char buf[128];
  snprintf(buf, sizeof(buf),
           "{\"time\":%u,\"books\":%u,\"pages\":%u}",
           totalReadingTimeSeconds, booksFinished, totalPagesTurned);

  return Storage.writeFile(STATS_FILE, buf);
}

void ReadingStats::startSession() {
  if (!sessionActive) {
    sessionStartMs = millis();
    sessionActive = true;
  }
}

void ReadingStats::endSession() {
  if (sessionActive) {
    uint32_t elapsed = (millis() - sessionStartMs) / 1000;
    // Sanity check: cap at 4 hours per session (prevent overflow from sleep/wake)
    if (elapsed > 14400) elapsed = 0;
    totalReadingTimeSeconds += elapsed;
    sessionActive = false;
  }
}

void ReadingStats::incrementBooksFinished() {
  booksFinished++;
}

void ReadingStats::incrementPagesTurned() {
  totalPagesTurned++;
}

std::string ReadingStats::getFormattedReadingTime() const {
  uint32_t total = totalReadingTimeSeconds;
  // Include current session if active
  if (sessionActive) {
    uint32_t elapsed = (millis() - sessionStartMs) / 1000;
    if (elapsed <= 14400) total += elapsed;
  }

  uint32_t hours = total / 3600;
  uint32_t minutes = (total % 3600) / 60;

  char buf[32];
  if (hours > 0) {
    snprintf(buf, sizeof(buf), "%uh %um", hours, minutes);
  } else {
    snprintf(buf, sizeof(buf), "%um", minutes);
  }
  return std::string(buf);
}
