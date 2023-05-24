#pragma once

#include <iostream>
#include <torch/torch.h>
#include <torch/nn.h>

namespace Net {

class NeuralNet : torch::nn::Module {
public:
    NeuralNet(int input_dims, int output_dims)
        : input_dims_(input_dims), output_dims_(output_dims),
        fc1(register_module("fc1", torch::nn::Linear(input_dims, input_dims))),
        fc2(register_module("fc2", torch::nn::Linear(input_dims, output_dims))),
        out(register_module("out", torch::nn::Linear(output_dims, 1))) {}

    torch::Tensor Forward(torch::Tensor in) {
        in = torch::relu(fc1(in));
        in = torch::relu(fc2(in));
        in = out(in);
        return in;
    }

private:
    torch::nn::Linear fc1 = nullptr;
    torch::nn::Linear fc2 = nullptr;
    torch::nn::Linear out = nullptr;

    int input_dims_;
    int output_dims_;
};

} // namespace Net