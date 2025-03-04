#pragma once

#include "gl.hpp"
#include "renderer.hpp"
#include "camera.hpp"

namespace gl {

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
                glfwDestroyWindow(window_);
                glfwTerminate();
                throw glException("Failed to initialize GLAD");
            }
        }

        ~Window() {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }

        void SetEventHandler(std::unique_ptr<IEventHandler> &&handler) noexcept {
            handler_ = std::move(handler);
        }    

        GLFWwindow *Get() const noexcept { return window_; }
        
        void DrawFrames(Renderer &renderer, std::vector<std::unique_ptr<IMesh>> &scene, Camera &camera) const {
            while (!IsShouldBeClosed()) {
                handler_->UpdateEvent();
                
                renderer.Render(scene, camera);
                glRUN(glfwSwapBuffers, window_);
                glRUN(glfwPollEvents);
            }
        }
        
        bool CompareKeyState(int key, int action) const {
            return (action == glRUN(glfwGetKey, window_, key));
        }

        void SetUser(Camera &camera) {
            glRUN(glfwSetWindowUserPointer, window_, &camera);

            auto callback = [] (GLFWwindow* window, int widht, int height) {
                glRUN(glViewport, 0, 0, widht, height);
                Camera *cam = static_cast<Camera*>(glRUN(glfwGetWindowUserPointer, window));
                cam->SetAspect(static_cast<float>(widht) / height);
            };
    
            glRUN(glfwSetFramebufferSizeCallback, window_, callback);
        }

        void Close() const {
            glRUN(glfwSetWindowShouldClose, window_, true);
        }
    
    private:
        bool IsShouldBeClosed() const {
            return glRUN(glfwWindowShouldClose, window_);
        }
        
        GLFWwindow* window_;
        std::unique_ptr<IEventHandler> handler_;
    }; // class Window

    class EventHandler final : public IEventHandler {
    public:
        EventHandler(Window &window, Camera &camera) : window_(window), camera_(camera) {
            window_.SetUser(camera_);
        }
    
        void UpdateEvent() override {
            if (window_.CompareKeyState(GLFW_KEY_ESCAPE, GLFW_PRESS)) {
                window_.Close();
            }
            if (window_.CompareKeyState(GLFW_KEY_W, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_UP, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetUp());
            }    
            if (window_.CompareKeyState(GLFW_KEY_S, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_DOWN, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetUp());
            }
            if (window_.CompareKeyState(GLFW_KEY_A, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_LEFT, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetRight());
            }
            if (window_.CompareKeyState(GLFW_KEY_D, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_RIGHT, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetRight());
            }
            if (window_.CompareKeyState(GLFW_KEY_LEFT_CONTROL, GLFW_PRESS)) {
                if (window_.CompareKeyState(GLFW_KEY_EQUAL, GLFW_PRESS)) {
                    camera_.SetPosition(camera_.GetPosition() + camera_speed_ * camera_.GetDirection());
                }
                else if (window_.CompareKeyState(GLFW_KEY_MINUS, GLFW_PRESS)) {
                    camera_.SetPosition(camera_.GetPosition() - camera_speed_ * camera_.GetDirection());
                }
            }
            if (window_.CompareKeyState(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS)) {
                camera_speed_ = 0.20f;
            }
            if (window_.CompareKeyState(GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE)) {
                camera_speed_ = 0.05f;
            }
        }

    private:
        float camera_speed_ = 0.05f;
        Window& window_;
        Camera& camera_;
    }; // class EventHandler
} // namespace gl