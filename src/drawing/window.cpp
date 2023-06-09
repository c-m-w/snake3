/// window.cpp

#include "../snake3.hpp"

window::window()
{
    glfwInit();
    wnd = glfwCreateWindow(1280, 720, "snake3", nullptr, nullptr);
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
