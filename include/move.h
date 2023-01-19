#pragma once

#include <array>
#include <string>
#include <tuple>

constexpr std::pair<int, int> nullmove = {0, 0};

struct Move {
        std::array<std::pair<int, int>, 4> moves;
        int n;

        Move(std::pair<int, int> a) : moves{a, nullmove, nullmove, nullmove}, n(1) {}
        Move(std::pair<int, int> a, std::pair<int, int> b) : moves{a, b, nullmove, nullmove}, n(2) {}
        Move(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c) : moves{a, b, c, nullmove}, n(3) {}
        Move(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c, std::pair<int, int> d)
            : moves{a, b, c, d}, n(4) {}

        std::string serialize() const;
};
