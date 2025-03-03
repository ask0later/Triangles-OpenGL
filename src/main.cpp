#include "gl.hpp"
#include "scene.hpp"
#include <iostream>
#include <cmath>

constexpr int START_WIDHT = 800, START_HEIGHT = 600;
constexpr float FoV = 45.0f;

int main() try {
    size_t point_count = 0;
    std::vector<float> data;
    glm::vec3 position, target;
    
    {
        scene::TriangleScene tscene{};
        tscene.Make();
        point_count = tscene.GetPointCount();
        target = tscene.GetCenter();
        auto offset = tscene.GetRadius() / glm::tan(glm::radians(FoV * 0.5f));
        position = target + glm::vec3(0.0f, 0.0f, offset);
        scene::GeometryData geom{tscene};
        geom.GetData(data);
    }

    gl::Window window{START_WIDHT, START_HEIGHT, "Triangle scene"};
    gl::Camera camera{position, target};
    std::unique_ptr<gl::IEventHandler> handler = std::move(std::make_unique<gl::EventHandler>(window, camera));
    window.SetEventHandler(std::move(handler));
    gl::Renderer renderer{};

    auto triangles = std::make_unique<gl::TriangleMesh>(point_count);
    triangles->LoadData(data);

    gl::Scene scene{};
    scene.AddObject(std::move(triangles));
    window.DrawFrames(renderer, scene, camera);

    return 0;
} catch (std::exception &ex) {
    std::cout << "Exceptions is catched: " << ex.what() << std::endl;
}