#pragma once

#include <vector>

#include <torch/torch.h>

using Action = torch::tensor;
using State = torch::tensor;
using Reward = torch::tensor;

using Transition = std::tuple<State, Action, State, Reward>;

class ReplayBuffer {
public:
    ReplayBuffer(int64_t new_capacity)
    : capacity(new_capacity) {}

    void Push(Transition transition) {
        if (buffer.size() < capacity) {
            buffer.push_back(transition);
        } else {
            while (buffer.size() >= capacity) {
                buffer.pop_front();
            }
            buffer.push_back(transition);
        }
    }

    int64_t Size() const {
        return buffer.size();
    }

    std::vector<Transition> SampleQueue(int64_t batch_size) {
        std::vector<Transition> batch(batch_size);
        std::sample(
            buffer.begin(), buffer.end(),
            batch.begin(), batch.end(),
            std::mt19937{std::random_device{}()}
        );
        return batch;
    }

private:
    std::deque<Transition> buffer;
    int64_t capacity;
};