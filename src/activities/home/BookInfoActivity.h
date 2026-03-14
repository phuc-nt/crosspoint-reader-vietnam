#pragma once

#include <string>

#include "../Activity.h"
#include "util/ButtonNavigator.h"

/**
 * BookInfoActivity — displays metadata for a book file.
 * Shows: title, author, file path, file size.
 * For epubs: also shows chapter count, language.
 */
class BookInfoActivity final : public Activity {
 public:
  explicit BookInfoActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                            std::string filePath)
      : Activity("BookInfo", renderer, mappedInput), filePath(std::move(filePath)) {}
  void onEnter() override;
  void render(RenderLock&&) override;
  void loop() override;

 private:
  std::string filePath;
  std::string title;
  std::string author;
  std::string language;
  std::string fileSizeStr;
  int chapterCount = 0;
  bool isEpub = false;
};
