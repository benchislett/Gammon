#include "board.h"
#include "net.h"

#include <torch/torch.h>

using Dataset = CustomDataset;

torch::Device device(torch::kCPU);

int main() {
        auto dataset = std::make_shared<Dataset>("tmp3/evals_partial.txt", "tmp3/games_partial.txt");

        auto net = std::make_shared<Net>();
        torch::load(net, "net.pt");

        net->to(device);

        net->eval();

        auto batch_size = 128;
        auto dataloader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset->map(torch::data::transforms::Stack<>()), batch_size);

        double total_loss = 0.0;
        int batch_index = 0;
        for (auto &batch : *dataloader) {
                auto pred = net->forward(batch.data);
                auto loss = torch::mse_loss(pred, batch.target, torch::Reduction::Sum);
                total_loss += loss.item<float>();
                batch_index++;
        }

        std::cout << "Loss (Mean): " << total_loss / (float)(batch_size * (batch_index + 1)) << '\n';
}
