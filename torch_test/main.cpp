// STL
#include <iostream>

// Torch
#pragma warning(push, 0)    
#include <torch/torch.h>
#pragma warning(pop)

#include "network.hpp"
#include "src/helpers.hpp"

const int NET_INPUT_SIZE = 3;
const int NET_INNER_SIZE = 2;

const int NET_OBJECTS_COUNT = 7;

int main() {
	NetNamespace::Net net(NET_INPUT_SIZE, NET_INNER_SIZE);

	std::string model_path{"model.pt"};
	if (CheckFileExists(model_path)) {
		std::cout << "Saved model file found! Loading..." << std::endl;
		torch::load(net, model_path);
	}

	torch::Tensor x = torch::ones({ NET_OBJECTS_COUNT, NET_INPUT_SIZE });
	torch::Tensor out;
	out = net->Forward(x);
	std::cout << out << std::endl;

	torch::save(net, model_path);
	return 0;
}