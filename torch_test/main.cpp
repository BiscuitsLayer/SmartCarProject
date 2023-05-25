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
	std::string model_path{"model.pt"};
	torch::serialize::InputArchive input_archive;
	if (CheckFileExists(model_path)) {
		std::cout << "Saved model file found! Loading..." << std::endl;
		input_archive.load_from(model_path);
	}

	// NetNamespace::NetImpl net;
	NetNamespace::NetImpl net(NET_INPUT_SIZE, NET_INNER_SIZE);

	// net.load(input_archive);


	// NetNamespace::NetImpl net(NET_INPUT_SIZE, NET_OUTPUT_SIZE);
	// std::cout << net << std::endl;

	// torch::Tensor x = torch::randn({ NET_OBJECTS_COUNT, NET_INPUT_SIZE });
	torch::Tensor x = torch::ones({ NET_OBJECTS_COUNT, NET_INPUT_SIZE });
	torch::Tensor out;

	out = net.Forward(x);
	std::cout << out << std::endl;

	// std::string model_path{"model.pt"};
	torch::serialize::OutputArchive output_archive;
	net.save(output_archive);
	output_archive.save_to(model_path);

	return 0;
}