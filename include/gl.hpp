#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include <exception>
#include <stdexcept>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include <format>
#include <type_traits>

namespace gl {

    class glException : public std::exception {
    public:
        explicit glException(const std::string &message) : message_(message) {}

        const char *what() const noexcept override {
            return message_.c_str();
        }
    private:
        std::string message_;
    }; // class glException

    #define glRUN(func, ...) \
        glRun(__LINE__, __FILE__, #func, func __VA_OPT__(,) __VA_ARGS__)

    void glCheckError(int line, std::string_view file_name, std::string_view func_name) {
        std::cerr << "S";
        if (glGetError() != GL_NO_ERROR) {
            std::string mes = std::format("OpenGL lib error in '{}' file on line '{}', when calling the function '{}'.\n", 
                file_name, line, func_name);

            throw glException(mes);
        }
        std::cerr << "C";
    }

    template <typename FuncT, typename... Args>
    auto glRun(int line, std::string_view file_name, std::string_view func_name, 
    FuncT func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        if constexpr (std::is_void_v<decltype(func(std::forward<Args>(args)...))>) {
            func(std::forward<Args>(args)...);
            glCheckError(line, file_name, func_name);
        } else {
            auto result = func(std::forward<Args>(args)...);
            glCheckError(line, file_name, func_name);
            return result;
        }
    }

    class Renderer;
    class Camera;

    template <typename T>
    struct UniformSeter final {};

    template <> struct UniformSeter<glm::mat4> {
        static void Set(unsigned int shader_id, const std::string &name, const glm::mat4 &value) {
            auto loc = glRUN(glGetUniformLocation, shader_id, name.data());
            glRUN(glUniformMatrix4fv, loc, 1, GL_FALSE, glm::value_ptr(value));
        }
    };

    template <> struct UniformSeter<glm::vec3> {
        static void Set(unsigned int shader_id, const std::string &name, const glm::vec3 &value) {
            auto loc = glRUN(glGetUniformLocation, shader_id, name.data());
            glRUN(glUniform3fv, loc, 1, glm::value_ptr(value));
        }
    };

    class IMesh {
    public:
        virtual ~IMesh() {}
        virtual void Draw() = 0;
    }; // class IMesh

    class IShader {
    public:
        virtual ~IShader() {}
        virtual int Use() const = 0;
        virtual void Compile(const std::string &file_name) = 0;
    }; // class IShader

    class IEventHandler {
    public:
        virtual void UpdateEvent() = 0;
        ~IEventHandler() {}
    }; // class IEventHandler

    class Scene {
    public:
        void AddObject(std::unique_ptr<IMesh> &&obj) {
            objects_.push_back(std::move(obj));
        }
    
        const std::vector<std::unique_ptr<IMesh>> &GetObjects() const {
            return objects_;
        }
    
    private:
        std::vector<std::unique_ptr<IMesh>> objects_;
    };
        
    class Window final {
    public:
        Window(int widht = 800, int height = 600, std::string title = "") {
            glfwInit();
            window_ = glfwCreateWindow(widht, height, title.data(), NULL, NULL);
            if (window_ == nullptr) {
                throw glException("Failed to create a window");
            }

            glfwMakeContextCurrent(window_);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                throw glException("Failed to initialize GLAD");
            }
        }

        ~Window() {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }

        void SetEventHandler(std::unique_ptr<IEventHandler> &&handler) {
            handler_ = std::move(handler);
        }    

        GLFWwindow *Get() { return window_; }
        
        void DrawFrames(Renderer &renderer, Scene &scene, Camera &camera);
    
    private:
        bool IsShouldBeClosed() {
            return glRUN(glfwWindowShouldClose, window_);
        }
        
        GLFWwindow* window_;
        std::unique_ptr<IEventHandler> handler_;
    }; // class Window

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
                throw glException("program linking is failed");
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
    
    class TriangleMesh final : public IMesh {
    public:
        TriangleMesh(size_t elem_count) : elem_count_(elem_count) {}

        void LoadData(const std::vector<float> &vertices) {
            glRUN(glGenVertexArrays, 1, &VAO_);
            glRUN(glBindVertexArray, VAO_);

            glRUN(glGenBuffers, 1, &VBO_);
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, VBO_);

            glRUN(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            
            glRUN(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glRUN(glEnableVertexAttribArray, 0);
            
            glRUN(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 3));
            glRUN(glEnableVertexAttribArray, 1);

            glRUN(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 6));
            glRUN(glEnableVertexAttribArray, 2);

            glRUN(glBindBuffer, GL_ARRAY_BUFFER, 0);
            glRUN(glBindVertexArray, 0);
        }

