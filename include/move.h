#pragma once

#include <array>
#include <string>
#include <tuple>

constexpr std::pair<int, int> nullmove = {0, 0};

struct Move {
  std::array<std::pair<int, int>, 4> moves;

  bool is_double;

  Move(std::pair<int, int> a, std::pair<int, int> b) : moves{a, b, nullmove, nullmove}, is_double(false) {}
  Move(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c, std::pair<int, int> d)
      : moves{a, b, c, d}, is_double(true) {}

  std::string serialize() const;
};
