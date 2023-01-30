#include "movegen.h"

#include <unordered_set>

std::vector<Move> legal_moves(Board b, int side, int d1, int d2) {
        std::unordered_set<Move> move_set;
        std::vector<Move> moves;

        auto add_move = [&](const Move &m) {
                if (!move_set.contains(m)) {
                        move_set.insert(m);
                        moves.push_back(m);
                }
        };

        if (b.on_bar(side) && !b.check_move(side, 24, 24 - d1)) {
                std::swap(d1, d2);
        }

        bool swapped = false;
        bool found = false;
x:

        for (int from1 = 0; from1 < 25; from1++) {
                int to1 = from1 - d1;
                if (!b.check_move(side, from1, to1))
                        continue;

                bool hit1 = b.make_move(side, from1, to1);

                bool found2 = false;

                for (int from2 = 0; from2 < 25; from2++) {
                        int to2 = from2 - d2;
                        if (!b.check_move(side, from2, to2))
                                continue;
                        found2 = true;

                        if (d1 != d2) {
                                add_move(Move(std::make_pair(from1, to1), std::make_pair(from2, to2)));
                                continue;
                        }

                        bool hit2 = b.make_move(side, from2, to2);

                        bool found3 = false;

                        for (int from3 = 0; from3 < 25; from3++) {
                                int to3 = from3 - d2;
                                if (!b.check_move(side, from3, to3))
                                        continue;
                                found3 = true;

                                bool hit3 = b.make_move(side, from3, to3);

                                bool found4 = false;

                                for (int from4 = 0; from4 < 25; from4++) {
                                        int to4 = from4 - d2;
                                        if (!b.check_move(side, from4, to4))
                                                continue;
                                        found4 = true;

                                        add_move(Move(std::make_pair(from1, to1), std::make_pair(from2, to2),
                                                      std::make_pair(from3, to3), std::make_pair(from4, to4)));
                                }

                                if (!found4)
                                        add_move(Move(std::make_pair(from1, to1), std::make_pair(from2, to2),
                                                      std::make_pair(from3, to3)));

                                b.unmake_move(side, from3, to3, hit3);
                        }

                        if (!found3)
                                add_move(Move(std::make_pair(from1, to1), std::make_pair(from2, to2)));

                        b.unmake_move(side, from2, to2, hit2);
                }

                if (!found2)
                        add_move(Move(std::make_pair(from1, to1)));

                b.unmake_move(side, from1, to1, hit1);
        }

        if (!found && !swapped) {
                swapped = true;
                std::swap(d1, d2);
                goto x;
        }

        // Filter moves that do not fully utilize the dice

        int longest_move = 0;
        for (auto &move : moves) {
                longest_move = std::max(longest_move, move.n);
        }

        std::vector<Move> long_moves;
        for (auto &move : moves) {
                if (move.n == longest_move)
                        long_moves.push_back(move);
        }

        if (longest_move == 1 && long_moves.size() > 1) {
                // must use greater dice

                std::vector<Move> valid_moves;
                for (auto &m : long_moves) {
                        if (std::abs(m.moves[0].second - m.moves[0].first) == std::max(d1, d2))
                                valid_moves.push_back(m);
                }

                if (valid_moves.size() > 0)
                        long_moves = valid_moves;
        }

        return long_moves;
}
