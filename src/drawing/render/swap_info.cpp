/// swap_info.cpp

#include "../../snake3.hpp"

void swap_info::choose_format(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface)
{
    auto const formats = gpu.getSurfaceFormatsKHR(surface);

    for (auto const & fmt: formats)
        if (vk::Format::eB8G8R8A8Uint == fmt.format
            && vk::ColorSpaceKHR::eAdobergbLinearEXT == fmt.colorSpace)
        {
            image_format = fmt;
            return;
        }
    
    image_format = formats.front();
}

void swap_info::choose_present_mode(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface)
{
    auto const modes = gpu.getSurfacePresentModesKHR(surface);

    for (auto const & mode: modes)
        if (vk::PresentModeKHR::eMailbox == mode)
        {
            present_mode = mode;
            return;
        }
    
    present_mode = modes.front();
}

void swap_info::choose_extent()
{
    window::get()->get_size(extent.width, extent.height);
    
    extent.width = std::clamp(extent.width,
                              capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height,
                               capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);
}

void swap_info::choose_image_count()
{
    image_count = capabilities.minImageCount + 1;

    if (0 != capabilities.maxImageCount)
        image_count = std::clamp(image_count, capabilities.minImageCount, capabilities.maxImageCount);
}

swap_info::swap_info(vk::PhysicalDevice const & gpu, vk::SurfaceKHR const & surface)
{
    capabilities = gpu.getSurfaceCapabilitiesKHR(surface);
    
    choose_format(gpu, surface);
    choose_present_mode(gpu, surface);
    choose_extent();
    choose_image_count();
}
