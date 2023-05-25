#pragma once

#include <iostream>
#include <torch/torch.h>
#include <torch/nn.h>

namespace NetNamespace {

class NetImpl : public torch::nn::Module {
public:
    NetImpl(int input_dims, int inner_dims)
        : input_dims_(input_dims), inner_dims_(inner_dims)
        // fc1(register_module("fc1", torch::nn::Linear(input_dims, input_dims))),
        // fc2(register_module("fc2", torch::nn::Linear(input_dims, inner_dims))),
        // out(register_module("out", torch::nn::Linear(input_dims, 1))) {}
        {
            seq->push_back(torch::nn::Linear(input_dims, 1));
            // seq->push_back(torch::nn::ReLU(torch::nn::ReLUOptions(true)));

            seq = register_module("seq", seq);
        }

    torch::Tensor Forward(torch::Tensor in) {
        return seq->forward(in);
    }

private:
    torch::nn::Sequential seq{};

    int input_dims_;
    int inner_dims_;
};

TORCH_MODULE(Net);

} // namespace NetNamespace