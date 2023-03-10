#pragma once

#include <iostream>
#include <torch/torch.h>

namespace Net {

class NeuralNet {
public:
    NeuralNet(int input_dims, int output_dims)
    : input_dims_(input_dims), output_dims_(output_dims) {
        register_modele("fc1", fc1);
        register_modele("fc2", fc2);
        register_modele("out", out);
    }

    torch::Tensor Forward(torch::Tensor in) {
        in = torch::relu(fc1(in));
        in = torch::relu(fc2(in));
        in = out(in);
        return in;
    }

private:
    torch::nn::Linear fc1;
    torch::nn::Linear fc2;
    torch::nn::Linear out;
};

} // namespace Net