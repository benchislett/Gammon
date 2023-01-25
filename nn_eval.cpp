#include "board.h"
#include "net.h"

#include <torch/torch.h>

int main() {
        CustomDataset dataset{"evals_160k.txt", "openings_160k.txt"};

        auto net = std::make_shared<Net>();
        torch::load(net, "net.pt");

        auto batch_size = 128;
        auto dataloader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset.map(torch::data::transforms::Stack<>()), batch_size);

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
