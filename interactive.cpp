#include "board.h"
#include "movegen.h"
#include "net.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <torch/torch.h>
#include <unordered_set>

int main() {

  Board b(true);

  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator

  std::cout << "New game\n";

  auto net = std::make_shared<Net>();
  torch::load(net, "net.pt");

  std::string input = "start";
  int botside       = 0;

  while (input != "quit") {
    std::cin >> input;

    if (input == "swap") {
      b.swap();
    }

    if (input == "botroll") {
      std::cin >> input;
      int d0     = input[0] - '0';
      int d1     = input[1] - '0';
      auto moves = legal_moves(b, botside, d0, d1);

      if (moves.size() == 0) {
        std::cout << "No legal moves\n";
      } else {

        auto before = b.encode().serialize();

        std::vector<float> move_evals;

        for (int move = 0; move < moves.size(); move++) {
          auto record = b.make_fullmove(botside, moves[move]);
          b.swap();

          auto board_tensor = load_board(b.encode().serialize()).reshape({1, 200});
          auto pred_tensor  = net->forward(board_tensor);

          // std::cout << moves[move].serialize() << '\n';
          // std::cout << "Bot eval: " << pred_tensor[0][0].item<float>() << '\n';

          move_evals.push_back(pred_tensor[0][0].item<float>());

          b.swap();
          b.unmake_fullmove(botside, moves[move], record);
        }

        int max_index = std::distance(move_evals.begin(), std::max_element(move_evals.begin(), move_evals.end()));

        b.make_fullmove(botside, moves[max_index]);

        std::cout << "Bot plays " << moves[max_index].serialize() << " with eval " << move_evals[max_index] << '\n';
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
    }

    std::cout << b.encode().serialize() << std::endl;
  }

  return 0;
}
