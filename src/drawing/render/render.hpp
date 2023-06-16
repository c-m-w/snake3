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

    constexpr static auto NFRAMEBUFFERS = 2;

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;
    vk::UniqueSurfaceKHR surface;
    queue_family_map_t queues;
    vk::UniqueDevice dev;
    swap_info sc_info;
    vk::UniqueSwapchainKHR swapchain;
    std::vector<vk::UniqueImageView> swap_image_views;
    std::vector<vk::UniqueFramebuffer> swap_framebuffers;
    vk::UniqueRenderPass render_pass;
    vk::UniquePipelineLayout pipe_layout;
    vk::UniquePipeline pipeline;
    vk::UniqueCommandPool cmd_pool;
    std::vector<vk::UniqueCommandBuffer> cmd;

    void create_instance();
    bool get_device_queue_families(vk::PhysicalDevice const & dev);
    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void get_surface();
    void pick_gpu();
    void create_device();
    void create_image_view(vk::Format fmt, vk::ImageAspectFlags flags, vk::Image const & img,
                           vk::UniqueImageView & view);
    void create_swapchain();
    void create_render_pass();
    void create_shader_module(std::vector<char> const & code, vk::UniqueShaderModule & mod);
    void create_pipeline();
    void create_commands();
    void create_framebuffers();

public:

    render();

    friend class queue_family;
};
