#pragma once

// STL
#include <ctime>
#include <iostream>

// Torch
#include <torch/torch.h>

// Constants
#include <constants/constants.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>
#include <dqn/dqn.hpp>

namespace App {

class Trainer{
public:
    Trainer() {
        if (torch::cuda::is_available()){
            std::cout << "CUDA available! Running on GPU..." << std::endl;
            device = torch::Device(torch::kCUDA);
        }
        net = Net{APP_RAY_INTERSECTOR_RAYS_COUNT, APP_CAR_ACTIONS_COUNT};
        LoadLastModel();

        // for (auto& param : net->named_parameters()) {
        //     std::cout << param.key() << ": \n" << param.value() << std::endl;
        // }
        net->to(device);
    }

    void Train() {
        auto& context = App::Context::Get();
        // torch::Tensor X_train_gpu = torch::from_blob(.data(), {3, 2}).clone().to(device);
	    // torch::Tensor Y_train_gpu = torch::from_blob(Y_train.data(), {3}).clone().to(device);
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


} // namespace App