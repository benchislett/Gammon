#include "board.h"
#include "movegen.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

constexpr int MAX_DEPTH = 2;

void generate_opening(Board b, int turn, int depth, std::unordered_set<std::string> &positions) {
        /* recursively determine all possible openings up to *depth* ply */

        if (depth >= MAX_DEPTH) return;

        // generate all possible rolls
        for (int d1 = 1; d1 <= 6; d1++) {
                for (int d2 = 1; d2 <= 6; d2++) {
                        auto moves = legal_moves(b, turn, d1, d2);
                        
                        for (auto move : moves) {
                                auto hits = b.make_fullmove(turn, move);
                                positions.insert(b.encode().serialize());
                                generate_opening(b, 1 - turn, depth + 1, positions);
                                b.unmake_fullmove(turn, move, hits);
                        }
                }
        }
}

int main() {
        std::unordered_set<std::string> positions;

        // generate starting board
        Board b(true);

        generate_opening(b, 0, 0, positions);

        std::ofstream file;
        file.open("openings.txt");
        for (auto& s : positions) {
                file << s << '\n';
        }
        file.close();

        return 0;
}
