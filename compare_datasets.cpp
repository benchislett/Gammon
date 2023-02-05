#include "board.h"
#include "dataset.h"
#include "net.h"

#include <torch/torch.h>

using Dataset = CustomDataset;

int main() {
        auto dataset_source = std::make_shared<Dataset>("tmp3/evals_partial.txt", "tmp3/games_partial.txt");
        auto dataset_target = std::make_shared<Dataset>("tmp5/evals_partial.txt", "tmp5/games_partial.txt");

        auto batch_size = 128;
        auto dataloader_src = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset_source->map(torch::data::transforms::Stack<>()), batch_size);
        auto dataloader_trg = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset_target->map(torch::data::transforms::Stack<>()), batch_size);

        double total_loss = 0.0;
        int batch_index = 0;
        for (auto it_src = dataloader_src->begin(), it_trg = dataloader_trg->begin();
             it_src != dataloader_src->end() && it_trg != dataloader_trg->end();
             it_src = std::next(it_src), it_trg = std::next(it_trg)) {
                auto loss = torch::mse_loss(it_src->target, it_trg->target, torch::Reduction::Sum);
                total_loss += loss.item<float>();
                batch_index++;
        }

        total_loss = total_loss / (float)(batch_size * (batch_index + 1));

        std::cout << "Mean Loss: " << total_loss << '\n';
}
