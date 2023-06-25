/// uniform_buffer.hpp

#pragma once

struct ubo_t
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    static auto layout_binding( )
    {
        return vk::DescriptorSetLayoutBinding(
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex
        );
    }
};
