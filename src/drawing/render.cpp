/// render.cpp

#include "../snake3.hpp"

render::render()
{
    auto const inst_info = vk::InstanceCreateInfo();

    inst = vk::createInstanceUnique(inst_info);
    std::cout << *inst << std::endl;
}
