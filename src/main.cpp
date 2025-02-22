#include "gl.hpp"
#include "scene.hpp"
#include <iostream>
#include <cmath>

int main() {
    scene::TriangleScene tscene{};
    tscene.Make();
    
    gl::Window window{800, 600, "Triangle"};
    gl::Camera camera{};
    auto handler = std::make_unique<gl::EventHandler>(window, camera);
    window.SetEventHandler(std::move(handler));

    gl::Renderer renderer{};
    gl::Scene scene{};
    
    window.DrawFrames(renderer, scene, camera);

    return 0;
}