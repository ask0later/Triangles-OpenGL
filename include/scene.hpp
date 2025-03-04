#pragma once

#include "geometry.hpp"
#include "octotree.hpp"
#include "real_nums.hpp"
#include "gl.hpp"
#include <cassert>

#include <iostream>
#include <glm/gtc/epsilon.hpp>

namespace scene {
    class TriangleScene final {
    public:
        TriangleScene() {
            size_t fig_count = 0;
            std::cin >> fig_count;
            if (!std::cin.good())
                throw std::runtime_error("Input is failed");
            
            point_count_ = fig_count * 3;
            points_.reserve(point_count_);
            
            for (size_t i = 0; i < point_count_; ++i) {
                float x, y, z;
                std::cin >> x >> y >> z;
                if (!std::cin.good())
                    throw std::runtime_error("Input is failed");

                points_.emplace_back(x, y, z);
            }
        }

        glm::vec3 GetCenter() {
            if (glm::all(glm::epsilonEqual(center_, glm::vec3(0.0f), 0.01f))) {
                glm::vec3 min_point = points_.front();
                glm::vec3 max_point = points_.front();

                for (const auto &point : points_) {
                    min_point = glm::min(min_point, point);
                    max_point = glm::max(max_point, point);
                }
                max_point_ = max_point;
                min_point_ = min_point;
                center_ = glm::vec3(min_point + max_point) * 0.5f;
            }
            
            return center_;
        }

        float GetRadius() {
            if (radius_ == 0.0f) {
                for (const auto &point : points_) {
                    radius_ = glm::max(radius_, glm::length(point - center_));
                }
            }

            return radius_;
        }

        const std::vector<glm::vec3> &GetPoints() const {
            return points_;
        }

        size_t GetPointCount() const {
            return point_count_;
        }

        float GetMax() const {
            return glm::max(glm::max(max_point_.x, max_point_.y), max_point_.z);
        }

        float GetMin() const {
            return glm::min(glm::min(min_point_.x, min_point_.y), min_point_.z);
        }

        float GetNearestDistanceFrom(const glm::vec3 &src) const {
            auto nearest = GetNearestFrom(src);
            return glm::length(nearest - src);
        }

        float GetFarestDistanceFrom(const glm::vec3 &src) const {
            auto farest = GetFarestFrom(src);
            return glm::length(farest - src);
        }

        glm::vec3 GetNearestFrom(const glm::vec3 &src) const {
            float distance = glm::length(points_[0] - src);
            glm::vec3 dst{0.0f};

            for (const auto &point : points_) {
                auto tmp = glm::length(point - src);
                if (distance > tmp) {
                    distance = tmp;
                    dst = point;
                }
            }
            return dst;
        }

        glm::vec3 GetFarestFrom(const glm::vec3 &src) const {
            float distance = 0.0f;
            glm::vec3 dst{0.0f};

            for (const auto &point : points_) {
                auto tmp = glm::length(point - src);
                if (distance < tmp) {
                    distance = tmp;
                    dst = point;
                }
            }
            return dst;
        }

    private:
        float radius_ = 0.0f;
        glm::vec3 center_ = glm::vec3(0.0f);
        glm::vec3 max_point_ = glm::vec3(0.0f);
        glm::vec3 min_point_ = glm::vec3(0.0f);
        size_t point_count_ = 0;
        std::vector<glm::vec3> points_;
    }; // class TriangleScene

    class GeometryData final {
    public:
        GeometryData(const TriangleScene &scene) : point_count_(scene.GetPointCount()) {
            size_t fig_count = point_count_ / 3;

            coords_.reserve(point_count_);
            colors_.reserve(fig_count);
            normals_.reserve(fig_count);
            
            std::vector<geometry::figure_t<float>> figs;
            auto points = scene.GetPoints();
            CreateFigsAndSetCoords(points, figs);
            SetColorsAndNormals(points, figs);
        }

        void GetData(std::vector<gl::Vertex> &vertices) const {
            vertices.reserve(point_count_);
            
            size_t fig_count = point_count_ / 3;
            for (size_t i = 0; i < fig_count; ++i)
                for (size_t j = 0; j < 3; ++j)
                    vertices.emplace_back(coords_[3 * i + j], colors_[i], normals_[i]);

            assert(vertices.size() == point_count_);
        }

    private:
        void CreateFigsAndSetCoords(const std::vector<glm::vec3> points, std::vector<geometry::figure_t<float>> &figs) {
            figs.reserve(point_count_ / 3);

            for (size_t i = 0; i < point_count_; i += 3) {
                coords_.push_back(points[i + 0]);
                coords_.push_back(points[i + 1]);
                coords_.push_back(points[i + 2]);
                
                geometry::point_t<float> point1{points[i + 0].x, points[i + 0].y, points[i + 0].z};
                geometry::point_t<float> point2{points[i + 1].x, points[i + 1].y, points[i + 1].z};
                geometry::point_t<float> point3{points[i + 2].x, points[i + 2].y, points[i + 2].z};
        
                geometry::figure_t<float> fig = geometry::figure_ctor<float>(point1, point2, point3);
                figs.push_back(fig);
            }

            assert(3 * figs.size() == point_count_);
        }

        void SetColorsAndNormals(const std::vector<glm::vec3> points, std::vector<geometry::figure_t<float>> &figs) {
            std::set<size_t> intersected_figs;
            octotree::intersect_figs<float>(figs, intersected_figs);
            
            auto it = intersected_figs.begin();
            auto end = intersected_figs.end();

            for (size_t i = 0; i < point_count_ / 3; ++i) {
                if (it != end && *it == i) {
                    ++it;
                    colors_.emplace_back(1.0f, 0.0f, 0.0f);
                } else {
                    colors_.emplace_back(0.0f, 0.0f, 1.0f);
                }

                auto AB = points[3 * i + 1] - points[3 * i + 0];
                auto AC = points[3 * i + 2] - points[3 * i + 0];

                normals_.push_back(glm::normalize(glm::cross(AB, AC)));
            }

            assert(normals_.size() == colors_.size());
            assert(coords_.size() == 3 * colors_.size());
        }

        std::vector<glm::vec3> coords_;
        std::vector<glm::vec3> colors_;
        std::vector<glm::vec3> normals_;
        size_t point_count_;
    };
}