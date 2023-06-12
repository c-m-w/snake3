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

render::render():
    loggable(__func__)
{
    create_instance();
    get_surface();
    pick_gpu();
    create_device();
}
