#include "board.h"
#include "movegen.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

int main() {
        std::unordered_set<std::string> positions;
        for (int i = 0; i < 1000; i++) {
                Board b(true);

                std::random_device rd;  // obtain a random number from hardware
                std::mt19937 gen(rd()); // seed the generator

                std::uniform_int_distribution<> dice(1, 6);
                int turn = 0;
                int nomoves = 0;
                while (nomoves < 2) {
                        if (nomoves == 0)
                                positions.insert(b.encode().serialize());

                        auto moves = legal_moves(b, turn, dice(gen), dice(gen));
                        if (moves.size() > 0) {
                                nomoves = 0;
                                std::uniform_int_distribution<> distr(0, moves.size() - 1);
                                int botmove = distr(gen);

                                b.make_fullmove(turn, moves[botmove]);
                        } else {
                                nomoves++;
                        }

                        turn = 1 - turn;
                }
        }

        std::ofstream file;
        file.open("games.txt");
        for (auto &s : positions) {
                file << s << '\n';
        }
        file.close();

        return 0;
}
