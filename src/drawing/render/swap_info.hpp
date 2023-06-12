/// swap_info.hpp

#pragma once

class swap_info
{
private:

    vk::SurfaceCapabilitiesKHR capabilities;
    vk::Extent2D extent;
    vk::SurfaceFormatKHR image_format;
    vk::PresentModeKHR present_mode;
    std::uint32_t image_count;

    void choose_format(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface);
    void choose_present_mode(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface);
    void choose_extent();
    void choose_image_count();

public:

    swap_info() = default;
    swap_info(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface);

    friend class render;
};
