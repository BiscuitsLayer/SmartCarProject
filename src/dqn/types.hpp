#pragma once

// STL
#include <array>

// Constants
#include <constants/constants.hpp>

#include "batch.hpp"

using Reward = int;
using Qvalues = std::array<float, App::APP_CAR_ACTIONS_COUNT>;
using Action = int;
using State = std::array<float, App::APP_CAR_STATE_PARAMETERS_COUNT>;

using Transition = std::tuple<State, Action, State, Reward>;
constexpr int APP_NN_TRANSITION_OLD_STATE_INDEX = 0;
constexpr int APP_NN_TRANSITION_ACTION_INDEX = 1;
constexpr int APP_NN_TRANSITION_NEW_STATE_INDEX = 2;
constexpr int APP_NN_TRANSITION_REWARD_INDEX = 3;

constexpr int APP_NN_BATCH_SIZE = 64;


std::array<float, APP_NN_BATCH_SIZE * App::APP_CAR_STATE_PARAMETERS_COUNT> StatesBatchToRaw(std::array<State, APP_NN_BATCH_SIZE> states_batch) {
    std::array<float, APP_NN_BATCH_SIZE * App::APP_CAR_STATE_PARAMETERS_COUNT> ans;
    for (int i = 0; i < states_batch.size(); ++i) {
        for (int j = 0; j < APP_NN_BATCH_SIZE; ++j) {
            ans[i * APP_NN_BATCH_SIZE + j] == states_batch[i][j];
        }
        // TODO: Slow, change with std::copy
        // std::copy(states_batch[i].begin(), states_batch[i].end(), ans.data() + i * APP_NN_BATCH_SIZE);
    }
    

    return ans;
}