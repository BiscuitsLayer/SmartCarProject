// STL
#include <iostream>

// Torch
#pragma warning(push, 0)    
#include <torch/torch.h>
#pragma warning(pop)

#include "network.hpp"

const int NET_INPUT_SIZE = 50;
const int NET_OUTPUT_SIZE = 10;

const int NET_OBJECTS_COUNT = 7;

int main() {
	Net::NeuralNet net(NET_INPUT_SIZE, NET_OUTPUT_SIZE);
	// std::cout << net << std::endl;

	torch::Tensor x = torch::randn({ NET_OBJECTS_COUNT, NET_INPUT_SIZE });
	torch::Tensor out;

	out = net.Forward(x);
	std::cout << out << std::endl;
	return 0;
}