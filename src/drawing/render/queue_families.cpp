/// queue_families.cpp

#include "../../snake3.hpp"

queue_family::queue_family():
    loggable(__func__)
{ }

queue_family::queue_family(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface):
    loggable(__func__)
{
    auto const & families = gpu.getQueueFamilyProperties();

    for (auto u = 0u; u < families.size(); u++)
    {
        auto & family = families[u];

        if ((family.queueFlags & vk::QueueFlagBits::eGraphics)
            && (family.queueFlags & vk::QueueFlagBits::eCompute))
            indices[QUEUE_FAMILY_GRAPHICS] = u;
        
        if (VK_FALSE != gpu.getSurfaceSupportKHR(u, surface))
            indices[QUEUE_FAMILY_PRESENT] = u;
    }
}

void queue_family::get_queues(vk::Device const & dev)
{
    for (auto u = 0u; u < indices.size(); u++)
    {
        if (!indices[u].has_value())
            runtime_error("cannot obtain all queues for current device.");
        
        queues[u] = dev.getQueue(indices[u].value(), 0);
    }
}

std::uint32_t queue_family::index_of(unsigned u) const
{
    if (!indices[u].has_value())
        runtime_error("attempt to access unfound queue index " + std::to_string(u) + ".");

    return indices[u].value();
}

queue_family::operator bool( ) const
{
    auto valid = true;

    for (auto const index : indices)
        valid &= index.has_value();

    return valid;
}

vk::Queue queue_family::operator[](unsigned u) const
{
    if (!queues[u].has_value())
        runtime_error("attempt to access unfound queue " + std::to_string(u) + ".");

    return queues[u].value();
}
