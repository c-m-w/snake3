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

void window::get_size(std::uint32_t & width, std::uint32_t & height)
{
    width = height = 0;

    do
    {
        glfwGetFramebufferSize(wnd,
                            reinterpret_cast<int *>(&width),
                            reinterpret_cast<int *>(&height));
        
        glfwWaitEvents();
    } while (width == 0 || height == 0);
}

window::operator GLFWwindow *() const
{
    return wnd;
}
