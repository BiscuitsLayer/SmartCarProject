#pragma once

// Sources
#include <constants/constants.hpp>

namespace App {

std::string ReadFileData(std::string filename, bool debug_dump = true) {
    std::ifstream source{filename};
    if (!source) {
        throw std::runtime_error("ReadFileData: can't open file: " + filename);
    }

    source.seekg(0, std::ios::end);
    size_t length = source.tellg();
    source.seekg(0, std::ios::beg);

    std::string buffer(length, 0);
    source.read(buffer.data(), length);

	if (debug_dump) {
		std::cout << buffer << std::endl;
	}
    return buffer;
}

void LimitMaxFps() {
    if (APP_MAX_FPS_MODE_ON) {
		static std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
		static std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

		const float max_elapsed_time = 1000.0f / APP_INIT_MAX_FPS;

		a = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> work_time = a - b;
		if (work_time.count() < max_elapsed_time) {
			std::chrono::duration<double, std::milli> delta_ms(max_elapsed_time - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}
		b = std::chrono::system_clock::now();
	}
}

} // namespace App