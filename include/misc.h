#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

struct ProgressBar {
  int n;
  double progress;

  ProgressBar(int nSteps, const std::string& title = "") : n(nSteps), progress(0.0) {
    if (!title.empty())
      std::cout << title << '\n';
    show();
  }

  void show() const;
  void tick(int k = 1);
};

int count_lines(std::ifstream& file);