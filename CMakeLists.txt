cmake_minimum_required(VERSION 3.0.0)
project(snake3 VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(snake3 src/snake3.cpp
                      src/drawing/window.cpp
                      src/drawing/render/queue_families.cpp
                      src/drawing/render/swap_info.cpp
                      src/drawing/render/render.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(glfw3 REQUIRED FATAL_ERROR)
find_package(Vulkan REQUIRED FATAL_ERROR)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

target_link_libraries(snake3 glfw vulkan)
