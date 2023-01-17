#include "board.h"
#include "movegen.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>

constexpr int MAX_DEPTH = 3;

void generate_opening(Board b, int turn, int depth, std::ofstream& file) {
        /* recursively determine all possible openings up to *depth* ply */

        if (depth >= MAX_DEPTH) return;

        // generate all possible rolls
        for (int d1 = 1; d1 <= 6; d1++) {
                for (int d2 = d1; d2 <= 6; d2++) {

                        if (depth == 0) printf("rolling %d, %d at depth 0\n", d1, d2);

                        // get legal moves
                        auto moves = legal_moves(b, turn, d1, d2);
                        
                        // recurse on each move
                        for (auto move : moves) {
                                auto hits = b.make_fullmove(turn, move);
                                file << b.encode().serialize() << '\n';
                                generate_opening(b, 1 - turn, depth + 1, file);
                                b.unmake_fullmove(turn, move, hits);
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

        // generate openings
        generate_opening(b, 0, 0, file);

        // close file
        file.close();

        return 0;
}
