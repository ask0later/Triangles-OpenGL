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
    class Scene;

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
                throw std::runtime_error("creating of window is failed");
            }
            SetCallback();

            glfwMakeContextCurrent(window_);
        }

        ~Window() {
            glfwTerminate();
        }

        bool IsShouldBeClosed() {
            return glfwWindowShouldClose(window_);
        }

        GLFWwindow *Get() { return window_; }
        
        void DrawFrames(Renderer &renderer, Scene &Scene);
    private:
        void SetCallback() {
            auto callback = [] (GLFWwindow* window, int widht, int height) {
                glViewport(0, 0, widht, height);
            };

            glfwSetFramebufferSizeCallback(window_, callback);
        }
        
        GLFWwindow* window_;
    }; // class Window

    class Shader final {
    public:
        Shader(int type, const std::string &file_name) {
            std::ifstream stream(file_name);
            if (!stream.is_open()) {
                throw std::runtime_error("shader file is not found");
            }

            std::string program_text = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
            const char *text = program_text.data();

            shader_ = glCreateShader(type);
            glShaderSource(shader_, 1, &text, NULL);
            glCompileShader(shader_);

            int success;
            glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
            if (!success) {
                throw std::runtime_error("shader compiling is failed");
            }
        }

        Shader(int type, const char **program_text) {
            shader_ = glCreateShader(type);
            glShaderSource(shader_, 1, program_text, NULL);
            glCompileShader(shader_);

            int success;
            glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
            if (!success) {
                throw std::runtime_error("shader compiling is failed");
            }
        }
        
        ~Shader() {
            glDeleteShader(shader_);
        }

        int operator()() const {
            return shader_;
        }
    private:
        int shader_;
    }; // class Shader

    class Program final {
        public:
        Program() {
            id_ = glCreateProgram();
        }

        void AttachShader(const Shader& shader_obj) {
            glAttachShader(id_, shader_obj());
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
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f))
            : position_(position), target_(target) {
            Update();
        }

        void SetPosition(const glm::vec3 &position) {
            position_ = position;
            Update();
        }

        void SetTarget(const glm::vec3 &target) {
            target_ = target;
            Update();
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

        glm::mat4 GetView() const {
            return glm::lookAt(position_, position_ + direction_, up_);
        }

    private:
        void Update() {
            direction_ = glm::normalize(target_ - position_);
            right_ = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction_));
            up_ = glm::cross(direction_, right_);
        }

        glm::vec3 position_;
        glm::vec3 target_;
        
        glm::vec3 direction_;
        glm::vec3 right_;
        glm::vec3 up_;
    }; // class Camera

    struct EventHandler final {
        static void UpdateEvent(Window &window, Camera &camera) {
            const float camera_speed = 0.05f;
            if (glfwGetKey(window.Get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window.Get(), true);
            }
            if (glfwGetKey(window.Get(), GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window.Get(), GLFW_KEY_UP) == GLFW_PRESS) {
                std::cout << "UP" << std::endl;
                camera.SetPosition(camera.GetPosition() + camera_speed * camera.GetUp());
            }    
            if (glfwGetKey(window.Get(), GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window.Get(), GLFW_KEY_DOWN) == GLFW_PRESS) {
                std::cout << "DOWN" << std::endl;
                camera.SetPosition(camera.GetPosition() - camera_speed * camera.GetUp());
            }
            if (glfwGetKey(window.Get(), GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window.Get(), GLFW_KEY_LEFT) == GLFW_PRESS) {
                std::cout << "LEFT" << std::endl;
                camera.SetPosition(camera.GetPosition() + camera_speed * camera.GetRight());
            }
            if (glfwGetKey(window.Get(), GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window.Get(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
                std::cout << "RIGHT" << std::endl;
                camera.SetPosition(camera.GetPosition() - camera_speed * camera.GetRight());
            }
            if (glfwGetKey(window.Get(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (glfwGetKey(window.Get(), GLFW_KEY_EQUAL) == GLFW_PRESS) {
                    camera.SetPosition(camera.GetPosition() + camera_speed * camera.GetDirection());
                }
                else if (glfwGetKey(window.Get(), GLFW_KEY_MINUS) == GLFW_PRESS) {
                    camera.SetPosition(camera.GetPosition() - camera_speed * camera.GetDirection());
                }
            }
        }
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
        Renderer() : camera_(), program_() {
            glEnable(GL_DEPTH_TEST);

            {
                gl::Shader vertex{GL_VERTEX_SHADER, "include/shaders/triangle.vs"};
                gl::Shader fragment{GL_FRAGMENT_SHADER, "include/shaders/triangle.fs"};
                program_.AttachShader(vertex);
                program_.AttachShader(fragment);
            }

            program_.Link();
        }

        
        void Render(Scene &scene) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            program_.Run();
    
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
            glm::mat4 view = camera_.GetView();
            
            gl::template UniformSeter<glm::mat4>::Set(program_(), "projection", projection);
            gl::template UniformSeter<glm::mat4>::Set(program_(), "view", view);
            gl::template UniformSeter<glm::mat4>::Set(program_(), "model", model);
            
            scene.vao.Bind(scene.vao());
        }

        const Camera &GetCamera() const {
            return camera_;
        }

        Camera &GetCamera() {
            return camera_;
        }

        ~Renderer() {
            glfwTerminate();
        }
    private:
        Camera camera_;
        Program program_;
    }; // class Renderer

    void Window::DrawFrames(Renderer &renderer, Scene &scene) {
        while (!IsShouldBeClosed()) {
            EventHandler::UpdateEvent(*this, renderer.GetCamera());
    
            renderer.Render(scene);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }
}; // namespace gl