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

void render::create_image_view(vk::Format fmt, vk::ImageAspectFlags flags, vk::Image const & img,
                               vk::UniqueImageView & view)
{
    auto const range = vk::ImageSubresourceRange(
        flags,
        0,
        1,
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

std::uint32_t render::mem_index(std::uint32_t type, vk::MemoryPropertyFlags flags)
{
    auto const props = gpu.getMemoryProperties();

    for (auto u = 0u; u < props.memoryTypeCount; u++)
        if (type & (1 << u) && props.memoryTypes[u].propertyFlags & flags)
            return u;
    
    runtime_error("unable to find memory index requested.");
    return 0;
}

void render::create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                           vk::MemoryPropertyFlags flags, vk::UniqueBuffer & buf,
                           vk::UniqueDeviceMemory & mem)
{
    auto const info = vk::BufferCreateInfo(
        { },
        size,
        usage,
        vk::SharingMode::eExclusive
    );

    buf = dev->createBufferUnique(info);

    auto const req = dev->getBufferMemoryRequirements(*buf);
    auto const mem_info = vk::MemoryAllocateInfo(
        req.size,
        mem_index(req.memoryTypeBits, flags)
    );

    mem = dev->allocateMemoryUnique(mem_info);
    dev->bindBufferMemory(*buf, *mem, 0);
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

    auto const create_info = vk::InstanceCreateInfo(
        {},
        &app_info,
#if defined (NDEBUG)
        0,
        nullptr
#else
        VALIDATION_LAYERS.size(),
        VALIDATION_LAYERS.data(),
#endif
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
	auto const priority = 1.f;
    std::set<std::uint32_t> added_queues {};

    for (auto const index : queue.indices)
    {
        if (added_queues.find(index.value()) != added_queues.end())
            continue;
        
        queues_to_create.emplace_back(
            vk::DeviceQueueCreateInfo(
                { },
                index.value(),
                1,
                &priority
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
        nullptr
#else
        VALIDATION_LAYERS.size(),
        VALIDATION_LAYERS.data(),
#endif
        DEVICE_EXTENSIONS.size(),
        DEVICE_EXTENSIONS.data(),
        &dev_features
    );
    
    dev = gpu.createDeviceUnique(dev_create_info);
    queue.get_queues(*dev);
}

void render::create_swapchain()
{
    sc_info = swap_info(gpu, *surface);
    
    std::uint32_t const indices[ ] = {queues[gpu].indices[QUEUE_FAMILY_GRAPHICS].value(),
                                      queues[gpu].indices[QUEUE_FAMILY_PRESENT].value()};
    auto sc_create_info = vk::SwapchainCreateInfoKHR(
        vk::SwapchainCreateFlagsKHR(),
        *surface,
        sc_info.image_count,
        sc_info.image_format.format,
        sc_info.image_format.colorSpace,
        sc_info.extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive,
        0,
        nullptr,
        sc_info.capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        sc_info.present_mode
    );

    if (indices[0] != indices[1])
    {
        sc_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
		sc_create_info.queueFamilyIndexCount = 2;
		sc_create_info.pQueueFamilyIndices = indices;
    }

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
    auto const dependency = vk::SubpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eColorAttachmentWrite
    );
    auto const create_info = vk::RenderPassCreateInfo(
        { },
        1,
        &color_attachment,
        1,
        &subpass,
        1,
        &dependency
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

void render::create_buffers()
{
    create_buffer(
        sizeof(ubo_t),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        ubo_buf,
        ubo_mem
    );
}

void render::create_buffer_descriptors()
{
    std::array<vk::DescriptorPoolSize, 1> pools
    {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
    };

    auto const pool_info = vk::DescriptorPoolCreateInfo(
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1,
        pools.size(),
        pools.data()
    );

    desc_pool = dev->createDescriptorPoolUnique(pool_info);

    auto const desc_layouts = std::vector<vk::DescriptorSetLayoutBinding>
    {
        ubo_t::layout_binding()
    };

    auto const desc_layout_info = vk::DescriptorSetLayoutCreateInfo(
        { },
        desc_layouts.size(),
        desc_layouts.data()
    );

    desc_layout = dev->createDescriptorSetLayoutUnique(desc_layout_info);

    auto const desc_allocate_info = vk::DescriptorSetAllocateInfo(
        *desc_pool,
        1,
        &*desc_layout
    );

    desc = std::move(dev->allocateDescriptorSetsUnique(desc_allocate_info)[0]);

    auto const ubo_info = vk::DescriptorBufferInfo(
        *ubo_buf,
        0,
        sizeof(ubo_t)
    );

    std::array<vk::WriteDescriptorSet, 1> writes
    {
        vk::WriteDescriptorSet(
            *desc,
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            nullptr,
            &ubo_info
        )
    };

    dev->updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
}

void render::create_pipeline()
{
    auto const vshader = utils::load_file("/home/cole/snake3/vert.spv");
    auto const fshader = utils::load_file("/home/cole/snake3/frag.spv");

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

    auto const rast_info = vk::PipelineRasterizationStateCreateInfo(
        {},
        false,
        false,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eFrontAndBack,
        vk::FrontFace::eCounterClockwise,
        false,
        0.f,
        0.f,
        0.f,
        1.f
    );
    auto const multisample_info = vk::PipelineMultisampleStateCreateInfo(
        { },
        vk::SampleCountFlagBits::e1,
        false,
        1.f,
        nullptr,
        false,
        false
    );
    auto const blend_attachment = vk::PipelineColorBlendAttachmentState(
        false,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eA 
        | vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
    );
    auto const blend_info = vk::PipelineColorBlendStateCreateInfo(
        { },
        VK_FALSE,
        vk::LogicOp::eCopy,
        1,
        &blend_attachment
    );

    auto const pipe_layout_info = vk::PipelineLayoutCreateInfo(
        { },
        1,
        &*desc_layout
    );

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
        &multisample_info,
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
            sc_info.extent.height,
            1
        );

        swap_framebuffers.emplace_back(dev->createFramebufferUnique(framebuffer_info));
    }
}

void render::create_sync_objects()
{
    auto const f_info = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
    auto const s_info = vk::SemaphoreCreateInfo();

    f_rendering = dev->createFenceUnique(f_info);
    s_image_acquired = dev->createSemaphoreUnique(s_info);
    s_render_finished = dev->createSemaphoreUnique(s_info);
}

vk::CommandBuffer render::begin_single_commands()
{
    auto const alloc_info = vk::CommandBufferAllocateInfo(
        *cmd_pool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    auto const cmd = dev->allocateCommandBuffers(alloc_info)[0];
    auto const begin_info = vk::CommandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );

    cmd.begin(begin_info);
    return cmd;
}

void render::end_single_commands(vk::CommandBuffer cmd)
{
    cmd.end();

    auto const submit_info = vk::SubmitInfo(
        0,
        nullptr,
        nullptr,
        1,
        &cmd
    );
    if (vk::Result::eSuccess != queues[gpu][QUEUE_FAMILY_GRAPHICS].submit(1, &submit_info, nullptr))
        runtime_error("failed to submit commands.");

    queues[gpu][QUEUE_FAMILY_GRAPHICS].waitIdle();

    dev->freeCommandBuffers(*cmd_pool, 1, &cmd);
}

void render::copy_buffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
    auto const cmd = begin_single_commands();
    auto const copy = vk::BufferCopy(
        0,
        0,
        size
    );

    cmd.copyBuffer(src, dst, 1, &copy);
    end_single_commands(cmd);
}

void render::make_vb()
{
    static std::array<vertex, 4> vertices 
    {
        vertex({-0.5f, -0.5f, 0.f}, {0.f, 1.f, 0.f, 1.f}),
        vertex({0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f, 1.f}),
        vertex({0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f, 1.f}),
        vertex({-0.5f, 0.5f, 0.f}, {1.f, 1.f, 1.f, 1.f})
    };
    static std::array<std::uint16_t, 6> indices
    {
	    0, 1, 2, 2, 3, 0
    };

    {
        auto const s = sizeof(vertex) * vertices.size();

        vk::UniqueBuffer stage;
        vk::UniqueDeviceMemory stage_mem;

        create_buffer(s, vk::BufferUsageFlagBits::eTransferSrc,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                    stage, stage_mem);
        create_buffer(s, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    vb, vm);

        auto const data = dev->mapMemory(*stage_mem, 0, s);
        std::memcpy(data, vertices.data(), s);
        dev->unmapMemory(*stage_mem);

        copy_buffer(*stage, *vb, s);
    }

    {
        auto const s = sizeof(std::uint16_t) * indices.size();

        vk::UniqueBuffer stage;
        vk::UniqueDeviceMemory stage_mem;

        create_buffer(s, vk::BufferUsageFlagBits::eTransferSrc,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                    stage, stage_mem);
        create_buffer(s, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    ib, im);

        auto const data = dev->mapMemory(*stage_mem, 0, s);
        std::memcpy(data, indices.data(), s);
        dev->unmapMemory(*stage_mem);

        copy_buffer(*stage, *ib, s);
    }
}

void render::begin_frame(std::size_t const img)
{
    if (vk::Result::eSuccess != dev->resetFences(1, &*f_rendering))
        runtime_error("failed to reset fence.");

    cmd[frame]->reset();

    auto begin_info = vk::CommandBufferBeginInfo();
    auto const viewport = vk::Viewport(
        0.f,
        0.f,
        static_cast<float>(sc_info.extent.width),
        static_cast<float>(sc_info.extent.height),
        0.f,
        1.f
    );
    auto const scissor = vk::Rect2D(
        {},
        sc_info.extent
    );
	std::vector< vk::ClearValue > clear_colors 
	{ 
		vk::ClearValue(
            vk::ClearColorValue(
                0.f, 0.f, 0.f, 0.f
            )
        )
	};
    auto const pass_info = vk::RenderPassBeginInfo(
        *render_pass,
        *swap_framebuffers[img],
        scissor,
        clear_colors.size(),
        clear_colors.data()
    );

    if (vk::Result::eSuccess != cmd[frame]->begin(&begin_info))
        runtime_error("failed to begin new command.");

    cmd[frame]->beginRenderPass(pass_info, vk::SubpassContents::eInline);
    cmd[frame]->setViewport(0, 1, &viewport);
    cmd[frame]->setScissor(0, 1, &scissor);
    //cmd[frame]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipe_layout, 0, 1, &*desc, 0, nullptr);
}

void render::end_frame()
{
    cmd[frame]->endRenderPass();
    cmd[frame]->end();

    auto const wait_stages = std::vector<vk::PipelineStageFlags>
    {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };
    auto const submit_info = vk::SubmitInfo(
        1,
        &*s_image_acquired,
        wait_stages.data(),
        1,
        &*cmd[frame],
        1,
        &*s_render_finished
    );

    if (vk::Result::eSuccess != queues[gpu][QUEUE_FAMILY_GRAPHICS].submit(1, &submit_info, *f_rendering))
        runtime_error("failed to submit render commands.");
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
    create_buffers();
    create_buffer_descriptors();
    create_pipeline();
    create_commands();
    create_framebuffers();
    create_sync_objects();

    make_vb();
}

render::~render()
{
    dev->waitIdle();
}

void render::draw_frame()
{
    if (vk::Result::eSuccess != dev->waitForFences(1, &*f_rendering, true, std::numeric_limits<std::uint64_t>::max()))
        runtime_error("failed to wait for next frame.");

    auto img = 0u;

    if (auto const res = dev->acquireNextImageKHR(*swapchain, 
                                                  std::numeric_limits<std::uint64_t>::max(),
                                                  *s_image_acquired,
                                                  nullptr,
                                                  &img);
        res == vk::Result::eErrorOutOfDateKHR)
        return void();
    else if (res != vk::Result::eSuccess && res != vk::Result::eSuboptimalKHR)
        runtime_error("failed to acquire next image.");

    begin_frame(img);
    {
        vk::DeviceSize const offset = 0;
        cmd[frame]->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
        cmd[frame]->bindVertexBuffers(0, 1, &*vb, &offset);
        cmd[frame]->bindIndexBuffer(*ib, 0, vk::IndexType::eUint16);
        cmd[frame]->drawIndexed(6, 1, 0, 0, 0);
        cmd[frame]->drawIndexed(6, 1, 0, 0, 0);
        cmd[frame]->drawIndexed(6, 1, 0, 0, 0);
        cmd[frame]->drawIndexed(6, 1, 0, 0, 0);
    }
    end_frame();

    auto const present_info = vk::PresentInfoKHR(
        1,
        &*s_render_finished,
        1,
        &*swapchain,
        &img
    );

    if (auto const res = queues[gpu][QUEUE_FAMILY_PRESENT].presentKHR(present_info);
        res == vk::Result::eErrorOutOfDateKHR
        || res == vk::Result::eSuboptimalKHR)
        runtime_error("failed to present image.");
    else if (res != vk::Result::eSuccess)
        runtime_error("failed to present image.");

    frame = (frame + 1) % NFRAMEBUFFERS;
}
