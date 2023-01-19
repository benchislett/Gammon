#include "board.h"
#include "movegen.h"

#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>

constexpr int MAX_DEPTH = 2;

/* recursively determine all possible openings up to *depth* ply */
void generate_opening(Board &b, int depth, std::ofstream &file, std::set<std::string> &unique_positions) {
        auto position = b.encode().serialize();

        if (unique_positions.find(position) != unique_positions.end())
                return;
        unique_positions.insert(position);

        file << position << '\n';

        if (depth >= MAX_DEPTH)
                return;

        // generate all possible rolls
        for (int d1 = 1; d1 <= 6; d1++) {
                for (int d2 = d1; d2 <= 6; d2++) {

                        if (depth == 0)
                                printf("rolling %d, %d at depth 0\n", d1, d2);

                        // get legal moves
                        auto moves = legal_moves(b, 0, d1, d2);

                        // recurse on each move
                        for (auto move : moves) {
                                auto hits = b.make_fullmove(0, move);
                                b.swap();
                                generate_opening(b, depth + 1, file, unique_positions);
                                b.swap();
                                b.unmake_fullmove(0, move, hits);
                        }
                }
        }
}

int main() {
        // generate starting board
        Board b(true);

        // open file
        std::ofstream file;
        file.open("openings.txt");

        std::set<std::string> unique_positions;

        // generate openings
        generate_opening(b, 0, file, unique_positions);

        // close file
        file.close();

        return 0;
}
