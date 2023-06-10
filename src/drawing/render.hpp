/// render.hpp

#pragma once

class render: public singleton<render>, loggable, dependable<window>
{
private:

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;
    vk::UniqueSurfaceKHR surface;

    void create_instance();
    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void pick_gpu();
    void get_surface();

public:

    render();
};
