/// window.cpp

#include "../snake3.hpp"

std::pair<const char**, std::size_t> window::get_extensions()
{
	auto ext_count = 0u;
	auto const ext = glfwGetRequiredInstanceExtensions(&ext_count);
	
	return std::make_pair(ext, ext_count);
}

window::window():
    loggable(__func__)
{
    if (GLFW_FALSE == glfwInit())
        runtime_error("failed to init GLFW.");
    
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
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

window::operator GLFWwindow * ( ) const
{
    return wnd;
}
