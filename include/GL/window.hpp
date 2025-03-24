#pragma once

#include "GL/gl.hpp"
#include "GL/renderer.hpp"
#include "GL/camera.hpp"
#include <functional>

namespace gl {

    class Window final {
        Window(int widht = 800, int height = 600, std::string_view title = "") {
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

            try {
                SetCallbacks();
            } catch (std::exception &ex) {
                glfwDestroyWindow(window_);
                glfwTerminate();
                throw ex;
            }
        }

        ~Window() {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
    public:
        static Window &QueryWindow(int widht = 800, int height = 600, std::string_view title = "") {
            static Window window(widht, height, title);
            return window;
        }

        Window(const Window &other) = delete;
        Window &operator=(const Window &other) = delete;
        Window(Window &&other) = delete;
        Window &operator=(Window &&other) = delete;

        void SetEventHandler(std::unique_ptr<IEventHandler> &&handler) noexcept {
            handler_ = std::move(handler);
        }    

        GLFWwindow *Get() const noexcept { return window_; }
        
        template <typename SceneIterator>
        void DrawFrames(SceneIterator begin, SceneIterator end, Renderer &renderer, Camera &camera) const {
            while (!IsShouldBeClosed()) {
                handler_->UpdateEvent();
                
                renderer.Render(begin, end, camera);
                glRUN(glfwSwapBuffers, window_);
                glRUN(glfwPollEvents);
            }
        }
        
        bool CompareKeyState(int key, int action) const {
            return (action == glRUN(glfwGetKey, window_, key));
        }

        void Close() const {
            glRUN(glfwSetWindowShouldClose, window_, true);
        }

        template<typename CallbackT>
        void AddFrameBufferCallback(CallbackT&& callback) {
            frame_buffer_callbacks_.emplace_back(std::forward<CallbackT>(callback));
        }
    
    private:
        bool IsShouldBeClosed() const {
            return glRUN(glfwWindowShouldClose, window_);
        }

        void SetCallbacks() {
            glRUN(glfwSetWindowUserPointer, window_, this);

            auto callback = [] (GLFWwindow* window, int widht, int height) {
                glRUN(glViewport, 0, 0, widht, height);
                auto *window_impl = static_cast<Window*>(glRUN(glfwGetWindowUserPointer, window));
                assert(window_impl);
                window_impl->CallFrameBufferCallbacks(widht, height);
            };

            glRUN(glfwSetFramebufferSizeCallback, window_, callback);
        }
  
        void CallFrameBufferCallbacks(int width, int height) const {
            for(auto&& callback: frame_buffer_callbacks_)
                std::invoke(callback, width, height);
        }
        
        GLFWwindow* window_;
        std::vector<std::function<void(int, int)>> frame_buffer_callbacks_;
        std::unique_ptr<IEventHandler> handler_;
    }; // class Window

    class EventHandler final : public IEventHandler {
    public:
        EventHandler(Window &window, Camera &camera) : window_(window), camera_(camera) {
            window_.AddFrameBufferCallback([&camera](int width, int height) {
                camera.SetAspect(static_cast<float>(width) / height);
            });
        }
    
        void UpdateEvent() override {
            if (window_.CompareKeyState(GLFW_KEY_ESCAPE, GLFW_PRESS)) {
                window_.Close();
            }
            if (window_.CompareKeyState(GLFW_KEY_W, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_UP, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() + camera_.GetSpeed() * camera_.GetUp());
            }    
            if (window_.CompareKeyState(GLFW_KEY_S, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_DOWN, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() - camera_.GetSpeed() * camera_.GetUp());
            }
            if (window_.CompareKeyState(GLFW_KEY_A, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_LEFT, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() + camera_.GetSpeed() * camera_.GetRight());
            }
            if (window_.CompareKeyState(GLFW_KEY_D, GLFW_PRESS) || window_.CompareKeyState(GLFW_KEY_RIGHT, GLFW_PRESS)) {
                camera_.SetPosition(camera_.GetPosition() - camera_.GetSpeed() * camera_.GetRight());
            }
            if (window_.CompareKeyState(GLFW_KEY_LEFT_CONTROL, GLFW_PRESS)) {
                if (window_.CompareKeyState(GLFW_KEY_EQUAL, GLFW_PRESS)) {
                    camera_.SetPosition(camera_.GetPosition() + camera_.GetSpeed() * camera_.GetDirection());
                }
                else if (window_.CompareKeyState(GLFW_KEY_MINUS, GLFW_PRESS)) {
                    camera_.SetPosition(camera_.GetPosition() - camera_.GetSpeed() * camera_.GetDirection());
                }
            }
        }

    private:
        Window& window_;
        Camera& camera_;
    }; // class EventHandler
} // namespace gl