/// render.cpp

#include "../../snake3.hpp"

using device_score_t = std::pair<vk::PhysicalDevice, std::size_t>;

namespace std
{
    template<>
    class less<device_score_t>
    {
    public:

        bool operator()(device_score_t const & lhs, device_score_t const & rhs)
        {
            return less<>{}(lhs.second, rhs.second);
        }
    };
}

void render::create_instance()
{
    vk::ApplicationInfo app_info(
        "snake3",
        VK_MAKE_VERSION(1, 0, 0),
        "no engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_0
    );

    auto const [glfw_extensions, n_glfw_extensions] = window::get()->get_extensions();

    vk::InstanceCreateInfo create_info(
        vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        &app_info,
        0,
        nullptr,
        n_glfw_extensions,
        glfw_extensions
    );

    inst = vk::createInstanceUnique(create_info);
}

std::size_t render::get_device_score(vk::PhysicalDevice const & dev)
{
    std::size_t score = 0u;

    queues.insert_or_assign(dev, queue_family(dev, *surface));

    if (!queues[dev])
        return score;

    auto const props = dev.getProperties();

    switch (props.deviceType)
    {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            score += 15;
            break;
        
        case vk::PhysicalDeviceType::eIntegratedGpu:
            score += 10;
            break;
        
        case vk::PhysicalDeviceType::eCpu:
            score += 5;
            break;

        default:
            break;
    }

    return score;
}

void render::get_surface()
{
    VkSurfaceKHR tmp;

    if (VK_SUCCESS != glfwCreateWindowSurface(*inst, *window::get(), nullptr, &tmp))
        runtime_error("unable to create surface.");
    
    surface = vk::UniqueSurfaceKHR(tmp, *inst);
}

void render::pick_gpu()
{
    std::priority_queue<device_score_t> devices;

    for (auto const & dev: inst->enumeratePhysicalDevices())
        devices.push(std::make_pair(dev, get_device_score(dev)));

    if (devices.empty())
        runtime_error("no available devices.");

    auto const [dev, score] = devices.top();

    if (score == 0)
        runtime_error("no usable physical device.");

    gpu = dev;
}

void render::create_device()
{
    auto & queue = queues[gpu];

    std::vector<vk::DeviceQueueCreateInfo> queues_to_create {};
    std::set<std::uint32_t> added_queues {};

    for (auto const index : queue.indices)
    {
        if (added_queues.find(index.value()) != added_queues.end())
            continue;
        
        queues_to_create.emplace_back(
            vk::DeviceQueueCreateInfo(
                { },
                index.value(),
                1
            )
        );
        added_queues.insert(index.value());
    }

    auto dev_features = vk::PhysicalDeviceFeatures();
    auto dev_create_info = vk::DeviceCreateInfo(
        { },
        queues_to_create.size(),
        &queues_to_create[0],
#if defined (NDEBUG)
        0,
        nullptr,
#else
        VALIDATION_LAYERS.size(),
        &VALIDATION_LAYERS[0],
#endif
        DEVICE_EXTENSIONS.size(),
        &DEVICE_EXTENSIONS[0],
        &dev_features
    );
    
    dev = gpu.createDeviceUnique(dev_create_info);
    queue.get_queues(*dev);
}

void render::create_image_view(vk::Format fmt, vk::ImageAspectFlags flags, vk::Image const & img,
                               vk::UniqueImageView & view)
{
    auto const range = vk::ImageSubresourceRange(
        flags,
        0,
        0,
        0,
        1
    );
    auto const info = vk::ImageViewCreateInfo(
        { },
        img,
        vk::ImageViewType::e2D,
        fmt,
        {vk::ComponentSwizzle::eIdentity,
         vk::ComponentSwizzle::eIdentity,
         vk::ComponentSwizzle::eIdentity,
         vk::ComponentSwizzle::eIdentity},
        range
    );

    view = dev->createImageViewUnique(info);
}

void render::create_swapchain()
{
    sc_info = swap_info(gpu, *surface);
    
    std::uint32_t const indices[ ] = {queues[gpu].indices[QUEUE_FAMILY_GRAPHICS].value(),
                                      queues[gpu].indices[QUEUE_FAMILY_PRESENT].value()};
    auto const sc_create_info = vk::SwapchainCreateInfoKHR(
        vk::SwapchainCreateFlagsKHR(),
        *surface,
        sc_info.image_count,
        sc_info.image_format.format,
        sc_info.image_format.colorSpace,
        sc_info.extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eConcurrent,
        2,
        indices,
        sc_info.capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        sc_info.present_mode
    );

    swapchain = dev->createSwapchainKHRUnique(sc_create_info);

    for (auto & img : dev->getSwapchainImagesKHR(*swapchain))
        create_image_view(sc_info.image_format.format, vk::ImageAspectFlagBits::eColor,
                          img, swap_image_views.emplace_back());
}

