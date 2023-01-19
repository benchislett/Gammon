#include "movegen.h"

std::vector<Move> legal_moves(Board b, int side, int d1, int d2) {
        std::vector<Move> moves;

        if (b.on_bar(side) && !b.check_move(side, 24, 24 - d1)) {
                std::swap(d1, d2);
        }

        bool swapped = false;
        bool found = false;
x:

        for (int from1 = d1 - 1; from1 < 25; from1++) {
                int to1 = from1 - d1;
                if (!b.check_move(side, from1, to1))
                        continue;

                bool hit1 = b.make_move(side, from1, to1);

                bool found2 = false;

                for (int from2 = d2 - 1; from2 < 25; from2++) {
                        int to2 = from2 - d2;
                        if (!b.check_move(side, from2, to2))
                                continue;
                        found2 = true;

                        if (d1 != d2) {
                                moves.emplace_back(std::make_pair(from1, to1), std::make_pair(from2, to2));
                                continue;
                        }

                        bool hit2 = b.make_move(side, from2, to2);

                        bool found3 = false;

                        for (int from3 = d2 - 1; from3 < 25; from3++) {
                                int to3 = from3 - d2;
                                if (!b.check_move(side, from3, to3))
                                        continue;
                                found3 = true;

                                bool hit3 = b.make_move(side, from3, to3);

                                bool found4 = false;

                                for (int from4 = d2 - 1; from4 < 25; from4++) {
                                        int to4 = from4 - d2;
                                        if (!b.check_move(side, from4, to4))
                                                continue;
                                        found4 = true;

                                        moves.emplace_back(std::make_pair(from1, to1), std::make_pair(from2, to2),
                                                           std::make_pair(from3, to3), std::make_pair(from4, to4));
                                }

                                if (!found4)
                                        moves.emplace_back(std::make_pair(from1, to1), std::make_pair(from2, to2),
                                                           std::make_pair(from3, to3));

                                b.unmake_move(side, from3, to3, hit3);
                        }

                        if (!found3)
                                moves.emplace_back(std::make_pair(from1, to1), std::make_pair(from2, to2));

                        b.unmake_move(side, from2, to2, hit2);
                }

                if (!found2)
                        moves.emplace_back(std::make_pair(from1, to1));

                b.unmake_move(side, from1, to1, hit1);
        }

        if (!found && !swapped) {
                swapped = true;
                std::swap(d1, d2);
                goto x;
        }

        return moves;
}
