/// snake3.cpp

#include "snake3.hpp"

int main(int, char**)
{
    settings::get()->load<int>("test", 0);

    camera::get();
    grid::get();

    while (window::get()->poll())
    {
        input::get()->process_repeats();
        render::get()->draw_frame();
    }
}
