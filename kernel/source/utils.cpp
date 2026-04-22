#include "utils.hpp"
#include "console.hpp"
#include <stdint.h>

void numstream(int64_t u, int x, int y) {
  if (x >= 0)
    Console::state.cursor_col = x;
  if (y < 0)
    y = Console::state.cursor_row;
  else
    Console::state.cursor_row = y;

  if (Console::state.cursor_col > Console::state.max_cols)
    Console::newline();

  y *= Console::CELL_H;

  if (u == 0) {
    Console::draw_char_transparent(Console::state.cursor_col * Console::CELL_W,
                                   y, '0', Console::state.fg_color);
    Console::state.cursor_col++;
    return;
  }

  char sign = -1 * (u < 0);

  if (sign < 0)
    u = -u;

  const uint8_t base = 10;
  int64_t big = 1;
  int max_digit = 0;

  int64_t ut = u;
  while (ut > 0) {
    max_digit++;
    big *= base;
    ut /= base;
  }

  if (sign < 0) {
    Console::draw_char_transparent(Console::state.cursor_col * Console::CELL_W,
                                   y, '-', Console::state.fg_color);
    Console::state.cursor_col++;
  }

  uint8_t prev_num = 0;
  for (; max_digit > 0; max_digit--) {
    big /= base;
    ut = u / big;
    Console::draw_char_transparent(Console::state.cursor_col * Console::CELL_W,
                                   y, '0' + ut - base * prev_num,
                                   Console::state.fg_color);
    prev_num = ut;
    Console::state.cursor_col++;
    if (Console::state.cursor_col >= Console::state.max_cols)
      Console::newline();
  }
}
