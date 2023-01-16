#include "board.h"
#include "net.h"

#include <fstream>
#include <torch/torch.h>

int main() {
  CustomDataset dataset = load_dataset();

  auto net = std::make_shared<Net>();
  try {
    torch::load(net, "net.pt");
  } catch (...) {
  }

  constexpr int batch_size = 128;
  auto data_loader         = torch::data::make_data_loader(dataset.map(torch::data::transforms::Stack<>()), batch_size);

  torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(0.001));
  net->train();

  for (size_t epoch = 1; epoch <= 2000; ++epoch) {
    size_t batch_index = 0;
    float total_loss   = 0;
    for (auto& batch : *data_loader) {
      optimizer.zero_grad();
      torch::Tensor prediction = net->forward(batch.data);
      torch::Tensor loss       = torch::mse_loss(prediction, batch.target);
      float tmp_loss           = loss.item<float>();
      float tmp_pred           = prediction[0][0].item<float>();
      float tmp_target         = batch.target[0][0].item<float>();
      total_loss += loss.item<float>();
      loss.backward();
      optimizer.step();
      ++batch_index;
    }

    std::cout << "Epoch: " << epoch << " | Batches Completed: " << batch_index << " | Loss: " << total_loss * batch_size
              << std::endl;
    torch::save(net, "net.pt");
  }
}