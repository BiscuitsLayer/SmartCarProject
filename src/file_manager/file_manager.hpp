#pragma once

#include <vector>
#include <filesystem>
#include <fstream>

namespace App {

/*
    Responsible for reloading file contents
    on their update in a system
*/
class FileManager {
public:
    class File {
    public:
        static enum class UpdateMode : bool {
            MONITOR,
            SKIP
        };

        std::string GetContents() const {
            is_updated_ = false;
            return contents_;
        }

        bool IsUpdated() const {
            return is_updated_;
        }

        void UpdateContents() {
            if (mode_ == UpdateMode::SKIP) {
                return;
            }

            is_updated_ = true;
            contents_.clear();
            ReadContents();
        }

    protected:
        /*
            Constructor is protected, so only FileManager::AddFile can create new files
        */
        File(const std::string& path, const UpdateMode mode)
        : path_(path), mode_(mode), is_updated_(true), contents_({}), last_modify_time_({}) {
            ReadContents();
        }

        friend std::shared_ptr<FileManager::File> AddFile(const std::string& path, const FileManager::File::UpdateMode mode);

    private:
        void ReadContents() {
            last_modify_time_ = std::filesystem::last_write_time(path_);

            std::ifstream source(path_, std::ios::binary);
            if (!source) {
                throw std::runtime_error("File::ReadContents(): can't open file: " + path_);
            }

            source.seekg(0, std::ios::end);
            size_t length = source.tellg();
            source.seekg(0, std::ios::beg);

            contents_.resize(length);
            source.read(contents_.data(), length);
        }

        std::string path_;
        UpdateMode mode_;
        mutable bool is_updated_;
        std::string contents_;
        std::filesystem::file_time_type last_modify_time_;

        friend class FileManager;
    };

public:
    FileManager() = default;

    std::shared_ptr<FileManager::File> AddFile(const std::string& path, const FileManager::File::UpdateMode mode = FileManager::File::UpdateMode::SKIP) {
        auto file_ptr = std::shared_ptr<FileManager::File>(new FileManager::File(path, mode));
        paths_to_files_.insert(std::make_pair(path, file_ptr));
        return paths_to_files_.at(path);
    }

    void UpdateAll() {
        for (auto&& [path, file] : paths_to_files_) {

            auto old_modify_time = file->last_modify_time_;
            auto new_modify_time = std::filesystem::last_write_time(file->path_);

            if (new_modify_time > old_modify_time) {
                file->UpdateContents();
            }
        }
    }

private:
    std::map<std::string, std::shared_ptr<FileManager::File>> paths_to_files_;
};

} // namespace App