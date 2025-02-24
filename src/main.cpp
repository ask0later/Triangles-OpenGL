#include "gl.hpp"
#include "scene.hpp"
#include <iostream>
#include <cmath>

constexpr int START_WIDHT = 800, START_HEIGHT = 600;

int main() {
    scene::TriangleScene tscene{};
    tscene.Make();
    
    gl::Window window{START_WIDHT, START_HEIGHT, "Triangle scene"};
    gl::Camera camera{};
    std::unique_ptr<gl::IEventHandler> handler = std::move(std::make_unique<gl::EventHandler>(window, camera));
    window.SetEventHandler(std::move(handler));

    gl::Renderer renderer{};
    gl::Scene scene{};
    
    window.DrawFrames(renderer, scene, camera);

    return 0;
}