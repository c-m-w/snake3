/// queue_family.hpp

#pragma once

enum
{
    QUEUE_FAMILY_GRAPHICS,
    QUEUE_FAMILY_PRESENT,
    QUEUE_FAMILY_MAX
};

class queue_family: loggable
{
private:

    std::array<std::optional<std::uint32_t>, QUEUE_FAMILY_MAX> indices;
    std::array<std::optional<vk::Queue>, QUEUE_FAMILY_MAX> queues;

public:

    queue_family();
    queue_family(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface);

    void get_queues(vk::Device const & dev);

    std::uint32_t index_of(unsigned u) const;

    operator bool( ) const;
    vk::Queue operator[](unsigned u) const;

    friend class render;
};
