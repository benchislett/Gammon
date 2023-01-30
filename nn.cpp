#include "board.h"
#include "net.h"

#include <fstream>
#include <torch/torch.h>

torch::Device device(torch::kCPU);

using Dataset = CustomDataset;

int main() {
        std::cout << "Loading dataset...\n";

        auto dataset = // std::make_shared<Dataset>("evals_160k.txt", "openings_160k.txt");
                       //     std::make_shared<ViewDataset<Dataset>>(
            std::make_shared<Dataset>("tmp3/evals_partial.txt", "tmp3/games_partial.txt");

        auto [train, test] = split_dataset(dataset, 0.9);

        std::cout << "Loaded dataset\n";

        auto net = std::make_shared<Net>();
        // try {
        //         torch::load(net, "net.pt");
        // } catch (...) {
        // }

        net->to(device);

        constexpr int batch_size = 1024;

        auto train_data_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            train.map(torch::data::transforms::Stack<>()),
            torch::data::DataLoaderOptions().batch_size(batch_size).workers(32));

        auto test_data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            test.map(torch::data::transforms::Stack<>()),
            torch::data::DataLoaderOptions().batch_size(batch_size).workers(32));

        torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(1e-3));
        net->train();

        std::cout << "Starting Training...\n";

        for (size_t epoch = 1; epoch <= 2000; ++epoch) {
                size_t train_batch_index = 0, test_batch_index = 0;
                float train_loss = 0, test_loss = 0;
                for (auto &batch : *train_data_loader) {
                        optimizer.zero_grad();
                        torch::Tensor prediction = net->forward(batch.data);
                        torch::Tensor loss = torch::mse_loss(prediction, batch.target, torch::Reduction::Mean);
                        double batch_loss = loss.item<float>();
                        train_loss += batch_loss;
                        loss.backward();
                        optimizer.step();
                        ++train_batch_index;

                        if (train_batch_index % 500 == 0) {
                                std::cout << "Epoch " << epoch << " | Batch " << train_batch_index << '\n';
                                //   << " | Batch Loss (Mean): " << batch_loss << '\n';
                        }
                }

                std::cout << "Epoch: " << epoch << " | Train Batches Completed: " << train_batch_index
                          << " | Train Loss (Mean): " << train_loss / (float)(train_batch_index + 1);

                for (auto &batch : *test_data_loader) {
                        torch::Tensor prediction = net->forward(batch.data);
                        torch::Tensor loss = torch::mse_loss(prediction, batch.target, torch::Reduction::Mean);
                        double batch_loss = loss.item<float>();
                        test_loss += batch_loss;
                        ++test_batch_index;
                }

                std::cout << " | Valid Batches Completed: " << test_batch_index
                          << " | Valid Loss (Mean): " << test_loss / (float)(test_batch_index + 1) << '\n';

                torch::save(net, "models/net" + std::to_string(epoch) + ".pt");
        }
}
