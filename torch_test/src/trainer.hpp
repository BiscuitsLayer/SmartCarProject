// SOURCE: https://github.com/navneet-nmk/Pytorch-RL-CPP/blob/master/Trainer.cpp

#pragma once

#include <random>
#include <ctime>
#include <cstdlib>

#include <torch/torch.h>

#include "dqn.hpp"
#include "replay_buffer.hpp"

/*
    select_action - will select an action accordingly 
    to an epsilon greedy policy. Simply put, we’ll sometimes 
    use our model for choosing the action, and sometimes we’ll 
    just sample one uniformly. The probability of choosing a 
    random action will start at EPS_START and will decay 
    exponentially towards EPS_END. EPS_DECAY controls the rate 
    of the decay.
*/
const double EPS_START = 0.9;
const double EPS_END = 0.9;
const double EPS_DECAY = 1000;


struct Trainer {
    Trainer(int64_t observations_count, int64_t actions_count)
    : policy_network(observations_count, actions_count),
    target_network(observations_count, actions_count)
    {
        // initialize rng
        srand(time(0));

        // https://pytorch.org/tutorials/intermediate/reinforcement_q_learning.html#dqn-algorithm
        for (size_t i = 0; i < target_network.parameters().size(); i++) {
                target_network.parameters()[i].copy_(policy_network->parameters()[i]);
        }
    }

    void RunTrain() {
        // load env
        // load legal actions
        // 

    }

    Action SelectAction(State state) {
        static int steps_count = 0;

        // Recalculate epsilon
        double sample = rand() / RAND_MAX;
        double eps_threshold = EPS_END + (EPS_START - EPS_END) * exp(steps_count / EPS_DECAY);
        ++steps_count;

        if (sample > eps_threshold) {
            int64_t best_action_index = policy_network.SelectBestActionIndex(state);
            return legal_actions[best_action_index];
        } else {
            int64_t random_action_index = rand() % legal_actions.size();
            return legal_actions[random_action_index];
        }
    }

private:
    DQN policy_network;
    DQN target_network;

    torch::optim::Adam optimizer;
};