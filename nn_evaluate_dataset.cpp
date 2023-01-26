#include "board.h"
#include "net.h"

#include <torch/torch.h>

using Dataset = CustomDataset;

torch::Device device(torch::kCPU);

int main() {
        auto dataset = std::make_shared<Dataset>("evals_160k.txt", "openings_160k.txt");

        auto net = std::make_shared<Net>();
        torch::load(net, "net.pt");

        net->to(device);

        net->eval();

        std::ofstream file;
        file.open("network_evals_160k.txt");

        auto batch_size = 1;
        auto dataloader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset->map(torch::data::transforms::Stack<>()), batch_size);

        for (auto &batch : *dataloader) {
                auto pred = net->forward(batch.data);
                auto loss = torch::mse_loss(pred, batch.target, torch::Reduction::Sum);
                file << pred.item<float>() << ' ' << loss.item<float>() << '\n';
        }
}
