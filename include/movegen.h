#pragma once

#include "board.h"
#include "move.h"

std::vector<Move> legal_moves(Board b, int side, int d1, int d2);