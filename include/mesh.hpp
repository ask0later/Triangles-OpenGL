#pragma once

#include "gl.hpp"

namespace gl {

    class TriangleMesh final : public IMesh {
    public:
        TriangleMesh(const std::vector<Vertex> &vertices) : vertex_count_(vertices.size()) {
            glRUN(glGenVertexArrays, 1, &VAO_);
            glRUN(glBindVertexArray, VAO_);

            glRUN(glGenBuffers, 1, &VBO_);
            glRUN(glBindBuffer, GL_ARRAY_BUFFER, VBO_);

            glRUN(glBufferData, GL_ARRAY_BUFFER, vertex_count_ * sizeof(gl::Vertex), vertices.data(), GL_STATIC_DRAW);
            
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
            glRUN(glDrawArrays, GL_TRIANGLES, 0, vertex_count_);
        }
    private:
        size_t vertex_count_;
        unsigned int VBO_, VAO_;  
    }; // class Mesh
} // namespace gl