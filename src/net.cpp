#include "net.h"

#include "misc.h"

#include <algorithm>
#include <random>

torch::Device device_(torch::kCPU);

torch::Tensor load_score(const std::string &line) {
        float rawdata[7];
        int res = sscanf(line.c_str(), " 2 ply: %f   %f   %f   %f   %f   %f   %f   ", &rawdata[0], &rawdata[1],
                         &rawdata[2], &rawdata[3], &rawdata[4], &rawdata[5], &rawdata[6]);
        if (res != 7) {
                std::cerr << "Failed to read line \"" << line << "\"\n";
        }
        std::array<float, 5> data{rawdata[0], rawdata[1], rawdata[2], rawdata[4], rawdata[5]};
        auto tensor =
            torch::from_blob(data.begin(), {1}, torch::TensorOptions().dtype(torch::kFloat32)).clone().to(device_);
        return tensor;
}

torch::Tensor load_board(const std::string &s) {
        Board game{EncodedBoard(s)};
        float features[2][25][4];
        for (int side = 0; side < 2; side++) {
                for (int position = 0; position < 25; position++) {
                        features[side][position][0] = game.pcs[side][position] == 1 ? 1.0 : 0.0;
                        features[side][position][1] = game.pcs[side][position] >= 2 ? 1.0 : 0.0;
                        features[side][position][2] = game.pcs[side][position] == 3 ? 1.0 : 0.0;
                        features[side][position][3] =
                            game.pcs[side][position] >= 4 ? 0.5 * (float)(game.pcs[side][position] - 3) : 0.0;
                }
        }
        auto tensor =
            torch::from_blob(features, {200}, torch::TensorOptions().dtype(torch::kFloat32)).clone().to(device_);
        return tensor;
}

Net::Net() {
        bn1 = register_module("batchnorm 1", torch::nn::BatchNorm1d{200});
        bn2 = register_module("batchnorm 2", torch::nn::BatchNorm1d{256});
        bn3 = register_module("batchnorm 3", torch::nn::BatchNorm1d{256});

        fc1 = register_module("input", torch::nn::Linear(200, 256));
        fc2 = register_module("h1", torch::nn::Linear(256, 256));
        fc3 = register_module("h2", torch::nn::Linear(256, 256));
        fc4 = register_module("output", torch::nn::Linear(256, 1));

        // torch::nn::init::uniform_(fc1->weight, -0.5, 0.5);
        // torch::nn::init::uniform_(fc2->weight, -0.5, 0.5);
        // torch::nn::init::uniform_(fc3->weight, -0.5, 0.5);
        // torch::nn::init::uniform_(fc4->weight, -0.5, 0.5);

        torch::nn::init::normal_(fc1->bias);
        torch::nn::init::normal_(fc2->bias);
        torch::nn::init::normal_(fc3->bias);
        torch::nn::init::normal_(fc4->bias);
}

torch::Tensor Net::forward(torch::Tensor x) {
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 200})));
        // x = torch::dropout(x, 0.5, is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::relu(fc3->forward(x));
        x = torch::sigmoid(fc4->forward(x));
        return x;
}

torch::data::Example<> LazyCustomDataset::get(size_t index) {
        std::string score_line, board_line;
        file_lock->lock();
        games_file->seekg(15 * index);
        scores_file->seekg(69 * index);
        std::getline(*games_file, board_line);
        std::getline(*scores_file, score_line);
        file_lock->unlock();
        auto g = load_board(board_line);
        auto s = load_score(score_line);
        return {g, s};
};

torch::optional<size_t> LazyCustomDataset::size() const { return line_cnt; };

CustomDataset::CustomDataset(const std::string &scores_path, const std::string &games_path) {
        {
                std::string line;
                std::ifstream score_file(scores_path);
                ProgressBar load_scores(count_lines(score_file), "Loading Scores");
                while (std::getline(score_file, line)) {
                        scores.push_back(load_score(line));
                        load_scores.tick();
                }
        }

        {
                std::ifstream games_file(games_path);
                std::string token;
                ProgressBar load_games(scores.size(), "Loading Games");
                for (int i = 0; i < scores.size(); i++) {
                        games_file >> token;
                        games.push_back(load_board(token));
                        load_games.tick();
                }
        }
}

torch::data::Example<> CustomDataset::get(size_t index) {
        torch::Tensor sample_game = games.at(index);
        torch::Tensor sample_score = scores.at(index);
        return {sample_game.clone(), sample_score.clone()};
};

torch::optional<size_t> CustomDataset::size() const { return scores.size(); };
