#pragma once

#include "GL/gl.hpp"
#include "GL/camera.hpp"
#include "utility.hpp"

namespace gl {
    constexpr size_t MAX_COUNT_ATTACHED_SHADERS = 2U;
    class Program final {
    public:
        Program() {
            id_ = glRUN(glCreateProgram);
        }

        ~Program() {
            DetachShaders();
            glRUN(glDeleteProgram, id_);
        }

        Program(const Program &other) {
            id_ = glRUN(glCreateProgram);
        }

        Program &operator=(const Program &other) {
            if (this != &other) {
                Program tmp = other;
                std::swap(*this, tmp);
            }

            return *this;
        }

        Program(Program &&other) noexcept {
            std::swap(id_, other.id_);
        }

        Program &operator=(Program &&other) noexcept {
            if (this != &other) {
                std::swap(id_, other.id_);
            }

            return *this;
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
        void DetachShaders() const {
            std::vector<GLuint> attached_shaders{MAX_COUNT_ATTACHED_SHADERS};
            int count = 0;
            glRUN(glGetAttachedShaders, id_, MAX_COUNT_ATTACHED_SHADERS, &count, attached_shaders.data());
            for (int i = 0; i < count; ++i)
                glRUN(glDetachShader, id_, attached_shaders[i]);
        }
        int id_;
    }; // class Program

    class Shader final : public IShader {
    public:
        Shader(int type) : type_(type) {
            shader_ = glRUN(glCreateShader, type_);
        }
        
        ~Shader() {
            glRUN(glDeleteShader, shader_);
        }

        Shader(const Shader &other) : type_(other.type_) {
            shader_ = glRUN(glCreateShader, type_);
        }

        Shader &operator=(const Shader &other) {
            if (this != &other) {
                Shader tmp = other;
                std::swap(*this, tmp);
            }

            return *this;
        }

        Shader(Shader &&other) noexcept : type_(std::move(other.type_)) {
            std::swap(shader_, other.shader_);
        }

        Shader &operator=(Shader &&other) noexcept {
            if (this != &other) {
                std::swap(shader_, other.shader_);
                std::swap(type_, other.type_);
            }

            return *this;
        }
        
        int Use() const override {
            return shader_;
        }
        
        void Compile(std::string_view file_name) override {
            std::ifstream stream(file_name.data());
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
        
    private:
        int shader_ = 0;
        int type_ = 0;
    }; // class Shader
    
    class Renderer final {
    public:
        Renderer() : program_() {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            auto &&vs = file::FindFile("shaders", ".vs");
            auto &&fs = file::FindFile("shaders", ".fs");
            
            gl::Shader vertex{GL_VERTEX_SHADER};
            gl::Shader fragment{GL_FRAGMENT_SHADER};
            
            vertex.Compile(vs.front());
            fragment.Compile(fs.front());
    
            program_.AttachShader(vertex);
            program_.AttachShader(fragment);
            program_.Link();
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

    private:
        Program program_;
    }; // class Renderer
} // namespace gl