/// camera.cpp

#include "../../snake3.hpp"

camera::camera()
{
    // todo load settings

    input::get()->add_callback('W', interval_callback(
        [this]()
        {
            distance -= 0.1f;
        },
        5ull
    ));
    input::get()->add_callback('S', interval_callback(
        [this]()
        {
            distance += 0.1f;
        },
        5ull
    ));
    input::get()->add_callback('A', interval_callback(
        [this]()
        {
            angle += 0.05f;
        },
        5ull
    ));
    input::get()->add_callback('D', interval_callback(
        [this]()
        {
            angle -= 0.05f;
        },
        5ull
    ));
    input::get()->add_callback(GLFW_KEY_LEFT_CONTROL, interval_callback(
        [this]()
        {
            height -= 0.05f;
        },
        5ull
    ));
    input::get()->add_callback(GLFW_KEY_SPACE, interval_callback(
        [this]()
        {
            height += 0.05f;
        },
        5ull
    ));
}

void camera::set_distance(float const & dist)
{
    distance = dist;
}

glm::vec3 camera::view_pos()
{
    return glm::vec3(
        distance * std::cos(angle),
        distance * std::sin(angle),
        height
    );
}
