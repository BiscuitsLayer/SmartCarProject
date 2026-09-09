#include "video_exporter.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/error.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#include <nlohmann/json.hpp>

namespace App {
namespace {

using json = nlohmann::json;

void CheckAv(int result, const std::string& action) {
    if (result >= 0) {
        return;
    }
    std::array<char, AV_ERROR_MAX_STRING_SIZE> message{};
    av_strerror(result, message.data(), message.size());
    throw std::runtime_error(action + ": " + message.data());
}

std::vector<uint32_t> DecodeUtf8(const std::string& text) {
    std::vector<uint32_t> result;
    for (size_t i = 0; i < text.size();) {
        const auto first = static_cast<unsigned char>(text[i]);
        uint32_t codepoint = 0;
        size_t length = 0;
        if ((first & 0x80) == 0) {
            codepoint = first;
            length = 1;
        } else if ((first & 0xe0) == 0xc0) {
            codepoint = first & 0x1f;
            length = 2;
        } else if ((first & 0xf0) == 0xe0) {
            codepoint = first & 0x0f;
            length = 3;
        } else if ((first & 0xf8) == 0xf0) {
            codepoint = first & 0x07;
            length = 4;
        } else {
            throw std::runtime_error("Invalid UTF-8 title");
        }
        if (i + length > text.size()) {
            throw std::runtime_error("Truncated UTF-8 title");
        }
        for (size_t continuation = 1; continuation < length; ++continuation) {
            const auto value = static_cast<unsigned char>(text[i + continuation]);
            if ((value & 0xc0) != 0x80) {
                throw std::runtime_error("Invalid UTF-8 title");
            }
            codepoint = (codepoint << 6) | (value & 0x3f);
        }
        result.push_back(codepoint);
        i += length;
    }
    return result;
}

} // namespace

struct VideoExporter::Impl {
    struct Title {
        std::string text;
        float start = 0.0f;
        float end = 0.0f;
        float fade = 0.0f;
        int font_size = 48;
        float x = 0.5f;
        float y = 0.5f;
        std::array<uint8_t, 3> color{255, 255, 255};
    };

    Impl(const std::string& config_path, int frame_width, int frame_height)
        : width(frame_width), height(frame_height) {
        std::ifstream input(config_path);
        if (!input) {
            throw std::runtime_error("Cannot open cinematic config: " + config_path);
        }

        json config;
        input >> config;
        const auto& export_config = config.at("export");
        output_path = export_config.at("output").get<std::string>();
        codec_name = export_config.value("codec", "libx264");
        fps = export_config.value("fps", 30);
        duration = export_config.at("duration").get<double>();
        font_path = export_config.at("font").get<std::string>();
        if (export_config.contains("audio")) {
            const auto& audio_config = export_config["audio"];
            audio_path = audio_config.at("path").get<std::string>();
            audio_offset = audio_config.value("offset", 0.0);
            audio_codec_name = audio_config.value("codec", "aac");
            audio_bitrate = audio_config.value("bitrate", 192000);
        }

        for (const auto& value : config.value("titles", json::array())) {
            Title title;
            title.text = value.at("text").get<std::string>();
            title.start = value.at("start").get<float>();
            title.end = value.at("end").get<float>();
            title.fade = value.value("fade", 0.0f);
            title.font_size = value.value("font_size", 48);
            const auto position = value.value("position", json::array({0.5f, 0.5f}));
            title.x = position.at(0).get<float>();
            title.y = position.at(1).get<float>();
            const auto color = value.value("color", json::array({255, 255, 255}));
            for (size_t channel = 0; channel < title.color.size(); ++channel) {
                title.color[channel] = static_cast<uint8_t>(color.at(channel).get<int>());
            }
            titles.push_back(std::move(title));
        }

        if (width <= 0 || height <= 0 || fps <= 0 || duration <= 0.0) {
            throw std::runtime_error("Video export needs positive dimensions, fps and duration");
        }
        if ((width % 2) != 0 || (height % 2) != 0) {
            throw std::runtime_error("Video export dimensions must be even for YUV420P");
        }

        std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
        raw_pixels.resize(static_cast<size_t>(width) * height * 3);
        rgb_pixels.resize(raw_pixels.size());

        if (FT_Init_FreeType(&free_type) != 0 || FT_New_Face(free_type, font_path.c_str(), 0, &font_face) != 0) {
            throw std::runtime_error("Cannot load title font: " + font_path);
        }

        OpenOutput();
    }

