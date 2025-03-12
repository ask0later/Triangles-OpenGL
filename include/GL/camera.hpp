#pragma once

#include "GL/gl.hpp"

namespace gl {
    constexpr float MIN_SPEED = 0.05f;
    class Camera final {
    public:
        Camera() {
            UpdatePosition();
        }

        Camera(glm::vec3 position, glm::vec3 target, float near = 1.0f, float far = 1000.0f, float FoV = 45.0f, float aspect = 800.0f / 600.0f)
            : position_(position), target_(target), FoV_(FoV), aspect_(aspect), near_(near), far_(far) {
            UpdatePosition();
        }

        void UpdateAspect() {
            GLint viewport[4];
            glRUN(glGetIntegerv, GL_VIEWPORT, viewport);
            aspect_ = static_cast<float>(viewport[2]) / viewport[3];
        }

    public:
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

        void SetNear(float near) {
            near_ = near;
        }
        
        void SetFar(float far) {
            far_ = far;
        }
    public:
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

        float GetSpeed() const {
            return speed_;
        }

        glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position_, position_ + direction_, up_);
        }

        glm::mat4 GetProjectionMatrix() const {
            return glm::perspective(glm::radians(FoV_), aspect_, near_, far_);
        }

    private:
        void UpdatePosition() {
            float lenght = glm::length(target_ - position_) * MIN_SPEED;
            speed_ = (lenght > MIN_SPEED) ? lenght : MIN_SPEED;
            direction_ = glm::normalize(target_ - position_);
            right_ = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction_));
            up_ = glm::cross(direction_, right_);
        }

        float FoV_ = 45.0f;
        float aspect_ = 800.0f / 600.0f;
        float near_ = 0.1f;
        float far_ = 100.0f;
        float speed_ = MIN_SPEED;

        glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 target_ = glm::vec3(0.0f, 0.0f, 0.0f);
        
        glm::vec3 direction_;
        glm::vec3 right_;
        glm::vec3 up_;
    }; // class Camera
} // namespace gl
