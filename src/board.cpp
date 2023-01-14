#include "board.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

typedef unsigned char uchar;

static std::string base64_encode(const std::string& in) {

  std::string out;

  int val = 0, valb = -6;
  for (uchar c : in) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
    out.push_back(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
  // while (out.size() % 4)
  //   out.push_back('=');
  return out;
}

static std::string base64_decode(const std::string& in) {

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

EncodedBoard::EncodedBoard(const std::string& bytes) {
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
      pcs[i][8 - 1]  = 3;
      pcs[i][6 - 1]  = 5;
    }
  }

  return;
}

Board::Board(EncodedBoard b) {
  int i = 0, j = 0, k;
  const unsigned char* a;

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
  unsigned int b = (((unsigned int) 0x1 << nBits) - 1) << r;

  auchKey[k] |= (unsigned char) b;

  if (k < 8) {
    auchKey[k + 1] |= (unsigned char) (b >> 8);
    auchKey[k + 2] |= (unsigned char) (b >> 16);
  } else if (k == 8) {
    auchKey[k + 1] |= (unsigned char) (b >> 8);
  }
}

EncodedBoard Board::encode() const {
  EncodedBoard b;
  memset(b.data, 0, 10 * sizeof(uint8_t));

  unsigned int i, iBit = 0;
  const unsigned int* j;
  for (i = 0; i < 2; ++i) {
    const unsigned int* bb = pcs[i];

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

bool Board::on_bar(int side) const {
  return pcs[side][24] != 0;
}

bool Board::can_bear_off(int side) const {
  for (int i = 6; i < 25; i++) {
    if (pcs[side][i] != 0)
      return false;
  }
  return true;
}

bool Board::has_blot(int side, int pos) const {
  return pos != 24 && pcs[side][pos] == 1;
}

bool Board::has_point(int side, int pos) const {
  return pos != 24 && pcs[side][pos] > 1;
}

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

  if (has_point(1 - side, to))
    return false;

  if (to < 0 && !can_bear_off(side))
    return false;

  return true;
}

void Board::make_move(int side, int from, int to) {
  assert(check_move(side, from, to));

  pcs[side][from]--;

  if (to >= 0)
    pcs[side][to]++;
}

void Board::unmake_move(int side, int from, int to) {
  if (to >= 0) {
    pcs[side][to]--;
  }

  pcs[side][from]++;
}
