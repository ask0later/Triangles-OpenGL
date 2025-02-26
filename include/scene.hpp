#pragma once
#include "geometry.hpp"
#include "octotree.hpp"
#include "real_nums.hpp"
#include <cassert>

#include <iostream>
#include <glm/gtc/epsilon.hpp>

namespace scene {

    class Scene {
    public:
        virtual void Make() = 0;
        virtual ~Scene() {}
    }; // class Scene

    class TriangleScene final : public Scene {
    public:
        void Make() override {
            size_t fig_count = 0;
            std::cin >> fig_count;
            point_count_ = fig_count * 3;

            points_.reserve(point_count_);

            if (!std::cin.good())
                throw std::runtime_error("input is failed");
            
            for (size_t i = 0; i < point_count_; ++i) {
                float x, y, z;
                std::cin >> x >> y >> z;
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

    private:
        float radius_ = 0.0f;
        glm::vec3 center_ = glm::vec3(0.0f);
        size_t point_count_ = 0;
        std::vector<glm::vec3> points_;
    }; // class TriangleScene

    class GeometryData final {
    public:
        GeometryData(const TriangleScene &scene) {
            size_t point_count = scene.GetPointCount();
            size_t fig_count = point_count / 3;
            auto points = scene.GetPoints();
            std::vector<geometry::figure_t<float>> figs;
            std::set<size_t> intersected_figs;

            coords_.reserve(3 * point_count);
            colors_.reserve(point_count);
            normals_.reserve(point_count);
            figs.reserve(fig_count);

            for (size_t i = 0; i < point_count; i += 3) {
                coords_.push_back(points[i + 0].x); coords_.push_back(points[i + 0].y); coords_.push_back(points[i + 0].z);
                coords_.push_back(points[i + 1].x); coords_.push_back(points[i + 1].y); coords_.push_back(points[i + 1].z);
                coords_.push_back(points[i + 2].x); coords_.push_back(points[i + 2].y); coords_.push_back(points[i + 2].z);
                
                geometry::point_t<float> point1{points[i + 0].x, points[i + 0].y, points[i + 0].z};
                geometry::point_t<float> point2{points[i + 1].x, points[i + 1].y, points[i + 1].z};
                geometry::point_t<float> point3{points[i + 2].x, points[i + 2].y, points[i + 2].z};
        
                geometry::figure_t<float> fig = geometry::figure_ctor<float>(point1, point2, point3);
                figs.push_back(fig);
            }

            octotree::intersect_figs<float>(figs, intersected_figs);
            
            auto it = intersected_figs.begin();
            auto end = intersected_figs.end();

            for (size_t i = 0; i < fig_count; ++i) {
                if (it != end && *it == i) {
                    ++it;
                    colors_.push_back(1.0f); colors_.push_back(0.0f); colors_.push_back(0.0f);
                } else {
                    colors_.push_back(0.0f); colors_.push_back(0.0f); colors_.push_back(1.0f);
                }

                auto AB = points[3 * i + 1] - points[3 * i + 0];
                auto AC = points[3 * i + 2] - points[3 * i + 0];

                auto normal = glm::normalize(glm::cross(AB, AC));
                normals_.push_back(normal.x); normals_.push_back(normal.y); normals_.push_back(normal.z);
            }
            
            assert(normals_.size() == colors_.size());
            assert(normals_.size() == 3 * fig_count);
            assert(coords_.size() == 9 * fig_count);
        }

        void GetData(std::vector<float> &data) const {
            size_t point_count = coords_.size() / 3;
            size_t fig_count = point_count / 3;
            data.reserve(9 * point_count);

            for (size_t i = 0; i < fig_count; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    data.push_back(coords_[9 * i + 3 * j + 0]); data.push_back(coords_[9 * i + 3 * j + 1]); data.push_back(coords_[9 * i + 3 * j + 2]);
                    
                    data.push_back(colors_[3 * i + 0]); data.push_back(colors_[3 * i + 1]); data.push_back(colors_[3 * i + 2]);         
                    data.push_back(normals_[3 * i + 0]); data.push_back(normals_[3 * i + 1]); data.push_back(normals_[3 * i + 2]);
                }
            }

            assert(data.size() == 3 * coords_.size());
            assert(point_count == colors_.size());
        }
    private:
        std::vector<float> coords_;
        std::vector<float> colors_;
        std::vector<float> normals_;
    };
}