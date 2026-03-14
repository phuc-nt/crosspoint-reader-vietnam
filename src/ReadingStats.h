#pragma once

#include <cstdint>
#include <string>

/**
 * Tracks reading statistics across sessions.
 * Stored as JSON at /.crosspoint/reading_stats.json
 *
 * Memory-efficient: only keeps aggregated stats in RAM.
 */
class ReadingStats {
 public:
  static ReadingStats &getInstance() { return instance; }

  /// Load stats from SD card
  bool load();

  /// Save stats to SD card
  bool save() const;

  /// Call when a reading session starts
  void startSession();

  /// Call when a reading session ends (page turn, exit, sleep)
  void endSession();

  /// Call when a book is finished (100% progress)
  void incrementBooksFinished();

  /// Get total reading time in seconds
  uint32_t getTotalReadingTimeSeconds() const { return totalReadingTimeSeconds; }

  /// Get total books finished
  uint16_t getBooksFinished() const { return booksFinished; }

  /// Get total pages turned
  uint32_t getTotalPagesTurned() const { return totalPagesTurned; }

  /// Increment page turn counter
  void incrementPagesTurned();

  /// Get formatted time string "Xh Ym"
  std::string getFormattedReadingTime() const;

 private:
  static ReadingStats instance;

  uint32_t totalReadingTimeSeconds = 0;
  uint16_t booksFinished = 0;
  uint32_t totalPagesTurned = 0;
  uint32_t sessionStartMs = 0;
  bool sessionActive = false;

  static constexpr const char* STATS_FILE = "/.crosspoint/reading_stats.json";
};

#define READING_STATS ReadingStats::getInstance()