    ~Impl() {
        if (!finished) {
            try {
                Finish();
            } catch (...) {
            }
        }
        if (audio_fifo) {
            av_audio_fifo_free(audio_fifo);
        }
        if (decoded_audio) {
            av_frame_free(&decoded_audio);
        }
        if (audio_frame) {
            av_frame_free(&audio_frame);
        }
        if (audio_packet) {
            av_packet_free(&audio_packet);
        }
        if (packet) {
            av_packet_free(&packet);
        }
        if (frame) {
            av_frame_free(&frame);
        }
        if (sws) {
            sws_freeContext(sws);
        }
        if (video_codec) {
            avcodec_free_context(&video_codec);
        }
        if (audio_codec) {
            avcodec_free_context(&audio_codec);
        }
        if (audio_decoder) {
            avcodec_free_context(&audio_decoder);
        }
        if (audio_input) {
            avformat_close_input(&audio_input);
        }
        if (output) {
            if (!(output->oformat->flags & AVFMT_NOFILE) && output->pb) {
                avio_closep(&output->pb);
            }
            avformat_free_context(output);
        }
        if (font_face) {
            FT_Done_Face(font_face);
        }
        if (free_type) {
            FT_Done_FreeType(free_type);
        }
    }

    void OpenOutput() {
        CheckAv(avformat_alloc_output_context2(&output, nullptr, nullptr, output_path.c_str()),
            "Create output video");

        const AVCodec* codec = avcodec_find_encoder_by_name(codec_name.c_str());
        if (!codec) {
            codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        }
        if (!codec) {
            throw std::runtime_error("No H.264 encoder is available");
        }

        video_stream = avformat_new_stream(output, nullptr);
        video_codec = avcodec_alloc_context3(codec);
        if (!video_stream || !video_codec) {
            throw std::runtime_error("Cannot allocate video encoder");
        }

        video_codec->codec_id = codec->id;
        video_codec->codec_type = AVMEDIA_TYPE_VIDEO;
        video_codec->width = width;
        video_codec->height = height;
        video_codec->pix_fmt = AV_PIX_FMT_YUV420P;
        video_codec->time_base = AVRational{1, fps};
        video_codec->framerate = AVRational{fps, 1};
        video_codec->bit_rate = 6'000'000;
        video_codec->gop_size = fps * 2;
        video_codec->max_b_frames = 2;
        if (output->oformat->flags & AVFMT_GLOBALHEADER) {
            video_codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
        av_opt_set(video_codec->priv_data, "preset", "medium", 0);
        av_opt_set(video_codec->priv_data, "crf", "18", 0);
        CheckAv(avcodec_open2(video_codec, codec, nullptr), "Open video encoder");
        CheckAv(avcodec_parameters_from_context(video_stream->codecpar, video_codec),
            "Copy video parameters");
        video_stream->time_base = video_codec->time_base;

        if (!audio_path.empty()) {
            CheckAv(avformat_open_input(&audio_input, audio_path.c_str(), nullptr, nullptr), "Open audio");
            CheckAv(avformat_find_stream_info(audio_input, nullptr), "Read audio stream");
            audio_input_index = av_find_best_stream(audio_input, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
            CheckAv(audio_input_index, "Find audio stream");
            const AVStream* input_stream = audio_input->streams[audio_input_index];

            const AVCodec* decoder = avcodec_find_decoder(input_stream->codecpar->codec_id);
            audio_decoder = avcodec_alloc_context3(decoder);
            if (!decoder || !audio_decoder) {
                throw std::runtime_error("Cannot allocate audio decoder");
            }
            CheckAv(avcodec_parameters_to_context(audio_decoder, input_stream->codecpar),
                "Copy audio decoder parameters");
            CheckAv(avcodec_open2(audio_decoder, decoder, nullptr), "Open audio decoder");

            const AVCodec* audio_encoder = avcodec_find_encoder_by_name(audio_codec_name.c_str());
            if (!audio_encoder) {
                audio_encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
            }
            audio_codec = avcodec_alloc_context3(audio_encoder);
            audio_stream = avformat_new_stream(output, nullptr);
            if (!audio_encoder || !audio_codec || !audio_stream) {
                throw std::runtime_error("Cannot allocate AAC encoder");
            }

            audio_codec->codec_id = audio_encoder->id;
            audio_codec->codec_type = AVMEDIA_TYPE_AUDIO;
            audio_codec->sample_rate = audio_decoder->sample_rate;
            audio_codec->sample_fmt = audio_encoder->sample_fmts
                ? audio_encoder->sample_fmts[0] : audio_decoder->sample_fmt;
            audio_codec->bit_rate = audio_bitrate;
            audio_codec->time_base = AVRational{1, audio_codec->sample_rate};
            CheckAv(av_channel_layout_copy(&audio_codec->ch_layout, &audio_decoder->ch_layout),
                "Copy audio channel layout");
            if (output->oformat->flags & AVFMT_GLOBALHEADER) {
                audio_codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            CheckAv(avcodec_open2(audio_codec, audio_encoder, nullptr), "Open AAC encoder");
            CheckAv(avcodec_parameters_from_context(audio_stream->codecpar, audio_codec),
                "Copy AAC parameters");
            audio_stream->codecpar->codec_tag = 0;
            audio_stream->time_base = audio_codec->time_base;

            if (audio_decoder->sample_fmt != audio_codec->sample_fmt ||
                audio_decoder->sample_rate != audio_codec->sample_rate ||
                av_channel_layout_compare(&audio_decoder->ch_layout, &audio_codec->ch_layout) != 0) {
                throw std::runtime_error("Audio source format needs resampling before AAC encoding");
            }

            audio_offset_pts = av_rescale_q(
                static_cast<int64_t>(std::llround(audio_offset * AV_TIME_BASE)),
                AV_TIME_BASE_Q, input_stream->time_base);
            CheckAv(avformat_seek_file(audio_input, audio_input_index, INT64_MIN,
                audio_offset_pts, INT64_MAX, 0), "Seek audio");
            avcodec_flush_buffers(audio_decoder);
        }

        if (!(output->oformat->flags & AVFMT_NOFILE)) {
            CheckAv(avio_open(&output->pb, output_path.c_str(), AVIO_FLAG_WRITE), "Open output file");
        }
        AVDictionary* mux_options = nullptr;
        av_dict_set(&mux_options, "movflags", "+faststart", 0);
        const int header_result = avformat_write_header(output, &mux_options);
        av_dict_free(&mux_options);
        CheckAv(header_result, "Write video header");

        frame = av_frame_alloc();
        packet = av_packet_alloc();
        if (!frame || !packet) {
            throw std::runtime_error("Cannot allocate video frame");
        }
        frame->format = video_codec->pix_fmt;
        frame->width = width;
        frame->height = height;
        CheckAv(av_frame_get_buffer(frame, 32), "Allocate video frame pixels");

        sws = sws_getContext(width, height, AV_PIX_FMT_RGB24,
            width, height, video_codec->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
        if (!sws) {
            throw std::runtime_error("Cannot create RGB to YUV converter");
        }

        if (audio_codec) {
            audio_packet = av_packet_alloc();
            decoded_audio = av_frame_alloc();
            audio_frame = av_frame_alloc();
            const int audio_frame_size = audio_codec->frame_size > 0 ? audio_codec->frame_size : 1024;
            audio_fifo = av_audio_fifo_alloc(audio_codec->sample_fmt,
                audio_codec->ch_layout.nb_channels, audio_frame_size * 2);
            if (!audio_packet || !decoded_audio || !audio_frame || !audio_fifo) {
                throw std::runtime_error("Cannot allocate AAC buffers");
            }
            audio_frame->format = audio_codec->sample_fmt;
            audio_frame->sample_rate = audio_codec->sample_rate;
            audio_frame->nb_samples = audio_frame_size;
            CheckAv(av_channel_layout_copy(&audio_frame->ch_layout, &audio_codec->ch_layout),
                "Copy AAC frame channel layout");
            CheckAv(av_frame_get_buffer(audio_frame, 0), "Allocate AAC frame samples");
        }
    }

    void CaptureFrame(float time) {
        glFinish();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadBuffer(GL_BACK);
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, raw_pixels.data());

        const size_t row_size = static_cast<size_t>(width) * 3;
        for (int y = 0; y < height; ++y) {
            const auto* source = raw_pixels.data() + static_cast<size_t>(height - y - 1) * row_size;
            auto* destination = rgb_pixels.data() + static_cast<size_t>(y) * row_size;
            std::copy(source, source + row_size, destination);
        }

        for (const auto& title : titles) {
            DrawTitle(title, time);
        }

        CheckAv(av_frame_make_writable(frame), "Prepare video frame");
        const uint8_t* source[] = {rgb_pixels.data()};
        const int source_stride[] = {width * 3};
        sws_scale(sws, source, source_stride, 0, height, frame->data, frame->linesize);
        frame->pts = frame_index++;
        EncodeFrame(frame);
        WriteAudioUntil(static_cast<double>(frame_index) / fps);
    }

    void EncodeFrame(AVFrame* source) {
        CheckAv(avcodec_send_frame(video_codec, source), "Send frame to encoder");
        while (true) {
            const int result = avcodec_receive_packet(video_codec, packet);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
                return;
            }
            CheckAv(result, "Encode video frame");
            av_packet_rescale_ts(packet, video_codec->time_base, video_stream->time_base);
            packet->stream_index = video_stream->index;
            CheckAv(av_interleaved_write_frame(output, packet), "Write video frame");
            av_packet_unref(packet);
        }
    }

    void WriteAudioUntil(double end_time) {
        if (!audio_codec) {
            return;
        }
        const int64_t target_samples = std::min(
            static_cast<int64_t>(std::ceil(end_time * audio_codec->sample_rate)),
            static_cast<int64_t>(std::llround(duration * audio_codec->sample_rate)));
        const int frame_size = audio_frame->nb_samples;
        while (audio_samples_encoded < target_samples) {
            FillAudioFifo(frame_size);
            CheckAv(av_frame_make_writable(audio_frame), "Prepare AAC frame");
            const int available = std::min(av_audio_fifo_size(audio_fifo), frame_size);
            if (available > 0) {
                const int read = av_audio_fifo_read(audio_fifo,
                    reinterpret_cast<void**>(audio_frame->extended_data), available);
                if (read != available) {
                    throw std::runtime_error("Cannot read decoded audio samples");
                }
            }
            if (available < frame_size) {
                CheckAv(av_samples_set_silence(audio_frame->extended_data, available,
                    frame_size - available, audio_codec->ch_layout.nb_channels,
                    audio_codec->sample_fmt), "Pad AAC frame");
            }
            audio_frame->pts = audio_samples_encoded;
            EncodeAudioFrame(audio_frame);
            audio_samples_encoded += frame_size;
        }
    }

    void FillAudioFifo(int minimum_samples) {
        while (av_audio_fifo_size(audio_fifo) < minimum_samples && !audio_source_exhausted) {
            const int decoded = avcodec_receive_frame(audio_decoder, decoded_audio);
            if (decoded == 0) {
                QueueDecodedAudio();
                av_frame_unref(decoded_audio);
                continue;
            }
            if (decoded == AVERROR_EOF) {
                audio_source_exhausted = true;
                break;
            }
            if (decoded != AVERROR(EAGAIN)) {
                CheckAv(decoded, "Decode audio frame");
            }

            if (audio_input_exhausted) {
                if (!audio_decoder_flushed) {
                    CheckAv(avcodec_send_packet(audio_decoder, nullptr), "Flush audio decoder");
                    audio_decoder_flushed = true;
                    continue;
                }
                audio_source_exhausted = true;
                break;
            }

            int read_result = 0;
            do {
                read_result = av_read_frame(audio_input, audio_packet);
                if (read_result < 0) {
                    audio_input_exhausted = true;
                    break;
                }
                if (audio_packet->stream_index != audio_input_index) {
                    av_packet_unref(audio_packet);
                }
            } while (audio_packet->stream_index != audio_input_index);
            if (audio_input_exhausted) {
                continue;
            }
            CheckAv(avcodec_send_packet(audio_decoder, audio_packet), "Send audio packet to decoder");
            av_packet_unref(audio_packet);
        }
    }

    void QueueDecodedAudio() {
        if (decoded_audio->format != audio_codec->sample_fmt ||
            decoded_audio->sample_rate != audio_codec->sample_rate ||
            av_channel_layout_compare(&decoded_audio->ch_layout, &audio_codec->ch_layout) != 0) {
            throw std::runtime_error("Decoded audio format changed during export");
        }

        int skip = 0;
        if (!audio_started && decoded_audio->best_effort_timestamp != AV_NOPTS_VALUE) {
            const auto time_base = audio_input->streams[audio_input_index]->time_base;
            const double frame_start = decoded_audio->best_effort_timestamp * av_q2d(time_base);
            skip = static_cast<int>(std::ceil(
                std::max(0.0, audio_offset - frame_start) * audio_codec->sample_rate));
            skip = std::min(skip, decoded_audio->nb_samples);
        }
        if (skip == decoded_audio->nb_samples) {
            return;
        }
        audio_started = true;

        const int sample_count = decoded_audio->nb_samples - skip;
        const int bytes_per_sample = av_get_bytes_per_sample(audio_codec->sample_fmt);
        const bool planar = av_sample_fmt_is_planar(audio_codec->sample_fmt) != 0;
        std::vector<void*> samples(static_cast<size_t>(
            planar ? audio_codec->ch_layout.nb_channels : 1));
        for (size_t channel = 0; channel < samples.size(); ++channel) {
            const int stride = planar ? bytes_per_sample
                                      : bytes_per_sample * audio_codec->ch_layout.nb_channels;
            samples[channel] = decoded_audio->extended_data[channel] + skip * stride;
        }
        CheckAv(av_audio_fifo_realloc(audio_fifo,
            av_audio_fifo_size(audio_fifo) + sample_count), "Grow decoded audio buffer");
        const int written = av_audio_fifo_write(audio_fifo, samples.data(), sample_count);
        if (written != sample_count) {
            throw std::runtime_error("Cannot buffer decoded audio samples");
        }
    }

    void EncodeAudioFrame(AVFrame* source) {
        CheckAv(avcodec_send_frame(audio_codec, source), "Send frame to AAC encoder");
        while (true) {
            const int result = avcodec_receive_packet(audio_codec, audio_packet);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
                return;
            }
            CheckAv(result, "Encode AAC frame");
            av_packet_rescale_ts(audio_packet, audio_codec->time_base, audio_stream->time_base);
            audio_packet->stream_index = audio_stream->index;
            audio_packet->pos = -1;
            CheckAv(av_interleaved_write_frame(output, audio_packet), "Write AAC frame");
            av_packet_unref(audio_packet);
        }
    }

    void DrawTitle(const Title& title, float time) {
        if (time < title.start || time >= title.end) {
            return;
        }
        float opacity = 1.0f;
        if (title.fade > 0.0f) {
            opacity = std::min({1.0f, (time - title.start) / title.fade,
                (title.end - time) / title.fade});
        }
        opacity = std::clamp(opacity, 0.0f, 1.0f);

        FT_Set_Pixel_Sizes(font_face, 0, title.font_size);
        const auto codepoints = DecodeUtf8(title.text);
        int text_width = 0;
        for (const auto codepoint : codepoints) {
            if (FT_Load_Char(font_face, codepoint, FT_LOAD_DEFAULT) == 0) {
                text_width += static_cast<int>(font_face->glyph->advance.x >> 6);
            }
        }

        const int start_x = static_cast<int>(title.x * width) - text_width / 2;
        const int baseline_y = static_cast<int>(title.y * height) + title.font_size / 3;
        for (int offset_y = -2; offset_y <= 2; ++offset_y) {
            for (int offset_x = -2; offset_x <= 2; ++offset_x) {
                if (offset_x * offset_x + offset_y * offset_y <= 4) {
                    DrawGlyphs(codepoints, start_x + offset_x, baseline_y + offset_y,
                        {12, 16, 26}, opacity * 0.85f);
                }
            }
        }
        DrawGlyphs(codepoints, start_x + 3, baseline_y + 4, {0, 0, 0}, opacity * 0.45f);
        DrawGlyphs(codepoints, start_x, baseline_y, title.color, opacity);
    }

    void DrawGlyphs(const std::vector<uint32_t>& codepoints, int start_x, int baseline_y,
        const std::array<uint8_t, 3>& color, float opacity) {
        int pen_x = start_x;
        for (const auto codepoint : codepoints) {
            if (FT_Load_Char(font_face, codepoint, FT_LOAD_RENDER) != 0) {
                continue;
            }
            const FT_GlyphSlot glyph = font_face->glyph;
            const int glyph_x = pen_x + glyph->bitmap_left;
            const int glyph_y = baseline_y - glyph->bitmap_top;
            for (unsigned int y = 0; y < glyph->bitmap.rows; ++y) {
                for (unsigned int x = 0; x < glyph->bitmap.width; ++x) {
                    const int pixel_x = glyph_x + static_cast<int>(x);
                    const int pixel_y = glyph_y + static_cast<int>(y);
                    if (pixel_x < 0 || pixel_x >= width || pixel_y < 0 || pixel_y >= height) {
                        continue;
                    }
                    const float alpha = opacity * glyph->bitmap.buffer[y * glyph->bitmap.pitch + x] / 255.0f;
                    auto* pixel = rgb_pixels.data() + (static_cast<size_t>(pixel_y) * width + pixel_x) * 3;
                    for (size_t channel = 0; channel < 3; ++channel) {
                        pixel[channel] = static_cast<uint8_t>(
                            pixel[channel] * (1.0f - alpha) + color[channel] * alpha);
                    }
                }
            }
            pen_x += static_cast<int>(glyph->advance.x >> 6);
        }
    }

    void Finish() {
        if (finished) {
            return;
        }
        EncodeFrame(nullptr);
        WriteAudioUntil(duration);
        if (audio_codec) {
            EncodeAudioFrame(nullptr);
        }
        CheckAv(av_write_trailer(output), "Finish output video");
        finished = true;
    }

    int width;
    int height;
    int fps = 30;
    int64_t frame_index = 0;
    int64_t audio_samples_encoded = 0;
    double duration = 0.0;
    double audio_offset = 0.0;
    int64_t audio_offset_pts = 0;
    bool finished = false;
    bool audio_started = false;
    bool audio_input_exhausted = false;
    bool audio_decoder_flushed = false;
    bool audio_source_exhausted = false;
    int64_t audio_bitrate = 192000;
    std::string output_path;
    std::string codec_name;
    std::string audio_path;
    std::string audio_codec_name = "aac";
    std::string font_path;
    std::vector<Title> titles;
    std::vector<uint8_t> raw_pixels;
    std::vector<uint8_t> rgb_pixels;
    FT_Library free_type = nullptr;
    FT_Face font_face = nullptr;
    AVFormatContext* output = nullptr;
    AVFormatContext* audio_input = nullptr;
    AVCodecContext* video_codec = nullptr;
    AVCodecContext* audio_decoder = nullptr;
    AVCodecContext* audio_codec = nullptr;
    AVStream* video_stream = nullptr;
    AVStream* audio_stream = nullptr;
    int audio_input_index = -1;
    AVFrame* frame = nullptr;
    AVFrame* decoded_audio = nullptr;
    AVFrame* audio_frame = nullptr;
    AVPacket* packet = nullptr;
    AVPacket* audio_packet = nullptr;
    AVAudioFifo* audio_fifo = nullptr;
    SwsContext* sws = nullptr;
};

VideoExporter::VideoExporter(const std::string& config_path, int width, int height)
    : impl_(std::make_unique<Impl>(config_path, width, height)) {}

VideoExporter::~VideoExporter() = default;

void VideoExporter::CaptureFrame(float time) {
    impl_->CaptureFrame(time);
}

void VideoExporter::Finish() {
    impl_->Finish();
}

} // namespace App
