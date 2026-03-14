#pragma once

#include <string>
#include <vector>

#include "../Activity.h"
#include "BookmarkStore.h"
#include "util/ButtonNavigator.h"

/**
 * Activity that displays a list of bookmarks for the current book.
 * User can select a bookmark to navigate to it, or remove it.
 */
class BookmarkListActivity final : public Activity {
 public:
  explicit BookmarkListActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                BookmarkStore& bookmarkStore);

  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  BookmarkStore& bookmarkStore;
  ButtonNavigator buttonNavigator;
  int selectedIndex = 0;
  bool confirmDelete = false;  // Showing delete confirmation for selected
};
