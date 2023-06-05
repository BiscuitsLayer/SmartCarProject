#pragma once

// Torch
#include <torch/torch.h>

// TODO FIX
#include "types.hpp"

namespace AppNN {
    
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

/*
    Batch<Qvalues, APP_NN_BATCH_SIZE> PredictForBatch(Batch<State, APP_NN_BATCH_SIZE> states, torch::Device device) {
        auto raw = StatesBatchToRaw(states);
        torch::Tensor states_gpu = torch::from_blob(raw.data(), {APP_NN_BATCH_SIZE, states[0].size()}).clone().to(device);

        torch::Tensor q_values = Forward(states_gpu);

        Batch<Qvalues, APP_NN_BATCH_SIZE> qvalues;
        for (int i = 0; i < APP_NN_BATCH_SIZE; ++i) {
            for (int j = 0; j < App::APP_CAR_ACTIONS_COUNT; ++j) {
                qvalues[i][j] = q_values[i][j].item<float>();
            }
        }

        return qvalues;
        // torch::Tensor actions_tensor = std::get<1>(q_values.max(1));

    //     Batch<Action, APP_NN_BATCH_SIZE> best_action_indices;
    //     for (int i = 0; i < APP_NN_BATCH_SIZE; ++i) {
    //         best_action_indices[i] = actions_tensor[i].item<int>();
    //     }
    //     // TODO: check that
    //     // long& best_action_indices[APP_NN_BATCH_SIZE] = actions_tensor.data<long>();
    //     return best_action_indices;
    }
*/

    Qvalues PredictForOne(State state, torch::Device device) {
        torch::Tensor state_gpu = torch::from_blob(state.data(), {1, state.size()}).clone().to(device);

        torch::Tensor q_values = Forward(state_gpu);

        Qvalues qvalues;
        for (int i = 0; i < App::APP_CAR_ACTIONS_COUNT; ++i) {
            qvalues[i] = q_values[0][i].item<float>();
        }

        return qvalues;
    //     torch::Tensor action_tensor = std::get<1>(q_value.max(1));
    //     long best_action_index = action_tensor[0].item<int>();
    //     return best_action_index;
    }

    torch::Tensor PredictForOneSupervised(State state, torch::Device device) {
        torch::Tensor state_gpu = torch::from_blob(state.data(), {1, state.size()}).clone().to(device);
        torch::Tensor q_values = Forward(state_gpu);
        return q_values;
    }

private:
    torch::nn::Sequential seq{};

    int observations_count_;
    int actions_count_;
};

TORCH_MODULE(Net);

} // namespace AppNN