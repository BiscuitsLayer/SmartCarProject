// STL
#include <iostream>

// Torch
#pragma warning(push, 0)    
#include <torch/torch.h>
#pragma warning(pop)

#include "network.hpp"
#include "src/helpers.hpp"

const int NET_INPUT_SIZE = 2;
const int NET_INNER_SIZE = 0;

const int NET_OBJECTS_COUNT = 3;

int main() {
	torch::Device device = torch::Device(torch::kCPU);
	if (torch::cuda::is_available()){
		std::cout << "CUDA available! Running on GPU..." << std::endl;
		device = torch::Device(torch::kCUDA);
	}

	NetNamespace::Net net(NET_INPUT_SIZE, NET_INNER_SIZE);
	std::string model_path{"model.pt"};
	if (CheckFileExists(model_path)) {
		std::cout << "Saved model file found! Loading..." << std::endl;
		torch::load(net, model_path);
	}

	for (auto& param : net->named_parameters()) {
		std::cout << param.key() << ": \n" << param.value() << std::endl;
	}
	net->to(device);

	// Let's learn f = 1*x + 2*y + 3
	// Generating train dataset
	std::vector<float> X_train {
		1.0, 1.0,
		1.0, 2.0,
		2.0, 1.0
	};
	std::vector<float> Y_train {
		6.0,
		8.0,
		7.0
	};

	// To gpu
	torch::Tensor X_train_gpu = torch::from_blob(X_train.data(), {3, 2}).clone().to(device);
	torch::Tensor Y_train_gpu = torch::from_blob(Y_train.data(), {3}).clone().to(device);
	// std::cout << X_train_gpu << std::endl;
	// std::cout << Y_train_gpu << std::endl;

	// Generating test dataset
	std::vector<float> X_test {
		2.0, 2.0
	};
	std::vector<float> Y_test {
		9.0
	};

	// To gpu
	torch::Tensor X_test_gpu = torch::from_blob(X_test.data(), {1, 2}).clone().to(device);
	torch::Tensor Y_test_gpu = torch::from_blob(Y_test.data(), {1}).clone().to(device);
	// std::cout << X_test_gpu << std::endl;
	// std::cout << Y_test_gpu << std::endl;

	// Training loop
	torch::optim::Adam optimizer{net->parameters(), torch::optim::AdamOptions(5e-3).betas(std::make_tuple(0.5, 0.5))};
	std::cout << "=========== LEARNING ===========" << std::endl;
	for (int i = 0; i < 1000; ++i) {
		// std::cout << "=========== NEW EPOCH = " << i << " ===========" << std::endl;
		optimizer.zero_grad();

		torch::Tensor Y_pred = net->Forward(X_train_gpu).reshape({3});
		torch::Tensor loss = torch::mse_loss(Y_pred, Y_train_gpu);

		if (i % 100 == 0) {
			std::cout << "LOSS: " << loss << std::endl;
		}

		loss.backward();
		optimizer.step();
	}

	// Test
	std::cout << "=========== TESTING ===========" << std::endl;
	torch::Tensor Y_pred = net->Forward(X_test_gpu);
	std::cout << Y_pred << std::endl;

	// Save to file
	torch::save(net, model_path);
	return 0;
}