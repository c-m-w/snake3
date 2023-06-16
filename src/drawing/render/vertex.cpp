/// vertex.cpp

#include "../../snake3.hpp"

vertex::vertex(glm::vec3 const & pos, glm::vec4 const & col):
    pos(pos), col(col)
{ }

vk::VertexInputBindingDescription
vertex::get_binding_description()
{
    return vk::VertexInputBindingDescription(
        0,
        sizeof(vertex),
        vk::VertexInputRate::eVertex
    );
}

std::array<vk::VertexInputAttributeDescription, 2>
vertex::get_attribute_descriptions( )
{
    std::array<vk::VertexInputAttributeDescription, 2> desc { };

    desc[0] = vk::VertexInputAttributeDescription(
        0,
        0,
        vk::Format::eR32G32B32Sfloat,
        offsetof(vertex, pos)
    );

    desc[1] = vk::VertexInputAttributeDescription(
        0,
        1,
        vk::Format::eR32G32B32A32Sfloat,
        offsetof(vertex, pos)
    );

    return desc;
}
