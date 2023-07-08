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

enum draw_mode
{
    DRAWMODE_LINE,
    DRAWMODE_FILL
};

template<typename T = void*>
class i_buffer
{
protected:

    vk::UniqueBuffer buf;
    vk::UniqueDeviceMemory mem;

public:

    virtual T& operator*() = 0;
};

template<typename T>
class host_buffer: public i_buffer<T>
{ };

template<>
class host_buffer<ubo_t>: public i_buffer<ubo_t>
{
public:

    host_buffer(); // todo add this

    ubo_t& operator*() override;
};

class device_buffer: public i_buffer<>
{
private:

    void*& operator*() override // todo add this
    {return *static_cast<void**>(nullptr);};

protected:

    std::uint32_t n;

public:

    device_buffer() = default;
    device_buffer(void * data, std::size_t size, std::uint32_t n, vk::BufferUsageFlags usage);
    device_buffer(std::uint16_t * indices, std::uint32_t n);
    device_buffer(vertex * vertices, std::uint32_t n);

    friend class render;

};

class render: public singleton<render>, loggable, dependable<window>
{
private:

    static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#if not defined(NDEBUG)

    constexpr static std::array< const char *, 1 > VALIDATION_LAYERS = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

#endif

    using queue_family_map_t = std::unordered_map<vk::PhysicalDevice, queue_family>;
    using pipe_pair = std::pair<vk::UniquePipelineLayout, vk::UniquePipeline>;

    constexpr static auto NFRAMEBUFFERS = 2;
    constexpr static std::array<float, 4> BACKGROUND_COLOR {
        0.133f,
        0.133f,
        0.231f,
        1.f
    };

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;
    vk::UniqueSurfaceKHR surface;
    queue_family_map_t queues;
    vk::UniqueDevice dev;
    swap_info sc_info;
    vk::UniqueSwapchainKHR swapchain;
    vk::UniqueImage color_image;
    vk::UniqueDeviceMemory color_image_mem;
    vk::UniqueImageView color_image_view;
    std::vector<vk::UniqueImageView> swap_image_views;
    std::vector<vk::UniqueFramebuffer> swap_framebuffers;
    vk::UniqueRenderPass render_pass;
    pipe_pair line_pipe;
    pipe_pair triangle_pipe;
    vk::UniqueCommandPool cmd_pool;
    std::vector<vk::UniqueCommandBuffer> cmd;

    vk::UniqueDescriptorPool desc_pool;
    vk::UniqueDescriptorSetLayout desc_layout;
    vk::UniqueDescriptorSet desc;

    vk::UniqueFence f_rendering;
    vk::UniqueSemaphore s_image_acquired;
    vk::UniqueSemaphore s_render_finished;

    vk::UniqueDeviceMemory ubo_mem;
    vk::UniqueBuffer ubo_buf;
    void * ubo_map;

    vk::UniqueDeviceMemory vm;
    vk::UniqueBuffer vb;
    vk::UniqueDeviceMemory im;
    vk::UniqueBuffer ib;

    std::size_t frame = 0;

    void create_image(vk::Extent2D const & extent, vk::Format fmt, vk::ImageUsageFlags usage,
                      vk::SampleCountFlagBits samples, vk::MemoryPropertyFlags flags,
                      vk::UniqueImage & img, vk::UniqueDeviceMemory & mem);
    void create_image_view(vk::Format fmt, vk::ImageAspectFlags flags, vk::Image const & img,
                           vk::UniqueImageView & view);
    std::uint32_t mem_index(std::uint32_t type, vk::MemoryPropertyFlags flags);
    void create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                       vk::MemoryPropertyFlags flags, vk::UniqueBuffer & buf,
                       vk::UniqueDeviceMemory & mem);

    void create_instance();
    bool get_device_queue_families(vk::PhysicalDevice const & dev);
    std::size_t get_device_score(vk::PhysicalDevice const & dev);
    void get_surface();
    void pick_gpu();
    void create_device();
    void create_swapchain();
    void create_render_pass();
    void create_framebuffers();
    void create_swapchain_dependencies();
    void create_shader_module(std::vector<char> const & code, vk::UniqueShaderModule & mod);
    void create_buffers();
    void create_buffer_descriptors();
    void create_pipeline(vk::PrimitiveTopology top,
                         std::pair<vk::UniquePipelineLayout, vk::UniquePipeline> & data);
    void create_pipelines();
    void create_commands();
    void create_sync_objects();

    vk::CommandBuffer begin_single_commands();
    void end_single_commands(vk::CommandBuffer cmd);
    void copy_buffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    void make_vb();

    void update_uniform_buffer();

    void begin_frame(std::size_t const img);
    void end_frame();

public:

    render();
    ~render();

    void draw_buffer(device_buffer & vertices,
                     device_buffer & indices,
                     draw_mode const mode);
    void draw_frame();

    friend class queue_family;
    friend class device_buffer;
};
