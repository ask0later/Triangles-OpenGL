#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <format>

namespace gl {
    
    struct Vertex{
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
    };

    class glException : public std::exception {
    public:
        explicit glException(std::string_view message) : message_(message) {}

        const char *what() const noexcept override {
            return message_.data();
        }
    private:
        std::string message_;
    }; // class glException

    #define glRUN(func, ...) \
        glRun(__LINE__, __FILE__, #func, func __VA_OPT__(,) __VA_ARGS__)

    void glCheckError(int line, std::string_view file_name, std::string_view func_name) {
        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            std::string mes = std::format("OpenGL lib '{}' error in '{}' file on line '{}', when calling the function '{}'.\n", 
                error, file_name, line, func_name);

            throw glException(mes);
        }
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

    void SetUniform(unsigned int shader_id, std::string_view name, const glm::mat4 &value) {
        auto loc = glRUN(glGetUniformLocation, shader_id, name.data());
        glRUN(glUniformMatrix4fv, loc, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void SetUniform(unsigned int shader_id, std::string_view name, const glm::vec3 &value) {
        auto loc = glRUN(glGetUniformLocation, shader_id, name.data());
        glRUN(glUniform3fv, loc, 1, glm::value_ptr(value));
    }

    class IMesh {
    public:
        virtual ~IMesh() {}
        virtual void Draw() = 0;
    }; // class IMesh

    class IShader {
    public:
        virtual ~IShader() {}
        virtual int Use() const = 0;
        virtual void Compile(std::string_view file_name) = 0;
    }; // class IShader

    class IEventHandler {
    public:
        virtual void UpdateEvent() = 0;
        ~IEventHandler() {}
    }; // class IEventHandler
}; // namespace gl