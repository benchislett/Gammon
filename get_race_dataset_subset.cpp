#include "board.h"
#include "net.h"

int main() {
        std::ifstream evals_in;
        evals_in.open("rollout_evals_large_partial.txt");

        std::ifstream positions_in;
        positions_in.open("rollout_positions_large_partial.txt");

        std::ofstream evals_out;
        evals_out.open("rollout_evals_large_partial_races.txt");

        std::ofstream positions_out;
        positions_out.open("rollout_positions_large_partial_races.txt");

        std::string eval_line;
        std::string board_line = "abcs";
        while (std::getline(evals_in, eval_line)) {
                std::getline(positions_in, board_line);

                Board b{EncodedBoard(board_line)};

                if (b.is_race()) {
                        evals_out << eval_line << '\n';
                        positions_out << board_line << '\n';
                }
        }
}