        void Draw() override {
            glRUN(glBindVertexArray, VAO_);
            glRUN(glDrawArrays, GL_TRIANGLES, 0, elem_count_);
        }
    private:
        size_t elem_count_;
        unsigned int VBO_, VAO_, EBO_;  
    }; // class Mesh

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
                throw std::runtime_error("shader file is not found");
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
            glDeleteShader(shader_);
        }

    private:
        int shader_;
    }; // class Shader

    class Camera final {
    public:
        Camera() {
            UpdatePosition();
        }

        Camera(glm::vec3 position, glm::vec3 target, float FoV = 45.0f, float aspect = 800.0f / 600.0f, float near = 0.1f, float far = 100.0f)
            : position_(position), target_(target), FoV_(FoV), aspect_(aspect), near_(near), far_(far) {
            UpdatePosition();
        }

        void SetPosition(const glm::vec3 &position) {
            position_ = position;
            UpdatePosition();
        }

        void SetTarget(const glm::vec3 &target) {
            target_ = target;
            UpdatePosition();
        }
        
        void SetAspect(float aspect) {
            aspect_ = aspect;
        }

        glm::vec3 GetPosition() const {
            return position_;
        }

        glm::vec3 GetTarget() const {
            return target_;
        }

        glm::vec3 GetDirection() const {
            return direction_;
        }

        glm::vec3 GetUp() const {
            return up_;
        }

        glm::vec3 GetRight() const {
            return right_;
        }

        float GetFoV() const {
            return FoV_;
        }

        glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position_, position_ + direction_, up_);
        }

        glm::mat4 GetProjectionMatrix() const {
            return glm::perspective(glm::radians(FoV_), aspect_, near_, far_);
        }

        void UpdateAspect() {
            GLint viewport[4];
            glRUN(glGetIntegerv, GL_VIEWPORT, viewport);
            aspect_ = static_cast<float>(viewport[2]) / viewport[3];
        }

    private:
        void UpdatePosition() {
            direction_ = glm::normalize(target_ - position_);
            right_ = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction_));
            up_ = glm::cross(direction_, right_);
        }

        float FoV_ = 45.0f;
        float aspect_ = 800.0f / 600.0f;
        float near_ = 0.1f;
        float far_ = 100.0f;

        glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 target_ = glm::vec3(0.0f, 0.0f, 0.0f);
        
        glm::vec3 direction_;
        glm::vec3 right_;
        glm::vec3 up_;
    }; // class Camera

    class EventHandler final : public IEventHandler {
    public:
        EventHandler(Window &window, Camera &camera) : window_(window), camera_(camera) {
            glRUN(glfwSetWindowUserPointer, window.Get(), &camera_);

            auto callback = [] (GLFWwindow* window, int widht, int height) {
                glRUN(glViewport, 0, 0, widht, height);
                Camera *camera = static_cast<Camera*>(glRUN(glfwGetWindowUserPointer, window));
                camera->SetAspect(static_cast<float>(widht) / height);
            };
    
            glRUN(glfwSetFramebufferSizeCallback, window_.Get(), callback);
        }
    
        void UpdateEvent() override {
            if (glfwGetKey(window_.Get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window_.Get(), true);
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_UP) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetUp());
            }    
            if (glfwGetKey(window_.Get(), GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_DOWN) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetUp());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_LEFT) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetRight());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetRight());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (glfwGetKey(window_.Get(), GLFW_KEY_EQUAL) == GLFW_PRESS) {
                    camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetDirection());
                }
                else if (glfwGetKey(window_.Get(), GLFW_KEY_MINUS) == GLFW_PRESS) {
                    camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetDirection());
                }
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera_speed_ = 0.20f;
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
                camera_speed_ = 0.05f;
            }
        }

    private:
        float camera_speed_ = 0.05f;
        Window& window_;
        Camera& camera_;
    };
    
    class Renderer final {
    public:
        Renderer() : program_() {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            gl::Shader vertex{GL_VERTEX_SHADER};
            vertex.Compile("include/shaders/triangle.vs");            
            gl::Shader fragment{GL_FRAGMENT_SHADER};
            fragment.Compile("include/shaders/triangle.fs");

            program_.AttachShader(vertex);
            program_.AttachShader(fragment);

            program_.Link();
        }
        
        void Render(Scene &scene, const Camera &camera) {
            glRUN(glClearColor, 0.4f, 0.4f, 0.4f, 1.0f);
            glRUN(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            program_.Run();
        
            glm::mat4 projection = camera.GetProjectionMatrix();
            glm::mat4 view = camera.GetViewMatrix();
            
            glm::vec3 lightPos = camera.GetTarget();
            glm::vec3 lightColor = glm::vec3{1.0f, 1.0f, 1.0f};

            gl::template UniformSeter<glm::mat4>::Set(program_(), "projection", projection);
            gl::template UniformSeter<glm::mat4>::Set(program_(), "view", view);

            gl::template UniformSeter<glm::vec3>::Set(program_(), "lightPos", lightPos);
            gl::template UniformSeter<glm::vec3>::Set(program_(), "lightColor", lightColor);
            
            auto &&vector = scene.GetObjects();
            for (auto &&it : vector) {
                it->Draw();
            }
        }

        ~Renderer() {
            glfwTerminate();
        }
    private:
        Program program_;
    }; // class Renderer


    void Window::DrawFrames(Renderer &renderer, Scene &scene, Camera &camera) {
        while (!IsShouldBeClosed()) {
            handler_->UpdateEvent();
            
            renderer.Render(scene, camera);
            glRUN(glfwSwapBuffers, window_);
            glRUN(glfwPollEvents);
        }
    }
}; // namespace gl