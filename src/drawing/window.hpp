/// window.hpp

#pragma once

class window: public singleton<window>
{
private:

    GLFWwindow * wnd;

public:

    window();
    ~window();

    bool poll();
};