void render::create_render_pass()
{
    auto const color_attachment = vk::AttachmentDescription(
        { },
        sc_info.image_format.format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );

    auto const color_ref = vk::AttachmentReference(
        0,
        vk::ImageLayout::eColorAttachmentOptimal
    );

    auto const subpass = vk::SubpassDescription(
        { },
        vk::PipelineBindPoint::eGraphics,
        0,
        nullptr,
        1,
        &color_ref
    );

    auto const create_info = vk::RenderPassCreateInfo(
        { },
        1,
        &color_attachment,
        1,
        &subpass
    );

    render_pass = dev->createRenderPassUnique(create_info);
}

void render::create_shader_module(std::vector<char> const & code, vk::UniqueShaderModule & mod)
{
    auto const info = vk::ShaderModuleCreateInfo(
        { },
        code.size(),
        reinterpret_cast<std::uint32_t const *>(&code[0])
    );

    mod = dev->createShaderModuleUnique(info);
}

void render::create_pipeline()
{
    auto const vshader = utils::load_file("vert.spv");
    auto const fshader = utils::load_file("frag.spv");

    vk::UniqueShaderModule vertex, fragment;

    create_shader_module(vshader, vertex);
    create_shader_module(fshader, fragment);
    
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages {};

    shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo(
        { },
        vk::ShaderStageFlagBits::eVertex,
        *vertex,
        "main"
    ));
    shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo(
        { },
        vk::ShaderStageFlagBits::eFragment,
        *fragment,
        "main"
    ));

    auto const binding_desc = vertex::get_binding_description();
    auto const attribute_desc = vertex::get_attribute_descriptions();

    auto const vert_info = vk::PipelineVertexInputStateCreateInfo(
        { },
        1,
        &binding_desc,
        attribute_desc.size(),
        attribute_desc.data()
    );

    auto const input_info = vk::PipelineInputAssemblyStateCreateInfo(
        { },
        vk::PrimitiveTopology::eTriangleList
    );

    auto const viewport_info = vk::PipelineViewportStateCreateInfo(
        { },
        1,
        nullptr,
        1,
        nullptr
    );

    std::vector< vk::DynamicState > const dynamic_states {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    auto const dynamic_state_info = vk::PipelineDynamicStateCreateInfo(
        { },
        dynamic_states.size(),
        dynamic_states.data()
    );

    auto const rast_info = vk::PipelineRasterizationStateCreateInfo();
    auto const blend_attachment = vk::PipelineColorBlendAttachmentState();
    auto const blend_info = vk::PipelineColorBlendStateCreateInfo(
        { },
        VK_FALSE,
        vk::LogicOp::eCopy,
        1,
        &blend_attachment
    );

    auto const pipe_layout_info = vk::PipelineLayoutCreateInfo();

    pipe_layout = dev->createPipelineLayoutUnique(pipe_layout_info);

    auto const pipe_info = vk::GraphicsPipelineCreateInfo(
        { },
        shader_stages.size(),
        shader_stages.data(),
        &vert_info,
        &input_info,
        nullptr,
        &viewport_info,
        &rast_info,
        nullptr,
        nullptr,
        &blend_info,
        &dynamic_state_info,
        *pipe_layout,
        *render_pass
    );

    if (auto && result = dev->createGraphicsPipelineUnique(nullptr, pipe_info);
        result.result != vk::Result::eSuccess)
        runtime_error("failed to make pipeline.");
    else
        pipeline = std::move(result.value);
}

void render::create_commands()
{
    auto const pool_info = vk::CommandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queues[gpu].index_of(QUEUE_FAMILY_GRAPHICS)
    );

    cmd_pool = dev->createCommandPoolUnique(pool_info);

    auto const cmd_info = vk::CommandBufferAllocateInfo(
        *cmd_pool,
        vk::CommandBufferLevel::ePrimary,
        NFRAMEBUFFERS
    );

    cmd = dev->allocateCommandBuffersUnique(cmd_info);
}

void render::create_framebuffers()
{
    for (auto & image_view: swap_image_views)
    {
        auto const framebuffer_info = vk::FramebufferCreateInfo(
            { },
            *render_pass,
            1,
            &*image_view,
            sc_info.extent.width,
            sc_info.extent.height
        );

        swap_framebuffers.emplace_back(dev->createFramebufferUnique(framebuffer_info));
    }
}

render::render():
    loggable(__func__)
{
    create_instance();
    get_surface();
    pick_gpu();
    create_device();
    create_swapchain();
    create_render_pass();
    create_pipeline();
    create_commands();
    create_framebuffers();
}
