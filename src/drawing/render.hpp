/// render.hpp

#pragma once

class render: public singleton<render>, loggable
{
private:

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;

    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void pick_gpu();

public:

    render();
};
