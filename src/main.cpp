#include "gl.hpp"
#include <iostream>
#include <cmath>

int main() {    
    gl::Window window(800, 600, "Triangle");
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    gl::Renderer renderer{};
    gl::Scene scene{};
    
    window.DrawFrames(renderer, scene);

    return 0;
}