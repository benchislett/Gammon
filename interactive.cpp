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

torch::Device device(torch::kCPU);

int main() {

        Board b(true);

        std::random_device rd;  // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator

        std::cout << "New game\n";

        auto net = std::make_shared<Net>();
        torch::load(net, "models_archive/net_4x256_noregularize_relu_sigmoid.pt");

        auto racenet = std::make_shared<DeepNet>();
        torch::load(racenet, "models_archive/net_race_5x512_32_noregularize_relu_sigmoid.pt");

        net->to(device);
        racenet->to(device);

        net->eval();
        racenet->eval();

        auto predict = [&](const Board &b) {
                auto board_tensor = load_board(b.encode().serialize()).reshape({1, 200});
                auto pred_tensor = (b.is_race() ? racenet->forward(board_tensor) : net->forward(board_tensor));
                return pred_tensor[0][0].item<float>();
        };

        std::string input = "start";
        int botside = 0;

        while (input != "quit") {
                std::cin >> input;

                if (input == "swap") {
                        b.swap();
                }

                if (input == "eval") {
                        std::cout << "Bot eval position " << b.encode().serialize() << " with eval " << predict(b)
                                  << '\n';
                }

                if (input == "botroll") {
                        std::cin >> input;
                        int d0 = input[0] - '0';
                        int d1 = input[1] - '0';
                        auto moves = legal_moves(b, botside, d0, d1);

                        if (moves.size() == 0) {
                                std::cout << "No legal moves\n";
                        } else {

                                auto before = b.encode().serialize();

                                std::vector<float> move_evals;

                                for (int move = 0; move < moves.size(); move++) {
                                        auto record = b.make_fullmove(botside, moves[move]);

                                        move_evals.push_back(1.0 - predict(b));

                                        std::cout << "Bot eval " << moves[move].serialize() << " with eval "
                                                  << move_evals.back() << " (board " << b.encode().serialize() << ")\n";

                                        b.unmake_fullmove(botside, moves[move], record);
                                }

                                int max_index = std::distance(move_evals.begin(),
                                                              std::max_element(move_evals.begin(), move_evals.end()));

                                b.make_fullmove(botside, moves[max_index]);

                                std::cout << "Bot plays " << moves[max_index].serialize() << " with eval "
                                          << move_evals[max_index] << '\n';
                        }
                }

                if (input == "playermove") {
                        std::cin >> input;
                        int from = std::stoi(input.substr(0, input.find('/'))) - 1;
                        int to = std::stoi(input.substr(input.find('/') + 1)) - 1;

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
