#pragma once

#include "GL/gl.hpp"

namespace gl {

    class VertexArrayObject final {
    public:
        VertexArrayObject() {
            glRUN(glGenVertexArrays, 1, &VAO_);
        }

        VertexArrayObject(const VertexArrayObject &other) = delete;
        VertexArrayObject &operator=(const VertexArrayObject &other) = delete;

        VertexArrayObject(VertexArrayObject &&other) noexcept {
            std::swap(VAO_, other.VAO_);
        }

        VertexArrayObject &operator=(VertexArrayObject &&other) noexcept {
            if (this != &other) {
                std::swap(VAO_, other.VAO_);
            }

            return *this;
        }

        ~VertexArrayObject() {
            glRUN(glDeleteVertexArrays, 1, &VAO_);
        }

        unsigned int operator()() const {
            return VAO_;
        }
    private:
        unsigned int VAO_ = 0;
    };

    class VertexBufferObject final {
    public:
        VertexBufferObject() {
            glRUN(glGenBuffers, 1, &VBO_);
        }

        VertexBufferObject(const VertexBufferObject &other) = delete;
        VertexBufferObject &operator=(const VertexBufferObject &other) = delete;

        VertexBufferObject(VertexBufferObject &&other) noexcept {
            std::swap(VBO_, other.VBO_);
        }

        VertexBufferObject &operator=(VertexBufferObject &&other) noexcept {
            if (this != &other) {
                std::swap(VBO_, other.VBO_);
            }

            return *this;
        }

        ~VertexBufferObject() {
            glRUN(glDeleteBuffers, 1, &VBO_);
        }

        unsigned int operator()() const {
            return VBO_;
        }
    private:
        unsigned int VBO_ = 0;
    };    

    class TriangleMesh final : public IMesh {
    public:
        TriangleMesh(const std::vector<Vertex> &vertices) : vertex_count_(vertices.size()) {
            glRUN(glBindVertexArray, VAO_());
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, VBO_());
            glRUN(glBufferData, GL_ARRAY_BUFFER, vertex_count_ * sizeof(gl::Vertex), vertices.data(), GL_STATIC_DRAW);

            SetVertexAttribute();
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, 0);
            glRUN(glBindVertexArray, 0);
        }

        TriangleMesh(const TriangleMesh &other) : vertex_count_(other.vertex_count_) {
            int buffer_size = 0;
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, other.VBO_());
            glRUN(glGetBufferParameteriv, GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
            
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, VBO_());
            glRUN(glBufferData, GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

            glBindBuffer(GL_COPY_READ_BUFFER, other.VBO_());
            glBindBuffer(GL_COPY_WRITE_BUFFER, VBO_());
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, buffer_size);    
            
            glRUN(glBindVertexArray, VAO_());
            SetVertexAttribute();
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, 0);
            glRUN(glBindVertexArray, 0);
        }

        TriangleMesh &operator=(const TriangleMesh &other) {
            if (this != &other) {
                TriangleMesh tmp = other;
                std::swap(*this, tmp);
            }

            return *this;
        }

        TriangleMesh(TriangleMesh &&other) noexcept {
            std::swap(vertex_count_, other.vertex_count_);
            std::swap(VBO_, other.VBO_);
            std::swap(VAO_, other.VAO_);
        }

        TriangleMesh &operator=(TriangleMesh &&other) noexcept {
            if (this != &other) {
                std::swap(vertex_count_, other.vertex_count_);
                std::swap(VBO_, other.VBO_);
                std::swap(VAO_, other.VAO_);
            }

            return *this;
        }

        ~TriangleMesh() {}

        void Draw() override {
            glRUN(glBindVertexArray, VAO_());
            glRUN(glDrawArrays, GL_TRIANGLES, 0, vertex_count_);
        }
    private:

        void SetVertexAttribute() {
            glRUN(glEnableVertexAttribArray, 0);
            glRUN(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            
            glRUN(glEnableVertexAttribArray, 1);
            glRUN(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 3));

            glRUN(glEnableVertexAttribArray, 2);
            glRUN(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 6));
        }

        size_t vertex_count_ = 0;
        VertexArrayObject VAO_;
        VertexBufferObject VBO_;
    }; // class Mesh
} // namespace gl