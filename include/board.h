#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct EncodedBoard {
  uint8_t data[10];

  EncodedBoard(){};
  EncodedBoard(const std::string&);

  std::string serialize() const;
};

struct Board {
  uint32_t white_pcs[25];
  uint32_t black_pcs[25];

  Board(bool);
  Board() : Board(true) {}
  Board(EncodedBoard);

  EncodedBoard encode() const;
};