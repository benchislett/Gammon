#include "board.h"
#include "net.h"

#include <torch/torch.h>

using Dataset = LazyCustomDataset;

torch::Device device(torch::kCPU);

int main() {
        auto dataset = std::make_shared<Dataset>("tmp3/evals_partial.txt", "tmp3/games_partial.txt");

        auto net = std::make_shared<Net>();
        torch::load(net, "net.pt");

        net->to(device);

        net->eval();

        std::ofstream file;
        file.open("network_race_evals.txt");

        auto batch_size = 1;
        auto dataloader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            dataset->map(torch::data::transforms::Stack<>()), batch_size);

        std::vector<std::pair<double, int>> net_evals;

        int idx = 0;
        for (auto &batch : *dataloader) {
                auto pred = net->forward(batch.data);
                auto loss = torch::mse_loss(pred, batch.target, torch::Reduction::Sum);
                file << pred.item<float>() << ' ' << loss.item<float>() << '\n';
                net_evals.emplace_back(loss.item<float>(), idx++);
                if (idx % 100000 == 0) {
                        std::cout << idx << '\n';
                        // break;
                }
        }

        /*

        std::sort(net_evals.begin(), net_evals.end());

        std::vector<int> filtered_idxs;
        int filtered_amt = 0.01 * net_evals.size();
        for (int i = net_evals.size() - 1; i >= net_evals.size() - 1 - filtered_amt; i--) {
                filtered_idxs.push_back(net_evals[i].second);
        }
        std::sort(filtered_idxs.begin(), filtered_idxs.end());
        int max_idx = filtered_idxs.back();

        std::ifstream evals_in;
        evals_in.open("rollout_evals_large_partial.txt");

        std::ifstream positions_in;
        positions_in.open("rollout_positions_large_partial.txt");

        std::ofstream evals_out;
        evals_out.open("rollout_evals_large_partial_worst.txt");

        std::ofstream positions_out;
        positions_out.open("rollout_positions_large_partial_worst.txt");

        std::string eval_line;
        std::string board_line;
        int current = 0;
        for (int i = 0; i <= max_idx; i++) {
                std::getline(evals_in, eval_line);
                std::getline(positions_in, board_line);

                if (i == filtered_idxs[current]) {
                        evals_out << eval_line << '\n';
                        positions_out << board_line << '\n';
                        current++;
                }
        }
        */
}
