#pragma once

// Torch
#include <torch/torch.h>

namespace App {
    
class NetImpl : public torch::nn::Module {
public:
    NetImpl(int observations_count, int actions_count)
        : observations_count_(observations_count), actions_count_(actions_count)
        {
            seq->push_back(torch::nn::Linear(observations_count_, 64));
            seq->push_back(torch::nn::Linear(64, actions_count_));
            seq->push_back(torch::nn::ReLU(torch::nn::ReLUOptions(true)));

            seq = register_module("seq", seq);
        }

    torch::Tensor Forward(torch::Tensor in) {
        return seq->forward(in);
    }

private:
    torch::nn::Sequential seq{};

    int observations_count_;
    int actions_count_;
};

TORCH_MODULE(Net);

} // namespace App