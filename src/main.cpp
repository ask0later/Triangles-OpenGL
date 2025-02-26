#include "gl.hpp"
#include "scene.hpp"
#include <iostream>
#include <cmath>

constexpr int START_WIDHT = 800, START_HEIGHT = 600;

int main() {
    gl::Window window{START_WIDHT, START_HEIGHT, "Triangle scene"};
    gl::Camera camera{};
    std::unique_ptr<gl::IEventHandler> handler = std::move(std::make_unique<gl::EventHandler>(window, camera));
    window.SetEventHandler(std::move(handler));
    gl::Renderer renderer{};

    size_t point_count = 0;
    std::vector<float> data;
    
    {
        scene::TriangleScene tscene{};
        tscene.Make();
        point_count = tscene.GetPointCount();
        auto center = tscene.GetCenter();
        auto offset = tscene.GetRadius() / glm::tan(glm::radians(camera.GetFoV() * 0.5f));
        camera.SetTarget(center);
        camera.SetPosition(center + glm::vec3(0.0f, 0.0f, offset)); 

        scene::GeometryData geom{tscene};
        geom.GetData(data);
    }

    auto triangles = std::make_unique<gl::TriangleMesh>(point_count);
    triangles->LoadData(data);

    gl::Scene scene{};
    scene.AddObject(std::move(triangles));
    window.DrawFrames(renderer, scene, camera);

    return 0;
}