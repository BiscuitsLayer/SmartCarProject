#pragma once

#include <memory>
#include <string>

namespace App {

class VideoExporter {
public:
    VideoExporter(const std::string& config_path, int width, int height);
    ~VideoExporter();

    VideoExporter(const VideoExporter&) = delete;
    VideoExporter& operator=(const VideoExporter&) = delete;

    void CaptureFrame(float time);
    void Finish();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace App
