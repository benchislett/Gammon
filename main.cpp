#include "board.h"

#include <iostream>

int main() {
  Board b(true);

  for (int i = 0; i < 2; i++) {
    // b.pcs[i][24 - 1] = 1;
    // b.pcs[i][23 - 1] = 1;
    // b.pcs[i][22 - 1] = 1;
  }

  std::cout << b.encode().serialize() << std::endl;
  return 0;
}
