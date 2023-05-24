#pragma once

#include <torch/torch.h>

class DQN : torch::nn::Module {
public:
    DQN(int64_t observations_count, int64_t actions_count)
        : first(register_module("first", torch::nn::Linear(observations_count, 128))),
        second(register_module("second", torch::nn::Linear(128, 64))),
        third(register_module("third", torch::nn::Linear(64, actions_count))) {}

    torch::Tensor Forward(torch::Tensor in) {
        in = torch::relu(first(in));
        in = torch::relu(second(in));
        return third(in);
    }

    int64_t SelectBestActionIndex(torch::Tensor state) {
        torch::Tensor q_values = Forward(state);
        torch::Tensor action_tensor = std::get<1>(q_values.max(1));
        int64_t best_action_index = action_tensor[0].item<int64_t>();
        return best_action_index;
    }

private:
    torch::nn::Linear first{nullptr}, second{nullptr}, third{nullptr};
};