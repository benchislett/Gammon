#pragma once

#include "board.h"
#include "misc.h"

#include <array>
#include <fstream>
#include <memory>
#include <string>
#include <torch/torch.h>

torch::Tensor load_score(const std::string &);

torch::Tensor load_board(const std::string &);

struct Net : torch::nn::Module {
        Net();
        torch::Tensor forward(torch::Tensor x);

        torch::nn::Linear fc1{nullptr}, fc2{nullptr};
};

struct LazyCustomDataset : public torch::data::datasets::Dataset<LazyCustomDataset> {
        std::shared_ptr<std::ifstream> scores_file;
        std::shared_ptr<std::ifstream> games_file;
        std::shared_ptr<std::mutex> file_lock;
        int line_cnt;

        LazyCustomDataset(const std::string &scores_path, const std::string &games_path)
            : scores_file{std::make_shared<std::ifstream>(scores_path)}, games_file{std::make_shared<std::ifstream>(
                                                                             games_path)},
              file_lock(std::make_shared<std::mutex>()), line_cnt(count_lines(*scores_file)) {}

        // Override get() function to return tensor at location index
        torch::data::Example<> get(size_t index) override;

        // Return the length of data
        torch::optional<size_t> size() const override;
};

struct CustomDataset : public torch::data::datasets::Dataset<CustomDataset> {
        std::vector<torch::Tensor> games, scores;

        CustomDataset(const std::string &scores_path, const std::string &games_path);

        // Override get() function to return tensor at location index
        torch::data::Example<> get(size_t index) override;

        // Return the length of data
        torch::optional<size_t> size() const override;

        void split(double frac, CustomDataset &A, CustomDataset &B) const;
};
