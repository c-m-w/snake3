/// render.hpp

#pragma once

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

    static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#if not defined( NDEBUG )

    constexpr static std::array< const char *, 1 > VALIDATION_LAYERS = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

#endif

    using queue_family_map_t = std::unordered_map<vk::PhysicalDevice, queue_family>;

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;
    vk::UniqueSurfaceKHR surface;
    queue_family_map_t queues;
    vk::UniqueDevice dev;
    swap_info sc_info;
    vk::UniqueSwapchainKHR swapchain;

    void create_instance();
    bool get_device_queue_families(vk::PhysicalDevice const & dev);
    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void get_surface();
    void pick_gpu();
    void create_device();
    void create_swapchain();

public:

    render();

    friend class queue_family;
};
