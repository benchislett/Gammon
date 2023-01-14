#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

struct EncodedBoard {
  uint8_t data[10];

  EncodedBoard(){};
  EncodedBoard(const std::string&);

  std::string serialize() const;
};

struct Board {
  uint32_t pcs[2][25];

  Board(bool);
  Board() : Board(true) {}
  Board(EncodedBoard);

  EncodedBoard encode() const;

  bool on_bar(int side) const;
  bool can_bear_off(int side) const;
  bool has_blot(int side, int pos) const;
  bool has_point(int side, int pos) const;
  bool check_move(int side, int from, int to) const;

  void make_move(int side, int from, int to);
  void unmake_move(int side, int from, int to);
};
