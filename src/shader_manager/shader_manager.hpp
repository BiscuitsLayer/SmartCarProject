#pragma once

#include <unordered_map>
#include <memory>

#include <helpers/helpers.hpp>

namespace App {

class ShaderManager {
public:
    ShaderManager() = default;

    void AddGraphicProgram(const std::string& name, std::shared_ptr<FileManager::File> vertex_file, std::shared_ptr<FileManager::File> fragment_file) {
        GL::Shader vertex_shader(GL::ShaderType::Vertex, vertex_file->GetContents());
        GL::Shader fragment_shader(GL::ShaderType::Fragment, fragment_file->GetContents());

        name_to_files_[name].push_back(vertex_file);
        name_to_files_[name].push_back(fragment_file);

        name_to_program_[name] = std::make_shared<GL::Program>(vertex_shader, fragment_shader);
    }

    void AddComputeProgram(const std::string& name, std::shared_ptr<FileManager::File> compute_file) {
        GL::Shader compute_shader(GL::ShaderType::Compute, compute_file->GetContents());

        name_to_files_[name].push_back(compute_file);

        name_to_program_[name] = std::make_shared<GL::Program>(compute_shader);
    }

    std::shared_ptr<GL::Program> GetProgram(const std::string& name) const {
        return name_to_program_.at(name);
    }

    void UpdateAll() {
        for (auto&& [name, files] : name_to_files_) {
            for (auto&& file : files) {
                if (file->IsUpdated()) {
                    // TODO: need to refactor that part (maybe the whole manager)
                    if (files.size() == 1) { // compute
                        try {
                            GL::Shader compute_shader(GL::ShaderType::Compute, files[0]->GetContents());
                            name_to_program_[name] = std::make_shared<GL::Program>(compute_shader);
                            std::cout << "Compute shader " << name << " contents successfully updated!" << std::endl;
                        } catch (GL::CompileException &e) {
                            std::cout << e.what() << std::endl;
                            std::cout << "Compute shader " << name << " contents not updated!" << std::endl;
                        }
                    }
                    else { // graphic
                        try {
                            GL::Shader vertex_shader(GL::ShaderType::Vertex, files[0]->GetContents());
                            GL::Shader fragment_shader(GL::ShaderType::Fragment, files[1]->GetContents());
                            name_to_program_[name] = std::make_shared<GL::Program>(vertex_shader, fragment_shader);
                            std::cout << "Graphic shader " << name << " contents successfully updated!" << std::endl;
                        } catch (GL::CompileException &e) {
                            std::cout << e.what() << std::endl;
                            std::cout << "Graphic shader " << name << " contents not updated!" << std::endl;
                        }
                    }
                    break;
                }
            }
        }
    }

    // Temporarily deprecated due to the reason 
    // we still need program pointer afterwards to bind arrays and buffers
    //
    // void UseProgram(const std::string& name) const {
    //     auto& context = App::Context::Get();
    //     auto& gl = context.gl->get();

    //     gl.UseProgram(*name_to_program_.at(name));
    // }

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<FileManager::File>>> name_to_files_;
    std::unordered_map<std::string, std::shared_ptr<GL::Program>> name_to_program_;

};

} // namespace App