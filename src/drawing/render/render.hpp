/// render.hpp

#pragma once

#include "queue_families.hpp"

namespace std
{
    template<>
    struct hash<vk::PhysicalDevice>
    {
        std::size_t operator()(vk::PhysicalDevice const & s) const noexcept
        {
            return hash<void *>( )(static_cast<void *>(s));
        }
    };
}

class render: public singleton<render>, loggable, dependable<window>
{
private:

    using queue_family_map_t = std::unordered_map<vk::PhysicalDevice, queue_family>;

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;
    vk::UniqueSurfaceKHR surface;
    queue_family_map_t queue_families;

    void create_instance();
    bool get_device_queue_families(vk::PhysicalDevice const & dev);
    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void pick_gpu();
    void get_surface();

public:

    render();

    friend class queue_family;
};
