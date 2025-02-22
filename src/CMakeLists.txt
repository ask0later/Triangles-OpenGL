find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(main main.cpp glad.c)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
target_include_directories(main PUBLIC ${INCLUDE_DIR})
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Triangles/include)
target_include_directories(main PUBLIC ${INCLUDE_DIR})

target_compile_features(main PUBLIC cxx_std_20)

target_link_libraries(main PRIVATE GLEW::GLEW OpenGL::GL glfw)