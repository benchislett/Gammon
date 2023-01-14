#include "move.h"

std::string Move::serialize() const {
  std::string out;

  int n = is_double ? 4 : 2;
  for (int i = 0; i < n; i++) {
    auto [from, to] = moves[i];

    out += (from == 24) ? "bar" : std::to_string(from + 1);
    out += "/";
    out += (to < 0) ? "off" : std::to_string(to + 1);

    if (i < n - 1)
      out += " ";
  }

  return out;
}
