#pragma once

#include "board.h"

#include <array>
#include <fstream>
#include <string>
#include <torch/torch.h>

torch::Tensor load_score(const std::string&);

torch::Tensor load_board(const std::string&);

struct Net : torch::nn::Module {
  Net();
  torch::Tensor forward(torch::Tensor x);

  torch::nn::Linear fc1{nullptr}, fc2{nullptr};
};

struct CustomDataset : public torch::data::datasets::Dataset<CustomDataset> {
  std::vector<torch::Tensor> games, scores;

  // Override get() function to return tensor at location index
  torch::data::Example<> get(size_t index) override;

  // Return the length of data
  torch::optional<size_t> size() const override;

  void split(double frac, CustomDataset &A, CustomDataset &B) const;
};

CustomDataset load_dataset();
