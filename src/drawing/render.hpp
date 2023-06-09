/// render.hpp

#pragma once

class render: public singleton<render>
{
private:

    vk::UniqueInstance inst;
    vk::PhysicalDevice gpu;

public:

    render();
};
