/// snake3.cpp

#include "snake3.hpp"

int main(int, char**)
{
    settings::get()->load<int>("test", 0);

    render::get();
    while (window::get()->poll())
    {
        render::get()->draw_frame();
    }
}
