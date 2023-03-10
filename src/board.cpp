#include "board.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

typedef unsigned char uchar;

static std::string base64_encode(const std::string &in) {

        std::string out;

        int val = 0, valb = -6;
        for (uchar c : in) {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0) {
                        out.push_back(
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
                        valb -= 6;
                }
        }
        if (valb > -6)
                out.push_back(
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) &
                                                                                       0x3F]);
        // while (out.size() % 4)
        //   out.push_back('=');
        return out;
}

static std::string base64_decode(const std::string &in) {

        std::string out;

        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++)
                T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val = 0, valb = -8;
        for (uchar c : in) {
                if (T[c] == -1)
                        break;
                val = (val << 6) + T[c];
                valb += 6;
                if (valb >= 0) {
                        out.push_back(char((val >> valb) & 0xFF));
                        valb -= 8;
                }
        }
        return out;
}

EncodedBoard::EncodedBoard(const std::string &bytes) {
        assert(bytes.size() > 0);
        std::string out = base64_decode(bytes);
        memcpy(data, out.data(), 10);
}

std::string EncodedBoard::serialize() const {
        std::string bytes("1234567890");
        memcpy(bytes.data(), data, 10);
        return base64_encode(bytes);
}

Board::Board(bool standard) {
        memset(pcs, 0, sizeof(pcs));

        if (standard) {
                for (int i = 0; i < 2; i++) {
                        pcs[i][24 - 1] = 2;
                        pcs[i][13 - 1] = 5;
                        pcs[i][8 - 1] = 3;
                        pcs[i][6 - 1] = 5;
                }
        }

        return;
}

Board::Board(EncodedBoard b) {
        int i = 0, j = 0, k;
        const unsigned char *a;

        memset(pcs[i], 0, sizeof(pcs));

        for (a = b.data; a < b.data + 10; ++a) {
                unsigned char cur = *a;

                for (k = 0; k < 8; ++k) {
                        if ((cur & 0x1)) {
                                if (i >= 2 || j >= 25) {
                                        return;
                                }
                                pcs[i][j]++;
                        } else {
                                if (++j == 25) {
                                        ++i;
                                        j = 0;
                                }
                        }
                        cur >>= 1;
                }
        }
        return;
}

static inline void addBits(unsigned char auchKey[10], unsigned int bitPos, unsigned int nBits) {
        unsigned int k = bitPos / 8;
        unsigned int r = (bitPos & 0x7);
        unsigned int b = (((unsigned int)0x1 << nBits) - 1) << r;

        auchKey[k] |= (unsigned char)b;

        if (k < 8) {
                auchKey[k + 1] |= (unsigned char)(b >> 8);
                auchKey[k + 2] |= (unsigned char)(b >> 16);
        } else if (k == 8) {
                auchKey[k + 1] |= (unsigned char)(b >> 8);
        }
}

EncodedBoard Board::encode() const {
        EncodedBoard b;
        memset(b.data, 0, 10 * sizeof(uint8_t));

        unsigned int i, iBit = 0;
        const unsigned int *j;
        for (i = 0; i < 2; ++i) {
                const unsigned int *bb = pcs[i];

                for (j = bb; j < bb + 25; ++j) {
                        const unsigned int nc = *j;
                        if (nc) {
                                addBits(b.data, iBit, nc);
                                iBit += nc + 1;
                        } else {
                                ++iBit;
                        }
                }
        }

        return b;
}

bool Board::on_bar(int side) const { return pcs[side][24] != 0; }

int Board::largest_piece(int side) const {
        for (int i = 24; i >= 0; i--) {
                if (pcs[side][i] != 0)
                        return i;
        }
        return -1;
}

bool Board::can_bear_off(int side) const { return largest_piece(side) < 6; }

bool Board::has_blot(int side, int pos) const { return pos != 24 && pcs[side][pos] == 1; }

bool Board::has_point(int side, int pos) const { return pos != 24 && pcs[side][pos] > 1; }

bool Board::check_move(int side, int from, int to) const {
        if (from == to)
                return false;

        if (pcs[side][from] == 0)
                return false;

        if (from < 0 || from >= 25 || to >= 25)
                return false;

        if (on_bar(side)) {
                if (from != 24)
                        return false;
        }

        if (to >= 0 && has_point(1 - side, 23 - to))
                return false;

        if (to < 0 && !can_bear_off(side))
                return false;

        if (to < 0) {
                if (!can_bear_off(side))
                        return false;
                if (to != -1 && largest_piece(side) > from)
                        return false;
        }

        return true;
}

bool Board::is_race() const {
        if (on_bar(0) || on_bar(1))
                return false;

        int p0_max = -1;
        int p1_max = -1;

        for (int i = 0; i < 25; i++) {
                if (pcs[0][i])
                        p0_max = i;
                if (pcs[1][i])
                        p1_max = i;
        }

        return (23 - p1_max) > p0_max;
}

bool Board::make_move(int side, int from, int to) {
        assert(check_move(side, from, to));

        pcs[side][from]--;

        if (to >= 0)
                pcs[side][to]++;

        if (to >= 0 && has_blot(1 - side, 23 - to)) {
                pcs[1 - side][23 - to]--;
                pcs[1 - side][24]++;
                return true;
        }

        return false;
}

void Board::unmake_move(int side, int from, int to, bool hit) {
        if (hit) {
                pcs[1 - side][23 - to]++;
                pcs[1 - side][24]--;
        }
        if (to >= 0) {
                pcs[side][to]--;
        }

        pcs[side][from]++;
}

void Board::swap() {
        for (int j = 0; j < 25; j++) {
                auto tmp = pcs[0][j];
                pcs[0][j] = pcs[1][j];
                pcs[1][j] = tmp;
        }
}

std::array<bool, 4> Board::make_fullmove(int side, const Move &m) {
        std::array<bool, 4> hits;
        int n = m.n;
        for (int i = 0; i < n; i++) {
                hits[i] = make_move(side, m.moves[i].first, m.moves[i].second);
        }

        return hits;
}

void Board::unmake_fullmove(int side, const Move &m, std::array<bool, 4> hits) {
        int n = m.n;
        for (int i = n - 1; i >= 0; i--) {
                unmake_move(side, m.moves[i].first, m.moves[i].second, hits[i]);
        }
}
