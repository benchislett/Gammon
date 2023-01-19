#include "board.h"
#include "net.h"

#include <fstream>
#include <torch/torch.h>

torch::Device device(torch::kCPU);

using Dataset = LazyCustomDataset;

int main() {
        Dataset dataset{"evals_16M.txt", "openings_16M.txt"};

        auto net = std::make_shared<Net>();
        /*try {
                torch::load(net, "net.pt");
        } catch (...) {
        }*/

        net->to(device);

        constexpr int train_batch_size = 128;

        auto train_data_loader = // torch::data::make_data_loader(train,
                                 // train_batch_size);
            torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
                dataset.map(torch::data::transforms::Stack<>()),
                torch::data::DataLoaderOptions().batch_size(train_batch_size).workers(128));
        // 3000 -> 8s
        torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(0.001));
        net->train();

        for (size_t epoch = 1; epoch <= 100; ++epoch) {
                size_t batch_index = 0;
                float total_loss = 0;
                for (auto &batch : *train_data_loader) {
                        optimizer.zero_grad();
                        torch::Tensor prediction = net->forward(batch.data);
                        torch::Tensor loss = torch::mse_loss(prediction, batch.target, torch::Reduction::Mean);
                        double batch_loss = loss.item<float>();
                        total_loss += batch_loss;
                        loss.backward();
                        optimizer.step();
                        ++batch_index;

                        if (batch_index % 100 == 0) {
                                std::cout << "Epoch " << epoch << " | Batch " << batch_index
                                          << " | Batch Loss (Mean): " << batch_loss << '\n';
                                torch::save(net, "net.pt");
                        }
                }

                std::cout << "Epoch: " << epoch << " | Batches Completed: " << batch_index
                          << " | Train Loss (Mean): " << total_loss / (float)(batch_index + 1) << '\n';
        }
}
