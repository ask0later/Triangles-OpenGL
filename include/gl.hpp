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

namespace gl {

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
     
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
     
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
     
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f
    };
    
    
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-4.5f, -2.1f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.3f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.6f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    class Renderer;
    class Camera;
    class Scene;
    class IEventHandler;

    template <typename T>
    struct UniformSeter final {};

    template <> struct UniformSeter<glm::mat4> {
        static void Set(unsigned int shader_id, const std::string &name, const glm::mat4 &value) {
            unsigned int loc = glGetUniformLocation(shader_id, name.data());
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
    };

    class Window final {
    public:
        Window(int widht = 800, int height = 600, std::string title = "") {
            glfwInit();
            window_ = glfwCreateWindow(widht, height, title.data(), NULL, NULL);
            if (window_ == nullptr) {
                throw std::runtime_error("Failed to create a window");
            }

            glfwMakeContextCurrent(window_);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                throw std::runtime_error("Failed to initialize GLAD");
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
            return glfwWindowShouldClose(window_);
        }
        
        GLFWwindow* window_;
        std::unique_ptr<IEventHandler> handler_;
    }; // class Window

    class IShader {
    public:
        virtual ~IShader() {}
        virtual int Use() const = 0;
        virtual void Compile(const std::string &file_name) = 0;
    }; // class IShader

    class Shader final : public IShader {
    public:
        Shader(int type) {
            shader_ = glCreateShader(type);
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
            glCompileShader(shader_);

            int success;
            glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
            if (!success) {
                throw std::runtime_error("Failed to compile shader");
            }
        }
        
        ~Shader() {
            glDeleteShader(shader_);
        }

    private:
        int shader_;
    }; // class Shader

    class Program final {
    public:
        Program() {
            id_ = glCreateProgram();
        }

        void AttachShader(const IShader& shader_obj) {
            glAttachShader(id_, shader_obj.Use());
        }

        void Link() {
            glLinkProgram(id_);
            int success;
            glGetProgramiv(id_, GL_LINK_STATUS, &success);
            if (!success) {
                throw std::runtime_error("program linking is failed");
            }
        }

        void Run() {
            glUseProgram(id_);
        }
        
        int operator()() const {
            return id_;
        }
    private:
        int id_;
    }; // class Program

    class Buffer final {
    public:
        Buffer() {
            glGenBuffers(1, &id_);
        }

        ~Buffer() {
            glDeleteBuffers(1, &id_);
        }

        template <typename T>
        void Store(const T &data, int type) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, type);
        }

        void Bind(unsigned int obj = 0) {
            glBindBuffer(GL_ARRAY_BUFFER, obj);
        }
    
        unsigned int operator()() {
            return id_;
        }

    private:
        unsigned int id_ = 0;
    }; // class Buffer

    class AttributeArray final {
    public:
        AttributeArray() {
            glGenVertexArrays(1, &id_);
        }

        ~AttributeArray() {
            glDeleteVertexArrays(1, &id_);
        }

        void Bind(unsigned int obj = 0) {
            glBindVertexArray(obj);
        }

        void Set(unsigned int location, unsigned int attr_size, unsigned int full_size, int type_el, unsigned int size_el, unsigned int offset = 0) {
            glVertexAttribPointer(location, attr_size, GL_FLOAT, GL_FALSE, full_size * size_el, reinterpret_cast<void *>(offset * size_el));
        }

        void Enable(unsigned int location = 0) {
            glEnableVertexAttribArray(location);
        }

        unsigned int operator()() {
            return id_;
        }

    private:
        unsigned int id_ = 0;
    }; // class AttributeArray

    class Camera final {
    public:
        Camera() {
            UpdatePosition();
        }

        Camera(glm::vec3 position, glm::vec3 target, float FoV, float aspect, float near, float far)
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

        glm::vec3 GetPosition() {
            return position_;
        }

        glm::vec3 GetDirection() {
            return direction_;
        }

        glm::vec3 GetUp() {
            return up_;
        }

        glm::vec3 GetRight() {
            return right_;
        }

        glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position_, position_ + direction_, up_);
        }

        glm::mat4 GetProjectionMatrix() const {
            return glm::perspective(glm::radians(FoV_), aspect_, near_, far_);
        }

        void UpdateAspect() {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
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

    class IEventHandler {
    public:
        virtual void UpdateEvent() = 0;
        ~IEventHandler() {}
    }; // class IEventHandler

    class EventHandler final : public IEventHandler {
    public:
        EventHandler(Window &window, Camera &camera) : window_(window), camera_(camera) {
            glfwSetWindowUserPointer(window.Get(), &camera_);

            auto callback = [] (GLFWwindow* window, int widht, int height) {
                glViewport(0, 0, widht, height);
                Camera *camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
                camera->SetAspect(static_cast<float>(widht) / height);
            };
    
            glfwSetFramebufferSizeCallback(window_.Get(), callback);
        }
    
        void UpdateEvent() override {
            const float camera_speed = 0.05f;
            if (glfwGetKey(window_.Get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window_.Get(), true);
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_UP) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed * camera_.GetUp());
            }    
            if (glfwGetKey(window_.Get(), GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_DOWN) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed * camera_.GetUp());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_LEFT) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed * camera_.GetRight());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window_.Get(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed * camera_.GetRight());
            }
            if (glfwGetKey(window_.Get(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (glfwGetKey(window_.Get(), GLFW_KEY_EQUAL) == GLFW_PRESS) {
                    camera_.SetPosition(camera_.GetPosition() + camera_speed * camera_.GetDirection());
                }
                else if (glfwGetKey(window_.Get(), GLFW_KEY_MINUS) == GLFW_PRESS) {
                    camera_.SetPosition(camera_.GetPosition() - camera_speed * camera_.GetDirection());
                }
            }
        }

    private:
        Window& window_;
        Camera& camera_;
    };

    class Scene final {
    public:
        Scene() {
            vao.Bind(vao());
            vbo.Bind(vbo());
            
            vbo.Store(vertices, GL_STATIC_DRAW);

            vao.Set(0, 3, 6, GL_FLOAT, sizeof(float));
            vao.Enable();
            vao.Set(1, 3, 6, GL_FLOAT, sizeof(float), 3);
            vao.Enable(1);
        
            vbo.Bind();
            vao.Bind();
        }

        AttributeArray vao;
        Buffer vbo;
    }; // class Scene
    
    class Renderer final {
    public:
        Renderer() : program_() {
            glEnable(GL_DEPTH_TEST);

            gl::Shader vertex{GL_VERTEX_SHADER};
            vertex.Compile("include/shaders/triangle.vs");            
            gl::Shader fragment{GL_FRAGMENT_SHADER};
            fragment.Compile("include/shaders/triangle.fs");

            program_.AttachShader(vertex);
            program_.AttachShader(fragment);

            program_.Link();
        }
        
        void Render(Scene &scene, const Camera &camera) {
            glClearColor(0.5f, 0.1f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            program_.Run();
    
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 projection = camera.GetProjectionMatrix();
            glm::mat4 view = camera.GetViewMatrix();
            
            gl::template UniformSeter<glm::mat4>::Set(program_(), "projection", projection);
            gl::template UniformSeter<glm::mat4>::Set(program_(), "view", view);
            gl::template UniformSeter<glm::mat4>::Set(program_(), "model", model);
            
            scene.vao.Bind(scene.vao());
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
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }
}; // namespace gl