#pragma once

#include <stdint.h>

extern "C" void numstream(int64_t u, int x = -1, int y = -1);
extern "C" void print(const char *fmr, ...);

namespace Color {
inline uint32_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return r | (g << 8) | (b << 16) | (a << 24);
}

} // namespace Color
