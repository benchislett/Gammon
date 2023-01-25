#include "misc.h"

constexpr int barwidth = 70;

// https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf
void ProgressBar::show() const {
        std::cout << '[';
        int pos = barwidth * progress;
        for (int i = 0; i < barwidth; ++i) {
                if (i < pos)
                        std::cout << '=';
                else if (i == pos)
                        std::cout << '>';
                else
                        std::cout << ' ';
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
}

void ProgressBar::tick(int k) {
        if (progress >= 1.0)
                return;

        int prev_pct = int(progress * 100);
        progress += (double)k / (double)n;
        int new_pct = int(progress * 100);

        if (new_pct != prev_pct) {
                show();
        }

        if (progress >= 1.0) {
                std::cout << '\n';
        }
}

int count_lines(std::ifstream &file) {
        int n = 0;
        std::string _line;
        while (std::getline(file, _line))
                n++;

        file.clear();
        file.seekg(0);

        return n;
}
