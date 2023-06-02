#pragma once

#include <vector>
#include <random>

#include <torch/torch.h>

// TODO FIX
#include "types.hpp"

class ReplayBuffer {
public:
    ReplayBuffer(int new_capacity)
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

    int Size() const {
        return buffer.size();
    }

    std::vector<Transition> SampleQueue(int batch_size) {
        std::vector<Transition> batch(batch_size);
        std::sample(
            buffer.begin(), buffer.end(),
            batch.begin(), batch_size,
            std::mt19937{std::random_device{}()}
        );
        return batch;
    }

private:
    std::deque<Transition> buffer;
    int capacity;
};