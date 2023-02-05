#include "net.h"

#include "misc.h"

Net::Net() {
        fc1 = register_module("input", torch::nn::Linear(200, 256));
        fc2 = register_module("h1", torch::nn::Linear(256, 256));
        fc3 = register_module("h2", torch::nn::Linear(256, 256));
        fc4 = register_module("output", torch::nn::Linear(256, 1));

        torch::nn::init::normal_(fc1->bias);
        torch::nn::init::normal_(fc2->bias);
        torch::nn::init::normal_(fc3->bias);
        torch::nn::init::normal_(fc4->bias);
}

torch::Tensor Net::forward(torch::Tensor x) {
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 200})));
        // x = torch::dropout(x, 0.5, is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::relu(fc3->forward(x));
        x = torch::sigmoid(fc4->forward(x));
        return x;
}

DeepNet::DeepNet() {
        fc1 = register_module("input", torch::nn::Linear(200, 512));
        fc2 = register_module("h1", torch::nn::Linear(512, 512));
        fc3 = register_module("h2", torch::nn::Linear(512, 512));
        fc4 = register_module("h3", torch::nn::Linear(512, 512));
        fc5 = register_module("h4", torch::nn::Linear(512, 32));
        fc6 = register_module("output", torch::nn::Linear(32, 1));

        torch::nn::init::normal_(fc1->bias);
        torch::nn::init::normal_(fc2->bias);
        torch::nn::init::normal_(fc3->bias);
        torch::nn::init::normal_(fc4->bias);
        torch::nn::init::normal_(fc5->bias);
        torch::nn::init::normal_(fc6->bias);
}

torch::Tensor DeepNet::forward(torch::Tensor x) {
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 200})));
        // x = torch::dropout(x, 0.5, is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::relu(fc3->forward(x));
        x = torch::relu(fc4->forward(x));
        x = torch::relu(fc5->forward(x));
        x = torch::sigmoid(fc6->forward(x));
        return x;
}
