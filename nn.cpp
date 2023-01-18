#include "board.h"
#include "net.h"

#include <fstream>
#include <torch/torch.h>

int main() {
        std::cout << "Loading dataset...\n";
        CustomDataset dataset = load_dataset();
        std::cout << "Loaded dataset\n";
        auto& train = dataset;
        //CustomDataset train, test;
        //dataset.split(0.5, train, test);

        auto net = std::make_shared<Net>();
        try {
                torch::load(net, "net.pt");
        } catch (...) {
        }

        constexpr int train_batch_size = 128;
        constexpr int test_batch_size = 1024;

        auto train_data_loader = torch::data::make_data_loader(train.map(torch::data::transforms::Stack<>()), train_batch_size);
        // auto test_data_loader  = torch::data::make_data_loader(test.map(torch::data::transforms::Stack<>()), test_batch_size);

        torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(0.001));
        net->train();

        std::cout << "Starting Training...\n";

        for (size_t epoch = 1; epoch <= 2000; ++epoch) {
                size_t batch_index = 0;
                float total_loss   = 0;
                for (auto& batch : *train_data_loader) {
                        optimizer.zero_grad();
                        torch::Tensor prediction = net->forward(batch.data);
                        torch::Tensor loss       = torch::mse_loss(prediction, batch.target);
                        double batch_loss = loss.item<float>();
                        total_loss += batch_loss;
                        loss.backward();
                        optimizer.step();
                        ++batch_index;

                        //std::cout << "Epoch: " << epoch << " | Batch: " << batch_index << " | Batch Loss: " << batch_loss << '\n';
                }

                /*float val_loss = 0;
                  for (auto& batch : *test_data_loader) {
                  torch::Tensor prediction = net->forward(batch.data);
                  torch::Tensor loss       = torch::mse_loss(prediction, batch.target);
                  val_loss += loss.item<float>();
                  }*/

                std::cout << "Epoch: " << epoch << " | Batches Completed: " << batch_index << " | Train Loss: " << total_loss * train_batch_size << '\n';
                //<< " | Val Loss: " << val_loss * test_batch_size << std::endl;
                torch::save(net, "net.pt");
        }
}
