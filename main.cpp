#include "board.h"

#include <iostream>

int main() {
  Board b(false);
  b.white_pcs[24 - 1] = 1;
  b.white_pcs[23 - 1] = 1;
  b.white_pcs[22 - 1] = 1;

  b.black_pcs[24 - 1] = 1;
  b.black_pcs[23 - 1] = 1;
  b.black_pcs[22 - 1] = 1;

  std::cout << b.encode().serialize() << std::endl;
  return 0;
}