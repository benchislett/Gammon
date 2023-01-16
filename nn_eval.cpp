#include "board.h"
#include "net.h"

#include <torch/torch.h>

int main() {
  CustomDataset dataset = load_dataset();

  auto net = std::make_shared<Net>();
  torch::load(net, "net.pt");

  float total_loss = 0.0;
  for (int i = 0; i < dataset.size(); i++) {
    auto [data, label] = dataset.get(i);
    float checkval     = label.item<float>();

    auto pred = net->forward(data.reshape({1, 200}));
    auto loss = torch::mse_loss(pred, label.reshape({1, 1}));

    total_loss += loss.item<float>();
  }

  std::cout << "Total loss: " << total_loss << '\n';
}