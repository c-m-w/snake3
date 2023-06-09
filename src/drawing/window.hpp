/// window.hpp

#pragma once

class window: public singleton<window>, loggable
{
private:

    constexpr static auto DEFAULT_WIDTH = 1280;
    constexpr static auto DEFAULT_HEIGHT = 720;

    GLFWwindow * wnd;

public:

    window();
    ~window();

    bool poll();
};
