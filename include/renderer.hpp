#pragma once

#include "gl.hpp"
#include "camera.hpp"
#include "utility.hpp"

namespace gl {
    class Program final {
    public:
        Program() {
            id_ = glRUN(glCreateProgram);
        }

        void AttachShader(const IShader& shader_obj) {
            glRUN(glAttachShader, id_, shader_obj.Use());
        }

        void Link() {
            glRUN(glLinkProgram, id_);
            int success;
            glRUN(glGetProgramiv, id_, GL_LINK_STATUS, &success);
            if (!success) {
                throw glException("Program linking is failed");
            }
        }

        void Run() {
            glRUN(glUseProgram, id_);
        }
        
        int operator()() const {
            return id_;
        }
    private:
        int id_;
    }; // class Program


    class Shader final : public IShader {
    public:
        Shader(int type) {
            shader_ = glRUN(glCreateShader, type);
        }

        int Use() const override {
            return shader_;
        }

        void Compile(const std::string &file_name) override {
            std::ifstream stream(file_name);
            if (!stream.is_open()) {
                throw std::runtime_error("Shader file is not found");
            }

            std::string program_text = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
            const char *text = program_text.data();

            glShaderSource(shader_, 1, &text, NULL);
            glRUN(glCompileShader, shader_);

            int success;
            glRUN(glGetShaderiv, shader_, GL_COMPILE_STATUS, &success);
            if (!success) {
                throw glException("Failed to compile shader");
            }
        }
        
        ~Shader() {
            glRUN(glDeleteShader, shader_);
        }

    private:
        int shader_;
    }; // class Shader
    
    class Renderer final {
    public:
        Renderer() : program_() {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            std::vector<std::string> vs;
            std::vector<std::string> fs;
            file::FindFile("shaders", ".vs", vs);
            file::FindFile("shaders", ".fs", fs);

            gl::Shader vertex{GL_VERTEX_SHADER};
            gl::Shader fragment{GL_FRAGMENT_SHADER};    
            
            vertex.Compile(vs.front());
            fragment.Compile(fs.front());
    
            try {
                program_.AttachShader(vertex);
                program_.AttachShader(fragment);
                program_.Link();
            } catch (std::exception &ex) {
                throw ex;
            }
        }
        
        void Render(std::vector<std::unique_ptr<IMesh>> &scene, const Camera &camera) {
            glRUN(glClearColor, 0.4f, 0.4f, 0.4f, 1.0f);
            glRUN(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            program_.Run();
        
            glm::mat4 projection = camera.GetProjectionMatrix();
            glm::mat4 view = camera.GetViewMatrix();
            
            glm::vec3 lightPos = camera.GetTarget();
            glm::vec3 lightColor = glm::vec3{1.0f, 1.0f, 1.0f};

            SetUniform(program_(), "projection", projection);
            SetUniform(program_(), "view", view);

            SetUniform(program_(), "lightPos", lightPos);
            SetUniform(program_(), "lightColor", lightColor);
            
            for (auto &&it : scene) {
                it->Draw();
            }
        }

        ~Renderer() {}
    private:
        Program program_;
    }; // class Renderer
} // namespace gl