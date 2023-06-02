#pragma once

// STL
#include <ctime>
#include <iostream>
#include <random>

// Torch
#include <torch/torch.h>

// Constants
#include <constants/constants.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>
#include <dqn/net.hpp>
#include <dqn/env.hpp>
#include <dqn/replay_buffer.hpp>

namespace AppNN {

/*
    select_action - will select an action accordingly 
    to an epsilon greedy policy. Simply put, we’ll sometimes 
    use our model for choosing the action, and sometimes we’ll 
    just sample one uniformly. The probability of choosing a 
    random action will start at EPS_START and will decay 
    exponentially towards EPS_END. EPS_DECAY controls the rate 
    of the decay.
*/
const double EPS_START = 1;
const double EPS_END = 0.01;
const double EPS_DECAY = 1000;

const double GAMMA = 0.99;

class Trainer {
public:
    Trainer()
    : net(Net{App::APP_CAR_STATE_PARAMETERS_COUNT, App::APP_CAR_ACTIONS_COUNT}),
    optimizer(torch::optim::Adam{net->parameters(), torch::optim::AdamOptions(5e-3).betas(std::make_tuple(0.5, 0.5))}) {
        if (torch::cuda::is_available()){
            std::cout << "CUDA available! Running on GPU..." << std::endl;
            device = torch::Device(torch::kCUDA);
        }
        LoadLastModel();

        // for (auto& param : net->named_parameters()) {
        //     std::cout << param.key() << ": \n" << param.value() << std::endl;
        // }
        
        net->to(device);
    }

    void TrainingStep(float delta_time) {
        auto& context = App::Context::Get();

        optimizer.zero_grad();

        static int steps_count = 0;
        static State no_state = State{};
        no_state.fill(0.0);

        // Recalculate epsilon
        double sample = 1.0 * rand() / RAND_MAX;
        double eps_threshold = EPS_END + (EPS_START - EPS_END) * exp(-1.0 * steps_count / EPS_DECAY);
        ++steps_count;

        context.actions.fill(false);
        State state = context.state;
        Qvalues qvalues;
        Action action;

        // Agent's act
        if (sample > eps_threshold) {
            // net == policy network
            Qvalues qvalues = net->PredictForOne(state, device);

            int best_action_index = 0;
            float best_action_qvalue = -1.0 * std::numeric_limits<float>::infinity();
            for (int i = 0; i < qvalues.size(); ++i) {
                if (qvalues[i] > best_action_qvalue) {
                    best_action_index = i;
                    best_action_qvalue = qvalues[i];
                }
            }
            action = best_action_index;
            context.actions[best_action_index] = true;
        } else {
            int random_action_index = std::mt19937{std::random_device{}()}() % context.actions.size();
            action = random_action_index;
            context.actions[random_action_index] = true;
        }

        // Environment step
        env.Step(delta_time);
        State new_state = context.state;
        Reward reward = env.GetReward();
        if (env.IsDone()) {
            std::cout << "DONE!" << std::endl;
            context.keyboard_mode.value() = App::KeyboardMode::CAR_MOVEMENT;
            new_state = no_state;
        }
    
        Transition transition = std::make_tuple(state, qvalues, action, reward);
        buffer.Push(transition);

        // REPLAY

        auto batch = buffer.SampleQueue(APP_NN_BATCH_SIZE);

        std::array<State, APP_NN_BATCH_SIZE> states{};
        for (int i = 0; i < APP_NN_BATCH_SIZE; ++i) {
            states[i] = std::get<APP_NN_TRANSITION_OLD_STATE_INDEX>(batch[i]);
        }

        std::array<State, APP_NN_BATCH_SIZE> new_states{};
        for (int i = 0; i < APP_NN_BATCH_SIZE; ++i) {
            new_states[i] = std::get<APP_NN_TRANSITION_NEW_STATE_INDEX>(batch[i]);
        }

        auto predicted_qvalues = net->PredictForBatch(states, device);
        auto predicted_new_qvalues = net->PredictForBatch(new_states, device);

        // OPTIMIZER STEP

        std::array<State, APP_NN_BATCH_SIZE> X;
        std::array<Qvalues, APP_NN_BATCH_SIZE> Y;

        for (int i = 0; i < APP_NN_BATCH_SIZE; ++i) {
            auto transition = batch[i];
            auto action = std::get<APP_NN_TRANSITION_ACTION_INDEX>(transition);

            auto qvalues = predicted_qvalues[i];
            auto new_qvalues = predicted_new_qvalues[i];


            if (std::get<APP_NN_TRANSITION_NEW_STATE_INDEX>(transition) == no_state) {
                qvalues[action] = reward;
            } else {
                qvalues[action] = reward + GAMMA * std::max_element(new_qvalues.begin(), new_qvalues.end());
            }

            X[i] = std::get<APP_NN_TRANSITION_OLD_STATE_INDEX>(transition);
            Y[i] = qvalues;
        }

        torch::Tensor X_gpu = torch::from_blob(X.data(), {APP_NN_BATCH_SIZE, APP_CAR_STATE_PARAMETERS_COUNT}).clone().to(device);
        torch::Tensor Y_gpu = torch::from_blob(Y.data(), {APP_NN_BATCH_SIZE}).clone().to(device);

        torch::Tensor Y_pred = net->Forward(X_gpu).reshape({APP_NN_BATCH_SIZE});
		torch::Tensor loss = torch::mse_loss(Y_pred, Y);

    }

