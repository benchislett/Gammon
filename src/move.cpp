#include "move.h"

#include <algorithm>

std::string Move::serialize() const {
        std::string out;

        for (int i = 0; i < n; i++) {
                auto [from, to] = moves[i];

                out += (from == 24) ? "bar" : std::to_string(from + 1);
                out += "/";

                // while (i < n - 1 && moves[i + 1].first == to) {
                //         to = moves[i + 1].second;
                //         i++;
                // }

                out += (to < 0) ? "off" : std::to_string(to + 1);

                if (i < n - 1)
                        out += " ";
        }

        return out;
}

Move Move::sorted() const {
        Move cpy(*this);
        std::sort(cpy.moves.begin(), cpy.moves.begin() + n);
        return cpy;
}

bool operator==(const Move &lhs, const Move &rhs) {
        if (lhs.n != rhs.n)
                return false;

        Move ls = lhs.sorted();
        Move rs = rhs.sorted();
        for (int i = 0; i < ls.n; i++) {
                if (ls.moves[i] != rs.moves[i])
                        return false;
        }

        return true;
}
