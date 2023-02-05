#pragma once

#include "board.h"
#include "misc.h"

#include <array>
#include <string>
#include <torch/torch.h>

inline torch::Device device(torch::kCPU);

struct Net : torch::nn::Module {
        Net();
        torch::Tensor forward(torch::Tensor x);

        torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr}, fc4{nullptr};
};

struct DeepNet : torch::nn::Module {
        DeepNet();
        torch::Tensor forward(torch::Tensor x);

        torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr}, fc4{nullptr}, fc5{nullptr}, fc6{nullptr};
};
