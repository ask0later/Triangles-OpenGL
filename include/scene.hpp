#pragma once
#include "geometry.hpp"
#include "octotree.hpp"

#include <iostream>

namespace scene {

    class Scene {
    public:
        virtual void Make() = 0;
        virtual ~Scene() {}
    }; // class Scene

    class TriangleScene final : public Scene {
    public:
        TriangleScene() {}
        ~TriangleScene() {}

        void Make() override {
            size_t num_figs = 0;
            std::cin >> num_figs;
            if (!std::cin.good())
                throw std::runtime_error("input is failed");

            figs_.reserve(num_figs);

            for (size_t i = 0; i < num_figs; i++) {
                double x1, x2, x3, y1, y2, y3, z1, z2, z3;
                std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
                if (!std::cin.good())
                    throw std::runtime_error("input is failed");
        
                geometry::point_t<double> point1{x1, y1, z1};
                geometry::point_t<double> point2{x2, y2, z2};
                geometry::point_t<double> point3{x3, y3, z3};
        
                geometry::figure_t<double> fig =
                    geometry::figure_ctor<double>(point1, point2, point3);
        
                figs_.push_back(fig);
            }

            octotree::intersect_figs<double>(figs_, intersect_figs_id_);
        }

    private:
        std::vector<geometry::figure_t<double>> figs_;
        std::set<size_t> intersect_figs_id_;
    }; // class TriangleScene
}