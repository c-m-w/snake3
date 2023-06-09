/// window.cpp

#include "../snake3.hpp"

window::window():
    loggable(__func__)
{
    if (GLFW_FALSE == glfwInit())
        runtime_error("failed to init GLFW.");
    
    wnd = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "snake3", nullptr, nullptr);

    if (nullptr == wnd)
        runtime_error("failed to create window.");
}

window::~window()
{
    glfwTerminate();
}

bool window::poll()
{
    if (glfwWindowShouldClose(wnd) == GLFW_TRUE)
        return false;
    
    glfwPollEvents();
    return true;
}
