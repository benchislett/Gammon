#pragma once

#include "move.h"

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

struct EncodedBoard {
        uint8_t data[10];

        EncodedBoard(){};
        EncodedBoard(const std::string &);

        std::string serialize() const;
};

struct Board {
        uint32_t pcs[2][25];

        Board(bool);
        Board() : Board(true) {}
        Board(EncodedBoard);

        EncodedBoard encode() const;

        bool on_bar(int side) const;
        int largest_piece(int side) const;
        bool can_bear_off(int side) const;
        bool has_blot(int side, int pos) const;
        bool has_point(int side, int pos) const;
        bool check_move(int side, int from, int to) const;
        bool is_race() const;

        bool make_move(int side, int from, int to);
        void unmake_move(int side, int from, int to, bool hit);
        void swap();

        std::array<bool, 4> make_fullmove(int side, const Move &);
        void unmake_fullmove(int side, const Move &, std::array<bool, 4>);
};
