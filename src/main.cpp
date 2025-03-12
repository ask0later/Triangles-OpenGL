#include "GL/gl.hpp"
#include "GL/window.hpp"
#include "GL/renderer.hpp"
#include "GL/mesh.hpp"
#include "scene.hpp"

#include <iostream>
#include <cmath>

constexpr int START_WIDHT = 800, START_HEIGHT = 600;
constexpr float FoV = 45.0f;

int main() try {
    std::vector<gl::Vertex> vertices;
    glm::vec3 cam_pos, cam_target;
    float near = 0.1f, far = 100.0f;

    {
        scene::TriangleScene tscene{};
        cam_target = tscene.GetCenter();
        auto offset = tscene.GetRadius() / glm::tan(glm::radians(FoV * 0.5f));
        cam_pos = cam_target + glm::vec3(0.0f, 0.0f, offset);

        auto nearest_dist = tscene.GetNearestDistanceFrom(cam_pos);
        auto farest_dist = tscene.GetFarestDistanceFrom(cam_pos);

        near = nearest_dist * 0.1f;
        far = 1000.0f * near;
        far = (far > farest_dist) ? far : farest_dist;

        scene::GeometryData geom{tscene};
        vertices = geom.GetData();
    }

    auto &window = gl::Window::QueryWindow(START_WIDHT, START_HEIGHT, "Triangle scene");
    gl::Camera camera{cam_pos, cam_target, near, far};
    window.SetEventHandler(std::move(std::make_unique<gl::EventHandler>(window, camera)));
    gl::Renderer renderer{};

    std::vector<std::unique_ptr<gl::IMesh>> scene;
    scene.push_back(std::move(std::make_unique<gl::TriangleMesh>(vertices)));
    window.DrawFrames(renderer, scene, camera);

    return 0;
} catch (std::exception &ex) {
    std::cout << "Exceptions is catched: " << ex.what() << std::endl;
}