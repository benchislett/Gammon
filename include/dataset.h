#pragma once

#include "board.h"
#include "misc.h"
#include "net.h"

#include <concepts>
#include <fstream>
#include <memory>
#include <string>
#include <torch/torch.h>

torch::Tensor load_score(const std::string &);

torch::Tensor load_board(const std::string &);

struct LazyCustomDataset : public torch::data::datasets::Dataset<LazyCustomDataset> {
        std::shared_ptr<std::ifstream> scores_file;
        std::shared_ptr<std::ifstream> games_file;
        std::shared_ptr<std::mutex> file_lock;
        int line_cnt, line_len_scores, line_len_games;

        LazyCustomDataset(const std::string &scores_path, const std::string &games_path)
            : scores_file{std::make_shared<std::ifstream>(scores_path)}, games_file{std::make_shared<std::ifstream>(
                                                                             games_path)},
              file_lock(std::make_shared<std::mutex>()), line_cnt(count_lines(*scores_file)),
              line_len_scores(line_length(*scores_file)), line_len_games(line_length(*games_file)) {}

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
};

// requires std::derived_from<T, torch::data::datasets::Dataset<T>>
template <typename T> struct ViewDataset : public torch::data::datasets::Dataset<ViewDataset<T>> {
        std::shared_ptr<T> dataset;

        int offset;
        int len;

        ViewDataset(std::shared_ptr<T> d, int o, int s) : dataset(d), offset(o), len(s) {}

        torch::data::Example<> get(size_t index) override { return dataset->get(index + offset); };

        torch::optional<size_t> size() const override { return len; };
};

template <typename T>
        requires std::derived_from<T, torch::data::datasets::Dataset<T>>
std::pair<ViewDataset<T>, ViewDataset<T>> split_dataset(std::shared_ptr<T> ds, float frac) {
        int N = ds->size().value();

        int o0 = 0;
        int n0 = (int)(frac * N);
        int o1 = n0;
        int n1 = N - n0;

        return std::make_pair(ViewDataset<T>(ds, o0, n0), ViewDataset<T>(ds, o1, n1));
}
