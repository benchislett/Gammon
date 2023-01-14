#include "board.h"
#include "movegen.h"

#include <iostream>
#include <random>

int main() {
  Board b(true);

  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator

  std::cout << "New game\n";

  std::string input = "start";
  int botside;
  while (input != "quit") {
    std::cin >> input;

    if (input == "botside") {
      std::cin >> input;
      botside = input[0] - '0';
    }

    if (input == "botroll") {
      std::cin >> input;
      int d0     = input[0] - '0';
      int d1     = input[1] - '0';
      auto moves = legal_moves(b, botside, d0, d1);

      if (moves.size() == 0) {
        std::cout << "No legal moves\n";
      } else {
        std::uniform_int_distribution<> distr(0, moves.size() - 1);
        int botmove = distr(gen);

        b.make_fullmove(botside, moves[botmove]);
        std::cout << moves[botmove].serialize() << '\n';
      }
    }

    if (input == "playermove") {
      std::cin >> input;
      int from = std::stoi(input.substr(0, input.find('/'))) - 1;
      int to   = std::stoi(input.substr(input.find('/') + 1)) - 1;

      b.make_move(1 - botside, from, to);
    }

    if (input == "board") {
      std::cin >> input;
      b = Board(EncodedBoard(input));
      // 4HPwAFjgc/ARIA
      // xDN4wGAaz4MREA
      // NS7QQEN2vB0AAg
    }

    std::cout << b.encode().serialize() << std::endl;
  }

  return 0;
}
