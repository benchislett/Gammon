#include "net.h"

#include <random>
#include <algorithm>

torch::Tensor load_score(const std::string& line) {
        float rawdata[7];
        int res = sscanf(line.c_str(), " 2 ply: %f   %f   %f   %f   %f   %f   %f   ", &rawdata[0], &rawdata[1], &rawdata[2], &rawdata[3],
                        &rawdata[4], &rawdata[5], &rawdata[6]);
        if (res != 7) {
                std::cerr << "Failed to read line \"" << line << "\"\n";
        }
        std::array<float, 5> data{rawdata[0], rawdata[1], rawdata[2], rawdata[4], rawdata[5]};
        auto tensor = torch::from_blob(data.begin(), {1}, torch::TensorOptions().dtype(torch::kFloat32)).clone();
        return tensor;
}

torch::Tensor load_board(const std::string& s) {
        Board game{EncodedBoard(s)};
        float features[2][25][4];
        for (int side = 0; side < 2; side++) {
                for (int position = 0; position < 25; position++) {
                        features[side][position][0] = game.pcs[side][position] == 1 ? 1.0 : 0.0;
                        features[side][position][1] = game.pcs[side][position] >= 2 ? 1.0 : 0.0;
                        features[side][position][2] = game.pcs[side][position] == 3 ? 1.0 : 0.0;
                        features[side][position][3] = game.pcs[side][position] >= 4 ? 0.5 * (float) (game.pcs[side][position] - 3) : 0.0;
                }
        }
        auto tensor = torch::from_blob(features, {200}, torch::TensorOptions().dtype(torch::kFloat32)).clone();
        return tensor;
}

Net::Net() {
        fc1 = register_module("input", torch::nn::Linear(200, 128));
        fc2 = register_module("output", torch::nn::Linear(128, 1));

        torch::nn::init::uniform_(fc1->weight, -0.5, 0.5);
        torch::nn::init::uniform_(fc2->weight, -0.5, 0.5);
        torch::nn::init::constant_(fc1->bias, 0.001);
        torch::nn::init::constant_(fc2->bias, 0.001);
}

torch::Tensor Net::forward(torch::Tensor x) {
        x = torch::sigmoid(fc1->forward(x.reshape({x.size(0), 200})));
        // x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
        x = torch::sigmoid(fc2->forward(x));
        return x;
}

torch::data::Example<> CustomDataset::get(size_t index) {
        torch::Tensor sample_game  = games.at(index);
        torch::Tensor sample_score = scores.at(index);
        return {sample_game.clone(), sample_score.clone()};
};

torch::optional<size_t> CustomDataset::size() const {
        return scores.size();
};

void CustomDataset::split(double frac, CustomDataset &A, CustomDataset &B) const {
  std::vector<int> indices(scores.size());
  for (int i = 0; i < size(); i++) indices[i] = i;
  std::random_shuffle(indices.begin(), indices.end());

  int nA = (int) (frac * scores.size());

  int i;
  for (i = 0; i < nA; i++) {
    int idx = indices[i];
    A.games.push_back(games.at(idx).clone());
    A.scores.push_back(scores.at(idx).clone());
  }
  for (; i < scores.size(); i++) {
    int idx = indices[i];
    B.games.push_back(games.at(idx).clone());
    B.scores.push_back(scores.at(idx).clone());
  }
}

CustomDataset load_dataset() {
        CustomDataset dataset;

        std::string line;
        std::ifstream score_file("evals.txt");
        while (std::getline(score_file, line)) {
                dataset.scores.push_back(load_score(line));
        }

        std::ifstream games_file("openings.txt");
        int i = 0;
        std::string token;
        for (int i = 0; i < dataset.scores.size(); i++) {
                games_file >> token;
                dataset.games.push_back(load_board(token));
        }

        return std::move(dataset);
}