    void LoadLastModel() {
        std::string model_filename = App::GetLastSavedFileWithPrefix(APP_NN_MODELS_DIR, "model");
        if (!model_filename.empty()) {
            std::cout << "Saved model file found! Loading from: " << model_filename << std::endl;
            torch::load(net, model_filename);
        }
    }

    void SaveModel() {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
        std::string datetime{buffer};
        std::string model_filename = "model_" + datetime + ".pt";
        std::string model_path = APP_NN_MODELS_DIR + model_filename;

        // WARNING: torch raises an error if file doesn't exist, so we "touch" it manually first
        std::fstream output_file(model_path, std::ios::binary | std::ios::out);
        output_file.close();
	    torch::save(net, model_path);
    }
    
private:
    torch::Device device = torch::Device(torch::kCPU);
    Net net{nullptr};
    torch::optim::Adam optimizer;

    ReplayBuffer buffer{100'000};
    Environment env{};
};


	// // Let's learn f = 1*x + 2*y + 3
	// // Generating train dataset
	// std::vector<float> X_train {
	// 	1.0, 1.0,
	// 	1.0, 2.0,
	// 	2.0, 1.0
	// };
	// std::vector<float> Y_train {
	// 	6.0,
	// 	8.0,
	// 	7.0
	// };

	// // To gpu
	// torch::Tensor X_train_gpu = torch::from_blob(X_train.data(), {3, 2}).clone().to(device);
	// torch::Tensor Y_train_gpu = torch::from_blob(Y_train.data(), {3}).clone().to(device);
	// // std::cout << X_train_gpu << std::endl;
	// // std::cout << Y_train_gpu << std::endl;

	// // Generating test dataset
	// std::vector<float> X_test {
	// 	2.0, 2.0
	// };
	// std::vector<float> Y_test {
	// 	9.0
	// };

	// // To gpu
	// torch::Tensor X_test_gpu = torch::from_blob(X_test.data(), {1, 2}).clone().to(device);
	// torch::Tensor Y_test_gpu = torch::from_blob(Y_test.data(), {1}).clone().to(device);
	// // std::cout << X_test_gpu << std::endl;
	// // std::cout << Y_test_gpu << std::endl;

	// // Training loop
	// torch::optim::Adam optimizer{net->parameters(), torch::optim::AdamOptions(5e-3).betas(std::make_tuple(0.5, 0.5))};
	// std::cout << "=========== LEARNING ===========" << std::endl;
	// for (int i = 0; i < 1000; ++i) {
	// 	// std::cout << "=========== NEW EPOCH = " << i << " ===========" << std::endl;
	// 	optimizer.zero_grad();

	// 	torch::Tensor Y_pred = net->Forward(X_train_gpu).reshape({3});
	// 	torch::Tensor loss = torch::mse_loss(Y_pred, Y_train_gpu);

	// 	if (i % 100 == 0) {
	// 		std::cout << "LOSS: " << loss << std::endl;
	// 	}

	// 	loss.backward();
	// 	optimizer.step();
	// }

	// // Test
	// std::cout << "=========== TESTING ===========" << std::endl;
	// torch::Tensor Y_pred = net->Forward(X_test_gpu);
	// std::cout << Y_pred << std::endl;


} // namespace AppNN