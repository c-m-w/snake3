/// window.hpp

#pragma once

class window: public singleton<window>, loggable
{
private:

    constexpr static auto DEFAULT_WIDTH = 1280;
    constexpr static auto DEFAULT_HEIGHT = 720;

    GLFWwindow * wnd;

    std::pair<const char**, std::size_t> get_extensions();
    
public:

    window();
    ~window();

    bool poll();

    operator GLFWwindow * () const;

    friend class render;
};
